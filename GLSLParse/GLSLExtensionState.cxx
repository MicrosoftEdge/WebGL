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
#include "GLSLExtensionState.hxx"
#include "IErrorSink.hxx"
#include "GLSLError.hxx"
#include "pre.tab.h"

//+-----------------------------------------------------------------------------
//
//  Function:   AddEntry
//
//  Synopsis:   Called to add an extension entry
//
//------------------------------------------------------------------------------
HRESULT CGLSLExtensionState::AddEntry(GLSLExtension ext, bool fSupported, GLSLExtensionBehavior behav)
{
    ExtensionStateEntry newEntry = { ext, fSupported, behav };

    return _aryExtensions.Add(newEntry);
}

//+-----------------------------------------------------------------------------
//
//  Function:   IsExtensionEnabled
//
//  Synopsis:   See if the given extension is both supported and enabled.
//
//              This function handles the logic of 'all' overriding everything.
//
//------------------------------------------------------------------------------
bool CGLSLExtensionState::IsExtensionEnabled(GLSLExtension ext) const
{
    bool fEnabled = false;

    // Cannot be enabled unless we find it
    UINT uIndex;
    if (SUCCEEDED(GetIndex(ext, &uIndex)))
    {
        GLSLExtensionBehavior behav;
        if (_aryExtensions[uIndex]._behavior != GLSLExtensionBehavior::notSet)
        {
            behav = _aryExtensions[uIndex]._behavior;
        }
        else
        {
            // Return the 'all' behavior
            behav = _aryExtensions[0]._behavior;
        }

        // all should never be notSet, and we should use all if the specific
        // extension is not set.
        Assert(behav != GLSLExtensionBehavior::notSet);

        // Anything other than disable means that #extension has turned this extension
        // to a state where it is exposing the behavior. That and the extension
        // being supported means we enable the behavior.
        if (behav != GLSLExtensionBehavior::disable && _aryExtensions[uIndex]._fSupported)
        {
            fEnabled = true;
        }        
    }

    return fEnabled;
}

