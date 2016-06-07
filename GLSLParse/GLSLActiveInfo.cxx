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

#include "PreComp.hxx"
#include "GLSLActiveInfo.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   InitWithVariableName
//
//  Synopsis:   Takes a variable name and other CGLSLActiveInfo (both with 
//              char names), combines the name and suffix, converts them to
//              a WCHAR string, and intializes the type and size.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLActiveInfo<WCHAR>::InitWithVariableName(__in PCSTR pszVariableName, const CGLSLActiveInfo<char>& other)
{
    CHK_START;

    PCSTR pszFullGLSLName = pszVariableName;

    // There will be a suffix in the GLSL active info if the variable
    // was an array or struct type. Append this suffix to get the full uniform name.
    CMutableString<char> spszNameWithSuffix;
    size_t cchNameSuffix = other.UseName().GetLength();
    if (cchNameSuffix > 0)
    {
        const char* pszNameSuffix = other.GetNameString();
        size_t cchFormatSize = strlen(pszVariableName) + cchNameSuffix + 1;
        CHK(spszNameWithSuffix.Format(cchFormatSize, "%s%s", pszVariableName, pszNameSuffix));

        pszFullGLSLName = spszNameWithSuffix;
    }

    // Convert the UTF8 name used by the GLSL parser back to wide characters
    int cchWideNameIncludingNull = ::MultiByteToWideChar(CP_UTF8, 0, pszFullGLSLName, -1, nullptr, 0);

    CHKB(cchWideNameIncludingNull > 0);

    _spszName.SetInitialSize(cchWideNameIncludingNull);

    int cchWideName = ::MultiByteToWideChar(CP_UTF8, 0, pszFullGLSLName, -1, static_cast<WCHAR*>(_spszName), cchWideNameIncludingNull);
    CHKB(cchWideName > 0);

    _glType = other.GetType();
    _uArraySize = other.GetArraySize();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitWithVariableName
//
//  Synopsis:   Not implemented. The function should be used for conversion
//              from char to wchar.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLActiveInfo<char>::InitWithVariableName(__in PCSTR pszVariableName, const CGLSLActiveInfo<char>& other)
{
    // Currently we don't initialize to char from a variable name (used with WCHAR for easy consumption
    // for script APIs).
    Assert(false);
    return E_NOTIMPL;
}
