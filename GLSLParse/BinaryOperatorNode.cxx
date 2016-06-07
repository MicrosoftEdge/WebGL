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
#include "BinaryOperatorNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "TypeHelpers.hxx"
#include "RefCounted.hxx"
#include "GLSLParser.hxx"
#include "GLSL.tab.h"
#include "StructGLSLType.hxx"
#include "TypeNameIdentifierInfo.hxx"

MtDefine(BinaryOperatorNode, CGLSLParser, "BinaryOperatorNode");

//+-----------------------------------------------------------------------------
//
//  Member:     OperatorInfo
//
//  Synopsis:   Table of operator information for binary operators. See the
//              OperatorInfo.*xx files for more information.
//
//              The information in the columns (in order):
//
//              _op                 The operator that the entry is for (using 
//                                  the Bison token).
//              _pszOpString        The string to use when writing out the 
//                                  operator.
//              _fHLSLBoolVector    Whether the operator returns a boolean 
//                                  vector in HLSL.
//              _fWriteLeft         Whether the operator writes to the left 
//                                  operand.
//              _inputType          Valid basic type for input, can be 
//                                  NUMERIC_TYPE or SIMPLE_TYPE as well as any 
//                                  Bison basic type token.
//              _glslExpand         Expansion supported by operator in GLSL.
//              _hlslExpand         Expansion supported by operator in HLSL.
//              _returnType         Return type, can be LEFT_EXPR_TYPE or any
//                                  Bison basic type token.
//
//------------------------------------------------------------------------------
const OperatorInfo BinaryOperatorNode::s_info[] = {
    { SLASH,        "/",    false,  false,  NUMERIC_TYPE,           ExpandType::Both,       ExpandType::Right,      LEFT_EXPR_TYPE }, // HLSL allows division of vector by a scalar
    { STAR,         "*",    false,  false,  NUMERIC_TYPE,           ExpandType::Both,       ExpandType::Both,       LEFT_EXPR_TYPE }, // HLSL allows multiplication of vectors and scalars
    { PLUS,         "+",    false,  false,  NUMERIC_TYPE,           ExpandType::Both,       ExpandType::None,       LEFT_EXPR_TYPE },
    { DASH,         "-",    false,  false,  NUMERIC_TYPE,           ExpandType::Both,       ExpandType::None,       LEFT_EXPR_TYPE },
    { EQ_OP,        "==",   true,   false,  SIMPLE_TYPE,            ExpandType::None,       ExpandType::None,       BOOL_TOK       }, // HLSL returns boolean vector for ==
    { NE_OP,        "!=",   true,   false,  SIMPLE_TYPE,            ExpandType::None,       ExpandType::None,       BOOL_TOK       }, // HLSL returns boolean vector for !=
    { LEFT_ANGLE,   "<",    false,  false,  SCALAR_NUMERIC_TYPE,    ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { RIGHT_ANGLE,  ">",    false,  false,  SCALAR_NUMERIC_TYPE,    ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { LE_OP,        "<=",   false,  false,  SCALAR_NUMERIC_TYPE,    ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { GE_OP,        ">=",   false,  false,  SCALAR_NUMERIC_TYPE,    ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { AND_OP,       "&&",   false,  false,  BOOL_TOK,               ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { OR_OP,        "||",   false,  false,  BOOL_TOK,               ExpandType::None,       ExpandType::None,       BOOL_TOK       },
    { XOR_OP,       "!=",   false,  false,  BOOL_TOK,               ExpandType::None,       ExpandType::None,       BOOL_TOK       }, // Logical XOR is equivalent to != for booleans
    { EQUAL,        "=",    false,  true,   SIMPLE_TYPE,            ExpandType::None,       ExpandType::None,       LEFT_EXPR_TYPE },
    { MUL_ASSIGN,   "*=",   false,  true,   NUMERIC_TYPE,           ExpandType::Right,      ExpandType::Right,      LEFT_EXPR_TYPE },
    { DIV_ASSIGN,   "/=",   false,  true,   NUMERIC_TYPE,           ExpandType::Right,      ExpandType::Right,      LEFT_EXPR_TYPE },
    { MOD_ASSIGN,   "%=",   false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
    { ADD_ASSIGN,   "+=",   false,  true,   NUMERIC_TYPE,           ExpandType::Right,      ExpandType::None,       LEFT_EXPR_TYPE },
    { SUB_ASSIGN,   "-=",   false,  true,   NUMERIC_TYPE,           ExpandType::Right,      ExpandType::None,       LEFT_EXPR_TYPE },
    { RIGHT_ASSIGN, ">>=",  false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
    { LEFT_ASSIGN,  "<<=",  false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
    { AND_ASSIGN,   "&=",   false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
    { OR_ASSIGN,    "|=",   false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
    { XOR_ASSIGN,   "^=",   false,  true,   NUMERIC_TYPE,           ExpandType::None,       ExpandType::None,       NO_TYPE        },
};

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
BinaryOperatorNode::BinaryOperatorNode() : 
    _fComponentMultiply(true),
    _fExpandLeft(false),
    _fExpandRight(false),
    _expandBasicType(NO_TYPE),
    _fWrapInAll(false),
    _fVerifiedOpOnStructTypes(false),
    _fAllowArrayAssignment(false),
    _pInfo(nullptr)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pLHS,                   // The LHS of the Multiplicative
    __in ParseTreeNode* pRHS,                   // The RHS of the expression
    const YYLTYPE &location,                    // The location of the symbol
    int op                                      // The operator
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pLHS));
    CHK(AppendChild(pRHS));

    _location = location;

    CHK(OperatorInfo::GetInfo(s_info, ARRAYSIZE(s_info), op, &_pInfo));
    _fComponentMultiply = true;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Initialize from another node, given the collection
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    BinaryOperatorNode* pOriginalOp = pOriginalColl->GetAs<BinaryOperatorNode>();

    _location = pOriginalOp->_location;
    _fComponentMultiply = pOriginalOp->_fComponentMultiply;
    _pInfo = pOriginalOp->_pInfo;

    // Add all of the clone children
    for (UINT i = 0; i < rgChildClones.GetCount(); i++)
    {
        CHK(AppendChild(rgChildClones[i]));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVerifyChildren
//
//  Synopsis:   We must add all logical operators to a global list on the parser
//              in order to translate them such that short circuiting works.
//
//              We do this adding to the list in PreVerifyChildren and not 
//              VerifySelf so that we keep the correct ordering.
//
//+----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::PreVerifyChildren()
{
    if (IsShortCircuitExpression())
    {
        return GetParser()->AddShortCircuitExpression(this);
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsShortCircuitExpression
//
//  Synopsis:   Determine if this expression needs to be handled for short
//              circuiting. Basically, this is && and || operators.
//
//+----------------------------------------------------------------------------
bool BinaryOperatorNode::IsShortCircuitExpression() const
{
    if (_pInfo->_op == AND_OP || _pInfo->_op == OR_OP)
    {
        return true;
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   This function ensures that the types of the left and right
//              match and calculates the return type.
//
//              Special care needs to be taken for multiply. In HLSL these 
//              have different syntax (mul and '*' respectively). For GLSL 
//              it depends on the types of the operands.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::VerifySelf()
{
    CHK_START;

    // First get the expression types of the left and right sides
    TSmartPointer<GLSLType> spLType, spRType;
    CHK(GetChild(0)->GetExpressionType(&spLType));
    CHK(GetChild(1)->GetExpressionType(&spRType));

    if (spLType->IsStructType() && spRType->IsStructType())
    {
        CHK(VerifyOperatorForStructTypes(spLType, spRType));
    }
    else if (spLType->IsBasicType() && spRType->IsBasicType())
    {
        CHK(VerifyOperatorForBasicTypes(spLType, spRType));
    }
    else if (_fAllowArrayAssignment && _pInfo->_op == EQUAL &&
        spLType->IsArrayType() && spRType->IsArrayType())
    {
        // There are special cases where we need to generate code that assigns
        // one array to another. This is not legal in GLSL, but we'll allow it since
        // the translated HLSL compiles fine. We need this to work in the following example
        // 
        //  void foo(int a[2]){} void bar { int x[2], y[2]; foo(true ? x : y); }
        //
        // In the above GLSL, in order to only execute one side of the ternary, we
        // translate it into an if/else. Both sides of the if/else assign into a temporary
        // variable, which in this case is an array type.
        //
        // These array types had better be equivalent types or the codegen has a bug.
        CHK_VERIFY(spLType->IsEqualType(spRType));
        CHK_VERIFY(SUCCEEDED(VerifyLValue()));

        SetExpressionType(spLType);
    }
    else
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyOperatorForStructTypes
//
//  Synopsis:   Verifies the types are equal and the operator is defined 
//              for the types. Performs some bookkeeping to set the expression
//              type and the fact that we verified (and thus should output)
//              for struct types.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::VerifyOperatorForStructTypes(__in GLSLType* pLType, __in GLSLType* pRType)
{
    CHK_START;

    Assert(pLType->IsStructType() && pRType->IsStructType());

    // In order to perform an binary operator, the types of the expressions must be equivalent...
    if (!pLType->IsEqualType(pRType))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // ... and not contain any array types in the fields.
    if (pLType->AsStructType()->ContainsArrayType())
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_OPUNDEFINEDFORTYPESWITHARRAYS, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    switch (_pInfo->_op)
    {
        case EQ_OP:
        case NE_OP:
            // Notify the type that its equals operator has been used, so that it will
            // generate the function.
            pLType->AsStructType()->SetEqualsOperatorUsed();
            // The expression type is the return type of the operator (in this case BOOL_TOK).
            SetBasicExpressionType(_pInfo->_returnType);
            break;
        case EQUAL:
            CHK(VerifyLValue());
            // The expression type for assignment is LEFT_EXPR_TYPE
            SetExpressionType(pLType);
            break;
        default:
            // Only the three above binary operators are allowed for struct types.
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_OPUNDEFINEDFORSTRUCTS, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
            break;
    }

    _fVerifiedOpOnStructTypes = true;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyOperatorForBasicTypes
//
//  Synopsis:   Completes the verification for operators on basic types
//
//              Special care needs to be taken for multiply. In HLSL these 
//              have different syntax (mul and '*' respectively). For GLSL 
//              it depends on the types of the operands.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::VerifyOperatorForBasicTypes(__in GLSLType* pLType, __in GLSLType* pRType)
{
    CHK_START;

    // These operators only operate on basic types
    int lType, rType;
    CHK(pLType->GetBasicType(&lType));
    CHK(pRType->GetBasicType(&rType));

    // No binary operators are defined for texture/sampler types
    if (TypeHelpers::IsTextureType(lType) || TypeHelpers::IsTextureType(rType) || lType == VOID_TOK || rType == VOID_TOK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_WRONGOPERANDTYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Make sure that the component types are compatible so that float * int does not work, or vec2 * int does not work
    int lComp, rComp;
    CHK(TypeHelpers::GetComponentType(lType, &lComp));
    CHK(TypeHelpers::GetComponentType(rType, &rComp));

    if (lComp != rComp)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Return type if we are multiplying
    int mulReturnType = NO_TYPE;

    // Make sure that the types of the children are compatible with the operators
    if (!_pInfo->AreCompatibleTypes(lType, rType))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // If the operator supports expansion, and we are mixing scalars with non-scalars,
    // then we need to do a different kind of argument consistency check.
    if (_pInfo->_glslExpand != ExpandType::None && 
        TypeHelpers::IsNumericScalarType(rType) && 
        !TypeHelpers::IsNumericScalarType(lType))
    {
        _fExpandRight = true;
        _expandBasicType = lType;
    }
    else if (_pInfo->_glslExpand == ExpandType::Both && 
        TypeHelpers::IsNumericScalarType(lType) && 
        !TypeHelpers::IsNumericScalarType(rType))
    {
        _fExpandLeft = true;
        _expandBasicType = rType;
    }
    else
    {
        // By now, any allowed combinations of nonscalar and scalar have been
        // determined with the expansion code above. The only valid unequal
        // type combinations now come from algebraic multiplication, and this
        // cannot happen with scalars
        if ((_pInfo->_op == MUL_ASSIGN || _pInfo->_op == STAR) && !TypeHelpers::IsNumericScalarType(lType) && !TypeHelpers::IsNumericScalarType(rType))
        {
            // The multiply operators are not as fussy as the rest - by this stage, anything
            // will be accepted, and the question is just about if it is algebraic or not.
            _fComponentMultiply = IsComponentMultiply(lType, rType, &mulReturnType);
        }
        else
        {
            // Make sure both sides are the same
            if (lType != rType)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);            
            }
        }
    }

    // Assignment operators need lValue on left
    switch(_pInfo->_op)
    {
    case EQUAL:
    case MUL_ASSIGN:
    case DIV_ASSIGN:
    case MOD_ASSIGN:
    case ADD_ASSIGN:
    case SUB_ASSIGN:
    case RIGHT_ASSIGN:
    case LEFT_ASSIGN:
    case AND_ASSIGN:
    case OR_ASSIGN:
    case XOR_ASSIGN:
        CHK(VerifyLValue());
        break;
    }

    // Some operators (like == and !=) return boolean vectors in HLSL and need
    // to have an all() function wrap the generated operator. We do this if
    // the operator returns a boolean vector and the arguments are not scalar.
    if (_pInfo->_fHLSLBoolVector && 
        (TypeHelpers::IsVectorType(rType) || TypeHelpers::IsMatrixType(rType)) &&
        (TypeHelpers::IsVectorType(lType) || TypeHelpers::IsMatrixType(lType)))
    {
        _fWrapInAll = true;
    }

    switch(_pInfo->_returnType)
    {
    case LEFT_EXPR_TYPE:
        if (_fExpandLeft)
        {
            // We are expanding the left type, so we need to make sure we
            // return the thing that we expanded to.
            SetBasicExpressionType(_expandBasicType);
        }
        else
        {
            if (mulReturnType != NO_TYPE)
            {
                // The multiplication operator override the return type
                SetBasicExpressionType(mulReturnType);
            }
            else
            {
                // Just use the left type
                SetExpressionType(pLType);
            }
        }
        break;

    case NO_TYPE:
        // These operators are reserved so we fail verification here
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_RESERVEDOPERATOR, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);             
        break;

    default:
        SetBasicExpressionType(_pInfo->_returnType);
        break;
    }

    // If the operator modifies the left operand, then mark that side as written
    if (_pInfo->_fWriteLeft)
    {
        CHK(GetChild(0)->MarkWritten());
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    if (_fVerifiedOpOnStructTypes)
    {
        CHK(OutputHLSLForStructTypes(pOutput));
    }
    else
    {
        CHK(OutputHLSLForBasicTypes(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLForStructTypes
//
//  Synopsis:   Output HLSL for a binary operator on two expressions of
//              the same struct type.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::OutputHLSLForStructTypes(__in IStringStream* pOutput)
{
    CHK_START;

    switch (_pInfo->_op)
    {
        case EQUAL:
            // Assignment operator for structs can just be directly translated
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteString(_pInfo->_pszOpString));
            CHK(GetChild(1)->OutputHLSL(pOutput));
            break;
        case NE_OP:
            // NE_OP and EQ_OP both invoke the generated equals operator function,
            // but NE_OP must negate the answer.
            CHK(pOutput->WriteChar('!'));
            __fallthrough;
        case EQ_OP:
        {
            TSmartPointer<GLSLType> spLType;
            CHK(GetChild(0)->GetExpressionType(&spLType));

            // Instead of performing an equals operation, we call the generated equals operator
            // function which takes two structs as parameters. In this case the arguments will
            // be the left and right children, whose expression types have already been verified
            // to be of the same struct type.
            const CTypeNameIdentifierInfo* pTypeNameInfo = spLType->AsStructType()->UseTypeNameInfo();
            CHK_VERIFY(pTypeNameInfo != nullptr);

            CHK(pOutput->WriteString(pTypeNameInfo->GetHLSLEqualsFunctionName()));
            CHK(pOutput->WriteChar('('));
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteChar(','));
            CHK(GetChild(1)->OutputHLSL(pOutput));
            CHK(pOutput->WriteChar(')'));
            break;
        }
        default:
            CHK_VERIFY_MSG(false, "Unexpected operator on verified binary operator node when outputting HLSL");
            break;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLForBasicTypes
//
//  Synopsis:   Output HLSL for an operator on basic types.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::OutputHLSLForBasicTypes(__in IStringStream* pOutput)
{
    CHK_START;

    if ((_pInfo->_op != MUL_ASSIGN && _pInfo->_op != STAR) || _fComponentMultiply)
    {
        if (_pInfo->_op == XOR_OP)
        {
            Assert(!_fExpandLeft);
            Assert(!_fExpandRight);

            // The XOR operation is simulated with a != operation, and
            // we don't want the HLSL output to be unclear to either
            // people debugging it or compilers compiling it. So we
            // wrap the expressions in paren before separating them
            // with the operator, and we wrap the whole thing in paren
            // to minimize issues with precedence.

            CHK(pOutput->WriteChar('('));

            // Do the left side of the expression
            CHK(pOutput->WriteChar('('));
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteChar(')'));

            // Do the operator
            CHK(pOutput->WriteString(_pInfo->_pszOpString));

            // Do the right side of the expression
            CHK(pOutput->WriteChar('('));
            CHK(GetChild(1)->OutputHLSL(pOutput));
            CHK(pOutput->WriteChar(')'));

            CHK(pOutput->WriteChar(')'));
        }
        else
        {
            // If the operator returns a boolean vector in HLSL, then
            // we need to use all() to convert it into a boolean.
            if (_fWrapInAll)
            {
                CHK(pOutput->WriteString("all("));
            }

            // We might need to write children out in expanded form
            CHK(WriteExpanded(pOutput));

            if (_fWrapInAll)
            {
                CHK(pOutput->WriteChar(')'));
            }
        }
    }
    else
    {
        Assert(!_fExpandLeft);
        Assert(!_fExpandRight);

        // Matrix multiply operations need to reverse the order, because we are
        // using row major matrices, and therefore each of our rows is really
        // a column.
        //
        // Since the transpose of a multiply of AB is the multiply of the transpose
        // of B and A (order reverses), we keep up the transpose lie by simple
        // reversing this order.
        if (_pInfo->_op == MUL_ASSIGN)
        {
            // We have a *= and it is not per component. This needs to be done
            // using the mul operator in HLSL. Since mul is not an assignment,
            // we need to encode that too.
            CHK(pOutput->WriteChar('('));
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteString("=mul("));
            CHK(GetChild(1)->OutputHLSL(pOutput));
            CHK(pOutput->WriteChar(','));
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteString("))"));
        }
        else
        {
            // Use the mul function in HLSL
            CHK(pOutput->WriteString("mul("));
            CHK(GetChild(1)->OutputHLSL(pOutput));
            CHK(pOutput->WriteString(","));
            CHK(GetChild(0)->OutputHLSL(pOutput));
            CHK(pOutput->WriteString(")"));        
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteExpanded
//
//  Synopsis:   Helper for outputting children, expanded if need be.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::WriteExpanded(
    __in IStringStream* pOutput                 // Where to write to
    )
{
    CHK_START;

    // If we need to expand the left side, then we need to do it ourselves
    // if HLSL does not support it. HLSL does not support it if the expand
    // type is regular or right.
    if (_fExpandLeft && _pInfo->_hlslExpand != ExpandType::Both)
    {
        CHK(GetChild(0)->WriteScalarExpanded(pOutput, _expandBasicType));
    }
    else
    {
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }

    // Do the operator
    CHK(pOutput->WriteString(_pInfo->_pszOpString));

    // If we need to expand the right side, then we need to do it ourselves
    // if HLSL does not support it. HLSL does not support it if the expand
    // type is regular.
    if (_fExpandRight && _pInfo->_hlslExpand == ExpandType::None)
    {
        CHK(GetChild(1)->WriteScalarExpanded(pOutput, _expandBasicType));
    }
    else
    {
        CHK(GetChild(1)->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::IsConstExpression(
    bool fIncludeIndex,                                 // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,                 // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                     // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    (*pfIsConstantExpression) = false;
    
    bool fChild0ConstExpression;
    ConstantValue value0;
    CHK(GetChild(0)->IsConstExpression(
        fIncludeIndex, 
        &fChild0ConstExpression, 
        pValue == nullptr ? nullptr : &value0
        ));

    if (fChild0ConstExpression)
    {
        bool fChild1ConstExpression;
        ConstantValue value1;
        CHK(GetChild(1)->IsConstExpression(
            fIncludeIndex,
            &fChild1ConstExpression, 
            pValue == nullptr ? nullptr : &value1
            ));
        
        if (fChild1ConstExpression)
        {
            if (pValue != nullptr)
            {
                int v0type, v1type;
                CHK(value0.GetBasicType(&v0type));
                CHK(value1.GetBasicType(&v1type));

                if (v0type == INT_TOK && v1type == INT_TOK)
                {
                    CHK(EvaluateConstant<int>(value0, value1, INT_TOK, pValue));
                }
                else if (v0type == FLOAT_TOK && v1type == FLOAT_TOK)
                {
                    CHK(EvaluateConstant<double>(value0, value1, FLOAT_TOK, pValue));
                }
                else
                {
                    // We have a constant expression, and we are expected to return a value. Since
                    // we currently do not have constant values implemented for things that are not
                    // int or float, we cannot make this work.
                    //
                    // Right now you can hit this easily by initializing an int or a float to an
                    // expression that has two booleans.
                    //
                    // Our assumption is that if you hit this, you are going to be dealing
                    // with an incompatible type error anyway.
                    CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
                    CHK(E_GLSLERROR_KNOWNERROR);            
                }
            }

            (*pfIsConstantExpression) = true;
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsComponentMultiply
//
//  Synopsis:   Operators that do a multiply might need to do an algebraic
//              multiply rather than the HLSL default of a per component
//              multiply.
//
//              The return type depends on the types of each side of the 
//              multiply. For algebraic multiplies each side is different 
//              and the result needs calculation.
//
//-----------------------------------------------------------------------------
bool BinaryOperatorNode::IsComponentMultiply(
    int lType,                                  // Type of left side of multiply expression
    int rType,                                  // Type of right side of multiply expression
    __out int* pReturnType                      // Calculated return type
    )
{
    // Unless we find we are doing an algebraic multiply, we will use the
    // left type as the return type.
    (*pReturnType) = lType;

    if (TypeHelpers::IsNumericScalarType(lType) || TypeHelpers::IsNumericScalarType(rType))
    {
        // No algebraic multiply for scalar types
        return true;
    }

    // Left side must be a vector or a matrix if we have gotten to here
    Assert(TypeHelpers::IsMatrixType(lType) || TypeHelpers::IsVectorType(lType));

    // Matrix times vector is a vector
    if (TypeHelpers::IsMatrixType(lType) && TypeHelpers::IsVectorType(rType))
    {
        (*pReturnType) = rType;
        return false;
    }
    else if (TypeHelpers::IsMatrixType(rType))
    {
        // Right side is a matrix, but the left side is a vector or matrix. Always
        // algebraic, and the result matches the type of the left side.
        (*pReturnType) = lType;
        return false;
    }

    return true;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::GetDumpString(__in IStringStream* pOutput)
{
    Assert(_pInfo != nullptr);

    return pOutput->WriteFormat(1024, "BinaryOperatorNode Operator='%s'", _pInfo->_pszOpString);
}

//+----------------------------------------------------------------------------
//
//  Function:   EvaluateExpression
//
//  Synopsis:   Template class for evaluating expressions.
//
//-----------------------------------------------------------------------------
template<typename T>
HRESULT BinaryOperatorNode::EvaluateExpression(
    T lConst,                                           // Left side of expression
    T rConst,                                           // Right side of expression
    __out T* pConstant                                  // Calculated value
    ) const
{
    CHK_START;

    // Do the calculation
    switch (_pInfo->_op)
    {
    case STAR:
        (*pConstant) = lConst * rConst;
        break;

    case SLASH:
        // No divide by zero and avoid integer overflow for the signed value special case.
        if (rConst == 0 || (lConst == INT_MIN && rConst == -1))
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_DIVIDEORMODBYZERO, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        (*pConstant) = lConst / rConst;
        break;

    case PLUS:
        (*pConstant) = lConst + rConst;
        break;

    case DASH:
        (*pConstant) = lConst - rConst;
        break;

    default:
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
        break;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   EvaluateConstant
//
//  Synopsis:   Template class for evaluating expressions as a ConstantValue.
//
//-----------------------------------------------------------------------------
template<typename T>
HRESULT BinaryOperatorNode::EvaluateConstant(
    const ConstantValue& left,                          // Left side of expression
    const ConstantValue& right,                         // Right side of expression
    int basicType,                                      // The type enum of the constant
    __out ConstantValue* pValue                         // The value being calculated
    ) const
{
    CHK_START;

    T val0, val1;
    if (SUCCEEDED(left.GetValue(&val0)) && SUCCEEDED(right.GetValue(&val1)))
    {
        T calcValue;
        CHK(EvaluateExpression(val0, val1, &calcValue));
        pValue->SetValue(calcValue);
    }
    else
    {
        pValue->SetTypeOnly(basicType);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyLValue
//
//  Synopsis:   Verifies that the left child of this operator is an LValue. If
//              not, logs the appropriate known error.
//
//-----------------------------------------------------------------------------
HRESULT BinaryOperatorNode::VerifyLValue() const
{
    CHK_START;

    if (!GetChild(0)->IsLValue())
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDLVALUE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}
