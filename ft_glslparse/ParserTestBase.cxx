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

#include "headers.hxx"
#include "ParserTestBase.hxx"
#include "GLSLConvertedShader.hxx"
#include "GLSLTranslate.hxx"
#include "WexString.h"
#include "WexTestClass.h"
#include "GLSLTranslateOptions.hxx"
#include "WebGLFeatureLevel.hxx"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace ft_glslparse
{
    //+----------------------------------------------------------------------------
    //
    //  Function:   TestHLSLProgramConversionFromXML
    //
    //  Synopsis:   This function pulls a complete GLSL shader program 
    //              and expected HLSL conversion from TAEF XML,
    //              replaces values defined by "%s" in both the GLSL and HLSL parts,
    //              and compares actual to expected via TestParserInput
    //
    //-----------------------------------------------------------------------------
    HRESULT ParserTestBase::TestHLSLProgramConversionFromXML(   
        __in_opt const LPWSTR pszReplacedGLSLValueName, 
        __in_opt const LPWSTR pszReplacedHLSLValueName
        )
    {
        CHK_START;
              
        // Test fragment shader
        WEX::Common::String strFragmentShaderGLSL;
        CHK(TestData::TryGetValue(L"FragmentGLSL", /*out*/strFragmentShaderGLSL));
        
        // Test vertex shader
        WEX::Common::String strVertexShaderGLSL;
        CHK(TestData::TryGetValue(L"VertexGLSL", /*out*/strVertexShaderGLSL));

        if (pszReplacedGLSLValueName != nullptr)
        {
            // Get variables to replace. These are defined by "%s" tokens in the source XML.
            WEX::Common::String strReplacedGLSLValue;
            CHK(TestData::TryGetValue(pszReplacedGLSLValueName, /*out*/strReplacedGLSLValue));

            strFragmentShaderGLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedGLSLValue));
            strVertexShaderGLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedGLSLValue));
        }

        WEX::Common::String strFragmentShaderHLSL;
        CHK(TestData::TryGetValue(L"FragmentHLSL", /*out*/strFragmentShaderHLSL));

        WEX::Common::String strVertexShaderHLSL;
        CHK(TestData::TryGetValue(L"VertexHLSL", /*out*/strVertexShaderHLSL));

        if (pszReplacedHLSLValueName != nullptr)
        {
            WEX::Common::String strReplacedHLSLValue;
            CHK(TestData::TryGetValue(pszReplacedHLSLValueName, /*out*/strReplacedHLSLValue));

            strFragmentShaderHLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedHLSLValue));
            strVertexShaderHLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedHLSLValue));
        }

        TestParserInput(GLSLShaderType::Fragment, strFragmentShaderGLSL, strFragmentShaderHLSL);
        
        TestParserInput(GLSLShaderType::Vertex, strVertexShaderGLSL, strVertexShaderHLSL);

        CHK_END;

        if (FAILED(hr))
        {
            Log::Error(L"Failed to retrieve parameters.");
        }

        return hr;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   TestHLSLProgramConversionFromXML (overload)
    //
    //  Synopsis:   This function pulls a complete GLSL shader program 
    //              and expected HLSL conversion from TAEF XML,
    //              replaces values defined by "spTokens" array 
    //              in both the GLSL and HLSL parts defined by "spGLSLValueNames"
    //              and "spHLSLValueNames" arrays
    //              and compares actual to expected via TestParserInput
    //
    //  Notes:      spGLSLValueNames, spHLSLValueNames, and spTokens are arrays and
    //              "count" is the length of the arrays (must be equal length)
    //-----------------------------------------------------------------------------
    HRESULT ParserTestBase::TestHLSLProgramConversionFromXML(    
        __in_ecount(count) const LPWSTR* ppszGLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszHLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszTokens,
        __in size_t count
    )
    {
        CHK_START;

        // Test fragment shader
        CHK(TestShaderConversion(
            L"FragmentGLSL", 
            L"FragmentHLSL", 
            GLSLShaderType::Fragment,
            ppszGLSLValueNames,
            ppszHLSLValueNames,
            ppszTokens,
            count
            ));
         
        // Test vertex shader
        CHK(TestShaderConversion(
            L"VertexGLSL", 
            L"VertexHLSL", 
            GLSLShaderType::Vertex,
            ppszGLSLValueNames,
            ppszHLSLValueNames,
            ppszTokens,
            count
            ));

        CHK_END;

        if (FAILED(hr))
        {
            Log::Error(L"Failed to translate GLSL program.");
        }

        return hr;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   TestShaderConversion 
    //
    //  Synopsis:   This function takes GLSL/HLSL shader names, a shader type,
    //              and three arrays - one for GLSL names to replace, one for
    //              HLSL names to replace, and one for actual tokens to replace.
    //              In addition, it takes a count. It iterates over the GLSL/HLSL/token
    //              arrays to allow for multiple token substitution for a given shader.
    //
    //  Notes:      spGLSLValueNames, spHLSLValueNames, and spTokens are arrays and
    //              "count" is the length of the arrays (must be equal length)
    //-----------------------------------------------------------------------------
    HRESULT ParserTestBase::TestShaderConversion(
        __in const LPWSTR strShaderGLSLName, 
        __in const LPWSTR strShaderHLSLName, 
        __in GLSLShaderType::Enum shaderType,
        __in_ecount(count) const LPWSTR* ppszGLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszHLSLValueNames,
        __in_ecount(count) const LPWSTR* ppszTokens,
        __in size_t count
        )
    {
        CHK_START;

        WEX::Common::String strVertexShaderGLSL;
        CHK(TestData::TryGetValue(strShaderGLSLName, /*out*/strVertexShaderGLSL));
        WEX::Common::String strVertexShaderHLSL;
        CHK(TestData::TryGetValue(strShaderHLSLName, /*out*/strVertexShaderHLSL));

        for(size_t i = 0; i < count; i++)
        {
            const LPWSTR szCurrentGLSLValueName = ppszGLSLValueNames[i];
            CHK_POINTER_ALLOC(szCurrentGLSLValueName);
            const LPWSTR szCurrentHLSLValueName = ppszHLSLValueNames[i];
            CHK_POINTER_ALLOC(szCurrentHLSLValueName);
            const LPWSTR szToken = ppszTokens[i];
            CHK_POINTER_ALLOC(szToken);

            WEX::Common::String strReplacedGLSLValue;
            CHK(TestData::TryGetValue(szCurrentGLSLValueName, /*out*/strReplacedGLSLValue));
            
            WEX::Common::String strReplacedHLSLValue;
            CHK(TestData::TryGetValue(szCurrentHLSLValueName, /*out*/strReplacedHLSLValue));
            
            strVertexShaderGLSL.Replace(szToken, static_cast<LPCWSTR>(strReplacedGLSLValue));
            strVertexShaderHLSL.Replace(szToken, static_cast<LPCWSTR>(strReplacedHLSLValue));
        }

        TestParserInput(shaderType, strVertexShaderGLSL, strVertexShaderHLSL);
        
        CHK_END;
        
        if (FAILED(hr))
        {
            Log::Error(L"Failed to convert shader.");
        }

        return hr;
    }


    //+----------------------------------------------------------------------------
    //
    //  Function:   TestInvalidGLSLParseFromXML
    //
    //  Synopsis:   This function pulls a GLSL shader program 
    //              from TAEF XML, replaces values defined by "%s",
    //              and verifies that GLSLConvert fails via TestParserInputNegative.
    //              Unlike TestHLSLProgramConversionFromXML, the vertex shader
    //              and fragment shader are optional
    //
    //-----------------------------------------------------------------------------
    HRESULT ParserTestBase::TestInvalidGLSLParseFromXML(    
        __in_opt const LPWSTR pszReplacedGLSLValueName,
        __in const LPWSTR pszReplacedGLSLFragmentShaderName,
        __in const LPWSTR pszReplacedGLSLVertexShaderName
        )
    {
        CHK_START;
              
        // Get variables to replace. These are defined by "%s" tokens in the source XML.
        WEX::Common::String strReplacedGLSLValue;
        if (pszReplacedGLSLValueName != nullptr)
        {
            hr = TestData::TryGetValue(pszReplacedGLSLValueName, /*out*/strReplacedGLSLValue);
        }
        else
        {
            strReplacedGLSLValue = L"";
        }

        // If replaced value is not found, default to empty string
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            strReplacedGLSLValue = L"";
            hr = S_OK;
        }
        CHK(hr);

        // Test optional fragment shader
        WEX::Common::String strFragmentShaderGLSL;
        hr = TestData::TryGetValue(pszReplacedGLSLFragmentShaderName, /*out*/strFragmentShaderGLSL);
        
        if (SUCCEEDED(hr))
        {
            strFragmentShaderGLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedGLSLValue));
            TestParserInputNegative(GLSLShaderType::Fragment, strFragmentShaderGLSL);
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            hr = S_OK;        
        }
        CHK(hr);

        // Test optional vertex shader
        WEX::Common::String strVertexShaderGLSL;
        hr = TestData::TryGetValue(pszReplacedGLSLVertexShaderName, /*out*/strVertexShaderGLSL);
        
        if (SUCCEEDED(hr))
        {
            strVertexShaderGLSL.Replace(L"%s", static_cast<LPCWSTR>(strReplacedGLSLValue));
            TestParserInputNegative(GLSLShaderType::Vertex, strVertexShaderGLSL);
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            hr = S_OK;        
        }
        CHK(hr);
        
        CHK_RETURN;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   NormalizeWhitespace
    //
    //  Synopsis:   Removes whitespace from a given string while maintaining valid HLSL
    //-----------------------------------------------------------------------------
    void ParserTestBase::NormalizeWhitespace(__in WEX::Common::String& strInput)
    {
        // Strip leading and trailing whitespace
        strInput.Trim();

        // Replace any multiple spaces with a single space. Note that WEX::Common::String::Replace is recursive.
        strInput.Replace(L"  ", L" ");

        // Remove any leading spaces (no indentation)
        const int MAX_INDENTATION_LEVELS = 10;
        for(int i = 0; i < MAX_INDENTATION_LEVELS; i++)
        {
            strInput.Replace(L"\n ", L"\n");
        }

        // Remove spaces before and after special characters
        for (int i = 0; i < ARRAYSIZE(OPTIONAL_WHITESPACE_CHARACTERS); i++)
        {
            WEX::Common::String strCurrentOptionalWhitespaceChar;
            strCurrentOptionalWhitespaceChar = OPTIONAL_WHITESPACE_CHARACTERS[i];

            // Replace space after special char
            strInput.Replace(strCurrentOptionalWhitespaceChar + L" ", strCurrentOptionalWhitespaceChar);

            // Replace space before special char
            strInput.Replace(L" " + strCurrentOptionalWhitespaceChar, strCurrentOptionalWhitespaceChar);
        }

        // Remove any empty lines
        strInput.Replace(L"\n\n", L"\n");
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   TestParserInput
    //
    //  Synopsis:   Compares an expected HLSL shader input to a GLSL shader after
    //              conversion via GLSLTranslate
    //-----------------------------------------------------------------------------
    void ParserTestBase::TestParserInput(   
        __in GLSLShaderType::Enum shaderType, 
        __in const WEX::Common::String& strGLSLInput, 
        __in const WEX::Common::String& strExpectedHLSL
        )
    {
        // GLSLTranslate requires BSTR input
        CSmartBstr bstrGLSLInput;
        bstrGLSLInput.Set(strGLSLInput);

        // Convert GLSL input to HLSL via GLSLTranslate
        TSmartPointer<CGLSLConvertedShader> spShader;
        VERIFY_SUCCEEDED(
            ::GLSLTranslate(
                bstrGLSLInput,
                shaderType,
                GLSLTranslateOptions::DisableBoilerPlate,
                WebGLFeatureLevel::Level_9_1,
                /*out*/&spShader
                )
            );

        CMutableString<char> spConvertedHLSL;
        VERIFY_SUCCEEDED(spShader->GetConvertedCodeWithParsedStructInfo(/*out*/spConvertedHLSL));

        // Trim spaces from both strings
        WEX::Common::String strExpectedHLSLTrimmed = strExpectedHLSL;
        NormalizeWhitespace(strExpectedHLSLTrimmed);

        WEX::Common::String strActualHLSLTrimmed = spConvertedHLSL;
        NormalizeWhitespace(strActualHLSLTrimmed);

        bool verboseOutput = false;
        if (SUCCEEDED(RuntimeParameters::TryGetValue(L"verbose", verboseOutput)) && verboseOutput)
        {
            Log::Comment(L"GLSL input:\r\n" + strGLSLInput);
            Log::Comment(L"Expected HLSL output:\r\n" + strExpectedHLSLTrimmed);
            Log::Comment(L"Actual HLSL output:\r\n" + strActualHLSLTrimmed);
        }

        // Compare trimmed strings
        VERIFY_ARE_EQUAL(strExpectedHLSLTrimmed, strActualHLSLTrimmed);
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   TestParserInputNegative
    //
    //  Synopsis:   Verifies the GLSLTranslate fails when provided invalid GLSL
    //                  
    //-----------------------------------------------------------------------------
    void ParserTestBase::TestParserInputNegative(   
        __in GLSLShaderType::Enum shaderType, 
        __in const WEX::Common::String& strGLSLInput
        )
    {
        // GLSLTranslate requires BSTR input
        CSmartBstr bstrGLSLInput;
        bstrGLSLInput.Set(strGLSLInput);

        bool verboseOutput = false;
        if (SUCCEEDED(RuntimeParameters::TryGetValue(L"verbose", verboseOutput)) && verboseOutput)
        {
            Log::Comment(L"Negative GLSL input:\r\n" + strGLSLInput);
        }
        
        TSmartPointer<CGLSLConvertedShader> spShader;
        VERIFY_SUCCEEDED(
            ::GLSLTranslate(
                bstrGLSLInput,
                shaderType,
                GLSLTranslateOptions::DisableBoilerPlate,
                WebGLFeatureLevel::Level_9_1,
                /*out*/&spShader
                )
            );

        CMutableString<char> spConvertedHLSL;
        VERIFY_FAILED(spShader->GetConvertedCodeWithParsedStructInfo(/*out*/spConvertedHLSL));
    }
}
