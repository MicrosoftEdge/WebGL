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
#include "GLSLUnicodeConverter.hxx"
#include "MemoryStream.hxx"
#include "RefCounted.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   s_rgLegalChar
//
//  Synopsis:   This maps all of the 128 ASCII characters into a boolean that
//              determines if they are legal.
//
//-----------------------------------------------------------------------------
const bool CGLSLUnicodeConverter::s_rgLegalChar[128] = {
/*            0*/ false,  false,  false,  false,  false,  false,  false,  false,  
/*  WWWWW     8*/ false,  true,   true,   true,   true,   true,   false,  false,  
/*           16*/ false,  false,  false,  false,  false,  false,  false,  false,  
/*           24*/ false,  false,  false,  false,  false,  false,  false,  false,  
/* W!"#$%&'  32*/ true,   true,   false,  true,   false,  true,   true,   false,  
/* ()*+,-./  40*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* 01234567  48*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* 89:;<=>?  56*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* @ABCDEFG  64*/ false,  true,   true,   true,   true,   true,   true,   true,  
/* HIJKLMNO  72*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* PQRSTUVW  80*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* XYZ[\]^_  88*/ true,   true,   true,   true,   false,  true,   true,   true,  
/* `abcdefg  96*/ false,  true,   true,   true,   true,   true,   true,   true,  
/* hijklmno 104*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* pqrstuvw 112*/ true,   true,   true,   true,   true,   true,   true,   true,  
/* xyz{|}~  120*/ true,   true,   true,   true,   true,   true,   true,   false,  
};

//+----------------------------------------------------------------------------
//
//  Function:   s_rgWhitespaceChar
//
//  Synopsis:   This maps the first 33 ASCII characters into a boolean that
//              determines if they are whitespace.
//
//-----------------------------------------------------------------------------
const bool CGLSLUnicodeConverter::s_rgWhitespaceChar[33] = {
/*            0*/ false,  false,  false,  false,  false,  false,  false,  false,  
/*  WWWWW     8*/ false,  true,   true,   true,   true,   true,   false,  false,  
/*           16*/ false,  false,  false,  false,  false,  false,  false,  false,  
/*           24*/ false,  false,  false,  false,  false,  false,  false,  false,  
/* W         32*/ true };

//+----------------------------------------------------------------------------
//
//  Function:   ConvertToAscii
//
//  Synopsis:   GLSL is defined in terms of a subset of ASCII, so when we are 
//              given a BSTR from the DOM to compile, we have to convert it to
//              ASCII.
//
//              Since we only support a limited character set, we can be
//              faster and more explicit than OS functions. We can look at
//              each char and check that it is in our map and just cast if
//              it is.
//
//              Because this is called before preprocessing, characters outside
//              of the allowed set do not cause an error. They are transformed
//              into a special char ($) which is not legal, and the GLSL lexer
//              will cause an error if this character is encountered.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLUnicodeConverter::ConvertToAscii(
    __in BSTR bstrInput,                                        // Input BSTR to convert to Ascii
    __deref_out CMemoryStream** ppConverted                     // Output ASCII stream
    )
{
    CHK_START;

    // Create the output stream
    TSmartPointer<CMemoryStream> spConverted;
    CHK(RefCounted<CMemoryStream>::Create(/*out*/spConverted));

    UINT uLength = ::SysStringLen(bstrInput);
    for (UINT i = 0; i < uLength; i++)
    {
        wchar_t c = bstrInput[i];
        char converted = static_cast<char>(c);

        // Reject anything that is not ASCII, or not in the GLSL char spec
        if (!IsLegalChar(c))
        {
            converted = '$';
        }

        // Store the converted value now
        CHK(spConverted->WriteChar(converted));
    }
    
    // Our preprocessor assumes there is at least one newline in its
    // grammar. So we add it here.
    CHK(spConverted->WriteChar('\n'));

    (*ppConverted) = spConverted.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLegalChar
//
//  Synopsis:   Function test test legality of a character.
//
//-----------------------------------------------------------------------------
bool CGLSLUnicodeConverter::IsLegalChar(wchar_t c)
{
    if (c < 0 || c >= ARRAYSIZE(s_rgLegalChar) || !s_rgLegalChar[c])
    {
        return false;
    }

    return true;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsWhitespaceChar
//
//  Synopsis:   Function to test if character is GLSL whitespace.
//
//-----------------------------------------------------------------------------
bool CGLSLUnicodeConverter::IsWhitespaceChar(char c)
{
    if (c < 0 || c >= ARRAYSIZE(s_rgWhitespaceChar) || !s_rgWhitespaceChar[c])
    {
        return false;
    }

    return true;
}
