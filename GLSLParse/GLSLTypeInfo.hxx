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

//+-----------------------------------------------------------------------------
//
//  Struct:     BasicGLSLTypeInfo
//
//  Synopsis:   Information about GLSL type.
//
//------------------------------------------------------------------------------
struct BasicGLSLTypeInfo
{
    int _type;                                      // A type specifier (like int or float) using the Bison tag
    const char* _pszGLSLName;                       // The name of the type in GLSL
    const char* _pszHLSLName;                       // The name that type should have in HLSL
    const char* _pszAltHLSLName;                    // Alternate name in HLSL (used for types like samplers that map to multiple HLSL types)
    const char* _pszDefaultInit;                    // The initializer to use for default values
    UINT _constructorComponents;                    // Expected component count for 'regular' constructor
    int _constructorComponentType;                  // Expected component type for 'regular' constructor

    static const BasicGLSLTypeInfo s_typeInfo[];    // The types that are used in the parser

    static HRESULT GetInfo(
        int type,                                   // Type to get info for
        __deref_out const BasicGLSLTypeInfo** pInfo // Info for that type
        );
};
