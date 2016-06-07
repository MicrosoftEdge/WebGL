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

#pragma once

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_INIT
//
//  Synopsis:   Indicates the beginning of an HRESULT-returning method.
//              Opens a block that is closed by CHK_RETURN/CHK_END.
//
//              This version initializes the assumed result;
//
//              CHK_START or CHK_INIT is required for methods using CHK_RETURN.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_INIT(E_POINTER);
//                  ...
//                  CHK(MethodReturnsHR());
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_INIT(result)                                                        \
    HRESULT hrChk = result;                                                     \
    HRESULT & hr = hrChk; /* Allow CHK_END code to use hr */                    \
    (hr); /* reference hr */                                                    \
    {                                                                           \
        bool fInCHKScope; /* declare but don't reference; find empty CHK scopes \
        HRESULT & hr = hrChk; /* Prevent hr from being redeclared */            \
        (hr) /* reference hr; no semicolon to force one after the macro */      \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_START
//
//  Synopsis:   Indicates the beginning of an HRESULT-returning method.
//              Opens a block that is closed by CHK_RETURN.
//
//              CHK_START or CHK_INIT is required for methods using CHK_RETURN.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  CHK(MethodReturnsHR());
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------
#define CHK_START CHK_INIT(S_OK)

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_RETURN
//
//  Synopsis:   Indicates the end of a HRESULT-returning method.
//              Declares a ChkEnd label used for jumping to in error conditions
//              from CHK macros and returns hr.
//
//              This macro is recommend way of finishing a method body:
//              it assumes one exit point and no explicit cleanup code (destructors
//              are assumed as proper mechanism for cleanup).
//
//              In rare cases when explicit cleanup is needed or when return type
//              is not HRESULT, CHK_END_IGNOREHR macro should be used for indicating end of
//              main method logic.
//
//              ChkEnd label is not supposed to be used directly in code.
//              Only CHK_* macros should use it.
//
//              Do not use CHK_RETURN as regular return statement in the middle
//              of code. This code is incorrect: "if (c) CHK_RETURN();"
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_RETURN                                                              \
    CHK_END;                                                                    \
    return hr;                                                                  \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_END
//
//  Synopsis:   Indicates the end of a non-HRESULT-returning method.
//              Declares a ChkEnd label used for jumping to in error conditions
//              from CHK macros.
//
//              This macro should be used instead of CHK_END_IGNOREHR in cases
//              when no error handling is expected.
//
//  Usage:
//              void MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  CHK_END;
//              }
//
//--------------------------------------------------------------------------

#if 0 // STRICT version

#define CHK_END                                                                 \
    __noop; }                                                                   \
ChkEnd:

#else // TOLERANT version

#define CHK_END                                                                 \
    goto ChkEnd;                                                                \
    }                                                                           \
ChkEnd:                                                                         \

#endif


//+-------------------------------------------------------------------------
//
//  Macro:      CHK
//
//  Synopsis:   Evaluates the expr and assigns its resulting value to hr variable.
//              Checks the hr value for success and jumps to ChkEnd label
//              declared by CHK_RETURN or CHK_END_IGNOREHR macro in case of failure.
//              Needed for propagating error code from the callee to the caller.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//
//                  CHK(CallAnotherMethod(a, b, c));
//                  CHK(CallYetAnotherMethod());
//                  CHK(CallMoreMethod());
//
//                  CHK_RETURN;
//              }
//
//  Note:       do-while(false) construct is used to allow CHK as a single-
//              statement construct, such as:
//              if (condition) CHK(A); else CHK(B);
//              It allows to use ";" after CHK.
//--------------------------------------------------------------------------

#define CHK(expr)                                                               \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    void * hrTransact; /* this is here to prevent CHK inside a TRANSACT block*/ \
    (hrTransact);                                                               \
    hr = (expr);                                                                \
    if (FAILED(hr)) goto ChkEnd;                                                \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \


