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
#include "FunctionCallGenericNode.hxx"
#include "IStringStream.hxx"
#include "GLSLType.hxx"

MtDefine(FunctionCallGenericNode, CGLSLParser, "FunctionCallGenericNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionCallGenericNode::FunctionCallGenericNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallGenericNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pHeaderWithParam        // Function call header with params
    )
{
    ParseTreeNode::Initialize(pParser);

    return AppendChild(pHeaderWithParam);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallGenericNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Output the header with param
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // And the paren
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
HRESULT FunctionCallGenericNode::VerifySelf()
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
HRESULT FunctionCallGenericNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{
    return GetChild(0)->IsConstExpression(fIncludeIndex, pfIsConstantExpression, pValue);
}