//+-----------------------------------------------------------------------------
//
//  Function:   AdjustState
//
//  Synopsis:   Respond to an extension directive.
//
//              An extension name and behavior is given. The following behaviors
//              are understood, with the caveat that warnings are not yet 
//              emitted by the preprocessor.
//
//              * require - Behave as specified by the extension pszName.
//
//              Give an error on the #extension if the extension pszName is not
//              supported, or if all as the extension is specified.
//
//              * enable - Behave as specified by the extension pszName.
//
//              Warn on the #extension if the extension pszName is not supported.
//
//              Give an error on the #extension if all is specified.
//
//              * warn - Behave as specified by the extension pszName, except 
//              issue warnings on any detectable use of that extension, unless 
//              such use is supported by other enabled or required extensions.
//
//              If all is specified, then warn on all detectable uses of any 
//              extension used.
//
//              Warn on the #extension if the extension pszName is not supported.
//
//              * disable - Behave (including issuing errors and warnings) as if 
//              the extension extension_name is not part of the language 
//              definition.
//
//              If all is specified, then behavior must revert back to that of 
//              the non-extended core version of the language being compiled to.
//
//              Warn on the #extension if the extension pszName is not 
//              supported.
//
//------------------------------------------------------------------------------
HRESULT CGLSLExtensionState::AdjustState(
    __in_z const char* pszName,                                         // Name of extension
    __in_z const char* pszBehavior,                                     // String of behavior
    const YYLTYPE& location,                                            // Location of the define statement
    __in IErrorSink* pSink                                              // Error sink
    )
{ 
    CHK_START;

    // No matter what, you need an extension behavior that we understand
    GLSLExtensionBehavior behav = GLSLExtensionBehavior::notSet;
    CHK(ExtensionBehaviorFromString(pszBehavior, location, pSink, &behav));

    GLSLExtension ext;
    if (FAILED(ExtensionFromName(pszName, location, pSink, &ext)))
    {
        // We can only flag an error if this is required. Otherwise we can only
        // warn, and warning is not supported yet.
        if (behav == GLSLExtensionBehavior::require)
        {
            // Flag an error if the extension is not something we support
            CHK(pSink->AddError(location.first_line, location.first_column, E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED, pszName));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }
    else
    {
        // 'all' is a special extension name that applies rules to all extensions
        if (ext == GLSLExtension::all)
        {
            // Only warn or disable for all
            if (behav != GLSLExtensionBehavior::warn && behav != GLSLExtensionBehavior::disable)
            {
                CHK(pSink->AddError(location.first_line, location.first_column, E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            // Setting 'all' to a mode has the effect of setting all extensions to that mode,
            // with is either enable or warn. If 'all' is set, then unset all others so that
            // the mode for 'all' is returned for everything.
            _aryExtensions[0]._behavior = behav;
            for (UINT i = 1 ; i < _aryExtensions.GetCount(); i++)
            {
                _aryExtensions[i]._behavior = GLSLExtensionBehavior::notSet;
            }
        }
        else
        {
            UINT uIndex;
            if (FAILED(GetIndex(ext, &uIndex)))
            {
                // All extensions that we know about should exist in the array
                CHK(pSink->AddError(location.first_line, location.first_column, E_GLSLERROR_UNKNOWNEXTENSION, pszName));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            if (behav == GLSLExtensionBehavior::require && !_aryExtensions[uIndex]._fSupported)
            {
                // Flag an error if the extension is not something we support
                CHK(pSink->AddError(location.first_line, location.first_column, E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED, pszName));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            // Set the behavior for that extension
            _aryExtensions[uIndex]._behavior = behav;
        }
    }

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   GetIndex
//
//  Synopsis:   Map extension enum to index in internal array.
//
//------------------------------------------------------------------------------
HRESULT CGLSLExtensionState::GetIndex(GLSLExtension ext, __out UINT* puIndex) const
{
    for (UINT i = 1; i < _aryExtensions.GetCount(); i++)
    {
        if (_aryExtensions[i]._extension == ext)
        {
            (*puIndex) = i;
            return S_OK;
        }
    }

    return E_INVALIDARG;
}

//+----------------------------------------------------------------------------
//
//  Function:   ExtensionFromName
//
//  Synopsis:   Convert a symbol index to an extension enum.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLExtensionState::ExtensionFromName(
    __in_z const char* pszName,                                         // Name of extension
    const YYLTYPE& location,                                            // Location of the define statement
    __in IErrorSink* pSink,                                             // Error sink
    __out GLSLExtension* pExtension                                     // Extension output
    )
{
    CHK_START;

    if (::strcmp(pszName, "all") == 0)
    {
        (*pExtension) = GLSLExtension::all;
    }
    else if (::strcmp(pszName, "GL_OES_standard_derivatives") == 0)
    {
        (*pExtension) = GLSLExtension::GL_OES_standard_derivatives;
    }
    else if (::strcmp(pszName, "GL_EXT_frag_depth") == 0)
    {
        (*pExtension) = GLSLExtension::GL_EXT_frag_depth;
    }
    else
    {
        CHK(E_INVALIDARG);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ExtensionBehaviorFromString
//
//  Synopsis:   Convert a symbol index to an extension behavior enum.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLExtensionState::ExtensionBehaviorFromString(
    __in_z const char* pszBehavior,                                     // String of behavior
    const YYLTYPE& location,                                            // Location of the define statement
    __in IErrorSink* pSink,                                             // Error sink
    __out GLSLExtensionBehavior* pExtensionBehavior                     // Extension behavior output
    )
{
    CHK_START;

    // Figure out the behavior
    if (::strcmp(pszBehavior, "require") == 0)
    {
        (*pExtensionBehavior) = GLSLExtensionBehavior::require;
    }
    else if (::strcmp(pszBehavior, "enable") == 0)
    {
        (*pExtensionBehavior) = GLSLExtensionBehavior::enable;
    }
    else if (::strcmp(pszBehavior, "warn") == 0)
    {
        (*pExtensionBehavior) = GLSLExtensionBehavior::warn;
    }
    else if (::strcmp(pszBehavior, "disable") == 0)
    {
        (*pExtensionBehavior) = GLSLExtensionBehavior::disable;
    }
    else
    {
        CHK(pSink->AddError(location.first_line, location.first_column, E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

