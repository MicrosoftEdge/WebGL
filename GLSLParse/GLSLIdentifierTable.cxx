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
#include "GLSLIdentifierTable.hxx"
#include "VariableIdentifierInfo.hxx"
#include "CollectionNodeWithScope.hxx"
#include "VariableIdentifierNode.hxx"
#include "FunctionIdentifierNode.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "RefCounted.hxx"
#include "GLSLParser.hxx"
#include "GLSL.tab.h"
#include "FunctionHeaderWithParametersNode.hxx"
#include "VerificationHelpers.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::Initialize(
    __in CGLSLParser* pParser                                   // The parser that owns the table
    )
{ 
    CHK_START;

    _spSymbolTable = pParser->UseSymbolTable(); 

    Assert(pParser->UseRootNode()->GetParseNodeType() == ParseNodeType::translationUnit);
    CollectionNodeWithScope* pRootScope = static_cast<CollectionNodeWithScope*>(pParser->UseRootNode());

    bool fDeriveEnabled = pParser->UseExtensionState()->IsExtensionEnabled(GLSLExtension::GL_OES_standard_derivatives);
    bool fFragDepthEnabled = pParser->UseExtensionState()->IsExtensionEnabled(GLSLExtension::GL_EXT_frag_depth);
    GLSLShaderType::Enum shaderType = pParser->GetShaderType();

    // Go through the known GLSL symbols and add the appropriate identifier information
    // so that when they are referenced or somebody tries to declare them again, the
    // right thing happens.
    for (int i = 0; i < GLSLFunctions::count; i++)
    {
        GLSLFunctions::Enum known = static_cast<GLSLFunctions::Enum>(i);
        const GLSLFunctionInfo &info = GLSLKnownSymbols::GetKnownInfo<GLSLFunctionInfo>(known);

        // Some builtin functions are only extant in a certain pipeline stage or with an extension enabled
        bool fragmentOnly = (info._uFlags & static_cast<UINT>(GLSLIdentifierFlags::FragmentOnly)) != 0;
        bool vertexOnly = (info._uFlags & static_cast<UINT>(GLSLIdentifierFlags::VertexOnly)) != 0;
        bool derivOnly = (info._uFlags & static_cast<UINT>(GLSLIdentifierFlags::DerivOnly)) != 0;

        // Make this decision more readable - the above flags indicate if the function is blocked
        // in a stage or not. The below flags look at the shader type and determine if for this
        // shader, that means that they are blocked for a reason.
        bool fragmentBlocked = fragmentOnly && shaderType != GLSLShaderType::Fragment;
        bool vertexBlocked = vertexOnly && shaderType != GLSLShaderType::Vertex;
        bool derivBlocked = derivOnly && !fDeriveEnabled;

        // Only add the function if not blocked for some reason
        if (!fragmentBlocked && !vertexBlocked && !derivBlocked)
        {
            // Alloc and init the new info
            TSmartPointer<CFunctionIdentifierInfo> spNewInfo;
            CHK(RefCounted<CFunctionIdentifierInfo>::Create(info, _spSymbolTable, /*out*/spNewInfo));

            if (known != GLSLFunctions::main)
            {
                // Known things are always defined (except for main).
                // Known functions do not have a function definition node - they are implicitly defined by the language.
                spNewInfo->SetDefined(/*pFuncDefinition*/nullptr);
            }

            // Make sure that it is in the collection of root scope identifiers
            CHK(pRootScope->AddDeclaredIdentifier(spNewInfo));
        }
    }

    for (int i = 0; i < GLSLSpecialVariables::count; i++)
    {
        GLSLSpecialVariables::Enum known = static_cast<GLSLSpecialVariables::Enum>(i);
        const GLSLSpecialVariableInfo &info = GLSLKnownSymbols::GetKnownInfo<GLSLSpecialVariableInfo>(known);

        bool fragDepthOnly = (info._uFlags & GLSLIdentifierFlags::FragDepthOnly) != GLSLIdentifierFlags::None;
        bool fragDepthBlocked = fragDepthOnly && !fFragDepthEnabled;

        if (info.IsInScope(shaderType) && !fragDepthBlocked)
        {
            // Alloc and init the new info
            TSmartPointer<CVariableIdentifierInfo> spNewInfo;
            CHK(RefCounted<CVariableIdentifierInfo>::Create(known, pParser, /*out*/spNewInfo));

            // Add it to the list of all variable identifiers
            CHK(_aryVarList.Add(spNewInfo));

            // Make sure that it is in the collection of root scope identifiers
            CHK(pRootScope->AddDeclaredIdentifier(spNewInfo));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddVariableIdentifier
//
//  Synopsis:   Add an identifier to the table. Duplicates are not allowed.
//
//              The caller can optionally provide an out param to receive the
//              new info.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::AddVariableIdentifier(
    __in VariableIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
    __in GLSLType* pType,                                       // The GLSL type of the identifier
    int typeQualifier,                                          // The storage qualifier
    int precisionQualifier,                                     // The precision qualifier
    bool fIsParameter,                                          // Whether it is a function parameter
    const ConstantValue& initValue,                             // The initial value that the identifier has
    __deref_out CVariableIdentifierInfo** ppInfo                // The info that was added
    )
{
    CHK_START;

    CHK(pIdentifier->VerifyIdentifierName());

    // Find out the nearest scope defining node above the identifier
    CollectionNodeWithScope* pScope = CollectionNodeWithScope::GetCollectionNodeWithScope(pIdentifier);

    // Safety check - GetCollectionNodeWithScope should have asserted if we get here
    CHKB(pScope != nullptr);

    // If the scope already defined this identifier, this is an error and we can't add it.
    if (pScope->DefinesTypeNameOrVariableIdentifier(pIdentifier))
    {
        CHK(pIdentifier->GetParser()->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_IDENTIFIERALREADYDECLARED, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Alloc and init the new info
    TSmartPointer<CVariableIdentifierInfo> spNewInfo;
    CHK(RefCounted<CVariableIdentifierInfo>::Create(
        pIdentifier,
        pType,
        typeQualifier,
        precisionQualifier,
        fIsParameter,
        initValue,
        /*out*/spNewInfo
        ));

    // Add to list of all variable identifiers
    CHK(_aryVarList.Add(spNewInfo));

    // Add to the list of identifiers for the scope it is in
    CHK(pScope->AddDeclaredIdentifier(spNewInfo));

    // Return the new info
    (*ppInfo) = spNewInfo.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddFunctionIdentifier
//
//  Synopsis:   Add an function identifier to the table.
//
//              If the identifier exists already, then the function returns the
//              info for the existing identifier. The caller then figures out
//              if the situation demands an error from there.
//
//              If the identifier does not exist yet, then a new one is 
//              created.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::AddFunctionIdentifier(
    __in FunctionIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
    __in FunctionHeaderWithParametersNode* pFunctionHeader,     // The function header with param that owns the identifier
    __deref_out CFunctionIdentifierInfo** ppInfo                // The info for the function
    )
{
    CHK_START;

    CHK(pIdentifier->VerifyIdentifierName());

    // Safety check - GetCollectionNodeWithScope should have asserted if we get here
    CollectionNodeWithScope* pScope = CollectionNodeWithScope::GetCollectionNodeWithScope(pIdentifier);
    CHKB(pScope != nullptr);

    // Find out the nearest scope defining node above the identifier - it should be
    // a translation unit because all function declarations and definitions need
    // to be global.
    CHKB(pScope->GetParseNodeType() == ParseNodeType::translationUnit);

    // Alloc and init the new info
    TSmartPointer<CFunctionIdentifierInfo> spNewInfo;
    CHK(RefCounted<CFunctionIdentifierInfo>::Create(pIdentifier, pFunctionHeader, /*out*/spNewInfo));

    CModernArray<TSmartPointer<GLSLType>> aryArgTypes;
    CHK(pFunctionHeader->GetParameterTypes(aryArgTypes));

    bool fFound = false;
    CModernArray<TSmartPointer<IIdentifierInfo>> rgInfos;
    if (SUCCEEDED(pScope->GetIdentifierInfoList(pIdentifier->GetSymbolIndex(), rgInfos)))
    {
        for (UINT i = 0; i < rgInfos.GetCount(); i++)
        {
            CFunctionIdentifierInfo* pFuncInfo = rgInfos[i]->AsFunction();
            if (pFuncInfo != nullptr)
            {
                // This identifier exists - check to see whether the argument types match
                // the signature of the already declared/defined found function. 
                int genType;
                TSmartPointer<GLSLType> spReturnType;
                if (SUCCEEDED(pFuncInfo->GetSignature().SignatureMatchesArgumentTypes(aryArgTypes, &genType, &spReturnType)))
                {
                    // Based on the arguments of this function definition/declaration to be added, we matched
                    // an already defined function. Before we allow this matching, we must validate 
                    // a few things.
                    CHK(VerificationHelpers::VerifyNewFunctionInfoAgainstMatchedInfo(
                        spNewInfo,
                        pFuncInfo,
                        spReturnType,
                        pFunctionHeader,
                        pIdentifier
                        ));

                    // If we match against an already declared/defined function then we want
                    // to return the existing information that we have, not the one that
                    // we just created. The one that we made had the right signature but
                    // some other fields are not set up correctly.
                    spNewInfo = pFuncInfo;
                    fFound = true;
                    break;
                }
            }
            else
            {
                // The identifier exists for variables or typenames already
                CHK(pIdentifier->GetParser()->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_IDENTIFIERALREADYDECLARED, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);             
            }
        }
    }

    if (!fFound)
    {
        // main is a special function - you cannot declare it with a different signature. If
        // we got here, then we know that the signature does not match the correct main
        // signature, so check that the function is not main.
        if (spNewInfo->IsGLSLSymbol(GLSLSymbols::main))
        {
            CHK(pIdentifier->GetParser()->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_REDEFINEMAIN, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);             
        }

        // Add to the list of identifiers for the scope it is in
        CHK(pScope->AddDeclaredIdentifier(spNewInfo));
    }

    // Return the new info
    (*ppInfo) = spNewInfo.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddTypeNameIdentifier
//
//  Synopsis:   Add a typename identifier to the table.
//
//              Returns an error if the identifier exists already.
//
//              If the identifier does not exist yet, then a new one is 
//              created on the nearest scope.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::AddTypeNameIdentifier(
    __in TypeNameIdentifierNode* pIdentifier,                       // The typename id node that defines the type name
    __in GLSLType* pType,                                           // The type to associate with the id
    __deref_out CTypeNameIdentifierInfo** ppInfo                    // Created info for the typename
    )
{
    CHK_START;

    CHK(pIdentifier->VerifyIdentifierName());

    // Find out the nearest scope defining node above the identifier
    CollectionNodeWithScope* pScope = CollectionNodeWithScope::GetCollectionNodeWithScope(pIdentifier);

    // Safety check - GetCollectionNodeWithScope should have asserted if we get here
    CHKB(pScope != nullptr);

    // If the scope already defined this identifier, this is an error and we can't add it.
    if (pScope->DefinesTypeNameOrVariableIdentifier(pIdentifier))
    {
        CHK(pIdentifier->GetParser()->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_IDENTIFIERALREADYDECLARED, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Alloc and init the new info
    TSmartPointer<CTypeNameIdentifierInfo> spNewInfo;
    CHK(RefCounted<CTypeNameIdentifierInfo>::Create(pIdentifier, pType, /*out*/spNewInfo));

    // Add to list of all typename identifiers. We do this to keep the typename info alive
    // after we've completed translation as we'll need it during shared uniform verification.
    // A StructGLSLType and its associated CTypeNameIdentifierInfo are closely tied in that
    // they actually point to each other. In order to prevent circular references and leaks,
    // StructGLSLType has a const pointer to the CTypeNameIdentifier that represents the type
    // in glsl. When the CTypeNameIdentifier is destroyed, it clears out this back pointer on
    // the StructGLSLType. Making CTypeNameIdentifierInfo's have the same lifetime as the
    // identifier table allows StructGLSLType to reference the CTypeNameIdentifierInfo even
    // after the parser and parse tree have been destroyed.
    CHK(_aryTypeList.Add(spNewInfo));

    // Add to the list of identifiers for the scope it is in
    CHK(pScope->AddDeclaredIdentifier(spNewInfo));

    // Return the new info
    (*ppInfo) = spNewInfo.Extract();

    CHK_RETURN;
}
//+----------------------------------------------------------------------------
//
//  Function:   GetVariableInfoFromString
//
//  Synopsis:   Retrieve information about an identifier based on a string.
//              This is designed to be called from the WebGL code to allow
//              translation between the GLSL name and HLSL name.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::GetVariableInfoFromString(
    __in_z const char* pszString,                                   // The variable name to get info about
    GLSLQualifier::Enum typeQualifier,                              // What type qualifier the variable has
    __deref_out CVariableIdentifierInfo** ppInfo                    // The info about the identifier
    )
{
    CHK_START;

    int index;
    CHK(_spSymbolTable->LookupSymbolIndex(pszString, &index));

    // Find the equivalent qualifier
    int parserQualifier = GLSLQualifier::ToParserType(typeQualifier);

    bool fFound = false;
    UINT i;
    for (i = 0; i < _aryVarList.GetCount(); i++)
    {
        if (_aryVarList[i]->GetSymbolIndex() == index && _aryVarList[i]->GetTypeQualifier() == parserQualifier)
        {
            _aryVarList[i].CopyTo(ppInfo);
            fFound = true;
            break;
        }
    }

    CHKB_HR(fFound, E_INVALIDARG);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVariableInfoAndNameByIndex
//
//  Synopsis:   Returns the info and name for a given index into the id
//              table. Used for iterating all identifiers in the table.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::GetVariableInfoAndNameByIndex(
    UINT index,                                                     // The index of the identifier info to grab
    __deref_out CVariableIdentifierInfo** ppInfo,                   // The actual info
    __out PCSTR* ppszGLSLName                                       // The glsl name for this identifier
    )
{
    CHK_START;

    *ppInfo = nullptr;
    *ppszGLSLName = nullptr;

    CHKB(index < _aryVarList.GetCount());

    _aryVarList[index].CopyTo(ppInfo);

    *ppszGLSLName = _spSymbolTable->NameFromIndex(_aryVarList[index]->GetSymbolIndex());

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFieldInfoForVariableName
//
//  Synopsis:   Based on the supplied type and field name, returns the 
//              CVariableIdentifierInfo associated with the field name
//              for the type.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIdentifierTable::GetFieldInfoForVariableName(
    __in const GLSLType* pGLSLType,                                 // The type to query for field pszFieldName
    __in_z const char* pszFieldName,                                // Field name to look for
    __deref_out CVariableIdentifierInfo** ppInfo                    // Found variable identifier info
    )
{
    CHK_START;

    int iSymbolIndex;
    CHK(_spSymbolTable->LookupSymbolIndex(pszFieldName, &iSymbolIndex));

    CHKB_HR(pGLSLType->IsStructType(), E_INVALIDARG);

    CHK(pGLSLType->AsStructType()->GetFieldInfoForSymbol(iSymbolIndex, ppInfo));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetNameForSymbolIndex
//
//  Synopsis:   Returns the string name associated with the given symbol index.
//
//-----------------------------------------------------------------------------
const char* CGLSLIdentifierTable::GetNameForSymbolIndex(int iSymbolIndex) const
{
    return _spSymbolTable->NameFromIndex(iSymbolIndex);
}
