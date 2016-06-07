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

// Induces abandonment of the current process.

#pragma once

#include <sal.h>
#include <stdint.h>

class Abandonment final {
private:
    enum class Category : unsigned long
    {
        FastDOM,
        OutOfMemory,
        ArithmeticOverflow,
        InvalidArguments,
        PostConditionViolated,
        NotYetImplemented,
        UnreachableCode,
        HRESULTFailure,
        RequiredQI,
        AssertionFailure,
        DeprecatedAPI,
        GenericFailure,
        GCInvalidPointer,
        GCDoubleFree
    };

    Abandonment() = delete;
    void operator=(const Abandonment&) = delete;

    __declspec(noinline)
    static LONG HostExceptionFilterWrapper(_In_ LPEXCEPTION_POINTERS exceptionPointers);

    __declspec(noinline)
    __declspec(noreturn)
    static void InduceAbandonment(Category abandonmentCategory, _In_reads_(0) PVOID exceptionAddress);

    __declspec(noinline)
    __declspec(noreturn)
    static void InduceHRESULTAbandonment(HRESULT hr);

    __declspec(noinline)
    __declspec(noreturn)
    static void InduceRequiredQIAbandonment(HRESULT hr);
public:
    __declspec(noreturn)
    __declspec(noinline)
    static void DeprecatedAPI();

    __declspec(noreturn)
    __declspec(noinline)
    static void FastDOMInvariantViolation();

    __declspec(noreturn)
    __declspec(noinline)
    static void OutOfMemory();

    __declspec(noreturn)
    __declspec(noinline)
    static void ArithmeticOverflow();

    __declspec(noreturn)
    __declspec(noinline)
    static void InvalidArguments();

    __declspec(noreturn)
    __declspec(noinline)
    static void PostConditionViolated();

    __declspec(noreturn)
    __declspec(noinline)
    static void Fail();

#if DBG == 1
    __declspec(noreturn)
    __declspec(noinline)
    static void Fail(PCSTR message);
#else
    __declspec(noreturn)
    static void Fail(PCSTR)
    {
        // On Release bulds, we do not want the Fail("...") messages retained in the binary.
        // The instruction pointer of the call site should be sufficient for diagnostics.
        // So we force-inline this method to the caller, and ignore the string argument.
        // The optimizer should be smart enough not to retain the unused literal string.
        Fail();
    }
#endif

    __declspec(noreturn)
    __declspec(noinline)
    static void NotYetImplemented();

    __declspec(noreturn)
    __declspec(noinline)
    static void UnreachableCode();

    // The following 3 functions are sometimes not inlined by the compiler even though they just check an hr.
    // In those cases, Watson crashes will be mapped to a bucket for the Abandonment helper instead of the caller.
    // If you get a Watson bug that needs a unique bucket to collect more data, call Abandonment::Fail() instead.

    static void CheckHRESULT(HRESULT hr);

    // A variant of CheckHRESULT that only allows S_OK and treats other success codes as failure per Trident rules.
    static void CheckHRESULTStrict(HRESULT hr);

    static void RequiredQI(HRESULT hr);

    __declspec(noreturn)
    __declspec(noinline)
    static void AssertionFailed();

    __declspec(noreturn)
    __declspec(noinline)
    static void AssertionFailed(PCSTR message);

    static void CheckAllocation(const void* allocation);

    // Specific failure buckets for when Memory GC detects improper heap API use.
    __declspec(noreturn)
    __declspec(noinline)
    static void GCInvalidPointer();

    __declspec(noreturn)
    __declspec(noinline)
    static void GCDoubleFree();

    // Checks whether an allocation succeeded, and induces abandonment if the allocation failed.
    // The caller indicates whether the allocation request succeeded, as a boolean.  This method
    // is useful for checking the return value of SysReAllocString() and similar methods.
    static void CheckAllocationBoolean(bool succeeded)
    {
        if (!succeeded)
        {
            OutOfMemory();
        }
    }

    // The CheckAllocation<T> template function checks whether 'allocation' is null (presumably because
    // an allocation failed), and induces abandonment if it has.  Note that the control is carefully
    // set up so that static analysis tools can verify that the return value is never null.
    //
    // Use like so:
    //
    // float* points = Abandonment::CheckAllocation((float*)malloc(...));
    //
    // It may seem odd that the logic of such a simple CheckAllocation is split across two methods.
    // The reason is to prevent branches from being inserted into the calling code.  Instead of lots
    // of branches in all of the app code, we have a single branch inside CheckAllocationUntyped().
    // Speculatively, this wastes less space in the branch target buffer, since only a single branch
    // address is evaluated, rather than thousands of different branches.  Also, this prevents the
    // compiler from splitting the calling code into lots of small basic blocks.  Because the calls
    // to CheckAllocationUntyped are direct (no pointer indirection), they should pipeline nicely.
    //
    // There are two reasons this function returns the given pointer.  First, syntactic convenience;
    // it allows the call to CheckAllocation to be inserted into an expression, without assigning a
    // new local variable and without requiring a second statement.  Second, it simplifies static
    // analysis, since the method is (correctly) annotated that the return value will never be null.

