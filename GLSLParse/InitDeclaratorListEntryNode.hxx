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

class VariableIdentifierNode;

//+-----------------------------------------------------------------------------
//
//  Enum:       DeclarationType
//
//  Synopsis:   Enumeration for the type of declaration that is being created.
//
//              GLSL declarations can optionally have an initializer or be
//              an array declaration, but not both.
//
//------------------------------------------------------------------------------
namespace DeclarationType
{
    enum Enum
    {
        simple,                                             // No initializer and no array
        initialized,                                        // An initializer was provided
        arrayDecl                                           // An array is being declared
    };
}

//+-----------------------------------------------------------------------------
//
//  Class:      InitDeclaratorListEntryNode
//
//  Synopsis:   Node information for an entry in the InitDeclaratorList.
//
//              This is not a SingleDeclarationNode because it does not
//              correspond 1:1 to the definition of a single declaration in
//              the grammar (it has no type information).
//
//------------------------------------------------------------------------------
class InitDeclaratorListEntryNode : public CollectionNode
{
public:
    InitDeclaratorListEntryNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pIdentifier,                    // The identifier for the entry
        __in_opt ParseTreeNode* pInitializer,               // The optional initializer for the entry
        __in_opt ParseTreeNode* pArraySizeExpr              // The array size expression (this is optional)
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
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;
    HRESULT SetHLSLNameIndex(UINT uIndex) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::initDeclaratorListEntry; }

    // Other methods
    VariableIdentifierNode* GetIdentifierNode();
    ParseTreeNode* GetInitializerNode();
    int GetArraySize() const { return _arraySize; }
    DeclarationType::Enum GetDeclarationType() const { return _declarationType; }

    HRESULT OutputTranslation(
        __in IStringStream* pOutput,                        // Stream to output translation to
        __in_z_opt const char* pszIdentifierOverride        // String to override identifier name with
        );

    void SetVerifiedArraySize(int arraySize);

    void ExtractInitializer(const TSmartPointer<ParseTreeNode>& spInitExpr);

private:
    HRESULT VerifyTypeForQualifier(int qualifier, __in const GLSLType* pType) const;

private:
    DeclarationType::Enum _declarationType;                 // The type of declaration that we represent
    int _arraySize;                                         // The size of the array being declared
    UINT _uHLSLNameIndex;                                   // Which HLSL name to use
};
