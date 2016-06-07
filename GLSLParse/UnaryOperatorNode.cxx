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
#include "UnaryOperatorNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "TypeHelpers.hxx"
#include "GLSLParser.hxx"

MtDefine(UnaryOperatorNode, CGLSLParser, "UnaryOperatorNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
UnaryOperatorNode::UnaryOperatorNode() :
    _op(INVALID_OP),
    _type(UnaryOperatorType::Pre)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT UnaryOperatorNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int op,                                     // The operator for the expression
    UnaryOperatorType::Enum type,               // The type of unary operator
    __in ParseTreeNode* pExpr,                  // The unary expression being operated on
    const YYLTYPE &location                     // The location of the expression
    )
{
    ParseTreeNode::Initialize(pParser);

    _op = op;
    _type = type;
    _location = location;

    return AppendChild(pExpr);
}   
    
//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Depending on the operator the type will change. This figures
//              out what is happening.
//
//              This is also where we detect reserved things and error out.
//
//-----------------------------------------------------------------------------
HRESULT UnaryOperatorNode::VerifySelf()
{
    CHK_START;

    // Get the child expression type
    TSmartPointer<GLSLType> spType;
    CHK(GetChild(0)->GetExpressionType(&spType));

    // Get the basic type from that, since we only operate on basic types
    int childType;
    if (FAILED(spType->GetBasicType(&childType)))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_UNARYOPERATORNOTDEFINEDFORTYPE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // No unary operators are defined for texture/sampler types
    if (TypeHelpers::IsTextureType(childType) || childType == VOID_TOK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_WRONGOPERANDTYPES, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // TILDE is reserved
    if (_op == TILDE)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_RESERVEDOPERATOR, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);             
    }

    if (_op == BANG)
    {
        // The BANG operator reverses a boolean expression, so the child
        // needs to be boolean and this returns boolean.
        CHKB(childType == BOOL_TOK);

        SetBasicExpressionType(BOOL_TOK);
    }
    else
    {
        // PLUS and DASH operators work on arithmetic types and they do not change the type
        CHKB(TypeHelpers::IsNumericScalarType(childType) || TypeHelpers::IsMatrixType(childType) || TypeHelpers::IsVectorType(childType));

        SetExpressionType(spType);
    }

    // Increment and decrement modify things
    if (_op == INC_OP || _op == DEC_OP)
    {
        // If you want to modify things you need to have an lvalue
        if (!GetChild(0)->IsLValue())
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDLVALUE, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        // Mark as written to
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
HRESULT UnaryOperatorNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // We need to wrap this in paren, because tokens emitted by this are vulnerable
    // to being joined to other tokens. For example, this GLSL:
    //
    // a = a - -1;
    //
    // Will create the right tree, but without paren will end up like this in HLSL:
    //
    // a = a--1;
    //
    // And the '--' token will be read instead of two '-' tokens.
    CHK(pOutput->WriteChar('('));

    // Postfix operators output the expression they operate on first
    if (_type == UnaryOperatorType::Post)
    {
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }

    switch(_op)
    {
    case PLUS:
        CHK(pOutput->WriteChar('+'));
        break;

    case DASH:
        CHK(pOutput->WriteChar('-'));
        break;

    case BANG:
        CHK(pOutput->WriteChar('!'));
        break;

    case INC_OP:
        CHK(pOutput->WriteString("++"));
        break;

    case DEC_OP:
        CHK(pOutput->WriteString("--"));
        break;

    default:
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    if (_type == UnaryOperatorType::Pre)
    {
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }

    CHK(pOutput->WriteChar(')'));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstIndexExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              Since there is only one child which is an expression, the
//              logic is deferred to that child. It is assumed that any valid
//              operator applied to this constant expression will not change
//              if it is constant, and any invalid operator for the
//              expression will be flagged during verification.
//
//-----------------------------------------------------------------------------
HRESULT UnaryOperatorNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    CHK(GetChild(0)->IsConstExpression(fIncludeIndex, pfIsConstantExpression, pValue));

    if (pValue != nullptr && *pfIsConstantExpression)
    {
        int basicType;
        CHK(pValue->GetBasicType(&basicType));

        if (basicType == INT_TOK || basicType == FLOAT_TOK)
        {
            switch (_op)
            {
            case PLUS:
                // Nothing to do
                break;

            case DASH:
                if (basicType == INT_TOK)
                {
                    NegateValue<int>(pValue, INT_TOK);
                }
                else
                {
                    NegateValue<float>(pValue, FLOAT_TOK);
                }
                break;

            default:
                // Other operators not supported - there should be no way for user input to get here
                AssertSz(false, "Unsupported unary operator in constant expression calculation");
                CHK(E_UNEXPECTED);
                break;
            }

        }
        else
        {
            pValue->SetTypeOnly(basicType);
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   NegateValue
//
//  Synopsis:   Negate the value stored in the given constant value. The type
//              given is set if we cannot get a value to negate.
//
//-----------------------------------------------------------------------------
template<typename T>
void UnaryOperatorNode::NegateValue(
    __inout ConstantValue* pValue,              // Value to negate
    int basicType                               // Type to set to if it cannot be obtained
    )
{
    T value;
    if (SUCCEEDED(pValue->GetValue(&value)))
    {
        pValue->SetValue(-value);
    }
    else
    {
        pValue->SetTypeOnly(basicType);
    }
}
