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

#include "VariableIdentifierInfo.hxx"
#include <foundation/collections.hxx>
#include "IOStructType.hxx"
#include "GLSLSymbolTable.hxx"

namespace FeatureUsedFlags
{
    enum Enum : UINT;
}

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLIOStructInfo
//
//  Synopsis:   This represents the information for an IO struct, which is used
//              in HLSL to represent the data being input and output to the
//              shaders.
//
//------------------------------------------------------------------------------
class CGLSLIOStructInfo : public IUnknown
{
public:
    HRESULT AddEntry(
        __in CVariableIdentifierInfo* pInfo,                    // Identifier info for the entry
        __in CMemoryStream* pHLSLStream,                        // HLSL stream for the normal entry
        __in CMemoryStream* pUnusedStream                       // HLSL stream for the entry when unused
        );

    HRESULT OutputHLSL(
        __in_opt const CGLSLIOStructInfo *pVaryingInfo,         // Other shader's varying info
        __inout IStringStream* pOutput                          // Where to put the HLSL code
        ) const;

    bool IsFeatureUsed(FeatureUsedFlags::Enum feature) const;

    UINT GetVaryingVectorCount() const;

    static HRESULT ComputeLinkedVertexStructInfo(
        __in const CGLSLIOStructInfo* pVertexInfo,              // Vertex info to compute from
        __in const CGLSLIOStructInfo* pFragmentInfo,            // Fragment info to compute from
        __deref_out CGLSLIOStructInfo** ppLinkedVertexInfo      // Computed vertex info based on entries from vertex and fragment infos
        );
    
protected:
    CGLSLIOStructInfo();

    HRESULT Initialize(
        IOStructType::Enum structType,                          // Struct type info is for
        UINT uFeatureUsedFlags,                                 // Flags for features used
        __in CGLSLSymbolTable* pSymbolTable,                    // Symbol table
        GLSLShaderType::Enum shaderType                         // Our shader type
        );

private:
    UINT GetInfo(__in_z LPCSTR pszGLSLName) const;

private:
    //+-----------------------------------------------------------------------------
    //
    //  Class:      StructInfoEntry
    //
    //  Synopsis:   This represents a single entry in the struct. Derives from
    //              IUnknown so it can be refcounted and therefore moved around
    //              without copying strings.
    //
    //------------------------------------------------------------------------------
    class StructInfoEntry : public IUnknown
    {
    public:
        CMutableString<char> _hlslText;                         // HLSL for this entry
        CMutableString<char> _unusedText;                       // Format string for this entry when unused
        TSmartPointer<CVariableIdentifierInfo> _spInfo;         // Info for the variable for this entry
        
    protected:
        HRESULT Initialize() { return S_OK; }
    };

private:
    TSmartPointer<CGLSLSymbolTable> _spSymbolTable;             // The symbol table
    CModernArray<TSmartPointer<StructInfoEntry>> _aryEntries;   // Collection of entries
    IOStructType::Enum _structType;                             // What type of IO the struct represents
    UINT _uFeatureUsedFlags;                                    // Flags for features being used
    GLSLShaderType::Enum _shaderType;                           // Our shader type

    static UINT s_uNotFound;                                    // For GetInfo to return
};
