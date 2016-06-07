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

#undef Verify
#include "WexTestClass.h"
#include "GLSLShaderType.hxx"

namespace ft_glslparse
{
    class BasicGLSLTests : public WEX::TestClass<BasicGLSLTests>
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS(BasicGLSLTests)

        // Declare the tests within this class
        TEST_METHOD(ShaderVerificationTests)
        TEST_METHOD(BasicDeclarationTests)
        TEST_METHOD(VariableQualifierDeclarationTests)
        TEST_METHOD(LValueTests)
        TEST_METHOD(DefaultInitTests)
        TEST_METHOD(IntLiteralTests)
        TEST_METHOD(FloatLiteralTests)
        TEST_METHOD(ArrayDeclarationTests)
        TEST_METHOD(ScalarConstructorTests)
        TEST_METHOD(VectorConstructorTests)
        TEST_METHOD(MatrixConstructorTests)
        TEST_METHOD(ErrorCleanupTests)
        TEST_METHOD(InputDeclarationTests)
        TEST_METHOD(FragColorTests)
        TEST_METHOD(NonInputDeclarationTests)
        TEST_METHOD(ScopingDeclarationTests)
        TEST_METHOD(Texture2DTests)
        TEST_METHOD(SelectionTests)
        TEST_METHOD(ReturnTests)
        TEST_METHOD(LogicalOperatorTests)
        TEST_METHOD(ExpressionExpandScalarTests)
        TEST_METHOD(ExpressionTests)
        TEST_METHOD(IndexingTests)
        TEST_METHOD(ExpressionListTests)
        TEST_METHOD(KnownFunctionTests)
        TEST_METHOD(Stage0Tests)
        TEST_METHOD(CommentTests)
        TEST_METHOD(BasicLexerTests)
        TEST_METHOD(UnaryOperatorTests)
        TEST_METHOD(RelationalExpressionTests)
        TEST_METHOD(ConditionalExpressionTests)
        TEST_METHOD(PreprocessTests)
        TEST_METHOD(FunctionTests)
        TEST_METHOD(ForLoopTests)
        TEST_METHOD(ScopeTests)
        TEST_METHOD(DiscardTests)
        TEST_METHOD(SwizzleTests)
        TEST_METHOD(NegativeTests)
        TEST_METHOD(SpecialVariableTests)
        TEST_METHOD(VariableNameTests)
        TEST_METHOD(ErrorLine)
        TEST_METHOD(FrontFacingTests)
        TEST_METHOD(DepthLimitTests)
        TEST_METHOD(VariableCountLimit)
        TEST_METHOD(StructTests)
        TEST_METHOD(DerivativeTests)
        TEST_METHOD(FragDepthTests)
        TEST_METHOD(TestExtensions)
        TEST_METHOD(PrecisionTests)
        TEST_METHOD(GlobalDeclarationTests)

    private:
        void TestParserInput(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, const char* pszExpected);
        void TestParserInputDefault(__in const WCHAR* pszInput, __in const char* pszExpected);
        void TestParserInputNegativeError(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, HRESULT hrCode);
        void TestParserInputNegativeErrorLine(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, HRESULT hrCode, int line);
};
} /* namespace ft_glslparse */ 
