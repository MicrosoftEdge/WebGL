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

extern const __declspec(selectany) HRESULT E_NOTFOUND = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

//----------------------------------------------------------------------------
// NonCopyable - Base class for objects that can't be copied using assignement
// operators or copy constructors.  Objects that drive from this class
// typically provide a separate initialization function that reurn errors
// upon failure.  Constructors and assignment operators can not perform any
// operation that can fail because they have no good way to return errors.
//----------------------------------------------------------------------------
class NonCopyable
{
public:
    NonCopyable(){}
    ~NonCopyable(){}

private:
    NonCopyable(const NonCopyable& other);
    NonCopyable& operator=(const NonCopyable& other);
};

//----------------------------------------------------------------------------
// CDefaultTraits - Default helper for comparing, constructing, and
// assigning elements.
//
// Override if using complex types without operator== or operator= or copy
// constructors.
//----------------------------------------------------------------------------
template <class T>
class CDefaultTraits
{
public:
    static bool IsEqual(const T& t1, const T& t2)
    {
        return t1 == t2;
    }

    template<class X>
    static void Construct(
        __out T *pDest,                  // Pointer to destination element
        const X &xSrc                    // Source to assign
        )
    {
        // Use placement new to call the copy constructor.  Note that since we
        // don't use exceptions, constructors are not allowed to fail. Objects
        // that require more complex initialization should have a custom traits class
        // that contruct and then initialize returning errors as appropriate.
        new(pDest)T(xSrc);
    }

    template<class X>
    static void Assign(
        __in T *pDest,                  // Pointer to destination element
        const X &xSrc                   // Source to assign
        )
    {
        // Call the object's assignment operator.  Override this method if
        // with a custom traits class if assignment can fail.
        *pDest = xSrc;
    }
};

//----------------------------------------------------------------------------
// CModernArray - Dynamically sized array based off of ATL's CSimpleArray.
//----------------------------------------------------------------------------
template <class T, class TElementTraits = CDefaultTraits< T > >
class CModernArray : public NonCopyable
{
public:
    static const UINT NotFound;

    // Construction/destruction
    CModernArray();
    ~CModernArray();

    // Operations
    UINT        GetCount() const;
    UINT        GetCapacity() const;
    HRESULT_VOID RemoveAt(UINT nIndex);
    void        RemoveAll();
    void        RemoveAllAndMaintainCapacity();
    const T&    operator[] (UINT nIndex) const;
    T&          operator[] (UINT nIndex);
    T*          GetData() const;
    const T*    GetConstData() const;
    HRESULT_VOID Resize(UINT nNewSize);

    //
    // The following three functions are templated to permit the caller to
    // assign different types to our array.  The TElementTraits must support
    // the types used.  For example an array of CString objects might allow
    // both CString and PWSTR variable types to be added.
    //

    //
    // Removes the specified element from the array. All following elements are
    // shifted down.
    //
    template<typename X>
    HRESULT_VOID Remove(const X &x)
    {
        bool removed = TryRemove(x);
        UNREFERENCED_PARAMETER(removed);
        return S_OK_VOID;
    }

