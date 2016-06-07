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
#include "GLSLLineMap.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLLineMap::CGLSLLineMap() : 
    _uNextEntry(0)
{
}


//+-----------------------------------------------------------------------------
//
//  Function:   AddEntry
//
//  Synopsis:   Called when a #line directive is encountered to add a new
//              mapping.
//
//------------------------------------------------------------------------------
HRESULT CGLSLLineMap::AddEntry(int line, int value)
{
    MapEntry newEntry = { line, value };

    return _rgEntries.Add(newEntry);
}

//+-----------------------------------------------------------------------------
//
//  Function:   AdjustLogicalLine
//
//  Synopsis:   From a given line, adjust the logical line if need be.
//
//------------------------------------------------------------------------------
void CGLSLLineMap::AdjustLogicalLine(int line, __out int* pLogicalLine)
{ 
    if (_uNextEntry >= _rgEntries.GetCount())
    {
        // Nothing to do if we have no entries left
        return;
    }

    // See if the line we are given maps to the next entry we have
    if (_rgEntries[_uNextEntry]._line == line)
    {
        // Do the mapping
        (*pLogicalLine) = _rgEntries[_uNextEntry]._value;

        // Move onto looking for the next entry
        _uNextEntry++;
    }
}
