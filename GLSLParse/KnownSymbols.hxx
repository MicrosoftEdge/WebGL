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

#include "ParseTreeNode.hxx"
#include <foundation/collections.hxx>
#include "GLSLShaderType.hxx"
#include "GLSLSignatureType.hxx"

//+----------------------------------------------------------------------------
//
//  Enum:       HLSLFunctions
//
//  Synopsis:   Enumeration of all HLSL functions that we translate GLSL
//              functions into.
//
//-----------------------------------------------------------------------------
namespace HLSLFunctions
{
    enum Enum
    {
        // Trigonometry
        radians = 0,
        degrees = 1,
        sin = 2,
        cos = 3,
        tan = 4,
        asin = 5,
        acos = 6,
        atan = 7,
        atan2 = 8,

        // Exponential
        pow = 9,
        exp = 10,
        log = 11,
        exp2 = 12,
        log2 = 13,
        sqrt = 14,
        rsqrt = 15,

        // Common
        abs = 16,
        GLSLsign = 17,
        floor = 18,
        ceil = 19,
        frac = 20,
        GLSLmod = 21,
        min = 22,
        max = 23,
        clamp = 24,
        lerp = 25, 
        step = 26,
        smoothstep = 27,

        // Geometric
        length = 28,
        distance = 29,
        dot = 30,
        cross = 31,
        normalize = 32,
        faceforward = 33,
        reflect = 34,
        refract = 35,

        // Matrix
        GLSLmatrixCompMult = 36,
        GLSLmatrixCompMult_1 = 37,
        GLSLmatrixCompMult_2 = 38,

        // Vector relational
        GLSLlessThan = 39,
        GLSLlessThan_1 = 40,
        GLSLlessThanEqual = 41,
        GLSLlessThanEqual_1 = 42,
        GLSLgreaterThan = 43,
        GLSLgreaterThan_1 = 44,
        GLSLgreaterThanEqual = 45,
        GLSLgreaterThanEqual_1 = 46,
        GLSLequal = 47,
        GLSLequal_1 = 48,
        GLSLequal_2 = 49,
        GLSLnotEqual = 50,
        GLSLnotEqual_1 = 51,
        GLSLnotEqual_2 = 52,
        any = 53,
        all = 54,
        GLSLnot = 55,
        GLSLnot_1 = 56,
        GLSLnot_2 = 57,

        // Texture lookup
        GLSLtexture2D = 58,
        GLSLtexture2DBias = 59,
        GLSLtexture2DLod = 60,
        GLSLtexture2DProj = 61,
        GLSLtexture2DProj_1 = 62,
        GLSLtexture2DProjBias = 63,
        GLSLtexture2DProjBias_1 = 64,
        GLSLtexture2DProjLod = 65,
        GLSLtexture2DProjLod_1 = 66,
        GLSLtextureCube = 67,
        GLSLtextureCubeBias = 68,
        GLSLtextureCubeLod = 69,

        // Derivatives
        ddx = 70,
        ddy = 71,
        GLSLfwidth = 72,

        count = 73
    };
}

namespace GLSLSymbols
{
    enum Enum
    {
        // Basics
        main = 0,

        // Trigonometry
        radians,
        degrees,
        sin, 
        cos, 
        tan, 
        asin,
        acos,
        atan,

        // Exponential
        pow,
        exp,
        log,
        exp2,
        log2,
        sqrt,
        inversesqrt,

        // Common
        abs,
        sign, 
        floor,
        ceil, 
        fract,
        mod,
        min,
        max,
        clamp,
        mix,
        step, 
        smoothstep,

        // Geometric
        length,
        distance,
        dot,
        cross,
        normalize,
        faceforward,
        reflect,
        refract,

        // Matrix
        matrixCompMult,

        // Vector relational
        lessThan,
        lessThanEqual,
        greaterThan,
        greaterThanEqual,
        equal,
        notEqual,
        any,
        all,
        not,

        // Texture
        texture2D,
        texture2DLod,
        texture2DProj,
        texture2DProjLod,
        textureCube,
        textureCubeLod, 

        // GL_OES_standard_derivatives
        dFdx,
        dFdy,
        fwidth,

        // Vertex stage variables
        glPosition,
        glPointSize,

        // Fragment stage variables
        glFragColor,
        glFragData, 
        glFragCoord,
        glPointCoord, 
        glFrontFacing,
        glFragDepth,

