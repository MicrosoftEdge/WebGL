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
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "IdentifierInfo.hxx"
#include "VariableIdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "CollectionNodeWithScope.hxx"
#include "CompoundStatementNode.hxx"
#include "InitDeclaratorListEntryNode.hxx"
#include "ParameterDeclaratorNode.hxx"
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "FieldSelectionNode.hxx"
#include "GLSLSymbolTable.hxx"
#include "RefCounted.hxx"
#include "GLSLType.hxx"
#include "GLSL.tab.h"
#include "StructDeclaratorNode.hxx"

MtDefine(VariableIdentifierNode, CGLSLParser, "VariableIdentifierNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
VariableIdentifierNode::VariableIdentifierNode() : 
    _uHLSLNameIndex(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Clone the identifier
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    CHK_START;

    CHK(IdentifierNodeBase::InitializeAsClone(pOriginal));

    VariableIdentifierNode* pOriginalId = pOriginal->GetAs<VariableIdentifierNode>();

    _spInfo = pOriginalId->_spInfo;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    TSmartPointer<CVariableIdentifierInfo> spInfo;
    CHK(GetVariableIdentifierInfo(&spInfo));

    bool fDeclarationIdentifier = IsDeclarationIdentifier();

    // Attributes and varyings have been moved into the input struct, but don't show
    // the struct name when we are actually declaring the struct
    if (spInfo->IsInputStructVariable() && !fDeclarationIdentifier)
    {
        // We could not find a declaration as a parent. Now we need to figure out the
        // name of the generated struct. It changes depending on where it lives.
        if (spInfo->GetTypeQualifier() == ATTRIBUTE)
        {
            // Attributes are inputs to the vertex shader
            CHK(pOutput->WriteString("vsInput."));
        }
        else
        {
            Assert(spInfo->GetTypeQualifier() == VARYING);

            // Varyings are inputs to the fragment shader
            CHK(pOutput->WriteString("psInput."));
        }
    }

    CHKB(spInfo->GetHLSLName(_uHLSLNameIndex) != nullptr);
    CHK(pOutput->WriteString(spInfo->GetHLSLName(_uHLSLNameIndex)));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Calculate the type of the node if it is in an expression tree.
//
//              If the node is part of a declaration, then this is the time to
//              add the node to the identifier table.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::VerifySelf()
{
    CHK_START;

    ParseTreeNode* pParent = GetParent();
    CHKB(pParent != nullptr);

    // Flag that will indicate at the end of the function if we need to set type
    // information for this node based on a lookup in the identifier table.
    //
    // If this flag is set to false, it means that the identifier is not being
    // used in an expression and is instead being used in some kind of declaration
    // or definition, and the variable or function identifier info will be added
    // at some point to deal with this, either here or elsewhere if it is more
    // appropriate.
    //
    // If this flag is set to true, it means that the identifier is being used
    // in some kind of expression, and therefore the variable information must
    // be found in the identifier table, and subsequently cached on this node.
    bool fLookupAndSetInfoFromScope = false;

    switch(pParent->GetParseNodeType())
    {
    case ParseNodeType::initDeclaratorListEntry:
        {
            // If the parent of this node is an init decl list entry, then this is the
            // node that declares the identifier. 
            InitDeclaratorListEntryNode* pListEntry = pParent->GetAs<InitDeclaratorListEntryNode>();
            if (this != pListEntry->GetChild(0))
            {
                // The first child of the list entry is the symbol that is being declared by
                // the entry, and the entry node will add it to the identifier table, so we 
                // don't need to do anything about that here.
                //
                // If the other child of the list entry happens to be an identifier, we don't
                // want to add it as a variable. It is being used as an initializer here, and
                // it must already be added as an identifier earlier in the program. So we just
                // set this variable and hit the path we would for any other expression.
                fLookupAndSetInfoFromScope = true;
            }
        }
        break;

    case ParseNodeType::parameterDeclarator:
        {
            // For ParameterDeclaratorNode parent, we only lookup the info from the scope for
            // the array size expression (in order to potentially propagate constant info).
            // The ParameterDeclaratorNode handles adding the variable info for the identifier that
            // defines the symbol.
            ParameterDeclaratorNode* pParamDeclaratorNode = pParent->GetAs<ParameterDeclaratorNode>();
            if (this == pParamDeclaratorNode->GetArraySizeExprNode())
            {
                fLookupAndSetInfoFromScope = true;
            }
        }
        break;

    case ParseNodeType::fieldSelection:
        {
            // We only look at the first child, because the second child of the field
            // selection is either a swizzle identifier (like xyz or rgb) or struct field name.
            // The swizzle ident has nothing to do with our identifier table stuff.
            // The struct field name will have had its info set when the field selection node
            // was verified.
            FieldSelectionNode* pFieldSelection = pParent->GetAs<FieldSelectionNode>();
            if (this == pFieldSelection->GetChild(0))
            {
                // The first child of the field selection should already exist,
                // so lookup the info accordingly.
                fLookupAndSetInfoFromScope = true;
            }
        }
        break;

    case ParseNodeType::structDeclarator:
        {
            // The first child of the struct declarator is the variable being defined for the struct.
            // This first child will have its identifier info set based on the struct declaration type
            // and (potentially) the declarator's arrayness.
            // However, the constant expression (non-first-child of the struct declarator) does
            // need to set its info from the scope. This will propagate const initial values
            // and allow this variable identifier node to output itself.
            StructDeclaratorNode* pStructDeclaratorNode = pParent->GetAs<StructDeclaratorNode>();
            if (this == pStructDeclaratorNode->GetConstantExpressionChild())
            {
                fLookupAndSetInfoFromScope = true;
            }
        }
        break;

    default:
        fLookupAndSetInfoFromScope = true;
        break;
    }

    if (fLookupAndSetInfoFromScope)
    {
        // Since we feel safe setting the type from the identifier info, it means
        // that we are not calling a function and therefore must have an unambigious
        // way to procure the identifier information from walking up the tree.
        CHK(SetIdentifierInfoFromTree());

        // Get the type out of the info
        TSmartPointer<GLSLType> spType;
        CHK(_spInfo->GetExpressionType(&spType));

        SetExpressionType(spType);
    }

    // If we are verifying an identifier node, and it is not being declared,
    // then it is being used in some way and we will remember that.
    if (!IsDeclarationIdentifier() && _spInfo != nullptr)
    {
        _spInfo->SetUsed();
        CHK(GetParser()->AddSpecialVariableUsage(&GetLocation(), _spInfo->GetSpecialVariable()));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsDeclarationIdentifier
//
//  Synopsis:   Figure out if this identifier is defining a variable as
//              part of declaration list or struct declarator list.
//
//-----------------------------------------------------------------------------
bool VariableIdentifierNode::IsDeclarationIdentifier()
{
    // The parent must be a list entry
    ParseTreeNode* pParent = GetParent(); 
    if (pParent != nullptr)
    {
        if (pParent->GetParseNodeType() == ParseNodeType::initDeclaratorListEntry)
        {
            // The node should be the first child of the entry
            InitDeclaratorListEntryNode* pEntry = pParent->GetAs<InitDeclaratorListEntryNode>();
            return (pEntry->GetChild(0) == this);
        }
        else if (pParent->GetParseNodeType() == ParseNodeType::structDeclarator)
        {
            // The node should be the first child of the entry
            StructDeclaratorNode* pStructDeclaratorNode = pParent->GetAs<StructDeclaratorNode>();
            return (pStructDeclaratorNode->GetChild(0) == this);
        }
    }

    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetVariableIdentifierInfo
//
//  Synopsis:   Get the identifier info for the variable identifier that this 
//              node represents. This function retrieves the cached value that
//              has been previously set.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::GetVariableIdentifierInfo(
    __deref_out CVariableIdentifierInfo** ppInfo                    // The info about the identifier if it is a variable
    )
{
    CHK_START;

    CHKB(_spInfo != nullptr);
    _spInfo.CopyTo(ppInfo);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetIdentifierInfoFromTree
//
//  Synopsis:   Set the identifier info for the identifier that this node
//              represents.
//
//              This is not a simple operation that the identifier table is
//              able to do - it is a parse tree operation because the scopes
//              have to be searched from the bottom up.
//
//              The identifier info is not always set this way. It might be
//              set by a parent tree node as part of verification (like in the
//              case of function calls).
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::SetIdentifierInfoFromTree()
{
    CHK_START;

    // Search for all of the identifiers that match in our scope
    CModernArray<TSmartPointer<IIdentifierInfo>> rgInfos;
    if (FAILED(CollectionNodeWithScope::GetNearestScopeInfoList(this, rgInfos)))
    {
        // Failing to get anything means no identifier matches
        CHK(GetParser()->LogError(&GetLocation(), E_GLSLERROR_UNDECLAREDIDENTIFIER, GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    bool fFound = false;
    for (UINT i = 0; i < rgInfos.GetCount(); i++)
    {
        if (rgInfos[i]->AsVariable() != nullptr)
        {
            if (fFound)
            {
                // Since the above call succeeded, we should only have a single value here
                // that is a variable identifier. Anything else is an indication that our
                // code is broken - more than one matching identifier should not end up in the
                // table.
                CHK(E_UNEXPECTED);
                AssertSz(false, "Verification should never allow multiple variables in single scope");
            }
            else
            {
                _spInfo = rgInfos[i]->AsVariable();
                fFound = true;
            }
        }
    }

    if (!fFound)
    {
        // We can end up here if somebody uses a function name as a variable name in an expression -
        // we end up with values to look at but none of them are variables we can use.
        CHK(GetParser()->LogError(&GetLocation(), E_GLSLERROR_UNDECLAREDIDENTIFIER, GetText()));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetVariableIdentifierInfo
//
//  Synopsis:   Set the identifier info for the identifier that this node
//              represents.
//
//-----------------------------------------------------------------------------
void VariableIdentifierNode::SetVariableIdentifierInfo(__in CVariableIdentifierInfo* pInfo)
{
    _spInfo = pInfo;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLNameIndex
//
//  Synopsis:   When texture samplers are used in GLSL, the corresponding HLSL
//              code needs to create a sampler and texture for each GLSL
//              sampler.
//
//              When passing arguments to functions, both the sampler and
//              texture need to be passed. This is done by outputting the
//              expression twice, and changing the HLSL name index.
//
//              If the index given is out of range for this identifier, then
//              this is a no-op. It will only affect identifiers with the
//              large enough number of HLSL names, so that expressions with
//              non-sampler identifiers will still work.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::SetHLSLNameIndex(UINT uIndex)
{
    CHK_START;

    CHKB(_spInfo != nullptr);

    if (uIndex < _spInfo->GetHLSLNameCount())
    {
        _uHLSLNameIndex = uIndex;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MarkWritten
//
//  Synopsis:   Mark the variable owned by this node as written to.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::MarkWritten()
{
    // We will not have an info if we are an identifier for a member like 'xy'
    // so we only propogate down if we have identifier info.
    if (_spInfo != nullptr)
    {
        _spInfo->IncrementWriteCount();
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsConstExpression
//
//  Synopsis:   Figure out if an expression is made up only from constants
//              and (optionally) loop indices.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::IsConstExpression(
    bool fIncludeIndex,                                         // Whether to include loop index in the definition of a constant expression
    __out bool* pfIsConstantExpression,                         // Whether this node is a constant expression
    __out_opt ConstantValue* pValue                             // The value of the constant expression, if desired
    ) const
{
    (*pfIsConstantExpression) = false;

    if (_spInfo != nullptr)
    {
        // Things defined as const are constant expressions, unless they are function 
        // parameters. Loop index variables are constant expressions if requested as so.
        if ((_spInfo->GetTypeQualifier() == CONST_TOK && !_spInfo->IsParameter()) ||
            (_spInfo->IsLoopDeclared() && fIncludeIndex))
        {
            (*pfIsConstantExpression) = true;

            if (pValue != nullptr)
            {
                // Retrieve the value that it was initialized with
                (*pValue) = _spInfo->GetInitialValue();
            }
        }
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsLValue
//
//  Synopsis:   Determine if this node represents a valid Lvalue.
//
//              Identifiers for variables that have a type qualifier are generally
//              invalid lValues. This includes const, uniform, attribute and varying.
//              However, varyings in the vertex shader can be assigned to and
//              are valid lValues.
//
//-----------------------------------------------------------------------------
bool VariableIdentifierNode::IsLValue() const
{
    if (_spInfo != nullptr)
    {
        // The only variable qualifier the results in a lValue is varying in a vertex shader.
        // uniform, attribute, etc., are not assignable (not an lValue).
        int qualifier = _spInfo->GetTypeQualifier();
        if (qualifier == NO_QUALIFIER || (qualifier == VARYING && GetParser()->GetShaderType() == GLSLShaderType::Vertex))
        {
            return true;
        }
    }

    // No info means you cannot be an lValue
    return false;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetLValue
//
//  Synopsis:   Get the Lvalue for the subtree.
//
//              Since this is a variable identifier, it can return itself if
//              it meets the conditions.
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::GetLValue(__out CVariableIdentifierInfo** ppInfo) const
{
    if (IsLValue())
    {
        _spInfo.CopyTo(ppInfo);
        return S_OK;
    }

    return E_UNEXPECTED;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT VariableIdentifierNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "VariableIdentifierNode Symbol=%d", GetSymbolIndex());
}
