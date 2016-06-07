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
#include "IndexSelectionNode.hxx"
#include "IStringStream.hxx"
#include "GLSLParser.hxx"
#include "GLSLSymbolTable.hxx"
#include "TypeHelpers.hxx"
#include "VariableIdentifierNode.hxx"

MtDefine(IndexSelectionNode, CGLSLParser, "IndexSelectionNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
IndexSelectionNode::IndexSelectionNode() : 
    _fIsConstIndex(false),
    _constIndex(-1)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT IndexSelectionNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pExpr,                  // The postfix expression that the field is on
    const YYLTYPE &location,                    // The location of the postfix expression
    __in ParseTreeNode* pIndexExpr              // The expression for the index
    ) 
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pExpr));
    CHK(AppendChild(pIndexExpr));

    _location = location;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verify that the thing being indexed actually can be indexed -
//              right now that just means vector types and things declared
//              as arrays.
//
//              This also verifies that the index is valid if the index is
//              a constant expression.
//
//-----------------------------------------------------------------------------
HRESULT IndexSelectionNode::VerifySelf()
{
    CHK_START;

    TSmartPointer<GLSLType> spExprType;
    CHK(GetChild(0)->GetExpressionType(&spExprType));

    // Check that the index type is an integer
    TSmartPointer<GLSLType> spIndexType;
    CHK(GetChild(1)->GetExpressionType(&spIndexType));

    int basicIndexType;
    if (FAILED(spIndexType->GetBasicType(&basicIndexType)) || basicIndexType != INT_TOK)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDINDEX, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);            
    }

    // See if we have a valid constant expression for the index
    bool fConstantValid;
    ConstantValue constIndexValue;
    CHK(GetChild(1)->IsConstExpression(/*loopindex*/false, &fConstantValid, &constIndexValue));

    if (fConstantValid && SUCCEEDED(constIndexValue.GetValue<int>(&_constIndex)))
    {
        // Constant ints are valid
        _fIsConstIndex = true;
    }
    else
    {
        TSmartPointer<CVariableIdentifierInfo> spInfo;
        if (GetParser()->GetShaderType() == GLSLShaderType::Vertex &&
            GetChild(0)->GetParseNodeType() == ParseNodeType::variableIdentifier &&
            SUCCEEDED(GetChild(0)->GetAs<VariableIdentifierNode>()->GetVariableIdentifierInfo(&spInfo)) &&
            spInfo->GetTypeQualifier() == UNIFORM
            )
        {
            // Uniforms in vertex shaders can have any integer - we already checked for
            // that above so all is good now...
        }
        else
        {
            // If not a uniform in a vertex shader, then the index needs to 
            // be a constant index expression
            bool fIsConstantIndexExpression;
            CHK(GetChild(1)->IsConstExpression(/*fIncludeIndex*/true, &fIsConstantIndexExpression, /*value*/nullptr));
            if (!fIsConstantIndexExpression)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDINDEX, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }
    }

    int exprType;
    if (SUCCEEDED(spExprType->GetBasicType(&exprType)) && TypeHelpers::IsVectorType(exprType))
    {
        if (_fIsConstIndex)
        {
            // Figure out the largest component we can legally index
            int maxIndex = TypeHelpers::GetVectorLength(exprType) - 1;

            // Check index is in the right range
            if(_constIndex < 0 || _constIndex > maxIndex)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INDEXOUTOFRANGE, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);            
            }
        }

        int componentType;
        CHK_VERIFY(SUCCEEDED(TypeHelpers::GetComponentType(exprType, &componentType)));
        SetBasicExpressionType(componentType);
    }
    else if (SUCCEEDED(spExprType->GetBasicType(&exprType)) && TypeHelpers::IsMatrixType(exprType))
    {
        // Find out what type the columns of the matrix are
        int colType = TypeHelpers::GetMatrixColumnType(exprType);

        if (_fIsConstIndex)
        {
            // Figure out the largest component we can legally index - the row count
            // and the column count are the same, so the length of the column type
            // tells you the largest column you can index.
            int colLength = TypeHelpers::GetVectorLength(colType);

            // Check index is in the right range
            if(_constIndex < 0 || _constIndex >= colLength)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INDEXOUTOFRANGE, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);            
            }
        }

        // Components of a matrix are vectors
        SetBasicExpressionType(colType);
    }
    else
    {
        // If not a vector or matrix, needs to be an array of something
        if (!spExprType->IsArrayType())
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDINDEXTARGET, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);            
        }

        if (_fIsConstIndex)
        {
            int arraySize;

            // Figure out how big of a thing we are indexing
            CHK(spExprType->GetArraySize(&arraySize));

            // Check that this thing is in range
            if(_constIndex < 0 || _constIndex >= arraySize)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INDEXOUTOFRANGE, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);            
            }
        }

        // Find out what elements are in the array
        TSmartPointer<GLSLType> spElementType;
        CHK(spExprType->GetArrayElementType(&spElementType));

        // That is the return type
        SetExpressionType(spElementType);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree. HLSL supports the same
//              syntax for indexing so this is straightforward.
//
//-----------------------------------------------------------------------------
HRESULT IndexSelectionNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    // First output the expression
    CHK(GetChild(0)->OutputHLSL(pOutput));

    // Output the braces and index
    CHK(pOutput->WriteChar('['));

    if (_fIsConstIndex)
    {
        // If we can evaluate the constant expression, do so
        CHK(pOutput->WriteFormat(128, "%d", _constIndex));
    }
    else
    {
        CHK(GetChild(1)->OutputHLSL(pOutput));
    }

    CHK(pOutput->WriteChar(']'));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MarkWritten
//
//  Synopsis:   An index selection may have children which are variables, but
//              they won't get modified. So the expression being indexed is
//              traversed, but the index is not.
//
//-----------------------------------------------------------------------------
HRESULT IndexSelectionNode::MarkWritten()
{
    return GetChild(0)->MarkWritten();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLValue
//
//  Synopsis:   Determine if this node represents a valid Lvalue. Indexing
//              an LValue gives back an LValue.
//
//-----------------------------------------------------------------------------
bool IndexSelectionNode::IsLValue() const
{
    return GetChild(0)->IsLValue();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              Since the selection by index does not affect constness,
//              the expression is tested.
//
//-----------------------------------------------------------------------------
HRESULT IndexSelectionNode::IsConstExpression(
    bool fIncludeIndex,                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue             // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    // We always pass nullptr here for the value, because even if we do get it we don't
    // yet have the logic implemented to evaluate it as part of the index selection.
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
