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
//  Interface:  ITextNodeProvider
//
//  Synopsis:   Interface for an object that provides text to a text node.
//
//------------------------------------------------------------------------------
interface ITextNodeProvider
{
    virtual HRESULT OnOutputHLSL(__in IStringStream* pOutput) = 0;
};

//+-----------------------------------------------------------------------------
//
//  Class:      TextNode
//
//  Synopsis:   This is a node for inserting bulk text into the translation.
//
//------------------------------------------------------------------------------
class TextNode : public ParseTreeNode
{
public:
    TextNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,              // The parser that owns the tree
        __in ITextNodeProvider* pProvider       // The provider of the text
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser               // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::text; }

private:
    ITextNodeProvider* _pProvider;              // The provider of the text
};
