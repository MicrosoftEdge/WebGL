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
#include "GLSLSymbolTable.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Initialize the acceleration structure used for lookup.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLSymbolTable::Initialize()
{
    for (int i = 0; i < 128; i++)
    {
        _rgSymbolExists[i] = false;
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   EnsureSymbolIndex
//
//  Synopsis:   Look up or add a symbol to the symbol table. Do not call with
//              strings that have characters outside of ASCII 0-127.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLSymbolTable::EnsureSymbolIndex(
    __in_z const char* pszSymbol,           // Symbol to look up / add
    __out int* pIndex                       // Index of symbol
    )
{
    CHK_START;

    // If we did not find it, then add it
    if (FAILED(LookupSymbolIndex(pszSymbol, pIndex)))
    {
        CHKB(pszSymbol[0] >= 0 && pszSymbol[0] < 128);

        CHK(_rgSymbolList.Add(pszSymbol));
        _rgSymbolExists[pszSymbol[0]] = true;
        (*pIndex) = static_cast<int>(_rgSymbolList.GetCount() - 1);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   LookupSymbolIndex
//
//  Synopsis:   Look up or add a symbol to the symbol table. Do not call with
//              strings that have characters outside of ASCII 0-127.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLSymbolTable::LookupSymbolIndex(
    __in_z const char* pszSymbol,           // Symbol to look up
    __out int* pIndex                       // Index of symbol
    )
{
    CHK_START;

    CHKB(pszSymbol[0] >= 0 && pszSymbol[0] < 128);

    bool fFound = false;

    // We first check if there are any symbols that start with the same char
    if (_rgSymbolExists[pszSymbol[0]])
    {
        // Now search everything
        for (UINT i = 0; i < _rgSymbolList.GetCount(); i++)
        {
            if (::strcmp(_rgSymbolList[i], pszSymbol) == 0)
            {
                (*pIndex) = static_cast<int>(i);
                fFound = true;
                break;
            }
        }
    }

    CHKB_HR(fFound, E_INVALIDARG);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   NameFromIndex
//
//  Synopsis:   Look up or add a symbol to the symbol table
//
//-----------------------------------------------------------------------------
const char* CGLSLSymbolTable::NameFromIndex(
    int index                                                           // Index of symbol
    ) const
{
    Assert(index >=0 && index < (int)_rgSymbolList.GetCount());

    return _rgSymbolList[index];
}
