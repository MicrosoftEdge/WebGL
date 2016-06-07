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
#include "IdentifierNodeBase.hxx"
#include "IStringStream.hxx"
#include "IdentifierInfo.hxx"
#include "VariableIdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "CompoundStatementNode.hxx"
#include "InitDeclaratorListEntryNode.hxx"
#include "ParameterDeclaratorNode.hxx"
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "FieldSelectionNode.hxx"
#include "GLSLSymbolTable.hxx"
#include "RefCounted.hxx"
#include "GLSLType.hxx"
#include "GLSL.tab.h"

MtDefine(IdentifierNodeBase, CGLSLParser, "IdentifierNodeBase");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
IdentifierNodeBase::IdentifierNodeBase() : 
    _iSymbolIndex(-1),
    _fIsGenerated(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT IdentifierNodeBase::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int symbolIndex,                            // The index of the symbol
    const YYLTYPE &location                     // The location of the symbol
    )
{
    ParseTreeNode::Initialize(pParser);

    _iSymbolIndex = symbolIndex;
    _location = location;
    _fIsGenerated = false;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Clone the identifier
//
//-----------------------------------------------------------------------------
HRESULT IdentifierNodeBase::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    IdentifierNodeBase* pOriginalId = static_cast<IdentifierNodeBase*>(pOriginal);

    _iSymbolIndex = pOriginalId->_iSymbolIndex;
    _location = pOriginalId->_location;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyIdentifierName
//
//  Synopsis:   Verify that the identifier name is legal to declare in GLSL.
//
//              This is not checked for all identifiers, because clearly using
//              gl_FragCoord (for example) will not be a problem. But this is
//              called from places before adding the identifier.
//
//-----------------------------------------------------------------------------
HRESULT IdentifierNodeBase::VerifyIdentifierName()
{
    CHK_START;

    const char* pszText = GetText();

    if (::strncmp("gl_", pszText, 3) == 0 ||
        ::strncmp("webgl_", pszText, 6) == 0 ||
        ::strncmp("_webgl_", pszText, 7) == 0
        )
    {
        // Cannot start with these prefixes
        CHK(GetParser()->LogError(&GetLocation(), E_GLSLERROR_INVALIDIDENTIFIERNAME, GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    if (!_fIsGenerated && ::strstr(pszText, "__") != nullptr)
    {
        // Consecutive underscores are reserved. We use them internally for dynamically generated
        // identifiers to ensure no name collisions
        CHK(GetParser()->LogError(&GetLocation(), E_GLSLERROR_INVALIDIDENTIFIERNAME, GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetText
//
//  Synopsis:   Get the text of the identifier
//
//-----------------------------------------------------------------------------
const char* IdentifierNodeBase::GetText() const
{
    return GetParser()->UseSymbolTable()->NameFromIndex(_iSymbolIndex);
}
