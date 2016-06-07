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
#include "GLSLFunctionSignature.hxx"
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "TypeHelpers.hxx"
#include "GLSL.tab.h"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLFunctionSignature::ParamInfo::ParamInfo()
{
    _paramQual = EMPTY_TOK;
}

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLFunctionSignature::ParamInfo::ParamInfo(__in GLSLType* pType, int paramQual)
{
    _spType = pType;
    _paramQual = paramQual;
}

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLFunctionSignature::CGLSLFunctionSignature() : 
    _signatureType(GLSLSignatureType::Normal)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   AddBasicType
//
//  Synopsis:   Called by code that is building up a signature.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::AddBasicType(int type, int paramQual)
{
    CHK_START;

    Assert(_signatureType == GLSLSignatureType::Normal || _signatureType == GLSLSignatureType::GenType);

    if (type == GENTYPE)
    {
        _signatureType = GLSLSignatureType::GenType;
    }

    TSmartPointer<GLSLType> spType;
    CHK(GLSLType::CreateFromBasicTypeToken(type, &spType));
    CHK(AddType(spType, paramQual));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddType
//
//  Synopsis:   Called by code that is building up a signature.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::AddType(__in GLSLType* pType, int paramQual)
{
    ParamInfo info(pType, paramQual);

    return _rgArgTypes.Add(info);
}

//+----------------------------------------------------------------------------
//
//  Function:   InitFromKnown
//
//  Synopsis:   Initialize from the information for a known function. All
//              known functions are assumed to lack parameter qualifiers.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::InitFromKnown(const GLSLFunctionInfo &info)
{
    CHK_START;

    if (info._sigType == GLSLSignatureType::Normal)
    {
        // Add the return type
        CHK(AddBasicType(info._type, EMPTY_TOK));

        // Add the argument types
        for (int i = 0; i < info._numArgs; i++)
        {
            CHK(AddBasicType(info._rgArgTypes[i], EMPTY_TOK));
        }
    }
    else
    {
        _signatureType = info._sigType;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SignatureMatchesArgumentTypes
//
//  Synopsis:   Match the parameters and determine the return type, along 
//              with the computed genType.
//
//              In GLSL, some builtin function signatures have
//              gentype arguments, which means that it could take a vector
//              of any length.
//
//              In this case, the arguments need to match in count, and all
//              of the gentype arguments need to be the same length. If the
//              return type is a gentype then this length determines the
//              return type.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::SignatureMatchesArgumentTypes(
    const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
    __out int* pGenType,                                            // Determined gentype
    __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
    ) const
{
    CHK_START;

    // Only MatchNormalSignature will potentially set a meaningful gentype, so
    // init the out param for all paths here.
    *pGenType = NO_TYPE;

    switch (GetSignatureType())
    {
    case GLSLSignatureType::Normal:
    case GLSLSignatureType::GenType:
        CHK(MatchNormalSignature(aryArgumentTypes, pGenType, ppReturnType));
        break;

    case GLSLSignatureType::CompareFloatVector:
    case GLSLSignatureType::CompareIntVector:
    case GLSLSignatureType::CompareBoolVector:
        CHK(MatchCompareVectorSignature(aryArgumentTypes, ppReturnType));
        break;

    case GLSLSignatureType::TestBoolVector:
        CHK(MatchTestVectorSignature(aryArgumentTypes, ppReturnType));
        break;

    default:
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MatchCompareVectorSignature
//
//  Synopsis:   Match the parameters and determine the return type when this 
//              signature is a vector relational function.
//
//              The function must take two vectors the same size, and return
//              a boolean of this same size.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::MatchCompareVectorSignature(
    const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
    __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
    ) const
{
    CHK_START;

    // Comparison functions have 2 arguments
    CHKB(aryArgumentTypes.GetCount() == 2);

    // Figure out the component type we need
    int compType = NO_TYPE;
    switch (GetSignatureType())
    {
    case GLSLSignatureType::CompareFloatVector:
        compType = FLOAT_TOK;
        break;

    case GLSLSignatureType::CompareIntVector:
        compType = INT_TOK;
        break;

    case GLSLSignatureType::CompareBoolVector:
        compType = BOOL_TOK;
        break;

    default:
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    int vecLength = -1;

    // Loop through all of the arguments, ensuring that they match
    for (UINT i = 0; i < aryArgumentTypes.GetCount(); i++)
    {
        // Get the arg basic type
        const GLSLType* pArgType = aryArgumentTypes[i];

        int argBasicType;
        CHK(pArgType->GetBasicType(&argBasicType));

        // Get the arg component type and check it
        int argCompType;
        CHK(TypeHelpers::GetComponentType(argBasicType, &argCompType));
        CHKB(argCompType == compType);

        // Get the arg length and make sure it is consistent across args
        int argLength = TypeHelpers::GetVectorLength(argBasicType);
        if (vecLength == -1)
        {
            vecLength = argLength;
        }
        else
        {
            CHKB(argLength == vecLength);
        }

        // Must be a vector
        CHKB(argLength >=2 && argLength <= 4);
    }

    int retTypes[3] = { BVEC2_TOK, BVEC3_TOK, BVEC4_TOK };

    // The conditions above should force this
    Assert(vecLength >= 2 && vecLength <= 4);

    // Create a return type from the matched basic type
    TSmartPointer<GLSLType> spReturnType;
    CHK(GLSLType::CreateFromBasicTypeToken(retTypes[vecLength - 2], &spReturnType));

    *ppReturnType = spReturnType.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MatchTestVectorSignature
//
//  Synopsis:   Match the parameters and determine the return type when this 
//              signature is a vector test function.
//
//              The function must take a boolean vector and return a boolean.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::MatchTestVectorSignature(
    const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
    __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
    ) const
{
    CHK_START;

    // Test functions have 1 argument
    CHKB(aryArgumentTypes.GetCount() == 1);

    // Get the arg basic type
    const GLSLType* pArgType = aryArgumentTypes[0];

    int argBasicType;
    CHK(pArgType->GetBasicType(&argBasicType));

    // Get the arg component type and check it
    int argCompType;
    CHK(TypeHelpers::GetComponentType(argBasicType, &argCompType));
    CHKB(argCompType == BOOL_TOK);

    // Create a return type
    CHK(GLSLType::CreateFromBasicTypeToken(BOOL_TOK, ppReturnType));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MatchNormalSignature
//
//  Synopsis:   Match the parameters and determine the return type when 
//              this signature is for a normal or gentype function.
//
//              In GLSL, some builtin function signatures have
//              gentype arguments, which means that it could take a vector
//              of any length.
//
//              In this case, the arguments need to match in count, and all
//              of the gentype arguments need to be the same length. If the
//              return type is a gentype then this length determines the
//              return type.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLFunctionSignature::MatchNormalSignature(
    const CModernArray<TSmartPointer<GLSLType>>& aryArgumentTypes,  // Arguments types to match
    __out int* pGenType,                                            // Determined gentype
    __deref_out GLSLType** ppReturnType                             // Return type if signature matched args
    ) const
{
    CHK_START;

    int genType = NO_TYPE;

    // Need to have the same argument count
    CHKB(aryArgumentTypes.GetCount() == GetArgumentCount());

    // Loop through all of the arguments, ensuring that they match
    for (UINT i = 0; i < aryArgumentTypes.GetCount(); i++)
    {
        const GLSLType* pArgType = aryArgumentTypes[i];

        // Check that the argument passed matches the argument type of the template
        int templateBasicType;
        if (SUCCEEDED(UseArgumentType(i)->GetBasicType(&templateBasicType)) && templateBasicType == GENTYPE)
        {
            // We either create a new gentype match, or verify the current one. Either way
            // we need to have a basic type now.
            int argBasicType;
            CHK(pArgType->GetBasicType(&argBasicType));

            if (genType == NO_TYPE)
            {
                // We have not matched the gentype yet, so do so now
                genType = argBasicType;
            }
            else
            {
                // We have decided what the gentype represents, all others have to fall in line
                CHKB(argBasicType == genType);
            }
        }
        else
        {
            // Specifically check for equal types - no implicit casting when calling functions is allowed
            CHKB(UseArgumentType(i)->IsEqualType(pArgType));
        }

    }

    if (GetSignatureType() == GLSLSignatureType::GenType)
    {
        // We have a gentype function but we were unable to infer what the gentype
        // mapped to. This could mean the metadata is wrong.
        CHKB(genType != NO_TYPE);

        // gentype is only allowed to be be certain types
        CHKB(TypeHelpers::IsValidGenType(genType));
    }

    // By now, we know that the arguments match. So we can determine the return type
    // that is requested, transforming it to a real type if need be. We get
    // the return type from the template return type.
    TSmartPointer<GLSLType> spReturnType = UseReturnType();

    int returnBasicType;
    if (SUCCEEDED(spReturnType->GetBasicType(&returnBasicType)) && returnBasicType == GENTYPE)
    {
        // If we have a gentype in the type list then this flag should be set
        Assert(GetSignatureType() == GLSLSignatureType::GenType);

        // Create a return type from the matched basic type
        CHK(GLSLType::CreateFromBasicTypeToken(genType, &spReturnType));

        // If this assert fires then the pTemplate had a gentype in it, which
        // makes no sense. This needs to be investigated and fixed.
        Assert(genType != GENTYPE);
    }

    *ppReturnType = spReturnType.Extract();
    *pGenType = genType;

    CHK_RETURN;
}
