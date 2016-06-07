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
#include "GLSLPreParamList.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLPreParamList::CGLSLPreParamList()
{
}

//+-----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Parameter lists always have at least one parameter.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreParamList::Initialize()
{
    return AddParameter();
}

//+-----------------------------------------------------------------------------
//
//  Function:   AddParameter
//
//  Synopsis:   Called to add a parameter to the list.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreParamList::AddParameter() 
{ 
    return _rgParameters.Add(""); 
}

//+-----------------------------------------------------------------------------
//
//  Function:   AddToken
//
//  Synopsis:   Append the given token to the end of the last parameter.
//
//------------------------------------------------------------------------------
HRESULT CGLSLPreParamList::AddToken(const char* pszToken)
{
    CHK_START;

    CHKB(_rgParameters.GetCount() != 0);
    CHK(_rgParameters[_rgParameters.GetCount() - 1].Append(pszToken));

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetCount
//
//  Synopsis:   Return the count of tokens in the definition.
//
//------------------------------------------------------------------------------
UINT CGLSLPreParamList::GetCount() const 
{ 
    return _rgParameters.GetCount(); 
}

//+-----------------------------------------------------------------------------
//
//  Function:   IsEqual
//
//  Synopsis:   Determines if the list is the same as the given list
//
//------------------------------------------------------------------------------
bool CGLSLPreParamList::IsEqual(__in const CGLSLPreParamList* pOther) const 
{
    // Same parameter count
    if (_rgParameters.GetCount() != pOther->_rgParameters.GetCount())
    {
        return false;
    }

    // Same parameters
    for (UINT i = 0; i < _rgParameters.GetCount(); i++)
    {
        if (_rgParameters[i] != pOther->_rgParameters[i])
        {
            return false;
        }
    }

    return true;
}

