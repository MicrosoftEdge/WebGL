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

//  Synopsis:   Implements tests for GLSL->HLSL conversion of field selectors

#undef Verify
#include "WexTestClass.h"
#include "GLSLShaderType.hxx"
#include "ParserTestBase.hxx"

namespace ft_glslparse
{
    class FieldSelectorTests : public WEX::TestClass<FieldSelectorTests>
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.

        BEGIN_TEST_CLASS(FieldSelectorTests)
            TEST_CLASS_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#Programs")  
        END_TEST_CLASS()  

        // Declare the tests within this class
        BEGIN_TEST_METHOD(singleRValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#singleRValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(singleLValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#singleLValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(doubleRValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#doubleRValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(doubleLValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#doubleLValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(tripleRValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#tripleRValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(tripleLValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#tripleLValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(quadRValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#quadRValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(quadLValueTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#quadLValues")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(negativeTests)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:FieldSelectorsDataSource.xml#negativeValues")
        END_TEST_METHOD()
    };
}
