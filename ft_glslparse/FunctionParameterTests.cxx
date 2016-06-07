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

//  Class:      FunctionParameterTests
//  Synopsis:   Defines tests GLSL->HLSL conversion of function parameters

#include "headers.hxx"
#include "FunctionParameterTests.hxx"
#include "GLSLIdentifierTable.hxx"
#include "GLSLTranslate.hxx"
#include "WexString.h"
#include "WexTestClass.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace ft_glslparse
{
    //+----------------------------------------------------------------------------
    //
    //  Function:   FunctionParameterNoIdentifierTests
    //
    //  Synopsis:   Tests function parameter definition with no identifier
    //              "int foo([EMPTY|in|out|inout] int);"
    //
    //  Datasource: Table:FunctionDeclarationDataSource.xml#FunctionParameterNoIdentifierTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void FunctionParameterTests::FunctionParameterNoIdentifierTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }
    
    //+----------------------------------------------------------------------------
    //
    //  Function:   FunctionParametersNoIdentifierTests
    //
    //  Synopsis:   Tests multiple function parameter definitions with no identifier
    //              "int foo([EMPTY|in|out|inout] int, int);"
    //
    //  Datasource: Table:FunctionDeclarationDataSource.xml#FunctionParametersNoIdentifierTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void FunctionParameterTests::FunctionParametersNoIdentifierTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }
    
    //+----------------------------------------------------------------------------
    //
    //  Function:   FunctionParameterSingleIdentifierTests
    //
    //  Synopsis:   Tests function parameter definition with an identifier
    //              "int foo([EMPTY|in|out|inout] int bar);"
    //
    //  Datasource: Table:FunctionDeclarationDataSource.xml#FunctionParameterWithIdentifierTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void FunctionParameterTests::FunctionParameterSingleIdentifierTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }
    
    //+----------------------------------------------------------------------------
    //
    //  Function:   FunctionParameterMultipleIdentifierTests
    //
    //  Synopsis:   Tests multiple function parameter definitions with identifiers
    //              "int foo([EMPTY|in|out|inout] int bar, int baz);"
    //
    //  Datasource: Table:FunctionDeclarationDataSource.xml#FunctionParameterWithMultipleIdentifiersTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void FunctionParameterTests::FunctionParameterMultipleIdentifierTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }
    
    //+----------------------------------------------------------------------------
    //
    //  Function:   InvalidMultipleIdentifierTests
    //
    //  Synopsis:   Tests invalid indentifiers for variables
    //              "int int;" etc.
    //
    //  Datasource: Table:VaryingTypeDataSource.xml#BasicVaryingTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void FunctionParameterTests::InvalidMultipleIdentifierTests()
    {
        ParserTestBase::TestInvalidGLSLParseFromXML(L"TypeGLSL", L"FragmentGLSL", L"VertexGLSL");
    }
}
