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
#include "TypeNameIdentifierInfo.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSLSymbolTable.hxx"
#include "GLSLParser.hxx"

MtDefine(CTypeNameIdentifierInfo, CGLSLParser, "IdentifierInfo");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CTypeNameIdentifierInfo::CTypeNameIdentifierInfo() :
    _iSymbolIndex(-1)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Destructor
//
//-----------------------------------------------------------------------------
CTypeNameIdentifierInfo::~CTypeNameIdentifierInfo()
{
    // Before we release the type pointer, make sure we clear 
    // out the pointer it has back to us.
    if (_spType != nullptr)
    {
        _spType->ClearTypeInfoPointer();
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init typename info from an identifier node. This
//              indicates a user defined type.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::Initialize(
    __in TypeNameIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
    __in GLSLType* pTypeDefined                                 // The type defined by this identifier
    )
{
    CHK_START;

    // Find the nearest scope node
    CollectionNodeWithScope* pScope = CollectionNodeWithScope::GetCollectionNodeWithScope(pIdentifier);
    CHKB(pScope != nullptr);

    // The nearest scope node defines what scope we are declared in
    int declarationScopeId = pScope->GetScopeId();
    _iSymbolIndex = pIdentifier->GetSymbolIndex();

    // We have hard checks here to defend against the parser code slipping up somehow and
    // these end up not being set.
    CHKB(_iSymbolIndex != -1);

    // The known symbols occupy the first slots, so we shift the symbol index by the known symbol count to
    // have the numbering stay consistent as more known symbols are added.
    int iIdNumber;
    PCSTR pszTypeNameFormat;
    if (_iSymbolIndex < static_cast<int>(GLSLSymbols::count))
    {
        iIdNumber = _iSymbolIndex;
        pszTypeNameFormat = "typenamek_%d_%d";
    }
    else
    {
        iIdNumber = _iSymbolIndex - static_cast<int>(GLSLSymbols::count);
        pszTypeNameFormat = "typename_%d_%d";
    }

    // Generate the HLSL name from the symbol index and scope id
    CHK(_rgHLSLName.Format(s_uHLSLMAXLENGTH, pszTypeNameFormat, declarationScopeId, iIdNumber));

    // Generate the HLSL constructor name from HLSL typename
    CHK(_rgHLSLConstructorName.Format(s_uHLSLMAXLENGTH, "ctor_%s", static_cast<char*>(_rgHLSLName)));
    CHK(_rgHLSLEqualsFunctionName.Format(s_uHLSLMAXLENGTH, "eq_%s", static_cast<char*>(_rgHLSLName)));

    CHK_VERIFY(pTypeDefined->IsStructType());
    _spType = pTypeDefined->AsStructType();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init typename info from a special type enum. This indicates a
//              struct that is defined just to init special variables.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::Initialize(
    const GLSLSpecialTypeInfo &typeInfo,                        // The special type info
    __in GLSLType* pTypeDefined                                 // The type defined by this identifier
    )
{
    CHK_START;

    CHK_VERIFY(pTypeDefined->IsStructType());
    _spType = pTypeDefined->AsStructType();

    // The symbol indices start with the known symbols so we can cast like this - see the
    // CGLSLParser::Initialize function for the code that asserts this.
    _iSymbolIndex = static_cast<int>(typeInfo._symbolEnum);

    // The info has the type name
    CHK(_rgHLSLName.Set(typeInfo._pHLSLName));

    // Generate the HLSL constructor name from HLSL typename
    CHK(_rgHLSLConstructorName.Format(s_uHLSLMAXLENGTH, "ctor_%s", static_cast<char*>(_rgHLSLName)));
    CHK(_rgHLSLEqualsFunctionName.Format(s_uHLSLMAXLENGTH, "eq_%s", static_cast<char*>(_rgHLSLName)));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init typename info for an anonymous user defined type.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::Initialize(
    __in CGLSLParser* pParser,                                  // Parser that we're translating for
    __in GLSLType* pTypeDefined                                 // An anonymous type
    )
{
    CHK_START;


    CHK(_rgHLSLName.Format(s_uHLSLMAXLENGTH, "anontype_%d", pParser->GenerateIdentifierId()));
    CHK(_rgHLSLEqualsFunctionName.Format(s_uHLSLMAXLENGTH, "eq_%s", static_cast<char*>(_rgHLSLName)));

    CHK_VERIFY(pTypeDefined->IsStructType());
    _spType = pTypeDefined->AsStructType();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Init typename info from an existing typename info node. This
//              indicates we're copying from an existing type that does not
//              have easy access to the owning StructSpecifierNode or the
//              CollectionNodeWithScope where it is defined.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::Initialize(
    __in const CTypeNameIdentifierInfo* pOtherTypeInfo          // The parse tree node for the identifier
    )
{
    CHK_START;

    CHK(_rgHLSLName.Set(pOtherTypeInfo->_rgHLSLName));
    CHK(_rgHLSLConstructorName.Set(pOtherTypeInfo->_rgHLSLConstructorName));
    CHK(_rgHLSLEqualsFunctionName.Set(pOtherTypeInfo->_rgHLSLEqualsFunctionName));

    _iSymbolIndex = pOtherTypeInfo->_iSymbolIndex;
    _spType = pOtherTypeInfo->_spType;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::GetType(__deref_out GLSLType** ppType) const
{
    _spType.CopyTo(ppType);
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetHLSLConstructorName
//
//  Synopsis:   Returns the constructor name for this type.
//
//-----------------------------------------------------------------------------
const char* CTypeNameIdentifierInfo::GetHLSLConstructorName() const
{
    Assert(_spType->IsConstructorUsed());
    return _rgHLSLConstructorName;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetHLSLEqualsFunctionName
//
//  Synopsis:   Returns the equals operator function name for this type.
//
//-----------------------------------------------------------------------------
const char* CTypeNameIdentifierInfo::GetHLSLEqualsFunctionName() const
{
    Assert(_spType->IsEqualsOperatorUsed());
    return _rgHLSLEqualsFunctionName;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLConstructor
//
//  Synopsis:   Tell the struct type to output an HLSL function that acts 
//              as a constructor for the typename.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::OutputHLSLConstructor(__in IStringStream* pOutput) const
{
    return _spType->OutputHLSLConstructor(pOutput);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLEqualsFunction
//
//  Synopsis:   Tell the struct type to output an HLSL function that acts 
//              as an equals operator for the type.
//
//-----------------------------------------------------------------------------
HRESULT CTypeNameIdentifierInfo::OutputHLSLEqualsFunction(__in IStringStream* pOutput) const
{
    return _spType->OutputHLSLEqualsFunction(pOutput);
}
