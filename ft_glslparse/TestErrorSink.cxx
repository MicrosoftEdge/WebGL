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
#include "headers.hxx"
#include "TestErrorSink.hxx"
#include "RefCounted.hxx"
#include "GLSLError.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   AddError
//
//-----------------------------------------------------------------------------
HRESULT CTestErrorSink::AddError(
    int line,                       // The location that the error occured
    int column,
    HRESULT hrCode,                 // The error code for the error
    __in_z_opt const char* pszData  // The optional data to use making a message for the error
    )
{
    CHK_START;

    TSmartPointer<CGLSLError> spNewError;
    CHK(RefCounted<CGLSLError>::Create(line, column, hrCode, pszData, /*out*/spNewError));
    CHK(_rgErrors.Add(spNewError));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetErrorCount
//
//-----------------------------------------------------------------------------
UINT CTestErrorSink::GetErrorCount() const 
{ 
    return _rgErrors.GetCount(); 
}

//+----------------------------------------------------------------------------
//
//  Function:   UseError
//
//-----------------------------------------------------------------------------
CGLSLError* CTestErrorSink::UseError(UINT index) const 
{ 
    return _rgErrors[index]; 
}
