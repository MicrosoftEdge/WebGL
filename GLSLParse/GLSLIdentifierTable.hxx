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
#pragma once

#include "ParseTreeNode.hxx"
#include <foundation/collections.hxx>
#include "SmartMemory.hxx"
#include "GLSLSymbolTable.hxx"
#include "GLSLQualifier.hxx"
#include "VariableIdentifierInfo.hxx"

class GLSLType;
class FunctionIdentifierNode;
class CFunctionIdentifierInfo;
class TypeNameIdentifierNode;
class CTypeNameIdentifierInfo;
class CGLSLExtensionState;

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLIdentifierTable
//
//  Synopsis:   The identifier table holds a table of information about all of
//              the identifiers in the GLSL shader.
//
//              The data held here is different to the kind of data in the
//              symbol table - each unique symbol occupies only a single spot
//              in the symbol table, whereas a single symbol can be used
//              by multiple different identifiers.
//
//              Users of the identifier table also need to follow rules about
//              identifiers being added versus being looked up.
//
//------------------------------------------------------------------------------
class CGLSLIdentifierTable : public IUnknown
{
public:
    HRESULT AddVariableIdentifier(
        __in VariableIdentifierNode* pIdentifier,                       // The parse tree node for the identifier
        __in GLSLType* pType,                                           // The GLSL type of the identifier
        int typeQualifier,                                              // The storage qualifier
        int precisionQualifier,                                         // The precision qualifier
        bool fIsParameter,                                              // Whether it is a function parameter
        const ConstantValue& initValue,                                 // The initial value that the identifier has
        __deref_out CVariableIdentifierInfo** ppInfo                    // The info that was added
        );

    HRESULT AddFunctionIdentifier(
        __in FunctionIdentifierNode* pIdentifier,                       // The parse tree node for the identifier
        __in FunctionHeaderWithParametersNode* pFunctionHeader,         // The function header with param that owns the identifier
        __deref_out CFunctionIdentifierInfo** ppInfo                    // The info for the function
        );

    HRESULT AddTypeNameIdentifier(
        __in TypeNameIdentifierNode* pIdentifier,                       // The typename id node that defines the type name
        __in GLSLType* pType,                                           // The type to associate with the id
        __deref_out CTypeNameIdentifierInfo** ppInfo                    // Created info for the typename
        );

    HRESULT GetVariableInfoFromString(
        __in_z const char* pszString,                                   // The variable name to get info about
        GLSLQualifier::Enum typeQualifier,                              // What qualifier the variable has
        __deref_out CVariableIdentifierInfo** ppInfo                    // The info about the identifier
        );

    HRESULT GetVariableInfoAndNameByIndex(
        UINT index,                                                     // The index of the identifier info to grab
        __deref_out CVariableIdentifierInfo** ppInfo,                   // The actual info
        __out PCSTR* ppszGLSLName                                       // The glsl name for this identifier
        );

    HRESULT GetFieldInfoForVariableName(
        __in const GLSLType* pGLSLType,                                 // The type to query for field pszFieldName
        __in_z const char* pszFieldName,                                // Field name to look for
        __deref_out CVariableIdentifierInfo** ppInfo                    // Found variable identifier info
        );

    const char* GetNameForSymbolIndex(int iSymbolIndex) const;

protected:
    HRESULT Initialize(
        __in CGLSLParser* pParser                                       // The parser that owns the table
        );

private:
    TSmartPointer<CGLSLSymbolTable> _spSymbolTable;                     // The symbol table
    CModernArray<TSmartPointer<CVariableIdentifierInfo>> _aryVarList;   // The current list of variable identifiers
    CModernArray<TSmartPointer<CTypeNameIdentifierInfo>> _aryTypeList;  // The current list of typename identifiers
};