//+-------------------------------------------------------------------------
//
//  Function:   StrictResult
//
//  Synopsis:   Converts any success codes other than S_OK, like S_FALSE, into
//              E_UNEXPECTED. There is a lot of old code which does !hr checks
//              and lets S_FALSE propagate into SUCCEEDED(hr) checks. This insures
//              that if S_FALSE is considered an error that SUCCEEDED will treat
//              it as one. It also ensure that legacy !hr checks continue to work.
//
//--------------------------------------------------------------------------

inline HRESULT StrictResult(HRESULT hr)
{
    return (hr > 0) ? E_UNEXPECTED : hr;
}

//+-------------------------------------------------------------------------
//
//  Macro:      SUCCEEDED_STRICT
//
//  Synopsis:   Version of SUCCESS macro that is only true if hr == S_OK.
//              Success codes that are not S_OK are converted to E_UNEXPECTED.
//
//  Note:       This IS more expensive than the normal macro. Only use it when
//              working with functions that are not well formed. This is common
//              when working with external COM interfaces or with legacy code
//              that used either !hr checks or hr == S_OK checks.
//
//--------------------------------------------------------------------------

#define SUCCEEDED_STRICT(hr) (SUCCEEDED(StrictResult(hr)))

//+-------------------------------------------------------------------------
//
//  Macro:      FAILED_STRICT
//
//  Synopsis:   Version of FAILED macro that is only true if hr != S_OK.
//
//  Note:       This IS more expensive than the normal macro. Only use it when
//              working with functions that are not well formed. This is common
//              when working with external COM interfaces or with legacy code
//              that used either !hr checks or hr == S_OK checks.
//
//--------------------------------------------------------------------------

#define FAILED_STRICT(hr) (FAILED(StrictResult(hr)))

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_STRICT
//
//  Synopsis:   Similar to CHK, but only S_OK is considered SUCCESS. Any
//              success code that is not S_OK is converted to E_UNEXPECTED.
//              Helps for using CHK macros in older code that was using
//              !hr instead of SUCCEEDED macro.
//
//  Note:       This IS more expensive than the normal macro. Only use it when
//              working with functions that are not well formed. This is common
//              when working with external COM interfaces or with legacy code
//              that used either !hr checks or hr == S_OK checks.
//
//--------------------------------------------------------------------------

#define CHK_STRICT(expr)                                                        \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    void * hrTransact; /* this is here to prevent CHK inside a TRANSACT block*/ \
    (hrTransact);                                                               \
    hr = (expr);                                                                \
    if (FAILED_STRICT(hr)) goto ChkEnd;                                         \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_STRICT_HR
//
//  Synopsis:   Similar to CHK_STRICT where only S_OK is considered SUCCESS. Any
//              success code that is not S_OK is converted to the passed in hrError.
//              Helps for using CHK macros in older code that was using
//              !hr instead of SUCCEEDED macro.
//
//  Note:       This IS more expensive than the normal macro. Only use it when
//              working with functions that are not well formed. This is common
//              when working with external COM interfaces or with legacy code
//              that used either !hr checks or hr == S_OK checks.
//
//--------------------------------------------------------------------------

#define CHK_STRICT_HR(expr, hrError)                                            \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    void * hrTransact; /* this is here to prevent CHK inside a TRANSACT block*/ \
    (hrTransact);                                                               \
    hr = (expr);                                                                \
    if (FAILED(hr)) goto ChkEnd;                                                \
    if (FAILED_STRICT(hr))                                                                 \
    {                                                                           \
        hr = hrError;                                                           \
        goto ChkEnd;                                                            \
    }                                                                           \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      CHKB_HR
//
//  Synopsis:   Assigns the hrError value to the hr variable and jumps
//              to ChkEnd if the condition parameter is false.
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  CHKB_HR(condition, hrError);
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------
#define CHKB_HR(condition, hrError)                                             \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    if (!(condition))                                                           \
    {                                                                           \
        hr = hrError;                                                           \
        goto ChkEnd;                                                            \
    }                                                                           \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \


