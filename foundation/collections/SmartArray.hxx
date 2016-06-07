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

//  File:       SmartArray.hxx
//  Abstract:   Smart pointer to a simple array (new[]/delete[])

#pragma once

#include <strsafe.h>

#ifdef PERFMETER
#define ENABLE_TSMARTARRAY_MEMORY_TAGGING
#endif

#ifdef ENABLE_TSMARTARRAY_MEMORY_TAGGING
#pragma message("TSmartArray memory tracking enabled.")
#endif

//+-------------------------------------------------------------------------
//
//  Class:      TSmartArray
//
//  Synopsis:   Generic type for smart pointer to an allocated array (new[]/delete[])
//
//--------------------------------------------------------------------------
template <class T>
class TSmartArray
{
    #ifdef ENABLE_TSMARTARRAY_MEMORY_TAGGING
        DECLARE_MEMTRACK_NAME(TSmartArray);
        DECLARE_MEMMETER_NEW;
        static LPCSTR GetMemTrackerOwnerName()
        {
            return "mtTSmartArray"; // need to provide the prefix to match the macros
        }

        typedef T BaseClass;
        #include "templateMemTracking.inl"
    #endif

public:
    typedef T EntryType;

    // Constructor.
    // Mark this as explicit to avoid accidentally creating these objects
    // through implicit conversions.
    explicit TSmartArray(T *pT = NULL) throw()
        : m_pT(pT)
    {
    }

    // Destructor. Deletes the object.
    ~TSmartArray() throw()
    {
        Free();
    }

    // Allocation
    HRESULT_VOID New(size_t size)
    {
        requires(size > 0);

        Free();

        #ifdef ENABLE_TSMARTARRAY_MEMORY_TAGGING
            m_pT = new(GetMemTracker()) T[size];
        #else
            m_pT = new T[size];
        #endif

        return S_OK_VOID;
    }

    // Delete
    HRESULT_VOID Delete()
    {
        Free();
        m_pT = NULL;
        return S_OK_VOID;
    }

    // Casting to T*
    operator T*() const throw()
    {
        return m_pT;
    }

    // Getting an offset into the array
    T* GetOffsetPointer(size_t offset)
    {
        return &m_pT[offset];
    }

    // Dereferencing into T structure
    T* operator ->() const throw()
    {
        return m_pT;
    }

    // TransferFrom transfers ownership between other instances
    // of TSmartArray.
    void TransferFrom(TSmartArray<T>& smartArrayIn)
    {
        // no-op if transferring from ourselves
        if(m_pT != smartArrayIn.m_pT)
        {
            // Delete our current memory if we have any.
            Free();

            // Assign from smartArrayIn and clear out its memory
            m_pT = smartArrayIn.m_pT;
            smartArrayIn.m_pT = NULL;
        }
    }

    // Take out the object from a TSmartMemory without deleting it
    T* Extract() throw()
    {
        T* pT = m_pT;
        m_pT = NULL;
        return pT;
    }

    // Take ownership of a pointer
    // not safe if not allocated with new[]
    HRESULT_VOID TakeOwnership(__in_opt T* pT)
    {
        Free();
        m_pT = pT;

        return S_OK_VOID;
    }

/* NOTE: we don't seem to need these, compiler figures it out

    T &operator*() const        { return *m_pT; }
    T &operator[](int i) const  { return m_pT[i]; }
    BOOL operator!() const      { return (m_pT == NULL); }
*/

private:
    // disallow all assignments
    T* operator=(T* pT);
    T* operator=(const T&);
    TSmartArray<T>& operator=(const TSmartArray<T>&);
    TSmartArray(const TSmartArray<T>&);

    // disallow a pointer to the pointer
    T** operator &() throw();

    // free contained memory without nulling the pointer - safe only when we are about to overwrite the pointer
    void Free() throw()
    {
        delete [] (m_pT);
    }

protected:
    T* m_pT;
};

template<typename T>
class TSmartArrayWithCount : public TSmartArray<T>
{
public:
    // Replaced methods in TSmartArray
    HRESULT_VOID New(size_t size)
    {
        Assert(size > 0);
        TSmartArray<T>::New(size);

        _c = size;

        return S_OK_VOID;
    }

    HRESULT_VOID Delete()
    {
        _c = 0;
        TSmartArray<T>::Delete();
        return S_OK_VOID;
    }

    // Functionally explicit indexers (with range-checking asserts)
    T & Item(size_t i)
    {
        Assert(i < _c);
        return *(operator ->() + i);
    }
    const T & Item(size_t i) const
    {
        Assert(i < _c);
        return *(operator ->() + i);
    }

    size_t GetCount() const { return _c; }

    TSmartArrayWithCount() : _c(0) { }

    WORD ComputeCrc() const
    {
        const BYTE* pb = reinterpret_cast<const BYTE*>(m_pT);
        const size_t cb = GetCount() * sizeof(T);
        DWORD dwCrc = ns_sse::fnWeakHash(pb, cb);
        return LOWORD(dwCrc) ^ HIWORD(dwCrc);
    }

    BOOL Compare(__in const TSmartArrayWithCount<T>* pOther) const
    {
        BOOL fResult = (GetCount() == pOther->GetCount());
        if (fResult)
        {
            const size_t cb = GetCount() * sizeof(T);
            fResult = (memcmp(m_pT, pOther->m_pT, cb) == 0) ? TRUE : FALSE;
        }

        return fResult;
    }

    HRESULT_VOID ShallowCopyTo(__out TSmartArrayWithCount<T>& other) const
    {
        Assert(GetCount() > 0);
        other.New(GetCount());
        const size_t cb = GetCount() * sizeof(T);
        memcpy_s(other.m_pT, cb, m_pT, cb);

        return S_OK_VOID;
    }

protected:
    size_t _c;

private:
    TSmartArrayWithCount(const TSmartArrayWithCount&);
    TSmartArrayWithCount& operator=(const TSmartArrayWithCount&);
};

#ifdef ENABLE_TSMARTARRAY_MEMORY_TAGGING
template<class T>
PERFMETERTAG TSmartArray<T>::s_mtThisType = 0;
#endif
