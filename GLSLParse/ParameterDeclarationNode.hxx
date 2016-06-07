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
#include "GLSL.tab.h"

class TypeQualifierNode;
class ParameterDeclaratorNode;

//+-----------------------------------------------------------------------------
//
//  Class:      ParameterDeclarationNode
//
//  Synopsis:   Node information for a fully specified type in GLSL.
//
//------------------------------------------------------------------------------
class ParameterDeclarationNode : public CollectionNode
{
public:
    ParameterDeclarationNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        int paramQual,                                      // The parameter qualifier (in, out, etc)
        __in ParseTreeNode* pDecl,                          // The parameter declarator
        int typeQual,                                       // The type qualifier (const, attribute, etc)
        const YYLTYPE &location                             // The location of the declaration
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        );

    // ParseTreeNode override
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT SetHLSLNameIndex(UINT uIndex) override;

    // Other methods
    ParameterDeclaratorNode* GetParameterDeclaratorNode();
    HRESULT GetType(__deref_out GLSLType** ppType);

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::parameterDeclaration; }

    int GetTypeQualifier() const { return _typeQual; }
    int GetParamQualifier() const { return _paramQual; }

    const YYLTYPE& GetLocation() const { return _location; }

private:
    YYLTYPE _location;                                      // The location of the declarator
    UINT _uHLSLNameIndex;                                   // Which HLSL name to use
    int _paramQual;                                         // The parameter qualifier (in, out, etc)
    int _typeQual;                                          // The type qualifier (const, attribute, etc)
};
