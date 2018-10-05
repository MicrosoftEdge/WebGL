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

//  File:       SmartMemory.hxx
//  Abstract:   Smart pointer for allocated memory (new/delete)

#pragma once


//+-------------------------------------------------------------------------
//
//  Class:      TSmartMemory
//
//  Synopsis:   Generic type for smart pointer to memory (new/delete)
//
//--------------------------------------------------------------------------

template <class T> class TSmartMemory
{
public:
    // Constructor.
    TSmartMemory(T *pT = NULL) throw() 
        : m_pT(pT)
    {
    }

    // Destructor. Deletes the object.
    ~TSmartMemory() throw() 
    { 
        delete m_pT;
    }

    // Casting to T*
    operator T*() const throw() 
    { 
        return m_pT; 
    }

    // Dereferencing into T structure
    T* operator ->() const throw() 
    { 
        return m_pT; 
    }

    // Assignment from non-smart pointer
    T* operator=(T* pT) throw() 
    { 
        delete m_pT;
        return m_pT = pT; 
    }

    // Getting a pointer to the pointer
    //
    // Need this to pass as an out parameter
    T** operator &() throw() 
    {
        Assert(!m_pT); // initialized pointer as an [out] parameter is not allowed
        return &m_pT; 
    }

    T*& GetPointerReference() throw()
    {
        return m_pT;
    }

    // Take out the object from a TSmartMemory without deleting it
    T* Extract() throw() 
    { 
        T* pT = m_pT; 
        m_pT = NULL; 
        return pT; 
    }

    // take an unchecked address of the pointer
    T **UnsafeAddress()         
    { 
        return &m_pT; 
    }

    // Useful for helping templates resolve their types correctly.
    T* GetRawPointer()         
    { 
        return m_pT; 
    }

/* NOTE: we don't seem to need these, compiler figures it out

	T &operator*() const		{ return *m_pT; }
	T &operator[](int i) const	{ return m_pT[i]; }
    BOOL operator!() const      { return (m_pT == NULL); }
*/

    T *operator=(__inout TSmartMemory<T>& spT)
    {
        if (*this == spT)
        {
            if (this != &spT)
            {
                // If this assert fires, it means you attempted to assign one TSmartMemory to another when they both contained 
                // a pointer to the same underlying object. This means a bug in your code, since your object will get 
                // double-deleted. 
                AssertSz(false, "Attempted to assign one TSmartMemory to another when both contained pointers to the same object")

                // For safety, we are going to detach the other TSmartMemory to avoid a double-free. Your code still
                // has a bug, though.
                spT.Extract();
            }
            else
            {
                // Alternatively, this branch means that you are assigning a TSmartMemory to itself, which is
                // pointless but permissible

                // nothing to do
            }
        }
        else
        {
            delete m_pT;

            // Transfer ownership
            m_pT = spT.Extract();
        }
        return m_pT;
    }

private:

    // Make the copy constructor private to force explicit ownership changes. 
    TSmartMemory(__inout TSmartMemory<T>& spT);

    T* m_pT;
};

