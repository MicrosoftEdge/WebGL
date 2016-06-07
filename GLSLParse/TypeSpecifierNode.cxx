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
#include "TypeSpecifierNode.hxx"
#include "StructSpecifierNode.hxx"
#include "BasicTypeNode.hxx"
#include "IStringStream.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "BasicGLSLType.hxx"
#include "StructGLSLType.hxx"
#include "PrecisionDeclarationNode.hxx"
#include "TypeHelpers.hxx"
#include "CollectionNodeWithScope.hxx"

MtDefine(TypeSpecifierNode, CGLSLParser, "TypeSpecifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
TypeSpecifierNode::TypeSpecifierNode() :
    _precisionSpecified(NO_PRECISION),
    _precisionComputed(NO_PRECISION)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pType,                  // The type stored in the node
    int precision,                              // The precision stored in the node
    const YYLTYPE &location                     // The location of the type specifier
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pType));
    _precisionSpecified = precision;

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Clone the type specifier
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    TypeSpecifierNode* pOriginalTS = pOriginalColl->GetAs<TypeSpecifierNode>();

    Assert(rgChildClones.GetCount() == 1);

    // Add all of the clone children
    for (UINT i = 0; i < rgChildClones.GetCount(); i++)
    {
        CHK(AppendChild(rgChildClones[i]));
    }

    _precisionSpecified = pOriginalTS->_precisionSpecified;
    _precisionComputed = pOriginalTS->_precisionComputed;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Cache the type from the (already verified) child.
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::VerifySelf()
{
    CHK_START;

    ParseTreeNode* pFirstChild = GetChild(0);
    switch (pFirstChild->GetParseNodeType())
    {
    case ParseNodeType::basicType:
        {
            BasicTypeNode* pBasicTypeNode = pFirstChild->GetAs<BasicTypeNode>();
            CHK(pBasicTypeNode->GetType(&_spType));
            Assert(_spType->IsBasicType());
        }
        break;

    case ParseNodeType::structSpecifier:
        {
            StructSpecifierNode* pStructSpecifierNode = pFirstChild->GetAs<StructSpecifierNode>();
            ParseNodeType::Enum parentNodeType = GetParent()->GetParseNodeType();
            if (parentNodeType == ParseNodeType::structDeclaration)
            {
                // If the parent is a struct declaration, this is a nested struct specifiers, which is not allowed
                CHK(GetParser()->LogError(&pStructSpecifierNode->GetLocation(), E_GLSLERROR_NESTEDSTRUCTSINVALID, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
            else if (parentNodeType == ParseNodeType::parameterDeclarator)
            {
                // If the parent is a parameter declarator the function can't be called (the struct specifier's typename
                // is scoped to the function body).
                CHK(GetParser()->LogError(&pStructSpecifierNode->GetLocation(), E_GLSLERROR_STRUCTDECLINPARAMETER, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            CHK(pStructSpecifierNode->GetType(&_spType));
        }
        break;

    case ParseNodeType::typeNameIdentifier:
        {
            // If the type_specifier's child is a typename identifier, figure out its type from
            // the identifier info.
            TypeNameIdentifierNode* pTypeNameIdentifierNode = pFirstChild->GetAs<TypeNameIdentifierNode>();
            TSmartPointer<CTypeNameIdentifierInfo> spTypeNameInfo;
            CHK(pTypeNameIdentifierNode->GetTypeNameIdentifierInfo(&spTypeNameInfo));
            CHK(spTypeNameInfo->GetType(&_spType));
        }
        break;

    default:
        CHK_VERIFY(false);
        break;
    }

    CHK(VerifyAndComputePrecision());

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    return GetChild(0)->OutputHLSL(pOutput);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "TypeSpecifier precisionSpecified=%d, precisionComputed=%d", _precisionSpecified, _precisionComputed);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Return the resulting GLSLType for the type specifier node.
//
//-----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::GetType(__deref_out GLSLType** ppType) const
{
    AssertSz(IsVerified() && _spType != nullptr, "No one should ask for the type before the node is verified");

    _spType.CopyTo(ppType);

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateNodeFromType
//
//  Synopsis:   Based on the passed in type, construct a TypeSpecifierNode
//              (complete with subtree) that represents the type with parse
//              tree structure.
//
//+----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::CreateNodeFromType(__in CGLSLParser* pParser, __in GLSLType* pGLSLType, __deref_out TypeSpecifierNode** ppTypeSpecifierNode)
{
    CHK_START;

    // Array-ness comes from a declarator, not the type specifier. In these cases we must unwrap to
    // get to the underlying type
    TSmartPointer<GLSLType> spTypeUnwrapped;
    if (pGLSLType->IsArrayType())
    {
        CHK_VERIFY(SUCCEEDED(pGLSLType->GetArrayElementType(&spTypeUnwrapped)));

        // Array of array types are not currently allowed.
        CHK_VERIFY(!spTypeUnwrapped->IsArrayType());
    }
    else
    {
        spTypeUnwrapped = pGLSLType;
    }

    YYLTYPE nullLocation = {0};
    TSmartPointer<ParseTreeNode> spTypeSpecifierChild;
    if (spTypeUnwrapped->IsStructType())
    {
        // We want a typename id node with the same symbol as the given struct type. This will
        // result in the TypeNameIdentifierNode (and TypeSpecifierNode) looking up and having
        // the same type info that is passed in here.
        CHK(RefCounted<TypeNameIdentifierNode>::Create(pParser, spTypeUnwrapped->AsStructType()->UseTypeNameInfo()->GetSymbolIndex(), nullLocation, /*out*/spTypeSpecifierChild));
    }
    else 
    {
        Assert(spTypeUnwrapped->IsBasicType());
        CHK(RefCounted<BasicTypeNode>::Create(pParser, spTypeUnwrapped->AsBasicType()->GetBasicType(), /*out*/spTypeSpecifierChild));
    }

    // Use NO_PRECISION for the precision qualifier. Currently there are no plans to use this
    // except for shared uniform type checking. We should not be generating uniform types.
    TSmartPointer<TypeSpecifierNode> spNewTypeSpecifierNode;
    CHK(RefCounted<TypeSpecifierNode>::Create(pParser, spTypeSpecifierChild, NO_PRECISION, nullLocation, /*out*/spNewTypeSpecifierNode));

    *ppTypeSpecifierNode = spNewTypeSpecifierNode.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyAndComputePrecision
//
//  Synopsis:   Verifies the specified precision is allowed for the type.
//              Computes the precision as necessary when the precision was
//              not specified.
//
//+----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::VerifyAndComputePrecision()
{
    CHK_START;

    if (_spType->IsBasicType())
    {
        int basicType = _spType->AsBasicType()->GetBasicType();
        CHK(VerifyAndComputePrecisionForBasicType(basicType));
    }
    else
    {
        AssertSz(_spType->IsStructType(), "Array types are only composed at declaration time - type specifiers can only be basic or struct types");

        // Struct types can never have precision specified (individual members have precision).
        if (_precisionSpecified != NO_PRECISION)
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
        
        AssertSz(_precisionComputed == NO_PRECISION, "Computed precision should have been initialized to NO_PRECISION");
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyAndComputePrecisionForBasicType
//
//  Synopsis:   Some basic types are required to have a precision. If there
//              was a precision specified, that is also the computed precision.
//              Otherwise, for types that require it, we'll look up the
//              precision of its component type and set that as the computed
//              precision.
//              Boolean component basic types cannot have a precision specified.
//
//+----------------------------------------------------------------------------
HRESULT TypeSpecifierNode::VerifyAndComputePrecisionForBasicType(int basicType)
{
    CHK_START;

    bool fPrecisionComputed = false;

    // For basic types that have a component type, we must use that as a 
    // base type to look up the currently declared precision (e.g. ivec3 -> int).
    // Basic types that do not (sampler2D, samplerCube) are directly used.
    int componentType;
    if (TypeHelpers::GetComponentType(basicType, &componentType) != S_OK)
    {
        componentType = basicType;
    }

    if (_precisionSpecified == NO_PRECISION)
    {
        GLSLPrecisionType glslPrecisionType;
        if (PrecisionDeclarationNode::GetPrecisionTypeFromGLSLType(componentType, /*out*/glslPrecisionType) == S_OK)
        {
            // This type requires precision; if it is found on the nearest scope, we'll
            // set our member, otherwise this is a known error we must log.
            CollectionNodeWithScope* pNearestScope = CollectionNodeWithScope::GetCollectionNodeWithScope(this);
            if (pNearestScope->GetPrecisionForType(glslPrecisionType, &_precisionComputed) != S_OK)
            {
                const BasicGLSLTypeInfo* pTypeInfo;
                CHK_VERIFY(SUCCEEDED(BasicGLSLTypeInfo::GetInfo(basicType, &pTypeInfo)));
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_NOPRECISIONSPECIFIED, pTypeInfo->_pszGLSLName));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            fPrecisionComputed = true;
        }
    }
    else
    {
        GLSLPrecisionType glslPrecisionType;
        if (PrecisionDeclarationNode::GetPrecisionTypeFromGLSLType(componentType, /*out*/glslPrecisionType) != S_OK)
        {
            // Precision qualifiers are only allowed to be specified for certain types -
            // the same types that actually have precision in GLSL. For those that do
            // not have precision (GetPrecisionTypeFromGLSLType returns failure), we must raise an error
            const BasicGLSLTypeInfo* pTypeInfo;
            CHK_VERIFY(SUCCEEDED(BasicGLSLTypeInfo::GetInfo(basicType, &pTypeInfo)));
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE, pTypeInfo->_pszGLSLName));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    // If we were not required to compute the precision above, or precision was originally
    // specified, the computed value is simply the specified value
    if (!fPrecisionComputed)
    {
        _precisionComputed = _precisionSpecified;
    }

    CHK_RETURN;
}
