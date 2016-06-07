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
﻿#include "headers.hxx"
#include "ParserTestUtils.hxx"

bool CompareOutputs(const char* pszFirst, const char* pszSecond, int *pFirstDiff)
{
    int line = 1;

    for(int i = 0; ; i++)
    {
        // We declare these variables so that the debugger will show strings starting at the
        // comparison point. This makes pinpointing the difference easier.
        const char* pszFirstAtDiff = &pszFirst[i];
        const char* pszSecondAtDiff = &pszSecond[i];

        // If they are different then we found the line with the difference. If the strings
        // differ in length then we will hit this too (because one will have a null char and
        // the other will not).
        if ((*pszFirstAtDiff) != (*pszSecondAtDiff))
        {
            // Set a breakpoint in here and these variables start where the difference is
            (*pFirstDiff) = line;
            return false;
        }

        // From now on we can check one of them and it counts for both. See if we are at
        // the strings end. If we get here then they are the same.
        if (!pszFirst[i])
        {
            (*pFirstDiff) = -1;
            return true;
        }

        // If we have a newline
        if ((*pszFirst) == '\n')
        {
            line++;
        }
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateGiantIdentifierToken
//
//  Synopsis:   Create a massive token that is an identifier.
//
//-----------------------------------------------------------------------------
HRESULT CreateGiantIdentifierToken(__inout CMutableString<char>& strToken)
{
    CHK_START;

    strToken.SetInitialSize(300);

    // Make 256 chars (which is the max)
    for (int i = 0; i < 32; i++)
    {
        CHK(strToken.Append("abcd1234"));
    }

    // Now go past the rubicon
    CHK(strToken.Append("x"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateGiantIntToken
//
//  Synopsis:   Create a massive token that is an int
//
//-----------------------------------------------------------------------------
HRESULT CreateGiantIntToken(__inout CMutableString<char>& strToken)
{
    CHK_START;

    strToken.SetInitialSize(300);

    // Add the single '1' that starts it off
    CHK(strToken.Append("1"));

    // Make 256 chars more (bringing it over)
    for (int i = 0; i < 32; i++)
    {
        CHK(strToken.Append("01020304"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateGiantFloatToken
//
//  Synopsis:   Create a massive token that is a float
//
//-----------------------------------------------------------------------------
HRESULT CreateGiantFloatToken(__inout CMutableString<char>& strToken)
{
    CHK_START;

    strToken.SetInitialSize(300);

    // Add the single '1.' that starts it off
    CHK(strToken.Append("1."));

    // Make 256 chars more (bringing it over)
    for (int i = 0; i < 32; i++)
    {
        CHK(strToken.Append("01020304"));
    }

    CHK_RETURN;
}
