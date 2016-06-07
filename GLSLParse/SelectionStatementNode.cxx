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
#include "SelectionStatementNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"

MtDefine(SelectionStatementNode, CGLSLParser, "SelectionStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
SelectionStatementNode::SelectionStatementNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT SelectionStatementNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pExpr,                  // Expression that is tested
    __in ParseTreeNode* pRest,                  // The rest of the statement
    const YYLTYPE &location                     // The location of the statement
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    _location = location;

    CHK(AppendChild(pExpr));
    CHK(AppendChild(pRest));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Ensure that the condition expression is a boolean.
//
//-----------------------------------------------------------------------------
HRESULT SelectionStatementNode::VerifySelf()
{
    CHK_START;

    // First get the expression types of the test, as well as the true and false results
    TSmartPointer<GLSLType> spExprType;
    CHK(GetChild(0)->GetExpressionType(&spExprType));

    // Test should be a boolean
    int exprType;
    if (FAILED(spExprType->GetBasicType(&exprType)) || exprType != BOOL_TOK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDIFEXPRESSION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);             
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
HRESULT SelectionStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(pOutput->WriteString("if ("));
    CHK(GetChild(0)->OutputHLSL(pOutput));
    CHK(pOutput->WriteString(")"));
    CHK(GetChild(1)->OutputHLSL(pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT SelectionStatementNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("SelectionStatementNode");
}
