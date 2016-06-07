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

//  Class:      SamplerTests
//  Synopsis:   Implements tests for GLSL->HLSL conversion of sampler types

#include "headers.hxx"
#include "SamplerTests.hxx"
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
    //  Function:   BasicSamplerTests
    //
    //  Synopsis:   Tests integer literals in a variety of vector constructors
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void SamplerTests::BasicSamplerTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(NULL, NULL));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   SamplerIdentifierTests
    //
    //  Synopsis:   Tests integer literals in a variety of vector constructors
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void SamplerTests::SamplerIdentifierTests()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestHLSLProgramConversionFromXML(NULL, NULL));
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   InvalidSamplerTest
    //
    //  Synopsis:   Tests integer literals in a variety of vector constructors
    //
    //  Datasource: Table:LiteralValuesDataSource.xml#ValidIntegerValues (function)
    //              Table:LiteralValuesDataSource.xml#Programs (class)
    //-----------------------------------------------------------------------------
    void SamplerTests::InvalidSamplerTest()
    {
        VERIFY_SUCCEEDED(ParserTestBase::TestInvalidGLSLParseFromXML(NULL , L"FragmentGLSL", L"VertexGLSL"));
    }
}
