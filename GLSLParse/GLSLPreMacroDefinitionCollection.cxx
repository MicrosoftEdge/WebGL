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
#include "GLSLPreMacroDefinitionCollection.hxx"
#include "RefCounted.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   CreateDefinition
//
//  Synopsis:   Create a definition
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinitionCollection::CreateAndAddDefinition(
    __in CGLSLPreParser* pPreParser,                                    // The parser that owns the definition
    __out_opt int* pDefIndex,                                           // Returned index of definition
    __deref_out_opt CGLSLPreMacroDefinition** ppDefinition              // The definition
    )
{
    CHK_START;

    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    CHK(RefCounted<CGLSLPreMacroDefinition>::Create(pPreParser, /*out*/spDefinition));
    CHK(_rgDefinitions.Add(spDefinition));

    // The new definition was created at the end
    if (pDefIndex != nullptr)
    {
        (*pDefIndex) = static_cast<int>(_rgDefinitions.GetCount() - 1);
    }

    if (ppDefinition != nullptr)
    {
        (*ppDefinition) = spDefinition.Extract();
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDefinition
//
//  Synopsis:   Get the given definition. If the internal array has a null
//              pointer at this position then it fails.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinitionCollection::GetDefinition(
    int defIndex,                                                       // The definition index
    __deref_out CGLSLPreMacroDefinition** ppDefinition                  // The definition
    )
{
    if (_rgDefinitions[defIndex] != nullptr)
    {
        _rgDefinitions[defIndex].CopyTo(ppDefinition);
        return S_OK;
    }

    return E_INVALIDARG;
}

//+----------------------------------------------------------------------------
//
//  Function:   FindDefinition
//
//  Synopsis:   Search definitions for the corresponding value.
//
//              The definition must not have been removed (which would make it
//              nullptr) and it must be finalized.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinitionCollection::FindDefinition(
    int token,                                                          // Token that specifies the macro being defined
    __out_opt int* pDefIndex,                                           // Index of definition in definition table
    __deref_out_opt CGLSLPreMacroDefinition** ppValue                   // What the definition of this define is (what replaces the token)
    )
{
    for (UINT i = 0; i < _rgDefinitions.GetCount(); i++)
    {
        if (_rgDefinitions[i] != nullptr && 
            _rgDefinitions[i]->GetIdentifierIndex() == token && 
            _rgDefinitions[i]->IsFinalized())
        {
            if (pDefIndex != nullptr)
            {
                (*pDefIndex) = static_cast<int>(i);
            }

            if (ppValue != nullptr)
            {
                _rgDefinitions[i].CopyTo(ppValue);
            }

            return S_OK;
        }
    }

    return E_INVALIDARG;
}

//+----------------------------------------------------------------------------
//
//  Function:   RemoveDefinitionFromIndex
//
//  Synopsis:   Remove a definition from our definition list.
//
//              We don't want the index to change after creation, because
//              complex operations over time might depend on the index staying
//              the same.
//
//              Removal basically clears out the entry at the index, and
//              it will be forever skipped.
//
//-----------------------------------------------------------------------------
void CGLSLPreMacroDefinitionCollection::RemoveDefinitionFromIndex(
    int defIndex                                                        // Index of definition to remove
    )
{
    _rgDefinitions[defIndex].Release();
}

//+----------------------------------------------------------------------------
//
//  Function:   RemoveDefinitionFromIdentifier
//
//  Synopsis:   Remove a definition from our definition list.
//
//              We don't want the index to change after creation, because
//              complex operations over time might depend on the index staying
//              the same.
//
//              Removal basically clears out the entry at the index, and
//              it will be forever skipped.
//
//-----------------------------------------------------------------------------
void CGLSLPreMacroDefinitionCollection::RemoveDefinitionFromIdentifier(
    int iToken                                                          // Index of definition to remove
    )
{
    int existingDefIndex;
    if (SUCCEEDED(FindDefinition(iToken, &existingDefIndex, /*spDefinition*/nullptr)))
    {
        RemoveDefinitionFromIndex(existingDefIndex);
    }
}
