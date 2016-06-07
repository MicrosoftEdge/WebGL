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
#include "ReturnStatementNode.hxx"
#include "FunctionDefinitionNode.hxx"
#include "IStringStream.hxx"
#include "GLSLType.hxx"
#include "RefCounted.hxx"
#include "GLSLParser.hxx"
#include "GLSL.tab.h"

MtDefine(ReturnStatementNode, CGLSLParser, "ReturnStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ReturnStatementNode::ReturnStatementNode() : 
    _pOwningFunction(nullptr)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ReturnStatementNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in_opt ParseTreeNode* pExpr               // Expression that is returned (can be null)
    )
{
    ParseTreeNode::Initialize(pParser);

    return AppendChild(pExpr);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ReturnStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // We inject special code before we exit main
    if (_pOwningFunction->IsEntryPoint())
    {
        CHK(GetParser()->WriteEntryPointEnd(pOutput));
    }

    CHK(pOutput->WriteString("return"));

    if (GetChild(0) != nullptr)
    {
        CHK(pOutput->WriteChar(' '));
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }

    CHK(pOutput->WriteString(";\n"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   The type of the return statement needs to match the return
//              type of the function it is in.
//
//              The function definition node does its type verification after 
//              the return statement (it is in the function subtree). So this 
//              calculates the type of the expression and gives it to the 
//              function definition for it to check.
//
//-----------------------------------------------------------------------------
HRESULT ReturnStatementNode::VerifySelf()
{
    CHK_START;

    // First calculate and set our expression type based on the type of
    // the child expression. No expression matches with void.
    TSmartPointer<GLSLType> spType;
    if (GetChild(0) != nullptr)
    {
        CHK(GetChild(0)->GetExpressionType(&spType));
    }
    else
    {
        CHK(GLSLType::CreateFromBasicTypeToken(VOID_TOK, &spType));
    }

    SetExpressionType(spType);

    // Go through the parent nodes until the first function definition is
    // found. Then add this return statement to the list of those that
    // are in the function (there can be more than one).
    ParseTreeNode* pWalk = this;
    while (pWalk != nullptr)
    {
        if (pWalk->GetParseNodeType() == ParseNodeType::functionDefinition)
        {
            break;
        }

        pWalk = pWalk->GetParent();
    }

    // Return statements should not find their way into being outside of
    // a function definition (based on the grammar).
    Assert(pWalk != nullptr);
    CHKB(pWalk != nullptr);

    _pOwningFunction = pWalk->GetAs<FunctionDefinitionNode>();
    CHK(_pOwningFunction->AddReturnStatement(this));

    CHK_RETURN;
}
