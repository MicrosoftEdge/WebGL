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
#include "GLSLTranslate.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"
#include "IStringStream.hxx"
#include "GLSLConvertedShader.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   GLSLTranslate
//
//  Synopsis:   Global function that takes an input GLSL string as unicode and
//              outputs a converted shader.
//
//-----------------------------------------------------------------------------
HRESULT GLSLTranslate(
    __in BSTR bstrInput,                                        // Input unicode GLSL string
    GLSLShaderType::Enum shaderType,                            // Indicates what kind of shader is being translated
    UINT uOptions,                                              // Translation options
    WebGLFeatureLevel glFeatureLevel,                           // Feature level we're translating for
    __deref_out CGLSLConvertedShader** ppConvertedShader        // Converted shader
    )
{
    CHK_START;

    CGLSLParser parser;
    
    CHK(parser.Initialize(bstrInput, shaderType, uOptions, glFeatureLevel));

    TSmartPointer<CGLSLConvertedShader> spConvertedShader;
    CHK(parser.Translate(&spConvertedShader)); 

    *ppConvertedShader = spConvertedShader.Extract();

    CHK_RETURN;
}
