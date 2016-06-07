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
//  Struct:     BasicGLSLType
//
//  Synopsis:   Object to represent a GLSL type.
//
//              Basic types are the built in types (like float, vec2, etc).
//
//------------------------------------------------------------------------------
class BasicGLSLType : public GLSLType
{
public:
    // GLSLType overrides
    bool IsBasicType() const override { return true; }

    BasicGLSLType* AsBasicType() override { return this; }
    const BasicGLSLType* AsBasicType() const override { return this; }

    bool IsSampler2DType() const override;
    bool IsSamplerCubeType() const override;
    bool IsTypeOrArrayOfType(int basicType) const override;
    bool IsStructOrArrayOfStructs() const override { return false; }

    HRESULT GetBasicType(__out int* pBasicType) const override;
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

    HRESULT GetGLType(__out GLConstants::Type* pglType) const override;

    HRESULT EnumerateActiveInfoForType(
        __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
        __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
        ) const override;

    UINT GetStructNestingLevel() const { return 0; }

    int GetBasicType() const { return _basicType; }

    HRESULT GetDefaultInitValue(__deref_out PCSTR* ppszDefaultInit) const;

    PCSTR GetHLSLTypeName() const;

protected:
    BasicGLSLType() {}

    HRESULT Initialize(int basicType);

private:
    int _basicType;                                                     // The basic type (like int or float) using the Bison tag
};
