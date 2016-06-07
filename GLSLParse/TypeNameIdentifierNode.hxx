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

class CTypeNameIdentifierInfo;

MtExtern(TypeNameIdentifierNode);

//+-----------------------------------------------------------------------------
//
//  Class:      TypeNameIdentifierNode
//
//  Synopsis:   Node that represents a symbol that maps to a user-defined type
//              name. This name is looked up and resolved to the appropriate
//              type when the node is verified.
//
//------------------------------------------------------------------------------
class TypeNameIdentifierNode : public IdentifierNodeBase
{
public:
    TypeNameIdentifierNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        int symbolIndex,                                    // The index of the symbol
        const YYLTYPE &location                             // The location of the symbol
        )
    {
        return IdentifierNodeBase::Initialize(pParser, symbolIndex, location);
    }

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in CTypeNameIdentifierInfo* pTypeNameInfo         // Anonymous type name info to initialize with
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    DECLARE_MEMALLOC_NEW_DELETE(Mt(TypeNameIdentifierNode));

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) override { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT InitializeAsClone(__in ParseTreeNode* pOriginal) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::typeNameIdentifier; }

    // Other methods
    void SetTypeNameIdentifierInfo(__in CTypeNameIdentifierInfo* pInfo);

    HRESULT GetTypeNameIdentifierInfo(
        __deref_out CTypeNameIdentifierInfo** ppInfo        // The info about the identifier
        );

    void SetOutputAsConstructor() { _fOutputAsConstructor = true; }

private:
    TSmartPointer<CTypeNameIdentifierInfo> _spInfo;         // The identifier info for the identifier this node represents
    bool _fOutputAsConstructor;                             // Whether outputting this node is part of a constructor call
#if DBG
    bool _fIsClone;                                         // Whether this node was created as a result of cloning
#endif
};
