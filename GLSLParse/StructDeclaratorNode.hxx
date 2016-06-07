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

#include "CollectionNode.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      StructDeclaratorNode
//
//  Synopsis:   Represents a variable name and array size combination for
//              struct declarations
//
//------------------------------------------------------------------------------
class StructDeclaratorNode : public CollectionNode
{
public:
    StructDeclaratorNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                      // The parser that owns the tree
        __in ParseTreeNode* pVariableIdentifierNode,    // The variable identifier
        __in_opt ParseTreeNode* pConstantExpression     // The constant expression portion of an array declarator
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                       // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::structDeclarator; }

    int GetArraySize() const;

    VariableIdentifierNode* GetVariableIdentifierChild() const;

    ParseTreeNode* GetConstantExpressionChild() const;

private:
    int _arraySize;                                     // Verified size of array (if declared as one)
};