        // Constants
        glMaxVertexAttribs,
        glMaxVertexUniformVectors,
        glMaxVaryingVectors,
        glMaxVertexTextureImageUnits,
        glMaxCombinedTextureImageUnits,
        glMaxTextureImageUnits,
        glMaxFragmentUniformVectors,
        glMaxDrawBuffers,

        // Types
        glDepthRangeParameters,

        // Uniforms
        glDepthRange,

        count
    };
}

//+----------------------------------------------------------------------------
//
//  Enum:       GLSLFunctions
//
//  Synopsis:   Enumeration of all GLSL functions that we recognize and
//              translate. Overloads are done with number suffixes.
//
//-----------------------------------------------------------------------------
namespace GLSLFunctions
{
    enum Enum
    {
        // Basics
        main = 0,

        // Trigonometry
        radians = 1,
        degrees = 2,
        sin = 3,
        cos = 4,
        tan = 5,
        asin = 6,
        acos = 7,
        atan = 8,
        atan_1 = 9,

        // Exponential
        pow = 10,
        exp = 11,
        log = 12,
        exp2 = 13,
        log2 = 14,
        sqrt = 15,
        inversesqrt = 16,

        // Common
        abs = 17,
        sign = 18,
        floor = 19,
        ceil = 20,
        fract = 21,
        mod = 22,
        mod_1 = 23,
        min = 24,
        min_1 = 25,
        max = 26,
        max_1 = 27,
        clamp = 28,
        clamp_1 = 29,
        mix = 30,
        mix_1 = 31,
        step = 32,
        step_1 = 33,
        smoothstep = 34,
        smoothstep_1 = 35,

        // Geometric
        length = 36,
        distance = 37,
        dot = 38,
        cross = 39,
        normalize = 40,
        faceforward = 41,
        reflect = 42,
        refract = 43,

        // Matrix
        matrixCompMult = 44,
        matrixCompMult_1 = 45,
        matrixCompMult_2 = 46,

        // Vector relational
        lessThan = 47,
        lessThan_1 = 48,
        lessThanEqual = 49,
        lessThanEqual_1 = 50,
        greaterThan = 51,
        greaterThan_1 = 52,
        greaterThanEqual = 53,
        greaterThanEqual_1 = 54,
        equal = 55,
        equal_1 = 56,
        equal_2 = 57,
        notEqual = 58,
        notEqual_1 = 59,
        notEqual_2 = 60,
        any = 61,
        all = 62,
        not = 63,
        not_1 = 64,
        not_2 = 65,

        // Texture
        texture2D = 66,
        texture2D_1 = 67,
        texture2DLod = 68,
        texture2DProj = 69,
        texture2DProj_1 = 70,
        texture2DProj_2 = 71,
        texture2DProj_3 = 72,
        texture2DProjLod = 73,
        texture2DProjLod_1 = 74,
        textureCube = 75,
        textureCube_1 = 76, 
        textureCubeLod = 77,

        // Derivatives
        dFdx = 78,
        dFdy = 79,
        fwidth = 80,

        count = 81
    };  
}       

//+----------------------------------------------------------------------------
//
//  Enum:       GLSLSpecialTypes
//
//  Synopsis:   Enumeration of all types that GLSL special variables need to be
//              defined.
//
//-----------------------------------------------------------------------------
namespace GLSLSpecialTypes
{
    enum Enum
    {
        glDepthRangeParameters = 0,
        count
    };
}


//+----------------------------------------------------------------------------
//
//  Enum:       GLSLSpecialVariables
//
//  Synopsis:   Enumeration of all GLSL special variables that we recognize and
//              translate.
//
//-----------------------------------------------------------------------------
namespace GLSLSpecialVariables
{
    enum Enum
    {
        glPosition = 0,
        glPointSize,
        glFragColor,
        glFragData,
        glFragCoord,
        glPointCoord,
        glFrontFacing,
        glFragDepth,
        glMaxVertexAttribs,
        glMaxVertexUniformVectors,
        glMaxVaryingVectors,
        glMaxVertexTextureImageUnits,
        glMaxCombinedTextureImageUnits,
        glMaxTextureImageUnits,
        glMaxFragmentUniformVectors,
        glMaxDrawBuffers,
        glDepthRange,
        count
    };
}

//+----------------------------------------------------------------------------
//
//  Struct:     HLSLFunctionInfo
//
//  Synopsis:   Struct to represent information about HLSL functions that
//              we translate GLSL functions into.
//
//-----------------------------------------------------------------------------
struct HLSLFunctionInfo
{
    typedef HLSLFunctions::Enum IndexType;                      // Type that collections of this info are indexed by

