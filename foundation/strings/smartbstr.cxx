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

#include "SmartBstr.hxx"

CSmartBstr::CSmartBstr(BSTR bstr)
    : m_bstr(bstr)
{
}

// Deletes the object.
CSmartBstr::~CSmartBstr()
{
    Free();
}

// Cast to BSTR.
CSmartBstr::operator BSTR() const
{
    return m_bstr;
}

// Assignment from BSTR.
BSTR& CSmartBstr::operator=(const BSTR& bstr)
{
    Free();

    return m_bstr = bstr;
}

// Set to string.
void CSmartBstr::Set(_In_z_ PCWSTR wzValue)
{
    requires(wzValue != nullptr);

    Free();
    m_bstr = ::SysAllocString_FF(wzValue);
}

// Free the string.
void CSmartBstr::Free()
{
    // nullptr is OK for this function
    ::SysFreeString(m_bstr);
    m_bstr = nullptr;
}

// Realloc string.
void CSmartBstr::ReAlloc(_In_z_ PCWSTR wzValue)
{
    requires(wzValue != nullptr);
    ::SysReAllocString_FF(&m_bstr, wzValue);
}

// String length, same as CComBSTR::Length().
uint32_t CSmartBstr::Cch() const
{
    return ::SysStringLen(m_bstr);
}

// String size in bytes, same as CComBSTR::ByteLength().
uint32_t CSmartBstr::Cb() const
{
    return ::SysStringByteLen(m_bstr);
}

// Raw pointer to the internal BSTR.
//
// Note: The BSTR being pointed to by the return pointer remains stored within the CSmartBstr, and
// should not be freed/deleted/stored by any callers.
//
// If stored, it may be freed when the CSmartBstr is deleted or modified, resulting in an invalid
// pointer. If manually freed by the caller, the CSmartBstr's later attempt to free will be using
// an invalid pointer.
BSTR* CSmartBstr::operator &()
{
    return &m_bstr;
}

// Take out the BSTR from a TSmartBstr without deleting it.
BSTR CSmartBstr::Extract()
{
    BSTR bstr = m_bstr;
    m_bstr = nullptr;
    return bstr;
}

// Allocate and return a copy of the contained string.
BSTR CSmartBstr::Copy() const
{
    // Based on CComBSTR implementation
    if (m_bstr == nullptr)
    {
        return nullptr;
    }
    return ::SysAllocStringByteLen_FF(reinterpret_cast<char*>(m_bstr), ::SysStringByteLen(m_bstr));
}

// This function is meant for use in this file only, hence an anonymous namespace
namespace
{
    // Copy from the ANSI string to the BSTR with simple widening.
    void AnsiToWideChar(_In_count_(cchLen + 1) PCSTR szSource, _Out_cap_(cchLen + 1) BSTR bstrDest, size_t cchLen)
    {
        for (size_t i = 0; i < cchLen + 1; i++)
        {
            bstrDest[i] = szSource[i];
        }
    }
}

// Set from an ANSI string with simple widening.
void CSmartBstr::SetAnsi(_In_z_ PCSTR szValue)
{
    requires(szValue != nullptr);

    Free();

    size_t len = ::strlen(szValue);

    m_bstr = ::SysAllocStringLen_FF(nullptr, CheckedMath::SizeT_To_UInt32(len));

    AnsiToWideChar(szValue, m_bstr, len);
}

// Set from an ANSI string of the given length (not including null terminator) with simple widening
// and an additional null character.
void CSmartBstr::SetAnsi(_In_count_(cchValue) const char* pchValue, size_t cchValue)
{
    requires(pchValue != nullptr);

    Free();

    // Allocate +1 for null
    m_bstr = ::SysAllocStringLen_FF(nullptr, CheckedMath::SizeT_To_UInt32(cchValue + 1));
    m_bstr[cchValue] = L'\0';

    // Copy -1 since null is already accounted for
    AnsiToWideChar(pchValue, m_bstr, cchValue - 1);
}

// Set from a BSTR that may have embedded nulls. This method will successfully create a new BSTR
// when bstrValue == nullptr;
void CSmartBstr::SetBstr(_In_opt_ BSTR bstrValue)
{
    Free();

    // Get the length of the BSTR including embedded zeros.
    uint32_t len = ::SysStringLen(bstrValue);

    // Allocate and copy these characters.
    m_bstr = ::SysAllocStringLen_FF(bstrValue, len);
}

// Make an ANSI string from the BSTR
HRESULT CSmartBstr::ConvertToUTF8(_Inout_ TSmartArray<char>& spConverted,   // Smart array object to accept the converted string
                                  _Out_opt_ size_t* outputLength) const     // Optional place to receive length of converted string
{
    if (m_bstr == nullptr)
    {
        return E_POINTER;
    }

    return ConvertToUTF8(m_bstr, spConverted, outputLength);
}

HRESULT CSmartBstr::ConvertToUTF8(_In_z_ PCWSTR pszwInput,                  // String to convert
                                  _Inout_ TSmartArray<char>& spConverted,   // Smart array object to accept the converted string
                                  _Out_opt_ size_t* outputLength)           // Optional place to receive length of converted string
{
    // Figure out how much room we need - we pass -1 for length
    // so that the terminator is included.
    int32_t numBytes = ::WideCharToMultiByte(CP_UTF8   /*CodePage*/,
                                             0         /*dwFlags*/,
                                             pszwInput /*lpWideCharStr*/,
                                             -1        /*cchWideChar*/,
                                             nullptr   /*lpMultiByteStr*/,
                                             0         /*cbMultiByte*/,
                                             nullptr   /*lpDefaultChar*/,
                                             nullptr   /*lpUsedDefaultChar*/);

    if (numBytes == 0)
    {
        return E_UNEXPECTED;
    }

    size_t numBytesUnsigned = CheckedMath::Int32_To_UInt32(numBytes);

    spConverted.New(numBytesUnsigned);

    if (outputLength != nullptr)
    {
        // We want to return the string size, not the buffer size
        *outputLength = numBytesUnsigned - 1;
    }

    numBytes = ::WideCharToMultiByte(CP_UTF8     /*CodePage*/,
                                     0           /*dwFlags*/,
                                     pszwInput   /*lpWideCharStr*/,
                                     -1          /*cchWideChar*/,
                                     spConverted /*lpMultiByteStr*/,
                                     numBytes    /*cbMultiByte*/,
                                     nullptr     /*lpDefaultChar*/,
                                     nullptr     /*lpUsedDefaultChar*/);

    if (numBytes == 0)
    {
        Abandonment::Fail("WideCharToMultiByte() should not fail to write its output after it successfully read");
    }

    return S_OK;
}
