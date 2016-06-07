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
#include "FunctionPrototypeNode.hxx"
#include "IStringStream.hxx"
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "ParameterDeclarationNode.hxx"
#include "TypeSpecifierNode.hxx"
#include "GLSLParser.hxx"
#include "VariableIdentifierNode.hxx"
#include "GLSL.tab.h"

MtDefine(FunctionPrototypeNode, CGLSLParser, "FunctionPrototypeNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionPrototypeNode::FunctionPrototypeNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pDecl                   // The function declarator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    Assert(pDecl->GetParseNodeType() == ParseNodeType::functionHeaderWithParameters);

    CHK(AppendChild(pDecl));

    CHK_RETURN;
}
    
//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   Create a scope id if one has not already been created.
//
//              Forward declarations have no function definition to set this ID
//              so it is done here.
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeNode::PreVerifyChildren()
{
    CollectionNodeWithScope::PreVerifyChildren();

    if (GetScopeId() == -1)
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
HRESULT FunctionPrototypeNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Translate the declarator
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // Function prototypes end with a closing right paren. The forward
    // declaration node will output a semicolon and the function definition will
    // output a newline appropriately after this.
    CHK(pOutput->WriteString(")"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsScopeNode
//
//  Synopsis:   Tests if this node forms a scope for the given identifier. We
//              override this here because we want both function definitions
//              and function declarations to define a scope for variables but 
//              not for the defined/declared function name itself. 
//              The function prototype is common to both of these.
//
//              If we do not do this conditionally, then the identifier for the 
//              definition/declaration ends up in the scope of its own 
//              function prototype node (e.g. void foo() {} won't put the 
//              identifier in the global scope).
//
//-----------------------------------------------------------------------------
bool FunctionPrototypeNode::IsScopeNode(
    __in ParseTreeNode* pTreeNode,              // Node that scope query is for
    __deref_out_opt ParseTreeNode** ppActual    // Tree node that is the actual scope
    )
{
    ParseNodeType::Enum nodeType = pTreeNode->GetParseNodeType();
    ParseNodeType::Enum parentNodeType = pTreeNode->GetParent()->GetParseNodeType();
    if (nodeType == ParseNodeType::functionIdentifier && parentNodeType == ParseNodeType::functionHeader)
    {
        *ppActual = nullptr;
        return false;
    }
    else
    {
        (*ppActual) = this;
        return true;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FunctionPrototypeNode scope=%d", GetScopeId());
}
