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
#include "CollectionNode.hxx"
#include "SimpleStack.hxx"
#include "FunctionDefinitionNode.hxx"
#include "FunctionIdentifierNode.hxx"
#include "FunctionIdentifierInfo.hxx"

MtDefine(CollectionNode, CGLSLParser, "CollectionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CollectionNode::CollectionNode()
{        
}

//+----------------------------------------------------------------------------
//
//  Function:   Destructor
//
//  Synopsis:   We do a non-recursive walk through the parse tree to release
//              it, so that we cannot overflow the stack with some kind of
//              deep tree.
//
//-----------------------------------------------------------------------------
CollectionNode::~CollectionNode()
{
    CHK_START;

    // By this point, we should either be a root node ourselves (no parent) or
    // the parent we have should have cleared our children out.
    Assert(GetParent() == nullptr || _aryChildren.GetCount() == 0);

    TreePosition pos;
    pos._pParent = this;
    pos._uIndex = 0;

    CSimpleStack<TreePosition> parentStack;

    for (;;)
    {
        // Check if we are done with the current parent
        if (pos._uIndex == pos._pParent->GetChildCount())
        {
            // We are done walking all of the children of a node, so we will
            // remove them now before continuing onto siblings
            pos._pParent->RemoveAllChildren();

            // See if we are completely done
            if (parentStack.IsEmpty())
            {
                break;
            }

            // Pop off the old parent so we can keep walking from there
            CHK(parentStack.Pop(/*out*/pos));

            continue;
        }

        // Pull out the next child and move along the children
        ParseTreeNode* pCurrentChild = pos._pParent->GetChild(pos._uIndex);
        pos._uIndex++;

        // Check if we need to walk down into the current item
        if (pCurrentChild != nullptr && pCurrentChild->IsCollectionNode())
        {
            // Get the collection pointer for the next one down
            CollectionNode* pNewParent = pCurrentChild->AsCollection();

            // Remember where we are
            CHK(parentStack.Push(pos));

            // Set up the new position information
            pos._pParent = pNewParent;
            pos._uIndex = 0;

            // Start from that point
            continue;
        }
    }

    CHK_END;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Collections need to clone their children, so this does that
//              work and then calls a clone Initialize function for 
//              collections.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    CHK_START;

    Assert(pOriginal->IsCollectionNode());
    CollectionNode* pOriginalColl = static_cast<CollectionNode*>(pOriginal);

    // We use smart memory so that any failure case will clean up properly
    CModernParseTreeNodeArray aryChildClones;

    // Go through the original children and clone each of them
    for (UINT i = 0; i < pOriginalColl->_aryChildren.GetCount(); i++)
    {
        TSmartPointer<ParseTreeNode> spCloneChild;
        if (pOriginalColl->_aryChildren[i] != nullptr)
        {
            // Make a clone of the child
            CHK(pOriginalColl->_aryChildren[i]->Clone(&spCloneChild));
        }

        // Add it to our collection (null is OK)
        CHK(aryChildClones.Add(spCloneChild));
    }

    CHK(InitializeAsCloneCollection(pOriginalColl, aryChildClones));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AppendChild
//
//  Synopsis:   Static helper for Bison codegen to call without casting.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::AppendChild(
    __in ParseTreeNode* pCollectionNode,        // The collection to apply this to
    __in ParseTreeNode* pChildNode              // The child to add
    )
{
    Assert(pCollectionNode->IsCollectionNode());

    CollectionNode* pCollection = static_cast<CollectionNode*>(pCollectionNode);
    return pCollection->AppendChild(pChildNode);
}

//+----------------------------------------------------------------------------
//
//  Function:   AppendChild
//
//  Synopsis:   Called to append a child node to this collection. This node 
//              takes ownership of the passed child and is responsible for 
//              deleting it.
//
//              This is also a mechanism where the parent of a node is set.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::AppendChild(__in ParseTreeNode* pChild)
{
    return InsertChild(pChild, _aryChildren.GetCount());
}

//+----------------------------------------------------------------------------
//
//  Function:   InsertChild
//
//  Synopsis:   Called to insert a child node at the specified index to this 
//              collection. This node  takes ownership of the passed child and 
//              is responsible for deleting it.
//
//              This is also a mechanism where the parent of a node is set.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::InsertChild(__in ParseTreeNode* pChild, UINT uIndex)
{
    CHK_START;

    CHK(_aryChildren.InsertAt(uIndex, pChild));

    if (pChild != nullptr)
    {
        pChild->SetParent(this);
        if (pChild->IsVerified())
        {
            pChild->SetMovedAfterVerified();
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InsertBefore
//
//  Synopsis:   Called to insert a child node just before the specified node.
//              This node takes ownership of the passed child and 
//              is responsible for deleting it.
//
//              This is also a mechanism where the parent of a node is set.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::InsertBefore(__in ParseTreeNode* pChildToInsert, __in ParseTreeNode* pNodeInsertBefore)
{
    CHK_START;

    UINT uIndex;
    CHK(GetChildIndex(pNodeInsertBefore, &uIndex));

    CHK(InsertChild(pChildToInsert, uIndex));

    CHK_RETURN;
}
//+----------------------------------------------------------------------------
//
//  Function:   GetChildIndex
//
//  Synopsis:   Get the index of this child in the collection
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::GetChildIndex(__in ParseTreeNode* pChild, __out UINT* puIndex)
{
    CHK_START;

    (*puIndex) = _aryChildren.Find(pChild);

    if (*puIndex == CModernParseTreeNodeArray::NotFound)
    {
        CHK(E_NOTFOUND);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ExtractChild
//
//  Synopsis:   Called to extract a child node from this collection. This node 
//              relinquishes ownership of the returned child.
//
//              The parent relationship is also severed.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::ExtractChild(UINT index, __deref_out ParseTreeNode** ppChild)
{
    CHK_START;

    _aryChildren[index].CopyTo(ppChild);

    // Remove from the collection
    CHK(_aryChildren.RemoveAt(index));

    // Disconnect the parent (unless the node is null)
    if ((*ppChild) != nullptr)
    {
        (*ppChild)->SetParent(nullptr);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetChild
//
//  Synopsis:   Return the node at the given index. Caller has responsibility
//              to check it is in range.
//
//-----------------------------------------------------------------------------
ParseTreeNode* CollectionNode::GetChild(UINT index) const
{
    Assert(index < GetChildCount());

    return _aryChildren[index];
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyChildren
//
//  Synopsis:   Logic to iterate children for expression typing.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::VerifyChildren()
{
    CHK_START;

    for (UINT i = 0; i < _aryChildren.GetCount(); i++)
    {
        if (_aryChildren[i] != nullptr)
        {
            CHK(_aryChildren[i]->VerifyNode());
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLNameIndex
//
//  Synopsis:   Push the function down to the children.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::SetHLSLNameIndex(UINT uIndex)
{
    CHK_START;

    for (UINT i = 0; i < _aryChildren.GetCount(); i++)
    {
        if (_aryChildren[i] != nullptr)
        {
            CHK(_aryChildren[i]->SetHLSLNameIndex(uIndex));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MarkWritten
//
//  Synopsis:   Logic to iterate children for marking as written.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::MarkWritten()
{
    CHK_START;

    for (UINT i = 0; i < _aryChildren.GetCount(); i++)
    {
        if (_aryChildren[i] != nullptr)
        {
            CHK(_aryChildren[i]->MarkWritten());
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   DumpTree
//
//  Synopsis:   Logic to iterate children for tree dumping
//
//-----------------------------------------------------------------------------
HRESULT CollectionNode::DumpTree(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(ParseTreeNode::DumpTree(pOutput));

    for (UINT i = 0; i < _aryChildren.GetCount(); i++)
    {
        if (_aryChildren[i] != nullptr)
        {
            CHK(_aryChildren[i]->DumpTree(pOutput));
        }
    }

    CHK_RETURN;
}


//+----------------------------------------------------------------------------
//
//  Function:   GatherDefinitionsOfCalledFunctions
//
//  Synopsis:   Recursively search for function identifier nodes. If we find
//              any, lookup the function defintion node that defines the
//              identifier and add it to the passed in collection.
//
//+----------------------------------------------------------------------------
HRESULT CollectionNode::GatherDefinitionsOfCalledFunctions(__inout CModernArray<const FunctionDefinitionNode*>& aryCalled) const
{
    CHK_START;

    for (UINT i = 0; i < _aryChildren.GetCount(); i++)
    {
        // Children can be null as placeholders
        const ParseTreeNode* pChild = _aryChildren[i];
        if (pChild != nullptr)
        {
            if (pChild->GetParseNodeType() == ParseNodeType::functionIdentifier)
            {
                TSmartPointer<CFunctionIdentifierInfo> spFuncInfo;
                CHK_VERIFY(SUCCEEDED(pChild->GetAs<FunctionIdentifierNode>()->GetFunctionIdentifierInfo(&spFuncInfo)));

                // Known functions are implicity defined - they have no function definition node.
                const FunctionDefinitionNode* pCalledFunctionDefinition = spFuncInfo->UseFunctionDefinition();
                if (pCalledFunctionDefinition != nullptr)
                {
                    CHK(aryCalled.Add(pCalledFunctionDefinition));
                }
            }

            if (pChild->IsCollectionNode())
            {
                CHK(static_cast<const CollectionNode*>(pChild)->GatherDefinitionsOfCalledFunctions(aryCalled));
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AssertSubtreeFullyVerified
//
//  Synopsis:   Walks the tree recursively ensuring each node has been verified.
//
//+----------------------------------------------------------------------------
void CollectionNode::AssertSubtreeFullyVerified() const
{
    ParseTreeNode::AssertSubtreeFullyVerified();
    for (UINT i = 0; i < GetChildCount(); i++)
    {
        ParseTreeNode* pChild = _aryChildren[i];
        if (pChild != nullptr)
        {
            pChild->AssertSubtreeFullyVerified();
        }
    }
}
