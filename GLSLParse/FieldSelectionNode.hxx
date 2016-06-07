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
#include "GLSL.tab.h"

class VariableIdentifierNode;
class StructGLSLType;

namespace ComponentNames
{
    enum Enum
    {
        Xyzw,
        Rgba,
        Stpq
    };
};

//+-----------------------------------------------------------------------------
//
//  Class:      FieldSelectionNode
//
//  Synopsis:   Node information for a postfix expression that does a field
//              selection. This could be on a struct or on a vector.
//
//------------------------------------------------------------------------------
class FieldSelectionNode : public CollectionNode
{
public:
    FieldSelectionNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        __in ParseTreeNode* pExpr,                  // The expression that the field is on
        __in ParseTreeNode* pSelection,             // The field selection
        const YYLTYPE &location                     // The location of the selection
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;
    bool IsLValue() const override;

    HRESULT IsConstExpression(
        bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
        __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
        __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
        ) const override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::fieldSelection; }

private:
    HRESULT VerifySelectionOnBasicType(int exprBasicType);
    HRESULT VerifySelectionOnStructType(__in const StructGLSLType* pExprType);

    bool IsSwizzleSelection() const { return _numSwizzleComponents > 0; }

    static HRESULT InterpretComponent(
        char c,                                     // Component to interpret
        __out ComponentNames::Enum* peSet,          // What set it is in
        __out_range(0, 3) int* pIndex               // The index in that set
        );

    VariableIdentifierNode* GetVariableIdentifierNodeChild() const;

private:
    int _numSwizzleComponents;                      // The number of components calculated for the swizzle (0 if no swizzle)
    int _rgIndices[4];                              // The indices of the components
    bool _fRepeated;                                // If the swizzle is repeated
    YYLTYPE _location;                              // The location of the selection
};

