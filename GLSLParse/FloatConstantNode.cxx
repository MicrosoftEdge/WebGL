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
#include "FloatConstantNode.hxx"
#include "IStringStream.hxx"
#include "ConstantValue.hxx"
#include "GLSL.tab.h"
#include <float.h>

MtDefine(FloatConstantNode, CGLSLParser, "FloatConstantNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FloatConstantNode::FloatConstantNode() : 
    _constant(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FloatConstantNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    double constant                             // The index of the symbol
    )
{
    Assert(constant >= 0.0);

    ParseTreeNode::Initialize(pParser);

    _constant = constant;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FloatConstantNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    if (_finite(_constant) == 0)
    {
        CHK(pOutput->WriteString("0.0"));
    }
    else
    {
        CHK(pOutput->WriteFormat(128, "%e", _constant));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Get the type of the node. This is easy for constants.
//
//-----------------------------------------------------------------------------
HRESULT FloatConstantNode::VerifySelf()
{
    SetBasicExpressionType(FLOAT_TOK);
    
    return S_OK; 
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   All constant float expressions need to terminate with this
//              node since it is the only one with a value in it.
//
//-----------------------------------------------------------------------------
HRESULT FloatConstantNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{    
    (*pfIsConstantExpression) = true;

    if (pValue != nullptr)
    {
        pValue->SetValue(_constant);
    }

    return S_OK;
}
