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
#include "IterationStatementNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "CompoundStatementNode.hxx"

MtDefine(IterationStatementNode, CGLSLParser, "IterationStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
IterationStatementNode::IterationStatementNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT IterationStatementNode::Initialize(
    __in CGLSLParser* pParser,                              // The parser that owns the tree
    __in ParseTreeNode* pChild                              // The statement or compound statement
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    if (pChild->GetParseNodeType() == ParseNodeType::compoundStatement)
    {
        CHK(AppendChild(pChild));
    }
    else
    {
        // If the child of this scope is not a compound_statement_with_scope, that means there
        // is a single simple_statement. In order to generate a consistent parse tree,
        // and have valid insertion points for moving statements, we'll place the 
        // simple_statement in a statement list and wrap that in a compound_statement_with_scope (i.e. fDefinesScope == true)
        // and append the compound_statement_with_scope as the child.
        TSmartPointer<CompoundStatementNode> spCompoundStatement;
        CHK(pParser->WrapSimpleStatementAsCompoundStatement(pChild, /*fDefinesScope*/true, &spCompoundStatement));

        CHK(AppendChild(spCompoundStatement));
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
HRESULT IterationStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    Assert(GetChild(0)->GetParseNodeType() == ParseNodeType::compoundStatement);

    return GetChild(0)->OutputHLSL(pOutput);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT IterationStatementNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("IterationStatementNode");
}
