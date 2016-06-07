//--------------------------------------------------------------
//
// Microsoft Edge Implementation
// Copyright(c) Microsoft Corporation
// All rights reserved.
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files(the ""Software""),
// to deal in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------

/*

Provides support for "abandonment" / fail-fast.  Abandonment is used to recover from certain classes
of bugs and runtime failures, by terminating an entire scope of state.  In this case, the scope of
the state is the current process.

Abandonment is used to recover from software defects (bugs), such as failed array bounds checks.

Abandonment is also used to recover from heap allocation failures.  Instead of attempting to compensate
for allocation failures at every call site, we tear tear down the entire process when the OS / kernel
reports out-of-memory.

All abandonment paths end up calling Abandonment::InduceAbandonment.  This makes it easy to change
the mechanism and policy of abandonment, such as invoking Windows Error Reporting, etc.  For now,
we simply raise a non-continuable SEH exception.

The Abandonment class provides several different abandonment methods, for reporting different kinds
of abandonment.  They all have similar effects, but having different methods makes it easy to see
what kind of abandonment occurred in a stack trace.  For this reason, they are annotated with
__declspec(noinline).

*/

#include "public.hxx"
#include <intrin.h>

HRESULT Abandonment::LastError = S_OK;
uintmax_t CheckedMath::LastLeftOperand = 0;
uintmax_t CheckedMath::LastRightOperand = 0;
LPTOP_LEVEL_EXCEPTION_FILTER Abandonment::hostExceptionFilter = nullptr;

