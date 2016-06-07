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

#include "ParseTreeNode.hxx"

typedef CModernArray<TSmartPointer<ParseTreeNode>> CModernParseTreeNodeArray;

class FunctionDefinitionNode;

//+-----------------------------------------------------------------------------
//
//  Class:      CollectionNode
//
//  Synopsis:   Node information for a node that has multiple children.
//
//------------------------------------------------------------------------------
class CollectionNode : public ParseTreeNode
{
public:
    CollectionNode();

    ~CollectionNode();

    HRESULT InitializeAsClone(__in ParseTreeNode* pOriginal) override;

    virtual HRESULT InitializeAsCloneCollection(
        __in CollectionNode* pOriginalColl,                 // Node being cloned
        __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
        ) { Assert(false); return E_NOTIMPL; }

    HRESULT GatherDefinitionsOfCalledFunctions(__inout CModernArray<const FunctionDefinitionNode*>& aryCalled) const;

    // ParseTreeNode overrides
    bool IsCollectionNode() const override { return true; }
    HRESULT VerifyChildren() override;
    HRESULT SetHLSLNameIndex(UINT uIndex) override;
    HRESULT MarkWritten() override;

    UINT GetChildCount() const { return _aryChildren.GetCount(); }
    ParseTreeNode* GetChild(UINT index) const;
    HRESULT AppendChild(__in ParseTreeNode* pChild);
    HRESULT InsertChild(__in ParseTreeNode* pChild, UINT uIndex);
    HRESULT InsertBefore(__in ParseTreeNode* pChildToInsert, __in ParseTreeNode* pNodeInsertBefore);
    HRESULT GetChildIndex(__in ParseTreeNode* pChild, __out UINT* puIndex);
    void RemoveAllChildren() { _aryChildren.RemoveAll(); }

    void AssertSubtreeFullyVerified() const override;

    static HRESULT AppendChild(
        __in ParseTreeNode* pCollectionNode,                // The collection to apply this to
        __in ParseTreeNode* pChildNode                      // The child to add
        );

    //+-----------------------------------------------------------------------------
    //
    //  Function:   CreateAppendChild
    //
    //  Synopsis:   Called to create and append a child in one step.
    //
    //------------------------------------------------------------------------------
    template<typename T>
    static HRESULT CreateAppendChild(
        __in ParseTreeNode* pCollectionNode,
        __in CGLSLParser* pParser
        )
    {
        CHK_START;

        TSmartPointer<T> spNewNode;
        CHK(RefCounted<T>::Create(pParser, /*out*/spNewNode));
        CHK(AppendChild(pCollectionNode, spNewNode));

        CHK_RETURN;
    }

    //+-----------------------------------------------------------------------------
    //
    //  Function:   CreateAppendChild
    //
    //  Synopsis:   Called to create and append a child in one step.
    //
    //------------------------------------------------------------------------------
    template<typename T, typename A1>
    static HRESULT CreateAppendChild(
        __in ParseTreeNode* pCollectionNode,
        __in CGLSLParser* pParser, 
        const A1 &a1
        )
    {
        CHK_START;

        TSmartPointer<T> spNewNode;
        CHK(RefCounted<T>::Create(pParser, a1, /*out*/spNewNode));
        CHK(AppendChild(pCollectionNode, spNewNode));

        CHK_RETURN;
    }

    //+-----------------------------------------------------------------------------
    //
    //  Function:   CreateAppendChild
    //
    //  Synopsis:   Called to create and append a child in one step.
    //
    //------------------------------------------------------------------------------
    template<typename T, typename A1, typename A2, typename A3>
    static HRESULT CreateAppendChild(
        __in ParseTreeNode* pCollectionNode,
        __in CGLSLParser* pParser, 
        const A1 &a1, 
        const A2 &a2, 
        const A3 &a3 
        )
    {
        CHK_START;

        TSmartPointer<T> spNewNode;
        CHK(RefCounted<T>::Create(pParser, a1, a2, a3, /*out*/spNewNode));
        CHK(AppendChild(pCollectionNode, spNewNode));

        CHK_RETURN;
    }

    HRESULT DumpTree(__in IStringStream* pOutput) override;
    HRESULT ExtractChild(UINT index, __deref_out ParseTreeNode** ppChild);
    template<typename T>
    HRESULT ExtractChild(const TSmartPointer<T>& spChild);

private:
    //+----------------------------------------------------------------------------
    //
    //  Struct:     TreePosition
    //
    //  Synopsis:   The position in a tree - used to track position while doing
    //              a non-recursive walk.
    //
    //-----------------------------------------------------------------------------
    struct TreePosition
    {
        CollectionNode* _pParent;                           // Current parent in walk
        UINT _uIndex;                                       // Index of current child in walk
    };

private:
    CModernParseTreeNodeArray _aryChildren;
};

//+----------------------------------------------------------------------------
//
//  Function:   ExtractChild
//
//  Synopsis:   Called to extract a child node from this collection. This node 
//              releasese its referenced to the passed in child. Requires
//              passing in a smart pointer to ensure the Extract operation
//              does not result in a dangling pointer. The smart pointer
//              is a templated type instead of vanilla ParseTreeNode, 
//              so that you can do something like:
//                  TSmartPointer<DerivedNodeType> spDerivedNode;
//                  CHK(pCollectionNode->ExtractChild(spDerivedNode));
//
//              The parent relationship is also severed.
//
//-----------------------------------------------------------------------------
template<typename T>
HRESULT CollectionNode::ExtractChild(const TSmartPointer<T>& spChild)
{
    CHK_START;

    UINT uIndex = _aryChildren.Find(spChild.GetRawPointer());
    CHKB(uIndex != CModernParseTreeNodeArray::NotFound);

    // Remove from the collection
    CHK(_aryChildren.RemoveAt(uIndex));

    // Disconnect the parent
    spChild->SetParent(nullptr);

    CHK_RETURN;
}
