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

//  File:       SmartPointer.hxx
//  Abstract:   Smart pointers for ref-counted objects
//
// A summary of which ref does what:
//
//                 Blocks GC      Blocks         Blocks     SmartPointer 
// Method          Finalize(1)    Passivate      Delete     to use
// --------------------------------------------------------------------------------
// AddRef             X               X            X        TSmartPointer<>
// AddInternalRef                     X            X        TSmartInternalPointer<>
// SubAddRef                                       X        TSmartWeakPointer<>
//
// Note 1: Blocks finalize on attached JS Var objects because it causes CBase's 
//         implementation of JS_DeterminePinned to pin the object.


#pragma once

// Use this macro to resolve ambiguous access of AddRef/Release in classes that have multiple inheritance paths to
// to these methods.
#define DECLARE_DISAMBIGUATE_ADDREF_RELEASE(base)                                           \
    STDMETHOD_(ULONG, AddRef)() override { return static_cast<base*>(this)->AddRef(); }     \
    STDMETHOD_(ULONG, Release)() override { return static_cast<base*>(this)->Release(); }

#pragma warning(push)

// Warning 4127: "conditional expression is constant"
// Before this change, this warning did not need to be disabled because SmartPointer.hxx 
// was only #included inside header files that already had this warning disabled.  We now need to use  
// smartpointers in hxx files that comes before the header that disable the warning (ex: cdbase.hxx)
// hence the need to disable it here
#pragma warning(disable:4127)

class CStrongReferenceTraits;

//+-------------------------------------------------------------------------
//
//  Class:      TSmartPointer
//
//  Synopsis:   Generic type for ref-counting objects.
//
//              Define new reference traits below as needed.
//
//--------------------------------------------------------------------------

template <class T, class TReferenceTraits = CStrongReferenceTraits> 
class TSmartPointer
{
    friend T; // T is a friend to allow access to the create methods
    template <typename Q, typename QReferenceTraits> friend class TSmartPointer;

public:

    // Default constructor. Initializes with NULL
    inline TSmartPointer() throw()
        : m_pT(NULL)
    {
    }

    // Copy constructor from a non-smart pointer
    inline TSmartPointer(T* pT) throw()
        : m_pT(pT) 
    { 
        TReferenceTraits::AddRef(m_pT);
    }

    // Copy constructor from a smart pointer
    inline TSmartPointer(const TSmartPointer& rT) throw()
        : m_pT(rT.m_pT)
    { 
        TReferenceTraits::AddRef(m_pT); 
    }

    // Explicit conversion to base - Q must inherit T
    template <typename Q, typename QReferenceTraits>
    explicit inline TSmartPointer(const TSmartPointer<Q, QReferenceTraits>& rT) throw()
        : m_pT(rT.m_pT)
    { 
        TReferenceTraits::AddRef(m_pT); 
    }

    inline ~TSmartPointer() throw()
    {
        Release();
    }