    HLSLFunctionInfo(
        HLSLFunctions::Enum indexEnum,                          // Enum used for this info
        __in const char* pHLSLName,                             // Name to use in HLSL
        int type,                                               // Return type
        int numArgs,                                            // The number of arguments
        ...                                                     // Types of arguments
        );

    HLSLFunctions::Enum _indexEnum;                             // Enum used for this info
    const char* _pHLSLName;                                     // Name to use in HLSL
    int _type;                                                  // Return type
    int _numArgs;                                               // The number of arguments
    int _rgArgTypes[4];                                         // Types of arguments

    const static HLSLFunctionInfo s_info[HLSLFunctions::count]; // Collection of the info
};

//+----------------------------------------------------------------------------
//
//  Struct:     GLSLSymbolInfo
//
//  Synopsis:   Struct to represent information about GLSL symbols.
//
//-----------------------------------------------------------------------------
struct GLSLSymbolInfo
{
    typedef GLSLSymbols::Enum IndexType;                        // Type that collections of this info are indexed by

    GLSLSymbolInfo(
        GLSLSymbols::Enum indexEnum,                            // Enum of GLSL symbol text
        __in const char* pGLSLName                              // Actual GLSL symbol text
        );

    GLSLSymbols::Enum _indexEnum;                               // Enum of GLSL symbol text
    const char* _pGLSLName;                                     // Actual GLSL symbol text

    const static GLSLSymbolInfo s_info[GLSLSymbols::count];     // Collection of the info
};

//+----------------------------------------------------------------------------
//
//  Enum:       GLSLIdentifierFlags
//
//  Synopsis:   Flags to represent metadata for known functions.
//
//-----------------------------------------------------------------------------
enum class GLSLIdentifierFlags : UINT
{
	None            =   0,
    FragmentOnly    =   0x1,
    VertexOnly      =   0x2,
    DerivOnly       =   0x4,
    FragDepthOnly   =   0x8,
};

DEFINE_ENUM_FLAG_OPERATORS(GLSLIdentifierFlags);

//+----------------------------------------------------------------------------
//
//  Struct:     GLSLFunctionInfo
//
//  Synopsis:   Struct to represent information about GLSL functions that
//              we recognize and translate.
//
//-----------------------------------------------------------------------------
struct GLSLFunctionInfo
{
    typedef GLSLFunctions::Enum IndexType;                      // Type that collections of this info are indexed by

    GLSLFunctionInfo(
        GLSLFunctions::Enum indexEnum,                          // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                           // Enum of GLSL symbol text of function
        HLSLFunctions::Enum hlslEnum,                           // Enum of equivalent HLSL function
        UINT uFlags,                                            // Flags for the function (GLSLFunctionFlags)
        int type,                                               // Return type for functions
        int numArgs,                                            // The number of arguments
        ...                                                     // Types of arguments
        );

    GLSLFunctionInfo(
        GLSLFunctions::Enum indexEnum,                          // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                           // Enum of GLSL symbol text of function
        HLSLFunctions::Enum hlslEnum,                           // Enum of equivalent HLSL function
        UINT uFlags,                                            // Flags for the function (GLSLFunctionFlags)
        GLSLSignatureType::Enum sigType                         // Signature type
        );

    GLSLFunctions::Enum _indexEnum;                             // Enum used for this info
    GLSLSymbols::Enum _symbolEnum;                              // Enum of GLSL symbol text of function
    HLSLFunctions::Enum _hlslEnum;                              // Enum of equivalent HLSL function
    UINT _uFlags;                                               // Flags for the function
    int _type;                                                  // Return type for functions
    int _numArgs;                                               // The number of arguments
    int _rgArgTypes[4];                                         // Types of arguments
    GLSLSignatureType::Enum _sigType;                           // Signature type

    const static GLSLFunctionInfo s_info[GLSLFunctions::count]; // Collection of the info
};

//+----------------------------------------------------------------------------
//
//  Struct:     GLSLSpecialTypeInfo
//
//  Synopsis:   Struct to represent information about GLSL special variable 
//              types that we recognize and translate.
//
//-----------------------------------------------------------------------------
struct GLSLSpecialTypeInfo
{
    DELETE_COPYCONSTR_AND_ASSIGNMENT(GLSLSpecialTypeInfo);

    typedef GLSLSpecialTypes::Enum IndexType;                           // Type that collections of this info are indexed by

    struct FieldInfo
    {
        int _type;                                                      // Type of field
        const char* _pszName;                                           // Name of field
    };

