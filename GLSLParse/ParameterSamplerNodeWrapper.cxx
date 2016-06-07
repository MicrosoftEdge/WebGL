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
#include "ParameterSamplerNodeWrapper.hxx"
#include "ParameterDeclarationNode.hxx"
#include "ParameterDeclaratorNode.hxx"
#include "FullySpecifiedTypeNode.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CParameterSamplerNodeWrapper::CParameterSamplerNodeWrapper(
    __in ParseTreeNode* pSamplerNode,               // Node for sampler
    __in ParseTreeNode* pTextureNode                // Node for texture
    )
{
    _pSamplerParam = pSamplerNode->GetAs<ParameterDeclarationNode>();
    _pTextureParam = pTextureNode->GetAs<ParameterDeclarationNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   SetTypeQualifier
//
//  Synopsis:   Set the type qualifier appropriately for the nodes. Parameters
//              do not have type qualifiers, so this is moot for this class.
//
//-----------------------------------------------------------------------------
void CParameterSamplerNodeWrapper::SetTypeQualifier()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierCount
//
//  Synopsis:   Get the count of identifiers in the node. Parameters always
//              have a single identifier.
//
//-----------------------------------------------------------------------------
UINT CParameterSamplerNodeWrapper::GetIdentifierCount()
{
    return 1;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierNode
//
//  Synopsis:   Get the specified identifier from the sampler node.
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* CParameterSamplerNodeWrapper::GetIdentifierNode(UINT uIndex)
{
    Assert(uIndex == 0);

    return _pSamplerParam->GetParameterDeclaratorNode()->GetIdentifierNode();     
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type of the identifiers.
//
//-----------------------------------------------------------------------------
HRESULT CParameterSamplerNodeWrapper::GetType(__deref_out GLSLType** ppType) const
{
    return _pSamplerParam->GetType(ppType);
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateNames
//
//  Synopsis:   Create the names for the identifiers and semantics. Parameters 
//              do not have semantics in the generated HLSL, so they are left
//              alone.
//
//-----------------------------------------------------------------------------
HRESULT CParameterSamplerNodeWrapper::CreateNames(
    __in_z const char* pszCurrentName,              // The current GLSL name
    __inout CMutableString<char> &samplerName,      // Name of sampler var
    __inout CMutableString<char> &textureName,      // Name of texture var
    __inout CMutableString<char> &samplerSem,       // Semantic for sampler
    __inout CMutableString<char> &textureSem,       // Semantic for texture
    UINT uSamplerIndex                              // Index of texture
    )
{
    CHK_START;

    CHK(samplerName.Set(pszCurrentName));
    CHK(textureName.Format(64, "%s_tex", pszCurrentName));

    CHK_RETURN;
}
