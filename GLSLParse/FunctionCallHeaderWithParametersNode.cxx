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
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "FunctionIdentifierNode.hxx"
#include "BasicTypeNode.hxx"
#include "IStringStream.hxx"
#include "IdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "FunctionCallHeaderNode.hxx"
#include "FunctionHeaderNode.hxx"
#include "GLSLType.hxx"
#include "GLSLTypeInfo.hxx"
#include "TypeHelpers.hxx"
#include "KnownSymbols.hxx"
#include "CollectionNodeWithScope.hxx"
#include "GLSLError.hxx"
#include "FunctionCallIdentifierNode.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "StructGLSLType.hxx"

MtDefine(FunctionCallHeaderWithParametersNode, CGLSLParser, "FunctionCallHeaderWithParametersNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
FunctionCallHeaderWithParametersNode::FunctionCallHeaderWithParametersNode() : 
    _functionCallType(FunctionCallType::normal),
    _expectedConstructorArgCount(1),
    _lastArgTruncateCount(0),
    _fHasSignature(false),
    _genType(NO_TYPE),
    _basicConstructType(NO_TYPE)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pHeader,                // The header
    __in_opt ParseTreeNode* pArg,               // The first argument / expression
    const YYLTYPE &location                     // The location of the function call
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    _location = location;

    CHK(AppendChild(pHeader));

    if (pArg != nullptr)
    {
        CHK(AppendChild(pArg));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Verifies that the typing information has been figured out. Any
//              method that looks for typing information should call this first
//              before looking at the typing information.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifySelf()
{
    CHK_START;

    // A function call can be a normal function call or a constructor.
    // If the call is via an identifier, the identifier is either the name
    // of a function, or a struct constructor. Basic type constructors (e.g.
    // bool, mat3, etc.) will not have a function call identifier, but rather 
    // a BasicTypeNode that indicates which basic type is being constructed.
    FunctionCallIdentifierNode* pFuncCallId = GetFunctionCallIdentifier();
    if (pFuncCallId != nullptr)
    {
        // If we have an identifier for the function, that identifier should have
        // determined what type of identifier it was when it was verified (either
        // struct constructor or a normal function call). These go through
        // separate codepaths, since this node must lookup function calls by signature.
        FunctionIdentifierNode* pFuncId = pFuncCallId->GetFunctionIdentifierChild();
        if (pFuncId != nullptr)
        {
            CHK(VerifyNonConstructor(pFuncId));
        }
        else
        {
            TypeNameIdentifierNode* pTypeNameId = pFuncCallId->GetTypeNameIdentifierChild();
            CHK(VerifyStructConstructor(pTypeNameId));
        }
    }
    else
    {
        // This must be a constructor call - builtin constructors require at least one argument
        if (GetArgumentCount() == 0)
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
        
        _functionCallType = FunctionCallType::constructor;

        // Gather child nodes needed to figure out what is going on
        FunctionCallHeaderNode* pHeaderNode = GetChild(0)->GetAs<FunctionCallHeaderNode>();

        BasicTypeNode* pBasicTypeNode = pHeaderNode->GetChild(0)->GetAs<BasicTypeNode>();

        // Get the type information for the type we are constructing
        TSmartPointer<GLSLType> spFunctionType;
        CHK(pBasicTypeNode->GetType(&spFunctionType));

        // If BasicTypeNodes are returning types that are not basic... we have a problem.
        CHK_VERIFY(spFunctionType->GetBasicType(&_basicConstructType) == S_OK);

        // Only certain basic type can be used as a constructor
        if (!TypeHelpers::IsConstructableType(_basicConstructType))
        {
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_TYPENOTCONSTRUCTABLE, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        const BasicGLSLTypeInfo* pConstructTypeInfo;
        CHK(BasicGLSLTypeInfo::GetInfo(_basicConstructType, &pConstructTypeInfo));

        // Record the expected component count for the constructor
        _expectedConstructorArgCount = pConstructTypeInfo->_constructorComponents;

        // That indicates the expression type for the call
        SetBasicExpressionType(_basicConstructType);

        // Do special verification per type
        if (TypeHelpers::IsMatrixType(_basicConstructType))
        {
            CHK(VerifyMatrixConstructor());
        }
        else if (TypeHelpers::IsVectorType(_basicConstructType))
        {
            CHK(VerifyVectorConstructor());
        }
        else
        {
            CHK(VerifyTensorComponentConstructor(/*fForMatrixConstructor*/false));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyNonConstructor
//
//  Synopsis:   Perform verification for non-constructor function calls.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifyNonConstructor(__in FunctionIdentifierNode* pFuncId)
{
    CHK_START;

    // Get the info for the identifier
    CModernArray<TSmartPointer<IIdentifierInfo>> rgInfos;
    if (FAILED(CollectionNodeWithScope::GetNearestScopeInfoList(pFuncId, rgInfos)))
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_UNDECLAREDIDENTIFIER, pFuncId->GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);             
    }

    // Gather all the argument children types. The arg types will be used to 
    // match the correct function to call in the loop below.
    CModernArray<TSmartPointer<GLSLType>> aryArgTypes;
    CHK(GetArgumentTypes(aryArgTypes));

    bool fFoundMatch = false;
    for (UINT i = 0; i < rgInfos.GetCount(); i++)
    {
        CFunctionIdentifierInfo* pFuncInfo = rgInfos[i]->AsFunction();
        if (pFuncInfo != nullptr)
        {
            // Cannot call main yourself
            if (pFuncInfo->IsGLSLSymbol(GLSLSymbols::main))
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_CALLMAIN, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            // Get the signature information from that identifier and tell it to fill in the return type -
            // if it succeeds, that means that we have found the right function. If it fails, keep looking
            // for an overload that succeeds.
            int genType;
            TSmartPointer<GLSLType> spReturnType;
            if (SUCCEEDED(pFuncInfo->GetSignature().SignatureMatchesArgumentTypes(aryArgTypes, &genType, &spReturnType)))
            {
                // We found a function that matched. Before finalizing the return type and function identifier
                // info, ensure the args are valid for the found parameter qualifiers.
                CHK(ValidateArgumentsWithParameterQualifiers(pFuncInfo->GetSignature()));

                // Set the found genType and return type for this matched function
                _genType = genType;
                SetExpressionType(spReturnType);

                // This is a regular function call
                _functionCallType =  FunctionCallType::normal;

                // Set the identifier info for the identifier, because it could be ambiguous for
                // function calls like this one.
                pFuncId->SetFunctionIdentifierInfo(pFuncInfo);

                // Mark the function as called
                pFuncInfo->SetCalled();

                // Let the parser know what HLSL function equivalent (if any) is being called
                GetParser()->AddHLSLFunctionUsage(pFuncInfo->GetHLSLFunction());
                _fHasSignature = true;
                fFoundMatch = true;

                break;
            }
        }
    }

    // We need to find a match or we have a verification failure
    if (!fFoundMatch)
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDARGUMENTS, pFuncId->GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);             
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyStructConstructor
//
//  Synopsis:   Perform verification for user defined type constructor 
//              function calls.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifyStructConstructor(__in TypeNameIdentifierNode* pTypeNameId)
{
    CHK_START;
    
    // Gather the types for each of the arguments
    CModernArray<TSmartPointer<GLSLType>> aryTypes;
    CHK(GetArgumentTypes(aryTypes));

    TSmartPointer<CTypeNameIdentifierInfo> spTypeNameInfo;
    CHK_VERIFY(SUCCEEDED(pTypeNameId->GetTypeNameIdentifierInfo(&spTypeNameInfo)));

    TSmartPointer<GLSLType> spType;
    CHK(spTypeNameInfo->GetType(&spType));
    // Types we get back from CTypeNameIdentifierInfo should always be struct types
    CHK_VERIFY(spType->IsStructType());

    // Ensure that the arguments' types match the struct type's fields
    if (spType->AsStructType()->TypesMatchForConstructor(aryTypes))
    {
        // If so, we know this is a constructor call
        _functionCallType = FunctionCallType::constructor;

        // Notify the typename info that someone has used it as a constructor
        spType->AsStructType()->SetConstructorUsed();

        // HLSL doesn't have constructors, so let the typename identifier know that it
        // needs to output itself as a constructor name, not the typename
        pTypeNameId->SetOutputAsConstructor();

        // This function call expression type is the type of the constructor.
        SetExpressionType(spType);
    }
    else
    {
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyMatrixConstructor
//
//  Synopsis:   Perform verification for matrix constructor function calls.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifyMatrixConstructor()
{
    CHK_START;

    CHK_VERIFY(GetArgumentCount() > 0);

    int basicFirstParamType;
    TSmartPointer<GLSLType> spFirstParamType;
    CHK(GetChild(1)->GetExpressionType(&spFirstParamType));

    if (FAILED(spFirstParamType->GetBasicType(&basicFirstParamType)))
    {
        // You can only construct from basic types
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDMATCTORTYPE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // We separate out single argument constructors from the rest, with the exclusion of
    // when a single vector is passed in. We treat that like a regular constructor since
    // the checking involved is the same.
    if (GetArgumentCount() == 1 && !TypeHelpers::IsVectorType(basicFirstParamType))
    {
        if (TypeHelpers::IsVectorComponentType(basicFirstParamType))
        {
            // A vector component activates the path to create from a scalar
            GetParser()->AddFeatureUsage(FeatureUsedFlags::GLSLmatrixFromScalar);
            _functionCallType = FunctionCallType::matrixConstructorFromScalar;
        }
        else if (TypeHelpers::IsMatrixType(basicFirstParamType))
        {
            // Creating a matrix from a matrix always works
            if (basicFirstParamType != _basicConstructType)
            {
                // Creating from a different size needs our wrappers
                GetParser()->AddFeatureUsage(FeatureUsedFlags::GLSLmatrixFromMatrix);
                _functionCallType = FunctionCallType::matrixConstructorFromMatrix;
            }
            else
            {
                // Creating from the same type is just a regular constructor call
                _functionCallType = FunctionCallType::constructor;
            }
        }
        else
        {
            // Basic types must be either a vector component or matrix if
            // they are not a vector. We should not be able to get here.
            AssertSz(false, "Unexpected type in matrix constructor");
            CHK(E_UNEXPECTED);
        }
    }
    else
    {
        // If we get here, then we are not passing a scalar to create a diagonal
        // matrix and we are not creating from another matrix. The only possibility
        // left is to pass a combination of scalars and vectors that constitute
        // all of the components.
        //
        // HLSL allows the same construction - we just have to transpose it since
        // it creates from rows rather than columns. So all we have to do to
        // verify is ensure that the number of components given matches the right
        // count, and that the type of the arguments is valid.
        CHK(VerifyTensorComponentConstructor(/*fForMatrixConstructor*/true));

        _functionCallType = FunctionCallType::matrixConstructorFromComponents;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyVectorConstructor
//
//  Synopsis:   Perform verification for vector constructor function calls.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifyVectorConstructor()
{
    CHK_START;

    bool fVerifiedConstructor = false;
    if (GetArgumentCount() == 1)
    {
        // Get the type of the argument
        TSmartPointer<GLSLType> spParamType;
        CHK(GetChild(1)->GetExpressionType(&spParamType));

        int basicArgType;
        if (FAILED(spParamType->GetBasicType(&basicArgType)))
        {
            // You can only construct from basic types
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDVECCTOR, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (TypeHelpers::IsVectorComponentType(basicArgType))
        {
            // Somebody has passed a single argument for a constructor, but the constructor expected more, and the
            // type of the argument is the same type that they expect the large number of. HLSL won't allow this
            // if we output it, so we need to repeat this argument.
            _functionCallType = FunctionCallType::vectorConstructorFromScalar;

            fVerifiedConstructor = true;
        }
        else if (_basicConstructType == VEC4 && basicArgType == MAT2_TOK)
        {
            // It is legal to construct a vec4 from a mat2
            GetParser()->AddFeatureUsage(FeatureUsedFlags::GLSLvectorFromMatrix);
            _functionCallType = FunctionCallType::vectorConstructorFromMatrix;

            fVerifiedConstructor = true;
        }
    }

    if (!fVerifiedConstructor)
    {
        // We are constructing a vector from a non scalar value(s). All we have to do to
        // verify is ensure that the number of components given matches the right
        // count, and that the type of the arguments is valid.
        CHK(VerifyTensorComponentConstructor(/*fForMatrixConstructor*/false));
    }

    CHK_RETURN;
}
    
//+----------------------------------------------------------------------------
//
//  Function:   VerifyTensorComponentConstructor
//
//  Synopsis:   Perform verification for vector and matrix constructor function 
//              calls that construct with a collection of components.
//
//              This function ensures that the total number of components
//              passed in matches the number of components expected to
//              construct the given type.
//
//              It also ensures that only vectors or their components are used
//              to construct things.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::VerifyTensorComponentConstructor(bool fForMatrixConstructor)
{
    CHK_START;

    UINT uTotalComponents = 0;
    const UINT cChildren = GetChildCount();
    for (UINT i = 1; i < cChildren; i++)
    {
        // Get the type of the argument
        TSmartPointer<GLSLType> spParamType;
        CHK(GetChild(i)->GetExpressionType(&spParamType));

        int basicArgType;
        if (FAILED(spParamType->GetBasicType(&basicArgType)))
        {
            // You can only construct from basic types
            CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        UINT uAdditionalComponents = 0;
        CHK(GetComponentCountForConstructorArgType(basicArgType, fForMatrixConstructor, &uAdditionalComponents));

        bool fLastArg = (i == cChildren - 1);
        if (!fLastArg)
        {
            // There's at least one more argument, but we've exhausted the available component count:
            // this is a compile error.
            if (uTotalComponents + uAdditionalComponents >= _expectedConstructorArgCount)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }
        else
        {
            // Last argument must fill in at least the expected component count
            if (uTotalComponents + uAdditionalComponents < _expectedConstructorArgCount)
            {
                CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            // If the last argument contains more components than available, we will truncate it. Record
            // this by setting the _lastArgTruncateCount member.
            if (uTotalComponents + uAdditionalComponents > _expectedConstructorArgCount)
            {
                _lastArgTruncateCount = _expectedConstructorArgCount - uTotalComponents;
            }
        }

        uTotalComponents += uAdditionalComponents;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetComponentCountForConstructorArgType
//
//  Synopsis:   Given the basic type for an argument, returns the number
//              of components the argument has. Logs a known error if the
//              type is not allowed for builtin constructors.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::GetComponentCountForConstructorArgType(int basicArgType, bool fForMatrixConstructor, __out UINT* puComponents)
{
    CHK_START;

    UINT uComponents = 0;
    if (TypeHelpers::IsVectorType(basicArgType))
    {
        uComponents = TypeHelpers::GetVectorLength(basicArgType);
    }
    else if (!fForMatrixConstructor && TypeHelpers::IsMatrixType(basicArgType))
    {
        // Matrix type arguments are only allowed for non-matrix constructors.
        uComponents = TypeHelpers::GetNumMatrixComponents(basicArgType);
    }
    else if (TypeHelpers::IsVectorComponentType(basicArgType))
    {
        uComponents = 1;
    }
    else
    {
        // Invalid type for this kind of construction
        CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    *puComponents = uComponents;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    UINT uMatrixLength = 0;

    if (_functionCallType == FunctionCallType::matrixConstructorFromScalar ||
        _functionCallType == FunctionCallType::matrixConstructorFromMatrix)
    {
        uMatrixLength = TypeHelpers::GetMatrixLength(_basicConstructType);
        AssertSz(uMatrixLength >= 2 && uMatrixLength <= 4, "Unexpected matrix length");
    }

    // Output the header
    switch(_functionCallType)
    {
    case FunctionCallType::matrixConstructorFromScalar:
        CHK(pOutput->WriteFormat(64, "GLSLmatrix%uFromScalar(float(", uMatrixLength));
        break;

    case FunctionCallType::matrixConstructorFromMatrix:
        {
            TSmartPointer<GLSLType> spArgType;
            CHK(GetChild(1)->GetExpressionType(&spArgType));

            int basicArgType;
            CHK_VERIFY(SUCCEEDED(spArgType->GetBasicType(&basicArgType)));

            UINT uArgLength = TypeHelpers::GetMatrixLength(basicArgType);
            AssertSz(uArgLength >= 2 && uArgLength <= 4, "Unexpected matrix length");

            CHK(pOutput->WriteFormat(64, "GLSLmatrix%uFromMatrix%u(", uMatrixLength, uArgLength));
        }
        break;

    case FunctionCallType::vectorConstructorFromMatrix:
        CHK(pOutput->WriteString("GLSLvectorFromMatrix("));
        break;

    default:
        // Spit out the header that is equivalent to the GLSL one. This will generate a
        // '(' just like the other things, but a parent node of this one will close it too.
        CHK(GetChild(0)->OutputHLSL(pOutput));
        break;
    }

    // Output the arguments
    switch (_functionCallType)
    {
    case FunctionCallType::vectorConstructorFromScalar:
        {
            // During verification we determined that there was a single value passed to a
            // constructor that expected more. So we vectorize the argument.
            Assert(GetArgumentCount() == 1);

            // Write the only argument we have as a vector of the right length
            CHK(GetChild(1)->WriteScalarAsVector(pOutput, _expectedConstructorArgCount));
        }
        break;

    default:
        {
            TSmartPointer<CFunctionIdentifierInfo> spInfo;
            HLSLFunctions::Enum hlslFunction = HLSLFunctions::count;

            // Not all function calls have an identifier - check those that do for being known things
            FunctionIdentifierNode* pFuncId = GetFunctionIdentifier();
            if (pFuncId != nullptr)
            {
                CHK(pFuncId->GetFunctionIdentifierInfo(&spInfo));

                hlslFunction = spInfo->GetHLSLFunction();
            }

            // Do all of the arguments
            for (UINT i = 1; i < GetChildCount(); i++)
            {
                // For all but the first parameter, we need a comma before doing the next
                if (i != 1)
                {
                    // Write the arg as is
                    CHK(pOutput->WriteChar(','));
                }

                CHK(OutputFunctionCallArgument(pOutput, i, hlslFunction));
            }
        }
        break;
    }

    if (_functionCallType == FunctionCallType::matrixConstructorFromScalar)
    {
        // The float constructor call in matrixConstructorFromScalar requires a closing
        // paren to be added.
        CHK(pOutput->WriteString(")"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputFunctionCallArgument
//
//  Synopsis:   Output the given child's translation, based on the state
//              of this function call.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::OutputFunctionCallArgument(
    __in IStringStream* pOutput,                        // Stream to write output to
    UINT iChild,                                        // Child to output
    HLSLFunctions::Enum hlslFunction                    // Known hlsl function call that the arg is participating in
    )
{
    CHK_START;

    Assert(iChild != 0 && iChild < GetChildCount());
    
    TSmartPointer<GLSLType> spArgType;
    CHK(GetChild(iChild)->GetExpressionType(&spArgType));

    bool fLastArg = (iChild == GetChildCount() - 1);
    if (!fLastArg || _lastArgTruncateCount == 0)
    {
        // If this is not the last argument, or we're not truncating the last argument, just
        // output the child node as we normally would.
        int basicArgType;
        if (
            hlslFunction != HLSLFunctions::count &&                                                         // We are calling a function with an HLSL equivalent
            GLSLKnownSymbols::GetKnownInfo<HLSLFunctionInfo>(hlslFunction)._rgArgTypes[iChild - 1] == GENTYPE && // This argument in HLSL expects a GENTYPE
            SUCCEEDED(spArgType->GetBasicType(&basicArgType)) &&                                            // Get the basic type from that
            basicArgType == FLOAT_TOK &&                                                                    // See if it is a float
            _genType != FLOAT_TOK                                                                           // Don't do this special business if the type that matched gentype turned out to be float
            )
        {
            // Write the argument out as a vector:
            // Look at the matched signature to see how big gentype turned out to be
            int compCount = TypeHelpers::GetVectorLength(_genType);
            CHKB(compCount > 0 && compCount <= 4);

            CHK(GetChild(iChild)->WriteScalarAsVector(pOutput, compCount));
        }
        else if (
            spArgType->IsTypeOrArrayOfType(SAMPLER2D) || 
            spArgType->IsTypeOrArrayOfType(SAMPLERCUBE)
            )
        {
            // The function takes a sampler here, so the HLSL function will have both
            // a HLSL sampler and a HLSL texture passed in its place.
            CHK(GetChild(iChild)->SetHLSLNameIndex(0));
            CHK(GetChild(iChild)->OutputHLSL(pOutput));

            CHK(pOutput->WriteChar(','));

            CHK(GetChild(iChild)->SetHLSLNameIndex(1));
            CHK(GetChild(iChild)->OutputHLSL(pOutput));
        }
        else
        {
            CHK(GetChild(iChild)->OutputHLSL(pOutput));
        }
    }
    else
    {
        // The last argument should only be truncated for certain function call types.
        Assert(_functionCallType == FunctionCallType::constructor ||
            _functionCallType == FunctionCallType::matrixConstructorFromComponents
            );

        // If we're in a situation where we have a truncated argument, verification
        // code has ensured the argument is a basic type (in fact that it is a basic
        // type that is allowable as a parameter to a constructor).
        int basicArgType;
        CHK_VERIFY(SUCCEEDED(spArgType->GetBasicType(&basicArgType)));

        if (TypeHelpers::IsMatrixType(basicArgType))
        {
            CHK(GetChild(iChild)->WriteMatrixTruncated(pOutput, _lastArgTruncateCount));
        }
        else
        {
            Assert(TypeHelpers::IsVectorType(basicArgType));
            // During verification we determined that the constructor expected a smaller 
            // sized argument for the last argument. So we truncate it by swizzling.
            CHK(GetChild(iChild)->WriteVectorTruncated(pOutput, _lastArgTruncateCount));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFunctionCallIdentifier
//
//  Synopsis:   Get the function call identifier tree node.
//
//-----------------------------------------------------------------------------
FunctionCallIdentifierNode* FunctionCallHeaderWithParametersNode::GetFunctionCallIdentifier() const
{
    FunctionCallHeaderNode* pHeaderNode = GetChild(0)->GetAs<FunctionCallHeaderNode>();
    if (pHeaderNode->GetChild(0)->GetParseNodeType() == ParseNodeType::functionCallIdentifier)
    {
        return pHeaderNode->GetChild(0)->GetAs<FunctionCallIdentifierNode>();
    }

    return nullptr;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFunctionIdentifier
//
//  Synopsis:   Get the function identifier tree node.
//
//-----------------------------------------------------------------------------
FunctionIdentifierNode* FunctionCallHeaderWithParametersNode::GetFunctionIdentifier() const
{
    FunctionCallIdentifierNode* pFunctionCallId = GetFunctionCallIdentifier();
    if (pFunctionCallId != nullptr)
    {
        return pFunctionCallId->GetFunctionIdentifierChild();
    }

    return nullptr;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstIndexExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//              This is only true for constructor function calls with all
//              constant arguments.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::IsConstExpression(
    bool fIncludeIndex,                                 // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,                 // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                     // The value of the constant expression, if desired
    ) const
{
    CHK_START;

    (*pfIsConstantExpression) = false;

    if (_functionCallType != FunctionCallType::normal)
    {
        // Assume all true until we learn otherwise
        (*pfIsConstantExpression) = true;

        // Look through all of the arguments
        for (UINT i = 0; i < GetArgumentCount(); i++)
        {
            bool fChildConstExpression;
            CHK(GetArgumentNode(i)->IsConstExpression(fIncludeIndex, &fChildConstExpression, /*value*/nullptr));

            (*pfIsConstantExpression) = fChildConstExpression;

            if (!fChildConstExpression)
            {
                // No point in checking further
                break;
            }
        }

        // Now check if we need to calculate this value
        if (*pfIsConstantExpression && pValue != nullptr)
        {
            // We need to grab the constant value from the argument and convert it - since we are
            // only currently supporting integer and float constructors, and these only take a single
            // argument, we know we only care about the first argument.
            bool fChildConst;
            ConstantValue childValue;
            CHKB(GetArgumentCount() == 1);
            CHK(GetArgumentNode(0)->IsConstExpression(fIncludeIndex, &fChildConst, &childValue));

            switch (_basicConstructType)
            {
            case INT_TOK:
                int intValue;
                if (SUCCEEDED(childValue.AsInt(&intValue)))
                {
                    pValue->SetValue(intValue);
                }
                else
                {
                    // We know it is an int, we just don't know which one
                    pValue->SetTypeOnly(INT_TOK);
                }
                break;

            case FLOAT_TOK:
                double doubleValue;
                if (SUCCEEDED(childValue.AsDouble(&doubleValue)))
                {
                    pValue->SetValue(doubleValue);
                }
                else
                {
                    // We know it is a float, we just don't know which one
                    pValue->SetTypeOnly(FLOAT_TOK);
                }
                break;

            default:
                // We can only set the type here
                pValue->SetTypeOnly(_basicConstructType);
                break;
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "FunctionCallHeaderWithParameters FunctionCallType=%d", static_cast<int>(_functionCallType));
}

//+----------------------------------------------------------------------------
//
//  Function:   ValidateArgumentsWithParameterQualifiers
//
//  Synopsis:   Validates that the argument children are compatible with the
//              matched function signature's parameter qualifiers.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::ValidateArgumentsWithParameterQualifiers(
    __in const CGLSLFunctionSignature& signature        // Signature to grab qualifiers from
    ) const
{
    CHK_START;

    // Currently only Normal function signatures have relevant qualifiers
    // that must be checked (no known functions have out or inout semantics; we
    // don't need to verify LValues).
    if (signature.GetSignatureType() == GLSLSignatureType::Normal)
    {
        AssertSz(signature.GetArgumentCount() == GetArgumentCount(),
            "This function should be called only after a match between signature "
            "and arguments, which implies the same number in both"
            );

        for (UINT i = 0; i < GetArgumentCount(); i++)
        {
            const ParseTreeNode* pNodeArgument = GetArgumentNode(i);
            // Check that the argument does not clash with the qualifier of the template
            switch (signature.GetArgumentQualifier(i))
            {
            case OUT_TOK:
            case INOUT_TOK:
                // Must be an lvalue
                if (!pNodeArgument->IsLValue())
                {
                    CHK(GetParser()->LogError(&_location, E_GLSLERROR_INVALIDLVALUE, nullptr));
                    CHK(E_GLSLERROR_KNOWNERROR);
                }

                // Cannot be a loop index
                TSmartPointer<CVariableIdentifierInfo> spInfo;
                if (SUCCEEDED(pNodeArgument->GetLValue(&spInfo)))
                {
                    if (spInfo->IsLoopDeclared())
                    {
                        CHK(GetParser()->LogError(&_location, E_GLSLERROR_LOOPINDEXOUTPARAM, nullptr));
                        CHK(E_GLSLERROR_KNOWNERROR);
                    }
                }
                break;
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetArgumentTypes
//
//  Synopsis:   Fills the passed in array with the GLSLType for each of the
//              argument children.
//
//-----------------------------------------------------------------------------
HRESULT FunctionCallHeaderWithParametersNode::GetArgumentTypes(__out CModernArray<TSmartPointer<GLSLType>>& aryTypes) const
{
    CHK_START;

    for (UINT i = 0; i < GetArgumentCount(); i++)
    {
        TSmartPointer<GLSLType> spArgType;
        CHK(GetArgumentNode(i)->GetExpressionType(&spArgType));
        CHK(aryTypes.Add(spArgType));
    }

    CHK_RETURN;
}

