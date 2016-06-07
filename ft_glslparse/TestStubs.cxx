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

bool IEConfiguration_GetBool(IEConfigurationID iecID)
{
    return false;
}

namespace SettingStore
{
    static const GUID GUID_TestIESettings =
    { 0xa371e3d8, 0xe965, 0x4961, { 0xa1, 0x57, 0x4c, 0x45, 0x0, 0xbc, 0x34, 0x40 } };

    extern const struct VALUEID<DWORD> IEVALUE_Trident_MixedContentBlockImages = { &GUID_TestIESettings };
    IESSAPI GetDWORD(VALUEID<DWORD> Id, DWORD* pdwValue) { return E_NOTIMPL; }

    extern const struct VALUEID<BOOL> IEVALUE_ExperimentalFeatures_EnableVP9 = { &GUID_TestIESettings };
    IESSAPI GetBOOL(VALUEID<BOOL> Id, BOOL* pbValue) { return E_NOTIMPL; }
}