    static _Ret_notnull_ void* CheckAllocationUntyped(void* allocation);

    // Checks whether 'allocation' is null (presumably because an allocation failed), and induces
    // abandonment if it has.  Note that the control is carefully set up so that static analysis
    // tools can verify that the return value is never null.  This template
    template<typename T>
    static inline _Ret_notnull_ T* CheckAllocationT(T* allocation)
    {
        return (T*)CheckAllocationUntyped((void*)allocation);
    }

    template<typename T>
    static inline _Ret_notnull_ T* QueryInterface(_In_ IUnknown* pObject)
    {
        void* pInterfacePtr = nullptr;
        RequiredQI(pObject->QueryInterface(__uuidof(T), &pTypedObject));
        return (T*)CheckAllocationT(pInterfacePtr);
    }

    template<typename T, typename U>
    static inline _Ret_notnull_ T* QueryInterface(_In_ U* pObject)
    {
        void* pInterfacePtr = nullptr;
        RequiredQI(pObject->QueryInterface(__uuidof(T), &pInterfacePtr));
        return (T*)CheckAllocationT(pInterfacePtr);
    }

    static HRESULT LastError;
    static LPTOP_LEVEL_EXCEPTION_FILTER hostExceptionFilter;
};

// The "requires" macro is intended to serve the same purpose as the "requires" clause in System C#.
// Code that uses this method should be converted to a "requires" clause, when the code
// is converted to System C#.  These checks are performed (intentionally) on retail builds.
// Note: The "requires" statement is provided as a macro, rather than a function, so that
// analysis tools (OACR, etc.) can see the effects of the test/branch.
// Note: The assignment to a temporary of type "bool" is intended to force all callers to
// provide a valid boolean expression, as if calling a method with a "bool" parameter.
//
// All uses of requires() should precede all other code, within a method.  That is, each method that
// uses requires() should check all conditions before executing any other code.  Requires statements
// should not be dependent on (follow) any other logic.
#define requires(value) { bool _requires = (value); if (!_requires) { Abandonment::InvalidArguments(); } }

// Similarly, "ensures" is used by methods to verify that they meet their post-conditions.  Use ensures()
// to check post-conditions, at the end of a method.  No significant computation should occur after
// a method begins running its ensures() clauses.  That is, once a method starts running post-condition
// checks, it should not modify any state, trigger any side-effects, etc.  The only thing that should be
// done after starting a sequence of ensures() calls is assigning output parameters and returning.
#define ensures(value) { bool _ensures = (value); if (!_ensures) { Abandonment::PostConditionViolated(); } }

// Release_Assert() is equivalent to Platform.Release.Assert() in System C#.  It is an unconditional
// assertion check, and is present in release builds.  Release_Assert() is intended to check invariants
// which, if violated, could lead to memory safety violations, or other unacceptable runtime violations
// of invariants.  Note that Release_Assert() is not continuable; if a violation occurs, the program
// will immediately fail with a "fail-fast" exception.
//
// Note: It would be preferable to write Release_Assert() (and Debug_Assert()) as "static inline"
// functions, but unfortunately this prevents OACR analysis from understanding the implications of the
// assertion, and so OACR assumes that control flow will continue, even when the value is false.
// This is why Release_Assert() and Debug_Assert() are provided as macros, so that OACR can see the
// test and call to a __declspec(noreturn) function.

#define Release_Assert(value) if (!(value)) { Abandonment::AssertionFailed(); } else __noop
#define Release_Assert_Msg(value, message) if (!(value)) { Abandonment::AssertionFailed(message); } else __noop

// Debug_Assert() is equivalent to Platform.Debug.Assert() in System C#.  It is an assertion check
// that is present only in debug builds.  Note that Debug_Assert() is not continuable; if a violation
// occurs, the program will immediately fail with a "fail-fast" exception.

#if DBG == 1
#define Debug_Assert(value) if (!(value)) { Abandonment::AssertionFailed(); } else __noop
#define Debug_Assert_Msg(value, message) if (!(value)) { Abandonment::AssertionFailed(message); } else __noop
#else
#define Debug_Assert(value) __noop
#define Debug_Assert_Msg(value, message) __noop
#endif

