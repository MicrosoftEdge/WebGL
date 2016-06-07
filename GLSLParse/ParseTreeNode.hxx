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

#include "GLSLType.hxx"
#include "GLSLError.hxx"
#include "RefCounted.hxx"

class CGLSLParser;
class InitDeclaratorListNode;
class IdentifierNodeBase;
class CollectionNode;
class CVariableIdentifierInfo;
class ConstantValue;

interface IStringStream;

// Uncomment the following to ease debugging into parser issues. When more error
// propogation code is implemented in WI:50388 then this should not be needed.
//
//#undef CHK
//#define CHK(expr)                                                               \
//do {                                                                            \
//    void * hrTransact; /* this is here to prevent CHK inside a TRANSACT block*/ \
//    (hrTransact);                                                               \
//    hr = (expr);                                                                \
//    if (FAILED(hr)) { if (hr != E_GLSLERROR_KNOWNERROR) { Assert(false); } goto ChkEnd; }                                               \
//} while(false)                                                                  \

//+-----------------------------------------------------------------------------
//
//  Enum:       ParseNodeType
//
//  Synopsis:   Enum to describe what C++ type a node is so that safe casting
//              can be done with it.
//
//------------------------------------------------------------------------------
namespace ParseNodeType
{
    enum Enum
    {
        undefined,

        basicType,
        binaryOperator,
        breakStatement,
        compoundStatement,
        conditionalExpression,
        continueStatement,
        discardStatement,
        expressionList,
        fieldSelection,
        forStatement,
        forRestStatement,
        fullySpecifiedType,
        functionCallHeader,
        functionCallHeaderWithParameters,
        functionCallIdentifier,
        functionDefinition,
        functionHeader,
        functionHeaderWithParameters,
        functionIdentifier,
        functionPrototype,
        functionPrototypeDeclaration,
        identifierBase,
        indexSelection,
        initDeclaratorList,
        initDeclaratorListEntry,
        intConstant,
        ioStruct,
        iterationStatement,
        parameterDeclaration,
        parameterDeclarator,
        parenExpression,
        returnStatement,
        samplerCollection,
        selectionStatement,
        selectionRestStatement,
        singleDeclaration,
        statementList,
        statementWithScope,
        structSpecifier,
        structSpecifierCollection,
        structDeclarationList,
        structDeclaration,
        structDeclaratorList,
        structDeclarator,
        text,
        translationUnit,
        typeNameIdentifier,
        typeQualifier,
        typeSpecifier,
        unaryOperator,
        variableIdentifier,
    };
}

//+-----------------------------------------------------------------------------
//
//  Enum:       IdentifierUsage
//
//  Synopsis:   Identifiers are used for variables and for functions. This
//              categorizes the usage.
//
//------------------------------------------------------------------------------
namespace IdentifierUsage
{
    enum Enum
    {
        variable,
        function
    };
}

//+-----------------------------------------------------------------------------
//
//  Class:      ParseTreeNode
//
//  Synopsis:   Abstract base for parse tree nodes. The parse tree nodes are
//              always a derived class of this node.
//
//------------------------------------------------------------------------------
class ParseTreeNode : public IUnknown
{
public:
    ParseTreeNode();

    virtual HRESULT InitializeAsClone(__in ParseTreeNode* pOriginal) { Assert(false); return E_NOTIMPL; }

    virtual ~ParseTreeNode() {}

    void SetParent(__in CollectionNode* pParent) { _pParent = pParent; }
    HRESULT VerifyNode();
    HRESULT GetExpressionType(__deref_out GLSLType** ppType) const;
    HRESULT WriteScalarExpanded(__in IStringStream* pOutput, int expandedType);
    HRESULT WriteScalarAsVector(__in IStringStream* pOutput, int numComponents);
    HRESULT WriteScalarAsMatrix(__in IStringStream* pOutput, int matrixType);
    HRESULT WriteVectorTruncated(__in IStringStream* pOutput, int numComponents);
    HRESULT WriteMatrixTruncated(__in IStringStream* pOutput, int numComponents);

    UINT TreeDepth() { return _uDepth; }
    bool IsVerified() const { return _fTypesVerified; }

