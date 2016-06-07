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
#include "VariableIdentifierNode.hxx"
#include "GLSL.tab.h"

class FullySpecifiedTypeNode;
class InitDeclaratorListEntryNode;
class TypeSpecifierNode;

//+-----------------------------------------------------------------------------
//
//  Class:      InitDeclaratorListNode
//
//  Synopsis:   Node information for a initializer declarator list.
//
//------------------------------------------------------------------------------
class InitDeclaratorListNode : public CollectionNode
{
public:
    InitDeclaratorListNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pFullType,                      // The type of the things being declared
        __in ParseTreeNode* pIdentifier,                    // The first identifier in the list
        const YYLTYPE &location,                            // The location of the declarator
        __in_opt ParseTreeNode* pInitializer,               // The initializer (this is optional)
        __in_opt ParseTreeNode* pArraySizeExpr              // The array size expression (this is optional)
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pStructSpecifier,               // The struct specifier for the type being declared
        const YYLTYPE &location                             // The location of the declarator
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        );

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT PreVerifyChildren() override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::initDeclaratorList; }

    // Other methods
    HRESULT OutputTranslation(
        UINT uIdentifierIndex,                              // Index of identifier to write out, s_uAllChildrenIdentifiers to do all
        bool fUnused,                                       // Whether to print it as an unused entry for a varying
        __in IStringStream* pOutput                         // Stream to output to
        ) const;

    bool IsShaderInput() const { return (_typeQualifier == ATTRIBUTE || _typeQualifier == VARYING); }
    bool IsGLSLSampler() const { return (_spType->IsSampler2DType() || _spType->IsSamplerCubeType()); }
    int GetTypeQualifier() const { return _typeQualifier; }
    int GetPrecisionQualifier() const { return _precisionQualifier; }
    void SetPrecisionQualifier(int precisionQualifier) { _precisionQualifier = precisionQualifier; }
    HRESULT GetSpecifiedType(__deref_out GLSLType** ppType) const;
    void SetSpecifiedType(__in GLSLType* pType);
    UINT GetIdentifierCount() const { return GetChildCount() - 1; }
    VariableIdentifierNode* GetIdentifier(UINT index) const;
    InitDeclaratorListEntryNode* GetEntry(UINT index) const;
    UINT GetVectorCount() const;
    const YYLTYPE &GetLocation() const { return _location; }
    bool IsGlobalScope();
    FullySpecifiedTypeNode* GetFullySpecifiedTypeNode();
    TypeSpecifierNode* GetTypeSpecifierDescendant();

public:
    static const UINT s_uAllChildrenIdentifiers;            // Special value to tell OutputTranslation to output all identifiers

private:
    int _typeQualifier;                                     // The type qualifier, as a bison generated int
    int _precisionQualifier;                                // The precision qualifier, as a bison generated int
    TSmartPointer<GLSLType> _spType;                        // The type of the type specifier. Populated when FullySpecifiedTypeNode child is verified
    YYLTYPE _location;                                      // The location of the identifier in the source
};
