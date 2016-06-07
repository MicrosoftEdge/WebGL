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
#include "OperatorInfo.hxx"
#include "TypeHelpers.hxx"
#include "GLSL.tab.h"

//+----------------------------------------------------------------------------
//
//  Function:   AreCompatibleTypes
//
//  Synopsis:   Check if the two given types are compatible with the operator.
//
//-----------------------------------------------------------------------------
bool OperatorInfo::AreCompatibleTypes(
    int lType,                                          // The basic type of left argument
    int rType                                           // The basic type of right argument
    ) const
{
    switch(_inputType)
    {
    case SCALAR_NUMERIC_TYPE:
        {
            if (TypeHelpers::IsNumericScalarType(lType) && TypeHelpers::IsNumericScalarType(rType))
            {
                return true;
            }

            return false;
        }

    case NUMERIC_TYPE:
        {
            if (TypeHelpers::IsNumericScalarType(lType) || TypeHelpers::IsMatrixType(lType) || TypeHelpers::IsVectorType(lType))
            {
                if(TypeHelpers::IsNumericScalarType(rType) || TypeHelpers::IsMatrixType(rType) || TypeHelpers::IsVectorType(rType))
                {
                    return true;
                }
            }

            return false;
        }

    case SIMPLE_TYPE:
        {
            if (TypeHelpers::IsVectorComponentType(lType) || TypeHelpers::IsMatrixType(lType) || TypeHelpers::IsVectorType(lType))
            {
                if (TypeHelpers::IsVectorComponentType(rType) || TypeHelpers::IsMatrixType(rType) || TypeHelpers::IsVectorType(rType))
                {
                    return true;
                }
            }

            return false;
        }

    default:
        if (lType == _inputType && rType == _inputType)
        {
            return true;
        }

        return false;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   GetInfo
//
//  Synopsis:   Take an operator list and operator and look up the info.
//
//-----------------------------------------------------------------------------
HRESULT OperatorInfo::GetInfo(
    __in_ecount(opCount) const OperatorInfo* pOpList,   // Operator list
    int opCount,                                        // Size of the operator list
    int op,                                             // Operator to lookup
    const OperatorInfo** ppInfo                         // The operator info
    )
{
    CHK_START;

    bool fFound = false;

    for (int i = 0; i < opCount; i++)
    {
        if (pOpList[i]._op == op)
        {
            (*ppInfo) = &pOpList[i];
            fFound = true;
            break;
        }
    }

    CHKB(fFound);

    CHK_RETURN;
}
