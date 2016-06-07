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

#include "CollectionNodeWithScope.hxx"
#include "VariableIdentifierInfo.hxx"
#include "GLSL.tab.h"

//+-----------------------------------------------------------------------------
//
//  Class:      ForStatementNode
//
//  Synopsis:   Node information for a for statement in GLSL.
//
//------------------------------------------------------------------------------
class ForStatementNode : public CollectionNodeWithScope
{
public:
    ForStatementNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pInit,                          // For init statement
        __in ParseTreeNode* pRest,                          // For rest statement
        __in ParseTreeNode* pStatement,                     // What gets iterated
        const YYLTYPE &location                             // The location of the declarator
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::forStatement; }

private:
    HRESULT VerifyInitStatement();
    HRESULT VerifyCondStatement();
    HRESULT VerifyIterStatement();

    bool IsLoopIndexIdentifier(__in ParseTreeNode* pNode);
    HRESULT VerifyLoopIndexTypedConstant(__in ParseTreeNode* pNode, __out ConstantValue* pConstantValue);

    template <typename T>
    UINT DetermineLoopIterations() const;
private:

    //+----------------------------------------------------------------------------
    //
    //  Struct:     VerificationInfo
    //
    //  Synopsis:   Contains constant values that are set when verifiying the
    //              individual components of a for statement (init; condition; iter)
    //
    //+----------------------------------------------------------------------------
    struct VerificationInfo
    {
        ConstantValue loopInitializerConstant;
        ConstantValue loopComparisonConstant;
        ConstantValue loopIterationConstant;
    };

private:
    YYLTYPE _location;                                      // The location of the loop in the source
    TSmartPointer<CVariableIdentifierInfo> _spLoopIndex;    // The identifier info for the loop index
    int _basicType;                                         // The basic type of the loop index
    VerificationInfo _verificationInfo;                      // Information gathered about the for statement during verification
    bool _fRequestUnroll;                                   // Whether we should request the HLSL compiler to unroll this loop or not
};
