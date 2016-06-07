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
#include "GLSL.tab.h"

class FunctionIdentifierNode;
class TypeNameIdentifierNode;

MtExtern(FunctionCallIdentifierNode);

//+-----------------------------------------------------------------------------
//
//  Class:      FunctionCallIdentifierNode
//
//  Synopsis:   Node that represents an identifier that is used as a function
//              call. At parse time this could either be a normal function 
//              call or a user-defined typename constructor. This is determined
//              at verification time, at which point an appropriate child node
//              (either FunctionIdentifierNode or TypeNameIdentifierNode) is
//              generated and added as a child.
//
//------------------------------------------------------------------------------
class FunctionCallIdentifierNode : public CollectionNode
{
public:
    FunctionCallIdentifierNode();
    
    DECLARE_MEMALLOC_NEW_DELETE(Mt(FunctionCallIdentifierNode));

    HRESULT Initialize(
        __in CGLSLParser* pParser,          // The parser that owns the tree
        int symbolIndex,                    // The index of the symbol
        const YYLTYPE& location             // The location of the symbol
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::functionCallIdentifier; }

    FunctionIdentifierNode* GetFunctionIdentifierChild() const;
    TypeNameIdentifierNode* GetTypeNameIdentifierChild() const;

private:
    template<typename T1>
    T1* GetIdentifierChild() const;

private:
    int _iSymbolIndex;                      // Symbol for this identifier
    YYLTYPE _location;                      // Location for this identifier
};
