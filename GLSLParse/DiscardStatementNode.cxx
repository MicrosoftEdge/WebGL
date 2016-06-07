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
#include "DiscardStatementNode.hxx"
#include "FunctionDefinitionNode.hxx"
#include "IStringStream.hxx"
#include "GLSLType.hxx"
#include "RefCounted.hxx"
#include "GLSLParser.hxx"

MtDefine(DiscardStatementNode, CGLSLParser, "DiscardStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
DiscardStatementNode::DiscardStatementNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT DiscardStatementNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    const YYLTYPE &location                     // The location of the statement
    )
{
    ParseTreeNode::Initialize(pParser);

    _location = location;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT DiscardStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    return pOutput->WriteString("discard;\n");
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   'discard' statements have to be in a fragment shader.
//
//-----------------------------------------------------------------------------
HRESULT DiscardStatementNode::VerifySelf()
{
    CHK_START;

    if (GetParser()->GetShaderType() != GLSLShaderType::Fragment)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDDISCARDLOCATION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);             
    }

    CHK_RETURN;
}
