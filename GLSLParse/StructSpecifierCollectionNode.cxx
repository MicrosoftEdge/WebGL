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
#include "StructSpecifierCollectionNode.hxx"
#include "IStringStream.hxx"
#include "StructSpecifierNode.hxx"

MtDefine(StructSpecifierCollectionNode, CGLSLParser, "StructSpecifierCollectionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructSpecifierCollectionNode::StructSpecifierCollectionNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierCollectionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    for (UINT i = 0; i < GetChildCount(); i++)
    {
        // The struct specifier children should be output with a semicolon
        // in between them.
        StructSpecifierNode* pStructSpecifierNode = GetChild(i)->GetAs<StructSpecifierNode>();
        CHK(pStructSpecifierNode->OutputHLSL(pOutput));
        CHK(pOutput->WriteString(";\n"));

        // Now that we've output the type declaration in HLSL, we can ask the 
        // struct specifier node to output any functions that may be needed for
        // the defined type (e.g. constructors or equality functions).
        CHK(pStructSpecifierNode->OutputHLSLFunctions(pOutput));
    }

    for (UINT i = 0; i < _aryImplementationInfo.GetCount(); i++)
    {
        CHK(_aryImplementationInfo[i]->OutputHLSLConstructor(pOutput));
        CHK(_aryImplementationInfo[i]->OutputHLSLEqualsFunction(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddImplementationTypeInfo
//
//  Synopsis:   Add type info for types not made by GLSL structs (basically
//              implementation details).
//
//-----------------------------------------------------------------------------
HRESULT StructSpecifierCollectionNode::AddImplementationTypeInfo(__in CTypeNameIdentifierInfo* pTypeNameInfo)
{
    return _aryImplementationInfo.Add(pTypeNameInfo);
}
