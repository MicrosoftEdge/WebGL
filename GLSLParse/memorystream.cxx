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
#include "MemoryStream.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CMemoryStream::CMemoryStream() :
    _uIndent(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::Initialize()
{
    return ::CreateStreamOnHGlobal(nullptr, TRUE, &_spStream);
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteChar
//
//  Synopsis:   Write a single character to the stream.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::WriteChar(char c)
{
    return _spStream->Write(&c, 1, nullptr);
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteString
//
//  Synopsis:   Write a C string to the stream.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::WriteString(const char *pString)
{
    CHK_START;

    size_t length;
    CHK(::StringCchLengthA(pString, STRSAFE_MAX_CCH, &length));

    CHK(_spStream->Write(pString, length, nullptr));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteFormat
//
//  Synopsis:   printf-style format function for the stream. There is a limit
//              set locally on the length that the result can have, exceeding
//              this limit will propogate an error.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::WriteFormat(UINT uMax, const char* pszFormat, ...)
{
    CHK_START;

    va_list args;
    va_start(args, pszFormat);

    TSmartArray<char> spFormated;
    CHK(spFormated.New(uMax));
    CHK(::StringCchVPrintfA(spFormated, uMax, pszFormat, args));

    size_t length;
    CHK(::StringCchLengthA(spFormated, STRSAFE_MAX_CCH, &length));
    CHK(_spStream->Write(spFormated, length, nullptr));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ExtractString
//
//  Synopsis:   Convert the stream into a string.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::ExtractString(
    __inout CMutableString<char>& spCode                        // Output ASCII string
    )
{
    CHK_START;

    UINT length;
    CHK(GetSize(&length));

    // Now lock the memory and copy it across
    if (length != 0)
    {
        HGLOBAL hStreamGlobal;
        CHK(::GetHGlobalFromStream(_spStream, &hStreamGlobal));

        LPVOID pStreamMem = ::GlobalLock(hStreamGlobal);
        hr = spCode.Append(static_cast<char*>(pStreamMem), length);
        ::GlobalUnlock(hStreamGlobal);
        CHK(hr);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetSize
//
//  Synopsis:   Set the current size of the stream. Use in conjunction with
//              Seek to reinit the stream.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::SetSize(UINT uSize)
{
    ULARGE_INTEGER newSize;
    newSize.QuadPart = uSize;

    return _spStream->SetSize(newSize);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetSize
//
//  Synopsis:   Get the current size of the stream.
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::GetSize(__out UINT* puSize) const
{
    CHK_START;

    HGLOBAL hStreamGlobal;
    CHK(::GetHGlobalFromStream(_spStream, &hStreamGlobal));

    // Grab the size, which could be zero if the tree was valid but nothing was output
    SIZE_T length = ::GlobalSize(hStreamGlobal);
    (*puSize) = static_cast<UINT>(length);

    CHK_RETURN 
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteIndent
//
//  Synopsis:   Write the current indent size to the stream
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::WriteIndent()
{
    for (UINT i = 0 ; i < _uIndent; i++)
    {
        WriteString("  ");
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IncreaseIndent
//
//-----------------------------------------------------------------------------
void CMemoryStream::IncreaseIndent()
{
    _uIndent++;
}

//+----------------------------------------------------------------------------
//
//  Function:   DecreaseIndent
//
//-----------------------------------------------------------------------------
void CMemoryStream::DecreaseIndent()
{
    Assert(_uIndent > 0);
    if (_uIndent > 0)
    {
        _uIndent--;
    }    
}

//+----------------------------------------------------------------------------
//
//  Function:   ReadChar
//
//  Synopsis:   Pull a character from the stream
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::ReadChar(__out char* pChar)
{
    CHK_START;

    ULONG cbRead;
    CHK(_spStream->Read(pChar, 1, &cbRead));
    CHKB(cbRead == 1);    

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SeekToStart
//
//  Synopsis:   Seek to the start of the stream
//
//-----------------------------------------------------------------------------
HRESULT CMemoryStream::SeekToStart()
{
    LARGE_INTEGER li;
    li.QuadPart = 0;

    return _spStream->Seek(li, STREAM_SEEK_SET, nullptr);
}
