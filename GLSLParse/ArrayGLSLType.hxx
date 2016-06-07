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

//+-----------------------------------------------------------------------------
//
//  Struct:     ArrayGLSLType
//
//  Synopsis:   Object to represent an array of a GLSL type.
//
//------------------------------------------------------------------------------
class ArrayGLSLType : public GLSLType
{
public:
    // GLSLType overrides
    bool IsArrayType() const override { return true; }

    ArrayGLSLType* AsArrayType() override { return this; }
    const ArrayGLSLType* AsArrayType() const override { return this; }

    bool IsSampler2DType() const override;
    bool IsSamplerCubeType() const override;
    bool IsTypeOrArrayOfType(int basicType) const override;
    bool IsStructOrArrayOfStructs() const override { return _spType->IsStructType(); }

    HRESULT GetBasicType(__out int* pBasicType) const override { return E_UNEXPECTED; }
    HRESULT GetArrayElementType(__deref_out GLSLType** ppType) const override;
    HRESULT GetArraySize(__out int* pArraySize) const override;

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

    int GetArraySize() const { return _arraySize; }

    UINT GetStructNestingLevel() const override { return _spType->GetStructNestingLevel(); }

protected:
    ArrayGLSLType() {}

    HRESULT Initialize(__in GLSLType* pType, int arraySize);

private:
    TSmartPointer<GLSLType> _spType;                                    // The underlying type
    int _arraySize;                                                     // The size of the array (if it is an array) or -1 if not an array
};