// These are helper functions for directly calling external allocators (allocators other
// than MemAlloc / operator new), which will induce abandonment if the allocation fails.
// The _FF suffix is for Fail Fast.
//
// Note that these functions do not behave the same as FormsAllocString (and similarly-named
// functions), with respect to handling zero-length input strings.  SysAllocString and
// SysAllocString_FF will both return a non-NULL zero-length BSTR, if the input is a zero-length
// string.  However, FormsAllocString() will return a NULL BSTR for a zero-length input.
//
// Generally, Trident code should be converted to use FormsAllocString().  However, doing so
// requires dealing with the difference between NULL vs. zero-length BSTR, and that requires
// examining the logic at every call site.  Until that can be done, callers should convert to
// using SysAllocString_FF, and NULL returns should *not* be treated as an out-of-memory
// indication.

static inline _Ret_notnull_ BSTR SysAllocString_FF(_In_z_ const OLECHAR* sz)
{
    return Abandonment::CheckAllocationT(::SysAllocString(sz));
}

static inline _Ret_notnull_ BSTR SysAllocStringByteLen_FF(_In_reads_opt_(ui) LPCSTR strIn, _In_ UINT ui)
{
    return Abandonment::CheckAllocationT(::SysAllocStringByteLen(strIn, ui));
}

static inline _Ret_writes_maybenull_z_(ui+1) BSTR SysAllocStringLen_FF(_In_reads_opt_(ui) const OLECHAR* strIn, _In_ UINT ui)
{
    return Abandonment::CheckAllocationT(::SysAllocStringLen(strIn, ui));
}

static inline void SysReAllocString_FF(_Inout_ BSTR* pbstr, _In_opt_z_ const OLECHAR* psz)
{
    Abandonment::CheckAllocationBoolean(::SysReAllocString(pbstr, psz) != 0);
}

static inline void SysReAllocStringLen_FF(_Inout_ BSTR* pbstr, _In_opt_z_ const OLECHAR* psz, _In_ UINT len)
{
    Abandonment::CheckAllocationBoolean(::SysReAllocStringLen(pbstr, psz, len) != 0);
}

static inline void VariantCopy_FF(_Inout_ VARIANTARG* pvargDest, _In_ const VARIANTARG* pvargSrc)
{
    Abandonment::CheckHRESULT(::VariantCopy(pvargDest, pvargSrc));
}

#include <intsafe.h>

// The CheckedMath static class provides methods for basic arithmetic, using a fail-fast policy
// for overflow / underflow, rather than return an HRESULT (or worse, incorrect results). These
// methods are intended to provide the equivalent to checked arithmetic in System C#, although the
// code gen is not likely to be as efficient as it is for System C#.
class CheckedMath final
{
private:
    CheckedMath() = delete;
    void operator=(const CheckedMath&) = delete;
    static uintmax_t LastLeftOperand;
    static uintmax_t LastRightOperand;

public:
    template <typename T>
    static void StoreOverflowConditions(T left, T right)
    {
        static_assert(sizeof(T) <= sizeof(uintmax_t), "We are storing a value outside of the range supported by CheckedMath. Increase the storage limit.");

        CheckedMath::LastLeftOperand = static_cast<uintmax_t>(left);
        CheckedMath::LastRightOperand = static_cast<uintmax_t>(right);
    }

    _Post_satisfies_(return == value)
    static int32_t UInt32_To_Int32(uint32_t value)
    {
        if (value > INT32_MAX)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<int32_t>(value);
    }