    inline void Release() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        TReferenceTraits::Release(pT); 
    }

    // Casting to T*
    inline operator T*() const throw()
    { 
        return m_pT; 
    }

    // A compiler bug current prevents the introduction of this operator into our codebase.  
    // The VS 2015 toolset will fix the error and allow layout code to stop this double-casting pattern: ((Q*)(T*)spT).QMethod()
    //
    // Explicit casting to Q*
    // template <typename Q>
    // inline explicit operator Q*() const throw()
    // { 
    //     return m_pT; 
    // }

    // Dereferencing into T structure
    inline T* operator ->() const throw()
    { 
        return m_pT; 
    }

    // Assignment from non-smart pointer
    inline const TSmartPointer& operator=(T* pT) throw()
    {
        SetReference(pT); 
        return *this; 
    }

    // Assignment from smart pointer
    inline const TSmartPointer& operator=(const TSmartPointer& rT) throw()
    { 
        TReferenceTraits::AddRef(rT.m_pT); 
        T* pTOld = m_pT; 
        m_pT = rT.m_pT; 
        TReferenceTraits::Release(pTOld); 
        return *this; 
    }

    // Assignment from smart pointer
    template <typename Q, typename QReferenceTraits>
    inline const TSmartPointer& operator=(const TSmartPointer<Q, QReferenceTraits>& rT) throw()
    { 
        TReferenceTraits::AddRef(rT.m_pT); 
        T* pTOld = m_pT; 
        m_pT = rT.m_pT; 
        TReferenceTraits::Release(pTOld); 
        return *this; 
    }

    // Getting a pointer to the pointer
    // convenience function for OLE scenario when object already has refcount. Use with care. e.g.
    // TSmartPointer<IDataObject> pIDataObject;
    // if( SUCCEEDED(pIUnknown->QueryInterface( IID_IDataObject, &pIDataObject) && pIDataObject )
    // {  pIDataObject->GetData( ... );
    //
    inline T** operator &() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        TReferenceTraits::Release(pT); 
        return &m_pT; 
    }

    // take out a ref-counted object from a TSmartPointer without decreasing refcount. Used in OLE and Automation
    // scenarios when ref-counted interfaces need to be passed to clients
    inline T* Extract() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        return pT; 
    }

    // Caller is responsible for calling add ref on it.
    // Used in scenarios where we need to get the raw pointer value
    inline T* GetRawPointer() const throw()
    { 
        return m_pT; 
    }

    // Support for HtmlPresenter types
    inline T* Dereference() const throw()
    { 
        return m_pT; 
    }

    // Support for HtmlPresenter types
    inline void SetReference(T* pT) throw()
    {
        TReferenceTraits::AddRef(pT); 
        T* pTOld = m_pT; 
        m_pT = pT; 
        TReferenceTraits::Release(pTOld);  
    }

    // Take an unchecked address of the pointer.
    inline T** UnsafeAddress() throw()
    {
        return &m_pT;
    }

    // sets ref-counted object without increasing refcount. This is convenient for cases where you have functions
    // that return ref-counted objects. "new CRefCountedObject()" is an example
    template<typename InputType>
    inline T* TransferFrom(InputType* pT) throw()
    { 
        T* pTOld = m_pT; 
        m_pT = pT;
        TReferenceTraits::Release(pTOld); 
        return m_pT; 
    }

    // moves ownership from the smart pointer reference to this smart pointer; the ref-count remains unchanged
    template <typename Q> // Q must inherit T (or be T)
    inline T* TransferFrom(TSmartPointer<Q, TReferenceTraits>& rT)
    { 
        T* pTOld = m_pT;

        // Move the pointer without touching reference counter on both source and destination
        m_pT = rT.m_pT;
        rT.m_pT = nullptr;

        TReferenceTraits::Release(pTOld); 
        return m_pT; 
    }

    // gets an additional reference and puts it in *ppT.
    template<typename OutType>
    inline void CopyTo(_Out_ OutType** ppOT) const
    {
        TReferenceTraits::AddRef(m_pT); 
        *ppOT = m_pT;
    }

    // Swaps the inner pointer this smart pointer with another smart pointer without changing the ref-counts
    template <class T, class TReferenceTraits>
    inline void Swap(TSmartPointer<T, TReferenceTraits>& rT)
    {
        T* pTemp = m_pT;
        m_pT = rT.m_pT;
        rT.m_pT = pTemp;
    }

    //+-------------------------------------------------------------------------
    //
    //  Member:     TSmartPointer<T>::Create
    //
    //  Synopsis:   Template for a static Create method
    //
    //              A helper for implementing Class:Create pattern in every
    //              ref-counted class T.
    //
    //              This pattern assumes that default constructor T::T() is private
    //              which makes it inaccessible for allocating T on the stack
    //              or using the new operator.
    //  
    //              We avoid new operator as a constructor cannot propagate error codes (HRESULT).
    //              and because it cannot be virtual.
    //
    //              Instead, T::Create method should be used for object creation.
    //              For error handling this method returns HRESULT,
    //              it calls private default constructor which is supposed to do 
    //              only never-failing initialization, and calls a virtual T::Initialize
    //              method for error-prone initialization.
    //
    //              Because both default constructor T::T() and T::Initialize() are private
    //              or protected, this generic implementation assumes that
    //              TSmartPointer<T> is declared as a friend of T - 
    //              to make default constructor T::T() and T::Initialize() accessible.
    //
    //  Usage:
    //              With this helper implementing Create method in CMyClass is really easy:
    //
    //              HRESULT CMyClass::Create(SpCMyClass& spNewMyClass)
    //              {
    //                  return SpCMyClass::Create(spNewMyClass);
    //              }
    //
    //  Note:      
    //              There are several overrides for Create method with additional
    //              parameters passed to a constructor.
    //              All them assume that a default constructor exists with a matching
    //              list of parameters, and they are stored in constructor,
    //              so that T::Initialize method in all cases is parameterless.
    //
    //              All the Create templates defined below ar supposed to be called
    //              from inside a T::Create public method only - never directly.
    //              That is why they are declared as private, and the TSmartPointer<T>
    //              declares T as its friend.
    //
    //--------------------------------------------------------------------------

