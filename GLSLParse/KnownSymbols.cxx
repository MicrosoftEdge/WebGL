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
#include "KnownSymbols.hxx"
#include "GLSL.tab.h"

//+----------------------------------------------------------------------------
//
//  Member:     HLSLFunctionInfo::s_info
//
//  Synopsis:   List of HLSL functions that we output to, with the signature
//              that the function expects to be used with.
//
//              The order of the functions must match the HLSLFunctions enum.
//
//-----------------------------------------------------------------------------
const HLSLFunctionInfo HLSLFunctionInfo::s_info[HLSLFunctions::count] = 
{
    // Synopsis
    //               indexEnum                              pHLSLName               type        numArgs

    // Trigonometry
    HLSLFunctionInfo(HLSLFunctions::radians,                "radians",              GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::degrees,                "degrees",              GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::sin,                    "sin",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::cos,                    "cos",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::tan,                    "tan",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::asin,                   "asin",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::acos,                   "acos",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::atan,                   "atan",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::atan2,                  "atan2",                GENTYPE,    1,  GENTYPE, GENTYPE),
    
    // Exponential
    HLSLFunctionInfo(HLSLFunctions::pow,                    "pow",                  GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::exp,                    "exp",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::log,                    "log",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::exp2,                   "exp2",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::log2,                   "log2",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::sqrt,                   "sqrt",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::rsqrt,                  "rsqrt",                GENTYPE,    1,  GENTYPE),

    // Common
    HLSLFunctionInfo(HLSLFunctions::abs,                    "abs",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLsign,               "GLSLsign",             GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::floor,                  "floor",                GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::ceil,                   "ceil",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::frac,                   "frac",                 GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLmod,                "GLSLmod",              GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::min,                    "min",                  GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::max,                    "max",                  GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::clamp,                  "clamp",                GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::lerp,                   "lerp",                 GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::step,                   "step",                 GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::smoothstep,             "smoothstep",           FLOAT_TOK,  3,  FLOAT_TOK, FLOAT_TOK, FLOAT_TOK),

    // Geometric
    HLSLFunctionInfo(HLSLFunctions::length,                 "length",               FLOAT_TOK,  1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::distance,               "distance",             FLOAT_TOK,  2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::dot,                    "dot",                  FLOAT_TOK,  2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::cross,                  "cross",                VEC3,       2,  VEC3, VEC3),
    HLSLFunctionInfo(HLSLFunctions::normalize,              "normalize",            GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::faceforward,            "faceforward",          GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::reflect,                "reflect",              GENTYPE,    2,  GENTYPE, GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::refract,                "refract",              GENTYPE,    3,  GENTYPE, GENTYPE, FLOAT_TOK),

    // Matrix
    HLSLFunctionInfo(HLSLFunctions::GLSLmatrixCompMult,     "GLSLmatrixCompMult",   MAT2_TOK,   2,  MAT2_TOK, MAT2_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLmatrixCompMult_1,   "GLSLmatrixCompMult",   MAT3_TOK,   2,  MAT3_TOK, MAT3_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLmatrixCompMult_2,   "GLSLmatrixCompMult",   MAT4_TOK,   2,  MAT4_TOK, MAT4_TOK),

    // Vector relational
    HLSLFunctionInfo(HLSLFunctions::GLSLlessThan,           "GLSLlessThan",         BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLlessThan_1,         "GLSLlessThan",         BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLlessThanEqual,      "GLSLlessThanEqual",    BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLlessThanEqual_1,    "GLSLlessThanEqual",    BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLgreaterThan,        "GLSLgreaterThan",      BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLgreaterThan_1,      "GLSLgreaterThan",      BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLgreaterThanEqual,   "GLSLgreaterThanEqual", BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLgreaterThanEqual_1, "GLSLgreaterThanEqual", BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLequal,              "GLSLequal",            BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLequal_1,            "GLSLequal",            BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLequal_2,            "GLSLequal",            BVECTYPE,   2,  BVECTYPE, BVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLnotEqual,           "GLSLnotEqual",         BVECTYPE,   2,  VECTYPE, VECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLnotEqual_1,         "GLSLnotEqual",         BVECTYPE,   2,  IVECTYPE, IVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLnotEqual_2,         "GLSLnotEqual",         BVECTYPE,   2,  BVECTYPE, BVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::any,                    "any",                  BOOL_TOK,   1,  BVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::all,                    "all",                  BOOL_TOK,   2,  BVECTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLnot,                "GLSLnot",              BVEC2_TOK,  1,  BVEC2_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLnot_1,              "GLSLnot",              BVEC3_TOK,  1,  BVEC3_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLnot_2,              "GLSLnot",              BVEC4_TOK,  1,  BVEC4_TOK),

    // Texture
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2D,          "GLSLtexture2D",        VEC4,       2,  SAMPLER2D, VEC2),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DBias,      "GLSLtexture2DBias",    VEC4,       3,  SAMPLER2D, VEC2, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DLod,       "GLSLtexture2DLod",     VEC4,       3,  SAMPLER2D, VEC2, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProj,      "GLSLtexture2DProj",    VEC4,       2,  SAMPLER2D, VEC3),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProj_1,    "GLSLtexture2DProj",    VEC4,       2,  SAMPLER2D, VEC4),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProjBias,  "GLSLtexture2DProjBias",VEC4,       3,  SAMPLER2D, VEC3, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProjBias_1,"GLSLtexture2DProjBias",VEC4,       3,  SAMPLER2D, VEC4, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProjLod,   "GLSLtexture2DProjLod", VEC4,       3,  SAMPLER2D, VEC3, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtexture2DProjLod_1, "GLSLtexture2DProjLod", VEC4,       3,  SAMPLER2D, VEC4, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtextureCube,        "GLSLtextureCube",      VEC4,       2,  SAMPLERCUBE, VEC3),
    HLSLFunctionInfo(HLSLFunctions::GLSLtextureCubeBias,    "GLSLtextureCubeBias",  VEC4,       3,  SAMPLERCUBE, VEC3, FLOAT_TOK),
    HLSLFunctionInfo(HLSLFunctions::GLSLtextureCubeLod,     "GLSLtextureCubeLod",   VEC4,       3,  SAMPLERCUBE, VEC3, FLOAT_TOK),

    // Derivatives
    HLSLFunctionInfo(HLSLFunctions::ddx,                    "ddx",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::ddy,                    "ddy",                  GENTYPE,    1,  GENTYPE),
    HLSLFunctionInfo(HLSLFunctions::GLSLfwidth,             "GLSLfwidth",           GENTYPE,    1,  GENTYPE),
};

