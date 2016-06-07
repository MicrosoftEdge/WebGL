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
#include "GLSLType.hxx"
#include "GLSLSignatureType.hxx"

class VariableIdentifierNode;
class FunctionHeaderWithParametersNode;

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLFunctionSignature
//
//  Synopsis:   Class to encapsulate a GLSL function signature. Basically a
//              collection of types with the logic to match the types.
//
//------------------------------------------------------------------------------
class CGLSLFunctionSignature
{
public:
    CGLSLFunctionSignature();

    HRESULT InitFromKnown(const GLSLFunctionInfo &info);
    HRESULT AddType(__in GLSLType *pType, int paramQual);

    GLSLType* UseReturnType() const { return _rgArgTypes[0]._spType; }
    GLSLType* UseArgumentType(UINT uIndex) const { return _rgArgTypes[uIndex + 1]._spType; }
    int GetArgumentQualifier(UINT uIndex) const { return _rgArgTypes[uIndex + 1]._paramQual; }
    UINT GetArgumentCount() const { return (_rgArgTypes.GetCount() - 1); }
    GLSLSignatureType::Enum GetSignatureType() const { return _signatureType; }

    HRESULT SignatureMatchesArgumentTypes(
        const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
        __out int* pGenType,                                            // Determined gentype
        __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
        ) const;

private:
    HRESULT MatchNormalSignature(
        const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
        __out int* pGenType,                                            // Determined gentype
        __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
        ) const;

    HRESULT MatchCompareVectorSignature(
        const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
        __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
        ) const;

    HRESULT MatchTestVectorSignature(
        const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
        __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
        ) const;

    HRESULT AddBasicType(int type, int paramQual);

    struct ParamInfo
    {
        ParamInfo();
        ParamInfo(__in GLSLType* pType, int paramQual);

        TSmartPointer<GLSLType> _spType;
        int _paramQual;
    };

private:
    CModernArray<ParamInfo> _rgArgTypes;                // The argument types (return type is first)
    GLSLSignatureType::Enum _signatureType;             // What type of signature we have
};
