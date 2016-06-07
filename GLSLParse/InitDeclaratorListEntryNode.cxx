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
#include "InitDeclaratorListEntryNode.hxx"
#include "InitDeclaratorListNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "IStringStream.hxx"
#include "IdentifierInfo.hxx"
#include "GLSLParser.hxx"
#include "RefCounted.hxx"
#include "GLSLType.hxx"
#include "GLSLTypeInfo.hxx"
#include "TypeHelpers.hxx"
#include "VerificationHelpers.hxx"
#include "BasicGLSLType.hxx"
#include "StructGLSLType.hxx"
#include "TypeNameIdentifierInfo.hxx"

MtDefine(InitDeclaratorListEntryNode, CGLSLParser, "InitDeclaratorListEntryNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
InitDeclaratorListEntryNode::InitDeclaratorListEntryNode() : 
    _arraySize(-1),
    _uHLSLNameIndex(0),
    _declarationType(DeclarationType::simple)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    __in ParseTreeNode* pIdentifier,            // The identifier for the entry
    __in_opt ParseTreeNode* pInitializer,       // The optional initializer for the entry
    __in_opt ParseTreeNode* pArraySizeExpr      // The array size expression (this is optional)
    )
{
    CHK_START;

    ParseTreeNode::Initialize(pParser);

    CHK(AppendChild(pIdentifier));
    CHK(AppendChild(pInitializer));
    CHK(AppendChild(pArraySizeExpr));

    if (pInitializer == nullptr && pArraySizeExpr == nullptr)
    {
        _declarationType = DeclarationType::simple;
    }
    else if (pInitializer != nullptr && pArraySizeExpr == nullptr)
    {
        _declarationType = DeclarationType::initialized;
    }
    else if (pInitializer == nullptr && pArraySizeExpr != nullptr)
    {
        _declarationType = DeclarationType::arrayDecl;
    }
    else
    {
        // This means a bug in the bison definition
        Assert(false);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsCloneCollection
//
//  Synopsis:   Initialize from another node, given the collection
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::InitializeAsCloneCollection(
    __in CollectionNode* pOriginalColl,                 // Node being cloned
    __inout CModernParseTreeNodeArray &rgChildClones    // Clones of children
    )
{
    CHK_START;

    // Add all of the clone children
    for (UINT i = 0; i < rgChildClones.GetCount(); i++)
    {
        CHK(AppendChild(rgChildClones[i]));
    }

    InitDeclaratorListEntryNode* pOriginalList = pOriginalColl->GetAs<InitDeclaratorListEntryNode>();
    _declarationType = pOriginalList->_declarationType;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output HLSL for this node of the tree.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::OutputHLSL(__in IStringStream* pOutput)
{
    return OutputTranslation(pOutput, nullptr);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputTranslation
//
//  Synopsis:   Output HLSL for this node of the tree, possibly with some
//              modifications to the identifier name.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::OutputTranslation(
    __in IStringStream* pOutput,                        // Stream to output translation to
    __in_z_opt const char* pszIdentifierOverride        // String to override identifier name with
    )
{
    CHK_START;

    // First output the identifier
    if (pszIdentifierOverride == nullptr)
    {
        CHK(GetChild(0)->OutputHLSL(pOutput));
    }
    else
    {
        CHK(pOutput->WriteString(pszIdentifierOverride));
    }

    TSmartPointer<CVariableIdentifierInfo> spInfo;
    CHK(GetIdentifierNode()->GetVariableIdentifierInfo(&spInfo));

    // Then the array stuff if we have it
    char szArraySuffix[64]; 
    if (_declarationType == DeclarationType::arrayDecl)
    {
        Assert(spInfo->UseType()->IsArrayType());
        CHK(StringCchPrintfA(szArraySuffix, ARRAYSIZE(szArraySuffix), "[%d]", _arraySize));
        CHK(pOutput->WriteString(szArraySuffix));
    }
    else
    {
        AssertSz(!spInfo->UseType()->IsArrayType(), "Array declaration should have caused us to construct an array type");
        szArraySuffix[0] = '\0';
    }

    // When in the declaration, append the semantic for things that need it
    if (spInfo->HasSemantic())
    {
        // If we are grabbing the semantic, make sure that the index is sensible
        CHKB(spInfo->GetHLSLSemanticCount() > _uHLSLNameIndex);

        CHK(pOutput->WriteString(":"));

        if (pszIdentifierOverride == nullptr)
        {
            CHK(pOutput->WriteString(spInfo->GetHLSLSemantic(_uHLSLNameIndex)));
        }
        else
        {
            // When overriding, the name and semantic are the same
            CHK(pOutput->WriteString(pszIdentifierOverride));
        }
    }

    // If we have an initializer then do that too
    if (_declarationType == DeclarationType::initialized)
    {
        CHK(pOutput->WriteChar('='));

        CHK(GetInitializerNode()->OutputHLSL(pOutput));
    }
    else
    {
        // All things need to be initialized to zero - in this path, there
        // was not an initializer to translate. So we need to figure out
        // if we will force an initializer.
        //
        // We do not want to initialize inputs and outputs to shaders.
        // And texture types like samplers have no real values we can initialize to.
        //
        // Everything else we initialize based on the declarator type.
        if (spInfo->GetTypeQualifier() != ATTRIBUTE && 
            spInfo->GetTypeQualifier() != VARYING)
        {
            GLSLType* pType = spInfo->UseType();
            // Don't provide default initializers for samplers
            if (!pType->IsSampler2DType() && !pType->IsSamplerCubeType())
            {
                if (pType->IsBasicType())
                {
                    BasicGLSLType* pBasicType = pType->AsBasicType();
                    PCSTR pszDefaultInit;
                    CHK(pBasicType->GetDefaultInitValue(&pszDefaultInit));

                    // Write the default initializer
                    CHK(pOutput->WriteChar('=')); 
                    CHK(pOutput->WriteString(pszDefaultInit));
                }
                else if (pType->IsArrayType())
                {
                    Assert(_declarationType == DeclarationType::arrayDecl);
                    TSmartPointer<GLSLType> spTypeElement;
                    CHK_VERIFY(SUCCEEDED(pType->GetArrayElementType(&spTypeElement)));
                    PCSTR pszTypeName;
                    if (spTypeElement->IsBasicType())
                    {
                        pszTypeName = spTypeElement->AsBasicType()->GetHLSLTypeName();
                    }
                    else if (spTypeElement->IsStructType())
                    {
                        pszTypeName = spTypeElement->AsStructType()->UseTypeNameInfo()->GetHLSLName(0);
                    }
                    else
                    {
                        CHK_VERIFY_MSG(false, "Array of array type should not have been constructed");
                    }
                    CHK(pOutput->WriteString("=("));
                    CHK(pOutput->WriteString(pszTypeName));
                    CHK(pOutput->WriteString(szArraySuffix));
                    CHK(pOutput->WriteString(")0"));
                }
                else
                {
                    // Struct types also require initialization to 0. We accomplish this in HLSL 
                    // by doing a typecast (not available in GLSL) of 0. For example:
                    //      typename var=(typename)0;
                    // which 0-inits all the fields.
                    const CTypeNameIdentifierInfo* pTypeNameInfo = pType->AsStructType()->UseTypeNameInfo();
                    CHK(pOutput->WriteString("=("));
                    CHK(pOutput->WriteString(pTypeNameInfo->GetHLSLName(0)));
                    CHK(pOutput->WriteString(")0"));
                }
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetIdentifierNode
//
//  Synopsis:   Type safe accessor for the identifier child
//
//-----------------------------------------------------------------------------
VariableIdentifierNode* InitDeclaratorListEntryNode::GetIdentifierNode()
{
    return GetChild(0)->GetAs<VariableIdentifierNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   GetInitializerNode
//
//  Synopsis:   Type safe accessor for the initializer child
//
//-----------------------------------------------------------------------------
ParseTreeNode* InitDeclaratorListEntryNode::GetInitializerNode()
{
    return GetChild(1);
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Ensures that the initializer matches the type of the identifier
//              if there is an initializer.
//
//              Also adds the variable to the identifier table, since it is
//              the first point in the tree where the information for this
//              has been accumulated.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::VerifySelf()
{
    CHK_START;

    AssertSz(_arraySize == -1, "_arraySize should not be set prior to verifying a node");

    // The initial value - it might not get filled
    ConstantValue initialValue;

    ParseTreeNode* pParent = GetParent();
    CHKB(pParent != nullptr);

    CGLSLParser* pParser = GetParser();

    InitDeclaratorListNode* pInitDeclaratorList = pParent->GetAs<InitDeclaratorListNode>();
    TSmartPointer<GLSLType> spTypeSpecified;
    CHK(pInitDeclaratorList->GetSpecifiedType(&spTypeSpecified));
    AssertSz(spTypeSpecified != nullptr, "The InitDeclList's type should be populated from its fullyspecifiedtype child before processing its entry children");

    // If we have an array declaration, then get the size of the array first
    // because we will need this to construct the type of the declaration.
    if (_declarationType == DeclarationType::arrayDecl)
    {
        // Cannot have arrays of constants or attributes
        switch (pInitDeclaratorList->GetTypeQualifier())
        {
        case CONST_TOK:
        case ATTRIBUTE:
            CHK(pParser->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INVALIDARRAYQUALIFIER, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
            break;
        }

        // Arrays have to be declared with a constant size
        CHK(VerificationHelpers::VerifyArrayConstant(
            GetChild(2),
            pParser,
            &pInitDeclaratorList->GetLocation(),
            &_arraySize
            ));
    }
    else if (_declarationType == DeclarationType::initialized)
    {
        // Only const and non type-qualified types (precision qualifiers are ok) can be initialized.
        int qualifier = pInitDeclaratorList->GetTypeQualifier();
        if (qualifier != NO_QUALIFIER && qualifier != CONST_TOK)
        {
            CHK(pParser->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INVALIDINITQUALIFIER, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        // We need to detect whether the initializer is const, regardless if the declaration
        // itself is const qualified (see call to SetHasNonConstGlobalInitializer below).
        // However, we do NOT try to retrieve the value, since that can cause failures we
        // don't see when just testing whether the expression is const. Below, we do retrieve
        // the value for basic types that we care to propagate constant values.
        bool fConstInitializer;
        CHK(GetInitializerNode()->IsConstExpression(/*fIncludeIndex*/false, &fConstInitializer, /*value*/nullptr));

        // The value being initialized to needs to be a constant value itself if the value is const
        // We just require that this be a valid constant expression - we don't need to evaluate it
        if (qualifier == CONST_TOK && !fConstInitializer)
        {
            CHK(pParser->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_NONCONSTINITIALIZER, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        int basicType;
        if (spTypeSpecified->GetBasicType(&basicType) == S_OK)
        {
            // Initialized ints and floats need to propagate constant value from the initializer
            switch (basicType)
            {
            case FLOAT_TOK:
            case INT_TOK:
                {
                    // We will try to get the value here - if we cannot get the value because of some kind
                    // of error, then this will fail. Divide by zero could cause this. Even though this succeeds,
                    // we might fail to get the right type but we will detect that later.
                    bool fBasicConstInitializer;
                    CHK(GetInitializerNode()->IsConstExpression(/*fIncludeIndex*/false, &fBasicConstInitializer, &initialValue));
                    break;
                }
            }
        }

        // Notify the parser of any non-const global initializers; these may need to
        // eventually be transformed before outputting HLSL.
        if (pInitDeclaratorList->IsGlobalScope() && !fConstInitializer)
        {
            pParser->SetHasNonConstGlobalInitializer();
        }
    }
    else if (_declarationType == DeclarationType::simple)
    {
        // Const variables are required to be initialized.
        if (pInitDeclaratorList->GetTypeQualifier() == CONST_TOK)
        {
            CHK(pParser->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_CONSTREQUIRESINITIALIZATION, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    CHK(VerifyTypeForQualifier(pInitDeclaratorList->GetTypeQualifier(), spTypeSpecified));

    // Construct the type from the declaration - the basic type comes from the 
    // type specified for the list and the arrayness comes from the entry itself.
    TSmartPointer<GLSLType> spNewType;
    CHK(GLSLType::CreateFromType(
        spTypeSpecified,
        _arraySize,
        &spNewType
        ));

    // If we have a nonzero HLSL name index, then this is a node that was created by cloning. We 
    // don't want to add the identifier or set the info because these things won't work or are already done.
    if (_uHLSLNameIndex == 0)
    {
        // Now add to the identifier table
        TSmartPointer<CVariableIdentifierInfo> spNewInfo;
        CHK(pParser->UseIdentifierTable()->AddVariableIdentifier(
            GetIdentifierNode(), 
            spNewType, 
            pInitDeclaratorList->GetTypeQualifier(),
            pInitDeclaratorList->GetPrecisionQualifier(),
            false,          // Not a parameter declared identifier
            initialValue,
            &spNewInfo
            ));

        // Set the info for the identifier now that it is created
        GetIdentifierNode()->SetVariableIdentifierInfo(spNewInfo);

        if (pParent->GetParent()->GetParseNodeType() == ParseNodeType::forStatement)
        {
            // This variable is declared in a loop, so mark it as thus
            spNewInfo->SetIsLoopDeclared();
        }
    }

    if (_declarationType == DeclarationType::initialized)
    {
        // Now check that the declaration matches the type of the identifier
        TSmartPointer<GLSLType> spChildType;
        CHK(GetInitializerNode()->GetExpressionType(&spChildType));

        if (!spNewType->IsEqualType(spChildType))
        {
            CHK(pParser->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INCOMPATIBLETYPES, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);            
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLNameIndex
//
//  Synopsis:   Set whether to use the sampler or texture HLSL name for
//              identifiers in this expression. For this node, that will affect
//              the semantic that is used.
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::SetHLSLNameIndex(UINT uIndex)
{
    _uHLSLNameIndex = uIndex;

    return CollectionNode::SetHLSLNameIndex(uIndex);
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::GetDumpString(__in IStringStream* pOutput)
{
    return pOutput->WriteFormat(1024, "InitDeclaratorListEntry arraySize=%d", static_cast<int>(_arraySize));
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyTypeForQualifier
//
//  Synopsis:   Verifies the qualifier and basicType combination is
//              compatible.
//              
//-----------------------------------------------------------------------------
HRESULT InitDeclaratorListEntryNode::VerifyTypeForQualifier(int qualifier, __in const GLSLType* pType) const
{
    CHK_START;

    InitDeclaratorListNode* pInitDeclaratorList = GetParent()->GetAs<InitDeclaratorListNode>();

    if (pType->IsBasicType())
    {
        int basicType;
        CHK_VERIFY(pType->GetBasicType(&basicType) == S_OK);

        // Sampler types can only be uniform (no attribute, varying, const, or unqualified).
        if (TypeHelpers::IsTextureType(basicType))
        {
            if (qualifier != UNIFORM)
            {
                CHK(GetParser()->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INVALIDSAMPLERQUALIFIER, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }

        // Ensure that a declared attribute or varying is a valid type. Valid types are a fixed list in the
        // spec - they all have component type of float.
        if (qualifier == ATTRIBUTE || qualifier == VARYING)
        {
            int componentType;
            if (TypeHelpers::GetComponentType(basicType, &componentType) != S_OK ||
                componentType != FLOAT_TOK
                )
            {
                CHK(GetParser()->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INVALIDINPUTDATATYPE, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
        }
    }
    else
    {
        AssertSz(pType->IsStructType(), "Unexpected to have an array type at this point (specified type will only become array type once this node is verified)");
        switch (qualifier)
        {
        case ATTRIBUTE:
        case VARYING:
            CHK(GetParser()->LogError(&pInitDeclaratorList->GetLocation(), E_GLSLERROR_INVALIDTYPEQUALIFIERFORSTRUCT, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
            break;
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetVerifiedArraySize
//
//  Synopsis:   Generated declarator entries need to have their array-ness
//              set to a known size in order to output it. Generated nodes
//              do not go through the normal verification process as the
//              variable identifier info is precomputed and known.
//
//+----------------------------------------------------------------------------
void InitDeclaratorListEntryNode::SetVerifiedArraySize(int arraySize)
{
    AssertSz(_arraySize == -1, "This should only be set once");
    AssertSz(arraySize > 0, "An array of size < 0 is not valid, even when manually setting the array size");
    AssertSz(_declarationType == DeclarationType::arrayDecl, "Why is the array size being set for a declarator that is not an array?");

    _arraySize = arraySize;
}

//+----------------------------------------------------------------------------
//
//  Function:   ExtractInitializer
//
//  Synopsis:   Assumes this entry is an initialized one. Removes the
//              initializer expression child and sets the declaration
//              type to simple.
//
//+----------------------------------------------------------------------------
void InitDeclaratorListEntryNode::ExtractInitializer(const TSmartPointer<ParseTreeNode>& spInitExpr)
{
    requires(_declarationType == DeclarationType::initialized);

    requires(ExtractChild(spInitExpr) == S_OK);

    _declarationType = DeclarationType::simple;
}
