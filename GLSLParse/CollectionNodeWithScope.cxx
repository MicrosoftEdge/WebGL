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
#include "CollectionNodeWithScope.hxx"
#include "GLSLParser.hxx"
#include "VariableIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"

MtDefine(CollectionNodeWithScope, CGLSLParser, "CollectionNodeWithScope");

// See GLSL_ES 1.0 spec, section 4.5.3 - Default Precision Qualifiers
const int CollectionNodeWithScope::s_rgVertDefaultPrecisions[GLSLPrecisionType::Count] =
{
    HIGH_PRECISION,     // float
    HIGH_PRECISION,     // int
    LOW_PRECISION,      // sampler2D
    LOW_PRECISION,      // samplerCube
};

const int CollectionNodeWithScope::s_rgFragDefaultPrecisions[GLSLPrecisionType::Count] =
{
    NO_PRECISION,       // float
    MEDIUM_PRECISION,   // int
    LOW_PRECISION,      // sampler2D
    LOW_PRECISION,      // samplerCube
};

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CollectionNodeWithScope::CollectionNodeWithScope() : 
    _scopeId(-1)
{
    static_assert(ARRAYSIZE(_rgPrecisions) == static_cast<UINT>(GLSLPrecisionType::Count), "Precision array must be the same size as the number of enums");
    static_assert(ARRAYSIZE(_rgPrecisions) == ARRAYSIZE(s_rgVertDefaultPrecisions), "Precision array must be the same size as the default values");
    static_assert(ARRAYSIZE(s_rgFragDefaultPrecisions) == ARRAYSIZE(s_rgVertDefaultPrecisions), "Default precision arrays must be the same size");

    for (UINT i = 0; i < ARRAYSIZE(_rgPrecisions); i++)
    {
        _rgPrecisions[i] = NO_PRECISION;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   Before verifying our child nodes (children are verified before 
//              their parents), we must propagate precision information from
//              the scope we live inside. We could get away with not doing
//              this, but that would force walking up to the root for all
//              variable declarations, in the general case (usually there
//              are only global or implicit precision declarations).
//
//+----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::PreVerifyChildren()
{
    ParseTreeNode* pParent = GetParent();
    if (pParent == nullptr)
    {
        // If we are the root (TranslationUnitNode) set the defaults based on the
        // shader type as specified in GLSL spec.
        Assert(GetParseNodeType() == ParseNodeType::translationUnit);
        SetPrecisionsFromArray((GetParser()->GetShaderType() == GLSLShaderType::Vertex) ? s_rgVertDefaultPrecisions : s_rgFragDefaultPrecisions);
    }
    else
    {
        // Otherwise, search from the parent. Since this node is its own scope, searching
        // from itself will just return itself, which is not what we want.
        CollectionNodeWithScope* pParentScope = GetCollectionNodeWithScope(GetParent());
        SetPrecisionsFromArray(pParentScope->_rgPrecisions);
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddDeclaredIdentifier
//
//  Synopsis:   Add identifier info for an identifier that is declared in this
//              compound statement's scope.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::AddDeclaredIdentifier(__in IIdentifierInfo* pInfo)
{
    return _rgIdList.Add(pInfo);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierInfoList
//
//  Synopsis:   Function that does the heavy lifting for searching this scope.
//
//              Success means that 1 or more matching infos in the scope were
//              found, and failure means none were found.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::GetIdentifierInfoList(
    int iSymbolIndex,                                                   // Symbol index to retrive info list for
    __inout CModernArray<TSmartPointer<IIdentifierInfo>>& aryFoundList  // List of infos for that identifier
    )
{
    CHK_START;

    Assert(aryFoundList.GetCount() == 0);

    for (UINT i = 0; i < _rgIdList.GetCount(); i++)
    {
        if (_rgIdList[i]->GetSymbolIndex() == iSymbolIndex)
        {
            CHK(aryFoundList.Add(_rgIdList[i]));
        }
    }

    CHKB(aryFoundList.GetCount() != 0);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   DefinesTypeNameOrVariableIdentifier
//
//  Synopsis:   Returns true if this scope contains an identifier with the 
//              same symbol index as the passed in identifier node. However,
//              known functions do not define an identifier for typename and
//              variable identifier nodes as they essentially live in another
//              scope. This means you can have a global variable or typename
//              with the same identifier as a known function (unless the known
//              function has been overloaded by a user-defined function)
//
//-----------------------------------------------------------------------------
bool CollectionNodeWithScope::DefinesTypeNameOrVariableIdentifier(__in IdentifierNodeBase* pIdentifier) const
{
    Assert(pIdentifier->GetParseNodeType() == ParseNodeType::typeNameIdentifier || pIdentifier->GetParseNodeType() == ParseNodeType::variableIdentifier);

    for (UINT i = 0; i < _rgIdList.GetCount(); i++)
    {
        const IIdentifierInfo* pIdInfo = _rgIdList[i];
        if (pIdInfo->GetSymbolIndex() == pIdentifier->GetSymbolIndex())
        {
            // Function identifiers that are for known functions should be skipped, as they
            // are not defined for typename or variable identifiers.
            const CFunctionIdentifierInfo* pFuncInfo = pIdInfo->AsFunction();
            if (pFuncInfo == nullptr || !pFuncInfo->IsKnownFunction())
            {
                return true;
            }
        }
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetNearestScopeInfoList
//
//  Synopsis:   Retrieve identifier infos for the identifiers that match the
//              given node in the nearest scope.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::GetNearestScopeInfoList(
    __in IdentifierNodeBase* pIdentifier,                                       // The parse tree node for the identifier
    __inout CModernArray<TSmartPointer<IIdentifierInfo>>& rgInfos                // List of infos for that identifier
    )
{
    return GetNearestScopeInfoList(pIdentifier, pIdentifier->GetSymbolIndex(), rgInfos);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetNearestScopeInfoList
//
//  Synopsis:   Retrieve identifier infos for the identifiers that match the
//              given symbol index in the nearest scope to pTreeNode.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::GetNearestScopeInfoList(
    __in ParseTreeNode* pTreeNode,                                              // The parse tree node to begin the search from
    int iSymbolIndex,                                                           // Symbol to look for
    __inout CModernArray<TSmartPointer<IIdentifierInfo>>& rgInfos               // List of infos for that identifier
    )
{
    CHK_START;

    ParseTreeNode* pWalk = pTreeNode;
    ParseTreeNode* pScopeNode = nullptr;
    bool fFound = false;

    // First search up the parse tree to find the nearest scope that
    // declared something equivalent.
    while (pWalk != nullptr)
    {
        if (pWalk->IsScopeNode(pTreeNode, &pScopeNode))
        {
            // IsScopeNode should never return true without a node
            CHK_VERIFY(pScopeNode != nullptr);

            CollectionNodeWithScope* pScope = static_cast<CollectionNodeWithScope*>(pScopeNode);
            if (SUCCEEDED(pScope->GetIdentifierInfoList(iSymbolIndex, rgInfos)))
            {
                fFound = true;
                break;
            }
        }

        pWalk = pWalk->GetParent();
    }

    // This will have even looked in the global scope, so if we have not found it
    // then it does not exist.
    CHKB(fFound);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetCollectionNodeWithScope
//
//  Synopsis:   As part of figuring out the scoping of a node, we need
//              to know what scope it exists in. This walks up the tree
//              to find the nearest node that defines a scope.
//
//-----------------------------------------------------------------------------
CollectionNodeWithScope* CollectionNodeWithScope::GetCollectionNodeWithScope(
    __in ParseTreeNode* pStartNode                                              // Node to start search at
    )
{
    ParseTreeNode* pWalk = pStartNode;
    ParseTreeNode* pScopeNode = nullptr;

    while (pWalk != nullptr)
    {
        if (pWalk->IsScopeNode(pStartNode, &pScopeNode))
        {
            break;
        }

        pWalk = pWalk->GetParent();
    }

    // Since our root node supports scoping, a well formed parse tree should never
    // end up with a null pointer here. The malformed tree should have been detected
    // before doing anything like this.
    Assert(pScopeNode != nullptr);

    // All things that claim they are scope nodes should derive from this class
    CollectionNodeWithScope* pCompound = static_cast<CollectionNodeWithScope*>(pScopeNode);

    return pCompound;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetNearestScopeTypeNameInfo
//
//  Synopsis:   Based on a starting treenode, find the nearest scope that defines
//              the given symbol index, and, if it exists, return the typename 
//              identifier info that was defined. If the identifier is not
//              defined in the scope for pTreeNode, or it is not defined as
//              a typename, returns failure.
//
//-----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::GetNearestScopeTypeNameInfo(
    __in ParseTreeNode* pTreeNode,                                              // Node to begin scope search at
    int iSymbolIndex,                                                           // Symbol to look for
    __deref_out CTypeNameIdentifierInfo** ppTypeNameInfo                        // Found typename info
    )
{
    CHK_START;

    CModernArray<TSmartPointer<IIdentifierInfo>> rgInfos;
    CHK(CollectionNodeWithScope::GetNearestScopeInfoList(pTreeNode, iSymbolIndex, rgInfos));

    Assert(rgInfos.GetCount() > 0);

    bool fFound = false;
    TSmartPointer<CTypeNameIdentifierInfo> spTypeNameInfo;
    for (UINT i = 0; i < rgInfos.GetCount(); i++)
    {
        CTypeNameIdentifierInfo* pTypeNameIdentifierInfo = rgInfos[i]->AsTypeName();
        if (pTypeNameIdentifierInfo != nullptr)
        {
            if (fFound)
            {
                AssertSz(false, "Verification should never allow typenames with the same identifier in single scope");
                CHK(E_UNEXPECTED);
            }
            else
            {
                spTypeNameInfo = pTypeNameIdentifierInfo;
                fFound = true;
            }
        }
    }

    CHKB(fFound);

    AssertSz(spTypeNameInfo != nullptr, "fFound was true but we don't have a typename identifier info?");

    spTypeNameInfo.CopyTo(ppTypeNameInfo);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetPrecisionForType
//
//  Synopsis:   Sets the precision at the given array index for this scope's
//              type's precisions.
//
//+----------------------------------------------------------------------------
void CollectionNodeWithScope::SetPrecisionForType(GLSLPrecisionType type, int precision)
{
    UINT uType = static_cast<UINT>(type);
    _rgPrecisions[uType] = precision;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetPrecisionForType
//
//  Synopsis:   Looks up the precision for a given component type that has
//              precision. Returns an error if the precision for the component
//              type is NO_PRECISION as this implies precision is not set for
//              a type that requires precision.
//
//+----------------------------------------------------------------------------
HRESULT CollectionNodeWithScope::GetPrecisionForType(GLSLPrecisionType type, __out int* pPrecision) const
{
    CHK_START;

    UINT uType = static_cast<UINT>(type);
    CHKB(_rgPrecisions[uType] != NO_PRECISION);

    *pPrecision = _rgPrecisions[uType];

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetPrecisionsFromArray
//
//  Synopsis:   Sets the precision on this scope for each of the types
//              that require precision
//
//+----------------------------------------------------------------------------
void CollectionNodeWithScope::SetPrecisionsFromArray(const int rgPrecisions[GLSLPrecisionType::Count])
{
    for (UINT i = 0; i < ARRAYSIZE(_rgPrecisions); i++)
    {
        _rgPrecisions[i] = rgPrecisions[i];
    }
}
