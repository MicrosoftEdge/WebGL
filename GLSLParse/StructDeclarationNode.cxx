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
#include "StructDeclarationNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "VariableIdentifierNode.hxx"
#include "TypeSpecifierNode.hxx"
#include "StructDeclaratorListNode.hxx"
#include "StructDeclaratorNode.hxx"

MtDefine(StructDeclarationNode, CGLSLParser, "StructDeclarationNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructDeclarationNode::StructDeclarationNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationNode::Initialize(
    __in CGLSLParser* pParser,              // The parser that owns the tree
    __in ParseTreeNode* pTypeSpecifier,         // The type specifier for the declaration
    __in ParseTreeNode* pDeclaratorList         // The declarator list
    )
{
    ParseTreeNode::Initialize(pParser);

    CHK_START;

    CHK(AppendChild(pTypeSpecifier));
    CHK(AppendChild(pDeclaratorList));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Creates variable identifier infos for each descendent declarator
//              with the declaration type combined with the declarator arrayness.
//              These variable identifier infos are added to the scope (which
//              will be the declaration list).
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationNode::VerifySelf()
{
    CHK_START;

    TypeSpecifierNode* pTypeSpecifier = GetTypeSpecifierChild();
    TSmartPointer<GLSLType> spDeclType;
    CHK(pTypeSpecifier->GetType(&spDeclType));

    // Sampler types are not supported in structs. Samplers are required to be uniforms, but the
    // shader profiles we compile HLSL against don't support Texture2D and SamplerState in 
    // 'compound types' for globals. Therefore, instead of leaving the developer with and internal linking error,
    // we'll fail to compile with a actionable message in the shader log.
    if (spDeclType->IsSampler2DType() || spDeclType->IsSamplerCubeType())
    {
        // Use the location of the first variable ident for now as declarations generally will not need location
        StructDeclaratorListNode* pStructDeclaratorList = GetDeclaratorListChild();
        StructDeclaratorNode* pStructDeclarator = pStructDeclaratorList->GetChild(0)->GetAs<StructDeclaratorNode>();
        VariableIdentifierNode* pVariableIdentNode = pStructDeclarator->GetVariableIdentifierChild();
        CHK(GetParser()->LogError(&pVariableIdentNode->GetLocation(), E_GLSLERROR_SAMPLERNOTALLOWEDFORSTRUCTS, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    StructDeclaratorListNode* pStructDeclaratorList = GetDeclaratorListChild();
    for (UINT i = 0; i < pStructDeclaratorList->GetChildCount(); i++)
    {
        StructDeclaratorNode* pStructDeclarator = pStructDeclaratorList->GetChild(i)->GetAs<StructDeclaratorNode>();

        // We must construct a full type based on the declaration type and the arrayness of
        // the declarator.
        TSmartPointer<GLSLType> spFullType;
        CHK(GLSLType::CreateFromType(spDeclType, pStructDeclarator->GetArraySize(), &spFullType));

        // Now that we have a full type, we'll add the variable identifier to the id table.
        VariableIdentifierNode* pVariableIdentNode = pStructDeclarator->GetVariableIdentifierChild();

        // Use the default initial value as structs are initialized as a whole, not via individual variables
        ConstantValue initialValue;
        TSmartPointer<CVariableIdentifierInfo> spVariableInfo;
        CHK(GetParser()->UseIdentifierTable()->AddVariableIdentifier(
            pVariableIdentNode,
            spFullType,
            NO_QUALIFIER,                   // No type qualifier on individual member variables
            pTypeSpecifier->GetComputedPrecision(),
            false,                          // Not a parameter declared identifier
            initialValue,
            &spVariableInfo
            ));

        // Variable info was successfully created, commit the info to the variable node now.
        pVariableIdentNode->SetVariableIdentifierInfo(spVariableInfo);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output the children separated by a space and ending with a
//              semicolon and newline.
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    const UINT cChildren = GetChildCount();
    for (UINT i = 0; i < cChildren; i++)
    {
        CHK(GetChild(i)->OutputHLSL(pOutput));

        // Output a space between the children (but not after the last one)
        if (i < cChildren - 1)
        {
            CHK(pOutput->WriteString(" "));
        }
    }

    CHK(pOutput->WriteString(";\n"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Output the node type
//
//-----------------------------------------------------------------------------
HRESULT StructDeclarationNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("StructDeclaration");
}

//+----------------------------------------------------------------------------
//
//  Function:   GetTypeSpecifierChild
//
//  Synopsis:   Retrives the type specifier child.
//
//-----------------------------------------------------------------------------
TypeSpecifierNode* StructDeclarationNode::GetTypeSpecifierChild() const
{
    return GetChild(0)->GetAs<TypeSpecifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDeclaratorListChild
//
//  Synopsis:   Retrives the declarator list child
//
//-----------------------------------------------------------------------------
StructDeclaratorListNode* StructDeclarationNode::GetDeclaratorListChild() const
{
    return GetChild(1)->GetAs<StructDeclaratorListNode>();
}
