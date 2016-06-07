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
#include "ConstantValue.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      CVariableIdentifierInfo
//
//  Synopsis:   Specialization of identifier information for variables.
//
//------------------------------------------------------------------------------
class CVariableIdentifierInfo : public IIdentifierInfo
{
public:
    CVariableIdentifierInfo();

    HRESULT Initialize(
        __in VariableIdentifierNode* pNode,                         // The identifier node with the symbol information
        __in GLSLType* pType,                                       // The GLSL type of the identifier
        int typeQualifier,                                          // The storage qualifier of the identifier
        int precisionQualifier,                                     // The precision qualifier of the identifier
        bool fIsParameter,                                          // Whether it is a function parameter
        const ConstantValue& initValue                              // The initial value that the identifier has
        );

    HRESULT Initialize(
        int type,                                                   // Basic type of variable 
        __in_z const char* pszName,                                 // GLSL name of variable
        __in CGLSLSymbolTable* pSymTable                            // Symbol table
        );

    HRESULT Initialize(
        const GLSLSpecialVariables::Enum eKnown,                    // Index of known thing to add
        __in CGLSLParser* pParser                                   // Parser
        );

    // IIdentifierInfo overrides
    const char* GetHLSLName(UINT uIndex) const override;
    UINT GetHLSLNameCount() const override { return _rgHLSLNames.GetCount(); }
    int GetSymbolIndex() const override { return _iSymbolIndex; }
    HRESULT GetExpressionType(__deref_out GLSLType** ppType) const override;
    CVariableIdentifierInfo* AsVariable() override { return this; }
    const CVariableIdentifierInfo* AsVariable() const override { return this; }

    // Other methods
    HRESULT SetHLSLName(
        UINT uIndex,                                                // Index of name
        __in_z LPCSTR pszName,                                      // HLSL name at that index
        __in_z LPCSTR pszSemantic                                   // HLSL semantic at that index
        );

    HRESULT NameFromIndex(UINT uIndex);
    const char* GetHLSLSemantic(UINT uIndex) const;
    UINT GetHLSLSemanticCount() const { return _rgHLSLSemantics.GetCount(); }
    void SetUsed() { _fUsed = true; }
    bool IsUsed() const { return _fUsed; }
    void SetIsLoopDeclared() { _fIsLoopDeclared = true; }
    bool IsLoopDeclared() const { return _fIsLoopDeclared; }
    bool IsParameter() const { return _fIsParameter; }
    void IncrementWriteCount() { _uWriteCount++; }
    UINT GetWriteCount() const { return _uWriteCount; }
    int GetTypeQualifier() const { return _typeQualifier; }
    int GetPrecisionQualifier() const { return _precisionQualifier; }
    const ConstantValue& GetInitialValue() const { return _initialValue; }
    GLSLQualifier::Enum GetQualifierEnum() const { return GLSLQualifier::FromParserType(_typeQualifier); }
    GLSLType *UseType() const { return _spType; }
    int GetDeclarationScope() const { return _declarationScopeId; }
    GLSLSpecialVariables::Enum GetSpecialVariable() const { return _specialVariable; }
    bool HasSemantic() const { return (_rgHLSLSemantics.GetCount() != 0); }
    bool IsInputStructVariable() const;

    static bool AreEqualTypesForUniforms(
        __in const CVariableIdentifierInfo* pVertexVariableInfo,    // Variable in vertex shader
        __in const CGLSLIdentifierTable* pVertexIdTable,            // Id table for the shader the vertex variable belongs to
        __in const CVariableIdentifierInfo* pFragmentVariableInfo,  // Variable in fragment shader
        __in const CGLSLIdentifierTable* pFragmentIdTable           // Id table for the shader the fragment variable belongs to
        );

private:
    CMutableStringModernArray _rgHLSLNames;                         // The HLSL names, both calculated and set, for the identifier
    CMutableStringModernArray _rgHLSLSemantics;                     // The HLSL semantics for the identifier
    int _iSymbolIndex;                                              // The index of the original GLSL name in the symbol table
    int _declarationScopeId;                                        // The scope ID of the identifier where it is declared
    int _typeQualifier;                                             // The storage qualifier for the identifier
    int _precisionQualifier;                                        // The precision qualifier for the identifier
    bool _fUsed;                                                    // Whether the identifier is statically used
    bool _fIsLoopDeclared;                                          // Whether the identifier is declared as a loop identifier
    bool _fIsParameter;                                             // Whether the identifier is declared as a function parameter
    UINT _uWriteCount;                                              // Count of writes to identifier
    ConstantValue _initialValue;                                    // The initial value (if any) of the identifier
    TSmartPointer<GLSLType> _spType;                                // The type of the identifier
    GLSLSpecialVariables::Enum _specialVariable;                    // The GLSL variable that maps to this identifier
};
