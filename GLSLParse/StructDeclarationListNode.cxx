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
#include "StructDeclarationListNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"

MtDefine(StructDeclarationListNode, CGLSLParser, "StructDeclarationListNode");

StructDeclarationListNode::StructDeclarationListNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationListNode::Initialize(
    __in CGLSLParser* pParser,              // The parser that owns the tree
    __in ParseTreeNode* pDeclaration            // The declaration
    )
{
    ParseTreeNode::Initialize(pParser);

    // This node will be the scope for the struct members. Generate and store
    // a scope id upon creation.
    SetScopeId(GetParser()->GenerateScopeId());
    return AppendChild(pDeclaration);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Writes and indent then outputs each declaration child
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationListNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    for (UINT i = 0; i < GetChildCount(); i++)
    {
        CHK(pOutput->WriteIndent());
        CHK(GetChild(i)->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Output the node name
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationListNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("StructDeclarationList");
}

