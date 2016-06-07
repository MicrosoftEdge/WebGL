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
#include "SelectionRestStatementNode.hxx"
#include "IStringStream.hxx"

MtDefine(SelectionRestStatementNode, CGLSLParser, "SelectionRestStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
SelectionRestStatementNode::SelectionRestStatementNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT SelectionRestStatementNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pTrueStatement,         // Expression run when if statement is true
    __in_opt ParseTreeNode* pElseStatement      // Expression for when else is hit (not always present)
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pTrueStatement));

    if (pElseStatement != nullptr)
    {
        CHK(AppendChild(pElseStatement));
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
HRESULT SelectionRestStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Output the true statement
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // See if there was an else clause
    if (GetChildCount() == 2)
    {
        CHK(pOutput->WriteString("else\n"));
        CHK(GetChild(1)->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT SelectionRestStatementNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("SelectionRestStatementNode");
}
