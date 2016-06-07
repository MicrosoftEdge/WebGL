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
#include "ArrayGLSLType.hxx"
#include "RefCounted.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT ArrayGLSLType::Initialize(__in GLSLType* pType, int arraySize)
{
    _spType = pType;
    _arraySize = arraySize;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsSampler2DType
//
//  Synopsis:   Returns true if the type is a sampler 2D or an array thereof.
//
//-----------------------------------------------------------------------------
bool ArrayGLSLType::IsSampler2DType() const
{
    return _spType->IsSampler2DType();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsSamplerCubeType
//
//  Synopsis:   Returns true if the type is a cube sampler or an array thereof.
//
//-----------------------------------------------------------------------------
bool ArrayGLSLType::IsSamplerCubeType() const
{
    return _spType->IsSamplerCubeType();
}

//+----------------------------------------------------------------------------
//
//  Function:   IsTypeOrArrayOfType
//
//  Synopsis:   Determines if the type is the given basic type, or an array
//              of the given basic type.
//
//-----------------------------------------------------------------------------
bool ArrayGLSLType::IsTypeOrArrayOfType(int basicType) const
{
    return _spType->IsTypeOrArrayOfType(basicType);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetElementCount
//
//  Synopsis:   Returns 1 for non array types and array size for array types.
//
//-----------------------------------------------------------------------------
int ArrayGLSLType::GetElementCount() const
{
    return _arraySize;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetRowCount
//
//  Synopsis:   Returns the number of rows the array of a given type fills
//
//-----------------------------------------------------------------------------
HRESULT ArrayGLSLType::GetRowCount(__out UINT* puRowCount) const
{
    CHK_START;

    UINT uTypeRowCount;
    CHK(_spType->GetRowCount(&uTypeRowCount));

    UINT uArraySize;
    CHK_VERIFY(IntToUInt(_arraySize, &uArraySize) == S_OK);
    CHK(UIntMult(uArraySize, uTypeRowCount, puRowCount));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetArrayElementType
//
//  Synopsis:   Get the type that this is an array of.
//
//-----------------------------------------------------------------------------
HRESULT ArrayGLSLType::GetArrayElementType(__deref_out GLSLType** ppType) const
{
    _spType.CopyTo(ppType);

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetArraySize
//
//  Synopsis:   Get the size for an array type.
//
//-----------------------------------------------------------------------------
HRESULT ArrayGLSLType::GetArraySize(__out int* pArraySize) const
{
    (*pArraySize) = _arraySize;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualType
//
//  Synopsis:   Test equality of two types
//
//-----------------------------------------------------------------------------
bool ArrayGLSLType::IsEqualType(__in const GLSLType* pOther) const
{
    bool fEqual = false;
    if (pOther->IsArrayType())
    {
        const ArrayGLSLType* pOtherArray = pOther->AsArrayType();
        if (_arraySize == pOtherArray->GetArraySize())
        {
            TSmartPointer<GLSLType> spOtherElementType;
            if (pOtherArray->GetArrayElementType(&spOtherElementType) == S_OK)
            {
                fEqual = _spType->IsEqualType(spOtherElementType);
            }
            else
            {
                AssertSz(false, "A type that returned S_OK for GetArraySize should never fail to GetArrayElementType");
            }
        }
    }
    return fEqual;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualTypeForUniforms
//
//  Synopsis:   Test equality of two types for shared uniforms. Array types are
//              considered equal if their sizes are equal and the underlying
//              type is equal for shared uniforms.
//
//-----------------------------------------------------------------------------
bool ArrayGLSLType::IsEqualTypeForUniforms(
    __in const CGLSLIdentifierTable* pIdTableThis,                  // Identifier table for this type
    __in const GLSLType* pOther,                                    // Other type to compare
    __in const CGLSLIdentifierTable* pIdTableOther                  // Identifier table for other type
    ) const
{
    bool fEqual = false;
    if (pOther->IsArrayType())
    {
        const ArrayGLSLType* pOtherArray = pOther->AsArrayType();
        if (_arraySize == pOtherArray->GetArraySize())
        {
            TSmartPointer<GLSLType> spOtherElementType;
            if (pOtherArray->GetArrayElementType(&spOtherElementType) == S_OK)
            {
                fEqual = _spType->IsEqualTypeForUniforms(pIdTableThis, spOtherElementType, pIdTableOther);
            }
            else
            {
                AssertSz(false, "A type that returned S_OK for GetArraySize should never fail to GetArrayElementType");
            }
        }
    }

    return fEqual;
}

//+----------------------------------------------------------------------------
//
//  Function:   EnumerateActiveInfoForType
//
//  Synopsis:   Fills in aryActiveInfo with a GLSLActive info for each type
//              that is part of this array type.
//
//-----------------------------------------------------------------------------
HRESULT ArrayGLSLType::EnumerateActiveInfoForType(
    __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
    __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
    ) const
{
    CHK_START;

    // We should never have a valid ArrayGLSLType with a size less than 1.
    CHK_VERIFY(_arraySize > 0);

    if (_spType->IsBasicType())
    {
        // For basic types, first get the active info for the basic type.
        // This will fill in the appropriate glType.
        WHEN_DBG(UINT uOriginalSize = aryActiveInfo.GetCount());
        CHK(_spType->EnumerateActiveInfoForType(pIdTable, /*inout*/aryActiveInfo));
        AssertSz((uOriginalSize + 1) == aryActiveInfo.GetCount(), "Basic types should only add a single active info record");

        CGLSLActiveInfo<char>& activeInfo = aryActiveInfo[aryActiveInfo.GetCount() - 1];

        // Override the empty name (basic types will never have a suffix). For GLSL uniforms 
        // that are array types, the active info requires the "[0]" suffix.
        CHK(activeInfo.UseName().Set("[0]"));

        // Set the array size based on the size of this array type.
        UINT uArraySize;
        CHK_VERIFY(SUCCEEDED(IntToUInt(_arraySize, &uArraySize)));
        activeInfo.SetArraySize(uArraySize);
    }
    else
    {
        AssertSz(_spType->IsStructType(), "We should never have arrays of array types");

        // Array of struct types are different from arrays of basic types. We must
        // report all of the fields individually, and multiple times (with the suffixes "[0]"
        // to "[n-1]" where n is this array type's _arraySize. If we took a top down approach,
        // we'd have to call EnumerateActiveInfoForType on the struct type n times. Instead
        // we take a bottom up approach of building these active info entries (and more specifically
        // their suffix strings).

        // First we'll get all the entries from the struct type.  This will enumerate each 
        // of the fields' entries (recursively).
        CModernArray<CGLSLActiveInfo<char>> aryActiveInfoForStruct;
        CHK(_spType->EnumerateActiveInfoForType(pIdTable, /*inout*/aryActiveInfoForStruct));

        // Then, in order to report the leaf variable entries individually, we have to 
        // generate active info entries for each field for each valid array
        // index (0 to _arraySize - 1).
        for (int i = 0; i < _arraySize; i++)
        {
            UINT cCountBeforeArrayCopy = aryActiveInfo.GetCount();
            // Copy the gathered struct infos to the main array, for each array index.
            CHK(aryActiveInfo.AddArray(aryActiveInfoForStruct));

            for (UINT j = cCountBeforeArrayCopy; j < aryActiveInfo.GetCount(); j++)
            {
                // For each of the copies we just made, prepend the array notation for the
                // current array index.
                CGLSLActiveInfo<char>& activeInfoCopy = aryActiveInfo[j];
                const char* pszFieldSuffix = aryActiveInfoForStruct[j - cCountBeforeArrayCopy].GetNameString();

                size_t cchStructFieldSuffix = strlen(pszFieldSuffix);
                CHK(activeInfoCopy.UseName().Format(20 + cchStructFieldSuffix, "[%d]%s", i, pszFieldSuffix));
            }
        }
    }

    CHK_RETURN;
}
