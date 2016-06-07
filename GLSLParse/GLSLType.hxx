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

#include "GLSLActiveInfo.hxx"

namespace GLConstants
{
    enum Type;
};

interface IIdentifierInfo;

class VariableIdentifierNode;
class CVariableIdentifierInfo;
class CGLSLIdentifierTable;

class BasicGLSLType;
class ArrayGLSLType;
class StructGLSLType;

//+-----------------------------------------------------------------------------
//
//  Class:      GLSLType
//
//  Synopsis:   Abstract class that represents a GLSL type.
//
//              Provides creation functions that create types based on bison
//              token or another type and an array size.
//
//------------------------------------------------------------------------------
class GLSLType : public IUnknown
{
public:
    virtual bool IsBasicType() const { return false; }
    virtual bool IsArrayType() const { return false; }
    virtual bool IsStructType() const { return false; }

    virtual BasicGLSLType* AsBasicType() { return nullptr; }
    virtual const BasicGLSLType* AsBasicType() const { return nullptr; }
    virtual ArrayGLSLType* AsArrayType() { return nullptr; }
    virtual const ArrayGLSLType* AsArrayType() const { return nullptr; }
    virtual StructGLSLType* AsStructType() { return nullptr; }
    virtual const StructGLSLType* AsStructType() const { return nullptr; }

    virtual bool IsSampler2DType() const = 0;
    virtual bool IsSamplerCubeType() const = 0;
    virtual bool IsTypeOrArrayOfType(int basicType) const = 0;
    virtual bool IsStructOrArrayOfStructs() const = 0;

    virtual HRESULT GetBasicType(__out int* pBasicType) const = 0;
    virtual HRESULT GetArrayElementType(__deref_out GLSLType** ppType) const = 0;
    virtual HRESULT GetArraySize(__out int* pArraySize) const = 0;

    virtual int GetElementCount() const = 0;

    virtual bool IsEqualType(__in const GLSLType* pOther) const = 0;
    virtual bool IsEqualTypeForUniforms(
        __in const CGLSLIdentifierTable* pIdTableThis,                  // Identifier table for this type
        __in const GLSLType* pOther,                                    // Other type to compare
        __in const CGLSLIdentifierTable* pIdTableOther                  // Identifier table for other type
        ) const = 0;

    virtual HRESULT GetGLType(__out GLConstants::Type* pglType) const = 0;
    virtual HRESULT GetRowCount(__out UINT* puRowCount) const = 0;

    virtual UINT GetStructNestingLevel() const = 0;

    virtual HRESULT EnumerateActiveInfoForType(
        __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
        __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
        ) const = 0;

    static HRESULT CreateFromBasicTypeToken(int basicType, __deref_out GLSLType** ppNewType);
    static HRESULT CreateFromType(__in GLSLType* pType, int arraySize, __deref_out GLSLType** ppNewType);
    static HRESULT CreateStructTypeFromIdentifierInfoAry(
        const CModernArray<TSmartPointer<IIdentifierInfo>>& aryIdInfo,  // List of variable identifiers that define struct fields
        __deref_out StructGLSLType** ppNewType                          // Newly created struct type
        );
};
