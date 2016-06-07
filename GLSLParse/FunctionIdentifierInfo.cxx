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
#include "FunctionIdentifierInfo.hxx"
#include "FunctionIdentifierNode.hxx"
#include "CollectionNodeWithScope.hxx"
#include "FunctionHeaderNode.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "ParameterDeclarationNode.hxx"
#include "GLSLSymbolTable.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"
#include "GLSL.tab.h"

MtDefine(CFunctionIdentifierInfo, CGLSLParser, "IdentifierInfo");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CFunctionIdentifierInfo::CFunctionIdentifierInfo() :
    _iSymbolIndex(-1),
    _fDeclared(false),
    _fDefined(false),
    _fCalled(false),
    _hlslFunction(HLSLFunctions::count)
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
HRESULT CFunctionIdentifierInfo::Initialize(
    __in FunctionIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
    __in FunctionHeaderWithParametersNode* pHeaderWithParam     // The function header with param that owns the identifier
    )
{
    CHK_START;

    _iSymbolIndex = pIdentifier->GetSymbolIndex();

    // We have hard checks here to defend against the parser code slipping up somehow and
    // these end up not being set.
    CHKB(_iSymbolIndex != -1);

    // The known symbols occupy the first slots, so we shift the symbol index by the known symbol count to
    // have the numbering stay consistent as more known symbols are added.
    int iIdNumber = _iSymbolIndex - static_cast<int>(GLSLSymbols::count);

    if (iIdNumber >= 0)
    {
        // Generate the HLSL name from the symbol index and scope id
        CHK(_rgHLSLName.Format(s_uHLSLMAXLENGTH, "fn_0_%d", iIdNumber));
    }
    else
    {
        // Somebody is overloading a built in name
        CHK(_rgHLSLName.Format(s_uHLSLMAXLENGTH, "fn_kn_0_%d", _iSymbolIndex));
    }

    // Use the function header to create the signature - start with the return type
    FunctionHeaderNode* pHeader = pHeaderWithParam->GetChild(0)->GetAs<FunctionHeaderNode>();

    // Add the return type and argument types to the signature
    TSmartPointer<GLSLType> spReturnType;
    CHK_VERIFY(pHeader->GetType(&spReturnType) == S_OK);
    CHK(_signature.AddType(spReturnType, EMPTY_TOK));

    for (UINT i = 0; i < pHeaderWithParam->GetParameterCount(); i++)
    {
        ParameterDeclarationNode* pParam = pHeaderWithParam->GetParameterNode(i);

        TSmartPointer<GLSLType> spParamType;
        CHK(pParam->GetType(&spParamType));
        CHK(_signature.AddType(spParamType, pParam->GetParamQualifier()));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init identifier information from a known function. Since these
//              are always known, they are never declared.
//
//-----------------------------------------------------------------------------
HRESULT CFunctionIdentifierInfo::Initialize(
    const GLSLFunctionInfo &info,                               // Function info of function to add
    __in CGLSLSymbolTable* pSymTable                            // Symbol table
    )
{
    CHK_START;

    _hlslFunction = info._hlslEnum;

    // The symbol indices start with the known symbols so we can cast like this - see the
    // CGLSLParser::Initialize function for the code that asserts this.
    _iSymbolIndex = static_cast<int>(info._symbolEnum);

    // Init the signature
    CHK(_signature.InitFromKnown(info));

    if (_hlslFunction != HLSLFunctions::count)
    {
        const HLSLFunctionInfo &hlslFunctionInfo = GLSLKnownSymbols::GetKnownInfo<HLSLFunctionInfo>(_hlslFunction);

        CHK(_rgHLSLName.Set(hlslFunctionInfo._pHLSLName));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetDefined
//
//  Synopsis:   Marks the function identifier as being defined, and stores a
//              pointer to the function definition node that defines the function.
//              This will be null for known functions.
//
//+----------------------------------------------------------------------------
void CFunctionIdentifierInfo::SetDefined(__in_opt FunctionDefinitionNode* pFuncDefinition)
{
    AssertSz(!_fDefined && _spFunctionDefinition == nullptr, "A function should only be able to be defined once! There must be a bug in function identifier lookup");

    _fDefined = true;
    _spFunctionDefinition = pFuncDefinition;
}
