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
#include "StructDeclaratorListNode.hxx"
#include "IStringStream.hxx"

MtDefine(StructDeclaratorListNode, CGLSLParser, "StructDeclaratorListNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructDeclaratorListNode::StructDeclaratorListNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorListNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pDeclarator             // The first declarator
    )
{
    ParseTreeNode::Initialize(pParser);

    return AppendChild(pDeclarator);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output the declarator children as a comma separated string
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorListNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    const UINT cChildren = GetChildCount();
    for (UINT i = 0; i < cChildren; i++)
    {
        CHK(GetChild(i)->OutputHLSL(pOutput));

        // Output a comma with space between the children, but not after the last one
        if (i < cChildren - 1)
        {
            CHK(pOutput->WriteString(", "));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Output the node type
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorListNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("StructDeclaratorList");
}