private: 
    // This is a variadic implementation of Create().  It is named Create2() to prevent conflicts with Create()
    // during method overload resolution.  All uses of Create() should eventually be converted to use Create2(),
    // at which point we can retire Create() and rename Create2() to Create().
    //
    // Note that this method places the return type at the beginning of the parameter list, rather than the end.
    // Placing it at the end caused problems with MSVC's variadic template implementation.
    //
    // This method should be used with types whose Initialize() method returns HRESULT.
    template<typename... Arguments>
    static HRESULT Create2(_Out_ TSmartPointer& spNew, Arguments... args)
    {
        CHK_START;
        TSmartPointer<T> sp;
        spNew = NULL;
        sp.TransferFrom(new T(args...));
        Assert(sp->GetRefs() == 1);
        CHK(sp->Initialize());
        spNew.TransferFrom(sp.Extract());
        CHK_RETURN;
    }

    template<typename... Arguments>
    static void VCreate2(_Out_ TSmartPointer& spNew, Arguments... args)
    {
        TSmartPointer<T> sp;
        spNew = NULL;
        sp.TransferFrom(new T(args...));
        Assert(sp->GetRefs() == 1);

        __if_exists(T::Initialize)
        {
            typedef void(T::*init_t) ();
            init_t init = &T::Initialize;
            UNREFERENCED_PARAMETER(init);

            sp->Initialize();
        }

        spNew.TransferFrom(sp.Extract());
    }

protected:
    T* m_pT;
};

//+-------------------------------------------------------------------------
//
//  Class:      TSmartPointerConst
//
//  Synopsis:   Generic type for smart pointer to ref-counted objects
//              - same as TSmartPointer with const constraint.
//
//--------------------------------------------------------------------------
template <class T, class TReferenceTraits = CStrongReferenceTraits> 
class TSmartPointerConst
{
    friend T; // T is a friend to allow access to the create methods
    template <typename Q, typename QReferenceTraits> friend class TSmartPointerConst;

public:

    // Default constructor. Initializes with NULL
    inline TSmartPointerConst() throw()
        : m_pT(NULL)
    {
    }

    // Copy constructor from a non-smart pointer
    inline TSmartPointerConst(const T* pT) throw()
        : m_pT(const_cast<T*>(pT)) 
    { 
        TReferenceTraits::AddRef(m_pT); 
    }

    // Copy constructor from a smart pointer
    inline TSmartPointerConst(const TSmartPointerConst& rT) throw()
        : m_pT(rT.m_pT)
    { 
        TReferenceTraits::AddRef(m_pT); 
    }

