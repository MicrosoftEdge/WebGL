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

#include "IdentifierInfo.hxx"
#include "StructGLSLType.hxx"

class StructGLSLType;
class TypeNameIdentifierNode;
class FunctionHeaderWithParametersNode;
class CGLSLSymbolTable;

//+-----------------------------------------------------------------------------
//
//  Class:      CTypeNameIdentifierInfo
//
//  Synopsis:   Identifier information specific to type names.
//
//              This info can be created from an type name identifier declared
//              via a struct specifier, or from an anonymous type.
//
//------------------------------------------------------------------------------
class CTypeNameIdentifierInfo : public IIdentifierInfo
{
public:
    CTypeNameIdentifierInfo();
    ~CTypeNameIdentifierInfo();

    HRESULT Initialize(
        __in TypeNameIdentifierNode* pIdentifier,                   // The parse tree node for the identifier
        __in GLSLType* pTypeDefined                                 // The type defined by this identifier
        );

    HRESULT Initialize(
        const GLSLSpecialTypeInfo &typeInfo,                        // The special type info
        __in GLSLType* pTypeDefined                                 // The type defined by this identifier
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser,                                  // Parser that we're translating for
        __in GLSLType* pTypeDefined                                 // An anonymous type
        );

    HRESULT Initialize(
        __in const CTypeNameIdentifierInfo* pOtherTypeInfo          // Existing type info to copy from
        );

    // IIdentifierInfo override
    const char* GetHLSLName(UINT uIndex) const override { return _rgHLSLName; }
    UINT GetHLSLNameCount() const override { return 1; }
    int GetSymbolIndex() const { return _iSymbolIndex; }
    HRESULT GetExpressionType(__deref_out GLSLType** ppType) const override { return E_UNEXPECTED; }   
    CTypeNameIdentifierInfo* AsTypeName() override { return this; }
    const CTypeNameIdentifierInfo* AsTypeName() const override { return this; }

    // Other methods
    HRESULT GetType(__deref_out GLSLType** ppType) const;

    const char* GetHLSLConstructorName() const;
    const char* GetHLSLEqualsFunctionName() const;

    HRESULT OutputHLSLConstructor(__in IStringStream* pOutput) const;
    HRESULT OutputHLSLEqualsFunction(__in IStringStream* pOutput) const;

private:
    CMutableString<char> _rgHLSLName;                               // The HLSL name for the type, both calculated and set, for the identifier
    CMutableString<char> _rgHLSLConstructorName;                    // The HLSL constructor name for the identifier
    CMutableString<char> _rgHLSLEqualsFunctionName;                 // The HLSL name for the type's equals function
    int _iSymbolIndex;                                              // The index of the original GLSL name in the symbol table
    TSmartPointer<StructGLSLType> _spType;                          // The type that this typename defines
};
