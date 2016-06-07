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
#include "FunctionHeaderNode.hxx"
#include "IStringStream.hxx"
#include "VariableIdentifierNode.hxx"
#include "FullySpecifiedTypeNode.hxx"
#include "IdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "ParameterDeclarationNode.hxx"
#include "GLSL.tab.h"

MtDefine(FunctionHeaderNode, CGLSLParser, "FunctionHeaderNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionHeaderNode::FunctionHeaderNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pType,                  // The fully specified type
    __in ParseTreeNode* pIdNode                 // The identifier
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pType));
    CHK(AppendChild(pIdNode));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // The parent node (header with param) knows if this is an entry point
    FunctionHeaderWithParametersNode* pHeaderWithParam = GetParent()->GetAs<FunctionHeaderWithParametersNode>();

    if (pHeaderWithParam->IsEntryPoint())
    {
        // The entry point is defined very deliberately
        if (GetParser()->GetShaderType() == GLSLShaderType::Vertex)
        {
            CHK(pOutput->WriteString("void main(in VSInput vsInputArg, out PSInput psInputOut"));
        }
        else
        {
            CHK(pOutput->WriteString("void main(in PSInput psInputArg, out PSOutput psOutputOut"));
        }
    }
    else
    {
        // Output the type
        CHK(GetChild(0)->OutputHLSL(pOutput));

        // Add a space before the identifier
        CHK(pOutput->WriteChar(' '));

        // Now the identifier
        CHK(GetChild(1)->OutputHLSL(pOutput));

        // And the paren
        CHK(pOutput->WriteChar('('));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierNode
//
//  Synopsis:   Type safe accessor for the identifier child node
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* FunctionHeaderNode::GetIdentifierNode()
{
    return GetChild(1)->GetAs<VariableIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFullySpecifiedTypeNode
//
//  Synopsis:   Type safe accessor for the type child node
//
//-----------------------------------------------------------------------------
FullySpecifiedTypeNode* FunctionHeaderNode::GetFullySpecifiedTypeNode() const
{
    return GetChild(0)->GetAs<FullySpecifiedTypeNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type returned from the header
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderNode::GetType(__deref_out GLSLType** ppType) const
{
    return GetFullySpecifiedTypeNode()->GetType(ppType);
}
