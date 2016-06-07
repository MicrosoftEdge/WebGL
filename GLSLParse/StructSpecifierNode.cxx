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
#include "StructSpecifierNode.hxx"
#include "StructDeclarationListNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "StructGLSLType.hxx"

MtDefine(StructSpecifierNode, CGLSLParser, "StructSpecifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructSpecifierNode::StructSpecifierNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::Initialize(
    __in CGLSLParser* pParser,                          // The parser that owns the tree
    const YYLTYPE& location,                            // The location of the struct specifier
    __in_opt ParseTreeNode* pTypeNameIdentifierNode,    // The typename identifier (optional)
    __in ParseTreeNode* pDeclarationList                // The declaration list for the struct specifier
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pTypeNameIdentifierNode));
    CHK(AppendChild(pDeclarationList));

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Creates a struct type from the variable identifier info
//              on the declaration list child (the scope node to which the
//              field names were added as variables).
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::VerifySelf()
{
    CHK_START;

    AssertSz(_spType == nullptr, "StructSpecifierNode::_spType should only be set once during verification");
    AssertSz(_spTypeNameInfo == nullptr, "StructSpecifierNode::_spTypeNameInfo should only be set once during verification");

    // First create the type
    StructDeclarationListNode* pStructDeclarationList = GetStructDeclListChild();
    const CModernArray<TSmartPointer<IIdentifierInfo>>& aryFields = pStructDeclarationList->GetIdsInScope();
    hr = GLSLType::CreateStructTypeFromIdentifierInfoAry(aryFields, &_spType);
    if (hr == E_GLSLERROR_MAXSTRUCTNESTINGEXCEEDED)
    {
        CHK(GetParser()->LogError(&_location, hr, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }
    else
    {
        CHK(hr);
    }

    // If we have a typename that identifies the type add it to the ID table
    TypeNameIdentifierNode* pTypeNameIdentifierNode = GetTypeNameIdentifierChild();
    if (pTypeNameIdentifierNode != nullptr)
    {
        CHK(GetParser()->UseIdentifierTable()->AddTypeNameIdentifier(
            pTypeNameIdentifierNode,
            _spType,
            &_spTypeNameInfo
            ));

        pTypeNameIdentifierNode->SetTypeNameIdentifierInfo(_spTypeNameInfo);
    }
    else
    {
        // Otherwise, whip up an anonymous typename info which will be used
        // when outputting HLSL to facilitate things like default initializer
        CHK(RefCounted<CTypeNameIdentifierInfo>::Create(
            GetParser(),
            _spType,
            /*out*/_spTypeNameInfo
            ));
    }

    _spType->FinalizeTypeWithTypeInfo(_spTypeNameInfo);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Outputs the struct specific strings with the typename and
//              declaration list.
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(pOutput->WriteString("struct "));

    AssertSz(_spTypeNameInfo != nullptr, 
        "We should have set this node's typename info at verification time"
        );

    // Output the HLSL typename based on the typename info created when
    // this node was verified. Even anonymous structs have typename info since
    // we don't want to output anonymous structs in HLSL
    PCSTR pszHLSLName = _spTypeNameInfo->GetHLSLName(0);
    CHKB(pszHLSLName != nullptr);
    CHK(pOutput->WriteString(pszHLSLName));

    CHK(pOutput->WriteString(" {\n"));

    pOutput->IncreaseIndent();
    CHK(GetStructDeclListChild()->OutputHLSL(pOutput));
    pOutput->DecreaseIndent();

    CHK(pOutput->WriteString("}"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Output the node type
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("StructSpecifier");
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Returns the verified GLSLType for the struct specifier
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::GetType(__deref_out GLSLType** ppType) const
{
    AssertSz(IsVerified() && _spType != nullptr, "StructSpecifierNode type should only be retrieved after the node was verified");

    _spType.CopyTo(ppType);

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeNameIdentifierInfo
//
//  Synopsis:   Returns the verified TypeNameIdentifierInfo for the struct specifier
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::GetTypeNameIdentifierInfo(__deref_out CTypeNameIdentifierInfo** ppInfo) const
{
    AssertSz(IsVerified() && _spTypeNameInfo != nullptr, "StructSpecifierNode type info should only be retrieved after the node was verified");
    _spTypeNameInfo.CopyTo(ppInfo);
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetStructDeclListChild
//
//  Synopsis:   Returns the child StructDeclarationListNode
//
//-----------------------------------------------------------------------------
StructDeclarationListNode* StructSpecifierNode::GetStructDeclListChild() const
{
    return GetChild(1)->GetAs<StructDeclarationListNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeNameIdentifierChild
//
//  Synopsis:   Returns the child StructDeclarationListNode
//
//-----------------------------------------------------------------------------
TypeNameIdentifierNode* StructSpecifierNode::GetTypeNameIdentifierChild() const
{
    ParseTreeNode* pFirstChild = GetChild(0);
    return  (pFirstChild != nullptr) ? pFirstChild->GetAs<TypeNameIdentifierNode>() : nullptr;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLFunctions
//
//  Synopsis:   Ask the type name info to output the necessary HLSL functions.
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierNode::OutputHLSLFunctions(__in IStringStream* pOutput)
{
    CHK_START;
    CHK(_spTypeNameInfo->OutputHLSLConstructor(pOutput));
    CHK(_spTypeNameInfo->OutputHLSLEqualsFunction(pOutput));
    CHK_RETURN;
}
