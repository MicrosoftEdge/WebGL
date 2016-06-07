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
#include "CompoundStatementNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "FunctionDefinitionNode.hxx"
#include "StatementListNode.hxx"

MtDefine(CompoundStatementNode, CGLSLParser, "CompoundStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CompoundStatementNode::CompoundStatementNode() :
    _fDefinesScope(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CompoundStatementNode::Initialize(
    __in CGLSLParser* pParser,                              // The parser that owns the tree
    __in_opt ParseTreeNode* pStatementList,                 // The statement list
    bool fDefinesScope                                      // Whether this node actually defines a scope
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    _fDefinesScope = fDefinesScope;

    // Ensure that all compound statement nodes have an empty statement list. This is to
    // simplify translation code that may need to insert statements into an empty function
    TSmartPointer<StatementListNode> spStatementList;
    if (pStatementList == nullptr)
    {
        CHK(RefCounted<StatementListNode>::Create(GetParser(), /*out*/spStatementList));
        pStatementList = spStatementList;
    }

    CHK(AppendChild(pStatementList));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   Removing the scope from this node if it is in a for statement,
//              and generate a scope id if it still defines a scope.
//
//-----------------------------------------------------------------------------
HRESULT CompoundStatementNode::PreVerifyChildren()
{
    CollectionNodeWithScope::PreVerifyChildren();

    if (_fDefinesScope)
    {
        if (GetParent()->GetParseNodeType() == ParseNodeType::iterationStatement)
        {
            _fDefinesScope = false;
        }
    }

    if (_fDefinesScope)
    {
        SetScopeId(GetParser()->GenerateScopeId());
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT CompoundStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Figure out if we have an entry point block
    bool fEntryPoint = false;
    ParseTreeNode* pParent = GetParent();
    if (pParent->GetParseNodeType() == ParseNodeType::functionDefinition)
    {
        fEntryPoint = pParent->GetAs<FunctionDefinitionNode>()->IsEntryPoint();
    }

    // Wrap in braces
    CHK(pOutput->WriteString("{\n"));
    pOutput->IncreaseIndent();

    if (fEntryPoint)
    {
        CHK(GetParser()->WriteEntryPointBegin(pOutput));
    }

    // If we have a statement list, then we can convert it
    if (GetChildCount() != 0)
    {
        // Convert the statement list
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }

    if (fEntryPoint)
    {
        CHK(GetParser()->WriteEntryPointEnd(pOutput));
    }

    pOutput->DecreaseIndent();
    CHK(pOutput->WriteString("}\n"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT CompoundStatementNode::GetDumpString(__in IStringStream* pOutput)
{
    if (_fDefinesScope)
    {
        return pOutput->WriteFormat(1024, "CompoundStatementNode scope=%d", GetScopeId());
    }
    else
    {
        return pOutput->WriteString("CompoundStatementNode (no scope)");
    }
}
