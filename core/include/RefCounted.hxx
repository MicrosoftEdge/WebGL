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

#ifdef PERFMETER
#define ENABLE_REFCOUNT_MEMORY_TAGGING
#endif

#ifdef ENABLE_REFCOUNT_MEMORY_TAGGING
#pragma message("RefCounted memory tracking enabled.")
#endif

// Single-thread ref count. Asserts if AddRef of Release are ever called from a thread other than the creator.
struct SingleThreadedRefCount
{
    SingleThreadedRefCount()
    {
#ifdef DbgInitThreadId
        DbgInitThreadId();
#endif
    }

    ULONG IncrementRefCount(__inout ULONG* pulRef)
    {
#ifdef DbgAssertCorrectThread
        DbgAssertCorrectThread();
#endif
        return ++(*pulRef);
    }

    ULONG DecrementRefCount(__inout ULONG* pulRef)
    {
#ifdef DbgAssertCorrectThread
        DbgAssertCorrectThread();
#endif
        return --(*pulRef);
    }

private:

#ifdef DbgDeclareThreadId
    DbgDeclareThreadId;
#endif

};

struct MultiThreadedRefCount
{
    ULONG IncrementRefCount(__inout ULONG* pulRef)
    {
        return InterlockedIncrement(pulRef);
    }

    ULONG DecrementRefCount(__inout ULONG* pulRef)
    {
        return InterlockedDecrement(pulRef);
    }
};

//+---------------------------------------------------------------------------
//
//  Class:      RefCounted
//
//  Synopsis:   RefCounted is a template that implements IUnknown refcounting
//              for a class. It handles creating and setting a refcount for
//              the class, implementing AddRef / Release with the normal COM
//              implementation and implements QueryInterface by returning
//              an error all of the time.
//
//              The guidelines for using this template are:
//               - The class to use it on must have IUnknown methods in the
//                 vtable.
//               - The class should declare its constructor protected so
//                 that RefCount can see it but other code cannot. This helps
//                 to drive creation from the RefCount Construct methods.
//               - The class constructor must have no parameters. All
//                 parameters for construction are passed to Initialize.
//               - The class must have a public Initialize method that returns
//                 a HRESULT.
//
//              To use RefCount with a class, find the place where an instance
//              of the class needs to be created and call the construct method
//              from there. Example:
//
//              TSmartPointer<BaseClass> spNewObj;
//              CHK(RefCounted<Class>::Create(10, spNewObj));
//
//              Where Class is a class that derives from BaseClass and has an
//              Initialize method that takes a single integer.
//
//----------------------------------------------------------------------------

template<typename BaseClass, typename Threading = SingleThreadedRefCount>
class RefCounted sealed : public BaseClass
{

#ifdef ENABLE_REFCOUNT_MEMORY_TAGGING
    DECLARE_MEMTRACK_NAME(RefCounted);
    static LPCSTR GetMemTrackerOwnerName()
    {
        return "mtRefCounted"; // need to provide the prefix to match the macros
    }
    #include "templateMemTracking.inl"
#endif

public:

    // This is a variadic implementation of Create().  It is named Create2() to prevent conflicts with Create()
    // during method overload resolution.  All uses of Create() should eventually be converted to use Create2(),
    // at which point we can retire Create() and rename Create2() to Create().
    //
    // Note that this method places the return type at the beginning of the parameter list, rather than the end.
    // Placing it at the end caused problems with MSVC's variadic template implementation.
    //
    // This method should be used with types whose Initialize() method returns HRESULT.
    template<typename ReturnClass, typename... Arguments>
    static HRESULT Create2(_Out_ TSmartPointer<ReturnClass> &spNewInstance, Arguments... args)
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(args...));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    // This is another version of Create(), which is variadic and which supports types whose Initialize method
    // returns HRESULT_VOID.
    template<typename ReturnClass, typename... Arguments>
    static void HRVCreate2(_Out_ TSmartPointer<ReturnClass> &spNewInstance, Arguments... args)
    {
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        HRESULT_VOID hrv = spInstance->Initialize(args...);
        UNREFERENCED_PARAMETER(hrv);

        spNewInstance.TransferFrom(spInstance.Extract());
    }

    // This is another version of Create(), which is variadic and which supports types whose Initialize method
    // returns void.
    template<typename ReturnClass, typename... Arguments>
    static void VCreate2(_Out_ TSmartPointer<ReturnClass> &spNewInstance, Arguments... args)
    {
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        VCreate2_ReturnVoidVerifyHelper<ReturnClass, Arguments...>(spInstance, args...);
        spNewInstance.TransferFrom(spInstance.Extract());
    }

    // Non-variadic versions of Create().  All uses of these Create() methods should eventually be converted
    // to use the variadic Create2() method.