    GLSLSpecialTypeInfo(
        GLSLSpecialTypes::Enum indexEnum,                               // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                                   // Enum of GLSL symbol text of identifier
        __in const char* pHLSLName,                                     // Name to use in HLSL
        int numFields,                                                  // Number of fields
        ...                                                             // Types and names of fields
        );

    const GLSLSpecialTypes::Enum _indexEnum;                            // Enum used for this info
    const GLSLSymbols::Enum _symbolEnum;                                // Enum of GLSL symbol text of identifier
    const char* _pHLSLName;                                             // Name to use in HLSL
    int _numFields;                                                     // Number of fields
    FieldInfo _rgFields[3];                                             // Field information

    const static GLSLSpecialTypeInfo s_info[GLSLSpecialTypes::count];   // Collection of the info
};

//+----------------------------------------------------------------------------
//
//  Struct:     GLSLSpecialVariableInfo
//
//  Synopsis:   Struct to represent information about GLSL special variables 
//              that we recognize and translate.
//
//-----------------------------------------------------------------------------
class GLSLSpecialVariableInfo
{
public:
    typedef GLSLSpecialVariables::Enum IndexType;                               // Type that collections of this info are indexed by

    GLSLSpecialVariableInfo(
        GLSLSpecialVariables::Enum indexEnum,                                   // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                                           // Enum of GLSL symbol text of identifier
        __in const char* pHLSLName,                                             // Name to use in HLSL
		GLSLIdentifierFlags uFlags,                                             // Flags for the variable
        int type,                                                               // The type of the identifier
        int qualifier,                                                          // The qualifier for the identifier
        int arraySize                                                           // The size of the array for the identifier
        );

    GLSLSpecialVariableInfo(
        GLSLSpecialVariables::Enum indexEnum,                                   // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                                           // Enum of GLSL symbol text of identifier
        __in const char* pHLSLName,                                             // Name to use in HLSL
        int type,                                                               // The type of the identifier
        int qualifier,                                                          // The qualifier for the identifier
        int arraySize                                                           // The size of the array for the identifier
        );

    GLSLSpecialVariableInfo(
        GLSLSpecialVariables::Enum indexEnum,                                   // Enum used for this info
        GLSLSymbols::Enum symbolEnum,                                           // Enum of GLSL symbol text of identifier
        __in const char* pHLSLName,                                             // Name to use in HLSL
        GLSLSpecialTypes::Enum typeEnum,                                        // Special type of identifier
        int qualifier                                                           // The qualifier for the identifier
        );

    DELETE_COPYCONSTR_AND_ASSIGNMENT(GLSLSpecialVariableInfo);

    const GLSLSpecialVariables::Enum _indexEnum;                                // Enum used for this info
    const GLSLSymbols::Enum _symbolEnum;                                        // Enum of GLSL symbol text of identifier
    const char* _pHLSLName;                                                     // Name to use in HLSL
    const int _qualifier;                                                       // The qualifier for the identifier
    const int _arraySize;                                                       // The size of the array for the identifier

    const static GLSLSpecialVariableInfo s_info[GLSLSpecialVariables::count];   // Collection of the info

    bool IsInScope(GLSLShaderType::Enum shaderType) const;
    HRESULT GetBasicType(__out int* pType) const;
    HRESULT GetSpecialType(__out GLSLSpecialTypes::Enum* pSpecialType) const;

    const int _type;                                                            // The type of the identifier (only valid if _fSpecialTypeValid is false)
    const GLSLSpecialTypes::Enum _specialType;                                  // The type (only valid if _fSpecialTypeValid is true)
    const bool _fSpecialTypeValid;                                              // Whether we use a special type enum for the type
    const GLSLIdentifierFlags _uFlags;                                          // Flags for the variable
};

//+----------------------------------------------------------------------------
//
//  Namespace:  GLSLKnownSymbols
//
//  Synopsis:   GLSL has some special identifiers that need special handling
//              during translation. Their information is stored and accessed
//              here.
//
//              During parsing these are added to the symbol table to be
//              handled specially.
//
//-----------------------------------------------------------------------------
namespace GLSLKnownSymbols
{
    template<typename E>
    const E& GetKnownInfo(typename E::IndexType index)
    {
        // This is a good time to enforce the consistency of our table and enum
        static_assert(ARRAYSIZE(E::s_info) == E::IndexType::count, "Mismatch in array size and enum count");

        // count is a valid enum value, but not a valid thing to index with
        Assert(index != E::IndexType::count);

        // Make sure the table ordering is right
        Assert(E::s_info[index]._indexEnum == index);

        return E::s_info[index];
    }
};

