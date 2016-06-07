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
#include "ParameterDeclarationNode.hxx"
#include "ParameterDeclaratorNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "GLSLType.hxx"
#include "RefCounted.hxx"
#include "GLSL.tab.h"

MtDefine(ParameterDeclarationNode, CGLSLParser, "ParameterDeclarationNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ParameterDeclarationNode::ParameterDeclarationNode() : 
    _uHLSLNameIndex(0),
    _paramQual(EMPTY_TOK),
    _typeQual(NO_QUALIFIER)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int paramQual,                                      // The parameter qualifier (in, out, etc)
    __in ParseTreeNode* pDecl,                          // The parameter declarator
    int typeQual,                                       // The type qualifier (const, attribute, etc)
    const YYLTYPE &location                             // The location of the declaration
    )
{
    ParseTreeNode::Initialize(pParser);

    _paramQual = paramQual;
    _typeQual = typeQual;
    _location = location;
        
    return AppendChild(pDecl);
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Clone the parameter declaration
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    // Add all of the clone children
    for (UINT i = 0; i < rgChildClones.GetCount(); i++)
    {
        CHK(AppendChild(rgChildClones[i]));
    }

    ParameterDeclarationNode* pOriginalDecl = pOriginalColl->GetAs<ParameterDeclarationNode>();
    _paramQual = pOriginalDecl->_paramQual;
    _typeQual = pOriginalDecl->_typeQual;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Output the type qualifier if we have one
    if (_typeQual != NO_QUALIFIER)
    {
        // Our verification phase should have done the right thing here
        Assert(_typeQual == CONST_TOK);

        CHK(pOutput->WriteString("const "));
    }

    // Output the parameter qualifier if we have one
    switch (_paramQual)
    {
    case OUT_TOK:
    case INOUT_TOK:
        // The HLSL compiler will fail if it encounters 'out' parameters that are not fully
        // assigned to. The GLSL spec says that this results in undefined behavior. To get
        // deterministic behavior and not fail at the linking stage, we'll translate 'out' to
        // 'inout' which causes the parameter to be initialized and avoids the HLSL compiler error.
        CHK(pOutput->WriteString("inout "));
        break;

    default:
        AssertSz(_paramQual == IN_TOK || _paramQual == EMPTY_TOK, "Unexpected parameter qualifier");
        break;
    }

    // Just translate the declarator
    CHK(GetChild(0)->OutputHLSL(pOutput));

    CHK_RETURN;
}
    
//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verify that the node is correctly formed.
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::VerifySelf()
{
    CHK_START;

    // The type qualifer has to be either nothing or const
    if (_typeQual != NO_QUALIFIER)
    {
        if (_typeQual == CONST_TOK)
        {
            if (_paramQual == OUT_TOK || _paramQual == INOUT_TOK)
            {
                // out and inout cannot be const
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);             
            }
        }
        else
        {
            // const is the only valid qualifier for parameters
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);             
        }
    }

    // If we have a nonzero HLSL name index, then this is a node that was created by cloning. We 
    // don't want to add the identifier or set the info because these things won't work or are already done.
    if (_uHLSLNameIndex == 0)
    {
        // Add a variable if an identifier is there
        ParameterDeclaratorNode* pDecl = GetParameterDeclaratorNode();
        VariableIdentifierNode* pIdentifierNode = pDecl->GetIdentifierNode();
        if (pIdentifierNode != nullptr)
        {
            TSmartPointer<GLSLType> spType;
            CHK(pDecl->GetType(&spType));

            // We add an identifier to the table, because this will create our info. The info is where
            // the logic for creating the HLSL name will ultimately connect, which is why we will add
            // a variable even in cases where it won't get used (like in a function declaration).
            TSmartPointer<CVariableIdentifierInfo> spNewInfo;
            CHK(GetParser()->UseIdentifierTable()->AddVariableIdentifier(
                pIdentifierNode,
                spType,
                _typeQual,
                pDecl->GetTypeSpecifierNode()->GetComputedPrecision(),
                true,                   // This is a parameter declaration
                ConstantValue(),
                &spNewInfo
                ));

            // Set the info for the identifier now that it is created
            pIdentifierNode->SetVariableIdentifierInfo(spNewInfo);
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetParameterDeclaratorNode
//
//  Synopsis:   Type safe accessor for the parameter declarator child.
//
//-----------------------------------------------------------------------------
ParameterDeclaratorNode* ParameterDeclarationNode::GetParameterDeclaratorNode()
{
    return GetChild(0)->GetAs<ParameterDeclaratorNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type information for the type of this declaration.
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::GetType(__deref_out GLSLType** ppType)
{   
    return GetParameterDeclaratorNode()->GetType(ppType);
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLNameIndex
//
//  Synopsis:   Set whether to use the sampler or texture HLSL name for
//              identifiers in this expression. For this node, that will affect
//              the semantic that is used.
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclarationNode::SetHLSLNameIndex(UINT uIndex)
{
    _uHLSLNameIndex = uIndex;

    return CollectionNode::SetHLSLNameIndex(uIndex);
}

