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
#include "FunctionIdentifierInfo.hxx"

class FunctionIdentifierNode;
class ParameterDeclarationNode;

//+-----------------------------------------------------------------------------
//
//  Class:      FunctionHeaderWithParametersNode
//
//  Synopsis:   Node information for a function header that has parameters in 
//              GLSL. This node is used for function declarations and
//              definitions, but not function calls.
//
//------------------------------------------------------------------------------
class FunctionHeaderWithParametersNode : public CollectionNode
{
public:
    FunctionHeaderWithParametersNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                  // The parser that owns the tree
        __in ParseTreeNode* pHeader,                // The header
        __in_opt ParseTreeNode* pParam              // The first parameter
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                   // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;
    HRESULT GetDumpString(__in IStringStream* pOutput) override;
    HRESULT VerifySelf() override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::functionHeaderWithParameters; }

    // Other methods
    bool IsEntryPoint() const { return _fEntryPoint; }
    CFunctionIdentifierInfo* UseIdentifierInfo() { return _spInfo; }

    HRESULT GetParameterTypes(__out CModernArray<TSmartPointer<GLSLType>>& aryParameterTypes) const;

    bool ParameterQualifiersMatchSignature(const CGLSLFunctionSignature& signature) const;

    UINT GetParameterCount() const { return GetChildCount() - 1; }

    ParameterDeclarationNode* GetParameterNode(UINT i) const;

private:
    FunctionIdentifierNode* GetFunctionIdentifier();

private:
    TSmartPointer<CFunctionIdentifierInfo> _spInfo; // The identifier information for the function
    bool _fEntryPoint;                              // True if this function is an entry point function
};
