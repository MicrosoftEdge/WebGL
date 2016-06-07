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
#include "GLSLPreMacroDefinition.hxx"
#include "GLSLUnicodeConverter.hxx"
#include "GLSLPreExpandBuffer.hxx"
#include "GLSLPreParamList.hxx"
#include "GLSLError.hxx"
#include "GLSLPreParser.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLPreMacroDefinition::CGLSLPreMacroDefinition() :
    _idSymbol(-1),
    _fFinalized(false)
{
}

//+-----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinition::Initialize(__in CGLSLPreParser* pPreParser)
{
    _pPreParser = pPreParser;

    return S_OK;
}

//+-----------------------------------------------------------------------------
//
//  Function:   SetIdentifierIndex
//
//  Synopsis:   Called to set the index in the symbol table of the identifier
//              of the macro.
//
//------------------------------------------------------------------------------
void CGLSLPreMacroDefinition::SetIdentifierIndex(int index) 
{ 
    _idSymbol = index; 
}

//+-----------------------------------------------------------------------------
//
//  Function:   SetParameters
//
//  Synopsis:   Called to set the parameter list for the definition.
//
//------------------------------------------------------------------------------
void CGLSLPreMacroDefinition::SetParameters(__in CGLSLPreParamList* pParameters) 
{ 
    _spParameters = pParameters; 
}

