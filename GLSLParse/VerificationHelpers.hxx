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
#pragma once

class ParseTreeNode;
class CGLSLParser;
struct YYLTYPE;
class CFunctionIdentifierInfo;
class GLSLType;
class FunctionHeaderWithParametersNode;
class FunctionIdentifierNode;

//+-----------------------------------------------------------------------------
//
//  Class:      VerificationHelpers
//
//  Synopsis:   Contains static helper functions that aid in common verification
//              functionality shared across various node types.
//
//------------------------------------------------------------------------------
class VerificationHelpers
{
public:
    static HRESULT VerifyArrayConstant(
        __in ParseTreeNode* pConstantExpression,                        // Expression to retrieve int constant from 
        __in CGLSLParser* pParser,                                      // Parser that we're verifying for
        __in const YYLTYPE* pDeclaratorLocation,                        // Location of declarator the array size applies to
        __out int* pArraySizeConstant                                   // Verified array size constant
        );

    static HRESULT VerifyNewFunctionInfoAgainstMatchedInfo(
        __in const CFunctionIdentifierInfo* pNewInfo,                   // New function info to verify
        __in const CFunctionIdentifierInfo* pMatchedInfo,               // Matched function info to verify against
        __in const GLSLType* pReturnType,                               // Return type determine when matching signature
        __in const FunctionHeaderWithParametersNode* pFunctionHeader,   // Function header being declared/defined
        __in const FunctionIdentifierNode* pIdentifier                  // Function identifier of added info
        );
};
