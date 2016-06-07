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
#include "VariableIdentifierInfo.hxx"
#include "FunctionIdentifierInfo.hxx"
#include "IdentifierNodeBase.hxx"
#include <foundation/collections.hxx>
#include "GLSLPrecisionType.hxx"

class CTypeNameIdentifierInfo;

//+-----------------------------------------------------------------------------
//
//  Class:      CollectionNodeWithScope
//
//  Synopsis:   Node information for a compund statement in GLSL.
//
//------------------------------------------------------------------------------
class CollectionNodeWithScope : public CollectionNode
{
public:
    CollectionNodeWithScope();                                                                          
                                                                                    
    // ParseTreeNode overrides                                                      
    bool IsScopeNode(                                                               
        __in ParseTreeNode* pTreeNode,                                              // Identifier that scope query is for
        __deref_out_opt ParseTreeNode** ppActual                                    // Tree node that is the actual scope
        ) override { (*ppActual) = this; return true; }                             

    HRESULT PreVerifyChildren() override;
                                                                                    
    // Other methods                                                                
    void SetScopeId(int scopeId) { _scopeId = scopeId; }
    int GetScopeId() const { return _scopeId; }
    UINT GetIdentifierCount() const { return _rgIdList.GetCount(); }
    IIdentifierInfo* UseIdentifier(UINT uIndex) const { return _rgIdList[uIndex]; }
                                                                                    
    HRESULT AddDeclaredIdentifier(__in IIdentifierInfo* pInfo);             
                                                                                    
    HRESULT GetIdentifierInfoList(                                                  
        int iSymbolIndex,                                                   // Symbol index to retrive info list for
        __inout CModernArray<TSmartPointer<IIdentifierInfo>>& aryFoundList  // List of infos for that identifier
        );

    bool DefinesTypeNameOrVariableIdentifier(__in IdentifierNodeBase* pIdentifier) const;

    static HRESULT GetNearestScopeInfoList(
        __in IdentifierNodeBase* pIdentifier,                               // The parse tree node for the identifier
        __inout CModernArray<TSmartPointer<IIdentifierInfo>>& rgInfos       // List of infos for that identifier
        );

    static CollectionNodeWithScope* GetCollectionNodeWithScope(
        __in ParseTreeNode* pStartNode                                      // Node to start search at
        );

    static HRESULT GetNearestScopeTypeNameInfo(
        __in ParseTreeNode* pTreeNode,                                      // Node to begin scope search at
        int iSymbolIndex,                                                   // Symbol to look for
        __deref_out CTypeNameIdentifierInfo** ppTypeNameInfo                // Found typename info
        );

    void SetPrecisionForType(GLSLPrecisionType type, int precision);
    HRESULT GetPrecisionForType(GLSLPrecisionType type, __out int* pPrecision) const;

protected:
    const CModernArray<TSmartPointer<IIdentifierInfo>>& UseIdList() const { return _rgIdList; }

private:
    static HRESULT GetNearestScopeInfoList(
        __in ParseTreeNode* pTreeNode,                                      // The parse tree node to begin the search from
        int iSymbolIndex,                                                   // Symbol to look for
        __inout CModernArray<TSmartPointer<IIdentifierInfo>>& rgInfos       // List of infos for that identifier
        );

    void SetPrecisionsFromArray(const int rgPrecisions[GLSLPrecisionType::Count]);

private:
    int _scopeId;                                                           // The scope ID for this node
    CModernArray<TSmartPointer<IIdentifierInfo>> _rgIdList;                 // The identifiers declared in this scope
    int _rgPrecisions[GLSLPrecisionType::Count];                            // The declared precisions for each type in this scope
    static const int s_rgVertDefaultPrecisions[GLSLPrecisionType::Count];   // Default precisions for vertex shaders
    static const int s_rgFragDefaultPrecisions[GLSLPrecisionType::Count];   // Default precisions for fragment shaders
};
