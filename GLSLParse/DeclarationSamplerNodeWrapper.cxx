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
#include "DeclarationSamplerNodeWrapper.hxx"
#include "InitDeclaratorListNode.hxx"
#include "FullySpecifiedTypeNode.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CDeclarationSamplerNodeWrapper::CDeclarationSamplerNodeWrapper(
    __in ParseTreeNode* pSamplerNode,               // Node for sampler
    __in ParseTreeNode* pTextureNode                // Node for texture
    )
{
    _pSamplerDeclList = pSamplerNode->GetAs<InitDeclaratorListNode>();
    _pTextureDeclList = pTextureNode->GetAs<InitDeclaratorListNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   SetTypeQualifier
//
//  Synopsis:   Set the type qualifier appropriately for the nodes.
//
//-----------------------------------------------------------------------------
void CDeclarationSamplerNodeWrapper::SetTypeQualifier()
{
    // Set the qualifier to HLSL_UNIFORM - the original node was a uniform, but these are implementation detail
    // variables so they should not be turning up in lists of uniforms. NO_QUALIFIER will not work, because that
    // will end up making a static and you cannot have static textures and samplers.
    _pSamplerDeclList->GetFullySpecifiedTypeNode()->SetTypeQualifier(HLSL_UNIFORM);
    _pTextureDeclList->GetFullySpecifiedTypeNode()->SetTypeQualifier(HLSL_UNIFORM);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierCount
//
//  Synopsis:   Get the count of identifiers in the node.
//
//-----------------------------------------------------------------------------
UINT CDeclarationSamplerNodeWrapper::GetIdentifierCount()
{
    return _pSamplerDeclList->GetIdentifierCount();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierNode
//
//  Synopsis:   Get the specified identifier from the sampler node.
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* CDeclarationSamplerNodeWrapper::GetIdentifierNode(UINT uIndex)
{
    return _pSamplerDeclList->GetIdentifier(uIndex);     
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Get the type of the identifiers.
//
//-----------------------------------------------------------------------------
HRESULT CDeclarationSamplerNodeWrapper::GetType(__deref_out GLSLType** ppType) const
{
    return _pSamplerDeclList->GetSpecifiedType(ppType);
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateNames
//
//  Synopsis:   Create the names for the identifiers
//
//-----------------------------------------------------------------------------
HRESULT CDeclarationSamplerNodeWrapper::CreateNames(
    __in_z const char* pszCurrentName,              // The current GLSL name
    __inout CMutableString<char> &samplerName,      // Name of sampler var
    __inout CMutableString<char> &textureName,      // Name of texture var
    __inout CMutableString<char> &samplerSem,       // Semantic for sampler
    __inout CMutableString<char> &textureSem,       // Semantic for texture
    UINT uSamplerIndex                              // Index of texture
    )
{
    CHK_START;

    CHK(samplerName.Format(64, "sampler_%d", uSamplerIndex));
    CHK(textureName.Format(64, "texture_%d", uSamplerIndex));
    CHK(samplerSem.Format(64, "register(s[%d])", uSamplerIndex));
    CHK(textureSem.Format(64, "register(t[%d])", uSamplerIndex));

    CHK_RETURN;
}
