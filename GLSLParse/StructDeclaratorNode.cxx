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
#include "StructDeclaratorNode.hxx"
#include "GLSLParser.hxx"
#include "IStringStream.hxx"
#include "VariableIdentifierNode.hxx"
#include "VerificationHelpers.hxx"

MtDefine(StructDeclaratorNode, CGLSLParser, "StructDeclaratorNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructDeclaratorNode::StructDeclaratorNode() : 
    _arraySize(-1)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pVariableIdentifierNode,// The variable identifier
    __in_opt ParseTreeNode* pConstantExpression // The constant expression portion of an array declarator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pVariableIdentifierNode));

    if (pConstantExpression != nullptr)
    {
        CHK(AppendChild(pConstantExpression));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Validates the array expression is constant and sets the array
//              size on the member variable
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorNode::VerifySelf()
{
    CHK_START;

    ParseTreeNode* pConstantExpression = GetConstantExpressionChild();
    if (pConstantExpression != nullptr)
    {
        // An array size was specified, so ensure that it is an int constant
        CHK(VerificationHelpers::VerifyArrayConstant(
            pConstantExpression,
            GetParser(),
            &GetVariableIdentifierChild()->GetLocation(),
            &_arraySize
            ));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output the variable child along with the array size if it
//              was specified.
//
//-----------------------------------------------------------------------------
HRESULT StructDeclaratorNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Output variable name first
    CHK(GetVariableIdentifierChild()->OutputHLSL(pOutput));

    // Output arrayness if this is an array declarator
    if (_arraySize != -1)
    {
        CHK(pOutput->WriteChar('['));
        CHK(pOutput->WriteFormat(64, "%d", _arraySize));
        CHK(pOutput->WriteChar(']'));
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
HRESULT StructDeclaratorNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "StructDeclarator arraySize=%d", _arraySize);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetArraySize
//
//  Synopsis:   Retrieves the verified array size
//
//-----------------------------------------------------------------------------
int StructDeclaratorNode::GetArraySize() const
{
    AssertSz(IsVerified(), "array size of struct declarator only valid after it has been verified");

    return _arraySize;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVariableIdentifierChild
//
//  Synopsis:   Returns the VariableIdentifierNode child
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* StructDeclaratorNode::GetVariableIdentifierChild() const
{
    return GetChild(0)->GetAs<VariableIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetConstantExpressionChild
//
//  Synopsis:   If there is a constant expression (i.e. the declarator has
//              an array declarartion) return it. Otherwise return nullptr.
//
//-----------------------------------------------------------------------------
ParseTreeNode* StructDeclaratorNode::GetConstantExpressionChild() const
{
    return (GetChildCount() >= 2) ? GetChild(1) : nullptr;
}
