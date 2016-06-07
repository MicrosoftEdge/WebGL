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
#include "FunctionPrototypeDeclarationNode.hxx"
#include "IStringStream.hxx"
#include "FunctionPrototypeNode.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "GLSLParser.hxx"

MtDefine(FunctionPrototypeDeclarationNode, CGLSLParser, "FunctionPrototypeDeclarationNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionPrototypeDeclarationNode::FunctionPrototypeDeclarationNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeDeclarationNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pProto,                 // The function prototype
    const YYLTYPE &location                     // The location of the declarator
    )
{
    ParseTreeNode::Initialize(pParser);

    Assert(pProto->GetParseNodeType() == ParseNodeType::functionPrototype);

    _location = location;

    return AppendChild(pProto);
}
    
//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Mark the function as having a forward declaration, or flag
//              an error if it already had one.
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeDeclarationNode::VerifySelf()
{
    CHK_START;

    FunctionPrototypeNode* pFuncProto = GetChild(0)->GetAs<FunctionPrototypeNode>();
    FunctionHeaderWithParametersNode* pHeaderWithParam = pFuncProto->GetChild(0)->GetAs<FunctionHeaderWithParametersNode>();
    
    // We need an identifier info to continue
    Assert(pHeaderWithParam->UseIdentifierInfo() != nullptr);

    if (GetParent()->GetParseNodeType() != ParseNodeType::translationUnit)
    {
        // Function declarations are only allowed inside translation_unit (i.e.
        // they must be global). Function definitions have this enforced by
        // the grammar, but we must do a dynamic verification check.
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_LOCALFUNCTIONDECLARATION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    if (pHeaderWithParam->UseIdentifierInfo()->IsDeclared())
    {
        // This function has already been declared, and redeclaring is not allowed
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_FUNCTIONREDECLARATION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Mark as declared
    pHeaderWithParam->UseIdentifierInfo()->SetDeclared();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeDeclarationNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Output the translation of the prototype
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // Terminate with a semicolon
    CHK(pOutput->WriteString(";\n"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionPrototypeDeclarationNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteString("FunctionPrototypeDeclarationNode");
}
