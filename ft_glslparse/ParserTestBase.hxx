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

//  Class:      ParserTestBase
//  Synopsis:   Declares basic helper functions for testing GLSL->HLSL
//              conversion.

#undef Verify
#include "WexTestClass.h"
#include "GLSLShaderType.hxx"

namespace ft_glslparse
{
    class ParserTestBase
    {
    public:
        static HRESULT TestHLSLProgramConversionFromXML(    
            __in_opt const LPWSTR pszReplacedGLSLValueName, 
            __in_opt const LPWSTR pszReplacedHLSLValueName
            );

        static HRESULT TestInvalidGLSLParseFromXML( 
            __in_opt const LPWSTR pszReplacedGLSLValueName, 
            __in const LPWSTR pszReplacedGLSLFragmentShaderName, 
            __in const LPWSTR pszReplacedGLSLVertexShaderName
            );

        static HRESULT ParserTestBase::TestHLSLProgramConversionFromXML(    
            __in_ecount(count) const LPWSTR* ppszGLSLValueNames,
            __in_ecount(count) const LPWSTR* ppszHLSLValueNames,
            __in_ecount(count) const LPWSTR* ppszTokens,
            __in_ecount(count) size_t count
            );

        static void TestParserInput(    
            __in const GLSLShaderType::Enum shaderType, 
            __in const WEX::Common::String& strInput,
            __in const WEX::Common::String& strExpectedHLSL
            );

        static void TestParserInputNegative(    
            __in const GLSLShaderType::Enum shaderType, 
            __in const WEX::Common::String& strInput
            );

        static HRESULT ParserTestBase::TestShaderConversion(
        __in const LPWSTR pszShaderGLSLName, 
        __in const LPWSTR pszShaderHLSLName, 
        __in GLSLShaderType::Enum shaderType,
        __in_ecount(count) const LPWSTR* ppszGLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszHLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszTokens,
        __in size_t count
        );

        static void NormalizeWhitespace(__in WEX::Common::String& strInput);
    };
    static const wchar_t OPTIONAL_WHITESPACE_CHARACTERS[]  = {L',', L';', L'=', L'{', L'}', L'(', L')', L'+', L'-', L'*', L'/'};
}
