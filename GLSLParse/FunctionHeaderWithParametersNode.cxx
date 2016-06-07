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
#include "FunctionHeaderWithParametersNode.hxx"
#include "ParameterDeclarationNode.hxx"
#include "FunctionIdentifierNode.hxx"
#include "TypeSpecifierNode.hxx"
#include "IStringStream.hxx"
#include "IdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "FunctionHeaderNode.hxx"
#include "FunctionIdentifierInfo.hxx"

MtDefine(FunctionHeaderWithParametersNode, CGLSLParser, "FunctionHeaderWithParametersNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionHeaderWithParametersNode::FunctionHeaderWithParametersNode() : 
    _fEntryPoint(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderWithParametersNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pHeader,                // The header
    __in_opt ParseTreeNode* pParam              // The first parameter
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pHeader));

    if (pParam != nullptr)
    {
        CHK(AppendChild(pParam));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verifies that the typing information has been figured out. Any
//              method that looks for typing information should call this first
//              before looking at the typing information.
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderWithParametersNode::VerifySelf()
{
    CHK_START;

    // We must look at each parameter to do verification for parameters of type 'void'
    UINT cParameters = GetParameterCount();
    // Loop backwards as we may end up removing the first one, which modifies the loop conditions.
    for (UINT i = cParameters; i > 0; i--)
    {
        ParameterDeclarationNode* pParam = GetParameterNode(i - 1);
        TSmartPointer<GLSLType> spParamType;
        CHK(pParam->GetType(&spParamType));

        int basicType;
        if (SUCCEEDED(spParamType->GetBasicType(&basicType)) && basicType == VOID_TOK)
        {
            if (i == 1)
            {
                // If the first parameter is type void, we get rid of it so that things
                // will treat no parameters the same as single void parameter.
                // Additionally we will match the GLSL reference compiler that allows for
                // void to be the first parameter even if there is more than one. The
                // reference compiler acts as though it is not present, so doing the
                // extraction gives us the same behavior.
                TSmartPointer<ParseTreeNode> spChildPointer;
                CHK(ExtractChild(1, &spChildPointer));
            }
            else
            {
                // Any parameter other than the first one is not allowed to be of type void.
                // If the void parameter was named, this will have been detected and flagged as
                // an error when the parameter declaration was verified. However, unnamed void
                // parameters must be detected here.
                CHK(GetParser()->LogError(&pParam->GetLocation(), E_GLSLERROR_MULTIPLEPARAMSWITHVOID, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }
    }

    // Somebody is declaring or defining a function. So we figure out the signature
    // and add it to the identifier table. The identifier table will scream if
    // the entry does not match.
    FunctionIdentifierNode* pFuncId = GetFunctionIdentifier();
    Assert(pFuncId != nullptr);
    CHKB(pFuncId != nullptr);

    CHK(GetParser()->UseIdentifierTable()->AddFunctionIdentifier(
        pFuncId,
        this,
        &_spInfo
        ));

    // Set the info for the identifier
    pFuncId->SetFunctionIdentifierInfo(_spInfo);

    // See if the function is main and remember that so it can be translated correctly
    // by the function header during the output stage.
    _fEntryPoint = _spInfo->IsGLSLSymbol(GLSLSymbols::main);

    // Now collect the parameters that are samplers
    CModernArray<ParseTreeNode* > rgSamplers;

    // Gather all of the samplers from the declarations
    for (UINT i = 1; i < GetChildCount(); i++)
    {
        // Get the typed parameter node
        ParameterDeclarationNode* pParam = GetChild(i)->GetAs<ParameterDeclarationNode>();

        // And find its GLSL type
        TSmartPointer<GLSLType> spParamType;
        CHK(pParam->GetType(&spParamType));

        if (
            spParamType->IsTypeOrArrayOfType(SAMPLER2D) || 
            spParamType->IsTypeOrArrayOfType(SAMPLERCUBE)
            )
        {
            CHK(rgSamplers.Add(pParam));
        }
    }

    // Now clone any samplers to make corresponding textures
    UINT uSamplerCount;
    for (UINT i = 0; i < rgSamplers.GetCount(); i++)
    {
        // Make the texture clone
        CHK(GetParser()->CloneSamplerAsTexture(rgSamplers[i], &uSamplerCount));
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
HRESULT FunctionHeaderWithParametersNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Spit out the header
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // Do all of the parameters
    for (UINT i = 1; i < GetChildCount(); i++)
    {
        // For all but the first parameter, we need a comma before doing the next
        if (i != 1)
        {
            CHK(pOutput->WriteChar(','));
        }

        CHK(GetChild(i)->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFunctionIdentifier
//
//  Synopsis:   Get the function identifier tree node.
//
//-----------------------------------------------------------------------------
FunctionIdentifierNode* FunctionHeaderWithParametersNode::GetFunctionIdentifier()
{
    FunctionHeaderNode* pHeaderNode = GetChild(0)->GetAs<FunctionHeaderNode>();
    if (pHeaderNode->GetChild(1)->GetParseNodeType() == ParseNodeType::functionIdentifier)
    {
        return pHeaderNode->GetChild(1)->GetAs<FunctionIdentifierNode>();
    }

    return nullptr;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderWithParametersNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FunctionHeaderWithParameters EntryPoint=%d", static_cast<int>(_fEntryPoint));
}

//+----------------------------------------------------------------------------
//
//  Function:   GetParameterTypes
//
//  Synopsis:   Fills the passed in array with the GLSLType for each of the
//              parameter children.
//
//-----------------------------------------------------------------------------
HRESULT FunctionHeaderWithParametersNode::GetParameterTypes(__out CModernArray<TSmartPointer<GLSLType>>& aryParameterTypes) const
{
    CHK_START;

    for (UINT i = 0; i < GetParameterCount(); i++)
    {
        TSmartPointer<GLSLType> spParameterType;
        CHK(GetParameterNode(i)->GetType(&spParameterType));
        CHK(aryParameterTypes.Add(spParameterType));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ParameterQualifiersMatchSignature
//
//  Synopsis:   Compares the qualifiers of each of the parameter children with
//              the argument qualifiers from the given signature,
//              ensure they are equal.
//
//-----------------------------------------------------------------------------
bool FunctionHeaderWithParametersNode::ParameterQualifiersMatchSignature(const CGLSLFunctionSignature& signature) const
{
    Assert(GetParameterCount() == signature.GetArgumentCount());

    bool fMatches = true;
    for (UINT i = 0; i < GetParameterCount(); i++)
    {
        ParameterDeclarationNode* pParam = GetParameterNode(i);
        int paramQualifier = pParam->GetParamQualifier();
        int signatureParamQualifier = signature.GetArgumentQualifier(i);

        // Semantically, EMPTY_TOK is equivalent to IN_TOK. Adjust these to keep comparison simple below.
        paramQualifier = (paramQualifier == EMPTY_TOK) ? IN_TOK : paramQualifier;
        signatureParamQualifier = (signatureParamQualifier == EMPTY_TOK) ? IN_TOK : signatureParamQualifier;

        if (paramQualifier != signatureParamQualifier)
        {
            fMatches = false;
            break;
        }
    }

    return fMatches;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetParameterNode
//
//  Synopsis:   Returns the 'i'th parameter child. Assumes caller has validated
//              that 'i' is in range with GetParameterCount.
//
//-----------------------------------------------------------------------------
ParameterDeclarationNode* FunctionHeaderWithParametersNode::GetParameterNode(UINT i) const
{
    return GetChild(i + 1)->GetAs<ParameterDeclarationNode>();
}
