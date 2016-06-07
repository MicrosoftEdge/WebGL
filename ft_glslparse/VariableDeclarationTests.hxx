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
//  Synopsis:   Defines tests for GLSL->HLSL conversion of variable declarations

#undef Verify
#include "WexTestClass.h"
#include "GLSLShaderType.hxx"
#include "ParserTestBase.hxx"

namespace ft_glslparse
{
    class VariableDeclarationTests : public WEX::TestClass<VariableDeclarationTests>
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS(VariableDeclarationTests)

        // Declare the tests within this class
        BEGIN_TEST_METHOD(ValidIdentifiers)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:ValidVariableIdentifierDataSource.xml#Scenarios")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(BasicAttributeTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTableNoInt; Table:AttributeDataSource.xml#BasicAttributeTable")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(BasicUniformTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:UniformDataSource.xml#BasicUniformTable")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(InitializerTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:VariableTypesDataSource.xml#InitializerTable")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(ScopingTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:VariableTypesDataSource.xml#ScopingTable")
        END_TEST_METHOD()

        // Negative tests
        BEGIN_TEST_METHOD(InvalidIdentifiers)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:InvalidVariableDeclarationsDataSource.xml#IdentifierScenarios")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(InvalidAttributeTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:InvalidVariableDeclarationsDataSource.xml#InvalidAttributeIdentifier")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(MultipleStorageQualifiersTest)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:VariableTypesDataSource.xml#TypeTable; Table:InvalidVariableDeclarationsDataSource.xml#MultipleStorageQualifiers")
        END_TEST_METHOD()
    };
}
