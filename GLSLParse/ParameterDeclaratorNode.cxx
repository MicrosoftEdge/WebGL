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
#include "ParameterDeclaratorNode.hxx"
#include "IStringStream.hxx"
#include "VariableIdentifierNode.hxx"
#include "RefCounted.hxx"
#include "VerificationHelpers.hxx"
#include "GLSLParser.hxx"

MtDefine(ParameterDeclaratorNode, CGLSLParser, "ParameterDeclaratorNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ParameterDeclaratorNode::ParameterDeclaratorNode() : 
    _arraySize(-1)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pSpec,                  // The type specifier
    __in_opt ParseTreeNode* pIdentifier,        // The identifier (this is optional)
    __in_opt ParseTreeNode* pArraySizeExpr,     // The array size expression (this is optional)
    const YYLTYPE &location                     // The location of the declarator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    _location = location;

    // Some of these pointers might be nullptr, but the rest of the code knows how to handle that
    CHK(AppendChild(pSpec));
    CHK(AppendChild(pIdentifier));
    CHK(AppendChild(pArraySizeExpr));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Clone the parameter declaration
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::InitializeAsCloneCollection(
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

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verify that the thing being indexed actually can be indexed -
//              right now that just means vector types and things declared
//              as arrays.
//
//              This also verifies that the index is valid if the index is
//              a constant expression.
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::VerifySelf()
{
    CHK_START;

    ParseTreeNode* pConstantExpression = GetArraySizeExprNode();
    if (pConstantExpression != nullptr)
    {
        // An array size was specified, so ensure if is an int constant
        CHK(VerificationHelpers::VerifyArrayConstant(
            pConstantExpression,
            GetParser(),
            &_location,
            &_arraySize
            ));
    }

    TSmartPointer<GLSLType> spTypeSpecified;
    CHK_VERIFY(GetTypeSpecifierNode()->GetType(&spTypeSpecified) == S_OK);

    // Construct the type - parameter declarators have both the type and arrayness
    // encoded in them.
    CHK(GLSLType::CreateFromType(
        spTypeSpecified,
        _arraySize, 
        &_spType
        ));

    CHK_RETURN;
}
    
//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(GetChild(0)->OutputHLSL(pOutput));

    // Output the translation of the identifier if it existed in the original source,
    // or a placeholder unique identifier as HLSL requires parameters to be named.
    CHK(pOutput->WriteChar(' '));
    if (GetChild(1) != nullptr)
    {
        CHK(GetChild(1)->OutputHLSL(pOutput));
    }
    else
    {
        CHK(pOutput->WriteFormat(128, "anonarg_%d", GetParser()->GenerateIdentifierId()));
    }

    // Output the array size if it has one
    if (GetArraySizeExprNode() != nullptr)
    {
        CHK(pOutput->WriteChar('['));
        CHK(pOutput->WriteFormat(128, "%d", _arraySize));
        CHK(pOutput->WriteChar(']'));
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
TypeSpecifierNode* ParameterDeclaratorNode::GetTypeSpecifierNode() const
{
    return GetChild(0)->GetAs<TypeSpecifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierNode
//
//  Synopsis:   Type safe accessor for the identifier child
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* ParameterDeclaratorNode::GetIdentifierNode() const
{
    if (GetChild(1) != nullptr)
    {
        return GetChild(1)->GetAs<VariableIdentifierNode>();
    }
    else
    {
        return nullptr;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   GetArraySizeExprNode
//
//  Synopsis:   Accessor for the array size expression child
//
//-----------------------------------------------------------------------------
ParseTreeNode* ParameterDeclaratorNode::GetArraySizeExprNode() const
{
    return GetChild(2);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type information for the type of this declaration.
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::GetType(__deref_out GLSLType** ppType)
{
    if (_spType != nullptr)
    {
        _spType.CopyTo(ppType);
        return S_OK;
    }
    else
    {
        return E_UNEXPECTED;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT ParameterDeclaratorNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "ParameterDeclarator ArraySize=%d", static_cast<int>(_arraySize));
}
