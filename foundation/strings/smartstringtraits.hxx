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

// Traits class for TSmartString - used when making a CModernArray of TSmartString.
template <class T>
class CSmartStringTraits final
{
public:
    static bool IsEqual(const T& t1, const T& t2)
    {
        return t1 == t2;
    }

    template<class X>
    static void Construct(_Out_ T* pDest,   // Pointer to destination element
                          const X& xSrc)    // Source to assign
    {
        new(pDest) T;

        // Do the same thing as assignment
        Assign(pDest, xSrc);
    }

    template<class X>
    static void Assign(_In_ T* pDest,   // Pointer to destination element
                       const X& xSrc)   // Source to assign
    {
        pDest->Set(xSrc);
    }
};
