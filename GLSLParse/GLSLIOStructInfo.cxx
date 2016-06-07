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
#include "GLSLIOStructInfo.hxx"
#include "MemoryStream.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"

UINT CGLSLIOStructInfo::s_uNotFound = static_cast<UINT>(-1);

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLIOStructInfo::CGLSLIOStructInfo() :
    _structType(IOStructType::Undefined)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIOStructInfo::Initialize(
    IOStructType::Enum structType,                              // Struct type info is for
    UINT uFeatureUsedFlags,                                     // Flags for features used
    __in CGLSLSymbolTable* pSymbolTable,                        // Symbol table
    GLSLShaderType::Enum shaderType                             // Our shader type
    )
{
    _structType = structType;
    _uFeatureUsedFlags = uFeatureUsedFlags;
    _spSymbolTable = pSymbolTable;
    _shaderType = shaderType;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsFeatureUsed
//
//-----------------------------------------------------------------------------
bool CGLSLIOStructInfo::IsFeatureUsed(FeatureUsedFlags::Enum feature) const
{
    return CGLSLParser::IsFeatureUsed(_uFeatureUsedFlags, feature);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this IO struct.
//
//              If the vertex info is passed in, then it is assumed that we
//              are doing the varying for a fragment shader.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIOStructInfo::OutputHLSL(
    __in_opt const CGLSLIOStructInfo *pVaryingInfo, // Other shader's varying info
    __inout IStringStream* pOutput                  // Where to put the HLSL code
    ) const
{
    CHK_START;

    Assert(_aryEntries.GetCount() == _aryEntries.GetCount());

    // We cannot calculate logic for writing out emulation variables unless
    // we have varying info. We should only have this set to nullptr when
    // we are in testing mode, and that implies we won't check these
    // variables as well.
    const bool fWriteEmulationVariables = (pVaryingInfo != nullptr);

    const char* pszName = nullptr;

    switch(_structType)
    {
    case IOStructType::Attribute:
        pszName = "VSInput";
        break;

    case IOStructType::Varying:
        pszName = "PSInput";
        break;

    case IOStructType::Output:
        pszName = "PSOutput";
        break;

    default:
        AssertSz(false, "Unexpected struct type in CGLSLIOStructInfo::OutputHLSL");
        CHK(E_UNEXPECTED);
    }

    // Write the header of the input struct
    CHK(pOutput->WriteFormat(128, "struct %s\n{\n", pszName));
    pOutput->IncreaseIndent();

    bool fPointCoordIsInVarying = false;    
    const CGLSLIOStructInfo* pVertexInfo = ((_shaderType == GLSLShaderType::Vertex) ? this : pVaryingInfo);
    const CGLSLIOStructInfo* pFragmentInfo =  ((_shaderType == GLSLShaderType::Fragment) ? this : pVaryingInfo);

    switch (_structType)
    {
    case IOStructType::Output:
        CHK(pOutput->WriteIndent());
        CHK(pOutput->WriteString("float4 fragColor[1] : SV_Target;\n"));

        if (pFragmentInfo->IsFeatureUsed(FeatureUsedFlags::glFragDepth))
        {
            CHK(pOutput->WriteIndent());
            CHK(pOutput->WriteString("float fragDepth : SV_Depth;\n"));
        }
        break;

    case IOStructType::Varying:
        if (fWriteEmulationVariables)
        {
            if (pFragmentInfo->IsFeatureUsed(FeatureUsedFlags::glFragCoord))
            {                
                CHK(pOutput->WriteIndent());
                CHK(pOutput->WriteString("float4 fragCoord : FragCoord;\n"));
            }
            
            if (pVertexInfo->IsFeatureUsed(FeatureUsedFlags::glPointSize) && pFragmentInfo->IsFeatureUsed(FeatureUsedFlags::glPointCoord))
            {                
                // According the spec, we can only burn a varying of gl_PointCoord if present in the
                // fragment shader. Similarly, we will not emit the varying if the developer does
                // not gl_PointSize in the vertex shader. 
                CHK(pOutput->WriteIndent());
                CHK(pOutput->WriteString("float2 pointCoord: PointCoord;\n"));
                fPointCoordIsInVarying = true;
            }
        }
        break;

    case IOStructType::Attribute:
        if (IsFeatureUsed(FeatureUsedFlags::glPointSize))
        {
            // If we're using gl_PointSize, add instance information to the input struct.  
            // The context will query for the presence of this and it in as appropriate.  
            CHK(pOutput->WriteIndent());
            CHK(pOutput->WriteString("float4 instanceInfo : InstanceInfo;\n"));                 
        }
        break;

    default:
        AssertSz(false, "Unexpected struct type in CGLSLIOStructInfo::OutputHLSL");
        CHK(E_UNEXPECTED);
    }

    if (_structType == IOStructType::Varying && pVertexInfo != nullptr && _shaderType == GLSLShaderType::Fragment)
    {
        // Keep track of how many fragment shader entries
        // are found - all of them need to be accounted for
        UINT uFoundCount = 0;
        UINT uUnusedCount = 0;
        for (UINT i = 0; i < pVertexInfo->_aryEntries.GetCount(); i++)
        {
            CHK(pOutput->WriteIndent());

            // Find the GLSL name that the vertex shader uses for this entry
            LPCSTR pszGLSLName = pVertexInfo->_spSymbolTable->NameFromIndex(pVertexInfo->_aryEntries[i]->_spInfo->GetSymbolIndex());

            // Get the corresponding info in this struct, if any
            UINT uFoundIndex = GetInfo(pszGLSLName);
            if (uFoundIndex == s_uNotFound)
            {
                CHK(pOutput->WriteFormat(256, pVertexInfo->_aryEntries[i]->_unusedText, uUnusedCount, uUnusedCount));
                uUnusedCount++;
            }
            else
            {
                CHK(pOutput->WriteString(_aryEntries[uFoundIndex]->_hlslText));
                uFoundCount++;
            }
        }

        // All of the entries must be accounted for
        CHKB(uFoundCount == _aryEntries.GetCount());
    }
    else
    {
        // Just write out each entry
        for (UINT i = 0; i < _aryEntries.GetCount(); i++)
        {
            CHK(pOutput->WriteIndent());
            CHK(pOutput->WriteString(_aryEntries[i]->_hlslText));
        }
    }

    // System semantic variables go last so that the variable count is not eaten up with them.
    if (_structType == IOStructType::Varying)
    {
        // The pixel shader input has SV_Position in it - make sure it is there
        CHK(pOutput->WriteIndent());
        CHK(pOutput->WriteString("float4 position : SV_Position;\n"));

        if (IsFeatureUsed(FeatureUsedFlags::glFrontFacing))
        {
            // If there is usage of front facing, then spit it out here
            CHK(pOutput->WriteIndent());
            CHK(pOutput->WriteString("bool frontFacing : SV_IsFrontFace;\n"));
        }
    }

    // Write the footer of the input struct
    pOutput->DecreaseIndent();
    CHK(pOutput->WriteString("};\n"));

    if (_structType == IOStructType::Varying && fWriteEmulationVariables)
    {
        // The pixel shader calls ReadPointCoordFromPSInput at the beginning of its entrypoint.  If
        // the point coord exists in the varying struct, we'll return that.  Otherwise, we'll return
        // float2(0.0, 1.0) which is what other browsers do. 
        // The vertex shader calls WritePointCoordToPSInput at the very end of its entrypoint. if 
        // the point coord exists in the varying struct, we'll make sure it gets written.  Otherwise,
        // we'll do nothing. 
        if (fPointCoordIsInVarying)
        {
            CHK(pOutput->WriteString(
                "float2 ReadPointCoordFromPSInput(in PSInput psInput)\n"
                "{\n"
                "return psInput.pointCoord;\n"
                "}\n"
                "void WritePointCoordToPSInput(inout PSInput psInput, in float2 pointCoord)\n"
                "{\n"
                "psInput.pointCoord = pointCoord;\n"
                "}\n"
                ));
        }
        else
        {
            CHK(pOutput->WriteString(
                "float2 ReadPointCoordFromPSInput(in PSInput psInput)\n"
                "{\n"
                "return float2(0.0, 1.0);\n"
                "}\n"
                "void WritePointCoordToPSInput(inout PSInput psInput, in float2 pointCoord)\n"
                "{\n"
                "}\n"
                ));
        }

        // The function that calculates the FragCoord is declared here. Whether or not this
        // function does anything depends on whether or not the variable is used in the fragment
        // shader. The vertex shader always outputs a call to this function since it is not
        // aware of what variables are used by the fragment shader it will be paired with in the future.
        if (pFragmentInfo->IsFeatureUsed(FeatureUsedFlags::glFragCoord))
        {
            // If gl_FragCoord is used, we pass through the flipped position as a varying so that it is properly
            // linearly interpolated. Technically we could do the perspective divide ourselves here, and pass
            // a normalized coordinate through, but gl_FragCoord.w is supposed to yield 1/w, and as such doing
            // the perspective divide here would lose that information (all w's would be passed through as 1.0).
            CHK(pOutput->WriteString(
                "void WriteFragCoordToPSInput(inout PSInput psInputArg, float4 flippedPositionArg)\n"
                "{\n"
                "psInputArg.fragCoord = flippedPositionArg;\n"
                "}\n"
                ));
        }
        else
        {
            CHK(pOutput->WriteString(
                "void WriteFragCoordToPSInput(inout PSInput psInputArg, float4 flippedPositionArg)\n"
                "{\n"
                "}\n"
                ));
        }
    }
    
    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetInfo
//
//  Synopsis:   Search the info list for the entry with the given name in the
//              GLSL code.
//
//-----------------------------------------------------------------------------
UINT CGLSLIOStructInfo::GetInfo(__in_z LPCSTR pszVSGLSLName) const
{
    for (UINT i = 0; i < _aryEntries.GetCount(); i++)
    {
        LPCSTR pszGLSLName = _spSymbolTable->NameFromIndex(_aryEntries[i]->_spInfo->GetSymbolIndex());

        // See if this entry has the same name
        if (::strcmp(pszGLSLName, pszVSGLSLName) == 0)
        {
            return i;
        }
    }

    return s_uNotFound;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddEntry
//
//-----------------------------------------------------------------------------
HRESULT CGLSLIOStructInfo::AddEntry(
    __in CVariableIdentifierInfo* pInfo,            // Identifier info for the entry
    __in CMemoryStream* pHLSLStream,                // HLSL stream for the normal entry
    __in CMemoryStream* pUnusedStream               // HLSL stream for the entry when unused
    )
{
    CHK_START;

    TSmartPointer<StructInfoEntry> spNewEntry;
    CHK(RefCounted<StructInfoEntry>::Create(/*out*/spNewEntry));
    
    spNewEntry->_spInfo = pInfo;

    CHK(pHLSLStream->ExtractString(spNewEntry->_hlslText));
    CHK(pUnusedStream->ExtractString(spNewEntry->_unusedText));
    CHK(_aryEntries.Add(spNewEntry));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVaryingVectorCount
//
//  Synopsis:   Tallies up the row count for each of the entries variables'
//              types.
//
//+----------------------------------------------------------------------------
UINT CGLSLIOStructInfo::GetVaryingVectorCount() const
{
    UINT uVaryingVectorCount = 0;
    HRESULT hr = S_OK;
    for (UINT i = 0; i < _aryEntries.GetCount() && hr == S_OK; i++)
    {
        UINT uEntryVectorCount;
        hr = _aryEntries[i]->_spInfo->UseType()->GetRowCount(&uEntryVectorCount);
        if (hr == S_OK)
        {
            hr = UIntAdd(uVaryingVectorCount, uEntryVectorCount, &uVaryingVectorCount);
        }

        if (FAILED(hr))
        {
            uVaryingVectorCount = UINT_MAX;
        }
    }

    return uVaryingVectorCount;
}

//+----------------------------------------------------------------------------
//
//  Function:   ComputeLinkedVertexStructInfo
//
//  Synopsis:   Takes both vertex and fragment varying infos and determines
//              whether there are entries in the vertex shader's struct
//              that are not needed. Returns a vertex varying info object
//              that only contains entries that are used in either shader.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLIOStructInfo::ComputeLinkedVertexStructInfo(
    __in const CGLSLIOStructInfo* pVertexInfo,                          // Vertex info to compute from
    __in const CGLSLIOStructInfo* pFragmentInfo,                        // Fragment info to compute from
    __deref_out CGLSLIOStructInfo** ppLinkedVertexInfo                  // Computed vertex info based on entries from vertex and fragment infos
    )
{
    CHK_START;

    Assert(pVertexInfo->_structType == IOStructType::Varying);
    Assert(pFragmentInfo->_structType == IOStructType::Varying);

    // Make a copy the vertex struct info (minus the entries for now). The
    // entries will be filled in the loop below.
    TSmartPointer<CGLSLIOStructInfo> spVertexInfoLinked;
    CHK(RefCounted<CGLSLIOStructInfo>::Create(
        pVertexInfo->_structType,
        pVertexInfo->_uFeatureUsedFlags,
        pVertexInfo->_spSymbolTable,
        pVertexInfo->_shaderType,
        /*out*/spVertexInfoLinked
        ));

    // Loop over the original vertex entries in order to detect entries
    // that are not used by either shader.
    for (UINT i = pVertexInfo->_aryEntries.GetCount(); i > 0; i--)
    {
        const TSmartPointer<StructInfoEntry>& spEntry = pVertexInfo->_aryEntries[i-1];

        // We'll default to copying all the entries into the new vertex struct info.
        bool fAddToComputedEntries = true;

        // However, unused vertex varyings are eligible to be pruned.
        if (!spEntry->_spInfo->IsUsed())
        {
            LPCSTR pszGLSLName = pVertexInfo->_spSymbolTable->NameFromIndex(spEntry->_spInfo->GetSymbolIndex());

            // Unused fragment varyings should not be present at this time, so
            // if the fragment varying cannot be found, and is not used by
            // the vertex shader, we can safely discard it from the struct.
            UINT uFoundIndex = pFragmentInfo->GetInfo(pszGLSLName);
            if (uFoundIndex == s_uNotFound)
            {
                fAddToComputedEntries = false;
            }
            else
            {
                // Unused fragment varyings should not have made its way into
                // the fragment info's entry list.
                Assert(pFragmentInfo->_aryEntries[uFoundIndex]->_spInfo->IsUsed());
            }
        }

        if (fAddToComputedEntries)
        {
            CHK(spVertexInfoLinked->_aryEntries.Add(spEntry.GetRawPointer()));
        }
    }
    
    *ppLinkedVertexInfo = spVertexInfoLinked.Extract();

    CHK_RETURN;
}
