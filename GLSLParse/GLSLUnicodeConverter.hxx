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

class CMemoryStream;

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLUnicodeConverter
//
//  Synopsis:   Class to encapsulate the work of converting unicode text to
//              ASCII text consumable by the GLSL parser.
//
//------------------------------------------------------------------------------
class CGLSLUnicodeConverter
{
public:
    static HRESULT ConvertToAscii(
        __in BSTR bstrInput,                                        // Input BSTR to convert to Ascii
        __deref_out CMemoryStream** ppConverted                     // Output ASCII stream
        );

    static bool IsLegalChar(wchar_t c);
    static bool IsWhitespaceChar(char c);

private:
    static const bool s_rgLegalChar[128];                           // Legal character map
    static const bool s_rgWhitespaceChar[33];                       // Whitespace character map
};
