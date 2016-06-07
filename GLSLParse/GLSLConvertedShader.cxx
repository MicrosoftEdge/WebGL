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
#include "PreComp.hxx"
#include "GLSLConvertedShader.hxx"
#include "RefCounted.hxx"
#include "WebGLError.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::Initialize()
{
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetConvertedCodeWithParsedStructInfo
//
//  Synopsis:   Appends the parsed struct info as a string. Used only for
//              local tests - the normal workflow is to link a converted
//              shader and combine the struct info and code strings together
//              externally.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::GetConvertedCodeWithParsedStructInfo(
    __out CMutableString<char>& spszCode                        // Output ASCII HLSL string
    )
{
    CHK_START;

    CHKB(TranslationSucceeded());

    spszCode.SetInitialSize(512);

    // ExtractString will append to existing contents of spszCode, hence __inout.
    if (_spVaryingStructInfo != nullptr)
    {
        TSmartPointer<CMemoryStream> spVaryingStream;
        CHK(RefCounted<CMemoryStream>::Create(/*out*/spVaryingStream));
        CHK(_spVaryingStructInfo->OutputHLSL(/*pOtherInfo*/nullptr, spVaryingStream));
        CHK(spVaryingStream->ExtractString(spszCode));
    }

    CHK(AppendConvertedCode(spszCode));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AppendConvertedCode
//
//  Synopsis:   Appends the converted code as a string.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::AppendConvertedCode(
    __inout CMutableString<char>& spszCode                      // Output ASCII HLSL string
    )
{
    CHK_START;

    CHKB(TranslationSucceeded());

    // ExtractString will append to existing contents of spszCode, hence __inout.
    CHK(_spStreamConverted->ExtractString(spszCode));

    // There are valid translations that result in no output. To make callers happy
    // have spszCode be the empty string in this situation.
    if (static_cast<PCSTR>(spszCode) == nullptr)
    {
        CHK(spszCode.Append("", 0));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetLog
//
//  Synopsis:   Get the converted code as a string.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::GetLog(__deref_out BSTR* pbstrLog)
{
    CHK_START;

    // Lump all of the errors together into a stream
    TSmartPointer<CMemoryStream> spLogStream;
    UINT cErrors = _rgErrors.GetCount();
    if (cErrors > 0)
    {
        CHK(RefCounted<CMemoryStream>::Create(/*out*/spLogStream));

        CHK(spLogStream->WriteString("Shader compilation errors\n"));

        for (UINT i = 0; i < cErrors; i++)
        {
            CHK(_rgErrors[i]->WriteLog(spLogStream));
        }
    }

    CMutableString<char> spConvertedLog;
    CSmartBstr bstrLog;
    if (spLogStream != nullptr)
    {
        CHK(spLogStream->ExtractString(spConvertedLog));
        bstrLog.SetAnsi(spConvertedLog);
    }
    else
    {
        bstrLog.Set(L"");
    }

    (*pbstrLog) = bstrLog.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetMemorySize
//
//  Synopsis:   Returns approximate dynamic memory occupied by the converted 
//              shader so that the F12 memory profiler can report it as memory
//              cost of a WebGLShader
//-----------------------------------------------------------------------------
UINT CGLSLConvertedShader::GetMemorySize() const
{
    UINT cbMemorySize = 0;
    if (_spStreamConverted != nullptr)
    {        
        if (_spStreamConverted->GetSize(&cbMemorySize) != S_OK)
        {
            cbMemorySize = 0;
        }
    }
    return cbMemorySize;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddError
//
//  Synopsis:   Add an error to the sink.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::AddError(
    int line,                       // The location that the error occured
    int column,
    HRESULT hrCode,                 // The error code for the error
    __in_z_opt const char* pszData  // The optional data to use making a message for the error
    )
{
    CHK_START;

    TSmartPointer<CGLSLError> spNewError;
    CHK(RefCounted<CGLSLError>::Create(line, column, hrCode, pszData, /*out*/spNewError));
    CHK(_rgErrors.Add(spNewError));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetConverterOutput
//
//  Synopsis:   Set the HLSL output of the converted shader
//
//-----------------------------------------------------------------------------
void CGLSLConvertedShader::SetConverterOutput(
    __in CMemoryStream* pOutput,                                // The converted shader HLSL code
    __in_opt CGLSLIOStructInfo* pVaryingInfo                    // Info to make varying struct
    )
{ 
    _spStreamConverted = pOutput;
    _spVaryingStructInfo = pVaryingInfo;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslationSucceeded
//
//  Synopsis:   Query if the translation succeeded.
//
//-----------------------------------------------------------------------------
bool CGLSLConvertedShader::TranslationSucceeded() const
{
    if (_spStreamConverted != nullptr)
    {
        Assert(GetErrorCount() == 0);

        return true;
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   LinkVaryingStructEntries
//
//  Synopsis:   Takes converted vertex and fragment shaders determines whether
//              they can successfully link, and returns the varying struct
//              prologue in string form for each shader. For specific
//              types of linking errors, fills out errorRecord and returns
//              S_OK to facilitate detailed logging by the caller.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLConvertedShader::LinkVaryingStructEntries(
    __in const CGLSLConvertedShader* pConvertedVertex,          // Vertex shader to link varyings
    __in const CGLSLConvertedShader* pConvertedFragment,        // Fragment shader to link varyings
    UINT uMaxVaryingVectorCount,                                // Max varying vectors (changes with feature level)
    __out LinkingErrorRecord& errorRecord,                      // Place to record information about encountered errors
    __out CMutableString<char>& spszVertexHLSLPrologue,         // Computed Vertex HLSL prologue containing converted varying struct
    __out CMutableString<char>& spszFragmentHLSLPrologue        // Computed Fragment HLSL prologue containing converted varying struct
    )
{
    CHK_START;
    Assert(pConvertedVertex->TranslationSucceeded());
    Assert(pConvertedFragment->TranslationSucceeded());

    CGLSLIdentifierTable* pVertexIdentifierTable = pConvertedVertex->UseIdentifierTable();
    CGLSLIdentifierTable* pFragmentIdentifierTable = pConvertedFragment->UseIdentifierTable();

    CHKB(pVertexIdentifierTable != nullptr);
    CHKB(pFragmentIdentifierTable != nullptr);

    TSmartPointer<CVariableIdentifierInfo> spFragVaryingVarInfo;
    PCSTR pszGLSLName;
    for (UINT i = 0; SUCCEEDED(pFragmentIdentifierTable->GetVariableInfoAndNameByIndex(i, &spFragVaryingVarInfo, &pszGLSLName)); i++)
    {
        if (spFragVaryingVarInfo->GetQualifierEnum() == GLSLQualifier::Varying && spFragVaryingVarInfo->IsUsed())
        {
            // We found a varying that is statically used in the fragment shader. Verify that it exists in the vertex shader now.
            TSmartPointer<CVariableIdentifierInfo> spVertexVaryingVarInfo;
            if (SUCCEEDED(pVertexIdentifierTable->GetVariableInfoFromString(pszGLSLName, GLSLQualifier::Varying, &spVertexVaryingVarInfo)))
            {
                // We know the name and qualifier matches. The type should be the same too.
                if (!spFragVaryingVarInfo->UseType()->IsEqualType(spVertexVaryingVarInfo->UseType()))
                {
                    errorRecord.errorType = LinkingErrorRecord::ErrorType::TypeMismatch;
                    errorRecord.pszName = pszGLSLName;
                    break;
                }
            }
            else
            {
                errorRecord.errorType = LinkingErrorRecord::ErrorType::NotVertexDeclared;
                errorRecord.pszName = pszGLSLName;
                break;
            }
        }
    }

    if (errorRecord.errorType == LinkingErrorRecord::ErrorType::NoError)
    {
        // At this point, we have verified that every varying that is used in the fragment shader exists in
        // the vertex shader with the same type. However there could be vertex varyings that are not used in
        // either shader. These can be removed, which could end up letting us get under the maximum number of varyings.
        TSmartPointer<CGLSLIOStructInfo> spLinkedVertexInfo;
        CHK(CGLSLIOStructInfo::ComputeLinkedVertexStructInfo(
            pConvertedVertex->UseParsedVaryingInfo(),
            pConvertedFragment->UseParsedVaryingInfo(),
            &spLinkedVertexInfo
            ));

        // Now that we've settled on a final set of varyings we'll output to the PSInput struct, 
        // double-check that we are under the max.
        UINT uVaryingVectorCount = spLinkedVertexInfo->GetVaryingVectorCount();
        CHKB_HR(uVaryingVectorCount <= uMaxVaryingVectorCount, E_WEBGL_LINKED_VARYING_COUNT_EXCEEDED);

        // Finally, get the string representations of the linked HLSL varying structs and return them.
        TSmartPointer<CMemoryStream> spVaryingStream;
        CHK(RefCounted<CMemoryStream>::Create(/*out*/spVaryingStream));

        CHK(spLinkedVertexInfo->OutputHLSL(pConvertedFragment->UseParsedVaryingInfo(), spVaryingStream));
        CHK(spVaryingStream->ExtractString(spszVertexHLSLPrologue));

        // Re-use the stream for the fragment varying struct prologue, but begin writing at the beginning again.
        CHK(spVaryingStream->SeekToStart());
        CHK(pConvertedFragment->UseParsedVaryingInfo()->OutputHLSL(spLinkedVertexInfo, spVaryingStream));
        CHK(spVaryingStream->ExtractString(spszFragmentHLSLPrologue));
    }

    CHK_RETURN;
}
