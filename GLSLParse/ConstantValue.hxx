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
#pragma once

#include "ConstantValue.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      ConstantValue
//
//  Synopsis:   Class to store a constant expression value. This is stored
//              as the initialized value of an identifier for those that have
//              an initializer, or passed around when a constant is required.
//
//              Currently only int and double values are supported.
//
//              This stores both a basic type and (optionally) a value. It is
//              possible to have a type without a value.
//
//------------------------------------------------------------------------------
class ConstantValue
{
public:
    ConstantValue();

    void SetTypeOnly(int type);
    HRESULT GetBasicType(__out int* pBasicType) const;

    template<typename T> 
    void SetValue(T val) { AssertSz(false, "Invalid template of ConstantValue::SetValue used"); }

    template<>
    void SetValue<int>(int val);

    template<>
    void SetValue<double>(double val);

    template<typename T>
    HRESULT GetValue(__out T* pValue) const { return E_FAIL; }

    template<>
    HRESULT GetValue<int>(__out int* pValue) const;

    template<>
    HRESULT GetValue<double>(__out double* pValue) const;

    // Conversions
    HRESULT AsInt(__out int* pValue) const;
    HRESULT AsDouble(__out double* pValue) const;

private:
    int _type;                                                      // The type stored in the struct
    bool _fSet;                                                     // Whether a value is set or not

    union
    {
        int _intValue;                                              // The value if this is an int
        double _doubleValue;                                        // The value if this is a double
    };
};
