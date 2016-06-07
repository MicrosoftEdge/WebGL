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

#include <foundation/collections.hxx>
#include <foundation/runtime.hxx>

// This type wraps a BSTR, and calls SysFreeString when it gets destructed.
class CSmartBstr final
{
public:
    // Constructor
    CSmartBstr(BSTR bstr = nullptr) throw();

    // Destructor. Deletes the object.
    ~CSmartBstr() throw();

    NO_COPY(CSmartBstr);

    // Cast to BSTR
    operator BSTR() const throw();

    // Assignment from BSTR
    BSTR& operator=(const BSTR& bstr) throw();

    // Set to string
    void Set(_In_z_ PCWSTR wzValue);

    void SetAnsi(_In_z_ PCSTR szValue);
    void SetAnsi(_In_count_(cchValue) const char* pchValue, size_t cchValue);

    // Set from a BSTR that may have embedded nulls
    void SetBstr(_In_opt_ BSTR bstrValue);

    // Free the string
    void Free();

    // Realloc string
    void ReAlloc(_In_z_ PCWSTR wzValue);

    // String length, same as CComBSTR::Length()
    uint32_t Cch() const;

    //String size in bytes, same as CComBSTR::ByteLength()
    uint32_t Cb() const;

    // Raw pointer to the internal BSTR. This can be dangerous; see remarks on the implementation.
    BSTR* operator &() throw();

    // Take out the BSTR from a TSmartBstr without deleting it
    BSTR Extract() throw();

    // Allocate and return a copy of the contained string
    BSTR Copy() const;

    // Make an ANSI string from the BSTR. Returns E_UNEXPECTED if invalid Unicode was found in the BSTR.
    HRESULT ConvertToUTF8(_Out_ TSmartArray<char>& spConverted,     // Smart array object to accept the converted string
                          _Out_opt_ size_t* outputLength) const;    // Optional place to receive length of converted string

    static HRESULT ConvertToUTF8(_In_z_ PCWSTR pszwInput,               // String to convert
                                 _Out_ TSmartArray<char>& spConverted,  // Smart array object to accept the converted string
                                 _Out_opt_ size_t* outputLength);       // Optional place to receive length of converted string

private:
    BSTR m_bstr;
};
