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
#include "FunctionIdentifierNode.hxx"
#include "FunctionIdentifierInfo.hxx"
#include "IStringStream.hxx"
#include "GLSL.tab.h"

MtDefine(FunctionIdentifierNode, CGLSLParser, "FunctionIdentifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionIdentifierNode::FunctionIdentifierNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Clone the identifier
//
//-----------------------------------------------------------------------------
HRESULT FunctionIdentifierNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    CHK_START;

    // Let base class to its work first
    CHK(IdentifierNodeBase::InitializeAsClone(pOriginal));

    // Do specific stuff for this class
    FunctionIdentifierNode* pOriginalId = pOriginal->GetAs<FunctionIdentifierNode>();
    _spInfo = pOriginalId->_spInfo;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionIdentifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    TSmartPointer<CFunctionIdentifierInfo> spInfo;
    CHK(GetFunctionIdentifierInfo(&spInfo));
    CHKB(spInfo->GetHLSLName(0) != nullptr);
    CHK(pOutput->WriteString(spInfo->GetHLSLName(0)));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFunctionIdentifierInfo
//
//  Synopsis:   Get the identifier info for the function identifier that this 
//              node represents. This function retrieves the cached value that
//              has been previously set.
//
//-----------------------------------------------------------------------------
HRESULT FunctionIdentifierNode::GetFunctionIdentifierInfo(
    __deref_out CFunctionIdentifierInfo** ppInfo        // The info about the identifier
    ) const
{
    CHK_START;

    CHKB(_spInfo != nullptr);
    _spInfo.CopyTo(ppInfo);

    CHK_RETURN;
}
//+----------------------------------------------------------------------------
//
//  Function:   SetFunctionIdentifierInfo
//
//  Synopsis:   Set the identifier info for the identifier that this node
//              represents.
//
//-----------------------------------------------------------------------------
void FunctionIdentifierNode::SetFunctionIdentifierInfo(__in CFunctionIdentifierInfo* pInfo)
{
    _spInfo = pInfo;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionIdentifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FunctionIdentifierNode Symbol=%d", GetSymbolIndex());
}