//+-------------------------------------------------------------------------
//
//  Macro:      CHKB
//
//  Synopsis:   Evaluates a Boolean condition in a CHK context, throwing
//              E_UNEXPECTED if the condition evaluates to false.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  CHKB(FunctionThatReturnsBool(...));
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------
#define CHKB(condition) CHKB_HR(condition, E_UNEXPECTED)

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_BEGIN_TRANSACTION
//              CHK_AND_CONTINUE
//              CHK_END_TRANSACTION
//
//  Synopsis:   There are cases where we want to keep the error, but continue
//              with program execution flow. e.g. passivation.
//
//              this is a transactional style coding pattern where we want to block
//              out a set of steps that need to all execute, but if any of them fail
//              then the transaction returns an error code.
//
//              This set of macros is intended to be used together. The name of these
//              macros are intentionally bulky to make it visible in code as being something 'different'.
//
//              Not asserting on an error is also intentional. Code should not assert on expected error
//              conditions.
//
//              BEGIN_TRANSACTION declares a local-copy hresult, hrLocal, and opens up an execution block
//
//              CHK_AND_CONTINUE makes the (expr) call and collects the return value;
//              if there was a previous error, hr, then this is kept - first error in a
//              is the one that is returned. If there was no previous error, then we adopt
//              the hrLocal as the new/currrent hr state
//
//              CHK_END_TRANSACTION closes the execution block, and tests hr for failure
//              and only continues if there is success.
//
//  Usage:
//              HRESULT CleanupMySubObjectsForShutdown()
//              {
//                  CHK_START;
//                  // we want to call Method2() and Method3() even if Method1() has a return error
//                  // if any return an error we want to propgate and return the first one
//                  CHK_BEGIN_TRANSACTION;
//                  CHK_AND_CONTINUE(Method1());
//                  CHK_AND_CONTINUE(Method2());
//                  CHK_AND_CONTINUE(Method2());
//                  CHK_END_TRANSACTION;
//
//                  we only get here if the above transaction completed without error
//                  method4();
//
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_BEGIN_TRANSACTION                                                       \
    {                                                                               \
        fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
        HRESULT & hr = hrChk; /* Prevent hr from being redeclared */                \
        (hr); /* reference hr */                                                    \
        HRESULT hrTransact = S_OK /* no ; to force one after the macro */           \



#define CHK_AND_CONTINUE(expr)                                                  \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    hrTransact = (expr);                                                        \
    if (SUCCEEDED(hr)) { hr = hrTransact; }                                     \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \



#define CHK_END_TRANSACTION                                                     \
    }                                                                           \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    if (FAILED(hr)) goto ChkEnd;                                                \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \


//+-------------------------------------------------------------------------
//
//  Macro:      CHK_VOID
//
//  Synopsis:   Evaluates the expr and assigns S_OK to hr.
//              Used for readability to indicate that this method is designed
//              witout any result and error checking.
//              We should avoid such methods in the new api.
//              Use this macro for old api that we cannot change.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//
//                  CHK_VOID(CallOldMethod(a, b, c));
//
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_VOID(expr)                                                          \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    (expr);                                                                     \
    hr = S_OK;                                                                  \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \


