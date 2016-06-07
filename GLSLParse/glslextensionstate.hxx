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
#include "GLSLExtension.hxx"
#include "GLSLExtensionBehavior.hxx"

interface IErrorSink;
struct YYLTYPE;

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLExtensionState
//
//  Synopsis:   Class to store extension setting state.
//
//------------------------------------------------------------------------------
class CGLSLExtensionState : public IUnknown
{
public:
    HRESULT AddEntry(GLSLExtension ext, bool fSupported, GLSLExtensionBehavior behav);

    HRESULT AdjustState(
        __in_z const char* pszName,                                         // Name of extension
        __in_z const char* pszBehavior,                                     // String of behavior
        const YYLTYPE& location,                                            // Location of the define statement
        __in IErrorSink* pSink                                              // Error sink
    );

    bool IsExtensionEnabled(GLSLExtension ext) const;

protected:
    HRESULT Initialize() { return S_OK; }

private:
    HRESULT GetIndex(GLSLExtension ext, __out UINT* puIndex) const;

    static HRESULT ExtensionFromName(
        __in_z const char* pszName,                                         // Name of extension
        const YYLTYPE& location,                                            // Location of the define statement
        __in IErrorSink* pSink,                                             // Error sink
        __out GLSLExtension* pExtension                                     // Extension output
        );

    static HRESULT ExtensionBehaviorFromString(
        __in_z const char* pszBehavior,                                     // String of behavior
        const YYLTYPE& location,                                            // Location of the define statement
        __in IErrorSink* pSink,                                             // Error sink
        __out GLSLExtensionBehavior* pExtensionBehavior                     // Extension behavior output
        );

private:
    struct ExtensionStateEntry
    {
        GLSLExtension _extension;                                           // The extension that we are setting state for
        bool _fSupported;                                                   // Whether the extension is supported or not
        GLSLExtensionBehavior _behavior;                                    // The behavior of that extension
    };

private:
    CModernArray<ExtensionStateEntry> _aryExtensions;                       // Extension state
};
