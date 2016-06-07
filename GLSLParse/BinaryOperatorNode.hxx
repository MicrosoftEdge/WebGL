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
#include "OperatorInfo.hxx"
#include "GLSL.tab.h"

MtExtern(BinaryOperatorNode);

//+-----------------------------------------------------------------------------
//
//  Class:      BinaryOperatorNode
//
//  Synopsis:   Node information for a binary operator expression.
//
//              This is meant to hold all boolean operators, like addition, or
//              comparison, or even +=.
//
//------------------------------------------------------------------------------
class BinaryOperatorNode : public CollectionNode
{
public:
    BinaryOperatorNode();

    DECLARE_MEMALLOC_NEW_DELETE(Mt(BinaryOperatorNode));

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT PreVerifyChildren() override;
    HRESULT VerifySelf() override;
    HRESULT Clone(__deref_out ParseTreeNode **ppClone) { return ParseTreeNode::CreateClone(GetParser(), this, ppClone); }
    HRESULT GetDumpString(__in IStringStream* pOutput) override;

    HRESULT IsConstExpression(
        bool fIncludeIndex,                                 // Whether to include loop index in the definition of a constant expression
        __out bool* pfIsConstantExpression,                 // Whether this node is a constant expression
        __out_opt ConstantValue* pValue                     // The value of the constant expression, if desired
        ) const override;

    virtual bool IsShortCircuitExpression() const;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::binaryOperator; }

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pLHS,                           // The LHS of the expression
        __in ParseTreeNode* pRHS,                           // The RHS of the expression
        const YYLTYPE &location,                            // The location of the expression
        int op                                              // The operator
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        );

    int GetOperator() const { return _pInfo->_op; }

    void SetAllowArrayAssignment() { _fAllowArrayAssignment = true; }

private:
    HRESULT WriteExpanded(
        __in IStringStream* pOutput                         // Where to write to
        );

    bool IsComponentMultiply(
        int lType,                                          // Type of left side of multiply expression
        int rType,                                          // Type of right side of multiply expression
        __out int* pReturnType                              // Calculated return type
        );

    template<typename T>
    HRESULT EvaluateExpression(
        T lConst,                                           // Left side of expression
        T rConst,                                           // Right side of expression
        __out T* pConstant                                  // Calculated value
        ) const;

    template<typename T>
    HRESULT EvaluateConstant(
        const ConstantValue& left,                          // Left side of expression
        const ConstantValue& right,                         // Right side of expression
        int basicType,                                      // The type enum of the constant
        __out ConstantValue* pValue                         // The value being calculated
        ) const;

    HRESULT VerifyOperatorForStructTypes(__in GLSLType* pLType, __in GLSLType* pRType);
    HRESULT VerifyOperatorForBasicTypes(__in GLSLType* pLType, __in GLSLType* pRType);

    HRESULT OutputHLSLForStructTypes(__in IStringStream* pOutput);
    HRESULT OutputHLSLForBasicTypes(__in IStringStream* pOutput);

    HRESULT VerifyLValue() const;

private:
    const OperatorInfo* _pInfo;                             // The info about the operator being used
    bool _fComponentMultiply;                               // Set if we have *= and are doing per-component multiply
    bool _fExpandLeft;                                      // Set if we have a scalar on the left of the expression and it needs expanding
    bool _fExpandRight;                                     // Set if we have a scalar on the right of the expression and it needs expanding
    int _expandBasicType;                                   // What type to expand to
    bool _fWrapInAll;                                       // If we should wrap the generated code in an all() function
    bool _fVerifiedOpOnStructTypes;                         // If the operator was verified for struct types.
    bool _fAllowArrayAssignment;                            // Under certain conditions when we are generating code (e.g. ternary translation), array assignment binary operators are allowed
    YYLTYPE _location;                                      // The location of the expression in the source

    const static OperatorInfo s_info[];                     // Information about operators
};
