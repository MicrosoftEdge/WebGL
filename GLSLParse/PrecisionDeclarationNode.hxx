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

#include "CollectionNode.hxx"
#include "glsl.tab.h"
#include "GLSLPrecisionType.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      PrecisionDeclarationNode
//
//  Synopsis:   Node information for a multiplicative expression.
//
//------------------------------------------------------------------------------
class PrecisionDeclarationNode : public CollectionNode
{
public:
    PrecisionDeclarationNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        int precisionQual,                          // Precision qualifier in the declaration
        __in ParseTreeNode* pType,                  // Type specifier in the declaration
        const YYLTYPE &location                     // The location of the declaration
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    HRESULT VerifySelf() override;
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;

    static HRESULT GetPrecisionTypeFromGLSLType(int basicType, __out GLSLPrecisionType& glslPrecisionType);

private:
    ParseTreeNode* GetTypeChild() const { return GetChild(0); }

private:
    int _precisionQualifier;                        // The precision being set
    YYLTYPE _location;                              // Location of the declaration
};
