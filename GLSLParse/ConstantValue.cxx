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
#include "ConstantValue.hxx"
#include "GLSL.tab.h"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ConstantValue::ConstantValue() 
{ 
    _type = NO_TYPE; 
    _fSet = false;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetTypeOnly
//
//  Synopsis:   Set the type without a value
//
//-----------------------------------------------------------------------------
void ConstantValue::SetTypeOnly(int basicType)
{
    _type = basicType;
    _fSet = false;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetBasicType
//
//  Synopsis:   Get the type stored in this value
//
//-----------------------------------------------------------------------------
HRESULT ConstantValue::GetBasicType(__out int* pType) const
{
    if (_type == NO_TYPE)
    {
        return E_FAIL;
    }

    (*pType) = _type;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   AsInt
//
//  Synopsis:   Get as an integer
//
//-----------------------------------------------------------------------------
HRESULT ConstantValue::AsInt(__out int* pValue) const
{
    if (!_fSet)
    {
        // Cannot cast something that has no value...
        return E_FAIL;
    }

    switch (_type)
    {
    case INT_TOK:
        (*pValue) = _intValue;
        return S_OK;

    case FLOAT_TOK:
        (*pValue) = static_cast<int>(_doubleValue);
        return S_OK;

    default:
        return E_FAIL;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   AsDouble
//
//  Synopsis:   Get as a double
//
//-----------------------------------------------------------------------------
HRESULT ConstantValue::AsDouble(__out double* pValue) const
{
    if (!_fSet)
    {
        // Cannot cast something that has no value...
        return E_FAIL;
    }

    switch (_type)
    {
    case INT_TOK:
        (*pValue) = static_cast<double>(_intValue);
        return S_OK;

    case FLOAT_TOK:
        (*pValue) = _doubleValue;
        return S_OK;

    default:
        return E_FAIL;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   SetValue
//
//  Synopsis:   Helper to set int value and type.
//
//-----------------------------------------------------------------------------
template<>
void ConstantValue::SetValue<int>(int val) 
{ 
    _type = INT_TOK; 
    _intValue = val; 
    _fSet = true;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetValue
//
//  Synopsis:   Helper to set double value and type.
//
//-----------------------------------------------------------------------------
template<>
void ConstantValue::SetValue<double>(double val)
{
    _type = FLOAT_TOK;
    _doubleValue = val; 
    _fSet = true;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetValue
//
//  Synopsis:   Get the value if it is an int
//
//-----------------------------------------------------------------------------
template<>
HRESULT ConstantValue::GetValue<int>(__out int* pValue) const
{
    AssertSz(_type == INT_TOK, "Invalid call to GetValue<int>");

    if (!_fSet || _type != INT_TOK)
    {
        return E_FAIL;
    }

    (*pValue) = _intValue;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetValue
//
//  Synopsis:   Get the value if it is a double
//
//-----------------------------------------------------------------------------
template<>
HRESULT ConstantValue::GetValue<double>(__out double* pValue) const
{
    AssertSz(_type == FLOAT_TOK, "Invalid call to GetValue<double>");

    if (!_fSet || _type != FLOAT_TOK)
    {
        return E_FAIL;
    }

    (*pValue) = _doubleValue;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   ConstantValueLink
//
//  Synopsis:   Dummy function to force compilation of template functions.
//
//-----------------------------------------------------------------------------
void ConstantValueLink()
{
    ConstantValue value;
    value.SetValue(0);
    value.SetValue(0.0);

    int x;
    value.GetValue(&x);

    double y;
    value.GetValue(&y);
}
