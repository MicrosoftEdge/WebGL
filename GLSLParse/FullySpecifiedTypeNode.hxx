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
#include "TypeSpecifierNode.hxx"

class TypeSpecifierNode;

//+-----------------------------------------------------------------------------
//
//  Class:      FullySpecifiedTypeNode
//
//  Synopsis:   Node information for a fully specified type in GLSL.
//
//------------------------------------------------------------------------------
class FullySpecifiedTypeNode : public CollectionNode
{
public:
    FullySpecifiedTypeNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        int typeQual,                                       // The type qualifier (const, attribute, etc)
        __in ParseTreeNode* pSpec                           // The type specifier
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;

    // CollectionNode overrides
    HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        ) override;

    // Other methods
    TypeSpecifierNode* GetTypeSpecifierNode() const;
    HRESULT GetType(__deref_out GLSLType** ppType) const;
    int GetTypeQualifier() const { return _typeQual; }
    void SetTypeQualifier(int typeQual) { _typeQual = typeQual; }

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::fullySpecifiedType; }

private:
    int _typeQual;                                          // The type qualifier (const, attribute, etc)
};
