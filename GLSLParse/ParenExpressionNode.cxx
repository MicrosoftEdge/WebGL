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
#include "ParenExpressionNode.hxx"
#include "IStringStream.hxx"

MtDefine(ParenExpressionNode, CGLSLParser, "ParenExpressionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ParenExpressionNode::ParenExpressionNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pExpression             // The unary expression on the left of the assignment
    )
{
    ParseTreeNode::Initialize(pParser);

    return AppendChild(pExpression);
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Initialize from another node, given the collection
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    return AppendChild(rgChildClones[0]);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(pOutput->WriteChar('('));
    CHK(GetChild(0)->OutputHLSL(pOutput));
    CHK(pOutput->WriteChar(')'));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Get the type of the node if it is in an expression tree. This
//              depends on the child expression.
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::VerifySelf()
{
    CHK_START;

    TSmartPointer<GLSLType> spType;
    CHK(GetChild(0)->GetExpressionType(&spType));

    SetExpressionType(spType);

    CHK_RETURN
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstIndexExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::IsConstExpression(
    bool fIncludeIndex,                                 // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,                 // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                     // The value of the constant expression, if desired
    ) const
{
    return GetChild(0)->IsConstExpression(fIncludeIndex, pfIsConstantExpression, pValue);
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLValue
//
//  Synopsis:   Determine if this node represents a valid Lvalue. For an
//              expression in paren, you need an Lvalue as a child.
//
//-----------------------------------------------------------------------------
bool ParenExpressionNode::IsLValue() const
{
    return GetChild(0)->IsLValue();
}
       
//+----------------------------------------------------------------------------
//
//  Function:   GetLValue
//
//  Synopsis:   Get the Lvalue for the subtree.
//
//              If we are an Lvalue, the thing inside the paren can tell us
//              what variable this is.
//
//-----------------------------------------------------------------------------
HRESULT ParenExpressionNode::GetLValue(__out CVariableIdentifierInfo** ppInfo) const
{
    if (IsLValue())
    {
        return GetChild(0)->GetLValue(ppInfo);
    }

    return E_UNEXPECTED;
}

