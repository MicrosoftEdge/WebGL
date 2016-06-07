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
#include "GLSLTypeInfo.hxx"
#include "BasicTypeNode.hxx"
#include "glsl.tab.h"

//+-----------------------------------------------------------------------------
//
//  Class:      TypeSpecifierNode
//
//  Synopsis:   Node information for a terminal node with a type specifer in it.
//
//------------------------------------------------------------------------------
class TypeSpecifierNode : public CollectionNode
{
public:
    TypeSpecifierNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        __in ParseTreeNode* pType,                  // The type stored in the node
        int precision,                              // The precision stored in the node
        const YYLTYPE &location                     // The location of the type specifier
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        ) override;
    
    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::typeSpecifier; }

    HRESULT GetType(__deref_out GLSLType** ppType) const;

    static HRESULT CreateNodeFromType(__in CGLSLParser* pParser, __in GLSLType* pGLSLType, __deref_out TypeSpecifierNode** ppTypeSpecifierNode);

    int GetComputedPrecision() const { Assert(IsVerified()); return _precisionComputed; }

private:
    HRESULT VerifyAndComputePrecision();
    HRESULT VerifyAndComputePrecisionForBasicType(int basicType);

private:
    TSmartPointer<GLSLType> _spType;                // The type specified by this nodes child
    int _precisionSpecified;                        // The specified precision (NO_PRECISION if not specified)
    int _precisionComputed;                         // The precision computed at verification time
    YYLTYPE _location;                              // Location of the type specifier in source
};
