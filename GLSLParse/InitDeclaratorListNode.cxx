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
#include "InitDeclaratorListNode.hxx"
#include "InitDeclaratorListEntryNode.hxx"
#include "IStringStream.hxx"
#include "FullySpecifiedTypeNode.hxx"
#include "GLSLParser.hxx"
#include "TypeHelpers.hxx"
#include "GLSL.tab.h"

MtDefine(InitDeclaratorListNode, CGLSLParser, "InitDeclaratorListNode");

const UINT InitDeclaratorListNode::s_uAllChildrenIdentifiers = UINT_MAX;

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
InitDeclaratorListNode::InitDeclaratorListNode() :
    _typeQualifier(NO_QUALIFIER),
    _precisionQualifier(NO_PRECISION)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   This method is mainly concerned with:
//
//              * Adding the declarator list to the parser so that it can be
//                walked in a pre-translation step.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pFullType,              // The type of the things being declared
    __in ParseTreeNode* pIdentifier,            // The first identifier in the list
    const YYLTYPE &location,                    // The location of the declarator
    __in_opt ParseTreeNode* pInitializer,       // The initializer (this is optional)
    __in_opt ParseTreeNode* pArraySizeExpr      // The array size expression (this is optional)
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    _location = location;

    // First child is the fully specified type
    CHK(AppendChild(pFullType));

    // Each subsequent child is an entry - we just have the first one now
    TSmartPointer<ParseTreeNode> spEntryNode;
    CHK(RefCounted<InitDeclaratorListEntryNode>::Create(GetParser(), pIdentifier, pInitializer, pArraySizeExpr, /*out*/spEntryNode));

    CHK(AppendChild(spEntryNode));

    // Add to the list of declarators
    CHK(GetParser()->AddDeclaratorList(this));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   This initialization routine takes a struct specifier and builds
//              a TypeSpecifierNode then a FullySpecifiedTypeNode with which
//              this InitDeclaratorListNode can be initialized.
//              This function is used when a single declaration only contains
//              a struct_specifier with no variables.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pStructSpecifier,               // The struct specifier for the type being declared
    const YYLTYPE &location                             // The location of the declarator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    Assert(pStructSpecifier->GetParseNodeType() == ParseNodeType::structSpecifier);

    TSmartPointer<ParseTreeNode> spTypeSpecifierNode;
    CHK(RefCounted<TypeSpecifierNode>::Create(GetParser(), pStructSpecifier, NO_PRECISION, location, /*out*/spTypeSpecifierNode));

    TSmartPointer<ParseTreeNode> spFullySpecifiedTypeNode;
    CHK(RefCounted<FullySpecifiedTypeNode>::Create(GetParser(), NO_QUALIFIER, spTypeSpecifierNode, /*out*/spFullySpecifiedTypeNode));

    // First child is the fully specified type
    CHK(AppendChild(spFullySpecifiedTypeNode));

    // Add to the list of declarators
    CHK(GetParser()->AddDeclaratorList(this));

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Initialize from another node, given the collection
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    InitDeclaratorListNode* pOriginalList = pOriginalColl->GetAs<InitDeclaratorListNode>();

    _location = pOriginalList->_location;

    // Add all of the clone children
    for (UINT i = 0; i < rgChildClones.GetCount(); i++)
    {
        CHK(AppendChild(rgChildClones[i]));
    }

    // Add to the list of declarators
    CHK(GetParser()->AddDeclaratorList(this));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   Shader input qualifier is cached so that the parser can process
