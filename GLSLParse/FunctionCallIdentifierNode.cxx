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
#include "FunctionCallIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSLParser.hxx"
#include "FunctionIdentifierNode.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"

MtDefine(FunctionCallIdentifierNode, CGLSLParser, "FunctionCallIdentifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionCallIdentifierNode::FunctionCallIdentifierNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallIdentifierNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int symbolIndex,                            // The index of the symbol
    const YYLTYPE &location                     // The location of the symbol
    )
{
    ParseTreeNode::Initialize(pParser);

    _iSymbolIndex = symbolIndex;
    _location = location;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallIdentifierNode::VerifySelf()
{
    CHK_START;

    AssertSz(GetChildCount() == 0, "FunctionCallIdentifierNodes should have no children prior to verification");

    // Until this point, this identifier is ambiguous on whether it is a 
    // function identifier or a typename identifier. However, when this node 
    // gets verified, we can determine whether or not the identifier represents a 
    // typename constructor by seeing whether it resolves as such. This operation 
    // is done by looking at the identifier infos associated with this symbol 
    // in the nearest scope in which this symbol is defined (normal identifier lookup).
    TSmartPointer<ParseTreeNode> spIdentifierChildNode;
    TSmartPointer<CTypeNameIdentifierInfo> spTypeNameInfo;
    if (SUCCEEDED(CollectionNodeWithScope::GetNearestScopeTypeNameInfo(this, _iSymbolIndex, &spTypeNameInfo)))
    {
        // We found the identifier as a typename, now we'll create a TypeNameIdentifierNode and 
        // call VerifySelf on it, since its failure marks the translation
        // as failed due to its error logging.
        CHK(RefCounted<TypeNameIdentifierNode>::Create(
            GetParser(),
            _iSymbolIndex,
            _location,
            /*out*/spIdentifierChildNode
            ));

        // Since we know which typename this identifier refers to set the info now so we 
        // don't do the lookup twice
        TypeNameIdentifierNode* pTypeNameIdNode = spIdentifierChildNode->GetAs<TypeNameIdentifierNode>();
        pTypeNameIdNode->SetTypeNameIdentifierInfo(spTypeNameInfo);
    }
    else
    {
        // If we didn't find the identifier as a typename, this identifier
        // will have to refer to a function name, so we'll create and 
        // add a function identifier node.
        // FunctionCallHeaderWithParametersNode will eventually set
        // the CFunctionIdentifierInfo by doing scope lookup etc. since
        // it has the information needed to do so (argument types, etc.)
        CHK(RefCounted<FunctionIdentifierNode>::Create(
            GetParser(),
            _iSymbolIndex,
            _location,
            /*out*/spIdentifierChildNode
            ));
    }

    CHK(AppendChild(spIdentifierChildNode));
    Assert(GetChild(0) == spIdentifierChildNode);

    // Now verify the child since it didn't exist when this node's children
    // were originally verified.
    CHK(GetChild(0)->VerifyNode());

    AssertSz(GetChildCount() == 1, "FunctionCallIdentifierNodes should have exactly 1 child after successful verification");

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallIdentifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    return GetChild(0)->OutputHLSL(pOutput);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallIdentifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FunctionCallIdentifierNode Symbol=%d", _iSymbolIndex);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFunctionIdentifierChild
//
//  Synopsis:   If the child is a FunctionIdentifierNode, return it. Otherwise
//              return nullptr.
//
//-----------------------------------------------------------------------------
FunctionIdentifierNode* FunctionCallIdentifierNode::GetFunctionIdentifierChild() const
{
    return GetIdentifierChild<FunctionIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeNameIdentifierChild
//
//  Synopsis:   If the child is a TypeNameIdentifierNode, return it. Otherwise
//              return nullptr.
//
//-----------------------------------------------------------------------------
TypeNameIdentifierNode* FunctionCallIdentifierNode::GetTypeNameIdentifierChild() const
{
    return GetIdentifierChild<TypeNameIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierChild
//
//  Synopsis:   Private helper to get the child as an identifier node of a
//              certain type.
//
//-----------------------------------------------------------------------------
template<typename T1>
T1* FunctionCallIdentifierNode::GetIdentifierChild() const
{
    AssertSz(IsVerified(), "It is not correct to attempt to retrieve these children before the node is verified, as that is when it is generated");

    ParseTreeNode* pChild = GetChild(0);
    AssertSz(pChild != nullptr, "The child must have been generated when verified in order to return success");

    if (pChild->GetParseNodeType() == T1::GetClassNodeType())
    {
        return pChild->GetAs<T1>();
    }

    return nullptr;
}