    template<typename X>
    bool TryRemove(const X &x)
    {
        UINT nIndex = Find(x);
        if (nIndex != NotFound)
        {
            RemoveAt(nIndex);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator==(const CModernArray & other) const
    {
        if (other._nSize != _nSize)
        {
            return false;
        }
        for (UINT i = 0; i < _nSize; i++)
        {
            if (!TElementTraits::IsEqual(other._aT[i], _aT[i]))
            {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const CModernArray & other) const
    {
        return !operator==(other);
    }

    //
    // Finds an element in the array. Returns the index of the found element, or
    // NotFound if the element is not found.
    //
    template<typename X>
    UINT
        Find
        (
        const X &x                      // Element to find
        )
        const
    {
        for (UINT i = 0; i < _nSize; i++)
        {
            if (TElementTraits::IsEqual(_aT[i], x))
            {
                return i;
            }
        }

        // Not found
        return NotFound;
    }

    //
    // Inserts an item at the specified index shifting following items up one slot.
    //
    // Parameters:
    //
    //      nIndex - Index where to insert
    //      x - Element to insert
    //
    template<typename X>
    HRESULT_VOID InsertAt(UINT nIndex, const X &x)
    {
        UINT nOldSize = _nSize;

        //
        // Add an empty slot at the specified index.
        //

        UINT cElemGrowTo;

        if (nIndex >= _nSize)
        {
            // check for integer overflow since nIndex is unbounded
            if (nIndex == UINT_MAX)
            {
                CheckedMath::StoreOverflowConditions(nIndex, UINT_MAX);
                Abandonment::ArithmeticOverflow();
            }
            cElemGrowTo = nIndex + 1;
        }
        else
        {
            // _nSize + 1 is <= UINT_MAX because size/capacity can be no bigger than UINT_MAX - 1.
            Assert(_nSize != UINT_MAX);
            cElemGrowTo = _nSize + 1;
        }

        const X *pX = &x;
        if (_nAllocSize < cElemGrowTo)
        {
            EnsureLargerCapacity(
                cElemGrowTo,
                reinterpret_cast<const void **>(&pX)
                );
        }

        if (nIndex < _nSize)
        {
            // Move the remaining elements out so there's room
            memmove((void*)(_aT + nIndex + 1), (void*)(_aT + nIndex), (_nSize - nIndex) * sizeof(T));
        }

        _nSize = cElemGrowTo;

        // If index is past the end, call default constructor
        // for all items between the old end and new index
        if (nIndex > nOldSize)
        {
            for (UINT i = nOldSize; i < nIndex; ++i)
            {
                new(_aT + i) T();
            }
        }

        ConstructAt(nIndex, *pX);

        return S_OK_VOID;
    }

    //
    // Appends element to the end of the array.
    //
    // Parameters:
    //
    //      x - Element to add
    //
    template<typename X>
    HRESULT_VOID Add(const X &x)
    {
        if (_nSize == _nAllocSize)
        {
            IncreaseCapacityAndAdd(x);
        }
        else
        {
            _nSize++;
            ConstructAt(_nSize - 1, x);
        }
        return S_OK_VOID;
    }

    // Set the specified element in the array
    template<typename X>
    HRESULT_VOID
        SetAt
        (
        UINT nIndex,                     // Element index to Set
        const X &x                      // Element to assign
        )
    {
        requires(nIndex < _nSize);

        // Verify signature of Assign method

        typedef void(*Assign_t)(T* dest, const X& x);
        Assign_t assign = TElementTraits::Assign;
        UNREFERENCED_PARAMETER(assign);

        TElementTraits::Assign(_aT + nIndex, x);
        return S_OK_VOID;
    }

    // Copy a C array of a type into the array
    template<typename X>
    HRESULT_VOID AddArray(__in const X *pxSource, UINT cItems)
    {
        UINT newSize = _nSize + cItems;
        if (newSize < _nSize)
        {
            CheckedMath::StoreOverflowConditions(newSize, _nSize);
            Abandonment::ArithmeticOverflow();
        }

        if (_nAllocSize < newSize)
        {
            EnsureLargerCapacity(
                newSize,
                reinterpret_cast<const void **>(&pxSource)
                );
        }

        for (UINT i = 0; i < cItems; i++)
        {
            Add(pxSource[i]);
        }
        return S_OK_VOID;
    }

    template<typename X, typename Traits>
    HRESULT_VOID AddArray(const CModernArray<X, Traits> &other)
    {
        AddArray(other.GetData(), other.GetCount());
        return S_OK_VOID;
    }

    HRESULT_VOID EnsureCapacity(UINT cCapacityDesired);
    HRESULT_VOID EnsureCapacityForNextElement();
    HRESULT_VOID EnsureCapacityForNextElements(UINT cElementsAdditional);
    HRESULT_VOID EnsureSize(UINT cDesired);

    // Sort the array using the given context and compare function
    template<typename C>
    void Sort(C* pContext, int(__cdecl* pFnCompare)(__in C* pContext, const T*, const T*))
    {
        Sorter<C> holder;
        holder._pContext = pContext;
        holder._pFnCompare = pFnCompare;

        ::qsort_s(
            _aT,
            _nSize,
            sizeof(T),
            &Sorter<C>::VoidCompareFunction,
            &holder
            );
    }

protected:
    void EnsureLargerCapacity(
        UINT cCapacityDesired,
        _Inout_opt_ const void **ppSourceData
        );

    template<typename X>
    void ConstructAt(UINT nIndex, const X &x);

    template<typename X>
    void IncreaseCapacityAndAdd(const X &x);

private:
    // Object to encapsulate strong casting on the C library qsort function
    template<typename C>
    struct Sorter
    {
        typedef int(__cdecl *PFNCompare)(C*, const T*, const T*);

        static int __cdecl VoidCompareFunction(__in void *pContext, const void* pA, const void* pB)
        {
            // Devoid the Sorter object
            Sorter<C>* pHolder = static_cast<Sorter<C>*>(pContext);

            // Isolate the function pointer to make the function call more readable
            PFNCompare pFnCompare = pHolder->_pFnCompare;

            // Call the given compare function
            return (*pFnCompare)(
                pHolder->_pContext,
                static_cast<const T*>(pA),
                static_cast<const T*>(pB)
                );
        }

        C* _pContext;
        PFNCompare _pFnCompare;
    };

private:
    T*          _aT;
    UINT        _nSize;
    UINT        _nAllocSize;
};


template <class T, class TElementTraits>
const UINT CModernArray<T, TElementTraits>::NotFound = UINT_MAX;

//
// Constructor
//
template <class T, class TElementTraits>
CModernArray<T, TElementTraits>::CModernArray()
    :
    _aT(NULL),
    _nSize(0),
    _nAllocSize(0)
{
}

//
// Destructor
//
template <class T, class TElementTraits>
CModernArray<T, TElementTraits>::~CModernArray()
{
    RemoveAll();
}

//
// Return the number of elements in the array
//
template <class T, class TElementTraits>
UINT CModernArray<T, TElementTraits>::GetCount() const
{
    return _nSize;
}

template <class T, class TElementTraits>
template <typename X>
void
CModernArray<T, TElementTraits>::IncreaseCapacityAndAdd(const X &x)
{
    Assert(_nSize == _nAllocSize);

    const X *pX = &x;

    // _nSize + 1 is <= UINT_MAX because size/capacity can be no bigger than UINT_MAX - 1.
    Assert(_nSize != UINT_MAX);
    EnsureLargerCapacity(
        _nSize + 1,
        reinterpret_cast<const void **>(&pX)
        );

    _nSize++;

    ConstructAt(_nSize - 1, *pX);
}

template <class T, class TElementTraits>
template <typename X>
void
CModernArray<T, TElementTraits>::ConstructAt(UINT nIndex, const X &x)
{
    TElementTraits::Construct(_aT + nIndex, x);
}

//
// Return the capacity of the array
//
template <class T, class TElementTraits>
UINT CModernArray<T, TElementTraits>::GetCapacity() const
{
    return _nAllocSize;
}

//
// Allocates sufficient memory to hold the desired number of items.
//
template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::EnsureCapacity
(
    UINT cCapacityDesired                // Capacity count to check and allocate
)
{
    if (_nAllocSize < cCapacityDesired)
    {
        EnsureLargerCapacity(
            cCapacityDesired,
            nullptr
            );
    }
    return S_OK_VOID;
}

//
// Allocates sufficient memory to hold the desired number of items.
// Takes a pointer to source data. If that data references memory
// in the existing array, it is modified such that it continues
// to reference memory inside the new array memory.
//
template <class T, class TElementTraits>
void
CModernArray<T, TElementTraits>::EnsureLargerCapacity
(
    UINT cCapacityDesired,
    _Inout_opt_ const void **ppSourceData
)
{
    requires(_nAllocSize < cCapacityDesired);

    // Cap the max size/capacity such that it always possible to represent _nSize + 1.
    // This cap allows this class to add a single element to the array
    // without checking for integer overflow.
    //
    // Always try to allocate twice the currently used memory to reduce number of reallocs
    // In case the current size crosses (UINT_MAX / 2) then try to allocate the maximum
    // supported capacity.
    //
    // Also, cap the size so that the number of bytes required fits within a size_t.
    // Static cast to UINT is acceptable as all numbers are less than UINT_MAX
    const UINT c_maxAllocSize = min(static_cast<UINT>(SIZE_MAX / sizeof(T)), UINT_MAX - 1);

    UINT nMaxTargetSize;
    if (_nSize < UINT_MAX / 2)
    {
        nMaxTargetSize = min(_nSize * 2, c_maxAllocSize);
    }
    else
    {
        nMaxTargetSize = c_maxAllocSize;
    }
    const UINT nNewAllocSize = max(nMaxTargetSize, cCapacityDesired);

    if (nNewAllocSize > c_maxAllocSize)
    {
        CheckedMath::StoreOverflowConditions(nNewAllocSize, c_maxAllocSize);
        Abandonment::ArithmeticOverflow();
    }

    //
    // If we have already allocated the array. Note, _nSize might be zero
    // at this point if we had resized to 0.
    //
    size_t cbCapacityNeeded = nNewAllocSize * sizeof(T);
    if (_aT)
    {
        T *aTOld = _aT;
        _MemRealloc((void **)&_aT, cbCapacityNeeded);

        // We are currently adding data to the array while changing
        // the capacity. Check to see if the data comes from within the
        // array. If so, rebase the data pointer into the new memory allocation
        if (ppSourceData != nullptr
            && aTOld != _aT
            )
        {
            const BYTE *pSourceData = reinterpret_cast<const BYTE *>(*ppSourceData);
            if (pSourceData >= reinterpret_cast<BYTE*>(aTOld)
                && pSourceData < reinterpret_cast<BYTE*>(aTOld + _nAllocSize))
            {
                *ppSourceData = reinterpret_cast<BYTE*>(_aT)+(pSourceData - reinterpret_cast<BYTE*>(aTOld));
            }
        }
    }
    else
    {
        _aT = static_cast<T*>(_MemAlloc(cbCapacityNeeded));
    }

    _nAllocSize = nNewAllocSize;
}

template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::EnsureCapacityForNextElement()
{
    // _nSize + 1 is <= UINT_MAX because size/capacity can be no bigger than UINT_MAX - 1.
    requires(_nSize != UINT_MAX);
    EnsureCapacity(_nSize + 1);
    return S_OK_VOID;
}

template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::EnsureCapacityForNextElements(UINT cElementsAdditional)
{
    UINT newSize = _nSize + cElementsAdditional;
    if (newSize < _nSize)
    {
        CheckedMath::StoreOverflowConditions(newSize, _nSize);
        Abandonment::ArithmeticOverflow();
    }
    else
    {
        EnsureCapacity(newSize);
    }
    return S_OK_VOID;
}


//
// Ensures size is at least the given size
//
template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::EnsureSize
(
    UINT cDesired                   // Capacity count to check and allocate
)
{
    if (GetCount() < cDesired)
    {
        Resize(cDesired);
    }
    return S_OK_VOID;
}

//
// Removes the element at the specified index and shift all following elements
// down an index.
//
template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::RemoveAt
(
    UINT nIndex                      // Index of element to remove
)
{
    requires(nIndex < _nSize);

    _aT[nIndex].~T();
    if (nIndex != (_nSize - 1))
    {
        memmove((void *)(_aT + nIndex), (void *)(_aT + nIndex + 1), (_nSize - (nIndex + 1)) * sizeof(T));
    }
    _nSize--;
    return S_OK_VOID;
}

//
// Removes all elements from the array
//
template <class T, class TElementTraits>
void CModernArray<T, TElementTraits>::RemoveAll()
{
    if (_aT != NULL)
    {
        for (UINT i = 0; i < _nSize; i++)
        {
            _aT[i].~T();
        }

        _MemFree(_aT);
        _aT = NULL;
    }
    _nSize = 0;
    _nAllocSize = 0;
}

//
// Removes all elements from the array but does not de-allocate the storage
//
template <class T, class TElementTraits>
void CModernArray<T, TElementTraits>::RemoveAllAndMaintainCapacity()
{
    if (_aT != NULL)
    {
        for (UINT i = 0; i < _nSize; i++)
        {
            _aT[i].~T();
        }
    }
    _nSize = 0;
}

//
// Array operator for accessing elements.  This version works for const arrays.
//
template <class T, class TElementTraits>
const T& CModernArray<T, TElementTraits>::operator[] (UINT nIndex) const
{
    requires(nIndex < _nSize);
    return _aT[nIndex];
}

//
// Array operator for accessing elements.
//
template <class T, class TElementTraits>
T& CModernArray<T, TElementTraits>::operator[] (UINT nIndex)
{
    requires(nIndex < _nSize);
    return _aT[nIndex];
}

//
// Returns a pointer to the data stored in the array.
//
template <class T, class TElementTraits>
T* CModernArray<T, TElementTraits>::GetData() const
{
    return _aT;
}

template <class T, class TElementTraits>
const T* CModernArray<T, TElementTraits>::GetConstData() const
{
    return _aT;
}

//
// If the array's size is less than the requested size, nNewSize elements
// are added to the array until it reaches the requested size. If the
// array's size is larger than the requested size, the elements closest to
// the end of the array are deleted until the array reaches the requested size
// If the present size of the array is the same as the requested size, no
// action is taken.
//
template <class T, class TElementTraits>
HRESULT_VOID
CModernArray<T, TElementTraits>::Resize
(
    UINT nNewSize                    // Count to resize to
)
{
    if (nNewSize > _nSize)
    {
        // Add more elements to the end of the array
        EnsureCapacity(nNewSize);
        // Call the NULL constructor to init each element
        for (UINT i = _nSize; i < nNewSize; ++i)
        {
            new(_aT + i) T();
        }
        _nSize = nNewSize;
    }
    else if (nNewSize < _nSize)
    {
        // Remove elements from the end of the array
        for (UINT i = nNewSize; i < _nSize; ++i)
        {
            _aT[i].~T();
        }
        _nSize = nNewSize;
    }
    return S_OK_VOID;
}

//----------------------------------------------------------------------------
// TRefCountedAbstractModernArray
//
//    Adds refcounting of the array itself, not of the child objects. If the
//    child objects should have ref-counting semantics, then you can use this
//    class but then you will have to use smart objects inside eg:
//    TRefCountedAbstractModernArray<TSmartPointer<CElement>>
//----------------------------------------------------------------------------
template<typename T>
class TRefCountedAbstractModernArray : public CModernArray<T>, public IUnknown
{
protected:
    HRESULT Initialize() { return S_OK; }
};

//----------------------------------------------------------------------------
// TRefCountedAbstractModernArray<float>
//
//    A specialized template of TRefCountedAbstractModernArray for float data
//    type. This specialization exposes "Sort" method.
//----------------------------------------------------------------------------
template<>
class TRefCountedAbstractModernArray<float> : public CModernArray<float>, public IUnknown
{
protected:
    HRESULT Initialize() { return S_OK; }

private:
    static int __cdecl compare(const void *a, const void *b)
    {
        if (*(static_cast<const float *>(a)) < *(static_cast<const float *>(b)))
        {
            return -1;
        }
        else if (*(static_cast<const float *>(a)) == *(static_cast<const float *>(b)))
        {
            return 0;
        }
        return 1;
    }

public:
    void Sort()
    {
        qsort(GetData(), GetCount(), sizeof(float), compare);
    }

    void DedupSort()
    {
        if (GetCount() > 1)
        {
            Sort();
            UINT current = 1;

            // Do not cache GetCount() here, because RemoveAt() will affect the value it returns.
            while (current < GetCount())
            {
                if (compare(GetData() + current, GetData() + current - 1) == 0)
                {
                    RemoveAt(current);
                }
                else
                {
                    current++;
                }
            }
        }
    }
};

typedef TRefCountedAbstractModernArray<float> RefCountedModernFloatArray;
