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

#include "ParseTreeNode.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      FloatConstantNode
//
//  Synopsis:   Node information for a terminal node with a symbol in it.
//
//------------------------------------------------------------------------------
class FloatConstantNode : public ParseTreeNode
{
public:
    FloatConstantNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        double constant                             // The index of the symbol
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;

    HRESULT IsConstExpression(
        bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
        __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
        __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
        ) const override;

private:
    double _constant;                               // The constant from the parser
};