//+----------------------------------------------------------------------------
//
//  Member:     GLSLSymbolInfo::s_info
//
//  Synopsis:   List of known GLSL symbols that the table is seeded with. 
//
//              The order must match the GLSLSymbol enum which is used to 
//              index the array.
//
//-----------------------------------------------------------------------------
const GLSLSymbolInfo GLSLSymbolInfo::s_info[GLSLSymbols::count] =
{
    // Basics
    GLSLSymbolInfo(GLSLSymbols::main,                               "main"),

    // Trigonometry
    GLSLSymbolInfo(GLSLSymbols::radians,                            "radians"),
    GLSLSymbolInfo(GLSLSymbols::degrees,                            "degrees"),          
    GLSLSymbolInfo(GLSLSymbols::sin,                                "sin"),              
    GLSLSymbolInfo(GLSLSymbols::cos,                                "cos"),              
    GLSLSymbolInfo(GLSLSymbols::tan,                                "tan"),              
    GLSLSymbolInfo(GLSLSymbols::asin,                               "asin"),             
    GLSLSymbolInfo(GLSLSymbols::acos,                               "acos"),             
    GLSLSymbolInfo(GLSLSymbols::atan,                               "atan"),             

    // Exponential
    GLSLSymbolInfo(GLSLSymbols::pow,                                "pow"),              
    GLSLSymbolInfo(GLSLSymbols::exp,                                "exp"),              
    GLSLSymbolInfo(GLSLSymbols::log,                                "log"),              
    GLSLSymbolInfo(GLSLSymbols::exp2,                               "exp2"),             
    GLSLSymbolInfo(GLSLSymbols::log2,                               "log2"),             
    GLSLSymbolInfo(GLSLSymbols::sqrt,                               "sqrt"),             
    GLSLSymbolInfo(GLSLSymbols::inversesqrt,                        "inversesqrt"),      

    // Common
    GLSLSymbolInfo(GLSLSymbols::abs,                                "abs"),              
    GLSLSymbolInfo(GLSLSymbols::sign,                               "sign"),             
    GLSLSymbolInfo(GLSLSymbols::floor,                              "floor"),            
    GLSLSymbolInfo(GLSLSymbols::ceil,                               "ceil"),             
    GLSLSymbolInfo(GLSLSymbols::fract,                              "fract"),            
    GLSLSymbolInfo(GLSLSymbols::mod,                                "mod"),              
    GLSLSymbolInfo(GLSLSymbols::min,                                "min"),              
    GLSLSymbolInfo(GLSLSymbols::max,                                "max"),              
    GLSLSymbolInfo(GLSLSymbols::clamp,                              "clamp"),            
    GLSLSymbolInfo(GLSLSymbols::mix,                                "mix"),              
    GLSLSymbolInfo(GLSLSymbols::step,                               "step"),             
    GLSLSymbolInfo(GLSLSymbols::smoothstep,                         "smoothstep"),       

    // Geometric
    GLSLSymbolInfo(GLSLSymbols::length,                             "length"),           
    GLSLSymbolInfo(GLSLSymbols::distance,                           "distance"),         
    GLSLSymbolInfo(GLSLSymbols::dot,                                "dot"),              
    GLSLSymbolInfo(GLSLSymbols::cross,                              "cross"),            
    GLSLSymbolInfo(GLSLSymbols::normalize,                          "normalize"),        
    GLSLSymbolInfo(GLSLSymbols::faceforward,                        "faceforward"),      
    GLSLSymbolInfo(GLSLSymbols::reflect,                            "reflect"),          
    GLSLSymbolInfo(GLSLSymbols::refract,                            "refract"),          

    // Matrix
    GLSLSymbolInfo(GLSLSymbols::matrixCompMult,                     "matrixCompMult"),   

    // Vector relational
    GLSLSymbolInfo(GLSLSymbols::lessThan,                           "lessThan"),         
    GLSLSymbolInfo(GLSLSymbols::lessThanEqual,                      "lessThanEqual"),    
    GLSLSymbolInfo(GLSLSymbols::greaterThan,                        "greaterThan"),      
    GLSLSymbolInfo(GLSLSymbols::greaterThanEqual,                   "greaterThanEqual"), 
    GLSLSymbolInfo(GLSLSymbols::equal,                              "equal"),            
    GLSLSymbolInfo(GLSLSymbols::notEqual,                           "notEqual"),         
    GLSLSymbolInfo(GLSLSymbols::any,                                "any"),              
    GLSLSymbolInfo(GLSLSymbols::all,                                "all"),              
    GLSLSymbolInfo(GLSLSymbols::not,                                "not"),              

    // Texture
    GLSLSymbolInfo(GLSLSymbols::texture2D,                          "texture2D"),        
    GLSLSymbolInfo(GLSLSymbols::texture2DLod,                       "texture2DLod"),     
    GLSLSymbolInfo(GLSLSymbols::texture2DProj,                      "texture2DProj"),    
    GLSLSymbolInfo(GLSLSymbols::texture2DProjLod,                   "texture2DProjLod"), 
    GLSLSymbolInfo(GLSLSymbols::textureCube,                        "textureCube"),      
    GLSLSymbolInfo(GLSLSymbols::textureCubeLod,                     "textureCubeLod"),    

    // GL_OES_standard_derivatives
    GLSLSymbolInfo(GLSLSymbols::dFdx,                               "dFdx"),             
    GLSLSymbolInfo(GLSLSymbols::dFdy,                               "dFdy"),             
    GLSLSymbolInfo(GLSLSymbols::fwidth,                             "fwidth"),           

    // Vertex stage variables
    GLSLSymbolInfo(GLSLSymbols::glPosition,                         "gl_Position"),
    GLSLSymbolInfo(GLSLSymbols::glPointSize,                        "gl_PointSize"),

    // Fragment stage variables
    GLSLSymbolInfo(GLSLSymbols::glFragColor,                        "gl_FragColor"),
    GLSLSymbolInfo(GLSLSymbols::glFragData,                         "gl_FragData"),
    GLSLSymbolInfo(GLSLSymbols::glFragCoord,                        "gl_FragCoord"),
    GLSLSymbolInfo(GLSLSymbols::glPointCoord,                       "gl_PointCoord"),
    GLSLSymbolInfo(GLSLSymbols::glFrontFacing,                      "gl_FrontFacing"),
    GLSLSymbolInfo(GLSLSymbols::glFragDepth,                        "gl_FragDepthEXT"),

    // Constant variables
    GLSLSymbolInfo(GLSLSymbols::glMaxVertexAttribs,                 "gl_MaxVertexAttribs"),
    GLSLSymbolInfo(GLSLSymbols::glMaxVertexUniformVectors,          "gl_MaxVertexUniformVectors"),
    GLSLSymbolInfo(GLSLSymbols::glMaxVaryingVectors,                "gl_MaxVaryingVectors"),
    GLSLSymbolInfo(GLSLSymbols::glMaxVertexTextureImageUnits,       "gl_MaxVertexTextureImageUnits"),
    GLSLSymbolInfo(GLSLSymbols::glMaxCombinedTextureImageUnits,     "gl_MaxCombinedTextureImageUnits"),
    GLSLSymbolInfo(GLSLSymbols::glMaxTextureImageUnits,             "gl_MaxTextureImageUnits"),
    GLSLSymbolInfo(GLSLSymbols::glMaxFragmentUniformVectors,        "gl_MaxFragmentUniformVectors"),
    GLSLSymbolInfo(GLSLSymbols::glMaxDrawBuffers,                   "gl_MaxDrawBuffers"),

    // Types
    GLSLSymbolInfo(GLSLSymbols::glDepthRangeParameters,             "gl_DepthRangeParameters"),

    // Uniforms
    GLSLSymbolInfo(GLSLSymbols::glDepthRange,                       "gl_DepthRange"),
};

