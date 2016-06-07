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
#include "StructGLSLType.hxx"
#include "RefCounted.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "GLSLTypeInfo.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "BasicGLSLType.hxx"
#include "GLSLIdentifierTable.hxx"
#include "TypeHelpers.hxx"
#include "WebGLConstants.hxx"

const UINT StructGLSLType::s_cMaxNestingLevel = 4;

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
StructGLSLType::StructGLSLType() :
    _pTypeNameInfo(nullptr),
    _fConstructorUsed(false),
    _fEqualsOperatorUsed(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::Initialize(const CModernArray<TSmartPointer<IIdentifierInfo>>& aryFields)
{
    CHK_START;

    Assert(aryFields.GetCount() > 0);

    for (UINT i = 0; i < aryFields.GetCount(); i++)
    {
        CVariableIdentifierInfo* pVariableInfo = aryFields[i]->AsVariable();
        AssertSz(pVariableInfo != nullptr, "We don't expect non-variable identifier infos in the array from which we're constructing a struct type");
        CHK(_aryFields.Add(pVariableInfo));
    }

    // Ensure the fields that are a struct type (or array of struct type) doesn't cause this
    // type to go over our maximum for nested struct levels (as defined by the WebGL spec).
    CHKB_HR(GetStructNestingLevel() <= s_cMaxNestingLevel, E_GLSLERROR_MAXSTRUCTNESTINGEXCEEDED);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFieldInfoForVariable
//
//  Synopsis:   Provides lookup functionality on the struct type for a
//              variable identifier. 
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::GetFieldInfoForVariable(
    __in const VariableIdentifierNode* pVariableIdent,              // Variable to lookup by name
    __deref_out CVariableIdentifierInfo** ppVariableInfo            // Found identifier info
    ) const
{
    CHK_START;

    CHK(GetFieldInfoForSymbol(pVariableIdent->GetSymbolIndex(), ppVariableInfo));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetFieldInfoForSymbol
//
//  Synopsis:   Provides lookup functionality on the struct type. For the given
//              symbol, finds a CVariableIdentifierInfo that has the same 
//              symbol. Returns failure HRESULT if none found.
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::GetFieldInfoForSymbol(
    int iSymbolIndex,                                               // Symbol to lookup
    __deref_out CVariableIdentifierInfo** ppVariableInfo            // Found identifier info
    ) const
{
    CHK_START;

    bool fFound = false;

    for (UINT i = 0; i < _aryFields.GetCount() && !fFound; i++)
    {
        CVariableIdentifierInfo* pVariableInfo = _aryFields[i];
        if (iSymbolIndex == pVariableInfo->GetSymbolIndex())
        {
            *ppVariableInfo = pVariableInfo;
            pVariableInfo->AddRef();
            fFound = true;
        }
    }

    CHKB(fFound);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetElementCount
//
//  Synopsis:   Returns 1 for non array types
//
//-----------------------------------------------------------------------------
int StructGLSLType::GetElementCount() const
{
    return 1;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetRowCount
//
//  Synopsis:   Returns the cumulative sum of the fields' row counts
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::GetRowCount(__out UINT* puRowCount) const
{
    CHK_START;

    UINT uTotalRows = 0;
    for (UINT i = 0; i < _aryFields.GetCount(); i++)
    {
        TSmartPointer<GLSLType> spFieldType;
        CHK(_aryFields[i]->GetExpressionType(&spFieldType));

        UINT uFieldRowCount;
        CHK(spFieldType->GetRowCount(&uFieldRowCount));

        CHK(UIntAdd(uTotalRows, uFieldRowCount, &uTotalRows));
    }

    *puRowCount = uTotalRows;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualType
//
//  Synopsis:   Test equality of two types
//
//-----------------------------------------------------------------------------
bool StructGLSLType::IsEqualType(__in const GLSLType* pOther) const
{
    // Pointer equality is the correct check here because
    // struct types are only equivalent if the refer to the exact same declaration.
    // The only way this can happen is if a type is looked up via CTypeNameIdentifierInfo,
    // which after it is added to the identifier list for a given scope, will always
    // have a pointer to the same GLSLType object. 
    return (this == pOther);
}

//+----------------------------------------------------------------------------
//
//  Function:   IsEqualTypeForUniforms
//
//  Synopsis:   Test equality of two types for shared uniforms. Struct types
//              are considered equal if they have the same number of fields
//              with the same names, and those names have the same types.
//
//-----------------------------------------------------------------------------
bool StructGLSLType::IsEqualTypeForUniforms(
    __in const CGLSLIdentifierTable* pIdTableThis,                  // Identifier table for this type
    __in const GLSLType* pOther,                                    // Other type to compare
    __in const CGLSLIdentifierTable* pIdTableOther                  // Identifier table for other type
    ) const
{
    bool fEqual = false;

    CHK_START;

    if (pOther->IsStructType())
    {
        const StructGLSLType* pOtherStruct = pOther->AsStructType();

        // First step in checking equality is that the typenames are the same
        PCSTR pszTypeNameThis = pIdTableThis->GetNameForSymbolIndex(_pTypeNameInfo->GetSymbolIndex());
        PCSTR pszTypeNameOther = pIdTableOther->GetNameForSymbolIndex(pOtherStruct->_pTypeNameInfo->GetSymbolIndex());
        CHKB(strcmp(pszTypeNameThis, pszTypeNameOther) == 0);

        // Next, retrieve a list of all the variable info's with their 
        // symbol index converted to a name string from the id table
        CModernArray<VariableInfoWithName> aryVarInfoThis;
        CHK(GetVariableInfoWithNames(pIdTableThis, /*inout*/aryVarInfoThis));

        CModernArray<VariableInfoWithName> aryVarInfoOther;
        CHK(pOtherStruct->GetVariableInfoWithNames(pIdTableOther, /*inout*/aryVarInfoOther));

        Assert(aryVarInfoThis.GetCount() > 0);

        // Number of fields must be the same in order for types to be equivalent.
        CHKB(aryVarInfoThis.GetCount() == aryVarInfoOther.GetCount());

        // For each of the fields in order, ensure they have the same name and same type
        // for shared uniforms.
        for (UINT i = 0; i < aryVarInfoThis.GetCount(); i++)
        {
            const VariableInfoWithName& infoThis = aryVarInfoThis[i];
            const VariableInfoWithName& infoOther = aryVarInfoOther[i];
            CHKB(strcmp(infoThis.pszName, infoOther.pszName) == 0);

            CHKB(CVariableIdentifierInfo::AreEqualTypesForUniforms(
                infoThis.spInfo,
                pIdTableThis,
                infoOther.spInfo,
                pIdTableOther
                ));
        }

        // If we got through all the checks in the loop above, then indeed the types are equal.
        fEqual = true;
    }

    CHK_END;

    return fEqual;
}

//+----------------------------------------------------------------------------
//
//  Function:   EnumerateActiveInfoForType
//
//  Synopsis:   Adds the active info entries for each of the fields of this
//              struct type.
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::EnumerateActiveInfoForType(
    __in const CGLSLIdentifierTable* pIdTable,                      // Identifier table for the type
    __inout CModernArray<CGLSLActiveInfo<char>>& aryActiveInfo      // Array to append each active info entry that is part of this type
    ) const
{
    CHK_START;

    for (UINT i = 0; i < _aryFields.GetCount(); i++)
    {
        // Keep track of the original count so we know how many entries are added for
        // this field.
        UINT uOriginalCount = aryActiveInfo.GetCount();
        CHK(_aryFields[i]->UseType()->EnumerateActiveInfoForType(pIdTable, /*inout*/aryActiveInfo));

        // Types for each field should add at least one entry upon success.
        UINT uNewCount = aryActiveInfo.GetCount();
        CHK_VERIFY(uNewCount > uOriginalCount);

        // For each entry that was added for this field, we must prepend '.fieldName' to the 
        // entries' suffix.
        PCSTR pszFieldName = pIdTable->GetNameForSymbolIndex(_aryFields[i]->GetSymbolIndex());
        for (UINT j = uOriginalCount; j < uNewCount; j++)
        {
            static const size_t c_cchMaxSuffix = (2 * MAX_GLSL_TOKEN_SIZE) + 1;
            if (aryActiveInfo[j].UseName().GetLength() > 0)
            {
                // Make a copy of the string, since the suffix string currently lives in
                // the destination of the below Format call.
                CMutableString<char> spszFieldSuffix;
                CHK(spszFieldSuffix.Set(aryActiveInfo[j].GetNameString()));
                const char* pszFieldSuffix = static_cast<const char*>(spszFieldSuffix);
                CHK(aryActiveInfo[j].UseName().Format(c_cchMaxSuffix, ".%s%s", pszFieldName, pszFieldSuffix));
            }
            else
            {
                CHK(aryActiveInfo[j].UseName().Format(c_cchMaxSuffix, ".%s", pszFieldName));
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TypesMatchForConstructor
//
//  Synopsis:   Takes a list of types and ensures that each one is equivalent
//              to the ordered list of types that this struct type defines.
//
//-----------------------------------------------------------------------------
bool StructGLSLType::TypesMatchForConstructor(__in const CModernArray<TSmartPointer<GLSLType>>& aryCtorArgTypes) const
{
    bool fAllTypesMatch = false;

    const UINT cFields = _aryFields.GetCount();
    if (cFields == aryCtorArgTypes.GetCount())
    {
        UINT i = 0;
        for (; i < cFields; i++)
        {
            if (!_aryFields[i]->UseType()->IsEqualType(aryCtorArgTypes[i]))
            {
                break;
            }
        }

        // If the loop completed, then all the types matched
        fAllTypesMatch = (i == cFields);
    }

    return fAllTypesMatch;
}

//+----------------------------------------------------------------------------
//
//  Function:   FinalizeTypeWithTypeInfo
//
//  Synopsis:   StructGLSLType may need to acceess the typename information
//              that it is associated with. Here we cache a pointer to it.
//              We can cache this pointer since the CTypeNameIdentifierInfo 
//              passed in will have a reference to this type and clear it
//              before it is destructed.
//
//-----------------------------------------------------------------------------
void StructGLSLType::FinalizeTypeWithTypeInfo(__in CTypeNameIdentifierInfo* pTypeNameInfo)
{
#if DBG
    Assert(_pTypeNameInfo == nullptr);
    TSmartPointer<GLSLType> spType;
    Assert(pTypeNameInfo->GetType(&spType) == S_OK);
    Assert(spType == this);
#endif

    _pTypeNameInfo = pTypeNameInfo;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLConstructor
//
//  Synopsis:   Based on the ordered fields of this type, outputs an HLSL
//              function that takes parameters according to the fields and 
//              returns a value of this type whose fields are initialized to
//              this parameter.
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::OutputHLSLConstructor(__in IStringStream* pOutput) const
{
    CHK_START;

    // We only need to output this if someone used the constructor.
    if (_fConstructorUsed)
    {
        const char* pszTypeName = _pTypeNameInfo->GetHLSLName(0);

        // Output the typename as the return type and the constructor name as the function name
        CHK(pOutput->WriteString(pszTypeName));
        CHK(pOutput->WriteChar(' '));
        CHK(pOutput->WriteString(_pTypeNameInfo->GetHLSLConstructorName()));

        // Output the field variables as a parameter list
        CHK(pOutput->WriteChar('('));
        CHK(OutputHLSLVariablesAsParameters(pOutput));
        CHK(pOutput->WriteString(") {\n"));

        static const char* s_c_pszLocalStructName = "var_localstruct";

        // Output a local variable of the appropriate type
        CHK(pOutput->WriteString(pszTypeName));
        CHK(pOutput->WriteChar(' '));
        CHK(pOutput->WriteString(s_c_pszLocalStructName));
        CHK(pOutput->WriteString(";\n"));

        // Output the individual field selection assignments.
        CHK(OutputHLSLVariablesAsAssignments(s_c_pszLocalStructName, pOutput));

        // Then return the fully assigned local variable.
        CHK(pOutput->WriteString("return "));
        CHK(pOutput->WriteString(s_c_pszLocalStructName));
        CHK(pOutput->WriteString(";\n"));
        CHK(pOutput->WriteString("}\n"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLEqualsFunction
//
//  Synopsis:   Based on the fields of this type, outputs an HLSL
//              function that takes two parameters of this type and compares
//              their fields individually, returning a bool indicating whether
//              or not the two values are equal. 
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::OutputHLSLEqualsFunction(__in IStringStream* pOutput) const
{
    CHK_START;

    // Skip outputting this function if no one will call it.
    if (_fEqualsOperatorUsed)
    {
        // Output a return type of bool and the equals operator function name.
        CHK(pOutput->WriteString("bool "));
        CHK(pOutput->WriteString(_pTypeNameInfo->GetHLSLEqualsFunctionName()));

        // Get strings for the parameter names as well as their type
        static const char* s_pszVal1 = "val1";
        static const char* s_pszVal2 = "val2";
        const char* pszTypeName = _pTypeNameInfo->GetHLSLName(0);

        // Output the two parameters of this type.
        CHK(pOutput->WriteChar('('));
        CHK(pOutput->WriteString(pszTypeName));
        CHK(pOutput->WriteChar(' '));
        CHK(pOutput->WriteString(s_pszVal1));
        CHK(pOutput->WriteString(", "));
        CHK(pOutput->WriteString(pszTypeName));
        CHK(pOutput->WriteChar(' '));
        CHK(pOutput->WriteString(s_pszVal2));
        CHK(pOutput->WriteString(") {\n"));

        // Begin the return statement (which will be an expression 
        // 'anding' the fields' equivalence together).
        CHK(pOutput->WriteString("return ("));

        // We'll output comparison between each of the fields, &&'ing the results.
        const UINT cFields = _aryFields.GetCount();
        for (UINT i = 0; i < cFields; i++)
        {
            const CVariableIdentifierInfo* pVariableIdInfo = _aryFields[i];

            AssertSz(pVariableIdInfo->GetHLSLNameCount() == 1, 
                "Samplers are not allowed as struct type fields, so we should never have a variable that was cloned and has two names"
                );

            const char* pszHLSLName = pVariableIdInfo->GetHLSLName(0);
            const GLSLType* pFieldType = pVariableIdInfo->UseType();
            if (pFieldType->IsBasicType())
            {
                int basicType;
                CHK_VERIFY(SUCCEEDED(pFieldType->GetBasicType(&basicType)));

                // Non-component types (like vectors or matrices) must wrap their
                // == in the HLSL 'all' function in order to extract a bool
                // value to use in our 'and' expression.
                bool fWrapWithAll = !(TypeHelpers::IsVectorComponentType(basicType));
                if (fWrapWithAll)
                {
                    CHK(pOutput->WriteString("all("));
                }

                // Output the comparison for the two fields:
                //    var1.foo==var2.foo
                CHK(pOutput->WriteString(s_pszVal1));
                CHK(pOutput->WriteChar('.'));
                CHK(pOutput->WriteString(pszHLSLName));

                CHK(pOutput->WriteString("=="));

                CHK(pOutput->WriteString(s_pszVal2));
                CHK(pOutput->WriteChar('.'));
                CHK(pOutput->WriteString(pszHLSLName));

                // Close off the HLSL all() wrapper if necessary.
                if (fWrapWithAll)
                {
                    CHK(pOutput->WriteChar(')'));
                }
            }
            else
            {
                // Types containing arrays that use the equals operator should have been flagged at verification time.
                CHK_VERIFY(pFieldType->IsStructType());

                // Output a function call of the field type's equals function, since there is no
                // '==' operator for struct types (even when they are fields of another struct).
                CHK(pOutput->WriteString(pFieldType->AsStructType()->UseTypeNameInfo()->GetHLSLEqualsFunctionName()));
                CHK(pOutput->WriteChar('('));
                CHK(pOutput->WriteString(s_pszVal1));
                CHK(pOutput->WriteChar('.'));
                CHK(pOutput->WriteString(pszHLSLName));
                CHK(pOutput->WriteChar(','));
                CHK(pOutput->WriteString(s_pszVal2));
                CHK(pOutput->WriteChar('.'));
                CHK(pOutput->WriteString(pszHLSLName));
                CHK(pOutput->WriteChar(')'));
            }

            // Output an && operator between all the comparisons, but not after the last one.
            if (i + 1 < cFields)
            {
                CHK(pOutput->WriteString(" && "));
            }

        }

        // Close and finish the return statement, and close the function definition.
        CHK(pOutput->WriteString(");\n"));
        CHK(pOutput->WriteString("}\n"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetEqualsOperatorUsed
//
//  Synopsis:   Called when the equals operator is used for this type.
//              Propagates to fields types as well.
//
//-----------------------------------------------------------------------------
void StructGLSLType::SetEqualsOperatorUsed()
{
    // Mark ourselves as having the equals operator used
    _fEqualsOperatorUsed = true;

    // And mark any field struct types as well, since generating
    // the equals operator for this type will call the field's
    // operator equal. We don't have to worry about unwrapping
    // array types, since types containing arrays don't have the
    // equals operators defined.
    for (UINT i = 0; i < _aryFields.GetCount(); i++)
    {
        GLSLType* pFieldType = _aryFields[i]->UseType();
        if (pFieldType->IsStructType())
        {
            pFieldType->AsStructType()->SetEqualsOperatorUsed();
        }
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   ContainsArrayType
//
//  Synopsis:   Determines whether there is a field contained by this type
//              (or its subtypes) that is an array type.
//
//-----------------------------------------------------------------------------
bool StructGLSLType::ContainsArrayType() const
{
    bool fContainsArrayType = false;

    for (UINT i = 0; i < _aryFields.GetCount() && !fContainsArrayType; i++)
    {
        GLSLType* pFieldType = _aryFields[i]->UseType();
        if (pFieldType->IsArrayType())
        {
            fContainsArrayType = true;
        }
        else if (pFieldType->IsStructType())
        {
            fContainsArrayType = pFieldType->AsStructType()->ContainsArrayType();
        }
    }

    return fContainsArrayType;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLVariablesAsParameters
//
//  Synopsis:   Outputs a comma separated list of type and variable name pairs
//              including array size as necessary for this struct type's fields
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::OutputHLSLVariablesAsParameters(__in IStringStream* pOutput) const
{
    CHK_START;

    const UINT cFields = _aryFields.GetCount();
    for (UINT i = 0; i < cFields; i++)
    {
        const CVariableIdentifierInfo* pVariableIdInfo = _aryFields[i];

        AssertSz(pVariableIdInfo->GetHLSLNameCount() == 1, 
            "Samplers are not allowed as struct type fields, so we should never have a variable that was cloned and has two names"
            );

        // spTypeForOutput will hold a type that we know how to output as a string
        // (e.g. not an array type)
        TSmartPointer<GLSLType> spTypeForOutput;
        bool fFieldIsArrayType = pVariableIdInfo->UseType()->IsArrayType();
        if (fFieldIsArrayType)
        {
            CHK_VERIFY(pVariableIdInfo->UseType()->GetArrayElementType(&spTypeForOutput) == S_OK);
        }
        else
        {
            spTypeForOutput = pVariableIdInfo->UseType();
        }

        Assert(!spTypeForOutput->IsArrayType());

        if (spTypeForOutput->IsBasicType())
        {
            // Basic types are output via BasicGLSLTypeInfo
            int basicType = spTypeForOutput->AsBasicType()->GetBasicType();
            const BasicGLSLTypeInfo* pInfo;
            CHK(BasicGLSLTypeInfo::GetInfo(basicType, &pInfo));
            CHK(pOutput->WriteString(pInfo->_pszHLSLName));
        }
        else
        {
            // Struct types are output via the typename info associated with the type
            Assert(spTypeForOutput->IsStructType());
            CHK(pOutput->WriteString(spTypeForOutput->AsStructType()->UseTypeNameInfo()->GetHLSLName(0)));
        }

        CHK(pOutput->WriteChar(' '));

        CHK(pOutput->WriteString(pVariableIdInfo->GetHLSLName(0)));

        if (fFieldIsArrayType)
        {
            int arraySize;
            CHK_VERIFY(pVariableIdInfo->UseType()->GetArraySize(&arraySize) == S_OK);
            CHK(pOutput->WriteChar('['));
            CHK(pOutput->WriteFormat(64, "%d", arraySize));
            CHK(pOutput->WriteChar(']'));
        }

        // Output comma separation between all parameters, but not after the last one
        if (i + 1 < cFields)
        {
            CHK(pOutput->WriteString(", "));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSLVariablesAsAssignments
//
//  Synopsis:   Outputs field assignments for each field for this struct type.
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::OutputHLSLVariablesAsAssignments(__in const char* pszLocalStructVarName, __in IStringStream* pOutput) const
{
    CHK_START;

    const UINT cFields = _aryFields.GetCount();
    for (UINT i = 0; i < cFields; i++)
    {
        const CVariableIdentifierInfo* pVariableIdInfo = _aryFields[i];

        AssertSz(pVariableIdInfo->GetHLSLNameCount() == 1, 
            "Samplers are not allowed as struct type fields, so we should never have a variable that was cloned and has two names"
            );

        const char* pszHLSLName = pVariableIdInfo->GetHLSLName(0);

        // Output is of the form localvar.var = var
        // This assumes the parameters were named the same as the
        // field names in OutputHLSLVariablesAsParameters above.
        CHK(pOutput->WriteString(pszLocalStructVarName));
        CHK(pOutput->WriteChar('.'));
        CHK(pOutput->WriteString(pszHLSLName));

        CHK(pOutput->WriteChar('='));
        CHK(pOutput->WriteString(pszHLSLName));
        CHK(pOutput->WriteString(";\n"));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetStructNestingLevel
//
//  Synopsis:   Return the struct nesting level for this struct type.
//
//-----------------------------------------------------------------------------
UINT StructGLSLType::GetStructNestingLevel() const
{
    UINT uMaxNestedField = 0;

    // Determine the field with the max struct nesting.
    for (UINT i = 0; i < _aryFields.GetCount(); i++)
    {
        CVariableIdentifierInfo* pVariableInfo = _aryFields[i]->AsVariable();

        UINT uFieldStructNestingLevel = pVariableInfo->UseType()->GetStructNestingLevel();
        if (uFieldStructNestingLevel > uMaxNestedField)
        {
            uMaxNestedField = uFieldStructNestingLevel;
        }
    }

    // Add one to the max we found since this type is a struct type.
    return uMaxNestedField + 1;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVariableInfoWithNames
//
//  Synopsis:   Fill an array with VariableInfoWithName's by looking up the
//              name for the symbol index for each field's
//              CVariableIdentifierInfo.
//
//-----------------------------------------------------------------------------
HRESULT StructGLSLType::GetVariableInfoWithNames(
    __in const CGLSLIdentifierTable* pIdTable,                      // Id table for symbols in this type's field variable infos
    __inout CModernArray<VariableInfoWithName>& aryVarInfoWithNames // Array to fill with VariableInfoWithName structs
    ) const
{
    CHK_START;

    Assert(aryVarInfoWithNames.GetCount() == 0);
    aryVarInfoWithNames.RemoveAll();

    UINT cFields = _aryFields.GetCount();
    CHK(aryVarInfoWithNames.EnsureSize(cFields));

    for (UINT i = 0; i < cFields; i++)
    {
        CVariableIdentifierInfo* pVariableInfo = _aryFields[i];
        VariableInfoWithName& info = aryVarInfoWithNames[i];
        info.pszName = pIdTable->GetNameForSymbolIndex(pVariableInfo->GetSymbolIndex());
        info.spInfo = pVariableInfo;
    }

    CHK_RETURN;
}
