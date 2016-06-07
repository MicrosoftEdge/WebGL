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

namespace ft_glslparse
{
    typedef HRESULT(*PFNCreateToken)(__inout CMutableString<char>& strToken);

    class BasicPreprocessorTests : public WEX::TestClass<BasicPreprocessorTests>
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS(BasicPreprocessorTests)

        // Declare the tests within this class
        TEST_METHOD(VersionTests)
        TEST_METHOD(BasicDirectiveTests)
        TEST_METHOD(IfDefTests)
        TEST_METHOD(IfNDefTests)
        TEST_METHOD(IfDefinedTests)
        TEST_METHOD(ElifTests)
        TEST_METHOD(IfMultiplicationTests)
        TEST_METHOD(IfAdditionTests)
        TEST_METHOD(IfShiftTests)
        TEST_METHOD(IfRelationalTests)
        TEST_METHOD(IfEqualityTests)
        TEST_METHOD(IfLogicalTests)
        TEST_METHOD(IfUnaryTests)
        TEST_METHOD(IfBitwiseTests)
        TEST_METHOD(IfPrecedenceTests)
        TEST_METHOD(ExpressionTests)
        TEST_METHOD(FunctionMacroTests)
        TEST_METHOD(DefineTests)
        TEST_METHOD(RedefineTests)
        TEST_METHOD(UndefineTests)
        TEST_METHOD(CommentTests)
        TEST_METHOD(DisabledOutputTests)
        TEST_METHOD(PragmaTests)
        TEST_METHOD(CustomErrorTests)
        TEST_METHOD(ExtensionTests)
        TEST_METHOD(CharacterSetTests)
        TEST_METHOD(TokenLimitTests)
        TEST_METHOD(LineMacroTests)

    private:
        void TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, int lineNumber, const char* pszErrorExpected);
        void TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, int lineNumber);
        void TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, const char* pszErrorExpected);
        void TestPreprocessorNegative(char* pszInput, HRESULT hrExpected);
        void TestPreprocessorInput(char* pszInput, const char* pszExpected);

        void TestPreprocessorLargeTokenNegative(char* pszInput, HRESULT hrExpected, PFNCreateToken pfnCreateToken);
    };
} /* namespace ft_Preprocessorparse */ 
