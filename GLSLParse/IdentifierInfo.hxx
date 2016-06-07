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

class VariableIdentifierNode;
class FunctionIdentifierNode;
class FunctionHeaderWithParametersNode;
class CGLSLSymbolTable;
class CVariableIdentifierInfo;
class CFunctionIdentifierInfo;
class CTypeNameIdentifierInfo;

//+-----------------------------------------------------------------------------
//
//  Interface:  IIdentifierInfo
//
//  Synopsis:   Interface to implement for identifier information. Each
//              different identifier that the parser finds has this information
//              created once and it is stored in the identifier table.
//
//              Derived classes add more specialization for different kinds
//              of identifiers. 
//
//------------------------------------------------------------------------------
interface IIdentifierInfo : public IUnknown
{
public:
    virtual const char* GetHLSLName(UINT uIndex) const = 0;
    virtual UINT GetHLSLNameCount() const = 0;
    virtual int GetSymbolIndex() const = 0;
    virtual HRESULT GetExpressionType(__deref_out GLSLType** ppType) const = 0;

    virtual CVariableIdentifierInfo* AsVariable() { return nullptr; }
    virtual CFunctionIdentifierInfo* AsFunction() { return nullptr; }
    virtual CTypeNameIdentifierInfo* AsTypeName() { return nullptr; }
    
    virtual const CVariableIdentifierInfo* AsVariable() const { return nullptr; }
    virtual const CFunctionIdentifierInfo* AsFunction() const { return nullptr; }
    virtual const CTypeNameIdentifierInfo* AsTypeName() const { return nullptr; }

    bool IsGLSLSymbol(GLSLSymbols::Enum symbol) const { return (GetSymbolIndex() == static_cast<int>(symbol)); }

protected:
    const static UINT s_uHLSLMAXLENGTH = 64;                        // Maximum length for HLSL string
};
