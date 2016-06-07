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
#include "ParseTreeNode.hxx"    // Tree node is defined here

class CGLSLParser;              // Forward define for lexer header to compile

#include "GLSL.tab.h"           // We need this to include the lexer header

#pragma warning(disable:28718)
#include "lex.GLSL.h"           // We need this for the scanner type definitions

#include "GLSLParser.hxx"

//+-----------------------------------------------------------------------------
//
//  Function:   GLSLerror
//
//  Synopsis:   Called from the parser and lexer when an error is encountered.
//
//------------------------------------------------------------------------------
void GLSLerror(__in YYLTYPE* pLocation, yyscan_t scanner, __in_z const char* pszErrorText)
{
    CGLSLParser* pParser = GLSLget_extra(scanner);

    pParser->NotifyError(pLocation, pszErrorText);
}

//+-----------------------------------------------------------------------------
//
//  Function:   GLSLUpdateLocation
//
//  Synopsis:   Called as a user action to record the position of tokens.
//
//------------------------------------------------------------------------------
void GLSLUpdateLocation(__in CGLSLParser* pParser, __in YYLTYPE* pLocation, int lineNo, int tokenLength)
{
    pParser->UpdateLocation(pLocation, lineNo, tokenLength);
}

//+-----------------------------------------------------------------------------
//
//  Function:   GLSLRecordNewline
//
//  Synopsis:   Called on a newline to assist in location management.
//
//------------------------------------------------------------------------------
void GLSLRecordNewline(__in CGLSLParser* pParser)
{
    pParser->RecordNewline();
}

//+----------------------------------------------------------------------------
//
//  Function:   GLSLInput
//
//  Synopsis:   Global function called for pulling input into the parser. Used
//              instead of a stdin stream by the tokenizer.
//
//-----------------------------------------------------------------------------
int GLSLInput(__in CGLSLParser* pOutput, __out_ecount(1) char* buf, int /*max_size*/)
{
    return pOutput->UseInput()->PullInput(buf);
}

//+----------------------------------------------------------------------------
//
//  Function:   GLSLEnsureSymbolIndex
//
//  Synopsis:   Global function called for getting a symbol index. Defined
//              as a global to avoid circular dance when including header files.
//
//-----------------------------------------------------------------------------
HRESULT GLSLEnsureSymbolIndex(__in CGLSLParser* pParser, __in_z char* pSymbol, __out int *pIndex)
{
    return pParser->EnsureSymbolIndex(pSymbol, pIndex);
}

//+-----------------------------------------------------------------------------
//
//  Function:   GLSLwrap
//
//  Synopsis:   Called when input is exhausted to determine next action. We
//              always want to finish at this point.
//
//------------------------------------------------------------------------------
int GLSLwrap(yyscan_t)
{
    return 1;
}
