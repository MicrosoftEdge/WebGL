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
#include "ConditionalExpressionNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "TypeHelpers.hxx"
#include "GLSLParser.hxx"

MtDefine(ConditionalExpressionNode, CGLSLParser, "ConditionalExpressionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
ConditionalExpressionNode::ConditionalExpressionNode()
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ConditionalExpressionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pTest,                  // The LHS of the expression
    __in ParseTreeNode* pTrueResult,            // The RHS of the expression
    __in ParseTreeNode* pFalseResult,           // The RHS of the expression
    const YYLTYPE &location                     // The location of the statement
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pTest));
    CHK(AppendChild(pTrueResult));
    CHK(AppendChild(pFalseResult));

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   We must add all ternary operators to a global list on the parser
//              in order to translate them such that only one of the two
//              branches are executed (HLSL evaluates both). We do this adding
//              to the list in PreVerifyChildren and not VerifySelf so that
//              we keep the correct ordering. This allows us to process the list
//              in order and lets nested conditionals take care of themselves.
//
//+----------------------------------------------------------------------------
HRESULT ConditionalExpressionNode::PreVerifyChildren()
{
    return GetParser()->AddShortCircuitExpression(this);
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   This function ensures that the types of the left and right
//              match and calculates the return type.
//
//-----------------------------------------------------------------------------
HRESULT ConditionalExpressionNode::VerifySelf()
{
    CHK_START;

    // First get the expression types of the test, as well as the true and false results
    TSmartPointer<GLSLType> spTestType, spTrueType, spFalseType;
    CHK(GetChild(0)->GetExpressionType(&spTestType));
    CHK(GetChild(1)->GetExpressionType(&spTrueType));
    CHK(GetChild(2)->GetExpressionType(&spFalseType));

    // Test should be a boolean
    int testType;
    if (FAILED(spTestType->GetBasicType(&testType)) || testType != BOOL_TOK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_TERNARYCONDITIONALNOTBOOL, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Other expressions need to be the same type
    if (!spTrueType->IsEqualType(spFalseType))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_TERNARYCONDITIONALEXPRNE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Sampler and void types are not allowed to be operands in expressions
    if (spTrueType->IsSampler2DType() || spTrueType->IsSamplerCubeType() || spTrueType->IsTypeOrArrayOfType(VOID_TOK))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_WRONGOPERANDTYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // true / false type is our return type
    SetExpressionType(spTrueType);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT ConditionalExpressionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(GetChild(0)->OutputHLSL(pOutput));
    CHK(pOutput->WriteChar('?'));
    CHK(GetChild(1)->OutputHLSL(pOutput));
    CHK(pOutput->WriteChar(':'));
    CHK(GetChild(2)->OutputHLSL(pOutput));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstIndexExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              Currently we don't assign a value, since boolean evaluation
//              is not hooked up or needed at this point. We do assign the
//              value's type though (this is similar to how we handle field
//              selection ConstantValue queries).
//
//-----------------------------------------------------------------------------
HRESULT ConditionalExpressionNode::IsConstExpression(
    bool fIncludeIndex,                             // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,             // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                 // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    CHK_VERIFY(IsVerified());

    // Don't retrieve the value from the test child; We know it will be type boolean (which we don't
    // need to propagate) and we know that boolean evaluation is not yet supported.
    bool fChildIsConst;
    CHK(GetChild(0)->IsConstExpression(fIncludeIndex, &fChildIsConst, /*value*/nullptr));

    if (fChildIsConst)
    {
        // We don't retrieve the value from either true or false children; since we do not
        // know the constant boolean test value, we are not able to choose between the two.
        CHK(GetChild(1)->IsConstExpression(fIncludeIndex, &fChildIsConst, /*value*/nullptr));

        if (fChildIsConst)
        {
            CHK(GetChild(2)->IsConstExpression(fIncludeIndex, &fChildIsConst, /*value*/nullptr));
            
            if (fChildIsConst && pValue != nullptr)
            {
                TSmartPointer<GLSLType> spType;
                CHK(GetExpressionType(&spType));

                // At this point, no one queries for values other than basic types, and we don't
                // have the ability to store non-basic types in ConstantValue. For consistency
                // with other IsConstExpression implementations, return an error if this conditional
                // expression is not a basic type.
                int basicType;
                CHK(spType->GetBasicType(&basicType));

                // Set a type only value, so that somebody can at least inspect the type of
                // this value. If they try to get the actual value it will fail.
                pValue->SetTypeOnly(basicType);
            }
        }
    }

    *pfIsConstantExpression = fChildIsConst;

    CHK_RETURN;
}
