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

//  Synopsis:   Implements tests for GLSL->HLSL conversion of combinatorial scenarios

#include "headers.hxx"
#include "ComplexGLSLTests.hxx"
#include "GLSLIdentifierTable.hxx"
#include "GLSLTranslate.hxx"
#include "WexString.h"
#include "WexTestClass.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace ft_glslparse
{
    const LPCWSTR ComplexGLSLTests::VARIABLE_PREFIX = L"var_";
    const LPCWSTR ComplexGLSLTests::ATTRIBUTE_PREFIX = L"attr_";
    const LPCWSTR ComplexGLSLTests::VARYING_PREFIX = L"vary_";

    //+----------------------------------------------------------------------------
    //
    //  Function:   GenericDeclarationTests
    //
    //  Synopsis:   This tests the conversion
    //              From:    "attribute <type> foo;"
    //              To:      "struct VSInput\n{\n  <type> var_0_0:var_0_0;\n};\nstruct PSInput\n{\n
    //                        float4 position : SV_Position;};\n"
    //
    //  Datasource: VariableTypesDataSource.xml#TypeTable
    //
    //-----------------------------------------------------------------------------
    void ComplexGLSLTests::GenericDeclarationTests()
    {
        //CHK_START;

        //WEX::Common::String strVarTypeGLSL;
        //CHK(TestData::TryGetValue(L"TypeGLSL", /*out*/strVarTypeGLSL));

        //WEX::Common::String strFullGLSLString;
        //strFullGLSLString = strFullGLSLString.Format(L"attribute %s foo;", static_cast<LPCWSTR>(strVarTypeGLSL));

        //WEX::Common::String strVarTypeHLSL;
        //CHK(TestData::TryGetValue(L"TypeHLSL", /*out*/strVarTypeHLSL));

        //WEX::Common::String strFullHLSLString;
        //strFullHLSLString = strFullHLSLString.Format( 
        //                                            L"struct VSInput\n"
        //                                            L"{\n"
        //                                            L"  %s %s0:%s0_sem;\n"
        //                                            L"};\n"
        //                                            L"struct PSInput\n"
        //                                            L"{\n"
        //                                            L"  float4 position : SV_Position;\n"
        //                                            L"};\n",
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX
        //                                            );
        //   
        //ParserTestBase::TestParserInput(GLSLShaderType::Vertex, strFullGLSLString, strFullHLSLString);

        //CHK_END;
        //if (FAILED(hr))
        //{
        //    Log::Error(L"Failed to retrieve parameters.");
        //}
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GenericSingleTypeUniformQualifierTests
    //
    //  Synopsis:   This tests the conversion 
    //              From:    "attribute <type> foo; uniform <type> bar; attribute <type> baz;"
    //              To:      "struct VSInput\n {\n  <type> var_0_0:var_0_0;\n  <type> var_0_2:var_0_2;};\n
    //                        struct PSInput{\n  float4 position : SV_Position;\n};\n<type> var_0_1;"
    //
    //  Datasource: VariableTypesDataSource.xml#TypeTable
    //
    //-----------------------------------------------------------------------------
    void ComplexGLSLTests::GenericSingleTypeUniformQualifierTests()
    {
        //CHK_START;

        //WEX::Common::String strVarTypeGLSL;
        //CHK(TestData::TryGetValue(L"TypeGLSL", /*out*/strVarTypeGLSL));

        //
        //WEX::Common::String strFullGLSLString;
        //strFullGLSLString = strFullGLSLString.Format( 
        //                                            L"attribute %s foo; "
        //                                            L"uniform %s bar; "
        //                                            L"attribute %s baz;",
        //                                            static_cast<LPCWSTR>(strVarTypeGLSL),
        //                                            static_cast<LPCWSTR>(strVarTypeGLSL),
        //                                            static_cast<LPCWSTR>(strVarTypeGLSL)
        //                                            );

        //WEX::Common::String strVarTypeHLSL;
        //CHK(TestData::TryGetValue(L"TypeHLSL", /*out*/strVarTypeHLSL));

        //WEX::Common::String strFullHLSLString;
        //strFullHLSLString = strFullHLSLString.Format( 
        //                                            L"struct VSInput\n"
        //                                            L"{\n"
        //                                            L"  %s %s1:%s1_sem;\n"
        //                                            L"  %s %s0:%s0_sem;\n"
        //                                            L"};\n"
        //                                            L"struct PSInput\n"
        //                                            L"{\n"
        //                                            L"  float4 position : SV_Position;\n"
        //                                            L"};"
        //                                            L"\n%s %s0_1;\n",
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::VARIABLE_PREFIX
        //                                            );

        //ParserTestBase::TestParserInput(GLSLShaderType::Vertex, strFullGLSLString, strFullHLSLString);
        //
        //CHK_END;
        //if (FAILED(hr))
        //{
        //    Log::Error(L"Failed to retrieve parameters.");
        //}
    }

    
    //+----------------------------------------------------------------------------
    //
    //  Function:   GenericSingleTypeBasicFunctionTests
    //
    //  Synopsis:   This tests the conversion 
    //              From:    "attribute <type> foo; void main() { <type> bar; }"
    //              To:      "struct VSInput\n{\n  <type> var_0_0:var_0_0;\n};\n
    //                        struct PSInput\n{\n  float4 position : SV_Position;\n};\n
    //                        void main(in VSInput input, out PSInput psInput)\n{\nint var_1_1;\n}"
    //
    //  Datasource: VariableTypesDataSource.xml#TypeTable
    //
    //-----------------------------------------------------------------------------
    void ComplexGLSLTests::GenericSingleTypeBasicFunctionTests()
    {
        //CHK_START;

        //WEX::Common::String strVarTypeGLSL;
        //CHK(TestData::TryGetValue(L"TypeGLSL", /*out*/strVarTypeGLSL));
      
        //WEX::Common::String strFullGLSLString;
        //strFullGLSLString = strFullGLSLString.Format( 
        //                                            L"attribute %s foo; " 
        //                                            L"void main() "
        //                                            L"{"
        //                                            L" %s bar;"
        //                                            L"}",
        //                                            static_cast<LPCWSTR>(strVarTypeGLSL),
        //                                            static_cast<LPCWSTR>(strVarTypeGLSL)
        //                                            );

        //WEX::Common::String strVarTypeHLSL;
        //CHK(TestData::TryGetValue(L"TypeHLSL", /*out*/strVarTypeHLSL));

        //WEX::Common::String strFullHLSLString;
        //strFullHLSLString = strFullHLSLString.Format( 
        //                                            L"struct VSInput\n"
        //                                            L"{\n"
        //                                            L"  %s %s0:%s0_sem;\n"
        //                                            L"};\n"
        //                                            L"struct PSInput\n"
        //                                            L"{\n"
        //                                            L"  float4 position : SV_Position;\n"
        //                                            L"};\n"
        //                                            L"void main(in VSInput vsInputArg, out PSInput psInputOut)\n"
        //                                            L"{\n"
        //                                            L"%s %s1_1;\n"
        //                                            L"}\n",
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            ComplexGLSLTests::ATTRIBUTE_PREFIX,
        //                                            static_cast<LPCWSTR>(strVarTypeHLSL),
        //                                            ComplexGLSLTests::VARIABLE_PREFIX
        //                                            );
        //                                             
        //ParserTestBase::TestParserInput(GLSLShaderType::Vertex, strFullGLSLString, strFullHLSLString);

        //CHK_END;
        //if (FAILED(hr))
        //{
        //    Log::Error(L"Failed to retrieve parameters.");
        //}
    }
}
