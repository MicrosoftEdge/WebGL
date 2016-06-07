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
#include "FunctionDefinitionNode.hxx"
#include "FunctionPrototypeNode.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "FunctionHeaderNode.hxx"
#include "ReturnStatementNode.hxx"
#include "IdentifierNodeBase.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"

MtDefine(FunctionDefinitionNode, CGLSLParser, "FunctionDefinitionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionDefinitionNode::FunctionDefinitionNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionDefinitionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pFuncProto,             // The function prototype
    __in ParseTreeNode* pCompound,              // The compound statement
    const YYLTYPE &location                     // The location of the declarator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK_VERIFY(pCompound->GetParseNodeType() == ParseNodeType::compoundStatement);

    CHK(AppendChild(pFuncProto));
    CHK(AppendChild(pCompound));

    // Set the scope ID of the prototype here. This keeps the scope ID the same
    // as when it was set on this node in the past.
    FunctionPrototypeNode* pFuncProto = GetChild(0)->GetAs<FunctionPrototypeNode>();
    pFuncProto->SetScopeId(GetParser()->GenerateScopeId());

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionDefinitionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Convert the prototype
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // The function prototype ends with a '(' - to make the output somewhat
    // more readable, we insert a newline after it here. This used to be
    // a part of the prototype, but prototypes are also in forward declarations
    // and they need to handle newlines in a different way.
    CHK(pOutput->WriteString("\n"));

    // Convert the compound statement
    CHK(GetChild(1)->OutputHLSL(pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsScopeNode
//
//  Synopsis:   Tests if this node forms a scope for the given identifier.
//
//              The function prototype owns the actual scope (since it is
//              common between the declaration and definition). Function
//              identifiers that are the function name in the prototype are
//              not scoped by the prototype so return false. Other function
//              identifiers (e.g. function calls) are scoped by the prototype.
//              If we are asking ourselves don't redirect back to
//              the prototype node - we're asking for the real scope.
//
//-----------------------------------------------------------------------------
bool FunctionDefinitionNode::IsScopeNode(
    __in ParseTreeNode* pTreeNode,              // Node that scope query is for
    __deref_out_opt ParseTreeNode** ppActual    // Tree node that is the actual scope
    )
{
    ParseNodeType::Enum nodeType = pTreeNode->GetParseNodeType();
    ParseNodeType::Enum parentNodeType = pTreeNode->GetParent()->GetParseNodeType();
    if (nodeType == ParseNodeType::functionIdentifier && parentNodeType == ParseNodeType::functionHeader)
    {
        // Function identifiers that are a child of a function header (e.g. the function name 
        // in a declaration or definition), skip the definition and prototype scope.
        *ppActual = nullptr;
        return false;
    }
    else if (nodeType == ParseNodeType::functionDefinition)
    {
        Assert(pTreeNode == this);
        *ppActual = nullptr;
        return false;
    }
    else
    {
        (*ppActual) = GetChild(0);
        return true;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   AddReturnStatement
//
//  Synopsis:   Tests if this node forms a scope for the given identifier. We
//              override this here because we want function definitions to
//              define a scope for variables but not for functions.
//
//-----------------------------------------------------------------------------
HRESULT FunctionDefinitionNode::AddReturnStatement(ReturnStatementNode* pNode)
{
    return _rgReturnStatements.Add(pNode);
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   This function ensures that the return statements match the 
//              return type of the node.
//
//              It also marks the function as having a definition.
//
//-----------------------------------------------------------------------------
HRESULT FunctionDefinitionNode::VerifySelf()
{
    CHK_START;

    FunctionPrototypeNode* pFuncProto = GetChild(0)->GetAs<FunctionPrototypeNode>();
    FunctionHeaderWithParametersNode* pHeaderWithParam = pFuncProto->GetChild(0)->GetAs<FunctionHeaderWithParametersNode>();
    
    // We need an identifier info to continue
    Assert(pHeaderWithParam->UseIdentifierInfo() != nullptr);

    if (pHeaderWithParam->UseIdentifierInfo()->IsDefined())
    {
        // This function has already been defined, and redefining is not allowed
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_FUNCTIONREDEFINITION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Mark as defined and declared
    pHeaderWithParam->UseIdentifierInfo()->SetDefined(this);    
    pHeaderWithParam->UseIdentifierInfo()->SetDeclared();

    if (_rgReturnStatements.GetCount() != 0)
    {
        FunctionHeaderNode* pHeader = pHeaderWithParam->GetChild(0)->GetAs<FunctionHeaderNode>();

        // Get the basic type from the header
        TSmartPointer<GLSLType> spReturnType;
        CHK(pHeader->GetType(&spReturnType));

        for (UINT i = 0; i < _rgReturnStatements.GetCount(); i++)
        {
            TSmartPointer<GLSLType> spExprType;
            CHK(_rgReturnStatements[i]->GetExpressionType(&spExprType));

            if (!spReturnType->IsEqualType(spExprType))
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLERETURN, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }
    }

    if (IsEntryPoint())
    {
        GetParser()->SetEntryPointNode(this);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEntryPoint
//
//  Synopsis:   Returns true if this is the function definition node for the
//              main function.
//
//-----------------------------------------------------------------------------
bool FunctionDefinitionNode::IsEntryPoint()
{
    FunctionPrototypeNode* pProto = GetChild(0)->GetAs<FunctionPrototypeNode>();
    FunctionHeaderWithParametersNode* pHeader = pProto->GetChild(0)->GetAs<FunctionHeaderWithParametersNode>();

    return pHeader->IsEntryPoint();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionDefinitionNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("FunctionDefinitionNode");
}