#if MARK_NONVARIADIC_CREATE_DEPRECATED
#define NONVARIADIC_CREATE_DEPRECATED __declspec(deprecated("Change this code to use Create2() instead.  Move output parameter to beginning of parameter list."))
#else
#define NONVARIADIC_CREATE_DEPRECATED
#endif

    template<typename ReturnClass>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
         CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize());

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    // Pass input arguments by reference, not by value, to avoid copy constructor work.
    template<typename ReturnClass, typename P1>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        const P11& p11,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        const P11& p11,
        const P12& p12,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        const P11& p11,
        const P12& p12,
        const P13& p13,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        const P11& p11,
        const P12& p12,
        const P13& p13,
        const P14& p14,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    template<typename ReturnClass, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15>
    NONVARIADIC_CREATE_DEPRECATED
    static HRESULT Create(
        const P1& p1,
        const P2& p2,
        const P3& p3,
        const P4& p4,
        const P5& p5,
        const P6& p6,
        const P7& p7,
        const P8& p8,
        const P9& p9,
        const P10& p10,
        const P11& p11,
        const P12& p12,
        const P13& p13,
        const P14& p14,
        const P15& p15,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer< RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

#undef NONVARIADIC_CREATE_DEPRECATED

    template<typename Init1, typename P1, typename ReturnClass>
    static HRESULT CreateTemplateInit1(
        const P1& p1,
        TSmartPointer<ReturnClass> &spNewInstance
        )
    {
        CHK_START;

        // Create the RefCounted wrapper.  Any failure that returns
        // before the assignment below will destroy the object before
        // it is returned, thus ensuring either a successfully allocated
        // initialized object or nothing
        TSmartPointer<RefCounted<BaseClass, Threading> > spInstance;
        spInstance.TransferFrom(new RefCounted<BaseClass, Threading>);

        CHK(spInstance->Initialize<Init1>(p1));

        spNewInstance.TransferFrom(spInstance.Extract());

        CHK_RETURN;
    }

    STDMETHOD(QueryInterface)(
        __in REFIID riid,
        __out void **ppv
        )
    {
        __if_exists(BaseClass::QueryInterfaceImpl) {
            return BaseClass::QueryInterfaceImpl(riid, ppv);
        }

        __if_not_exists(BaseClass::QueryInterfaceImpl) {
            return E_NOTIMPL;
        }
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        return _Threading.IncrementRefCount(&_ulRefs);
    }

    STDMETHOD_(ULONG, Release)()
    {
        ULONG ulDecremented = _Threading.DecrementRefCount(&_ulRefs);
        if (ulDecremented == 0)
        {
            _ulRefs = ULREF_IN_DESTRUCTOR;
            __if_exists(BaseClass::OnRefCountedFinalRelease)
            {
                BaseClass::OnRefCountedFinalRelease();
            }
            delete this;
            return 0;
        }
        return ulDecremented;
    }

    ULONG GetRefs(void) const
    {
        return _ulRefs;
    }

private:
    // Constructor is private to force users of the class through the Create
    // methods above.
    RefCounted() :
       _ulRefs(1)
    {
    }

    template<typename ReturnClass, typename... Arguments>
    static void VCreate2_ReturnVoidVerifyHelper(_Out_ TSmartPointer<RefCounted<BaseClass, Threading> > &spNewInstance, Arguments... args)
    {
        spNewInstance.TransferFrom(new RefCounted<BaseClass, Threading>);
        __if_exists(BaseClass::Initialize)
        {
            // Verify the return value is void (by using "return" with this function which returns void).
            return spNewInstance->Initialize(args...);
        }
    }

private:
    ULONG _ulRefs;
    Threading _Threading;
};

#ifdef ENABLE_REFCOUNT_MEMORY_TAGGING
template<typename BaseClass, typename Threading = SingleThreadedRefCount>
PERFMETERTAG RefCounted<BaseClass, Threading>::s_mtThisType = 0;
#endif
