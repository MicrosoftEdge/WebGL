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
#include "IntConstantNode.hxx"
#include "IStringStream.hxx"
#include "ConstantValue.hxx"
#include "GLSL.tab.h"

MtDefine(IntConstantNode, CGLSLParser, "IntConstantNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
IntConstantNode::IntConstantNode() : 
    _iConstant(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT IntConstantNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int iConstant                               // The index of the symbol
    )
{
    ParseTreeNode::Initialize(pParser);

    _iConstant = iConstant;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Clone the constant node
//
//-----------------------------------------------------------------------------
HRESULT IntConstantNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    IntConstantNode* pOriginalConstant = pOriginal->GetAs<IntConstantNode>();

    _iConstant = pOriginalConstant->_iConstant;

    return S_OK;    
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT IntConstantNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(pOutput->WriteFormat(64, "%d", _iConstant));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetExpressionType
//
//  Synopsis:   Get the type of the node. This is easy for constants.
//
//-----------------------------------------------------------------------------
HRESULT IntConstantNode::VerifySelf()
{
    SetBasicExpressionType(INT_TOK);    
    return S_OK; 
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//-----------------------------------------------------------------------------
HRESULT IntConstantNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{    
    (*pfIsConstantExpression) = true;

    if (pValue != nullptr)
    {
        pValue->SetValue(_iConstant);
    }

    return S_OK;
}
