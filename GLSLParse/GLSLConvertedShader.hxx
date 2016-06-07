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

#include "MemoryStream.hxx"
#include "GLSLIdentifierTable.hxx"
#include "GLSLError.hxx"
#include "IErrorSink.hxx"
#include "GLSLIOStructInfo.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLConvertedShader
//
//  Synopsis:   This class encapsulates the shader after conversion. After
//              conversion this contains the generated HLSL, the identifier
//              table and any error information.
//
//------------------------------------------------------------------------------
class CGLSLConvertedShader : public IErrorSink
{
public:
    // IErrorSink implementation
    HRESULT AddError(
        int line,                       // The location that the error occured
        int column,
        HRESULT hrCode,                 // The error code for the error
        __in_z_opt const char* pszData  // The optional data to use making a message for the error
        ) override;

    UINT GetErrorCount() const override { return _rgErrors.GetCount(); }
    CGLSLError* UseError(UINT index) const override { return _rgErrors[index]; }

    void SetIdentifierTable(__in CGLSLIdentifierTable* pTable) { _spIdTable = pTable; }
    
    void SetConverterOutput(
        __in CMemoryStream* pOutput,                                // The converted shader HLSL code
        __in_opt CGLSLIOStructInfo* pVaryingInfo                    // Info to make varying struct
        );

    bool TranslationSucceeded() const;

    HRESULT GetConvertedCodeWithParsedStructInfo(
        __out CMutableString<char>& spszCode                        // Output ASCII HLSL string
        );

    HRESULT AppendConvertedCode(
        __inout CMutableString<char>& spszCode                      // Output ASCII HLSL string
        );

    HRESULT GetLog(__deref_out BSTR* pbstrLog);
    CGLSLIdentifierTable* UseIdentifierTable() const { return _spIdTable; }

    UINT GetMemorySize() const;

    struct LinkingErrorRecord
    {
        enum ErrorType
        {
            NoError,
            TypeMismatch,
            NotVertexDeclared,
        };

        LinkingErrorRecord() : errorType(ErrorType::NoError), pszName(nullptr) {}

        const char* pszName;
        ErrorType errorType;
    };

    static HRESULT LinkVaryingStructEntries(
        __in const CGLSLConvertedShader* pConvertedVertex,          // Vertex shader to link varyings
        __in const CGLSLConvertedShader* pConvertedFragment,        // Fragment shader to link varyings
        UINT uMaxVaryingVectorCount,                                // Max varying vectors (changes with feature level)
        __out LinkingErrorRecord& errorRecord,                      // Place to record information about encountered errors
        __out CMutableString<char>& spszVertexHLSLPrologue,         // Computed Vertex HLSL prologue containing converted varying struct
        __out CMutableString<char>& spszFragmentHLSLPrologue        // Computed Fragment HLSL prologue containing converted varying struct
        );

protected:
    HRESULT Initialize();

private:
    const CGLSLIOStructInfo* UseParsedVaryingInfo() const { return _spVaryingStructInfo; }

private:
    CModernArray<TSmartPointer<CGLSLError>> _rgErrors;              // Errors found in parsing
    TSmartPointer<CMemoryStream> _spStreamConverted;                // The converted shader HLSL code
    TSmartPointer<CGLSLIOStructInfo> _spVaryingStructInfo;          // HLSL code for the VS output / PS input, as originally parsed
    TSmartPointer<CGLSLIdentifierTable> _spIdTable;                 // The identifier table
};
