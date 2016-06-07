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
#include "IOStructNode.hxx"
#include "MemoryStream.hxx"
#include "FullySpecifiedTypeNode.hxx"
#include "InitDeclaratorListNode.hxx"
#include "GLSLParser.hxx"
#include "GLSLIOStructInfo.hxx"
#include "RefCounted.hxx"
#include "GLSL.tab.h"

MtDefine(IOStructNode, CGLSLParser, "IOStructNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
IOStructNode::IOStructNode() : 
    _structType(IOStructType::Undefined)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT IOStructNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    IOStructType::Enum type                     // What type of IO the struct represents
    )
{
    ParseTreeNode::Initialize(pParser);

    _structType = type;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   CompareIdentifiers
//
//  Synopsis:   Compare two identifiers alphabetically
//
//-----------------------------------------------------------------------------
int IOStructNode::CompareIdentifiers(
    __in CGLSLSymbolTable* pTable,                      // Symbol table to look up names in
    const TSmartPointer<CVariableIdentifierInfo>* pA,   // First identifier to compare
    const TSmartPointer<CVariableIdentifierInfo>* pB    // Second identifier to compare
    )
{
    CVariableIdentifierInfo* pAInfo = (*pA);
    CVariableIdentifierInfo* pBInfo = (*pB);

    if (pAInfo->GetSymbolIndex() != -1 && pBInfo->GetSymbolIndex() != -1)
    {
        return ::strcmp(
            pTable->NameFromIndex(pAInfo->GetSymbolIndex()),
            pTable->NameFromIndex(pBInfo->GetSymbolIndex())
            );
    }
    else
    {
        return 0;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateIdentifierNames
//
//  Synopsis:   Create HLSL names for the identifiers in this struct. The names
//              are based on alphabetical order to ensure that the same order
//              will occur between shaders.
//
//-----------------------------------------------------------------------------
HRESULT IOStructNode::CreateIdentifierNames()
{
    CHK_START;

    // Collect identifier info for all of the identifers that are in the struct
    CModernArray<TSmartPointer<CVariableIdentifierInfo>> rgIdentifiers;

    for (UINT i = 0; i < GetChildCount(); i++)
    {
        InitDeclaratorListNode *pListNode = GetChild(i)->GetAs<InitDeclaratorListNode>();

        for(UINT j = 0; j < pListNode->GetIdentifierCount(); j++)
        {
            VariableIdentifierNode* pIdentifier = pListNode->GetIdentifier(j);

            TSmartPointer<CVariableIdentifierInfo> spInfo;
            CHK(pIdentifier->GetVariableIdentifierInfo(&spInfo));
            CHK(rgIdentifiers.Add(spInfo));
        }
    }

    // Sort the identifiers by GLSL name, and then use that order to set the
    // HLSL names.
    rgIdentifiers.Sort(GetParser()->UseSymbolTable(), &CompareIdentifiers);

    for (UINT i = 0; i < rgIdentifiers.GetCount(); i++)
    {
        CHK(rgIdentifiers[i]->NameFromIndex(i));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT IOStructNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    if (GetParser()->GetWriteInputs())
    {
        // Create the HLSL names
        CHK(CreateIdentifierNames());

        // Make a struct info
        TSmartPointer<CGLSLIOStructInfo> spStructInfo;
        CHK(RefCounted<CGLSLIOStructInfo>::Create(
            _structType,
            GetParser()->GetFeaturesUsed(),
            GetParser()->UseSymbolTable(), 
            GetParser()->GetShaderType(),
            /*out*/spStructInfo
            ));

        // Make a stream for writing entries, both regular and unused
        TSmartPointer<CMemoryStream> spEntryStream;
        CHK(RefCounted<CMemoryStream>::Create(/*out*/spEntryStream));

        TSmartPointer<CMemoryStream> spUnusedStream;
        CHK(RefCounted<CMemoryStream>::Create(/*out*/spUnusedStream));

        // Add the entries
        for (UINT i = 0; i < GetChildCount(); i++)
        {
            // Get the declarator list for this entry
            InitDeclaratorListNode* pDeclList = GetChild(i)->GetAs<InitDeclaratorListNode>();

            // We must add an enry for each identifier in the declaration list,
            // since comma separated declarations are represented by one list but
            // structs need each member declared on its own.
            UINT cIdentifiers = pDeclList->GetIdentifierCount();
            for (UINT j = 0; j < cIdentifiers; j++)
            {
                // Get the info for the identifier
                TSmartPointer<CVariableIdentifierInfo> spInfo;
                CHK(pDeclList->GetIdentifier(j)->GetVariableIdentifierInfo(&spInfo));

                const bool fVertexVarying = (_structType == IOStructType::Varying && GetParser()->GetShaderType() == GLSLShaderType::Vertex);

                // A variable that is used must be added to the struct.
                // The only unused variables we add are unused vertex varyings; 
                // these must be added for detection of use by fragment shader at link time.
                const bool fAddToStruct = spInfo->IsUsed() || fVertexVarying;
                if (fAddToStruct)
                {
                    // Set the streams up
                    CHK(spEntryStream->SeekToStart());
                    CHK(spEntryStream->SetSize(0));

                    CHK(spUnusedStream->SeekToStart());
                    CHK(spUnusedStream->SetSize(0));

                    // Write the entry for this child
                    CHK(spEntryStream->WriteIndent());
                    CHK(pDeclList->OutputTranslation(j, /*fUnused*/false, spEntryStream));

                    // Write the entry for this child
                    CHK(spUnusedStream->WriteIndent());
                    CHK(pDeclList->OutputTranslation(j, /*fUnused*/true, spUnusedStream));

                    // Add the entry
                    CHK(spStructInfo->AddEntry(spInfo, spEntryStream, spUnusedStream));
                }
            }
        }

        // If it is not a varying, then output this to the main stream
        if (_structType == IOStructType::Varying)
        {
            GetParser()->SetVaryingStructInfo(spStructInfo);
        }
        else
        {
            if (GetParser()->GetWriteInputs())
            {
                CHK(spStructInfo->OutputHLSL(nullptr, pOutput));
            }
        }
    }

    CHK_RETURN;
}