__declspec(noinline)
LONG Abandonment::HostExceptionFilterWrapper(_In_ LPEXCEPTION_POINTERS exceptionPointers)
{
    if (hostExceptionFilter != nullptr)
    {
        (*hostExceptionFilter)(exceptionPointers);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

// Induces abandonment, which currently destroys the current process, by raising a non-continuable exception.
//
// For the categories where each failure is a unique defect that deserves its own Watson bucket, the caller
// passes in the exceptionAddress.  When nullptr is passed, InduceAbandonment picks the address, which will
// create a single bucket for that category, e.g. the address becomes Abandonment::OutOfMemory.

__declspec(noreturn)
__declspec(noinline)
void Abandonment::InduceAbandonment(Category abandonmentCategory, _In_reads_(0) PVOID exceptionAddress)
{
    // Note: Debuggers will be notified through RaiseFailFastException so no additional work is necessary for that case.
    // For hosts such as MSHtmPad though we need to look for unhandled exception filters which might want to run and
    // dump the process before we try to terminate the process ourselves.  The Edge app uses a filter to notify the manager.
    // We'll raise an exception and then call the unhandled exception filters directly. This allows us to execute our
    // own handler as a result of the filter if the process hasn't already been terminated. In this handler we'll then
    // execute our fail fast code path.

    // Retrieve the current filter to call directly.  If we try and call it through kernelbase!UnhandledExceptionFilter,
    // it can invoke WER to end the process, and there will be no way to specify the exceptionAddress which is used
    // for Watson bucketing.
    //
    // Even if there is no host filter, go through the same steps for consistent behavior to spot problems with that path.
    hostExceptionFilter = SetUnhandledExceptionFilter(nullptr);

    if (exceptionAddress == nullptr)
    {
        exceptionAddress = _ReturnAddress();
    }

    __try
    {
        // abandonmentCategory is passed to RaiseException even though the param will be ignored.  This is to stop the
        // compiler from optimizing it out and folding together the calling functions.
        RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, static_cast<unsigned long>(abandonmentCategory), nullptr);
    }
    __except(HostExceptionFilterWrapper(GetExceptionInformation()))
    {
        // Create an EXCEPTION_RECORD that matches the defaults of RaiseFailFastException
        EXCEPTION_RECORD ExceptionRecord;
        ZeroMemory(&ExceptionRecord, sizeof(ExceptionRecord));

        if (abandonmentCategory == Category::HRESULTFailure)
        {
            // Use the hr as the exception code to get separate Watson buckets/failures for different issues
            ExceptionRecord.ExceptionCode = static_cast<DWORD>(Abandonment::LastError);
        }
        else
        {
            // Embed the value for STATUS_FAIL_FAST_EXCEPTION because including ntstatus.h conflicts with other Windows headers
            ExceptionRecord.ExceptionCode = 0xC0000602L;
        }
        ExceptionRecord.ExceptionAddress = exceptionAddress;
        ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

        RaiseFailFastException(&ExceptionRecord, nullptr, 0);
    }
}

// Induce abandonment because an external caller is using an API we have deprecated.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::DeprecatedAPI()
{
    InduceAbandonment(Category::DeprecatedAPI, nullptr);
}

// Induce abandonment because an invariant of the FastDOM and Chakra interaction layer has been
// violated. This is used for core failures only in either CFastDOM:: or CJScript9Holder::
__declspec(noreturn)
__declspec(noinline)
void Abandonment::FastDOMInvariantViolation()
{
    InduceAbandonment(Category::FastDOM, nullptr);
}

// Induce abandonment because a dynamic allocation has failed.  This is used for heap failures,
// as well as allocation of Windows kernel objects, such as event handles.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::OutOfMemory()
{
    InduceAbandonment(Category::OutOfMemory, nullptr);
}

// Induce abandonment because an arithmetic operation has overflowed.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::ArithmeticOverflow()
{
    InduceAbandonment(Category::ArithmeticOverflow, _ReturnAddress());
}

// General-purpose abandonment.  This should be used instead of Assert(FALSE).
__declspec(noreturn)
__declspec(noinline)
void Abandonment::Fail()
{
    InduceAbandonment(Category::GenericFailure, _ReturnAddress());
}

#if DBG == 1
// General-purpose abandonment.  This should be used instead of Assert(FALSE).
// Note: It is intentional that this is a different method from Abandonment::Fail().
// This allows codegen at call sites to be more specialized, resulting in fewer
// wasted registers and instructions.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::Fail(PCSTR message)
{
    UNREFERENCED_PARAMETER(message);
    InduceAbandonment(Category::GenericFailure, _ReturnAddress());
}
#endif

// Induce abandonment because we reached some point in control flow that should be
// impossible to reach.  The most common use for this is in the default: case of a
// switch statement, where the caller knows that the selector values are in some
// fixed set (such as an enum).
__declspec(noreturn)
__declspec(noinline)
void Abandonment::UnreachableCode()
{
    InduceAbandonment(Category::UnreachableCode, _ReturnAddress());
}

// Induces abandonment, because parameter validation has failed, indicating that the caller
// has a code defect.  This method is called by the requires() macro (which see, below).
// Methods should generally use requires() instead of calling InvalidArguments() directly.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::InvalidArguments()
{
    InduceAbandonment(Category::InvalidArguments, _ReturnAddress());
}

// Induces abandonment, because a function failed one of its ensures() clauses.  This means
// that the function did not correctly meet one of the contractual guarantees that it advertises.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::PostConditionViolated()
{
    InduceAbandonment(Category::PostConditionViolated, _ReturnAddress());
}

// Induce abandonment if an allocation failed, by testing a pointer for null.
void Abandonment::CheckAllocation(const void* allocation)
{
    if (allocation == nullptr)
    {
        Abandonment::OutOfMemory();
    }
}

__declspec(noreturn)
__declspec(noinline)
void Abandonment::GCInvalidPointer()
{
    InduceAbandonment(Category::GCInvalidPointer, nullptr);
}

__declspec(noreturn)
__declspec(noinline)
void Abandonment::GCDoubleFree()
{
    InduceAbandonment(Category::GCDoubleFree, nullptr);
}

// Induces abandonment, because a code path is not yet implemented.  It is not indicated whether
// the code path is intended to be implemented at some point in the future, or is never intended
// to be implemented.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::NotYetImplemented()
{
    InduceAbandonment(Category::NotYetImplemented, nullptr);
}

__declspec(noreturn)
__declspec(noinline)
void Abandonment::InduceHRESULTAbandonment(HRESULT hr)
{
    switch (hr)
    {
    case E_OUTOFMEMORY:
        Abandonment::OutOfMemory();
        break;
    default:
        Abandonment::LastError = hr;
        InduceAbandonment(Category::HRESULTFailure, _ReturnAddress());
        break;
    }
}

// Induce abandonment if an HRESULT is a failure code.
void Abandonment::CheckHRESULT(HRESULT hr)
{
    if (FAILED(hr))
    {
        InduceHRESULTAbandonment(hr);
    }
}

// Induce abandonment if an HRESULT is anything other than S_OK per Trident rules for internal HRESULT usage.
void Abandonment::CheckHRESULTStrict(HRESULT hr)
{
    if (hr != S_OK)
    {
        InduceHRESULTAbandonment(hr);
    }
}

// Induce abandonment if a QI fails for any reason
void Abandonment::RequiredQI(HRESULT hr)
{
    if (hr != S_OK)
    {
        InduceRequiredQIAbandonment(hr);
    }
}

__declspec(noinline)
__declspec(noreturn)
void Abandonment::InduceRequiredQIAbandonment(HRESULT hr)
{
    Abandonment::LastError = hr;
    InduceAbandonment(Category::RequiredQI, _ReturnAddress());
}

/*static*/ _Ret_notnull_ void* Abandonment::CheckAllocationUntyped(void* allocation)
{
    if (allocation == nullptr)
    {
        Abandonment::LastError = E_OUTOFMEMORY;
        OutOfMemory();
    }

    return allocation;
}

// Induce abandonment because an assertion has failed.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::AssertionFailed()
{
    InduceAbandonment(Category::AssertionFailure, _ReturnAddress());
}

// Induce abandonment because an assertion has failed.
//
// This is a separate method from AssertionFailed(), rather than using a single
// method with an optional / defaulted parameter, so that the code generated for
// the most common case (no message passed) is more efficient.
__declspec(noreturn)
__declspec(noinline)
void Abandonment::AssertionFailed(PCSTR message)
{
    // The message can be examined in the debugger.
    UNREFERENCED_PARAMETER(message);

    InduceAbandonment(Category::AssertionFailure, _ReturnAddress());
}
