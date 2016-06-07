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
#include "ForStatementNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "InitDeclaratorListNode.hxx"
#include "InitDeclaratorListEntryNode.hxx"
#include "BinaryOperatorNode.hxx"
#include "UnaryOperatorNode.hxx"

MtDefine(ForStatementNode, CGLSLParser, "ForStatementNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ForStatementNode::ForStatementNode() : 
    _basicType(NO_TYPE),
    _fRequestUnroll(true)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pInit,                  // For init statement
    __in ParseTreeNode* pRest,                  // For rest statement
    __in ParseTreeNode* pStatement,             // What gets iterated
    const YYLTYPE &location                     // The location of the declarator
    )
{
    ParseTreeNode::Initialize(pParser);

    _location = location;

    AppendChild(pInit);
    AppendChild(pRest);
    AppendChild(pStatement);

    SetScopeId(GetParser()->GenerateScopeId());

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Appendix A of the GLSL spec details that for loops are very
//              tightly restricted. This verification is done here.
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::VerifySelf()
{
    CHK_START;

    if (FAILED(VerifyInitStatement()))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDFORLOOPINIT, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    if (FAILED(VerifyCondStatement()))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDFORLOOPCOND, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    if (FAILED(VerifyIterStatement()))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDFORLOOPITER, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // The loop variable cannot be modified in the loop - the iteration statement should
    // have the only write count for the variable.
    if (_spLoopIndex->GetWriteCount() != 1)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDFORLOOPINDEXWRITE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    const UINT cLoopIterations = (_basicType == INT_TOK) ? DetermineLoopIterations<int>() : DetermineLoopIterations<double>();

    if (GetParser()->GetFeatureLevel() < WebGLFeatureLevel::Level_10)
    {
        // If we detect a non-terminating loop, then this will not succeed compilation
        // on a 9 feature level. Make a nice error message here rather than some odd link
        // error from the HLSL compile failing.
        if (cLoopIterations == UINT_MAX)
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_CANNOTUNROLLLOOP, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }        
        else
        {
            // In FL9_X, all loops are unrolled by the shader compiler. Specifying
            // [loop] will cause a compiler error.
            _fRequestUnroll = true;
        }
    }
    else
    {
        // If our loop iterations are over our threshold, we will not ask the HLSL
        // compiler to unroll the loop, since doing so may cause excessively long
        // HLSL compilation time.
        static const UINT s_cMaxLoopUnrolls = 3;
        _fRequestUnroll = (cLoopIterations <= s_cMaxLoopUnrolls);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyInitStatement
//
//  Synopsis:   Verify that the init statement meets the restrictions outlined
//              in the spec.
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::VerifyInitStatement()
{
    CHK_START;

    // The for_init statement needs to be an inititialization declaration of
    // the loop variable. So it needs to be an init declarator list.
    CHKB(GetChild(0)->GetParseNodeType() == ParseNodeType::initDeclaratorList);
    InitDeclaratorListNode* pInitDeclList = GetChild(0)->GetAs<InitDeclaratorListNode>();

    // There needs to be a single identifier
    CHKB(pInitDeclList->GetIdentifierCount() == 1);
    InitDeclaratorListEntryNode* pEntry = pInitDeclList->GetEntry(0);

    CHK(pEntry->GetIdentifierNode()->GetVariableIdentifierInfo(&_spLoopIndex));

    // The identifer needs to be initialized
    CHKB(pEntry->GetDeclarationType() == DeclarationType::initialized);

    CHK(_spLoopIndex->UseType()->GetBasicType(&_basicType));

    // The initializer needs to be a constant expression
    CHK(VerifyLoopIndexTypedConstant(pEntry->GetInitializerNode(), &_verificationInfo.loopInitializerConstant));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyCondStatement
//
//  Synopsis:   Verify that the conditional statement meets the restrictions 
//              outlined in the spec.
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::VerifyCondStatement()
{
    CHK_START;

    CHKB(GetChild(1)->IsCollectionNode());
    CollectionNode* pForRest = static_cast<CollectionNode*>(GetChild(1));
    
    // Make sure there is a condition at all
    CHKB(pForRest->GetChild(0) != nullptr);

    // It needs to be a binary operator
    CHKB(pForRest->GetChild(0)->GetParseNodeType() == ParseNodeType::binaryOperator);
    BinaryOperatorNode* pOperator = pForRest->GetChild(0)->GetAs<BinaryOperatorNode>();

    // Left side needs to be an identifier
    CHKB(IsLoopIndexIdentifier(pOperator->GetChild(0)));

    // The operator needs to be in a fixed list
    int comparisonOperator = pOperator->GetOperator();
    switch (comparisonOperator)
    {
    case LEFT_ANGLE:
    case RIGHT_ANGLE:
    case LE_OP:
    case GE_OP:
    case EQ_OP:
    case NE_OP:
        break;

    default:
        CHK(E_FAIL);
    }

    // The initializer needs to be a constant expression
    CHK(VerifyLoopIndexTypedConstant(pOperator->GetChild(1), &_verificationInfo.loopComparisonConstant));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyIterStatement
//
//  Synopsis:   Verify that the iteration statement meets the restrictions 
//              outlined in the spec.
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::VerifyIterStatement()
{
    CHK_START;

    CHKB(GetChild(1)->IsCollectionNode());
    CollectionNode* pForRest = static_cast<CollectionNode*>(GetChild(1));
    
    // Make sure there is an iterator at all
    CHKB(pForRest->GetChild(1) != nullptr);

    // It needs to be a binary operator or unary operator
    if (pForRest->GetChild(1)->GetParseNodeType() == ParseNodeType::binaryOperator)
    {
        BinaryOperatorNode* pOperator = pForRest->GetChild(1)->GetAs<BinaryOperatorNode>();

        // Left side needs to be an identifier
        CHKB(IsLoopIndexIdentifier(pOperator->GetChild(0)));

        int iterationOperator = pOperator->GetOperator();

        // The operator needs to be increment or decrement
        CHKB(iterationOperator == ADD_ASSIGN || iterationOperator == SUB_ASSIGN);

        // The right side needs to be a constant expression that we can extract a value from.
        CHK(VerifyLoopIndexTypedConstant(pOperator->GetChild(1), &_verificationInfo.loopIterationConstant));
    }
    else if (pForRest->GetChild(1)->GetParseNodeType() == ParseNodeType::unaryOperator)
    {
        UnaryOperatorNode* pOperator = pForRest->GetChild(1)->GetAs<UnaryOperatorNode>();

        // The operated quantity needs to be the loop index
        CHKB(IsLoopIndexIdentifier(pOperator->GetChild(0)));

        int iterationOperator = pOperator->GetOperator();

        // The operator needs to be increment or decrement
        CHKB(iterationOperator == INC_OP || iterationOperator == DEC_OP);

        if (_basicType == INT_TOK)
        {
            _verificationInfo.loopIterationConstant.SetValue<int>(1);
        }
        else
        {
            _verificationInfo.loopIterationConstant.SetValue<double>(1.0);
        }
    }
    else
    {
        // Not a supported expression
        CHK(E_FAIL);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLoopIndexIdentifier
//
//  Synopsis:   Checks if the given node is an identifier node for the loop
//              index variable.
//
//-----------------------------------------------------------------------------
bool ForStatementNode::IsLoopIndexIdentifier(__in ParseTreeNode* pNode)
{
    if (pNode->GetParseNodeType() == ParseNodeType::variableIdentifier)
    {
        VariableIdentifierNode* pIdentifier = pNode->GetAs<VariableIdentifierNode>();

        // This needs to be the loop index
        TSmartPointer<CVariableIdentifierInfo> spIdInfo;
        if (SUCCEEDED(pIdentifier->GetVariableIdentifierInfo(&spIdInfo)))
        {
            return (spIdInfo == _spLoopIndex);
        }
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyLoopIndexTypedConstant
//
//  Synopsis:   Checks if the given node is an expression that resolves to a
//              constant of the same type as the loop index.
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::VerifyLoopIndexTypedConstant(__in ParseTreeNode* pNode, __out ConstantValue* pConstantValue)
{
    CHK_START;
    
    bool fIsLoopIndexTypedConstant = false;

    if (_basicType == INT_TOK || _basicType == FLOAT_TOK)
    {
        bool fConstant;
        CHK(pNode->IsConstExpression(/*loopindex*/false, &fConstant, pConstantValue));

        int constantType;
        if (SUCCEEDED(pConstantValue->GetBasicType(&constantType)) && (constantType == _basicType))
        {
            fIsLoopIndexTypedConstant = true;
        }
    }

    CHKB(fIsLoopIndexTypedConstant);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // Only request unrolling if we determined at verification time that
    // we should do so. If not, the HLSL compiler will use flow control if available.
    if (_fRequestUnroll)
    {
        CHK(pOutput->WriteString("[unroll] "));
    }
    else
    {
        Assert(GetParser()->GetFeatureLevel() >= WebGLFeatureLevel::Level_10);
        CHK(pOutput->WriteString("[loop] "));
    }

    CHK(pOutput->WriteString("for ("));
    CHK(GetChild(0)->OutputHLSL(pOutput));
    CHK(GetChild(1)->OutputHLSL(pOutput));
    CHK(pOutput->WriteString(")\n"));
    CHK(GetChild(2)->OutputHLSL(pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT ForStatementNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "ForStatementNode scope=%d", GetScopeId());
}

//+----------------------------------------------------------------------------
//
//  Function:   DetermineLoopIterations
//
//  Synopsis:   Figures out the number of times a this verified for loop will
//              execute. Since we know that the loop variable was initialized
//              to a constant, the condition compares the loop variable to a
//              constant, and the loop variable is incremented/decremented
//              by a constant, we can perform this calculation.
//
//+----------------------------------------------------------------------------
template <typename T>
UINT ForStatementNode::DetermineLoopIterations() const
{
    // The initializer currently can fail to extract an actual value as we do
    // not support evaluating all constant expressions (e.g. constant ternary expression)
    // If we don't know, default to a low number that will default to [unroll]
    UINT cIterations = 1;

    T initialValue;
    if (SUCCEEDED(_verificationInfo.loopInitializerConstant.GetValue(&initialValue)))
    {
        T comparisonValue;
        Release_Assert(SUCCEEDED(_verificationInfo.loopComparisonConstant.GetValue(&comparisonValue)));

        T iterationValue;
        Release_Assert(SUCCEEDED(_verificationInfo.loopIterationConstant.GetValue(&iterationValue)));

        if (iterationValue != 0)
        {
            // We don't keep track of the comparison and increment/decrement operators, and
            // thus do not determine whether the loop will terminate. We calculate the number
            // of iterations based on the spread between start and ending values. Dividing
            // this by how much the loop variable changes on each iteration yields the 
            // number of iterations we expect this loop to execute.
            cIterations = static_cast<UINT>(abs(initialValue - comparisonValue) / iterationValue);
        }
        else
        {
            cIterations = UINT_MAX;
        }
    }
    
    return cIterations;
}
