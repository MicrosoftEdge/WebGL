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

#include "CollectionNode.hxx"
#include "IOStructType.hxx"

class CVariableIdentifierInfo;
class CGLSLSymbolTable;

//+-----------------------------------------------------------------------------
//
//  Class:      IOStructNode
//
//  Synopsis:   This node is inserted as the at the root level, and declarations
//              that are transformed into input / output structures are moved
//              here.
//
//------------------------------------------------------------------------------
class IOStructNode : public CollectionNode
{
public:
    IOStructNode();

    HRESULT Initialize(
        __in CGLSLParser* pParser,                          // The parser that owns the tree
        IOStructType::Enum type                             // What type of IO the struct represents
        );

    HRESULT Initialize(
        __in CGLSLParser* pParser                           // The parser that owns the tree
        ) { ParseTreeNode::Initialize(pParser); return S_OK; }

    // ParseTreeNode overrides
    ParseNodeType::Enum GetParseNodeType() const override { return GetClassNodeType(); }
    HRESULT OutputHLSL(__in IStringStream* pOutput) override;

    // For GetAs et al
    static ParseNodeType::Enum GetClassNodeType() { return ParseNodeType::ioStruct; }

    IOStructType::Enum GetStructType() const { return _structType; }

private:
    static int __cdecl CompareIdentifiers(
        __in CGLSLSymbolTable* pTable,                      // Symbol table to look up names in
        const TSmartPointer<CVariableIdentifierInfo>* pA,   // First identifier to compare
        const TSmartPointer<CVariableIdentifierInfo>* pB    // Second identifier to compare
        );

    HRESULT CreateIdentifierNames();

private:
    IOStructType::Enum _structType;                         // What type of IO the struct represents
};
