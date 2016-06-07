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
#include "FieldSelectionNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "GLSLSymbolTable.hxx"
#include "TypeHelpers.hxx"
#include "VariableIdentifierNode.hxx"
#include "BasicGLSLType.hxx"
#include "StructGLSLType.hxx"

MtDefine(FieldSelectionNode, CGLSLParser, "FieldSelectionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FieldSelectionNode::FieldSelectionNode() :
    _fRepeated(false),
    _numSwizzleComponents(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pExpr,                  // The expression that the field is on
    __in ParseTreeNode* pSelection,             // The field selection
    const YYLTYPE &location                     // The location of the selection
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pExpr));
    CHK(AppendChild(pSelection));

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Ensure that the field selection is valid and calculate what
//              it means. This might be done first for translation purposes
//              or to do expression typing.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::VerifySelf()
{
    CHK_START;

    TSmartPointer<GLSLType> spExprType;
    CHK(GetChild(0)->GetExpressionType(&spExprType));

    if (spExprType->IsBasicType())
    {
        // Right now, for basic types, we only support field selectors that are swizzle operators. So
        // we take the selection and try to make an array of swizzle indices out of
        // it. The expression needs to be a vector type of the appropriate size for
        // this to work.
        CHK(VerifySelectionOnBasicType(spExprType->AsBasicType()->GetBasicType()));
    }
    else if (spExprType->IsStructType())
    {
        CHK(VerifySelectionOnStructType(spExprType->AsStructType()));
    }
    else
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTYPEFORFIELDSELECTION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelectionOnBasicType
//
//  Synopsis:   Ensure that the field selection is valid for a basic type.
//              On success, sets the expression type for this node, based
//              on the result of the swizzle.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::VerifySelectionOnBasicType(int exprBasicType)
{
    CHK_START;

    // Must be a vector
    CHKB(TypeHelpers::IsVectorType(exprBasicType));

    // Figure out the largest component we can legally index. Vectors are never
    // expected to be more than 4 components long, so we have a hard check for
    // that.
    int maxIndex = TypeHelpers::GetVectorLength(exprBasicType) - 1;
    Assert(maxIndex < 4);
    CHKB(maxIndex < 4);

    // Calculate the number of components that are being sourced from the child
    // expression. Since we only support swizzle operations, and swizzles can
    // never return vectors larger than 4 components, we can check for that.
    VariableIdentifierNode* pSelectionIdentifier = GetVariableIdentifierNodeChild();
    const char* pszSelectionText = pSelectionIdentifier->GetText();
    _numSwizzleComponents = ::strlen(pszSelectionText);

    // Give an error for the component count being too long if we encounter
    // that condition. Again, GLSL vectors never exceed 4 components so that
    // is what we check against.
    if (_numSwizzleComponents > 4)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_TOOMANYSWIZZLECOMPONENTS, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // We should not end up with a zero length unless our parser went haywire,
    // but ensuring this here is cheap and will make static analysis happy.
    CHK_VERIFY(IsSwizzleSelection());

    // Keep track of how many of each component was used here. We know that we
    // will never need more than 4 UINTs to track this, since we loop through
    // the number of components later, and we have already clamped that to 4.
    UINT rguCount[4] = { 0 };

    ComponentNames::Enum firstSet = ComponentNames::Xyzw;

    // Translate each component, ensuring that they are in the same set as the
    // first component, and keep track of how many times each component is
    // referenced in the string.
    for (int i = 0; i < _numSwizzleComponents; i++)
    {
        ComponentNames::Enum set;

        // Get the numeric index and the set it is from for the given index
        if (FAILED(InterpretComponent(pszSelectionText[i], &set, &_rgIndices[i])))
        {
            // We did not understand what component this was
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDSWIZZLECOMPONENT, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (i == 0)
        {
            firstSet = set;
        }
        else
        {
            if (set != firstSet)
            {
                // You cannot mix and match different sets
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }

        if (_rgIndices[i] > maxIndex)
        {
            // You cannot index something out of the range of the thing you are indexing
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        // Increment count for the component found this time around
        rguCount[_rgIndices[i]]++;
    }

    // See if any indices are repeated
    for (int i = 0 ; i <= maxIndex; i++)
    {
        if (rguCount[i] > 1)
        {
            _fRepeated = true;
            break;
        }
    }

    // Find out what the type of the components is
    int cType;
    CHK(TypeHelpers::GetComponentType(exprBasicType, &cType));

    // The type we will return is a vector of those components with our new length
    int type;
    CHK(TypeHelpers::GetVectorType(cType, _numSwizzleComponents, &type));

    SetBasicExpressionType(type);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelectionOnStructType
//
//  Synopsis:   Ensure that the field selection is valid for a struct type.
//              On success, sets the expression type for this node, based
//              on the field that was selected.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::VerifySelectionOnStructType(__in const StructGLSLType* pExprType)
{
    CHK_START;

    // For struct types, lookup the field name's identifier info
    VariableIdentifierNode* pSelectionIdentifier = GetVariableIdentifierNodeChild();
    TSmartPointer<CVariableIdentifierInfo> spVariableInfo;
    if (pExprType->GetFieldInfoForVariable(pSelectionIdentifier, &spVariableInfo) != S_OK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDSTRUCTFIELDNAME, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // Based on the found info, propagate the field's type to this node's expression type.
    // For example in the field selection 'a.b', the expression type is the type of 'b'.
    TSmartPointer<GLSLType> spFieldType;
    CHK(spVariableInfo->GetExpressionType(&spFieldType));
    SetExpressionType(spFieldType);

    // And set the identifier info on the variable node. It doesn't have enough context to
    // do this on its own.
    pSelectionIdentifier->SetVariableIdentifierInfo(spVariableInfo);

    AssertSz(!IsSwizzleSelection(), "FieldSelection should not be considered a swizzle right after we selected a field from a struct");

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InterpretComponent
//
//  Synopsis:   For a char in a selection, figure out the index of the vector
//              field that it is selecting and what set it comes from.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::InterpretComponent(
    char c,                                     // Component to interpret
    __out ComponentNames::Enum* peSet,          // What set it is in
    __out_range(0, 3) int* pIndex               // The index in that set
    )
{
    switch(c)
    {
    case 'x': (*peSet) = ComponentNames::Xyzw; (*pIndex) = 0; break;
    case 'y': (*peSet) = ComponentNames::Xyzw; (*pIndex) = 1; break;
    case 'z': (*peSet) = ComponentNames::Xyzw; (*pIndex) = 2; break;
    case 'w': (*peSet) = ComponentNames::Xyzw; (*pIndex) = 3; break;
    case 'r': (*peSet) = ComponentNames::Rgba; (*pIndex) = 0; break;
    case 'g': (*peSet) = ComponentNames::Rgba; (*pIndex) = 1; break;
    case 'b': (*peSet) = ComponentNames::Rgba; (*pIndex) = 2; break;
    case 'a': (*peSet) = ComponentNames::Rgba; (*pIndex) = 3; break;
    case 's': (*peSet) = ComponentNames::Stpq; (*pIndex) = 0; break;
    case 't': (*peSet) = ComponentNames::Stpq; (*pIndex) = 1; break;
    case 'p': (*peSet) = ComponentNames::Stpq; (*pIndex) = 2; break;
    case 'q': (*peSet) = ComponentNames::Stpq; (*pIndex) = 3; break;
    default:
        return E_INVALIDARG;
    }

    return S_OK;
}


//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    CHK(GetChild(0)->OutputHLSL(pOutput));
    CHK(pOutput->WriteChar('.'));

    if (IsSwizzleSelection())
    {
        // Output the components always using xyzw
        for (int i = 0; i < _numSwizzleComponents; i++)
        {
            const static char s_comp[4] = { 'x', 'y', 'z', 'w' };

            // The verification phase should never generate something out of range
            // and let us get here, so this assert means that something is very
            // wrong with verification.
            __analysis_assume_and_assert(_rgIndices[i] >= 0 && _rgIndices[i] <= 3);
            CHK(pOutput->WriteChar(s_comp[_rgIndices[i]]));
        }
    }
    else
    {
        CHK(GetVariableIdentifierNodeChild()->OutputHLSL(pOutput));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLValue
//
//  Synopsis:   Determine if this node represents a valid lvalue.
//
//              Selecting a field of an lvalue is an lvalue, provided the
//              swizzle does not repeat.
//
//-----------------------------------------------------------------------------
bool FieldSelectionNode::IsLValue() const
{
    // Nobody should call this until we are verified or the information will be wrong
    Assert(IsVerified());

    // Child needs to be an lvalue and no swizzle repeats
    bool fIsSwizzleRepeat = (IsSwizzleSelection() && _fRepeated);
    if (GetChild(0)->IsLValue() && !fIsSwizzleRepeat)
    {
        return true;
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVariableIdentifierNodeChild
//
//  Synopsis:   Returns the variable identifier node child
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* FieldSelectionNode::GetVariableIdentifierNodeChild() const
{
    return GetChild(1)->GetAs<VariableIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              Since the selection of a field does not affect constness,
//              the expression is tested.
//
//-----------------------------------------------------------------------------
HRESULT FieldSelectionNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    // We always pass nullptr here for the value, because even if we do get it we don't
    // yet have the logic implemented to evaluate it as part of the field selection.
    CHK(GetChild(0)->IsConstExpression(fIncludeIndex, pfIsConstantExpression, /*value*/nullptr));

    if (pValue != nullptr && *pfIsConstantExpression)
    {
        TSmartPointer<GLSLType> spType;
        CHK(GetExpressionType(&spType));

        int basicType;
        CHK(spType->GetBasicType(&basicType));

        // Set a type only value, so that somebody can at least inspect the type of
        // this value. If they try to get the actual value it will fail.
        pValue->SetTypeOnly(basicType);
    }

    CHK_RETURN;
}
