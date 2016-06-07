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
#include "KnownSymbols.hxx"
#include "GLSLFunctionSignature.hxx"
#include "GLSLType.hxx"
#include "GLSLQualifier.hxx"
#include "IdentifierInfo.hxx"
#include "FunctionDefinitionNode.hxx"

class FunctionIdentifierNode;
class FunctionHeaderWithParametersNode;
class CGLSLSymbolTable;

//+-----------------------------------------------------------------------------
//
//  Class:      CFunctionIdentifierInfo
//
//  Synopsis:   Identifier information specific to functions.
//
//              This info can be created from an identifier declared
//              in the code, or from a known library of builtin identifiers
//              which correspond to builtin functions.
//
//------------------------------------------------------------------------------
class CFunctionIdentifierInfo : public IIdentifierInfo
{
public:
    CFunctionIdentifierInfo();

    HRESULT Initialize(
        __in FunctionIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
        __in FunctionHeaderWithParametersNode* pHeaderWithParam     // The function header with param that owns the identifier
        );

    HRESULT Initialize(
        const GLSLFunctionInfo &info,                               // Function info of function to add
        __in CGLSLSymbolTable* pSymTable                            // Symbol table
        );

    // IIdentifierInfo override
    const char* GetHLSLName(UINT uIndex) const override { return _rgHLSLName; }
    UINT GetHLSLNameCount() const override { return 1; }
    int GetSymbolIndex() const { return _iSymbolIndex; }
    HRESULT GetExpressionType(__deref_out GLSLType** ppType) const override { return E_UNEXPECTED; }   
    
    CFunctionIdentifierInfo* AsFunction() override { return this; }
    const CFunctionIdentifierInfo* AsFunction() const override { return this; }

    // Other methods
    void SetDefined(__in_opt FunctionDefinitionNode* pFuncDefinition);
    bool IsDefined() const { return _fDefined; }
    const FunctionDefinitionNode* UseFunctionDefinition() const { return _spFunctionDefinition; }
    void SetCalled() { _fCalled = true; }
    bool IsCalled() const { return _fCalled; }
    void SetDeclared() { _fDeclared = true; }
    bool IsDeclared() const { return _fDeclared; }
    const CGLSLFunctionSignature &GetSignature() const { return _signature; }
    HLSLFunctions::Enum GetHLSLFunction() const { return _hlslFunction; }

    bool IsKnownFunction() const { return _hlslFunction != HLSLFunctions::count; }

private:
    TSmartPointer<FunctionDefinitionNode> _spFunctionDefinition;    // The node (if any) that defines this function. Will always be null for known functions, except for main.
    CMutableString<char> _rgHLSLName;                               // The HLSL names, both calculated and set, for the identifier
    int _iSymbolIndex;                                              // The index of the original GLSL name in the symbol table
    CGLSLFunctionSignature _signature;                              // The signature
    HLSLFunctions::Enum _hlslFunction;                              // The HLSL function that maps to this identifier
    bool _fDeclared;                                                // Whether the function has a forward declaration
    bool _fDefined;                                                 // Whether the function has a definition
    bool _fCalled;                                                  // Whether the function is called anywhere in the shader
};
