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
#include "GLSLTypeInfo.hxx"
#include "GLSL.tab.h"

const BasicGLSLTypeInfo BasicGLSLTypeInfo::s_typeInfo[] = {
    { FLOAT_TOK,        "float",        "float",                nullptr,                "0.0",      1,  FLOAT_TOK   },
    { VEC2,             "vec2",         "float2",               nullptr,                nullptr,    2,  FLOAT_TOK   },
    { VEC3,             "vec3",         "float3",               nullptr,                nullptr,    3,  FLOAT_TOK   },
    { VEC4,             "vec4",         "float4",               nullptr,                nullptr,    4,  FLOAT_TOK   },
    { INT_TOK,          "int",          "int",                  nullptr,                "0",        1,  INT_TOK     },
    { IVEC2_TOK,        "ivec2",        "int2",                 nullptr,                nullptr,    2,  INT_TOK     },
    { IVEC3_TOK,        "ivec3",        "int3",                 nullptr,                nullptr,    3,  INT_TOK     },
    { IVEC4_TOK,        "ivec4",        "int4",                 nullptr,                nullptr,    4,  INT_TOK     },
    { BOOL_TOK,         "bool",         "bool",                 nullptr,                "false",    1,  BOOL_TOK    },
    { BVEC2_TOK,        "bvec2",        "bool2",                nullptr,                nullptr,    2,  BOOL_TOK    },
    { BVEC3_TOK,        "bvec3",        "bool3",                nullptr,                nullptr,    3,  BOOL_TOK    },
    { BVEC4_TOK,        "bvec4",        "bool4",                nullptr,                nullptr,    4,  BOOL_TOK    },
    { MAT2_TOK,         "mat2",         "float2x2",             nullptr,                nullptr,    4,  FLOAT_TOK   },
    { MAT3_TOK,         "mat3",         "float3x3",             nullptr,                nullptr,    9,  FLOAT_TOK   },
    { MAT4_TOK,         "mat4",         "float4x4",             nullptr,                nullptr,    16, FLOAT_TOK   },
    { VOID_TOK,         "void",         "void",                 nullptr,                nullptr,    0,  NO_TYPE     },
    { SAMPLER2D,        "sampler2D",    "SamplerState",         "Texture2D<float4>",    nullptr,    1,  SAMPLER2D   },
    { SAMPLERCUBE,      "samplerCube",  "SamplerState",         "TextureCube<float4>",  nullptr,    1,  SAMPLERCUBE },
};

//+----------------------------------------------------------------------------
//
//  Function:   GetInfo
//
//  Synopsis:   Get the type information for the given type.
//
//-----------------------------------------------------------------------------
HRESULT BasicGLSLTypeInfo::GetInfo(
    int type,                                   // Type to get info for
    __deref_out const BasicGLSLTypeInfo** pInfo // Info for that type
    )
{
    CHK_START;

    int i;
    for (i = 0 ; i < ARRAYSIZE(s_typeInfo); i++)
    {
        if (s_typeInfo[i]._type == type)
        {
            (*pInfo) = &s_typeInfo[i];
            break;
        }
    }

    if (i == ARRAYSIZE(s_typeInfo))
    {
        // This indicates that our parser is pushing a type into the tree
        // that we don't understand, and it is a bug.
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    CHK_RETURN;
}