    inline ~TSmartPointerConst() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        TReferenceTraits::Release(pT);
    }

    inline void Release() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        TReferenceTraits::Release(pT); 
    }

    // Casting to T*
    inline operator const T*() const throw()
    { 
        return m_pT; 
    }

    // A compiler bug current prevents the introduction of this operator into our codebase.  
    // The VS 2015 toolset will fix the error and allow layout code to stop this double-casting pattern: ((Q*)(T*)spT).QMethod()
    //
    // Explicit casting to Q*
    // template <typename Q>
    // inline explicit operator const Q*() const throw()
    // { 
    //     return m_pT; 
    // }

    // Dereferencing into T structure
    inline const T* operator ->() const throw()
    { 
        return m_pT; 
    }

    // Assignment from non-smart pointer
    inline const TSmartPointerConst& operator=(const T* pT) throw()
    { 
        SetReference(pT);
        return *this; 
    }

    // Assignment from smart pointer
    inline const TSmartPointerConst& operator=(const TSmartPointerConst& rT) throw()
    { 
        TReferenceTraits::AddRef(rT.m_pT); 
        T* pTOld = m_pT; 
        m_pT = rT.m_pT; 
        TReferenceTraits::Release(pTOld); 
        return *this; 
    }

    // Getting a pointer to the pointer
    // convenience function for OLE scenario when object already has refcount. Use with care. e.g.
    // TSmartPointer<IDataObject> pIDataObject;
    // if( SUCCEEDED(pIUnknown->QueryInterface( IID_IDataObject, &pIDataObject) && pIDataObject )
    // {  pIDataObject->GetData( ... );
    //
    inline const T** operator &() throw()
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        TReferenceTraits::Release(m_pT);
        return &m_pT; 
    }

    // take out a ref-counted object from a TSmartPointer without decreasing refcount. Used in OLE and Automation
    // scenarios when ref-counted interfaces need to be passed to clients
    inline const T* Extract() throw()
    { 
        T* pT = m_pT;
        m_pT = NULL;
        return pT;
    }

    // sets ref-counted object without increasing refcount. This is convenient for cases where you have functions
    // that return ref-counted objects. "new CRefCountedObject()" is an example
    inline const T* TransferFrom(const T* pT) throw()
    { 
        T* pTOld = m_pT; 
        m_pT = const_cast<T*>(pT); 
        TReferenceTraits::Release(pTOld); 
        return m_pT; 
    }

    // moves ownership from the smart pointer reference to this smart pointer; the ref-count remains unchanged
    template <typename Q> // Q must inherit T (or be T)
    inline const T* TransferFrom(TSmartPointerConst<Q>& rT)
    { 
        const T* pTOld = m_pT;

        // Move the pointer without touching reference counter on both source and destination
        m_pT = const_cast<T*>(rT.m_pT); 
        rT.m_pT = nullptr;

        TReferenceTraits::Release(pTOld); 
        return m_pT; 
    }

    // gets an additional reference and puts it in *ppT.
    inline void CopyTo(__out const T** ppT)
    {
        TReferenceTraits::AddRef(m_pT);
        *ppT = m_pT;
    }

    // Support for HtmlPresenter types
    inline const T* Dereference() const throw()
    { 
        return m_pT; 
    }

    // Support for HtmlPresenter types
    inline void SetReference(const T* pT) throw()
    {
        TReferenceTraits::AddRef(const_cast<T*>(pT));
        T* pTOld = m_pT; 
        m_pT = const_cast<T*>(pT); 
        TReferenceTraits::Release(pTOld); 
    }

private: 
    // This is a variadic implementation of Create().  It is named Create2() to prevent conflicts with Create()
    // during method overload resolution.  All uses of Create() should eventually be converted to use Create2(),
    // at which point we can retire Create() and rename Create2() to Create().
    //
    // Note that this method places the return type at the beginning of the parameter list, rather than the end.
    // Placing it at the end caused problems with MSVC's variadic template implementation.
    //
    // This method should be used with types whose Initialize() method returns HRESULT.
    template<typename... Arguments>
    static HRESULT Create2(_Out_ TSmartPointerConst& spNew, Arguments... args)
    {
        CHK_START;
        TSmartPointerConst<T> sp;
        spNew = NULL;
        sp.TransferFrom(new T(args...));
        Assert(sp->GetRefs() == 1);
        CHK(sp->Initialize());
        spNew.TransferFrom(sp.Extract());
        CHK_RETURN;
    }

    template<typename... Arguments>
    static void VCreate2(_Out_ TSmartPointerConst& spNew, Arguments... args)
    {
        TSmartPointer<T> sp;
        spNew = NULL;
        sp.TransferFrom(new T(args...));
        Assert(sp->GetRefs() == 1);

        __if_exists(T::Initialize)
        {
            typedef void(T::*init_t) ();
            init_t init = &T::Initialize;
            UNREFERENCED_PARAMETER(init);

            sp->Initialize();
        }

        spNew.TransferFrom(sp.Extract());
    }

protected:
    T* m_pT;
};

//+-------------------------------------------------------------------------
//
//  Macro:      DeclareSmartPointer
//
//  Synopsis:   Declares a type for a smart pointer to a particular class
//
//  Parameters:
//              pointerType - name of a class implementing AddRef & Release.
//
//--------------------------------------------------------------------------

#define DeclareSmartPointer(type, pointerType)                    \
    type pointerType;                                             \
    typedef TSmartPointer<pointerType> Sp ## pointerType;         \
    typedef TSmartPointerConst<pointerType> Spc ## pointerType;   \
    MtExtern(pointerType)

//+-------------------------------------------------------------------------
//
//  Class:      CStrongReferenceTraits
//
//  Synopsis:   Traits for managing a strong reference via
//              AddRef() and Release().
//
//--------------------------------------------------------------------------
class CStrongReferenceTraits
{
public:
    template<class T>
    static void AddRef(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->AddRef();
        }
    }
    template<class T>
    static void Release(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->Release();
        }
    }
};

