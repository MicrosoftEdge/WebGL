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
#include "GLSLType.hxx"
#include "BasicGLSLType.hxx"
#include "ArrayGLSLType.hxx"
#include "StructGLSLType.hxx"
#include "RefCounted.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   CreateFromBasicTypeToken
//
//  Synopsis:   Creates a BasicGLSLType from the passed in basic token type.
//
//-----------------------------------------------------------------------------
HRESULT GLSLType::CreateFromBasicTypeToken(int basicType, __deref_out GLSLType** ppNewType)
{
    CHK_START;

    TSmartPointer<BasicGLSLType> spBasicType;
    CHK(RefCounted<BasicGLSLType>::Create(basicType, /*out*/spBasicType));

    spBasicType.CopyTo(ppNewType);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateFromType
//
//  Synopsis:   If passed an array size, creates an ArrayGLSLType that wraps
//              the passed in type. Otherwise returns a pointer to the input
//              object with an extra reference.
//
//-----------------------------------------------------------------------------
HRESULT GLSLType::CreateFromType(__in GLSLType* pType, int arraySize, __deref_out GLSLType** ppNewType)
{
    CHK_START;

    if (arraySize != -1)
    {
        CHK_VERIFY(!pType->IsArrayType());
        TSmartPointer<ArrayGLSLType> spArrayType;
        CHK(RefCounted<ArrayGLSLType>::Create(pType, arraySize, /*out*/spArrayType));

        spArrayType.CopyTo(ppNewType);
    }
    else
    {
        // If it's not actually an array, just return the passed in type
        *ppNewType = pType;
        pType->AddRef();
    }

    CHK_RETURN;

}

//+----------------------------------------------------------------------------
//
//  Function:   CreateStructTypeFromIdentifierInfoAry
//
//  Synopsis:   Creates a struct type from the identifier infos passed in. The
//              bulk of the work takes place in StructGLSLType::Initialize
//
//-----------------------------------------------------------------------------
HRESULT GLSLType::CreateStructTypeFromIdentifierInfoAry(
    const CModernArray<TSmartPointer<IIdentifierInfo>>& aryIdInfo,  // List of variable identifiers that define struct fields
    __deref_out StructGLSLType** ppNewType                          // Newly created struct type
    )
{
    CHK_START;

    TSmartPointer<StructGLSLType> spStructType;
    CHK(RefCounted<StructGLSLType>::Create(aryIdInfo, /*out*/spStructType));
    spStructType.CopyTo(ppNewType);

    CHK_RETURN;
}