//              it properly.
//
//              This is done before child verification because the child
//              verification depends on the output of this function.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::PreVerifyChildren()
{
    CHK_START;

    FullySpecifiedTypeNode* pTypeNode = GetFullySpecifiedTypeNode();

    // See if there is a type qualifier (it is optional)
    _typeQualifier = pTypeNode->GetTypeQualifier();
    if (_typeQualifier != NO_QUALIFIER)
    {
        // Global declarations do not restrict the type qualifier, but
        // locals are restricted to only using const.
        if (!IsGlobalScope() && _typeQualifier != CONST_TOK)
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDNONGLOBALQUALIFIER, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);            
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Shader inputs were handled in a pre-pass, so we do not handle them during
    // this pass. They are emitted as members of a struct before the rest of the
    // converted code.
    //
    CHK(OutputTranslation(s_uAllChildrenIdentifiers, /*fUnused*/false, pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputTranslation
//
//  Synopsis:   Helper to output the raw translation of the declarator list.
//
//              If s_uAllChildrenIdentifiers is passed as uIdentifierIndex,
//              this function will output all identifiers in a comma separated
//              list. Otherwise, the identifier at the given index will be 
//              output, with complete type information.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::OutputTranslation(
    UINT uIdentifierIndex,                              // Index of identifier to write out, s_uAllChildrenIdentifiers to do all
    bool fUnused,                                       // Whether to print it as an unused entry for a varying
    __in IStringStream* pOutput                         // Stream to output to
    ) const
{
    CHK_START;

    CHKB(uIdentifierIndex == s_uAllChildrenIdentifiers || uIdentifierIndex < GetIdentifierCount());

    // Don't output anything if we have no identifiers. Struct specifiers have
    // already been reparented via CGLSLParser::TranslateStructDefinitions, so the
    // only remaining declarations (e.g. something like 'const int;') in this 
    // subtree have no meaning and don't compile for HLSL. 
    if (GetIdentifierCount() > 0)
    {
        // Spit out the first declaration starting with the fully specified type
        CHK(GetChild(0)->OutputHLSL(pOutput));

        // Write out a space between the type and identifier
        CHK(pOutput->WriteChar(' '));

        if (uIdentifierIndex == s_uAllChildrenIdentifiers)
        {
            AssertSz(!fUnused, "Cannot print all declarations as unused at once");

            const UINT cChildren = GetChildCount();
            for (UINT i = 1; i < cChildren; i++)
            {
                CHK(GetChild(i)->OutputHLSL(pOutput));
                // Output a comma between each of the entry children (not after the last one though)
                if (i < (cChildren - 1))
                {
                    CHK(pOutput->WriteString(", "));
                }
            }
        }
        else
        {
            InitDeclaratorListEntryNode* pEntry = GetEntry(uIdentifierIndex);

            // Override the identifier name if the entry is being printed as unused. This affects the
            // semantic as well, so we make sure it does not end in a digit, because the HLSL
            // compiler will interpret that as being significant when it comes to multirow variables
            // and packing.
            const char* pszOverride = fUnused ? "unused%d_  " : nullptr;

            // Output the specified indentifier at the given index
            CHK(pEntry->OutputTranslation(pOutput, pszOverride));
        }

        // Write out the semicolon terminating the list, and a newline for each list
        CHK(pOutput->WriteString(";\n"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFullySpecifiedTypeNode
//
//  Synopsis:   Type safe accessor for the fully specified type node.
//
//-----------------------------------------------------------------------------
FullySpecifiedTypeNode* InitDeclaratorListNode::GetFullySpecifiedTypeNode()
{
    // Do some sanity checks
    return GetChild(0)->GetAs<FullySpecifiedTypeNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeSpecifierDescendant
//
//  Synopsis:   Walks through the FullySpecifiedTypeNode to retrieve its
//              TypeSpecifierNode child.
//
//-----------------------------------------------------------------------------
TypeSpecifierNode* InitDeclaratorListNode::GetTypeSpecifierDescendant()
{
    FullySpecifiedTypeNode* pFullType = GetFullySpecifiedTypeNode();
    return pFullType->GetTypeSpecifierNode();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsGlobalScope
//
//  Synopsis:   Test if the node is representing an list declared with global
//              scope. This test is needed because globals are transformed
//              differently to other variables.
//
//              If the parent node is an external declaration then this is
//              a global scope variable.
//
//-----------------------------------------------------------------------------
bool InitDeclaratorListNode::IsGlobalScope()
{
    bool fRegularGlobal = (GetParent()->GetParseNodeType() == ParseNodeType::translationUnit);

    bool fSamplerGlobal = (GetParent()->GetParseNodeType() == ParseNodeType::samplerCollection);

    return (fRegularGlobal || fSamplerGlobal);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetSpecifiedType
//
//  Synopsis:   Return the specified type for the declarator list. This can only
//              be called after the FullySpecifiedTypeNode child (0th child) 
//              has been verified, which will set the specified type.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::GetSpecifiedType(__deref_out GLSLType** ppType) const
{
    AssertSz(_spType != nullptr, "Don't ask for the specified type before it has been verified");

    _spType.CopyTo(ppType);

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetSpecifiedType
//
//  Synopsis:   Each declarator list has a specified type, which comes from
//              the FullySpecifiedTypeNode child. That child sets the specified
//              type for the list when it is verified.
//
//-----------------------------------------------------------------------------
void InitDeclaratorListNode::SetSpecifiedType(__in GLSLType* pType)
{
    Assert(_spType == nullptr);
    _spType = pType;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifier
//
//  Synopsis:   Each entry has an identifier and maybe it has an initializer
//              too. This grabs the identifier part and returns it.
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* InitDeclaratorListNode::GetIdentifier(UINT index) const
{
    InitDeclaratorListEntryNode* pEntry = GetChild(index + 1)->GetAs<InitDeclaratorListEntryNode>();
    return pEntry->GetIdentifierNode();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetEntry
//
//  Synopsis:   Obtain the given entry in the list.
//
//-----------------------------------------------------------------------------
InitDeclaratorListEntryNode* InitDeclaratorListNode::GetEntry(UINT index) const
{
    return GetChild(index + 1)->GetAs<InitDeclaratorListEntryNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVectorCount
//
//  Synopsis:   Count how many vector slots this declaration will eat up.
//
//-----------------------------------------------------------------------------
UINT InitDeclaratorListNode::GetVectorCount() const
{
    UINT uCount = 0;
    UINT uCountPerDecl;
    if (_spType->GetRowCount(&uCountPerDecl) == S_OK)
    {
        for (UINT i = 0; i < GetIdentifierCount(); i++)
        {
            InitDeclaratorListEntryNode* pEntry = GetChild(i + 1)->GetAs<InitDeclaratorListEntryNode>();

            VariableIdentifierNode* pVariableIdentNode = pEntry->GetIdentifierNode();
            TSmartPointer<CVariableIdentifierInfo> spInfo;
            Verify(SUCCEEDED(pVariableIdentNode->GetVariableIdentifierInfo(&spInfo)));

            // Used variables always count towards the max. Unused input struct variables are not 
            // counted here, because in general we won't output them. However, we don't have enough
            // information for vertex varyings to say whether they can be optimized away or not
            // (we need info from the fragment shader). An extra max varying check will be done at link time.
            if (spInfo->IsUsed() || !spInfo->IsInputStructVariable())
            {
                // If we declared an array then adjust accordingly
                if (pEntry->GetDeclarationType() == DeclarationType::arrayDecl)
                {
                    UINT uArrayEntries = 0;
                    if (FAILED(UIntMult(uCountPerDecl, pEntry->GetArraySize(), &uArrayEntries)) || FAILED(UIntAdd(uCount, uArrayEntries, &uCount)))
                    {
                        // If we overflow return UINT_MAX. The remainder of the logic in the pipeline will also overflow and return an error.
                        uCount = UINT_MAX;
                        break;
                    }
                }
                else
                {
                    if (FAILED(UIntAdd(uCount, uCountPerDecl, &uCount)))
                    {
                        // If we overflow return UINT_MAX. The remainder of the logic in the pipeline will also overflow and return an error.
                        uCount = UINT_MAX;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // If we overflow return UINT_MAX. The remainder of the logic in the pipeline will also overflow and return an error.
        uCount = UINT_MAX;
    }

    return uCount;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("InitDeclaratorList");
}
