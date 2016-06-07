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
#include "GLSLStreamParserInput.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLStreamParserInput::CGLSLStreamParserInput()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Initialize from the given string.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLStreamParserInput::Initialize(__in CMemoryStream* pInput)
{
    CHK_START;

    // We want the input to start at the start
    CHK(pInput->SeekToStart());

    _spInput = pInput;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PullInput
//
//  Synopsis:   Method called by tokenizer to get more input.
//
//-----------------------------------------------------------------------------
int CGLSLStreamParserInput::PullInput(__out_ecount(1) char* buf)
{
    if (SUCCEEDED(_spInput->ReadChar(buf)))
    {
        return 1;
    }
    else
    {
        return 0; // YY_NULL
    }
}
