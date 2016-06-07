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
#include "ExpressionListNode.hxx"
#include "IStringStream.hxx"

MtDefine(ExpressionListNode, CGLSLParser, "ExpressionListNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ExpressionListNode::ExpressionListNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ExpressionListNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pFirst,                     // The first statement
    __in ParseTreeNode* pSecond                     // The second statement
    )
{
    CHK_START;

    CHK(AppendChild(pFirst));
    CHK(AppendChild(pSecond));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Sets the type of the expression list.
//
//-----------------------------------------------------------------------------
HRESULT ExpressionListNode::VerifySelf()
{
    CHK_START;

    UINT cChildren = GetChildCount();
    CHK_VERIFY(cChildren > 0);

    // Expression list takes on the same type as its rightmost (last) child.
    TSmartPointer<GLSLType> spType;
    CHK(GetChild(cChildren - 1)->GetExpressionType(&spType));

    SetExpressionType(spType);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree. The expression list just
//              pushes out the expressions separated by commas.
//
//-----------------------------------------------------------------------------
HRESULT ExpressionListNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    for (UINT i = 0; i < GetChildCount(); i++)
    {
        CHK(GetChild(i)->OutputHLSL(pOutput));

        if (i != GetChildCount() - 1)
        {
            CHK(pOutput->WriteChar(','));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateOrAppend
//
//  Synopsis:   In the rare case that expressions are separated by commas, an
//              expression list is created.
//
//              When the first comma is encountered, this call will see that
//              the pExpression is not a list, so it will add the first
//              two expressions to a list.
//
//              Subsequent commas will see that the pExpression is a list and
//              just add to it.
//
//-----------------------------------------------------------------------------
HRESULT ExpressionListNode::CreateOrAppend(
    __in CGLSLParser* pParser,                      // The parser
    __in ParseTreeNode* pExpression,                // The expression before the comma
    __in ParseTreeNode* pAssignmentExpression,      // The assignment expression after the comma
    __deref_out ParseTreeNode** ppExpressionList    // The expression list
    )
{
    CHK_START;

    TSmartPointer<ExpressionListNode> spExpressionList;
    if (pExpression->GetParseNodeType() == ParseNodeType::expressionList)
    {
        // First argument is a list, so append to it
        CHK(CollectionNode::AppendChild(pExpression, pAssignmentExpression));

        // List is already created, so return it
        spExpressionList = pExpression->GetAs<ExpressionListNode>();
    }
    else
    {
        // First argument is not a list, so create one and return it
        CHK(RefCounted<ExpressionListNode>::Create(pParser, pExpression, pAssignmentExpression, /*out*/spExpressionList));
    }

    spExpressionList.CopyTo(ppExpressionList);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstIndexExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              The value that is assigned is the last expression, so it is the
//              only one tested.
//
//-----------------------------------------------------------------------------
HRESULT ExpressionListNode::IsConstExpression(
    bool fIncludeIndex,                             // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,             // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                 // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    UINT cChildren = GetChildCount();
    CHK_VERIFY(cChildren > 0);

    CHK(GetChild(cChildren - 1)->IsConstExpression(fIncludeIndex, pfIsConstantExpression, pValue));

    CHK_RETURN;
}