//+----------------------------------------------------------------------------
//
//  Member:     g_rgGLSLFunctions
//
//  Synopsis:   List of known GLSL functions that the table is seeded with. 
//
//              The order must match the GLSLFunctionInfo enum which is used to 
//              index the array.
//
//-----------------------------------------------------------------------------
const GLSLFunctionInfo GLSLFunctionInfo::s_info[GLSLFunctions::count] =
{
    // Synopsis
    //               indexEnum                          symbolEnum                      hlslEnum                                uFlags                                  type        numArgs

    // Basics
    GLSLFunctionInfo(GLSLFunctions::main,               GLSLSymbols::main,              HLSLFunctions::count,                   0,                                      VOID_TOK,   0),

    // Trigonometry
    GLSLFunctionInfo(GLSLFunctions::radians,            GLSLSymbols::radians,           HLSLFunctions::radians,                 0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::degrees,            GLSLSymbols::degrees,           HLSLFunctions::degrees,                 0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::sin,                GLSLSymbols::sin,               HLSLFunctions::sin,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::cos,                GLSLSymbols::cos,               HLSLFunctions::cos,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::tan,                GLSLSymbols::tan,               HLSLFunctions::tan,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::asin,               GLSLSymbols::asin,              HLSLFunctions::asin,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::acos,               GLSLSymbols::acos,              HLSLFunctions::acos,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::atan,               GLSLSymbols::atan,              HLSLFunctions::atan,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::atan_1,             GLSLSymbols::atan,              HLSLFunctions::atan2,                   0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),

    // Exponential
    GLSLFunctionInfo(GLSLFunctions::pow,                GLSLSymbols::pow,               HLSLFunctions::pow,                     0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::exp,                GLSLSymbols::exp,               HLSLFunctions::exp,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::log,                GLSLSymbols::log,               HLSLFunctions::log,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::exp2,               GLSLSymbols::exp2,              HLSLFunctions::exp2,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::log2,               GLSLSymbols::log2,              HLSLFunctions::log2,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::sqrt,               GLSLSymbols::sqrt,              HLSLFunctions::sqrt,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::inversesqrt,        GLSLSymbols::inversesqrt,       HLSLFunctions::rsqrt,                   0,                                      GENTYPE,    1,  GENTYPE),

    // Common
    GLSLFunctionInfo(GLSLFunctions::abs,                GLSLSymbols::abs,               HLSLFunctions::abs,                     0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::sign,               GLSLSymbols::sign,              HLSLFunctions::GLSLsign,                0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::floor,              GLSLSymbols::floor,             HLSLFunctions::floor,                   0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::ceil,               GLSLSymbols::ceil,              HLSLFunctions::ceil,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::fract,              GLSLSymbols::fract,             HLSLFunctions::frac,                    0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::mod,                GLSLSymbols::mod,               HLSLFunctions::GLSLmod,                 0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::mod_1,              GLSLSymbols::mod,               HLSLFunctions::GLSLmod,                 0,                                      GENTYPE,    2,  GENTYPE, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::min,                GLSLSymbols::min,               HLSLFunctions::min,                     0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::min_1,              GLSLSymbols::min,               HLSLFunctions::min,                     0,                                      GENTYPE,    2,  GENTYPE, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::max,                GLSLSymbols::max,               HLSLFunctions::max,                     0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::max_1,              GLSLSymbols::max,               HLSLFunctions::max,                     0,                                      GENTYPE,    2,  GENTYPE, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::clamp,              GLSLSymbols::clamp,             HLSLFunctions::clamp,                   0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::clamp_1,            GLSLSymbols::clamp,             HLSLFunctions::clamp,                   0,                                      GENTYPE,    3,  GENTYPE, FLOAT_TOK, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::mix,                GLSLSymbols::mix,               HLSLFunctions::lerp,                    0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::mix_1,              GLSLSymbols::mix,               HLSLFunctions::lerp,                    0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::step,               GLSLSymbols::step,              HLSLFunctions::step,                    0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::step_1,             GLSLSymbols::step,              HLSLFunctions::step,                    0,                                      GENTYPE,    2,  FLOAT_TOK, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::smoothstep,         GLSLSymbols::smoothstep,        HLSLFunctions::smoothstep,              0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::smoothstep_1,       GLSLSymbols::smoothstep,        HLSLFunctions::smoothstep,              0,                                      GENTYPE,    3,  FLOAT_TOK, FLOAT_TOK, GENTYPE),

    // Geometric
    GLSLFunctionInfo(GLSLFunctions::length,             GLSLSymbols::length,            HLSLFunctions::length,                  0,                                      FLOAT_TOK,  1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::distance,           GLSLSymbols::distance,          HLSLFunctions::distance,                0,                                      FLOAT_TOK,  2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::dot,                GLSLSymbols::dot,               HLSLFunctions::dot,                     0,                                      FLOAT_TOK,  2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::cross,              GLSLSymbols::cross,             HLSLFunctions::cross,                   0,                                      VEC3,       2,  VEC3, VEC3),
    GLSLFunctionInfo(GLSLFunctions::normalize,          GLSLSymbols::normalize,         HLSLFunctions::normalize,               0,                                      GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::faceforward,        GLSLSymbols::faceforward,       HLSLFunctions::faceforward,             0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::reflect,            GLSLSymbols::reflect,           HLSLFunctions::reflect,                 0,                                      GENTYPE,    2,  GENTYPE, GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::refract,            GLSLSymbols::refract,           HLSLFunctions::refract,                 0,                                      GENTYPE,    3,  GENTYPE, GENTYPE, FLOAT_TOK),

    // Matrix
    GLSLFunctionInfo(GLSLFunctions::matrixCompMult,     GLSLSymbols::matrixCompMult,    HLSLFunctions::GLSLmatrixCompMult,      0,                                      MAT2_TOK,   2,  MAT2_TOK, MAT2_TOK),
    GLSLFunctionInfo(GLSLFunctions::matrixCompMult_1,   GLSLSymbols::matrixCompMult,    HLSLFunctions::GLSLmatrixCompMult,      0,                                      MAT3_TOK,   2,  MAT3_TOK, MAT3_TOK),
    GLSLFunctionInfo(GLSLFunctions::matrixCompMult_2,   GLSLSymbols::matrixCompMult,    HLSLFunctions::GLSLmatrixCompMult,      0,                                      MAT4_TOK,   2,  MAT4_TOK, MAT4_TOK),

    // Vector relational
    GLSLFunctionInfo(GLSLFunctions::lessThan,           GLSLSymbols::lessThan,          HLSLFunctions::GLSLlessThan,            0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::lessThan_1,         GLSLSymbols::lessThan,          HLSLFunctions::GLSLlessThan_1,          0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::lessThanEqual,      GLSLSymbols::lessThanEqual,     HLSLFunctions::GLSLlessThanEqual,       0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::lessThanEqual_1,    GLSLSymbols::lessThanEqual,     HLSLFunctions::GLSLlessThanEqual_1,     0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::greaterThan,        GLSLSymbols::greaterThan,       HLSLFunctions::GLSLgreaterThan,         0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::greaterThan_1,      GLSLSymbols::greaterThan,       HLSLFunctions::GLSLgreaterThan_1,       0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::greaterThanEqual,   GLSLSymbols::greaterThanEqual,  HLSLFunctions::GLSLgreaterThanEqual,    0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::greaterThanEqual_1, GLSLSymbols::greaterThanEqual,  HLSLFunctions::GLSLgreaterThanEqual_1,  0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::equal,              GLSLSymbols::equal,             HLSLFunctions::GLSLequal,               0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::equal_1,            GLSLSymbols::equal,             HLSLFunctions::GLSLequal_1,             0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::equal_2,            GLSLSymbols::equal,             HLSLFunctions::GLSLequal_1,             0,                                      GLSLSignatureType::CompareBoolVector),
    GLSLFunctionInfo(GLSLFunctions::notEqual,           GLSLSymbols::notEqual,          HLSLFunctions::GLSLnotEqual,            0,                                      GLSLSignatureType::CompareFloatVector),
    GLSLFunctionInfo(GLSLFunctions::notEqual_1,         GLSLSymbols::notEqual,          HLSLFunctions::GLSLnotEqual_1,          0,                                      GLSLSignatureType::CompareIntVector),
    GLSLFunctionInfo(GLSLFunctions::notEqual_2,         GLSLSymbols::notEqual,          HLSLFunctions::GLSLnotEqual_1,          0,                                      GLSLSignatureType::CompareBoolVector),
    GLSLFunctionInfo(GLSLFunctions::any,                GLSLSymbols::any,               HLSLFunctions::any,                     0,                                      GLSLSignatureType::TestBoolVector),
    GLSLFunctionInfo(GLSLFunctions::all,                GLSLSymbols::all,               HLSLFunctions::all,                     0,                                      GLSLSignatureType::TestBoolVector),
    GLSLFunctionInfo(GLSLFunctions::not,                GLSLSymbols::not,               HLSLFunctions::GLSLnot,                 0,                                      BVEC2_TOK,  1,  BVEC2_TOK),
    GLSLFunctionInfo(GLSLFunctions::not_1,              GLSLSymbols::not,               HLSLFunctions::GLSLnot_1,               0,                                      BVEC3_TOK,  1,  BVEC3_TOK),
    GLSLFunctionInfo(GLSLFunctions::not_2,              GLSLSymbols::not,               HLSLFunctions::GLSLnot_2,               0,                                      BVEC4_TOK,  1,  BVEC4_TOK),
              
    // Texture
    GLSLFunctionInfo(GLSLFunctions::texture2D,          GLSLSymbols::texture2D,         HLSLFunctions::GLSLtexture2D,           0,                                      VEC4,       2,  SAMPLER2D, VEC2),
    GLSLFunctionInfo(GLSLFunctions::texture2D_1,        GLSLSymbols::texture2D,         HLSLFunctions::GLSLtexture2DBias,       (UINT)GLSLIdentifierFlags::FragmentOnly,VEC4,       3,  SAMPLER2D, VEC2, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::texture2DLod,       GLSLSymbols::texture2DLod,      HLSLFunctions::GLSLtexture2DLod,        (UINT)GLSLIdentifierFlags::VertexOnly,  VEC4,       3,  SAMPLER2D, VEC2, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::texture2DProj,      GLSLSymbols::texture2DProj,     HLSLFunctions::GLSLtexture2DProj,       0,                                      VEC4,       2,  SAMPLER2D, VEC3),
    GLSLFunctionInfo(GLSLFunctions::texture2DProj_1,    GLSLSymbols::texture2DProj,     HLSLFunctions::GLSLtexture2DProj_1,     0,                                      VEC4,       2,  SAMPLER2D, VEC4),
    GLSLFunctionInfo(GLSLFunctions::texture2DProj_2,    GLSLSymbols::texture2DProj,     HLSLFunctions::GLSLtexture2DProjBias,   (UINT)GLSLIdentifierFlags::FragmentOnly,VEC4,       3,  SAMPLER2D, VEC3, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::texture2DProj_3,    GLSLSymbols::texture2DProj,     HLSLFunctions::GLSLtexture2DProjBias_1, (UINT)GLSLIdentifierFlags::FragmentOnly,VEC4,       3,  SAMPLER2D, VEC4, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::texture2DProjLod,   GLSLSymbols::texture2DProjLod,  HLSLFunctions::GLSLtexture2DProjLod,    (UINT)GLSLIdentifierFlags::VertexOnly,  VEC4,       3,  SAMPLER2D, VEC3, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::texture2DProjLod_1, GLSLSymbols::texture2DProjLod,  HLSLFunctions::GLSLtexture2DProjLod_1,  (UINT)GLSLIdentifierFlags::VertexOnly,  VEC4,       3,  SAMPLER2D, VEC4, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::textureCube,        GLSLSymbols::textureCube,       HLSLFunctions::GLSLtextureCube,         0,                                      VEC4,       2,  SAMPLERCUBE, VEC3),
    GLSLFunctionInfo(GLSLFunctions::textureCube_1,      GLSLSymbols::textureCube,       HLSLFunctions::GLSLtextureCubeBias,     (UINT)GLSLIdentifierFlags::FragmentOnly,VEC4,       3,  SAMPLERCUBE, VEC3, FLOAT_TOK),
    GLSLFunctionInfo(GLSLFunctions::textureCubeLod,     GLSLSymbols::textureCubeLod,    HLSLFunctions::GLSLtextureCubeLod,      (UINT)GLSLIdentifierFlags::VertexOnly,  VEC4,       3,  SAMPLERCUBE, VEC3, FLOAT_TOK), 
                                  
    // GL_OES_standard_derivatives
    GLSLFunctionInfo(GLSLFunctions::dFdx,               GLSLSymbols::dFdx,              HLSLFunctions::ddx,                     (UINT)GLSLIdentifierFlags::DerivOnly,   GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::dFdy,               GLSLSymbols::dFdy,              HLSLFunctions::ddy,                     (UINT)GLSLIdentifierFlags::DerivOnly,   GENTYPE,    1,  GENTYPE),
    GLSLFunctionInfo(GLSLFunctions::fwidth,             GLSLSymbols::fwidth,            HLSLFunctions::GLSLfwidth,              (UINT)GLSLIdentifierFlags::DerivOnly,   GENTYPE,    1,  GENTYPE),
};                                  

