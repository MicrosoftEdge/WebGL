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

#include "GLSLPreParamList.hxx"
#include "pre.tab.h"

class CGLSLPreExpandBuffer;

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLPreMacroDefinition
//
//  Synopsis:   Class to represent list of tokens collected as part of a
//              preprocessor define.
//
//------------------------------------------------------------------------------
class CGLSLPreMacroDefinition : public IUnknown
{
public:
    CGLSLPreMacroDefinition();

    void SetIdentifierIndex(int index);
    void SetParameters(__in CGLSLPreParamList* pParameters);

    HRESULT AddToken(__in_z const char* pszToken);
    HRESULT AddParamToken(UINT uParamIndex);
    HRESULT SetToNumber(int value);

    HRESULT Expand(
        __in YYLTYPE* pLocation,                                            // Location of macro being expanded
        __in_opt CGLSLPreParamList* pParams,                                // The optional parameters for the expansion
        __inout CGLSLPreExpandBuffer* pBuffer                               // The buffer to expand into
        );

    UINT GetCount() const;
    char* GetItemText(UINT index);
    int GetIdentifierIndex() const;
    bool HasParameters() const;
    bool IsEqual(__in const CGLSLPreMacroDefinition* pOther) const;
    void Finalize() { _fFinalized = true; }
    bool IsFinalized() const { return _fFinalized; }

protected:
    HRESULT Initialize(__in CGLSLPreParser* pPreParser);

private:
    CGLSLPreParser* _pPreParser;                                            // The preparser that owns this definition
    int _idSymbol;                                                          // Symbol table index of identifier being defined
    CMutableStringModernArray _rgTokenList;                                 // The collected parameters
    TSmartPointer<CGLSLPreParamList> _spParameters;                         // The parameters on the definition
    bool _fFinalized;                                                       // Flag to indicate that the definition is finalized

    static const UINT MAX_PARAM = 64;                                       // Maximum number of allowed parameters
};
