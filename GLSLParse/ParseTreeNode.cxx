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
#include "ParseTreeNode.hxx"
#include "IStringStream.hxx"
#include "RefCounted.hxx"
#include "GLSLType.hxx"
#include "TypeHelpers.hxx"
#include "GLSLTypeInfo.hxx"
#include "GLSLParser.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSL.tab.h"

MtDefine(ParseTreeNode, CGLSLParser, "ParseTreeNode");

const UINT ParseTreeNode::s_uMaxTreeDepth = 140;
const UINT ParseTreeNode::s_uMaxFunctionCallDepth = 256;

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ParseTreeNode::ParseTreeNode() :
    _pParent(nullptr),
    _pParser(nullptr),
    _fTypesVerified(false),
    _fMovedAfterVerified(false),
    _uDepth(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//+----------------------------------------------------------------------------
void ParseTreeNode::Initialize(
    __in CGLSLParser* pParser           // The parser that owns the tree
    )
{
    _pParser = pParser;
}

//+----------------------------------------------------------------------------
//
//  Function:   Verify
//
//  Synopsis:   Verifies that the typing information has been figured out for
//              this node. This should only be called once during the verify
//              pass. Subclasses override methods to complete this algorithm.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::VerifyNode()
{
    CHK_START;

    if (_fMovedAfterVerified)
    {
        // If the node was moved after it was verified, we allow the skipping
        // of verification of this subtree (and the check that nodes can only be verified once).
        // The code that moved the node must be aware of the consequences of doing so
        // and ensure that it is moved to a location such that the resulting parse tree is valid.
        Assert(_fTypesVerified);
        return S_OK;
    }

    // This should only be called once
    Assert(!_fTypesVerified);
    CHKB(!_fTypesVerified);

    // Calculate depth of this node and verify it is not beyond our maximum
    if (_pParent == nullptr)
    {
        _uDepth = 0;
    }
    else
    {
        _uDepth = _pParent->_uDepth + 1;
    }

    if (_uDepth >= s_uMaxTreeDepth)
    {
        CHK(GetParser()->LogError(nullptr, E_GLSLERROR_SHADERCOMPLEXITY, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Do any work before verifying children
    CHK(PreVerifyChildren());

    // First verify children
    CHK(VerifyChildren());

    // Then verify self
    hr = VerifySelf();
    CHK(hr);

    _fTypesVerified = true;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Default implementation of IsConstExpression.
//
//              We are never a const expression unless overridden.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::IsConstExpression(
    bool fIncludeIndex,                                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,                         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                             // The value of the constant expression, if desired
    ) const
{
    (*pfIsConstantExpression) = false;
    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetExpressionType
//
//  Synopsis:   Retrieve the calculated expression type. Not all nodes are
//              part of an expression in a valid way, and these will fail this
//              call because their type has not changed from construction.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::GetExpressionType(__deref_out GLSLType** ppType) const
{
    CHK_START;

    // Calling before verify pass is not valid - this is a product bug, not
    // something that even a bad shader should be able to do.
    Assert(_fTypesVerified);
    CHKB(_fTypesVerified);

    // No type set means not a valid expression
    CHKB(_spExprType != nullptr);

    _spExprType.CopyTo(ppType);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CalculateDepth
//
//  Synopsis:   Get the depth of this node, calculated on the fly. Used only
//              in debugging situations since it does not cache.
//
//              This is used instead of TreeDepth() for cases like dumping
//              when the depth is not calculated yet.
//
//-----------------------------------------------------------------------------
UINT ParseTreeNode::CalculateDepth() const
{
    if (_pParent == nullptr)
    {
        return 0;
    }

    return (_pParent->CalculateDepth() + 1);
}

//+----------------------------------------------------------------------------
//
//  Function:   DumpTree
//
//  Synopsis:   Debug dump of tree under and including this node.
//
//              This is only meant to be called in debugging scenarios or for
//              testing.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::DumpTree(__in IStringStream* pOutput)
{
    CHK_START;

    UINT uDepth = CalculateDepth();
    for (UINT i = 0; i < uDepth * 2; i++)
    {
        CHK(pOutput->WriteChar(' '));
    }

    CHK(GetDumpString(pOutput));
    CHK(pOutput->WriteFormat(1024, ": %p\n", this));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Default string to put in tree dump
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::GetDumpString(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(pOutput->WriteFormat(1024, "%d", GetParseNodeType()));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetBasicExpressionType
//
//  Synopsis:   Set the expression type to the given basic type.
//
//-----------------------------------------------------------------------------
void ParseTreeNode::SetBasicExpressionType(int type)
{
    TSmartPointer<GLSLType> spType;
    if (SUCCEEDED(GLSLType::CreateFromBasicTypeToken(type, &spType)))
    {
        SetExpressionType(spType);
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteScalarExpanded
//
//  Synopsis:   Treat this node as a scalar expression and write HLSL to
//              express it as the given type.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::WriteScalarExpanded(__in IStringStream* pOutput, int expandType)
{
    CHK_START;

    if (TypeHelpers::IsVectorType(expandType))
    {
        CHK(WriteScalarAsVector(pOutput, TypeHelpers::GetVectorLength(expandType)));
    }
    else if (TypeHelpers::IsMatrixType(expandType))
    {
        CHK(WriteScalarAsMatrix(pOutput, expandType));
    }
    else
    {
        // This is a bug
        Assert(false);
        CHK(E_INVALIDARG);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteScalarAsVector
//
//  Synopsis:   Treat this node as a scalar expression and write HLSL to
//              express it as a vector with the given component count.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::WriteScalarAsVector(__in IStringStream* pOutput, int numComponents)
{
    CHK_START;

    // Must be a basic scalar type for this to work
    TSmartPointer<GLSLType> spType;
    CHK(GetExpressionType(&spType));

    int basicType;
    CHK_VERIFY(SUCCEEDED(spType->GetBasicType(&basicType)));
    CHKB(TypeHelpers::IsVectorComponentType(basicType));

    // Wrap in paren + swizzle with 'x' will give us what we want
    CHK(pOutput->WriteString("("));
    CHK(OutputHLSL(pOutput));
    CHK(pOutput->WriteString(")."));

    for (int i = 0; i < numComponents; i++)
    {
        CHK(pOutput->WriteString("x"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteScalarAsMatrix
//
//  Synopsis:   Treat this node as a scalar expression and write HLSL to
//              express it as a matrix with the given component count on
//              each axis.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::WriteScalarAsMatrix(__in IStringStream* pOutput, int matrixType)
{
    CHK_START;

    // Must be a basic scalar type for this to work
    TSmartPointer<GLSLType> spType;
    CHK(GetExpressionType(&spType));

    int basicType;
    CHK_VERIFY(SUCCEEDED(spType->GetBasicType(&basicType)));
    CHKB(TypeHelpers::IsNumericScalarType(basicType));

    // Find out what the type of the matrix is
    int numComponents = TypeHelpers::GetMatrixLength(matrixType);

    // Get the info for that matrix type
    const BasicGLSLTypeInfo* pInfo;
    CHK(BasicGLSLTypeInfo::GetInfo(matrixType, &pInfo));

    // Construct a matrix with the scalar written as a vector as arguments
    CHK(pOutput->WriteString(pInfo->_pszHLSLName));
    CHK(pOutput->WriteString("("));

    for (int i = 0; i < numComponents; i++)
    {
        CHK(WriteScalarAsVector(pOutput, numComponents));
        if (i != numComponents - 1)
        {
            CHK(pOutput->WriteString(","));
        }
    }

    CHK(pOutput->WriteString(")"));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteVectorTruncated
//
//  Synopsis:   Treat this node as a vector expression and write HLSL to
//              express it as a vector with the given component count.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::WriteVectorTruncated(__in IStringStream* pOutput, int numComponents)
{
    CHK_START;

    // Must be a vector type for this to work
    TSmartPointer<GLSLType> spType;
    CHK(GetExpressionType(&spType));

    int basicType;
    CHK_VERIFY(SUCCEEDED(spType->GetBasicType(&basicType)));
    CHKB(TypeHelpers::IsVectorType(basicType));

    // Should not be called unless the vector is too big
    Assert((int)TypeHelpers::GetVectorLength(basicType) > numComponents);

    char szComps[4] = { 'x', 'y', 'z', 'w' };

    // Wrap in paren + swizzle with 'x' will give us what we want
    CHK(pOutput->WriteString("("));
    CHK(OutputHLSL(pOutput));
    CHK(pOutput->WriteString(")."));

    for (int i = 0; i < numComponents; i++)
    {
        CHK(pOutput->WriteChar(szComps[i]));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteMatrixTruncated
//
//  Synopsis:   Treat this node as a vector expression and write HLSL to
//              express it as a vector with the given component count.
//
//-----------------------------------------------------------------------------
HRESULT ParseTreeNode::WriteMatrixTruncated(__in IStringStream* pOutput, int numComponents)
{
    CHK_START;

    // Must be a matrix type for this to work
    TSmartPointer<GLSLType> spType;
    CHK(GetExpressionType(&spType));

    int basicType;
    CHK_VERIFY(SUCCEEDED(spType->GetBasicType(&basicType)));
    CHKB(TypeHelpers::IsMatrixType(basicType));

    // Should not be called unless the matrix is larger than the number of
    // components we need to output.
    Assert((int)TypeHelpers::GetNumMatrixComponents(basicType) > numComponents);

    // We should only be requested to output 1-4 components. 0 components is
    // invalid. > 4 implies this is an argument to a matrix constructor, but
    // matrix constructors don't support matrix truncation.
    Assert(0 < numComponents && numComponents <= 4);

    UINT cMatrixDimension = TypeHelpers::GetMatrixLength(basicType);
    // Wrap in paren + swizzle with 'x' will give us what we want
    CHK(pOutput->WriteString("("));
    CHK(OutputHLSL(pOutput));
    CHK(pOutput->WriteString(")."));

    for (int i = 0; i < numComponents; i++)
    {
        // Output a swizzle in row order (e.g. for 2x2: 00, 01, 10, 11)
        UINT uRow = i % cMatrixDimension;
        UINT uColumn = i / cMatrixDimension;
        CHK(pOutput->WriteFormat(5, "_m%d%d", uColumn, uRow));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AsCollection
//
//  Synopsis:   Cast helper
//
//-----------------------------------------------------------------------------
CollectionNode* ParseTreeNode::AsCollection()
{
    // This is not meant to be called unless you know it is going to work
    Assert(IsCollectionNode());

    return static_cast<CollectionNode*>(this);
}

//+----------------------------------------------------------------------------
//
//  Function:   OccursInLoop
//
//  Synopsis:   Returns whether this node occurs inside a loop.
//
//-----------------------------------------------------------------------------
bool ParseTreeNode::OccursInLoop() const
{
    bool fInLoop = false;

    // Go through the parent nodes to find a loop node, or
    // function definition (at which point we can stop looking).
    const ParseTreeNode* pWalk = this;
    while (pWalk != nullptr)
    {
        if (pWalk->GetParseNodeType() == ParseNodeType::forStatement)
        {
            fInLoop = true;
            break;
        }

        if (pWalk->GetParseNodeType() == ParseNodeType::functionDefinition)
        {
            break;
        }

        pWalk = pWalk->GetParent();
    }

    return fInLoop;
}

//+----------------------------------------------------------------------------
//
//  Function:   GatherExpressions
//
//  Synopsis:   Finds tree nodes that are considered expressions and adds them
//              to the list in reverse order. Does not recurse down nodes that
//              are added to the list.
//
//+----------------------------------------------------------------------------
HRESULT ParseTreeNode::GatherPreviousSiblingExpressions(
    __in CollectionNode* pParent,                           // Parent collection node to gather expressions from
    __in ParseTreeNode* pChild,                             // Child to start gathering expressions from previous sibling
    __inout CModernArray<TSmartPointer<ParseTreeNode>>& aryOrderedExpressions // List of expressions to add to
    )
{
    CHK_START;

    Assert(RequiresOrderedExpressionExecution(pParent));

    // Loop backwards from, but not including, the branch we walked up to
    // gather expressions that must execute before the given child.
    UINT uChildIndex;
    CHK_VERIFY(SUCCEEDED(pParent->GetChildIndex(pChild, &uChildIndex)));
    for (UINT i = uChildIndex; i > 0; i--)
    {
        // Most children of nodes that require ordered execution will
        // themselves be expression. Expressions should have set their expression
        // type during verification.
        // If this node has a non-null expression type, that means it is an expression;
        ParseTreeNode* pLeftChild = pParent->GetChild(i - 1);
        GLSLType* pExprType = pLeftChild->_spExprType;
        bool fChildHasExpressionType = (pExprType != nullptr);
        if (fChildHasExpressionType)
        {
            // Add it to the list if it is not an l-value. l-values by definition cannot have
            // side effects (e.g. assignment expressions and function calls are r-values),
            // so we leave them in place.
            // We also exclude samplers - expressions they are involved in by
            // definition can't have side effects, since they are only allowed to be declared as uniform
            // and cannot be assigned to or even declared as a local.
            if (!pLeftChild->IsLValue() && !pExprType->IsSampler2DType() && !pExprType->IsSamplerCubeType())
            {
                CHK(aryOrderedExpressions.Add(pLeftChild));
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   RequiresOrderedExpressionExecution
//
//  Synopsis:   According to GLSL ES 1.0 spec, expressions only need to be
//              executed in left-to-right ordering for function calls and
//              expression or declaration lists. Technically parenthesized expressions
//              require an order, but it is an less strict ordering that only
//              must be satisfied such that operations within parentheses
//              are done before operations across parentheses.
//
//               *  Except where stated otherwise, the order of evaluation of
//                  operands in an expression is undefined.
//
//+----------------------------------------------------------------------------
bool ParseTreeNode::RequiresOrderedExpressionExecution(__in ParseTreeNode* pNode)
{
    ParseNodeType::Enum nodeType = pNode->GetParseNodeType();
    return (nodeType == ParseNodeType::functionCallHeaderWithParameters ||
        nodeType == ParseNodeType::expressionList ||
        nodeType == ParseNodeType::initDeclaratorList
        );
}