//+-------------------------------------------------------------------------
//
//  Macro:      CHK_CATCH_ERROR
//
//  Synopsis:   Assigns the expr value to hr variable.
//              Checks the hr value for failure and returns TRUE if failed.
//              Does not jump to the ChkEnd label.
//              Intended to be used for "exception catching".
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  if (CHK_CATCH_ERROR(CallAnotherMethod(a, b, c))
//                  {
//                      DoErrorRecoveryOrCleanup();
//                      if (recovery_is_impossible)
//                      {
//                          CHK_RETHROW_ERROR(); // propagate the error or throw another error
//                      }
//                      CHK_RESET_CAUGHT_ERROR(); // we recovered, we will be OK.
//                  }
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_CATCH_ERROR(expr)                                                   \
    (FAILED(hr = (expr)))                                                       \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_RETHROW_ERROR
//
//  Synopsis:   Re-throws the last thrown error. Supposed to be used from
//              CHK_CATCH_ERROR block after local resource cleanup
//              to propagate the original error further through the call stack.
//
//  Usage:
//              HRESULT MyMethod()
//              {
//                  CHK_START;
//                  ...
//                  if (CHK_CATCH_ERROR(Subroutine()))
//                  {
//                      // Local recovery goes here
//                      CHK_RETHROW_ERROR(); // the same error is propagated further
//                  }
//                  CHK_RESET_CAUGHT_ERROR(); // we recovered, we will be OK.
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_RETHROW_ERROR()                                                     \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    goto ChkEnd;                                                                \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_RESET_CAUGHT_ERROR
//
//  Synopsis:   Resets the error caught in the CHK_CATCH_ERROR block.
//
//  Usage:      See CHK_CATCH_ERROR usage.
//
//--------------------------------------------------------------------------

#define CHK_RESET_CAUGHT_ERROR()                                                \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    Assert(FAILED(hr));                                                         \
    hr = S_OK;                                                                  \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_VERIFY/CHK_ASSERT
//
//  Synopsis:   CHK_VERIFY and CHK_ASSERT are exactly the same.
//              If the given condition is true, does nothing.
//              If the condition is false, then asserts in debug and throws
//              E_UNEXPECTED error in both debug and retail builds.
//
//  Usage:
//              HRESULT MyMethod(int iParameter)
//              {
//                  CHK_START;
//                  CHK_VERIFY(iParameter > 0);
//                  CHK_VERIFY(iParameter < MAX_PARAMETER_VALUE);
//                  ...
//                  CHK_RETURN();
//              }
//
//  Note:       Use this macro only in situations where you can validly use
//              Assert(...). Do not use it to check conditions that form
//              part of your routine's normal logic.
//
//--------------------------------------------------------------------------

#define CHK_VERIFY(condition) CHK_VERIFY_MSG(condition, #condition)

#define CHK_VERIFY_MSG(condition, szMessage)                           \
    CHK_VERIFY_MSG2(condition, E_UNEXPECTED, szMessage)

#define CHK_VERIFY_MSG2(condition, errorCode, szMessage)               \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    if (!(condition))                                                           \
    {                                                                           \
        hr = errorCode;                                                         \
        AssertSz(false, szMessage);                                             \
        goto ChkEnd;                                                            \
    }                                                                           \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

#define CHK_ASSERT CHK_VERIFY
#define CHK_ASSERT_MSG CHK_VERIFY_MSG
#define CHK_ASSERT_MSG2 CHK_VERIFY_MSG2

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_POINTER_ARG
//
//  Synopsis:   Validates that the given pointer is not null.
//              Throws an error if it is null.
//              Intended for validating input parameters.
//              Should not be used for checking failed memory allocations.
//
//  Usage:
//              HRESULT MyMethod(__in void* pData)
//              {
//                  CHK_START;
//                  CHK_POINTER_ARG(pData);
//                  ...
//                  CHK_RETURN;
//              }
//
//--------------------------------------------------------------------------

#define CHK_POINTER_ARG(pointer)   CHK_ASSERT_MSG2((pointer) != NULL, E_POINTER, "Null pointer is not expected for this argument")
#define CHK_VERIFY_POINTER_ARG(pointer)   CHK_VERIFY_MSG2((pointer) != NULL, E_POINTER, "Null pointer is not expected for this argument")


//+-------------------------------------------------------------------------
//
//  Macro:      CHK_POINTER_ALLOC
//
//  Synopsis:   Validates that the memory allocation is successful.
//              Induces abandonment if the argument is null.
//              Intended for using immediately after calling new.
//
//      THIS MACRO IS OBSOLETE.  operator new will induce abandonment
//      if allocation fails.  If you call other allocators directly,
//      such as VirtualAlloc or GlobalAlloc, then use
//      Abandonment::CheckAllocation() instead.
//
//--------------------------------------------------------------------------

#define CHK_POINTER_ALLOC(pointer)   Abandonment::CheckAllocation(pointer)
#define CHK_VERIFY_POINTER_ALLOC(pointer)   Abandonment::CheckAllocation(pointer)


//+-------------------------------------------------------------------------
//
//  Macro:      CHK_FLOAT_ARG
//
//  Synopsis:   Validates that a float is not infinite or a NaN
//              Intended for validating input parameters.
//              The error code is the first param for neater alignment.
//
//  Usage:
//              HRESULT MyMethod(float alpha)
//              {
//                  CHK_START;
//                  CHK_FLOAT_ARG(E_INVALIDARG, alpha);
//                  ...
//                  CHK_RETURN;
//              }
//
//              You can pass a success HRESULT to bail out with a no-op but
//              remember to initialize any out-params beforehand.
//
//--------------------------------------------------------------------------

#define CHK_FLOAT_ARG(hrReturn, floatValue)                                     \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    if (!isFinite(floatValue))                                                  \
    {                                                                           \
        hr = hrReturn;                                                          \
        goto ChkEnd;                                                            \
    }                                                                           \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      CHK_ACQUIRESYNC
//
//  Synopsis:   Used for calling IDXGIKeyedMutex::AcquireSync in place of CHK.
//
//--------------------------------------------------------------------------

#define CHK_ACQUIRESYNC(expr)                                                   \
do {                                                                            \
    fInCHKScope; /* reference to verify CHK is being called before CHK_END */   \
    void * hrTransact; /* this is here to prevent CHK inside a TRANSACT block*/ \
    (hrTransact);                                                               \
    hr = (expr);                                                                \
    if (hr == WAIT_ABANDONED || hr == WAIT_TIMEOUT) hr = E_FAIL;                \
    if (FAILED(hr)) goto ChkEnd;                                                \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \

//+-------------------------------------------------------------------------
//
//  Macro:      WATSON_ASSERT
//
//  WARNING:    This will crash the process both in debug and retail builds
//              when the given condition is not met
//
//  Synopsis:   Some of the asserts in a file are important enough that if
//              they fire there will be data corruption.  The subsequent Watson
//              buckets "some time after" the corruption are barely actionable
//              because they represent subsequent catastrophe rather than the
//              root problem.
//
//              To address this issue, we are promoting selective asserts to
//              WATSON_ASSERT which leverages the __debugbreak mechanism
//              to send off a Watson bucket at the point of assertion failure.
//
//              This is a "hard assert" which differs from the standard
//              Assert() in that it is evaluated in both debug and retail
//              builds and makes a call to __debugbreak.
//              If the given condition is true, does nothing.
//              If the condition is false, then asserts in debug and calls
//              __debugbreak
//
//  Usage:
//              HRESULT MyMethod(int iParameter)
//              {
//                  ...
//                  WATSON_ASSERT(iParameter > 0);
//                  WATSON_ASSERT(iParameter < MAX_PARAMETER_VALUE);
//                  ...
//              }
//
//--------------------------------------------------------------------------
#define WATSON_ASSERT(condition) WATSON_ASSERT_MSG(condition, #condition)

#define WATSON_ASSERT_MSG(condition, szMessage)                                 \
do {                                                                            \
    if (!(condition))                                                           \
    {                                                                           \
        EnableTag(tagHardAssert, TRUE);                                         \
        AssertSz(false, szMessage);                                             \
        __debugbreak();                                                         \
    }                                                                           \
__pragma(warning(push))                                                         \
__pragma(warning(disable:4127))                                                 \
} while(false)                                                                  \
__pragma(warning(pop))                                                          \


//+-------------------------------------------------------------------------
//
//  Macro:      FailFastAssert
//
//  WARNING:    This will crash the process both in debug and retail builds
//              when the given condition is not met.
//              In debug builds "szMessage" will assert a message.
//
//  Usage:
//              HRESULT MyFunc(int iParam)
//              {
//                  ...
//                  FailFastAssert(iParam >= 0, "iParam must not be negative.");
//                  ...
//              }
//
//--------------------------------------------------------------------------
LONG RaiseFailFastExceptionFilter(__in PEXCEPTION_POINTERS pExceptionInfo);

#define FailFastAssert(condition, szMessage)           \
if (!(condition))                                      \
{                                                      \
    AssertSz(false, szMessage);                        \
    EXCEPTION_POINTERS exceptionPointers = {};         \
    RaiseFailFastExceptionFilter(&exceptionPointers);  \
}                                                      \