//+----------------------------------------------------------------------------
//                                  
//  Member:     s_info
//
//  Synopsis:   List of GLSL special variables types
//
//              The order must match the GLSLSpecialType enum which is used to 
//              index the array.
//
//-----------------------------------------------------------------------------
const GLSLSpecialTypeInfo GLSLSpecialTypeInfo::s_info[GLSLSpecialTypes::count] =
{
    {GLSLSpecialTypes::glDepthRangeParameters,                   GLSLSymbols::glDepthRangeParameters,            "gl_DepthRangeParameters",          3,  FLOAT_TOK,  "near", FLOAT_TOK,  "far",  FLOAT_TOK,  "diff"},
};

//+----------------------------------------------------------------------------
//                                  
//  Member:     s_info
//
//  Synopsis:   List of GLSL special variables.
//
//              The order must match the GLSLKnownSymbols enum which is used to 
//              index the array.
//
//-----------------------------------------------------------------------------
const GLSLSpecialVariableInfo GLSLSpecialVariableInfo::s_info[GLSLSpecialVariables::count] =
{
    // Vertex stage variables
    {GLSLSpecialVariables::glPosition,                       GLSLSymbols::glPosition,                        "flippedPosition",                  GLSLIdentifierFlags::VertexOnly,		VEC4,       NO_QUALIFIER,   -1},
    {GLSLSpecialVariables::glPointSize,                      GLSLSymbols::glPointSize,                       "gl_PointSize",                     GLSLIdentifierFlags::VertexOnly,		FLOAT_TOK,  NO_QUALIFIER,   -1},

    // Fragment stage variables
    {GLSLSpecialVariables::glFragColor,                      GLSLSymbols::glFragColor,                       "psOutput.fragColor[0]",            GLSLIdentifierFlags::FragmentOnly,		VEC4,       NO_QUALIFIER,   -1},
    {GLSLSpecialVariables::glFragData,                       GLSLSymbols::glFragData,                        "psOutput.fragColor",               GLSLIdentifierFlags::FragmentOnly,		VEC4,       NO_QUALIFIER,   1},
    {GLSLSpecialVariables::glFragCoord,                      GLSLSymbols::glFragCoord,                       "gl_FragCoord",                     GLSLIdentifierFlags::FragmentOnly,		VEC4,       NO_QUALIFIER,   -1},
    {GLSLSpecialVariables::glPointCoord,                     GLSLSymbols::glPointCoord,                      "gl_PointCoord",                    GLSLIdentifierFlags::FragmentOnly,		VEC2,       CONST_TOK,      -1},
    {GLSLSpecialVariables::glFrontFacing,                    GLSLSymbols::glFrontFacing,                     "gl_FrontFacing",                   GLSLIdentifierFlags::FragmentOnly,		BOOL_TOK,   CONST_TOK,      -1},
    {GLSLSpecialVariables::glFragDepth,                      GLSLSymbols::glFragDepth,                       "psOutput.fragDepth",               GLSLIdentifierFlags::FragmentOnly | GLSLIdentifierFlags::FragDepthOnly,    FLOAT_TOK,  NO_QUALIFIER,   -1},

    // Constants
    {GLSLSpecialVariables::glMaxVertexAttribs,               GLSLSymbols::glMaxVertexAttribs,                "gl_MaxVertexAttribs",              INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxVertexUniformVectors,        GLSLSymbols::glMaxVertexUniformVectors,         "gl_MaxVertexUniformVectors",       INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxVaryingVectors,              GLSLSymbols::glMaxVaryingVectors,               "gl_MaxVaryingVectors",             INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxVertexTextureImageUnits,     GLSLSymbols::glMaxVertexTextureImageUnits,      "gl_MaxVertexTextureImageUnits",    INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxCombinedTextureImageUnits,   GLSLSymbols::glMaxCombinedTextureImageUnits,    "gl_MaxCombinedTextureImageUnits",  INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxTextureImageUnits,           GLSLSymbols::glMaxTextureImageUnits,            "gl_MaxTextureImageUnits",          INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxFragmentUniformVectors,      GLSLSymbols::glMaxFragmentUniformVectors,       "gl_MaxFragmentUniformVectors",     INT_TOK,    CONST_TOK,      -1},
    {GLSLSpecialVariables::glMaxDrawBuffers,                 GLSLSymbols::glMaxDrawBuffers,                  "gl_MaxDrawBuffers",                INT_TOK,    CONST_TOK,      -1},

    // Uniforms
    {GLSLSpecialVariables::glDepthRange,                     GLSLSymbols::glDepthRange,                      "gl_DepthRange",                    GLSLSpecialTypes::glDepthRangeParameters, CONST_TOK},
};

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
HLSLFunctionInfo::HLSLFunctionInfo(
    HLSLFunctions::Enum indexEnum,              // Enum used for this info
    __in const char* pHLSLName,                 // Name to use in HLSL
    int type,                                   // Return type
    int numArgs,                                // The number of arguments
    ...                                         // Types of arguments
    )
{
    _indexEnum = indexEnum;
    _pHLSLName = pHLSLName;
    _type = type;

    va_list args;
    va_start(args, numArgs);

    // Only support fixed max args - make _rgArgTypes bigger if you want more
    const static int MAX_ARGS = ARRAYSIZE(_rgArgTypes);
    Assert(numArgs <= MAX_ARGS);
    _numArgs = (numArgs <= MAX_ARGS) ? numArgs : MAX_ARGS;

    // Pull the argument types out of the variable arg list
    int i;
    for (i = 0; i < _numArgs; i++)
    {
        _rgArgTypes[i] = va_arg(args, int);
    }

    // Fill the rest with a known sentinel
    for (;i < MAX_ARGS; i++)
    {
        _rgArgTypes[i] = NO_TYPE;
    }

    va_end(args);
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLSymbolInfo::GLSLSymbolInfo(
    GLSLSymbols::Enum indexEnum,                // Enum of GLSL symbol text
    __in const char* pGLSLName                  // Actual GLSL symbol text
    )
{
    _indexEnum = indexEnum;
    _pGLSLName = pGLSLName;
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLFunctionInfo::GLSLFunctionInfo(
    GLSLFunctions::Enum indexEnum,              // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of function
    HLSLFunctions::Enum hlslEnum,               // Enum of equivalent HLSL function
    UINT uFlags,                                // Flags for the function (GLSLIdentifierFlags)
    int type,                                   // Return type for functions
    int numArgs,                                // The number of arguments
    ...                                         // Types of arguments
    )
{
    _indexEnum = indexEnum;
    _symbolEnum = symbolEnum;
    _hlslEnum = hlslEnum;
    _uFlags = uFlags;
    _type = type;
    _sigType = GLSLSignatureType::Normal;

    va_list args;
    va_start(args, numArgs);

    // Only support fixed max args - make _rgArgTypes bigger if you want more
    const static int MAX_ARGS = ARRAYSIZE(_rgArgTypes);
    Assert(numArgs <= MAX_ARGS);
    _numArgs = (numArgs <= MAX_ARGS) ? numArgs : MAX_ARGS;

    // Pull the argument types out of the variable arg list
    int i;
    for (i = 0; i < _numArgs; i++)
    {
        _rgArgTypes[i] = va_arg(args, int);
    }

    // Fill the rest with a known sentinel
    for (;i < MAX_ARGS; i++)
    {
        _rgArgTypes[i] = NO_TYPE;
    }

    va_end(args);
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLFunctionInfo::GLSLFunctionInfo(
    GLSLFunctions::Enum indexEnum,              // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of function
    HLSLFunctions::Enum hlslEnum,               // Enum of equivalent HLSL function
    UINT uFlags,                                // Flags for the function (GLSLIdentifierFlags)
    GLSLSignatureType::Enum sigType             // Signature type
    )
{
    _indexEnum = indexEnum;
    _symbolEnum = symbolEnum;
    _hlslEnum = hlslEnum;
    _uFlags = uFlags;
    _type = NO_TYPE;
    _numArgs = 0;
    _sigType = sigType;
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLSpecialTypeInfo::GLSLSpecialTypeInfo(
    GLSLSpecialTypes::Enum indexEnum,           // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of identifier
    __in const char* pHLSLName,                 // Name to use in HLSL
    int numFields,                              // Number of fields
    ...                                         // Types and names of fields
    ) :
    _indexEnum(indexEnum),
    _symbolEnum(symbolEnum),
    _pHLSLName(pHLSLName)
{
    va_list args;
    va_start(args, numFields);

    // Only support fixed max args - make _rgArgTypes bigger if you want more
    const static int MAX_FIELDS = ARRAYSIZE(_rgFields);
    Assert(_numFields <= MAX_FIELDS);
    _numFields = (numFields <= MAX_FIELDS) ? numFields : MAX_FIELDS;

    // Pull the argument types out of the variable arg list
    for (int i = 0; i < _numFields; i++)
    {
        _rgFields[i]._type = va_arg(args, int);
        _rgFields[i]._pszName = va_arg(args, const char*);
    }

    va_end(args);
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLSpecialVariableInfo::GLSLSpecialVariableInfo(
    GLSLSpecialVariables::Enum indexEnum,       // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of function
    __in const char* pHLSLName,                 // Name to use in HLSL
	GLSLIdentifierFlags uFlags,                 // Identifier flags
    int type,                                   // The type of the identifier
    int qualifier,                              // The qualifier for the identifier
    int arraySize                               // The size of the array for the identifier
    ) :
    _indexEnum(indexEnum),
    _symbolEnum(symbolEnum),
    _pHLSLName(pHLSLName),
    _uFlags(uFlags),
    _type(type),
    _qualifier(qualifier),
    _arraySize(arraySize),
    _fSpecialTypeValid(false),
    _specialType(GLSLSpecialTypes::count)
{
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLSpecialVariableInfo::GLSLSpecialVariableInfo(
    GLSLSpecialVariables::Enum indexEnum,       // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of function
    __in const char* pHLSLName,                 // Name to use in HLSL
    int type,                                   // The type of the identifier
    int qualifier,                              // The qualifier for the identifier
    int arraySize                               // The size of the array for the identifier
    ) :
    _indexEnum(indexEnum),
    _symbolEnum(symbolEnum),
    _pHLSLName(pHLSLName),
	_uFlags(GLSLIdentifierFlags::None),
    _type(type),
    _qualifier(qualifier),
    _arraySize(arraySize),
    _fSpecialTypeValid(false),
    _specialType(GLSLSpecialTypes::count)
{
}

//+----------------------------------------------------------------------------
//
//  Member:     Constructor
//
//-----------------------------------------------------------------------------
GLSLSpecialVariableInfo::GLSLSpecialVariableInfo(
    GLSLSpecialVariables::Enum indexEnum,       // Enum used for this info
    GLSLSymbols::Enum symbolEnum,               // Enum of GLSL symbol text of identifier
    __in const char* pHLSLName,                 // Name to use in HLSL
    GLSLSpecialTypes::Enum typeEnum,            // Special type of identifier
    int qualifier                               // The qualifier for the identifier
    ) :
    _indexEnum(indexEnum),
    _symbolEnum(symbolEnum),
    _pHLSLName(pHLSLName),
	_uFlags(GLSLIdentifierFlags::None),
    _type(NO_TYPE),
    _qualifier(qualifier),
    _arraySize(-1),
    _fSpecialTypeValid(true),
    _specialType(typeEnum)
{
}

//+----------------------------------------------------------------------------
//
//  Member:     IsInScope
//
//  Synopsis:   Check if this variable exists in the scope of this shader.
//
//-----------------------------------------------------------------------------
bool GLSLSpecialVariableInfo::IsInScope(GLSLShaderType::Enum shaderType) const
{
    if ((_uFlags & GLSLIdentifierFlags::FragmentOnly) != GLSLIdentifierFlags::None)
    {
        if (shaderType == GLSLShaderType::Fragment)
        {
            return true;
        }

        return false;
    }

	if ((_uFlags & GLSLIdentifierFlags::VertexOnly) != GLSLIdentifierFlags::None)
    {
        if (shaderType == GLSLShaderType::Vertex)
        {
            return true;
        }

        return false;
    }

    return true;
}

//+----------------------------------------------------------------------------
//
//  Member:     GetBasicType
//
//  Synopsis:   Get the basic type - fails if variable is not a basic type.
//
//-----------------------------------------------------------------------------
HRESULT GLSLSpecialVariableInfo::GetBasicType(__out int* pType) const
{
    if (_fSpecialTypeValid)
    {
        return E_FAIL;
    }
    else
    {
        (*pType) = _type;
        return S_OK;
    }
}

//+----------------------------------------------------------------------------
//
//  Member:     GetSpecialType
//
//  Synopsis:   Gets the enum of the special type - fails if it is not using
//              one of these types.
//
//-----------------------------------------------------------------------------
HRESULT GLSLSpecialVariableInfo::GetSpecialType(__out GLSLSpecialTypes::Enum* pSpecialType) const
{
    if (_fSpecialTypeValid)
    {
        (*pSpecialType) = _specialType;
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}
