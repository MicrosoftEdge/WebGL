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

#include <strsafe.h>
#include <foundation/runtime.hxx>

// Traits class that redirects narrow or wide string functions so that the string template class can
// do its work. It has no methods because one of the specializations should always be used.
template<typename T>
class CharTraits
{
};

// Specialization of traits for char. Calls 'A' variant functions.
template<>
class CharTraits<char>
{
public:
    static void StringCchLength(_In_ PCSTR psz, size_t cchMax, _Out_ size_t* pcch)
    {
        Abandonment::CheckHRESULT(::StringCchLengthA(psz, cchMax, pcch));
    }

    static HRESULT StringCchCopy(_Out_z_cap_(cchDest) PSTR pszDest, size_t cchDest, _In_ PCSTR pszSrc)
    {
        return ::StringCchCopyA(pszDest, cchDest, pszSrc);
    }

    static HRESULT StringCchCopyN(_Out_z_cap_(cchDest) PSTR pszDest, size_t cchDest, _In_ PCSTR pszSrc, size_t cchSrc)
    {
        return ::StringCchCopyNA(pszDest, cchDest, pszSrc, cchSrc);
    }

    static HRESULT StringCchVPrintf(_Out_z_cap_(cchDest) PSTR pszDest, size_t cchDest, _In_z_ PCSTR pszFormat, va_list argList)
    {
        return ::StringCchVPrintfA(pszDest, cchDest, pszFormat, argList);
    }

    static int32_t Compare(_In_z_ PCSTR pszFirst, _In_z_ PCSTR pszSecond)
    {
        return ::strcmp(pszFirst, pszSecond);
    }
};

// Specialization of traits for wchar_t. Calls 'W' variant functions.
template<>
class CharTraits<wchar_t>
{
public:
    static void StringCchLength(_In_ PCWSTR psz, size_t cchMax, _Out_ size_t* pcch)
    {
        Abandonment::CheckHRESULT(::StringCchLengthW(psz, cchMax, pcch));
    }

    static HRESULT StringCchCopy(_Out_z_cap_(cchDest) PWSTR pszDest, size_t cchDest, _In_ PCWSTR pszSrc)
    {
        return ::StringCchCopyW(pszDest, cchDest, pszSrc);
    }

    static HRESULT StringCchCopyN(_Out_z_cap_(cchDest) PWSTR pszDest, size_t cchDest, _In_ PCWSTR pszSrc, size_t cchSrc)
    {
        return ::StringCchCopyNW(pszDest, cchDest, pszSrc, cchSrc);
    }

    static HRESULT StringCchVPrintf(_Out_z_cap_(cchDest) PWSTR pszDest, size_t cchDest, _In_z_ PCWSTR pszFormat, va_list argList)
    {
        return ::StringCchVPrintfW(pszDest, cchDest, pszFormat, argList);
    }

    static int32_t Compare(_In_z_ PCWSTR pszFirst, _In_z_ PCWSTR pszSecond)
    {
        return ::wcscmp(pszFirst, pszSecond);
    }

    static PWSTR EmptyString()
    {
        return L"";
    }
};
