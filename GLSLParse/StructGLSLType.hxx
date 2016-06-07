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
#pragma once

#include "GLSLType.hxx"
#include "VariableIdentifierInfo.hxx"

class CTypeNameIdentifierInfo;

//+-----------------------------------------------------------------------------
//
//  Class:      StructGLSLType
//
//  Synopsis:   Object to represent a user defined GLSL type.
//
//------------------------------------------------------------------------------
class StructGLSLType : public GLSLType
{
public:
    // GLSLType overrides
    bool IsStructType() const override { return true; }

    StructGLSLType* AsStructType() override { return this; }
    const StructGLSLType* AsStructType() const override { return this; }

    bool IsSampler2DType() const override { return false; }
    bool IsSamplerCubeType() const override { return false; }
    bool IsTypeOrArrayOfType(int basicType) const override { return false; }
    bool IsStructOrArrayOfStructs() const override { return true; }

    HRESULT GetBasicType(__out int* pBasicType) const override { return E_UNEXPECTED; }
    HRESULT GetArrayElementType(__deref_out GLSLType** ppType) const override { return E_UNEXPECTED; }
    HRESULT GetArraySize(__out int* pArraySize) const override { return E_UNEXPECTED; }

    int GetElementCount() const override;
    HRESULT GetRowCount(__out UINT* puRowCount) const override;

    bool IsEqualType(__in const GLSLType* pOther) const override;
    bool IsEqualTypeForUniforms(
        __in const CGLSLIdentifierTable* pIdTableThis,                  // Identifier table for this type
        __in const GLSLType* pOther,                                    // Other type to compare
        __in const CGLSLIdentifierTable* pIdTableOther                  // Identifier table for other type
        ) const override;

    HRESULT GetGLType(__out GLConstants::Type* pglType) const override { return E_UNEXPECTED; }

    HRESULT EnumerateActiveInfoForType(
        __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
        __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
        ) const override;

    HRESULT GetFieldInfoForVariable(
        __in const VariableIdentifierNode* pVariableIdent,              // Variable to lookup by name
        __deref_out CVariableIdentifierInfo** ppVariableInfo            // Found identifier info
        ) const;

    HRESULT GetFieldInfoForSymbol(
        int iSymbolIndex,                                               // Symbol to lookup
        __deref_out CVariableIdentifierInfo** ppVariableInfo            // Found identifier info
        ) const;

    bool TypesMatchForConstructor(__in const CModernArray<TSmartPointer<GLSLType>>& aryCtorArgTypes) const;

    void FinalizeTypeWithTypeInfo(__in CTypeNameIdentifierInfo* pTypeNameInfo);
    void ClearTypeInfoPointer() { _pTypeNameInfo = nullptr; }
    const CTypeNameIdentifierInfo* UseTypeNameInfo() const { return _pTypeNameInfo; }
    HRESULT OutputHLSLConstructor(__in IStringStream* pOutput) const;
    HRESULT OutputHLSLEqualsFunction(__in IStringStream* pOutput) const;

    bool IsConstructorUsed() const { return _fConstructorUsed; }
    void SetConstructorUsed() { _fConstructorUsed = true; }
    bool IsEqualsOperatorUsed() const { return _fEqualsOperatorUsed; }
    void SetEqualsOperatorUsed();

    UINT GetStructNestingLevel() const override;

    bool ContainsArrayType() const;

protected:
    StructGLSLType();

    HRESULT Initialize(const CModernArray<TSmartPointer<IIdentifierInfo>>& aryFields);

private:
    //+-----------------------------------------------------------------------------
    //  Struct:     VariableInfoWithName
    //  Synopsis:   Points to a field's CVariableIdentifierInfo with the name
    //              string associated with the symbol index.
    //------------------------------------------------------------------------------
    struct VariableInfoWithName
    {
        TSmartPointer<CVariableIdentifierInfo> spInfo;
        const char* pszName;
    };

    HRESULT GetVariableInfoWithNames(
        __in const CGLSLIdentifierTable* pIdTable,                      // Id table for symbols in this type's field variable infos
        __inout CModernArray<VariableInfoWithName>& aryVarInfoWithNames // Array to fill with VariableInfoWithName structs
        ) const;

    HRESULT OutputHLSLVariablesAsParameters(__in IStringStream* pOutput) const;
    HRESULT OutputHLSLVariablesAsAssignments(__in const char* pszLocalStructVarName, __in IStringStream* pOutput) const;

private:
    CModernArray<TSmartPointer<CVariableIdentifierInfo>> _aryFields;    // Array that describes the variable info for this struct type
    const CTypeNameIdentifierInfo* _pTypeNameInfo;                      // The typename that this struct type was declared for
    bool _fConstructorUsed;                                             // Whether the constructor for this type is used
    bool _fEqualsOperatorUsed;                                          // Whether the equals operator for this type is used

    static const UINT s_cMaxNestingLevel;                               // A constant that represents the maximum nesting level for struct types
    static const UINT s_cchMaxFieldName;                                // The max length of a fieldname for reflection purposes
};