//+-----------------------------------------------------------------------------
//
//  Function:   AddToken
//
//  Synopsis:   Called to add a token to the macro definition.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinition::AddToken(__in_z const char* pszToken) 
{ 
    CHK_START;

    bool fFound = false;
    if (_spParameters != nullptr)
    {
        for (UINT i = 0; i < _spParameters->GetCount(); i++)
        {
            if (::strcmp(_spParameters->GetParameter(i), pszToken) == 0)
            {
                fFound = true;
                CHK(AddParamToken(i));
                break;
            }
        }
    }

    if (!fFound)
    {
        CHK(_rgTokenList.Add(pszToken));
    }

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   AddParamToken
//
//  Synopsis:   Called to add a reference to a parameter to the macro
//              definition.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinition::AddParamToken(UINT uParamIndex)
{
    CHK_START;

    // Make sure that this token is in range
    CHKB(_spParameters != nullptr);
    CHKB(uParamIndex < _spParameters->GetCount());

    // We will construct a string that represents the token - we can use '@'
    // because it is not a legal char and after conversion cannot appear in
    // the stream, so it can be used to identifity these tokens later.
    //
    // We assume that the parameter count always allows a parameter index
    // to be represented with a character.
    static_assert(MAX_PARAM < 127, "Maximum parameter count needs to be in ASCII range");
    AssertSz(!CGLSLUnicodeConverter::IsLegalChar('@'), "Preprocessor design assumes that '@' is not in input");

    char szTokenString[] = "@ ";
    szTokenString[1] = static_cast<char>(uParamIndex + 1);

    CHK(_rgTokenList.Add(szTokenString));

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   SetToNumber
//
//  Synopsis:   Called to set the value of the macro to a number
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinition::SetToNumber(int value) 
{ 
    CHK_START;

    // Technically, the definition could have a set of tokens and parameters
    // stored here. Calling this is meant to make the definition resolve
    // to a single integer, so we clean out everything.
    _spParameters.Release();
    _rgTokenList.RemoveAll();

    // Add a single token representing the number
    CMutableString<char> numString;
    CHK(numString.Format(64, "%d", value));
    CHK(_rgTokenList.Add(numString));

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   Expand
//
//  Synopsis:   Expand the definition given the parameters.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreMacroDefinition::Expand(
    __in YYLTYPE* pLocation,                                            // Location of macro being expanded
    __in_opt CGLSLPreParamList* pParams,                                // The optional parameters for the expansion
    __inout CGLSLPreExpandBuffer* pBuffer                               // The buffer to expand into
    )
{
    CHK_START;

    if (pParams == nullptr)
    {
        // No parameters were provided to expand the macro. This is only valid if we
        // have no parameters in the definition. The lexer should have ensured this
        // because if the macro has parameters, it will demand that they be passed.
        Assert(_spParameters == nullptr);
        CHKB(_spParameters == nullptr);
    }
    else
    {
        // If we have provided parameters then the definition needs them. The lexer should
        // have ensured this with the way that the states are set up.
        Assert(_spParameters != nullptr);
        CHKB(_spParameters != nullptr);

        // The parameter count must be equal
        if (_spParameters->GetCount() != pParams->GetCount())
        {
            CHK(_pPreParser->LogError(pLocation, E_GLSLERROR_PREINVALIDMACROPARAMCOUNT, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);             
        }
    }

    // Go through the stream and either output or replace each thing
    for (UINT i = 0; i < _rgTokenList.GetCount(); i++)
    {
        if (_rgTokenList[i].GetLength() == 2 && _rgTokenList[i][0] == '@')
        {
            // Time to substitute a parameter
            UINT uParamIndex = static_cast<UINT>(_rgTokenList[i][1]) - 1;

            CHK(pBuffer->WriteString(pParams->GetParameter(uParamIndex)));
        }
        else
        {
            // This id token was not redefined, so just spit it out
            CHK(pBuffer->WriteString(_rgTokenList[i]));
        }

        // Space between tokens, but not after them
        if (i != _rgTokenList.GetCount() - 1)
        {
            CHK(pBuffer->WriteString(" "));
        }
    }

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetCount
//
//  Synopsis:   Return the count of tokens in the definition.
//
//------------------------------------------------------------------------------
UINT CGLSLPreMacroDefinition::GetCount() const 
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
char* CGLSLPreMacroDefinition::GetItemText(UINT index) 
{ 
    return _rgTokenList[index]; 
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetIdentifierIndex
//
//  Synopsis:   Get the index of the macro identifier in the symbol table.
//
//------------------------------------------------------------------------------
int CGLSLPreMacroDefinition::GetIdentifierIndex() const 
{ 
    return _idSymbol; 
}

//+-----------------------------------------------------------------------------
//
//  Function:   HasParameters
//
//  Synopsis:   Determines if the macro has parameters or not.
//
//------------------------------------------------------------------------------
bool CGLSLPreMacroDefinition::HasParameters() const 
{ 
    return (_spParameters != nullptr); 
}

//+-----------------------------------------------------------------------------
//
//  Function:   IsEqual
//
//  Synopsis:   Determines if the macro is the same as the given macro
//
//------------------------------------------------------------------------------
bool CGLSLPreMacroDefinition::IsEqual(__in const CGLSLPreMacroDefinition* pOther) const 
{
    // Symbol must be the same
    if (_idSymbol != pOther->_idSymbol)
    {
        return false;
    }

    // Both must be the same from the parameter POV
    if ((_spParameters == nullptr && pOther->_spParameters != nullptr) ||
        (_spParameters != nullptr && pOther->_spParameters == nullptr))
    {
        return false;
    }

    if (_spParameters != nullptr)
    {
        // Same parameter count
        if (_spParameters->GetCount() != pOther->_spParameters->GetCount())
        {
            return false;
        }

        // Same parameters
        for (UINT i = 0; i < _spParameters->GetCount(); i++)
        {
            if (_spParameters->GetParameter(i) != pOther->_spParameters->GetParameter(i))
            {
                return false;
            }
        }
    }

    // Same token count
    if (_rgTokenList.GetCount() != pOther->_rgTokenList.GetCount())
    {
        return false;
    }

    // Same tokens
    for (UINT i = 0; i < _rgTokenList.GetCount(); i++)
    {
        if (_rgTokenList[i] != pOther->_rgTokenList[i])
        {
            return false;
        }
    }

    return true;
}