    virtual bool IsCollectionNode() const { return false; }
    virtual ParseNodeType::Enum GetParseNodeType() const { return ParseNodeType::undefined; }
    virtual HRESULT PreVerifyChildren() { return S_OK; }
    virtual HRESULT VerifySelf() { return S_OK; }
    virtual HRESULT VerifyChildren() { return S_OK; }
    virtual HRESULT Clone(__deref_out ParseTreeNode **ppClone) { Assert(false); return E_NOTIMPL; }
    virtual HRESULT SetHLSLNameIndex(UINT uIndex) { return S_OK; }
    virtual HRESULT MarkWritten() { return S_OK; }
    virtual bool IsLValue() const { return false; }
    virtual HRESULT GetLValue(__out CVariableIdentifierInfo** ppInfo) const { return E_NOTIMPL; }

    virtual bool IsScopeNode(
        __in ParseTreeNode* pTreeNode,                              // Node that scope query is for
        __deref_out_opt ParseTreeNode** ppActual                    // Tree node that is the actual scope
        ) { return false; }

    virtual HRESULT IsConstExpression(
        bool fIncludeIndex,                                         // Whether to include loop index in the definition of a constant expression
        __out bool* pfIsConstantExpression,                         // Whether this node is a constant expression
        __out_opt ConstantValue* pValue                             // The value of the constant expression, if desired
        ) const;

    virtual bool IsShortCircuitExpression() const { return false; }

    // Abstract methods to be implemented by subclasses
    virtual HRESULT OutputHLSL(__in IStringStream* pOutput) = 0;

    template<typename T>
    T* GetAs()
    {
        // Sanity check that the type being asked for matches the node type
        Assert(GetParseNodeType() == T::GetClassNodeType());

        return static_cast<T*>(this);
    }

    template<typename T>
    const T* GetAs() const
    {
        // Sanity check that the type being asked for matches the node type
        Assert(GetParseNodeType() == T::GetClassNodeType());

        return static_cast<const T*>(this);
    }

    CollectionNode* GetParent() const { return _pParent; }
    CGLSLParser* GetParser() const { return _pParser; }

    // Methods used for testing and debugging
    UINT CalculateDepth() const;
    virtual HRESULT DumpTree(__in IStringStream* pOutput);
    virtual HRESULT GetDumpString(__in IStringStream* pOutput);

    // Cast to collection
    CollectionNode* AsCollection();

    // Factory method to do cloning
    template<typename T>
    static HRESULT CreateClone(
        __in CGLSLParser* pParser,                                  // Parser that nodes live in
        __in T* pOriginal,                                          // The original node being cloned
        __deref_out ParseTreeNode **ppClone                         // The clone
        )
    {
        CHK_START;

        TSmartPointer<T> spTypedNode;
        CHK(RefCounted<T>::Create(pParser, /*out*/spTypedNode));

        CHK(spTypedNode->InitializeAsClone(pOriginal));

        // Only extract if initialize succeeded
        (*ppClone) = spTypedNode.Extract();

        CHK_RETURN;
    }

    static bool RequiresOrderedExpressionExecution(__in ParseTreeNode* pNode);

    static HRESULT GatherPreviousSiblingExpressions(
        __in CollectionNode* pParent,                           // Parent collection node to gather expressions from
        __in ParseTreeNode* pChild,                             // Child to start gathering expressions from previous sibling
        __inout CModernArray<TSmartPointer<ParseTreeNode>>& aryOrderedExpressions // List of expressions to add to
        );

    static UINT GetMaxFunctionNestingLevel() { return s_uMaxFunctionCallDepth; }

    virtual void AssertSubtreeFullyVerified() const { Assert(_fTypesVerified); }

    void SetMovedAfterVerified() { Assert(_fTypesVerified); _fMovedAfterVerified = true; }

protected:

    void SetExpressionType(__in GLSLType *pType) { _spExprType = pType; }
    void SetBasicExpressionType(int type);

    bool OccursInLoop() const;

    void Initialize(
        __in CGLSLParser* pParser                                   // The parser that owns the tree
        );

protected:
    const static UINT s_uMaxTreeDepth;                              // Maximum allowed parse tree depth
    const static UINT s_uMaxFunctionCallDepth;                      // Maximum allowed function call depth

private:
    CGLSLParser* _pParser;                                          // The parser that owns this tree node
    CollectionNode* _pParent;                                       // The parent of this tree node
    bool _fTypesVerified;                                           // Set to true when the type verification phase finishes
    bool _fMovedAfterVerified;                                      // Whether this node has been shuffled around as part of the translation step after verification. See VerifyNode for more details.
    TSmartPointer<GLSLType> _spExprType;                            // The expression type of the node
    UINT _uDepth;                                                   // Depth of node when created
};
