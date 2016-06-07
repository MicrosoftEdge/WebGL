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
#include <foundation/collections.hxx>
#include "KnownSymbols.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLSymbolTable
//
//  Synopsis:   Class to encapsulate the symbol table for the GLSL parse tree.
//
//              The implementation of the table is an array of strings, with
//              the addition of a lookup array that indicates for each ASCII
//              codepoint if an entry starting with that codepoint exists.
//
//------------------------------------------------------------------------------
class CGLSLSymbolTable : public IUnknown
{
public:
    HRESULT EnsureSymbolIndex(
        __in_z const char* pszSymbol,                                       // Symbol to look up / add
        __out int* pIndex                                                   // Index of symbol
        );

    HRESULT LookupSymbolIndex(
        __in_z const char* pszSymbol,                                       // Symbol to look up / add
        __out int* pIndex                                                   // Index of symbol
        );

    const char* NameFromIndex(
        int index                                                           // Index of symbol
        ) const;

protected:
    HRESULT Initialize();

private:
    CModernArray<CSmartSTR, CSmartStringTraits<CSmartSTR>> _rgSymbolList;   // The current list of symbols
    bool _rgSymbolExists[128];                                              // Map of ASCII char to symbol existence
};
