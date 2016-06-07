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
#include "TypeHelpers.hxx"
#include "ParseTreeNode.hxx"
#include "GLSL.tab.h"

namespace TypeHelpers
{
    //+----------------------------------------------------------------------------
    //
    //  Function:   IsVectorComponentType
    //
    //  Synopsis:   Figure out if the given token type corresponds to a type that
    //              can be a component of a GLSL vector.
    //
    //-----------------------------------------------------------------------------
    bool IsVectorComponentType(int type)
    {
        switch(type)
        {
        case FLOAT_TOK:
        case INT_TOK:
        case BOOL_TOK:
            return true;
        }

        return false;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsNumericScalarType
    //
    //  Synopsis:   Figure out if the given token type corresponds to a scalar type
    //              that has a numeric representation.
    //
    //-----------------------------------------------------------------------------
    bool IsNumericScalarType(int type)
    {
        switch(type)
        {
        case FLOAT_TOK:
        case INT_TOK:
            return true;
        }

        return false;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsMatrixType
    //
    //  Synopsis:   Figure out if the given token type corresponds to a GLSL matrix
    //              type.
    //
    //-----------------------------------------------------------------------------
    bool IsMatrixType(int type)
    {
        switch(type)
        {
        case MAT2_TOK:
        case MAT3_TOK:
        case MAT4_TOK:
            return true;
        }

        return false;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsVectorType
    //
    //  Synopsis:   Figure out if the given token type corresponds to a GLSL vector
    //              type.
    //
    //-----------------------------------------------------------------------------
    bool IsVectorType(int type)
    {
        switch(type)
        {
        case VEC2:
        case VEC3:
        case VEC4:
        case IVEC2_TOK:
        case IVEC3_TOK:
        case IVEC4_TOK:
        case BVEC2_TOK:
        case BVEC3_TOK:
        case BVEC4_TOK:
            return true;
        }

        return false;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsTextureType
    //
    //  Synopsis:   Figure out if the given token type corresponds to a texture or
    //              a sampler type.
    //
    //-----------------------------------------------------------------------------
    bool IsTextureType(int type)
    {
        switch(type)
        {
        case SAMPLER2D:
        case SAMPLERCUBE:
            return true;
        }

        return false;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetVectorLength
    //
    //  Synopsis:   Get the length of a vector type. Assumes that the type is
    //              a vector type.
    //
    //-----------------------------------------------------------------------------
    UINT GetVectorLength(int type)
    {
        Assert(IsVectorType(type));

        switch(type)
        {
        case VEC2:
        case IVEC2_TOK:
        case BVEC2_TOK:
            return 2;

        case VEC3:
        case IVEC3_TOK:
        case BVEC3_TOK:
            return 3;

        case VEC4:
        case IVEC4_TOK:
        case BVEC4_TOK:
            return 4;

        default:
            return 0;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetMatrixLength
    //
    //  Synopsis:   Get the length of a matrix type. Assumes that the type is
    //              a matrix type.
    //
    //-----------------------------------------------------------------------------
    UINT GetMatrixLength(int type)
    {
        Assert(IsMatrixType(type));

        switch(type)
        {
        case MAT2_TOK:
            return 2;

        case MAT3_TOK:
            return 3;

        case MAT4_TOK:
            return 4;

        default:
            return 0;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetNumMatrixComponents
    //
    //  Synopsis:   Get the number of components in a matrix type. Assumes that 
    //              the type is a matrix type.
    //
    //-----------------------------------------------------------------------------
    UINT GetNumMatrixComponents(int type)
    {
        Assert(IsMatrixType(type));

        switch(type)
        {
        case MAT2_TOK:
            return 4;

        case MAT3_TOK:
            return 9;

        case MAT4_TOK:
            return 16;

        default:
            return 0;
        }
    }
    //+----------------------------------------------------------------------------
    //
    //  Function:   GetMatrixColumnType
    //
    //  Synopsis:   For a given matrix type, return the type of the columns of the
    //              matrix.
    //
    //-----------------------------------------------------------------------------
    int GetMatrixColumnType(int type)
    {
        Assert(IsMatrixType(type));

        switch(type)
        {
        case MAT2_TOK:
            return VEC2;

        case MAT3_TOK:
            return VEC3;

        case MAT4_TOK:
            return VEC4;

        default:
            return NO_TYPE;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetComponentType
    //
    //  Synopsis:   For vectorized types like matrices and vectors, get the type
    //              of the component that it is made up of.
    //
    //-----------------------------------------------------------------------------
    HRESULT GetComponentType(int vType, __out int* pType)
    {
        switch(vType)
        {
        case BOOL_TOK:
        case BVEC2_TOK:
        case BVEC3_TOK:
        case BVEC4_TOK:
            (*pType) = BOOL_TOK;
            return S_OK;

        case FLOAT_TOK:
        case VEC2:
        case VEC3:
        case VEC4:
        case MAT2_TOK:
        case MAT3_TOK:
        case MAT4_TOK:
            (*pType) = FLOAT_TOK;
            return S_OK;

        case INT_TOK:
        case IVEC2_TOK:
        case IVEC3_TOK:
        case IVEC4_TOK:
            (*pType) = INT_TOK;
            return S_OK;

        default:
            return E_INVALIDARG;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetVectorType
    //
    //  Synopsis:   Given a component type and a number of components, returns
    //              the type with that many components.
    //
    //-----------------------------------------------------------------------------
    HRESULT GetVectorType(int cType, int numc, __out int* pVType)
    {
        CHK_START;

        CHKB(numc > 0 && numc <= 4);

        static const int intTypes[4] = { INT_TOK, IVEC2_TOK, IVEC3_TOK, IVEC4_TOK };
        static const int floatTypes[4] = { FLOAT_TOK, VEC2, VEC3, VEC4 };
        static const int boolTypes[4] = { BOOL_TOK, BVEC2_TOK, BVEC3_TOK, BVEC4_TOK };

        switch(cType)
        {
        case INT_TOK:
            (*pVType) = intTypes[numc - 1];
            break;

        case FLOAT_TOK:
            (*pVType) = floatTypes[numc - 1];
            break;

        case BOOL_TOK:
            (*pVType) = boolTypes[numc - 1];
            break;

        default:
            Assert(false);
            CHK(E_INVALIDARG);
        }

        CHK_RETURN;
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GetRowCount
    //
    //  Synopsis:   Get the count of rows - every type is 1, except for matrices.
    //
    //-----------------------------------------------------------------------------
    UINT GetRowCount(int type)
    {
        switch(type)
        {
        case MAT2_TOK:
            return 2;

        case MAT3_TOK:
            return 3;

        case MAT4_TOK:
            return 4;

        default:
            return 1;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsConstructableType
    //
    //  Synopsis:   Only certain types are constructable. This is specified in the
    //              GLES grammar, but we parse it anyways. This must be checked
    //              as part of verifying a basic constructor.
    //
    //-----------------------------------------------------------------------------
    bool IsConstructableType(int basicType)
    {
        switch (basicType)
        {
        case SAMPLER2D:
        case SAMPLERCUBE:
        case VOID_TOK:
            return false;
        default:
            return true;
        }
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   IsValidGenType
    //
    //  Synopsis:   According the GLES spec, genType can only be one of
    //              float, vec2, vec3, or vec4.
    //
    //-----------------------------------------------------------------------------
    bool IsValidGenType(int basicType)
    {
        switch (basicType)
        {
        case FLOAT_TOK:
        case VEC2:
        case VEC3:
        case VEC4:
            return true;
        default:
            return false;
        }

    }
}
