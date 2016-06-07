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

//  Class:      VariableDeclarationTests
//  Synopsis:   Implements tests for GLSL->HLSL conversion of variable declarations

#include "headers.hxx"
#include "VariableDeclarationTests.hxx"
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
    //  Function:   ValidIdentifiers
    //
    //  Synopsis:   Tests various valid identifier scenarios
    //
    //  Datasource: Table:ValidVariableIdentifierDataSource.xml#Scenarios (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::ValidIdentifiers()
    {
        //VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }


    //+----------------------------------------------------------------------------
    //
    //  Function:   BasicAttributeTest
    //
    //  Synopsis:   Tests the "attribute" modifier for all variable types
    //              "attribute int foo;"
    //
    //  Datasource: Table:AttributeDataSource.xml#BasicAttributeTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::BasicAttributeTest()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   BasicUniformTest
    //
    //  Synopsis:   Tests the "uniform" modifier for all variable types
    //              "uniform int foo;"
    //
    //  Datasource: Table:UniformDataSource.xml#BasicUniformTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::BasicUniformTest()
    {
        //VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"TypeGLSL", L"TypeHLSL"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   InitializerTest
    //
    //  Synopsis:   Variables with an initializer
    //              "int foo = 1;"
    //
    //  Datasource: Table:VariableTypesDataSource.xml#InitializerTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::InitializerTest()
    { 
        //const LPWSTR spGLSLValueNames[] = {L"TypeGLSL", L"InitializerGLSL"};
        //const LPWSTR spHLSLValueNames[] = {L"TypeHLSL", L"InitializerHLSL"};
        //const LPWSTR spTokens[] = {L"%a", L"%b"};
        //const int nNumberOfElements = 2;

        //VERIFY_SUCCEEDED(
        //    ParserTestBase::TestHLSLProgramConversionFromXML(spGLSLValueNames, spHLSLValueNames, spTokens, nNumberOfElements)
        //    );
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   ScopingTest
    //
    //  Synopsis:   Variables with the same name in different scopes
    //              "int foo = 1; void main(void){ int foo = 1}"
    //
    //  Datasource: Table:VariableTypesDataSource.xml#ScopingTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::ScopingTest()
    { 
        //const LPWSTR spGLSLValueNames[] = {L"TypeGLSL", L"InitializerGLSL"};
        //const LPWSTR spHLSLValueNames[] = {L"TypeHLSL", L"InitializerHLSL"};
        //const LPWSTR spTokens[] = {L"%a", L"%b"};
        //const int nNumberOfElements = 2;

        //VERIFY_SUCCEEDED(
        //    ParserTestBase::TestHLSLProgramConversionFromXML(spGLSLValueNames, spHLSLValueNames, spTokens, nNumberOfElements)
        //    );
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   InvalidIdentifiers
    //
    //  Synopsis:   Tests various invalid identifiers
    //
    //  Datasource: Table:InvalidVariableDeclarationsDataSource.xml#BasicVaryingTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::InvalidIdentifiers()
    {
        ParserTestBase::TestInvalidGLSLParseFromXML(L"TypeGLSL", L"FragmentGLSL", L"VertexGLSL");
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   InvalidAttributeTest
    //
    //  Synopsis:   Tests invalid scenarios for the "attribute" modifier for all variable types
    //
    //  Datasource: Table:InvalidVariableDeclarationsDataSource.xml#BasicVaryingTable (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::InvalidAttributeTest()
    {
        ParserTestBase::TestInvalidGLSLParseFromXML(L"TypeGLSL", L"FragmentGLSL", L"VertexGLSL");
    }
    
    //+----------------------------------------------------------------------------
    //
    //  Function:   MultipleStorageQualifiersTest
    //
    //  Synopsis:   Tests invalid with multiple qualifiers
    //              "varying attribute foo;"
    //
    //  Datasource: Table:InvalidVariableDeclarationsDataSource.xml#MultipleStorageQualifiers (function)
    //              Table:VariableTypesDataSource.xml#TypeTable(class)
    //-----------------------------------------------------------------------------
    void VariableDeclarationTests::MultipleStorageQualifiersTest()
    {
        ParserTestBase::TestInvalidGLSLParseFromXML(L"TypeGLSL", L"FragmentGLSL", L"VertexGLSL");
    }
}
