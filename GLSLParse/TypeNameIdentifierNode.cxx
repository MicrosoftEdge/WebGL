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
#include "TypeNameIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "IStringStream.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSLParser.hxx"

MtDefine(TypeNameIdentifierNode, CGLSLParser, "TypeNameIdentifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
TypeNameIdentifierNode::TypeNameIdentifierNode() :
#if DBG
    _fIsClone(false),
#endif
    _fOutputAsConstructor(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Used to create an anonymous type name id node from an existing
//              type name info. When anonymous structs' typenames are lifted 
//              to a global declaration, we need a typename placeholder where
//              the anonymous struct_specifier originally occurred so that
//              we can output the correct HLSL typename in the declaration.
//
//-----------------------------------------------------------------------------
HRESULT TypeNameIdentifierNode::Initialize(
    __in CGLSLParser* pParser,                      // The parser that owns the tree
    __in CTypeNameIdentifierInfo* pTypeNameInfo     // Anonymous type name info to initialize with
    )
{
    ParseTreeNode::Initialize(pParser);

    _spInfo = pTypeNameInfo;

    // This anonymous identifier node is not created via parsing GLSL source, but is a generated node.
    SetIsGenerated();

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Typename identifiers that are not part of a struct specifier
//              must lookup their corresponding identifier info from its
//              scope and cache that for later use. This identifier info
//              defines what type this identifier represents (as well as
//              how to output this to HLSL).
//
//-----------------------------------------------------------------------------
HRESULT TypeNameIdentifierNode::VerifySelf()
{
    CHK_START;

    // Type name identifiers that are part of a struct_specifier will have the
    // TypeNameIdentifierInfo set on them by the parent (who creates it and adds
    // it to the scope). Other type name identifiers must lookup the type from the
    // tree.
    ParseTreeNode* pParent = GetParent();
    if (pParent->GetParseNodeType() != ParseNodeType::structSpecifier)
    {
        // If we already had our info set, there's nothing to verify
        if (_spInfo == nullptr)
        {
            if (FAILED(CollectionNodeWithScope::GetNearestScopeTypeNameInfo(this, this->GetSymbolIndex(), &_spInfo)))
            {
                // Failing to find the typename info in an ancsestor scope means the typename is not declared
                CHK(GetParser()->LogError(&GetLocation(), E_GLSLERROR_UNDECLAREDTYPENAME, GetText()));
                CHK(E_GLSLERROR_KNOWNERROR);

            }
        }
        else
        {
            // Cloned nodes will have grabbed the identifier info from the original. Generated nodes (for outputting
            // anonymous struct type names in HLSL were initialized with a type info.
            // FunctionCallIdentifierNode must disambiguate between a regular function call and a call to a constructor and also creates
            // the typename id node and sets the typename id info when the FunctionCallIdentifierNode is verified.
            AssertSz(_fIsClone || _fIsGenerated || GetParent()->GetParseNodeType() == ParseNodeType::functionCallIdentifier,
                "FunctionCallIdentifierNode or cloned nodes are the only nodes that should be setting typename id info before verification"
                );
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
HRESULT TypeNameIdentifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    TSmartPointer<CTypeNameIdentifierInfo> spInfo;
    CHK(GetTypeNameIdentifierInfo(&spInfo));

    PCSTR pszHLSLName = (_fOutputAsConstructor) ? spInfo->GetHLSLConstructorName() : spInfo->GetHLSLName(0);
    CHKB(pszHLSLName != nullptr);
    CHK(pOutput->WriteString(pszHLSLName));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeNameIdentifierInfo
//
//  Synopsis:   Get the identifier info for the typename identifier that this 
//              node represents. This function retrieves the cached value that
//              has been previously set.
//
//-----------------------------------------------------------------------------
HRESULT TypeNameIdentifierNode::GetTypeNameIdentifierInfo(
    __deref_out CTypeNameIdentifierInfo** ppInfo    // The info about the identifier
    )
{
    CHK_START;

    CHKB(_spInfo != nullptr);
    _spInfo.CopyTo(ppInfo);

    CHK_RETURN;
}
//+----------------------------------------------------------------------------
//
//  Function:   SetTypeNameIdentifierInfo
//
//  Synopsis:   Set the identifier info for the identifier that this node
//              represents.
//
//-----------------------------------------------------------------------------
void TypeNameIdentifierNode::SetTypeNameIdentifierInfo(__in CTypeNameIdentifierInfo* pInfo)
{
    _spInfo = pInfo;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT TypeNameIdentifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "TypeNameIdentifierNode Symbol=%d", GetSymbolIndex());
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Initialize from an original typename id node. This copies over
//              all the data.
//
//+----------------------------------------------------------------------------
HRESULT TypeNameIdentifierNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    TypeNameIdentifierNode* pOriginalTypeNameIdNode = pOriginal->GetAs<TypeNameIdentifierNode>();

    _spInfo = pOriginalTypeNameIdNode->_spInfo;
    _fOutputAsConstructor = pOriginalTypeNameIdNode->_fOutputAsConstructor;

#if DBG
    _fIsClone = true;
#endif

    return S_OK;
}