    _Post_satisfies_(return == left + right)
    static uint32_t UInt32_Add(uint32_t left, uint32_t right)
    {
        if (left > UINT32_MAX - right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left + right;
    }

    _Post_satisfies_(return == left - right)
    static uint32_t UInt32_Sub(uint32_t left, uint32_t right)
    {
        if (left < right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left - right;
    }

    _Post_satisfies_(return == left * right)
    static uint32_t UInt32_Mult(uint32_t left, uint32_t right)
    {
        uint64_t result = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);
        return UInt64_To_UInt32(result);
    }

    _Post_satisfies_(return == value)
    static uint32_t Int32_To_UInt32(int32_t value)
    {
        if (value < 0)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<uint32_t>(value);
    }

    _Post_satisfies_(return == left + right)
    static int32_t Int32_Add(int32_t left, int32_t right)
    {
        int64_t result = static_cast<int64_t>(left) + static_cast<int64_t>(right);
        return Int64_To_Int32(result);
    }

    _Post_satisfies_(return == left - right)
    static int32_t Int32_Sub(int32_t left, int32_t right)
    {
        int64_t result = static_cast<int64_t>(left) - static_cast<int64_t>(right);
        return Int64_To_Int32(result);
    }

    _Post_satisfies_(return == left * right)
    static int32_t Int32_Mult(int32_t left, int32_t right)
    {
        int64_t result = static_cast<int64_t>(left) * static_cast<int64_t>(right);
        return Int64_To_Int32(result);
    }

    // Rounds a up to the next multiple of b. Equivalent to the ROUNDUP macro, for int32_t.
    static int32_t Int32_RoundUp(int32_t a, int32_t b)
    {
        requires(b > 0);

        // Equivalent to:
        // return ((a + b - 1) / b) * b;
        int32_t temp1 = Int32_Add(Int32_Add(a, b), -1);
        int32_t temp2 = temp1 / b;
        return Int32_Mult(temp2, b);
    }

    _Post_satisfies_(return == value)
    static uint32_t UInt64_To_UInt32(uint64_t value)
    {
        if (value > UINT32_MAX)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<uint32_t>(value);
    }

    _Post_satisfies_(return == value)
    static int64_t UInt64_To_Int64(uint64_t value)
    {
        if (value > INT64_MAX)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<int64_t>(value);
    }

    _Post_satisfies_(return == left + right)
    static uint64_t UInt64_Add(uint64_t left, uint64_t right)
    {
        if (left > UINT64_MAX - right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left + right;
    }

    _Post_satisfies_(return == left - right)
    static uint64_t UInt64_Sub(uint64_t left, uint64_t right)
    {
        if (left < right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left - right;
    }

    _Post_satisfies_(return == left * right)
    static uint64_t UInt64_Mult(uint64_t left, uint64_t right)
    {
        if (left > UINT64_MAX / right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left * right;
    }

    _Post_satisfies_(return == value)
    static int32_t Int64_To_Int32(int64_t value)
    {
        // Test equality after casting down to an int32_t and casting back up to an int64_t. The
        // casts handle sign extension correctly, so if we get the same value back, the conversion
        // is safe.
        int32_t result = static_cast<int32_t>(value);
        int64_t test = static_cast<int64_t>(result);

        if (value != test)
        {
            Abandonment::ArithmeticOverflow();
        }

        return result;
    }

    _Post_satisfies_(return == value)
    static uint64_t Int64_To_UInt64(int64_t value)
    {
        if (value < 0)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<uint64_t>(value);
    }

    _Post_satisfies_(return == left + right)
    static int64_t Int64_Add(int64_t left, int64_t right)
    {
        int64_t result = left + right;

        // Adding positive to negative never overflows.
        // If you add two positive numbers, you expect a positive result.
        // If you add two negative numbers, you expect a negative result.

        // Overflow if inputs are the same sign and output is not that sign.
        if (((left < 0) == (right < 0)) &&
            ((left < 0) != (result < 0)))
        {
            Abandonment::ArithmeticOverflow();
        }

        return result;
    }

    _Post_satisfies_(return == left - right)
    static int64_t Int64_Sub(int64_t left, int64_t right)
    {
        int64_t result = left - right;

        // Subtracting a positive number from a positive number never overflows.
        // Subtracting a negative number from a negative number never overflows.
        // If you subtract a negative number from a positive number, you expect a positive result.
        // If you subtract a positive number from a negative number, you expect a negative result.

        // Overflow if inputs vary in sign and the output is not the same sign as the first input.
        if (((left < 0) != (right < 0)) &&
            ((left < 0) != (result < 0)))
        {
            Abandonment::ArithmeticOverflow();
        }
        return result;

    }

    _Post_satisfies_(return == value)
    static uint32_t SizeT_To_UInt32(size_t value)
    {
        if (value > UINT32_MAX)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<uint32_t>(value);
    }

    _Post_satisfies_(return == value)
    static int32_t SizeT_To_Int32(size_t value)
    {
        if (value > INT32_MAX)
        {
            Abandonment::ArithmeticOverflow();
        }

        return static_cast<int32_t>(value);
    }

    _Post_satisfies_(return == left + right)
    static size_t SizeT_Add(size_t left, size_t right)
    {
        if (left > SIZE_MAX - right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left + right;
    }

    _Post_satisfies_(return == left - right)
    static size_t SizeT_Sub(size_t left, size_t right)
    {
        if (left < right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left - right;
    }

    _Post_satisfies_(return == left * right)
    static size_t SizeT_Mult(size_t left, size_t right)
    {
        if (left > SIZE_MAX / right)
        {
            Abandonment::ArithmeticOverflow();
        }

        return left * right;
    }
};
