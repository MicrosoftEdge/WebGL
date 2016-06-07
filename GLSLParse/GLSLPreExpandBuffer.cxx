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
#include "GLSLPreExpandBuffer.hxx"
#include "GLSLPreParser.hxx"
#include "GLSLPreMacroDefinition.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLPreExpandBuffer::CGLSLPreExpandBuffer() :
    _stringBuffer(16)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Uses the given definition index to expand the definition.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreExpandBuffer::Initialize(
    __in YYLTYPE* pLocation,                    // Location of macro being expanded
    __in CGLSLPreMacroDefinition* pDefinition,  // The definition of the macro being expanded
    __in_opt CGLSLPreParamList *pParamList      // The parameter list to use for expansion
    )
{
    CHK_START;

    CHK(pDefinition->Expand(pLocation, pParamList, this));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteString
//
//  Synopsis:   Called from the parser as part of building the buffer.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreExpandBuffer::WriteString(__in_z const char* pszString)
{
    return _stringBuffer.Append(pszString);
}
