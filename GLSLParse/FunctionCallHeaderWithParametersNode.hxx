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
#include "GLSLFunctionSignature.hxx"
#include "GLSL.tab.h"

class FunctionIdentifierNode;
class FunctionCallIdentifierNode;
struct BasicGLSLTypeInfo;
class TypeNameIdentifierNode;
class CFunctionIdentifierInfo;

namespace FunctionCallType
{
    enum Enum
    {
        normal,
        constructor,
        vectorConstructorFromScalar,
        vectorConstructorFromMatrix,
        matrixConstructorFromComponents,
        matrixConstructorFromScalar,
        matrixConstructorFromMatrix,
    };
}

//+-----------------------------------------------------------------------------
//
//  Class:      FunctionCallHeaderWithParametersNode
//
//  Synopsis:   Node information for a function header that has parameters in 
//              GLSL. This node is used only for function calls.
//
//------------------------------------------------------------------------------
class FunctionCallHeaderWithParametersNode : public CollectionNode
{
public:
    FunctionCallHeaderWithParametersNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        __in ParseTreeNode* pHeader,                        // The header
        __in_opt ParseTreeNode* pArg,                       // The first argument / expression
        const YYLTYPE &location                             // The location of the function call
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;

    HRESULT IsConstExpression(
        bool fIncludeIndex,                                 // Whether to include loop index in the definition of a constant expression
        __out bool* pfIsConstantExpression,                 // Whether this node is a constant expression
        __out_opt ConstantValue* pValue                     // The value of the constant expression, if desired
        ) const override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::functionCallHeaderWithParameters; }

private:
    FunctionCallIdentifierNode* GetFunctionCallIdentifier() const;
    FunctionIdentifierNode* GetFunctionIdentifier() const;

    HRESULT VerifyNonConstructor(__in FunctionIdentifierNode* pFuncId);
    HRESULT VerifyStructConstructor(__in TypeNameIdentifierNode* pTypeNameId);
    HRESULT VerifyMatrixConstructor();
    HRESULT VerifyVectorConstructor();

    HRESULT VerifyTensorComponentConstructor(bool fAllowMatrixArgs);

    UINT GetArgumentCount() const { return GetChildCount() - 1; }

    ParseTreeNode* GetArgumentNode(UINT i) const { return GetChild(i + 1); }

    HRESULT ValidateArgumentsWithParameterQualifiers(
        __in const CGLSLFunctionSignature& signature        // Signature to grab qualifiers from
        ) const;

    HRESULT GetArgumentTypes(__out CModernArray<TSmartPointer<GLSLType>>& aryTypes) const;

    HRESULT OutputFunctionCallArgument(
        __in IStringStream* pOutput,                        // Stream to write output to
        UINT iChild,                                        // Child to output
        HLSLFunctions::Enum hlslFunction                    // Known hlsl function call that the arg is participating in
        );

    HRESULT GetComponentCountForConstructorArgType(
        int basicArgType,                                   // Type of the argument
        bool fForMatrixConstructor,                         // Whether this is counting for a matrix ctor or not
        __out UINT* puComponents                            // Number of components in the type
        );

private:
    YYLTYPE _location;                                      // The location of the function call
    FunctionCallType::Enum _functionCallType;               // Type of function call
    int _genType;                                           // The type that was matched for signatures that do matching
    bool _fHasSignature;                                    // Whether we have a signature
    UINT _expectedConstructorArgCount;                      // Repeat count
    UINT _lastArgTruncateCount;                             // The number of components to truncate the last argument to (for constructors)
    int _basicConstructType;                                // Type being constructed, for basic type constructors
};
