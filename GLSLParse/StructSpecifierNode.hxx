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
#include "StructGLSLType.hxx"
#include "TypeNameIdentifierInfo.hxx"

class StructDeclarationListNode;
class TypeNameIdentifierNode;
class CTypeNameIdentifierInfo;

//+-----------------------------------------------------------------------------
//
//  Class:      StructSpecifierNode
//
//  Synopsis:   Node that encapsulates declaring a struct type, including the
//              typename and field declarations.
//
//------------------------------------------------------------------------------
class StructSpecifierNode : public CollectionNode
{
public:
    StructSpecifierNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        const YYLTYPE& location,                            // The location of the struct specifier
        __in_opt ParseTreeNode* pTypeNameIdentifierNode,    // The typename identifier (optional)
        __in ParseTreeNode* pDeclarationList                // The declaration list for the struct specifier
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::structSpecifier; }

    HRESULT GetType(__deref_out GLSLType** ppType) const;
    HRESULT GetTypeNameIdentifierInfo(__deref_out CTypeNameIdentifierInfo** ppInfo) const;
    const YYLTYPE &GetLocation() const { return _location; }
    TypeNameIdentifierNode* GetTypeNameIdentifierChild() const;

    HRESULT OutputHLSLFunctions(__in IStringStream* pOutput);

private:
    StructDeclarationListNode* GetStructDeclListChild() const;

private:
    TSmartPointer<StructGLSLType> _spType;                              // Type that is defined by this struct specifier node
    TSmartPointer<CTypeNameIdentifierInfo> _spTypeNameInfo;             // Typename info that this struct specifier defines
    YYLTYPE _location;                                                  // The location of the identifier in the source
};
