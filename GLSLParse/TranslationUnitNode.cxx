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
#include "TranslationUnitNode.hxx"
#include "IStringStream.hxx"
#include "SamplerCollectionNode.hxx"
#include "IOStructNode.hxx"
#include "GLSLParser.hxx"
#include "TextNode.hxx"
#include "StructSpecifierCollectionNode.hxx"
#include "FunctionDefinitionNode.hxx"
#include "CompoundStatementNode.hxx"

MtDefine(TranslationUnitCollectionNode, CGLSLParser, "TranslationUnitCollectionNode");

// IOStruct children start at index 1, and there are 2 of them.
const UINT TranslationUnitCollectionNode::s_uIOStructChildStartIndex = 1;
const UINT TranslationUnitCollectionNode::s_cIOStructChildren = 2;

// Struct specifier collection node is the child right after the IOStructNode children and the text node
const UINT TranslationUnitCollectionNode::s_uStructSpecifierChildIndex = TranslationUnitCollectionNode::s_uIOStructChildStartIndex + TranslationUnitCollectionNode::s_cIOStructChildren + 1;
//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
TranslationUnitCollectionNode::TranslationUnitCollectionNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT TranslationUnitCollectionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pExtDeclNode            // The first translation unit
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    // The first child will be the sampler collection
    CHK(CollectionNode::CreateAppendChild<SamplerCollectionNode>(this, GetParser()));

    // Then the IO structs
    if (GetParser()->GetShaderType() == GLSLShaderType::Vertex)
    {
        CHK(CollectionNode::CreateAppendChild<IOStructNode>(this, GetParser(), IOStructType::Attribute));
        CHK(CollectionNode::CreateAppendChild<IOStructNode>(this, GetParser(), IOStructType::Varying));
    }
    else
    {
        Assert(GetParser()->GetShaderType() == GLSLShaderType::Fragment);
        
        CHK(CollectionNode::CreateAppendChild<IOStructNode>(this, GetParser(), IOStructType::Output));
        CHK(CollectionNode::CreateAppendChild<IOStructNode>(this, GetParser(), IOStructType::Varying));
    }

    // And then a text node from the parser
    CHK(CollectionNode::CreateAppendChild<TextNode>(this, GetParser(), static_cast<ITextNodeProvider*>(GetParser())));

    // And then the struct specifiers
    CHK(CollectionNode::CreateAppendChild<StructSpecifierCollectionNode>(this, GetParser()));

    // Now add the first node from the parser
    CHK(AppendChild(pExtDeclNode));
    
    // The root scope has the zero scope id
    SetScopeId(0);

    CHK_RETURN;   
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verifies that all of the functions declared in the shader
//              have definitions.
//
//-----------------------------------------------------------------------------
HRESULT TranslationUnitCollectionNode::VerifySelf()
{
    CHK_START;

    const FunctionDefinitionNode* pEntryPointDefinition = nullptr;

    for (UINT i = 0; i < GetIdentifierCount(); i++)
    {
        CFunctionIdentifierInfo* pFuncInfo = UseIdentifier(i)->AsFunction();
        if (pFuncInfo != nullptr)
        {
            const bool fFunctionDefined = pFuncInfo->IsDefined();
            if (!fFunctionDefined && pFuncInfo->IsCalled())
            {
                // The function is called somewhere, but it is not defined
                CHK(GetParser()->LogError(
                    nullptr, 
                    E_GLSLERROR_UNDEFINEDFUNCTION, 
                    GetParser()->UseSymbolTable()->NameFromIndex(pFuncInfo->GetSymbolIndex())
                    ));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            // When we encounter 'main', grab a pointer to the function definition for
            // function call depth verification below.
            if (fFunctionDefined && pFuncInfo->IsGLSLSymbol(GLSLSymbols::main))
            {
                pEntryPointDefinition = pFuncInfo->UseFunctionDefinition();
            }
        }
    }

    if (pEntryPointDefinition != nullptr)
    {
        // The D3D compiler basically inlines all function calls, but performs this in
        // a recursive manner. This can lead to crashes via stack overflows. In order to
        // prevent this, part of verfication/translation is verifing that the function
        // call depth for this shader is reasonable.
        CHK(VerifyFunctionCallDepth(pEntryPointDefinition, GetParser()));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Converts the translation unit by doing all of the children
//              in turn.
//
//-----------------------------------------------------------------------------
HRESULT TranslationUnitCollectionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    for (UINT i = 0; i < GetChildCount(); i++)
    {
        // Convert each external declaration in turn
        CHK(GetChild(i)->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetSamplerCollection
//
//  Synopsis:   Return the first child with the appropriate type.
//
//-----------------------------------------------------------------------------
SamplerCollectionNode* TranslationUnitCollectionNode::GetSamplerCollection()
{
    return GetChild(0)->GetAs<SamplerCollectionNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIOStruct
//
//  Synopsis:   Return the first child with the appropriate type.
//
//-----------------------------------------------------------------------------
IOStructNode* TranslationUnitCollectionNode::GetIOStruct(IOStructType::Enum type)
{
    for (UINT i = s_uIOStructChildStartIndex; i < s_uIOStructChildStartIndex + s_cIOStructChildren; i++)
    {
        IOStructNode* pStructNode = GetChild(i)->GetAs<IOStructNode>();
        if (pStructNode->GetStructType() == type)
        {
            return pStructNode;
        }
    }

    return nullptr;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetStructSpecifierCollection
//
//  Synopsis:   Return the StructSpecifierCollectionNode child (retrieved from
//              a known index).
//
//-----------------------------------------------------------------------------
StructSpecifierCollectionNode* TranslationUnitCollectionNode::GetStructSpecifierCollection()
{
    return GetChild(s_uStructSpecifierChildIndex)->GetAs<StructSpecifierCollectionNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyFunctionCallDepth
//
//  Synopsis:   Verifies that the function call graph does not exceed a 
//              predetermined limit; The HLSL compiler walks recursively
//              into each function call which can lead to stack overflows so
//              we have to protect ourselves.
//
//+----------------------------------------------------------------------------
HRESULT TranslationUnitCollectionNode::VerifyFunctionCallDepth(__in const FunctionDefinitionNode* pEntryPointDefinition, __in CGLSLParser* pParser)
{
    CHK_START;

    CModernArray<const FunctionDefinitionNode*> aryCallers;
    CHK(aryCallers.Add(pEntryPointDefinition));

    UINT cNestingLevel = 0;
    const UINT cMaxNestingLevel = ParseTreeNode::GetMaxFunctionNestingLevel();

    CModernArray<const FunctionDefinitionNode*> aryCalled;
    while (aryCallers.GetCount() != 0 && cNestingLevel <= cMaxNestingLevel)
    {
        // For each of the current callers, find each function that is called
        for (UINT i = 0; i < aryCallers.GetCount(); i++)
        {
            ParseTreeNode* pFunctionBody = aryCallers[i]->GetFunctionBody();
            Assert(pFunctionBody->GetParseNodeType() == ParseNodeType::compoundStatement);
            CHK(pFunctionBody->GetAs<CompoundStatementNode>()->GatherDefinitionsOfCalledFunctions(aryCalled));
        }

        // Transfer the called functions to the caller collection,
        // clear the called functions to start with an empty collection
        // for the next iteration, and go deeper in the next iteration...
        aryCallers.RemoveAllAndMaintainCapacity();
        CHK(aryCallers.AddArray(aryCalled));
        aryCalled.RemoveAllAndMaintainCapacity();

        // ... keeping record of the increased nesting level.
        cNestingLevel++;
    }

    if (cNestingLevel > cMaxNestingLevel)
    {
        CHK(pParser->LogError(nullptr, E_GLSLERROR_MAXFUNCTIONDEPTHEXCEEDED, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

