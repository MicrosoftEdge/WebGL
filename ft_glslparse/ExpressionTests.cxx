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

//  Synopsis:   Implements tests for GLSL->HLSL conversion of numeric expressions

#include "headers.hxx"
#include "ExpressionTests.hxx"
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
    //  Function:   BasicExpressionTests
    //
    //  Synopsis:   Tests basic same-type math expressions for all supported native GLSL types
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void ExpressionTests::BasicExpressionTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"Operator", L"Operator"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   ScalarExpressionTests
    //
    //  Synopsis:   Tests scalar math expressions for all supported native GLSL types
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void ExpressionTests::ScalarExpressionTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(NULL, NULL));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   MatrixExpressionTests
    //
    //  Synopsis:   Tests scalar math expressions on matrix types
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void ExpressionTests::MatrixExpressionTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(NULL, NULL));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   ParenthesisExpressionTests
    //
    //  Synopsis:   Tests expressions with parenthesis
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ParenthesisExpressions (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void ExpressionTests::ParenthesisExpressionTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"Operator", L"Operator"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   InvalidExpressionTests
    //
    //  Synopsis:   Tests invalid math expressions
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void ExpressionTests::InvalidExpressionTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestInvalidGLSLParseFromXML(NULL, L"FragmentGLSL", L"VertexGLSL"));
    }
}
