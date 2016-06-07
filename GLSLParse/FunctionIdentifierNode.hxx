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

#include "IdentifierNodeBase.hxx"
#include "KnownSymbols.hxx"
#include "IdentifierInfo.hxx"
#include "GLSL.tab.h"

MtExtern(FunctionIdentifierNode);

class CollectionNodeWithScope;
class CFunctionIdentifierInfo;

//+-----------------------------------------------------------------------------
//
//  Class:      FunctionIdentifierNode
//
//  Synopsis:   Node information for a terminal node with a symbol in it.
//
//------------------------------------------------------------------------------
class FunctionIdentifierNode : public IdentifierNodeBase
{
public:
    FunctionIdentifierNode();
    
    DECLARE_MEMALLOC_NEW_DELETE(Mt(FunctionIdentifierNode));

    HRESULT InitializeAsClone(__in ParseTreeNode* pOriginal) override;
    
    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::functionIdentifier; }

    // Other methods
    void SetFunctionIdentifierInfo(__in CFunctionIdentifierInfo* pInfo);

    HRESULT GetFunctionIdentifierInfo(
        __deref_out CFunctionIdentifierInfo** ppInfo        // The info about the identifier
        ) const;

private:
    TSmartPointer<CFunctionIdentifierInfo> _spInfo;         // The identifier info for the identifier this node represents
};
