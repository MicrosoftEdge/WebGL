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
#include "VerificationHelpers.hxx"
#include "ParseTreeNode.hxx"
#include "GLSLParser.hxx"
#include "GLSLType.hxx"
#include "FunctionIdentifierInfo.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "FunctionIdentifierNode.hxx"

//+-----------------------------------------------------------------------------
//
//  Function:   VerifyArrayConstant
//
//  Synopsis:   Checks the parse tree node for an int constant. If it is a
//              valid constant, sets the constant size in the out param,
//              otherwise logs the appropriate known error and returns a
//              failed HRESULT.
//
//------------------------------------------------------------------------------
HRESULT VerificationHelpers::VerifyArrayConstant(
    __in ParseTreeNode* pConstantExpression,            // Expression to retrieve int constant from 
    __in CGLSLParser* pParser,                          // Parser that we're verifying for
    __in const YYLTYPE* pDeclaratorLocation,            // Location of declarator the array size applies to
    __out int* pArraySizeConstant                       // Verified array size constant
    )
{
    CHK_START;

    ConstantValue arraySize;
    bool fConstantValid;
    CHK(pConstantExpression->IsConstExpression(
        false,                                          // Don't consider loop index variables as constants for this
        &fConstantValid, 
        &arraySize
        ));

    if (!fConstantValid)
    {
        // Arrays have to have a constant size
        CHK(pParser->LogError(pDeclaratorLocation, E_GLSLERROR_ARRAYDECLARATORNOTCONST, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    int arraySizeType;
    if (FAILED(arraySize.GetBasicType(&arraySizeType)) || arraySizeType != INT_TOK)
    {
        // Arrays need to have an integer size (no floats allowed)
        CHK(pParser->LogError(pDeclaratorLocation, E_GLSLERROR_ARRAYSIZENOTINT, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);        
    }

    int arraySizeValue;
    if (FAILED(arraySize.GetValue<int>(&arraySizeValue)))
    {
        // Cannot calculate array size, which means we cannot do bounds checks
        CHK(pParser->LogError(pDeclaratorLocation, E_GLSLERROR_CANNOTCALCARRAYSIZE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);        
    }

    if (arraySizeValue <= 0)
    {
        // Arrays have to be nonzero positive size
        CHK(pParser->LogError(pDeclaratorLocation, E_GLSLERROR_ARRAYSIZELEQZERO, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    *pArraySizeConstant = arraySizeValue;

    CHK_RETURN;
}

//+-----------------------------------------------------------------------------
//
//  Function:   VerifyNewFunctionInfoAgainstMatchedInfo
//
//  Synopsis:   Given a new CFunctionIdentifierInfo and another CFunctionIdentifierInfo
//              that it matched against, determines whether this is an
//              allowable match (return types equal, not known function, 
//              qualifers equivalent). Logs known errors and returns a failure
//              HRESULT in the case where all conditions are not satisfied.
//
//------------------------------------------------------------------------------
HRESULT VerificationHelpers::VerifyNewFunctionInfoAgainstMatchedInfo(
    __in const CFunctionIdentifierInfo* pNewInfo,                   // New function info to verify
    __in const CFunctionIdentifierInfo* pMatchedInfo,               // Matched function info to verify against
    __in const GLSLType* pReturnType,                               // Return type determine when matching signature
    __in const FunctionHeaderWithParametersNode* pFunctionHeader,   // Function header being declared/defined
    __in const FunctionIdentifierNode* pIdentifier                  // Function identifier of added info
    )
{
    CHK_START;

    CGLSLParser* pParser = pIdentifier->GetParser();

    // You cannot overload on return type. This means the declared/defined return type
    // must match the one from the matched signature.
    if (!pReturnType->IsEqualType(pNewInfo->GetSignature().UseReturnType()))
    {
        // 'main' is special - it's "known", internally but is not a built-in function.
        // As such it technically isn't being overloaded here.  
        // We'll output a different error for this in order to give developers clarity.
        if (pNewInfo->IsGLSLSymbol(GLSLSymbols::main))
        {
            CHK(pParser->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_REDEFINEMAIN, nullptr));
        }
        else
        {
            CHK(pParser->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_FUNCOVERLOADONRETURNTYPE, nullptr));
        }

        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // You cannot redefine/redeclare a known function.
    if (pMatchedInfo->IsKnownFunction())
    {
        CHK(pParser->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // The declared/defined function parameter qualifiers must match the found function signature
    if (!pFunctionHeader->ParameterQualifiersMatchSignature(pMatchedInfo->GetSignature()))
    {
        CHK(pParser->LogError(&pIdentifier->GetLocation(), E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}
