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

#include <foundation/collections.hxx>

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLLineMap
//
//  Synopsis:   Class to store line mappings (from #line directives) in a more
//              efficient way than a single big list.
//
//              Since GLSL shaders are limited in length in various ways, we
//              store the mappings in groups of 16, up until the first 256
//              lines. After that, all of the mappings go in the end.
//
//------------------------------------------------------------------------------
class CGLSLLineMap : public IUnknown
{
public:
    HRESULT AddEntry(int line, int value);
    void AdjustLogicalLine(int line, __out int* pLogicalLine);

protected:
    CGLSLLineMap();
    HRESULT Initialize() { return S_OK; }

private:
    struct MapEntry
    {
        int _line;                                                          // The real line that the entry is on
        int _value;                                                         // The new line number
    };

private:
    CModernArray<MapEntry> _rgEntries;                                      // The array of entries
    UINT _uNextEntry;                                                       // What entry we are up to
};