//+-------------------------------------------------------------------------
//
//  Class:      CWeakReferenceTraits
//
//  Synopsis:   Traits for managing a weak reference via
//              SubAddRef() and SubRelease().
//
//--------------------------------------------------------------------------
class CWeakReferenceTraits
{
public:
    template<class T>
    static void AddRef(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->SubAddRef();
        }
    }
    template<class T>
    static void Release(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->SubRelease();
        }
    }
};
template <class T> using TSmartWeakPointer = TSmartPointer<T, CWeakReferenceTraits>;

//+-------------------------------------------------------------------------
//
//  Class:      CInternalReferenceTraits
//
//  Synopsis:   Traits for managing a GC reference via
//              AddInternalRef() and ReleaseInternalRef().
//
//--------------------------------------------------------------------------
class CInternalReferenceTraits
{
public:
    template<class T>
    static void AddRef(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->AddInternalRef();
        }
    }
    template<class T>
    static void Release(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->ReleaseInternalRef();
        }
    }
};
template <class T> using TSmartInternalPointer = TSmartPointer<T, CInternalReferenceTraits>;

//+-------------------------------------------------------------------------
//
//  Class:      CPrivateReferenceTraits
//
//  Synopsis:   Traits for managing a strong reference via
//              PrivateAddRef() and PrivateRelease().
//
//--------------------------------------------------------------------------
class CPrivateReferenceTraits
{
public:
    template<class T>
    static void AddRef(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->PrivateAddRef();
        }
    }
    template<class T>
    static void Release(T* pBase)
    {
        if (pBase != nullptr)
        {
            pBase->PrivateRelease();
        }
    }
};
template <class T> using TSmartPrivatePointer = TSmartPointer<T, CPrivateReferenceTraits>;

//+-------------------------------------------------------------------------
//
//  Class:      Tree::NodeReferenceTraits
//
//  Synopsis:   Temporary traits class for managing a reference to DOM types
//              ANode and CTreeNode via NodeAddRef() and NodeRelease().
//
//--------------------------------------------------------------------------
namespace Tree
{
    class NodeReferenceTraits
    {
    public:
        template<class T>
        static void AddRef(T* pT)
        {
            if (pT != nullptr)
            {
                pT->NodeAddRef();
            }
        }
        template<class T>
        static void Release(T* pT)
        {
            if (pT != nullptr)
            {
                pT->NodeRelease();
            }
        }
    };
}

//+-------------------------------------------------------------------------
//
//  Class:      CDispOwnerNotifyTraits
//
//  Synopsis:   Traits for managing a sub reference via
//              SubAddRefOwner() and SubReleaseOwner()
//
//--------------------------------------------------------------------------
class CDispOwnerNotifyTraits
{
public:
    template<class T>
    static void AddRef(T* pDisp)
    {
        if (pDisp != nullptr)
        {
            pDisp->SubAddRefOwner();
        }
    }
    template<class T>
    static void Release(T* pDisp)
    {
        if (pDisp != nullptr)
        {
            pDisp->SubReleaseOwner();
        }
    }
};
template <class T> using TSmartDispOwnerNotifyPointer = TSmartPointer<T, CDispOwnerNotifyTraits>;

//+-------------------------------------------------------------------------
//
//  Class:      SmartDispClientDeferredReleaseReferenceTraits
//
//  Synopsis:   Traits for managing a reference to a Layout::SmartDispClient
//              (e.g. LayoutBox) that should defer its releases to the display
//              tree patch manager.
//
//--------------------------------------------------------------------------
class SmartDispClientDeferredReleaseReferenceTraits
{
public:
    template<class T>
    static void AddRef(_In_opt_ T* pSmartDispClient)
    {
        if (pSmartDispClient != nullptr)
        {
            pSmartDispClient->AddRef();
        }
    }

    template<class T>
    static void Release(_In_opt_ T* pSmartDispClient)
    {
        if (pSmartDispClient != nullptr)
        {
            if (pSmartDispClient->DisplayTreePatchManager() != nullptr)
            {
                pSmartDispClient->DisplayTreePatchManager()->DeferReleaseSmartDispClient(pSmartDispClient);
            }
            else
            {
                pSmartDispClient->Release();
            }
        }
    }
};

#pragma warning(pop)
