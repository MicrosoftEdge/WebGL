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
#include "GLSLPreTokenList.hxx"
#include "GLSLPreExpandBuffer.hxx"

//+-----------------------------------------------------------------------------
//
//  Function:   AddToken
//
//  Synopsis:   Called to add a token to the macro definition.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreTokenList::AddToken(__in_z const char* pszToken) 
{
    return _rgTokenList.Add(pszToken);
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetCount
//
//  Synopsis:   Return the count of tokens in the definition.
//
//------------------------------------------------------------------------------
UINT CGLSLPreTokenList::GetCount() const 
{ 
    return _rgTokenList.GetCount(); 
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetItemText
//
//  Synopsis:   Get the text for one of the tokens in the definition.
//
//------------------------------------------------------------------------------
char* CGLSLPreTokenList::GetItemText(UINT index) 
{ 
    return _rgTokenList[index]; 
}

//+-----------------------------------------------------------------------------
//
//  Function:   ConvertToString
//
//  Synopsis:   Convert the token list into a string.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreTokenList::ConvertToString(__inout CMutableString<char> &converted)
{
    CHK_START;

    for (UINT i = 0; i < _rgTokenList.GetCount(); i++)
    {
        CHK(converted.Append(_rgTokenList[i]));

        if (i != _rgTokenList.GetCount() - 1)
        {
            CHK(converted.Append(" ", 1));
        }
    }

    CHK_RETURN;
}
