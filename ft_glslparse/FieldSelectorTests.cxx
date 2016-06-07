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

#include "headers.hxx"
#include "FieldSelectorTests.hxx"
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
    //  Function:   singleRValueTests
    //
    //  Synopsis:   Tests retrieval of float values via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#singleRValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::singleRValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   singleLValueTests
    //
    //  Synopsis:   Tests assigment to float variables via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#singleLValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::singleLValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   doubleRValueTests
    //
    //  Synopsis:   Tests retrieval of vec2 values via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#doubleRValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::doubleRValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   doubleLValueTests
    //
    //  Synopsis:   Tests assigment to vec2 variables via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#doubleLValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::doubleLValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   tripleRValueTests
    //
    //  Synopsis:   Tests retrieval of vec3 values via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#tripleRValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::tripleRValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   tripleLValueTests
    //
    //  Synopsis:   Tests assigment to vec3 variables via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#tripleLValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::tripleLValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   quadRValueTests
    //
    //  Synopsis:   Tests retrieval of vec4 values via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#quadRValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::quadRValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   quadLValueTests
    //
    //  Synopsis:   Tests assigment to vec4 variables via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#quadLValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::quadLValueTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(L"GLSLVector", L"HLSLVector"));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   negativeTests
    //
    //  Synopsis:   Tests negative assignment and retrieval of variable values via field selectors
    //
    //  Datasource: Table:FieldSelectorsDataSource.xml#singleLValues (function)
    //              Table:FieldSelectorsDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void FieldSelectorTests::negativeTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestInvalidGLSLParseFromXML(NULL, L"InvalidFragmentGLSL", L"InvalidVertexGLSL"));
    }
}
