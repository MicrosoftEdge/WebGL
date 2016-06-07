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
#include "PrecisionDeclarationNode.hxx"
#include "GLSL.tab.h"
#include "GLSLParser.hxx"
#include "BasicTypeNode.hxx"
#include "CollectionNodeWithScope.hxx"

MtDefine(PrecisionDeclarationNode, CGLSLParser, "PrecisionDeclarationNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
PrecisionDeclarationNode::PrecisionDeclarationNode() : 
    _precisionQualifier(NO_PRECISION)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT PrecisionDeclarationNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int precisionQual,                          // Precision qualifier in the declaration
    __in ParseTreeNode* pType,                  // Type specifier in the declaration
    const YYLTYPE &location                     // The location of the declaration
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pType));

    _precisionQualifier = precisionQual;

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verify that the precision declaration is properly formed.
//
//+----------------------------------------------------------------------------
HRESULT PrecisionDeclarationNode::VerifySelf()
{
    CHK_START;

    ParseTreeNode* pTypeChild = GetTypeChild();
    ParseNodeType::Enum childType = pTypeChild->GetParseNodeType();

    GLSLPrecisionType glslPrecisionType = GLSLPrecisionType::Count;
    bool fPrecisionTypeVerified = false;

    // type_specifier_no_prec only has three productions: struct_specifier, typename_identifier,
    // and basic_type. struct_specifier and typename_identifier are not valid since only 4 basic
    // types are valid.
    if (childType != ParseNodeType::structSpecifier && childType != ParseNodeType::typeNameIdentifier)
    {
        CHK_VERIFY(childType == ParseNodeType::basicType);

        // The child has been verified, we must be able to get its type which must not be an array type
        TSmartPointer<GLSLType> spType;
        CHK_VERIFY(pTypeChild->GetAs<BasicTypeNode>()->GetType(&spType) == S_OK);

        int basicType;
        CHK_VERIFY(spType->GetBasicType(&basicType) == S_OK);

        fPrecisionTypeVerified = (GetPrecisionTypeFromGLSLType(basicType, /*out*/glslPrecisionType) == S_OK);
    }

    if (!fPrecisionTypeVerified)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CollectionNodeWithScope* pNearestScope = CollectionNodeWithScope::GetCollectionNodeWithScope(this);
    pNearestScope->SetPrecisionForType(glslPrecisionType, _precisionQualifier);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetPrecisionTypeFromGLSLType
//
//  Synopsis:   Certain types are allowed to be used in precision declarations
//              This function translates between type from the parser and
//              allowed types with precision, returning an error if the type
//              is not recognized.
//
//+----------------------------------------------------------------------------
HRESULT PrecisionDeclarationNode::GetPrecisionTypeFromGLSLType(int basicType, __out GLSLPrecisionType& glslPrecisionType)
{
    switch (basicType)
    {
    case FLOAT_TOK:
        glslPrecisionType = GLSLPrecisionType::Float;
        break;

    case INT_TOK:
        glslPrecisionType = GLSLPrecisionType::Int;
        break;

    case SAMPLER2D:
        glslPrecisionType = GLSLPrecisionType::Sampler2D;
        break;

    case SAMPLERCUBE:
        glslPrecisionType = GLSLPrecisionType::SamplerCube;
        break;

    default:
        return E_FAIL;
    }

    return S_OK;
}
//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree. Right now precision
//              statements are understood but they have no impact on the
//              translation.
//
//-----------------------------------------------------------------------------
HRESULT PrecisionDeclarationNode::OutputHLSL(__in IStringStream* pOutput)
{
    return S_OK;
}
