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

#include "IStringStream.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      CMemoryStream
//
//  Synopsis:   Class to encapsulate a stream that is fed the generated scanner and the associated
//              input / output.
//
//              This is not meant to be consumed from outside of the lib - 
//              use the GLSLTranslate function rather than this directly.
//
//------------------------------------------------------------------------------
class CMemoryStream : public IStringStream
{
public:
    CMemoryStream();

    HRESULT SeekToStart();

    HRESULT ExtractString(
        __inout CMutableString<char>& spCode                        // Output ASCII string
        );

    HRESULT ReadChar(__out char* pChar);
  
    HRESULT SetSize(UINT uSize);
    HRESULT GetSize(__out UINT* puSize) const;

    // IStringStream implementation
    HRESULT WriteChar(char c) override;
    HRESULT WriteString(const char* pString) override;
    HRESULT WriteFormat(UINT uMax, const char* pszFormat, ...) override;
    HRESULT WriteIndent() override;
    void IncreaseIndent() override;
    void DecreaseIndent() override;

protected:
    HRESULT Initialize();

private:
    TSmartPointer<IStream> _spStream;
    UINT _uIndent;
};
