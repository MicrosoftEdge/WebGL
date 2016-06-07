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
#include "GLSLTypeInfo.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      BasicTypeNode
//
//  Synopsis:   Represents a basic type (as opposed to user defined or struct type)
//
//------------------------------------------------------------------------------
class BasicTypeNode : public ParseTreeNode
{
public:
    BasicTypeNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        int type                                    // The type stored in the node
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    HRESULT InitializeAsClone(__in ParseTreeNode* pOriginal) override;
    
    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT SetHLSLNameIndex(UINT uIndex) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::basicType; }

    HRESULT GetType(__deref_out GLSLType** ppType) const;

private:
    TSmartPointer<GLSLType> _spType;                // GLSL type that represents the basic type
    int _basicType;                                 // The GLSL basic type (like float, vec2, etc)
    UINT _uHLSLNameIndex;                           // Which HLSL name to use
};
