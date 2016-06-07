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
#include "BasicGLSLType.hxx"
#include "RefCounted.hxx"
#include "TypeHelpers.hxx"
#include "GLSL.tab.h"
#include "WebGLConstants.hxx"
#include "GLSLTypeInfo.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::Initialize(int basicType)
{
    _basicType = basicType;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsSampler2DType
//
//  Synopsis:   Returns true if the type is a sampler 2D.
//
//-----------------------------------------------------------------------------
bool BasicGLSLType::IsSampler2DType() const
{
    return (_basicType == SAMPLER2D);
}

//+----------------------------------------------------------------------------
//
//  Function:   IsSamplerCubeType
//
//  Synopsis:   Returns true if the type is a cube sampler.
//
//-----------------------------------------------------------------------------
bool BasicGLSLType::IsSamplerCubeType() const
{
    return (_basicType == SAMPLERCUBE);
}

//+----------------------------------------------------------------------------
//
//  Function:   IsTypeOrArrayOfType
//
//  Synopsis:   Determines if the type is the given basic type, or an array
//              of the given basic type.
//
//-----------------------------------------------------------------------------
bool BasicGLSLType::IsTypeOrArrayOfType(int basicType) const
{
    return (basicType == _basicType);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetElementCount
//
//  Synopsis:   Returns 1 for non array types and array size for array types.
//
//-----------------------------------------------------------------------------
int BasicGLSLType::GetElementCount() const
{
    return 1;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetRowCount
//
//  Synopsis:   Returns the number of rows this basic type fills
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::GetRowCount(__out UINT* puRowCount) const
{
    *puRowCount = TypeHelpers::GetRowCount(_basicType);
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetBasicType
//
//  Synopsis:   Get the basic type token value if this is a basic type.
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::GetBasicType(__out int* pBasicType) const
{
    (*pBasicType) = _basicType;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualType
//
//  Synopsis:   Test equality of two types
//
//-----------------------------------------------------------------------------
bool BasicGLSLType::IsEqualType(__in const GLSLType* pOther) const
{
    bool fEqual = false;
    if (pOther->IsBasicType())
    {
        fEqual = (_basicType == pOther->AsBasicType()->GetBasicType());
    }

    return fEqual;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualTypeForUniforms
//
//  Synopsis:   Test equality of two types for shared uniforms. Given that
//              tokens are the same across different parses, we can just 
//              do the normal type comparison.
//
//-----------------------------------------------------------------------------
bool BasicGLSLType::IsEqualTypeForUniforms(
    __in const CGLSLIdentifierTable* pIdTableThis,      // Identifier table for this type
    __in const GLSLType* pOther,                        // Other type to compare
    __in const CGLSLIdentifierTable* pIdTableOther      // Identifier table for other type
    ) const
{
    return IsEqualType(pOther);
}


//+----------------------------------------------------------------------------
//
//  Function:   GetGLType
//
//  Synopsis:   Based on the token type, converts to and returns a GLConstants
//              type.
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::GetGLType(__out GLConstants::Type* pglType) const
{
    CHK_START;

    GLConstants::Type glType;

    switch(_basicType)
    {
        case FLOAT_TOK:
            glType = GLConstants::FLOAT;
            break;
        case VEC2:
            glType = GLConstants::FLOAT_VEC2;
            break;
        case VEC3:
            glType = GLConstants::FLOAT_VEC3;
            break;
        case VEC4:
            glType = GLConstants::FLOAT_VEC4;
            break;
        case INT_TOK:
            glType = GLConstants::INT;
            break;
        case IVEC2_TOK:
            glType = GLConstants::INT_VEC2;
            break;
        case IVEC3_TOK:
            glType = GLConstants::INT_VEC3;
            break;
        case IVEC4_TOK:
            glType = GLConstants::INT_VEC4;
            break;
        case BOOL_TOK:
            glType = GLConstants::BOOL;
            break;
        case BVEC2_TOK:
            glType = GLConstants::BOOL_VEC2;
            break;
        case BVEC3_TOK:
            glType = GLConstants::BOOL_VEC3;
            break;
        case BVEC4_TOK:
            glType = GLConstants::BOOL_VEC4;
            break;
        case MAT2_TOK:
            glType = GLConstants::FLOAT_MAT2;
            break;
        case MAT3_TOK:
            glType = GLConstants::FLOAT_MAT3;
            break;
        case MAT4_TOK:
            glType = GLConstants::FLOAT_MAT4;
            break;
        case SAMPLER2D:
            glType = GLConstants::SAMPLER_2D;
            break;
        case SAMPLERCUBE:
            glType = GLConstants::SAMPLER_CUBE;
            break;
        default:
            CHK_VERIFY_MSG(false, "Unexpected _basicType for BasicGLSLType queried for GL type");
            break;
    }

    *pglType = glType;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   EnumerateActiveInfoForType
//
//  Synopsis:   Adds a single entry to aryActiveInfo for this basic type.
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::EnumerateActiveInfoForType(
    __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
    __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
    ) const
{
    CHK_START;

    GLConstants::Type glType;
    CHK(GetGLType(&glType));

    // Basic types have no name suffix contribution, its glType is 
    // derived from the basic type, and the array size is 1.
    CGLSLActiveInfo<char> activeInfo;
    activeInfo.SetGLType(glType);
    activeInfo.SetArraySize(1);

    CHK(aryActiveInfo.Add(activeInfo));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDefaultInitValue
//
//  Synopsis:   Returns a string that contains the default HLSL value for
//              the component of this type. Should not be called on sampler
//              or void types, as there are no values to init with.
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLType::GetDefaultInitValue(__deref_out PCSTR* ppszDefaultInit) const
{
    CHK_START;

    // The component type defines the init value for a basic type.
    // This will fail for sampler or void types, which should be checked by the caller.
    int compType;
    CHK_VERIFY(SUCCEEDED(TypeHelpers::GetComponentType(_basicType, &compType)));

    const BasicGLSLTypeInfo* pCompTypeInfo;
    CHK(BasicGLSLTypeInfo::GetInfo(compType, &pCompTypeInfo));

    *ppszDefaultInit = pCompTypeInfo->_pszDefaultInit;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetHLSLTypeName
//
//  Synopsis:   Returns a string that represents the basic type's HLSL type.
//
//-----------------------------------------------------------------------------
PCSTR BasicGLSLType::GetHLSLTypeName() const
{
    // We should always be able to get typeinfo for a basic type that was validly constructed
    const BasicGLSLTypeInfo* pTypeInfo;
    Verify(SUCCEEDED(BasicGLSLTypeInfo::GetInfo(_basicType, &pTypeInfo)));
    return pTypeInfo->_pszHLSLName;
}
