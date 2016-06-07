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

//+-----------------------------------------------------------------------------
//
//  Enum:       ExpandType
//
//  Synopsis:   Enumeration to describe expansion support of an operator.
//
//              Some operators in GLSL support taking a scalar on one side and
//              a nonscalar on the other. The operation is done as if the scalar
//              was expanded to match the other side.
//
//              HLSL does support this in a limited fashion (with multiply
//              and with divide for the RHS). We try to use this where we can
//              to make the output more compact and efficient.
//
//              This enum is used to describe this support in both languages -
//              the GLSL support is used to verify correct usage, and the HLSL
//              support is used when translating.
//
//------------------------------------------------------------------------------
namespace ExpandType
{
    enum Enum
    {
        None,       // Expansion not supported (e.g. '=' in GLSL)
        Right,      // Expansion supported only on the right (e.g. '+=' in GLSL)
        Both        // Expansion supported on both sides (e.g. "+" in GLSL)
    };
};

//+-----------------------------------------------------------------------------
//
//  Struct:     OperatorInfo
//
//  Synopsis:   Helper class for mapping operators to HLSL strings.
//
//              This is used to build tables that represent how to verify
//              operators are being used correctly.
//
//              The Bison file defines some special tokens that share a
//              namespace with the type tokens.
//
//              Tokens for describing input types:
//
//              NUMERIC_SCALAR_TYPE - represents float and int
//              NUMERIC_TYPE - represents all scalar + vector + matrix types
//              SIMPLE_TYPE - represents NUMERIC_TYPE in addition to boolean
//
//              Tokens for describing return types:
//
//              LEFT_EXPR_TYPE - represents return value being equal to LHS
//              NO_TYPE - represents return value for reserved operators
//
//------------------------------------------------------------------------------
struct OperatorInfo
{
    int _op;                                                // An operator (uses the parser token)
    const char* _pszOpString;                               // String to use when writing out the operator
    bool _fHLSLBoolVector;                                  // Whether the HLSL operator returns a boolean vector
    bool _fWriteLeft;                                       // Whether the operator writes to the left operand
    int _inputType;                                         // Valid type for input, can be NUMERIC_TYPE or SIMPLE_TYPE
    ExpandType::Enum _glslExpand;                           // Expansion supported by operator in GLSL
    ExpandType::Enum _hlslExpand;                           // Expansion supported by operator in HLSL
    int _returnType;                                        // Return type, can be LEFT_EXPR_TYPE

    bool AreCompatibleTypes(
        int lType,                                          // The basic type of left argument
        int rType                                           // The basic type of right argument
        ) const;

    static HRESULT GetInfo(
        __in_ecount(opCount) const OperatorInfo* pOpList,   // Operator list
        int opCount,                                        // Size of the operator list
        int op,                                             // Operator to lookup
        const OperatorInfo** ppInfo                         // The operator info
        );
};
