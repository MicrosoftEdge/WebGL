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
#include "VariableIdentifierNode.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"
#include "StructGLSLType.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "StructSpecifierCollectionNode.hxx"
#include "TranslationUnitNode.hxx"
#include "GLSL.tab.h"

MtDefine(CVariableIdentifierInfo, CGLSLParser, "IdentifierInfo");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CVariableIdentifierInfo::CVariableIdentifierInfo() :
    _iSymbolIndex(-1),
    _declarationScopeId(-1),
    _typeQualifier(NO_QUALIFIER),
    _precisionQualifier(NO_PRECISION),
    _fUsed(false),
    _fIsLoopDeclared(false),
    _fIsParameter(false),
    _uWriteCount(0),
    _specialVariable(GLSLSpecialVariables::count)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init identifier information from an identifier node. This
//              indicates a user defined identifer.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::Initialize(
    __in VariableIdentifierNode* pNode,                         // The identifier node with the symbol information
    __in GLSLType* pType,                                       // The GLSL type of the identifier
    int typeQualifier,                                          // The storage qualifier of the identifier
    int precisionQualifier,                                     // The precision qualifier of the identifier
    bool fIsParameter,                                          // Whether it is a function parameter
    const ConstantValue& initValue                              // The initial value that the identifier has
    )
{
    CHK_START;

    _iSymbolIndex = pNode->GetSymbolIndex();
    _typeQualifier = typeQualifier;
    _precisionQualifier = precisionQualifier;
    _initialValue = initValue;
    _fIsParameter = fIsParameter;
    _spType = pType;

    // Find the nearest scope node
    CollectionNodeWithScope* pScope = CollectionNodeWithScope::GetCollectionNodeWithScope(pNode);
    CHKB(pScope != nullptr);

    // The nearest scope node defines what scope we are declared in
    _declarationScopeId = pScope->GetScopeId();

    // Find out what basic type we have - make sure it is not void or array of void
    if (_spType->IsTypeOrArrayOfType(VOID_TOK))
    {
        CHK(pNode->GetParser()->LogError(&pNode->GetLocation(), E_GLSLERROR_VARIABLETYPEVOID, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // We have hard checks here to defend against the parser code slipping up somehow and
    // these end up not being set.
    CHKB(_iSymbolIndex != -1);
    CHKB(_declarationScopeId != -1);

    // Calculate HLSL name, unless it is an attribute or varying. Those will be calculated
    // with data provided by the IOStructNode later.
    if (!IsInputStructVariable())
    {
        if (_iSymbolIndex >= static_cast<int>(GLSLSymbols::count))
        {
            // The known symbols occupy the first slots, so we shift the symbol index by the known symbol count to
            // have the numbering stay consistent as more known symbols are added.
            CHK(_rgHLSLNames.Add(nullptr));
            CHK(_rgHLSLNames[0].Format(
                s_uHLSLMAXLENGTH,
                "var_%d_%d", 
                _declarationScopeId, 
                _iSymbolIndex - static_cast<int>(GLSLSymbols::count)
                ));
        }
        else
        {
            // It is perfectly valid to use a known index for a different namespace (like using 'distance' for
            // a variable name even though there is a function called 'distance').
            CHK(_rgHLSLNames.Add(nullptr));
            CHK(_rgHLSLNames[0].Format(
                s_uHLSLMAXLENGTH,
                "vark_%d_%d", 
                _declarationScopeId, 
                _iSymbolIndex
                ));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init identifier information from known type information.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::Initialize(
    int type,                                                   // Basic type of variable 
    __in_z const char* pszName,                                 // GLSL name of variable
    __in CGLSLSymbolTable* pSymTable                            // Symbol table
    )
{
    CHK_START;

    CHK(pSymTable->EnsureSymbolIndex(pszName, &_iSymbolIndex));
    CHK(GLSLType::CreateFromBasicTypeToken(type, &_spType));

    // Reuse the name from the GLSL table
    CHK(_rgHLSLNames.Add(pszName));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init identifier information from a known special variable. 
//              Since these are always known, they are never declared.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::Initialize(
    const GLSLSpecialVariables::Enum eKnown,                    // Index of known thing to add
    __in CGLSLParser* pParser                                   // Parser
    )
{
    CHK_START;

    const GLSLSpecialVariableInfo &variableInfo = GLSLKnownSymbols::GetKnownInfo<GLSLSpecialVariableInfo>(eKnown);

    _declarationScopeId = 0;
    _specialVariable = eKnown;
    _typeQualifier = variableInfo._qualifier;

    // The symbol indices start with the known symbols so we can cast like this - see the
    // CGLSLParser::Initialize function for the code that asserts this.
    _iSymbolIndex = static_cast<int>(variableInfo._symbolEnum);

    // Create the type object
    int basicType;
    if (SUCCEEDED(variableInfo.GetBasicType(&basicType)))
    {
        TSmartPointer<GLSLType> spBasicType;
        CHK(GLSLType::CreateFromBasicTypeToken(basicType, &spBasicType));
        CHK(GLSLType::CreateFromType(spBasicType, variableInfo._arraySize, &_spType));
    }
    else
    {
        // If not basic, it must be using a special type
        GLSLSpecialTypes::Enum eKnownType;
        CHK_VERIFY(SUCCEEDED(variableInfo.GetSpecialType(&eKnownType)));
        const GLSLSpecialTypeInfo &typeInfo = GLSLKnownSymbols::GetKnownInfo<GLSLSpecialTypeInfo>(eKnownType);

        // Make the list of identifier info
        CModernArray<TSmartPointer<IIdentifierInfo>> aryIdInfo;
        for (int i = 0; i < typeInfo._numFields; i++)
        {
            TSmartPointer<IIdentifierInfo> spNewInfo;
            CHK(RefCounted<CVariableIdentifierInfo>::Create(
                typeInfo._rgFields[i]._type, 
                typeInfo._rgFields[i]._pszName,
                pParser->UseSymbolTable(),
                /*out*/spNewInfo
                ));

            CHK(aryIdInfo.Add(spNewInfo));
        }

        // Now we can make a type
        TSmartPointer<StructGLSLType> spStructType;
        CHK(GLSLType::CreateStructTypeFromIdentifierInfoAry(aryIdInfo, &spStructType));

        // We also need a typename identifier info for that type
        TSmartPointer<CTypeNameIdentifierInfo> spTypeNameInfo;
        CHK(RefCounted<CTypeNameIdentifierInfo>::Create(
            typeInfo,
            spStructType,
            /*out*/spTypeNameInfo
            ));

        spStructType->FinalizeTypeWithTypeInfo(spTypeNameInfo);

        // Now we can save away our type
        _spType = spStructType;

        // Add the info to the struct specifier collection so that the constructor and equality functions
        // are output if need be. This will also hold a reference to the typename info.
        Assert(pParser->UseRootNode()->GetParseNodeType() == ParseNodeType::translationUnit);
        TranslationUnitCollectionNode* pTranslationUnitCollection = pParser->UseRootNode()->GetAs<TranslationUnitCollectionNode>();
        StructSpecifierCollectionNode* pStructSpecifierCollection = pTranslationUnitCollection->GetStructSpecifierCollection();
        CHK(pStructSpecifierCollection->AddImplementationTypeInfo(spTypeNameInfo));

        // We want to grab the root scope and put the type name info there - we want it to behave like
        // a struct with global scope so it can be used anywhere in the shader.
        CollectionNodeWithScope* pRootScope = static_cast<CollectionNodeWithScope*>(pParser->UseRootNode());
        CHK(pRootScope->AddDeclaredIdentifier(spTypeNameInfo));
    }

    if (variableInfo._pHLSLName != nullptr)
    {
        CHK(_rgHLSLNames.Add(variableInfo._pHLSLName));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLName
//
//  Synopsis:   Allows the caller to set the HLSL name to something specific.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::SetHLSLName(
    UINT uIndex,                                                // Index of name
    __in_z LPCSTR pszName,                                      // HLSL name at that index
    __in_z LPCSTR pszSemantic                                   // HLSL semantic at that index
    )
{
    CHK_START;
    
    CHK(_rgHLSLNames.EnsureSize(uIndex + 1));
    CHK(_rgHLSLNames[uIndex].Set(pszName));

    CHK(_rgHLSLSemantics.EnsureSize(uIndex + 1));
    CHK(_rgHLSLSemantics[uIndex].Set(pszSemantic));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetExpressionType
//
//  Synopsis:   Gets the type of the identifier for usage in an expression.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::GetExpressionType(__deref_out GLSLType** ppType) const
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
//  Function:   GetHLSLName
//
//  Synopsis:   Gets the name of the identifier to use in HLSL.
//
//-----------------------------------------------------------------------------
const char* CVariableIdentifierInfo::GetHLSLName(UINT uIndex) const 
{ 
    return _rgHLSLNames[uIndex];
}

//+----------------------------------------------------------------------------
//
//  Function:   GetHLSLSemantic
//
//  Synopsis:   Gets the semantic of the identifier to use in HLSL.
//
//-----------------------------------------------------------------------------
const char* CVariableIdentifierInfo::GetHLSLSemantic(UINT uIndex) const 
{ 
    return _rgHLSLSemantics[uIndex];
}

//+----------------------------------------------------------------------------
//
//  Function:   NameFromIndex
//
//  Synopsis:   Calculate the HLSL name and the semantic from an index - should
//              only be used on attributes and varyings.
//
//-----------------------------------------------------------------------------
HRESULT CVariableIdentifierInfo::NameFromIndex(UINT uIndex)
{
    CHK_START;

    // Attribute and varying equivalents need a semantic. The semantic is made to not end with
    // a number, because that ends up causing some parsing to happen in D3D that we don't want.
    if (_typeQualifier == ATTRIBUTE)
    {
        CHK(_rgHLSLNames.Add(nullptr));
        CHK(_rgHLSLNames[0].Format(s_uHLSLMAXLENGTH, "attr_%d", uIndex));
        CHK(_rgHLSLSemantics.Add(nullptr));
        CHK(_rgHLSLSemantics[0].Format(s_uHLSLMAXLENGTH, "attr_%d_sem", uIndex));
    }
    else
    {
        Assert(_typeQualifier == VARYING);

        CHK(_rgHLSLNames.Add(nullptr));
        CHK(_rgHLSLNames[0].Format(s_uHLSLMAXLENGTH, "vary_%d", uIndex));
        CHK(_rgHLSLSemantics.Add(nullptr));
        CHK(_rgHLSLSemantics[0].Format(s_uHLSLMAXLENGTH, "vary_%d_sem", uIndex));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsInputStructVariable
//
//  Synopsis:   Returns whether the variable is part of an input struct
//              (for now VSInput and PSInput, which correspond to attributes
//              and varyings, respectively).
//
//+----------------------------------------------------------------------------
bool CVariableIdentifierInfo::IsInputStructVariable() const
{
    return (_typeQualifier == ATTRIBUTE || _typeQualifier == VARYING);
}

//+----------------------------------------------------------------------------
//
//  Function:   AreEqualTypesForUniforms
//
//  Synopsis:   Variables' types are equivalent for uniforms if both their
//              type and precision match.
//
//+----------------------------------------------------------------------------
bool CVariableIdentifierInfo::AreEqualTypesForUniforms(
    __in const CVariableIdentifierInfo* pVertexVariableInfo,    // Variable in vertex shader
    __in const CGLSLIdentifierTable* pVertexIdTable,            // Id table for the shader the vertex variable belongs to
    __in const CVariableIdentifierInfo* pFragmentVariableInfo,  // Variable in fragment shader
    __in const CGLSLIdentifierTable* pFragmentIdTable           // Id table for the shader the fragment variable belongs to
    )
{
    bool fTypesEqual = pVertexVariableInfo->UseType()->IsEqualTypeForUniforms(
        pVertexIdTable,
        pFragmentVariableInfo->UseType(),
        pFragmentIdTable
        );

    bool fPrecisionsEqual = (pVertexVariableInfo->GetPrecisionQualifier() == pFragmentVariableInfo->GetPrecisionQualifier());

    return fTypesEqual && fPrecisionsEqual;
}
