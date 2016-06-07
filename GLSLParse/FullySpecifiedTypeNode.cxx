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
#include "PreComp.hxx"
#include "FullySpecifiedTypeNode.hxx"
#include "IStringStream.hxx"
#include "TypeSpecifierNode.hxx"
#include "InitDeclaratorListNode.hxx"
#include "GLSL.tab.h"

MtDefine(FullySpecifiedTypeNode, CGLSLParser, "FullySpecifiedTypeNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FullySpecifiedTypeNode::FullySpecifiedTypeNode() : 
    _typeQual(NO_QUALIFIER)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int typeQual,                               // The type qualifier (const, attribute, etc)
    __in ParseTreeNode* pSpec                   // The type specifier
    )
{
    ParseTreeNode::Initialize(pParser);

    _typeQual = typeQual;

    return AppendChild(pSpec);
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Initialize from another node, given the collection
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    FullySpecifiedTypeNode* pOriginal = pOriginalColl->GetAs<FullySpecifiedTypeNode>();

    _typeQual = pOriginal->_typeQual;

    // Add the clone child
    CHK(AppendChild(rgChildClones[0]));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   If the parent is an InitDeclList, propagate the type from 
//              the (now verified) type specifier node to the parent InitDeclList
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::VerifySelf()
{
    CHK_START;

    if (GetParent()->GetParseNodeType() == ParseNodeType::initDeclaratorList)
    {
        // If our parent is a InitDeclList, cache the specified (but not fully formed due to arrayness)
        // type so that siblings (InitDeclListEntryNodes) can create the correct types for themselves.
        TSmartPointer<GLSLType> spTypeFromSpecifier;
        TypeSpecifierNode* pTypeSpecifier = GetTypeSpecifierNode();
        CHK(pTypeSpecifier->GetType(&spTypeFromSpecifier));

        InitDeclaratorListNode* pDeclList = GetParent()->GetAs<InitDeclaratorListNode>();
        pDeclList->SetSpecifiedType(spTypeFromSpecifier);

        // Also cache the precision qualifier from the specified type
        pDeclList->SetPrecisionQualifier(pTypeSpecifier->GetComputedPrecision());
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//              Attribute and varying are moved into a special struct, so we 
//              don't need to do anything with them. That just leaves uniform 
//              and const.
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    bool fGlobalScope = false;
    if (GetParent()->GetParseNodeType() == ParseNodeType::initDeclaratorList)
    {
        InitDeclaratorListNode* pDeclList = GetParent()->GetAs<InitDeclaratorListNode>();
        fGlobalScope = pDeclList->IsGlobalScope();
        AssertSz(pDeclList->GetIdentifierCount() > 0, "We should not be outputting a type for a InitDeclaratorList that has no identifiers");
    }

    // We ignore all qualifiers other than const when it comes to doing this
    // output. Attributes and varyings get moved into another place and that
    // effectively translates them. Uniforms (both declared as such in GLSL,
    // and implemention variables that are HLSL_UNIFORM) stay as globals and 
    // we don't want to touch them either.
    if (_typeQual == CONST_TOK || _typeQual == NO_QUALIFIER)
    {
        // Globals need to be static in HLSL, if indeed this type
        // is declaring a variable (declaration list has identifiers).
        if (fGlobalScope)
        {
            CHK(pOutput->WriteString("static "));
        }

        if (_typeQual == CONST_TOK)
        {
            CHK(pOutput->WriteString("const "));
        }
    }

    // Just translate the type specifier
    CHK(GetChild(0)->OutputHLSL(pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFullySpecifiedTypeNode
//
//  Synopsis:   Type safe accessor for the fully specified type node.
//
//-----------------------------------------------------------------------------
TypeSpecifierNode* FullySpecifiedTypeNode::GetTypeSpecifierNode() const
{
    return GetChild(0)->GetAs<TypeSpecifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type that this node is specifying.
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::GetType(__deref_out GLSLType** ppType) const
{
    return GetTypeSpecifierNode()->GetType(ppType);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FullySpecifiedTypeNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FullySpecifiedTypeNode typeQual=%d", _typeQual);
}
