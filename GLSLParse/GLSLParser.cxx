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
#include "GLSLParser.hxx"
#include "IStringStream.hxx"
#include "RefCounted.hxx"
#include "ParseTree.hxx"
#include "SamplerCollectionNode.hxx"
#include "GLSLUnicodeConverter.hxx"
#include "GLSLPreprocess.hxx"
#include "GLSLStreamParserInput.hxx"
#include "GLSLStringParserInput.hxx"
#include "KnownSymbols.hxx"
#include "FeatureControlHelper.hxx"
#include "WebGLConstants.hxx"
#include "DeclarationSamplerNodeWrapper.hxx"
#include "ParameterSamplerNodeWrapper.hxx"
#include "StructSpecifierCollectionNode.hxx"
#include "StructGLSLType.hxx"
#include "TypeNameIdentifierInfo.hxx"
#include "ArrayGLSLType.hxx"

#pragma warning(disable:28718)
#include "lex.GLSL.h"

#pragma warning(default:28718)

const UINT CGLSLParser::s_uMaxShaderSize = 131072;              // Maximum size of input to GLSL parser
const char* CGLSLParser::s_pszMaxShaderSizeString = "131072";   // Max size as string
const YYLTYPE CGLSLParser::s_nullLocation = {0};                // Placeholder location for generated parse tree nodes

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLParser::CGLSLParser() : 
    _realLine(1),
    _logicalLine(1),
    _column(1),
    _shaderType(GLSLShaderType::Vertex),
    _currentScopeId(1),                     // Root scope is always has the 0 id
    _generatedIdentifierId(0),
    _fErrors(false),
    _fWriteInputs(false),
    _fWriteBoilerPlate(true),
    _uFeaturesUsed(0),
    _glFeatureLevel(WebGLFeatureLevel::Level_9_1),
    _fHasNonConstGlobalInitializers(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Initialize the tokenizer and make our copy of the input.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::Initialize(
    __in BSTR bstrInput,                                        // Input unicode text of shader
    GLSLShaderType::Enum shaderType,                            // Type of shader to translate
    UINT uOptions,                                              // Translation options
    WebGLFeatureLevel glFeatureLevel                            // Feature level to translate for
    )
{
    CHK_START;

    _shaderType = shaderType;
    _fWriteInputs = (uOptions & GLSLTranslateOptions::DisableWriteInputs) == 0;
    _fWriteBoilerPlate = (uOptions & GLSLTranslateOptions::DisableBoilerPlate) == 0;

    if ((uOptions & GLSLTranslateOptions::ForceFeatureLevel9) != 0)
    {
        _glFeatureLevel = WebGLFeatureLevel::Level_9_1;
    }
    else
    {
        _glFeatureLevel = glFeatureLevel;
    }

    CHK(RefCounted<CGLSLSymbolTable>::Create(/*out*/_spSymbolTable));

    // Seed the symbol table with the known symbols so that the number of known symbols in
    // the table is known. This is used to make the output of the variables more predictable.
    for (int i = 0; i < GLSLSymbols::count; i++)
    {
        GLSLSymbols::Enum known = static_cast<GLSLSymbols::Enum>(i);
        const GLSLSymbolInfo &info = GLSLKnownSymbols::GetKnownInfo<GLSLSymbolInfo>(known);

        int symbolIndex;
        CHK(_spSymbolTable->EnsureSymbolIndex(info._pGLSLName, &symbolIndex));

        // Make sure that casting GLSL function enums to int gives its index, because we
        // have code that makes use of this fact to do fast comparisons.
        Assert(static_cast<int>(known) == symbolIndex);
    }

    // Create the object we will ultimately return back
    CHK(RefCounted<CGLSLConvertedShader>::Create(/*out*/_spConverted));

    // We need to convert to Ansi for our parser to work
    TSmartPointer<CMemoryStream> spConvertedInput;
    CHK(CGLSLUnicodeConverter::ConvertToAscii(bstrInput, &spConvertedInput));

    // Preprocess the input - first make an input object from the converted input
    TSmartPointer<CGLSLStreamParserInput> spPreprocessInput;
    CHK(RefCounted<CGLSLStreamParserInput>::Create(spConvertedInput, /*out*/spPreprocessInput));

    // Run the preprocessor
    TSmartPointer<CMemoryStream> spPreprocessed;
    if (SUCCEEDED(::GLSLPreprocess(spPreprocessInput, _spConverted, uOptions, shaderType, &spPreprocessed, &_spLineMap, &_spExtensionState)))
    {
        UINT uSize;
        CHK(spPreprocessed->GetSize(&uSize));

        if (uSize > s_uMaxShaderSize)
        {
            CHK(LogError(nullptr, E_GLSLERROR_SHADERTOOLONG, s_pszMaxShaderSizeString));
        }
        else
        {
            // Make an input object from the preprocessor output
            CHK(RefCounted<CGLSLStreamParserInput>::Create(spPreprocessed, /*out*/_spInput));

            // Before we start, check the line map so that a #line on the first line will have the right effect
            _spLineMap->AdjustLogicalLine(_realLine, &_logicalLine);

            // Kick off the parser
            yyscan_t scanner;
            GLSLlex_init(&scanner);
            GLSLset_extra(this, scanner);
            GLSLparse(scanner);
            GLSLlex_destroy(scanner);
        }
    }
    else
    {
        // The preprocessor should have tried to log errors for us
        _fErrors = true;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   NotifyError
//
//  Synopsis:   Method called when GLSLerror happens, which is essentially
//              used for syntax errors or other rare errors that Bison will
//              report.
//
//              Verification failures will report directly to LogError.
//
//-----------------------------------------------------------------------------
void CGLSLParser::NotifyError(__in YYLTYPE* pLocation, __in_z const char* error)
{
    if (strcmp(error, "memory exhausted") == 0)
    {
        // Report stack exhaustion as shader complexity error. This is the same
        // error that we report if we detect the max depth of the parse tree
        // has been exceeded.
        LogError(pLocation, E_GLSLERROR_SHADERCOMPLEXITY, nullptr);
    }
    else
    {
        // Anything besides stack exhaustion will be reported as a syntax error.
        LogError(pLocation, E_GLSLERROR_SYNTAXERROR, error);
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   UpdateLocation
//
//  Synopsis:   Called to pass along location information.
//
//-----------------------------------------------------------------------------
void CGLSLParser::UpdateLocation(__in YYLTYPE* pLocation, int lineNo, int tokenLength)
{
    pLocation->first_line = pLocation->last_line = _logicalLine;
    pLocation->first_column = _column;    
    pLocation->last_column = _column + tokenLength - 1;

    _column += tokenLength;
}

//+----------------------------------------------------------------------------
//
//  Function:   RecordNewline
//
//  Synopsis:   Called when encountering a newline. This resets the column
//              location counter, and increments the line counters.
//
//-----------------------------------------------------------------------------
void CGLSLParser::RecordNewline()
{
    // Reset column counter and increment line counter
    _realLine++;
    _logicalLine++;

    // Adjust the logical line based on the line map
    _spLineMap->AdjustLogicalLine(_realLine, &_logicalLine);
            
    _column = 1;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateTree
//
//  Synopsis:   Function called on an assumed correctly built tree to do
//              translation to HLSL.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateTree(
    bool fWriteInputs,                                          // Flag used in testing to suppress input structures from output
    __deref_out CMemoryStream** ppConverted                     // Stream with converted HLSL
    )
{
    CHK_START;

    // We need to make sure we even have a root node - if not, then the parser failed in
    // some fundamental way and we should not be here.
    Assert(_spRootNode != nullptr);
    CHKB(_spRootNode != nullptr);

    TSmartPointer<CMemoryStream> spConvertedStream;
    CHK(RefCounted<CMemoryStream>::Create(/*out*/spConvertedStream));

    // Initialize the identifier table
    CHK(RefCounted<CGLSLIdentifierTable>::Create(this, /*out*/_spIdTable));

    // Give it to the converted shader
    _spConverted->SetIdentifierTable(_spIdTable);

    // Do the type verification pass
    CHK(_spRootNode->VerifyNode());

    // Verify the inputs
    CHK(VerifyInputs());

    // Now that everything has passed verification, we do various transformations. The order
    // of these is important, because some of them transform stuff done in previous stages.
    // For example, we move short-circuit initializer expressions before ensuring short-cirtuiting
    // is respected and thus don't have to worry about doing so in the global scope.
    if (_fHasNonConstGlobalInitializers)
    {
        CHK(TranslateGlobalDeclarations());
    }

    CHK(TranslateStructDeclarations());
    CHK(TranslateShortCircuitExpressions());

    // Translate the samplers
    CHK(TranslateSamplers());

    if (fWriteInputs)
    {
        // Output the HLSL inputs
        CHK(TranslateInputs(spConvertedStream));
    }

#if DBG
    // Translation has been completed. At this point all nodes in the tree must be verified
    _spRootNode->AssertSubtreeFullyVerified();
#endif
    

    // Start at the root and work down...
    CHK(_spRootNode->OutputHLSL(spConvertedStream));

    (*ppConverted) = spConvertedStream.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateGlobalDeclarations
//
//  Synopsis:   Global declarations that have initializer expressions can
//              depend on shader input variables (GLSL: attribute/varying, 
//              HLSL: vsInput/psInput). In HLSL, vsInput and psInput are only
//              accessible as passed parameters to the entry point function.
//              We immediately assign these parameters to a static global as
//              the first thing in main() (see WriteEntryPointBegin) but if
//              global variables depend on these values, they must not be
//              initialized until after this happens. In order to facilitate
//              this we perform the following steps:
//                  1.) generate a function that is declared just before the
//                      entry point and defined at the very end of the shader
//                  2.) move all global initializer expressions into assignment
//                      statements inside that function definition
//                  3.) call this initialization function as the first statement
//                      in main() which will then execute after the shader
//                      input variables are correctly initialized.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateGlobalDeclarations()
{
    CHK_START;

    // Before we do this translation, we need to figure out the entry point function
    // where we're going to inject the function call statement. This could potentially
    // be null if there are no functions, and will be the last function in the shader
    // if main() is not defined (this is mainly for testability in our unit tests).
    TSmartPointer<FunctionDefinitionNode> spEntryPoint;
    CHK(DetermineEntryPoint(&spEntryPoint));
    if (spEntryPoint != nullptr)
    {
        TSmartPointer<FunctionPrototypeDeclarationNode> spGlobalInitializerFunctionDeclaration;
        TSmartPointer<FunctionDefinitionNode> spGlobalInitializerFunctionDefinition;
        int iFunctionIdent;
        TSmartPointer<CollectionNode> spFunctionDefinitionStatementList;
        CHK(GenerateFunctionDeclarationAndDefinition(
            &spGlobalInitializerFunctionDeclaration,
            &spGlobalInitializerFunctionDefinition,
            &iFunctionIdent,
            &spFunctionDefinitionStatementList
            ));

        CHK(MoveGlobalInitializersIntoStatementList(spFunctionDefinitionStatementList));

        // Insert the init function declaration just before the entry point function definition;
        CHK(_spRootNode->InsertBefore(spGlobalInitializerFunctionDeclaration, spEntryPoint));

        // Insert the init function definition at the end of the shader (append as a child to root node)
        CHK(_spRootNode->AppendChild(spGlobalInitializerFunctionDefinition));

        // ...and verify the nodes once they are attached
        requires(SUCCEEDED(spGlobalInitializerFunctionDeclaration->VerifyNode()));
        requires(SUCCEEDED(spGlobalInitializerFunctionDefinition->VerifyNode()));

        // Last step is to call the function at the beginning of the entry point.
        CHK(AddFunctionCallToEntryPointBegin(spEntryPoint, iFunctionIdent));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MoveGlobalInitializersIntoStatementList
//
//  Synopsis:   Iterates through each declaration in the global scope and moves
//              any non-const initializer expressions into the passed in
//              statement list. Performs this move by cloning the identifier of
//              the entry being intialized and constructing an assignment
//              statement that is put into the statement list.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::MoveGlobalInitializersIntoStatementList(__in CollectionNode* pStatementList)
{
    CHK_START;

    requires(_fHasNonConstGlobalInitializers);

    for (UINT i = 0; i < _spRootNode->GetChildCount(); i++)
    {
        ParseTreeNode* pChild = _spRootNode->GetChild(i);
        if (pChild->GetParseNodeType() == ParseNodeType::initDeclaratorList)
        {
            InitDeclaratorListNode* pInitDeclList = pChild->GetAs<InitDeclaratorListNode>();
            if (pInitDeclList->GetTypeQualifier() != CONST_TOK)
            {
                // For each non-const declaration, look at each decl entry for non-const initializer expressions
                for (UINT j = 0; j < pInitDeclList->GetIdentifierCount(); j++)
                {
                    InitDeclaratorListEntryNode* pEntry = pInitDeclList->GetEntry(j);
                    if (pEntry->GetDeclarationType() == DeclarationType::initialized)
                    {
                        TSmartPointer<ParseTreeNode> spInitializer(pEntry->GetInitializerNode());
                        bool fIsConstInitializer;
                        CHK(spInitializer->IsConstExpression(false, &fIsConstInitializer, nullptr));
                        if (!fIsConstInitializer)
                        {
                            // For each one that is initialized, clone the variable id node, and generate
                            // an assigment statement to append to the statement list.
                            TSmartPointer<ParseTreeNode> spClonedIdentifier;
                            CHK(pEntry->GetIdentifierNode()->Clone(&spClonedIdentifier));

                            pEntry->ExtractInitializer(spInitializer);
                            TSmartPointer<ParseTreeNode> spAssignStatement;
                            CHK(GenerateAssignmentStatement(spInitializer, spClonedIdentifier->GetAs<VariableIdentifierNode>(), &spAssignStatement));

                            CHK(pStatementList->AppendChild(spAssignStatement));
                        }
                    }
                }
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddFunctionCallToEntryPointBegin
//
//  Synopsis:   Given an entry point function definition and a symbol of a
//              function to call, adds a function call to that function as
//              the first statement in the function body's statement list.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::AddFunctionCallToEntryPointBegin(
    const FunctionDefinitionNode* pEntryPoint,                          // The entry point function definition where the function call statement should be added
    int iFunctionIdent                                                  // The identifier of the function to call
    )
{
    CHK_START;

    // Create the nodes that correspond with a function call
    TSmartPointer<FunctionCallIdentifierNode> spFunctionCallIdentifier;
    CHK(RefCounted<FunctionCallIdentifierNode>::Create(this, iFunctionIdent, s_nullLocation, /*out*/spFunctionCallIdentifier));

    TSmartPointer<FunctionCallHeaderNode> spFunctionCallHeader;
    CHK(RefCounted<FunctionCallHeaderNode>::Create(this, spFunctionCallIdentifier, /*out*/spFunctionCallHeader));

    TSmartPointer<FunctionCallHeaderWithParametersNode> spFunctionCallHeaderWithParams;
    CHK(RefCounted<FunctionCallHeaderWithParametersNode>::Create(this, spFunctionCallHeader, /*noargs*/nullptr, s_nullLocation, /*out*/spFunctionCallHeaderWithParams));

    TSmartPointer<FunctionCallGenericNode> spFunctionCallGeneric;
    CHK(RefCounted<FunctionCallGenericNode>::Create(this, spFunctionCallHeaderWithParams, /*out*/spFunctionCallGeneric));

    // Merge the function call into a statement
    TSmartPointer<ExpressionStatementNode> spFunctionCallStatement;
    CHK(RefCounted<ExpressionStatementNode>::Create(this, spFunctionCallGeneric, /*out*/spFunctionCallStatement));

    CompoundStatementNode* pFunctionBody = pEntryPoint->GetFunctionBody()->GetAs<CompoundStatementNode>();
    StatementListNode* pEntryPointStatementList = pFunctionBody->GetChild(0)->GetAs<StatementListNode>();
    CHK(pEntryPointStatementList->InsertChild(spFunctionCallStatement, 0));

    // All nodes are required to be verified before outputting their HLSL.
    requires(SUCCEEDED(spFunctionCallStatement->VerifyNode()));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   DetermineEntryPoint
//
//  Synopsis:   Determines the entry point function. This could potentially
//              be null if there are no functions in the shader, and will be
//              the last function in the shader if main() is not defined.
//              For our unit tests, many do not define main() as that brings in
//              a certain amount of boilerplate. Using a different function as
//              the entry point helps us keep testability without forcing
//              the test to have main() defined.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::DetermineEntryPoint(__deref_out_opt FunctionDefinitionNode** ppEntryPoint)
{
    TSmartPointer<FunctionDefinitionNode> spFoundEntryPoint;
    if (_spEntryPoint == nullptr)
    {
        // Function definitions can only happen in the global scope; look for them there.
        for (UINT i = _spRootNode->GetChildCount(); i > 0; i--)
        {
            ParseTreeNode* pChild = _spRootNode->GetChild(i - 1);
            if (pChild->GetParseNodeType() == ParseNodeType::functionDefinition)
            {
                spFoundEntryPoint = pChild->GetAs<FunctionDefinitionNode>();
                break;
            }
        }
    }
    else
    {
        spFoundEntryPoint = _spEntryPoint;
    }

    *ppEntryPoint = spFoundEntryPoint.Extract();

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   Translate
//
//  Synopsis:   Outputs HLSL from the parsed tree into the member stream.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::Translate(
    __deref_out CGLSLConvertedShader** ppConvertedShader        // Converted shader object
    )
{
    CHK_START;

    // If no errors were found until now, then try to do conversion to HLSL
    TSmartPointer<CMemoryStream> spConvertedStream;
    if (!_fErrors)
    {
        hr = TranslateTree(_fWriteInputs, &spConvertedStream);
        if (SUCCEEDED(hr))
        {
            // Conversion has succeeded - store the output to return
            _spConverted->SetConverterOutput(spConvertedStream, _spVaryingStructInfo);
        }
        else
        {
            if (hr != E_GLSLERROR_KNOWNERROR)
            {
                // We hit an error, but it was not something that was known
                // and logged. We want to make sure that something is still logged
                // to be somewhat helpful. Over time we should minimize this message
                // because it is not very helpful to people.
                CHK(LogError(nullptr, E_GLSLERROR_INTERNALERROR, nullptr));
            }

            // Eat the error
            hr = S_OK;

            // Now we should be in a state where we have errors to report, because
            // all known errors should be logging and unknown errors record an
            // internal error.
            Assert(_fErrors);
        }
    }

    _spConverted.CopyTo(ppConvertedShader);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddDeclaratorList
//
//  Synopsis:   Called when declarator lists are instantiated in the tree to
//              collect them here.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::AddDeclaratorList(
    InitDeclaratorListNode* pNewDecl                            // The declarator to add
    )
{
    return _aryDeclarations.Add(pNewDecl);
}

//+----------------------------------------------------------------------------
//
//  Function:   AddShortCircuitExpression
//
//  Synopsis:   Add to the global list of short circuit expressions. The only
//              callers should be ConditionalExpressionNode::PreVerifyChildren
//              and BinaryOperatorNode::PreVerifyChildren which guarantee these 
//              are added in execution order.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::AddShortCircuitExpression(
    __in ParseTreeNode* pExpr                                           // The expression to add
    )
{
    Assert(pExpr->IsShortCircuitExpression());

    return _aryShortCircuitExprs.Add(pExpr);
}

//+----------------------------------------------------------------------------
//
//  Function:   SetEntryPointNode
//
//  Synopsis:   Sets a pointer to the definition of 'main()' on parser for
//              quick access during translation.
//
//+----------------------------------------------------------------------------
void CGLSLParser::SetEntryPointNode(__in FunctionDefinitionNode* pEntryPoint)
{
    Assert(_spEntryPoint == nullptr);
    _spEntryPoint = pEntryPoint;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateIdentifierNode
//
//  Synopsis:   Generates a Node of type T, assuming that T derives from
//              IdentifierNodeBase and is initialized with a symbol index
//              and location. We generate a reserved symbol (leading __) that
//              ensures we don't have name collisions with existing GLSL
//              identifiers.
//
//+----------------------------------------------------------------------------
template <typename T>
HRESULT CGLSLParser::GenerateIdentifierNode(__deref_out T** ppIdentifier)
{
    CHK_START;
    int iSymbolIndex;
    CHK(GenerateIdentifierSymbol(&iSymbolIndex));
    CHK(GenerateIdentifierNodeFromSymbol(iSymbolIndex, ppIdentifier));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateIdentifierNodeFromSymbol
//
//  Synopsis:   Given a symbol index, generates an identifier node
//
//+----------------------------------------------------------------------------
template <typename T>
HRESULT CGLSLParser::GenerateIdentifierNodeFromSymbol(int iSymbolIndex, __deref_out T** ppIdentifier)
{
    CHK_START;

    TSmartPointer<T> spIdentifierNode;
    CHK(RefCounted<T>::Create(this, iSymbolIndex, s_nullLocation, /*out*/spIdentifierNode));

    // Mark the new identifier as being generated so verification code allows the leading
    // underscores - this is normally rejected, which is why we can use it internally.
    spIdentifierNode->SetIsGenerated();

    *ppIdentifier = spIdentifierNode.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateIdentifierSymbol
//
//  Synopsis:   Generates a glsl symbol index using our unique identifier id
//              counter combined with the reserved "__tmp_" prefix.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::GenerateIdentifierSymbol(__out int* piIdentifier)
{
    CHK_START;

    CMutableString<char> spszReservedName;
    CHK(spszReservedName.Format(20, "__tmp_%d", GenerateIdentifierId()));

    CHK(_spSymbolTable->EnsureSymbolIndex(static_cast<const char*>(spszReservedName), piIdentifier));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GeneratePlaceholderDeclaration
//
//  Synopsis:   Generates a declaration for a placeholder variable that has the
//              same type as the given expression. It performs this by creating
//              the necessary bits to have a fully formed InitDeclaratorListNode.
//              Use this method if you are moving an expression, and need to sub back
//              in the result of the expression in the original location.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::GeneratePlaceholderDeclaration(
    __in GLSLType* pExprType,                                           // Type of an expression for which a placeholder variable needs to be generated
    __in_opt ParseTreeNode* pInitializer,                               // Optional initializer to init the placeholder variable with
    __deref_out InitDeclaratorListNode** ppInitDeclaratorListNode       // The generated declaration. Must be verified after attaching to the tree for the variable to be considered as declared and in scope
    )
{
    CHK_START;

    // Generate a reserved identifier node; this identifier will be used to declare the placeholder variable.
    TSmartPointer<VariableIdentifierNode> spVariableIdent;
    CHK(GenerateIdentifierNode(&spVariableIdent));

    // Generate a fully specified type - the type is the type of the expression passed in, potentially with
    // the arrayness stripped off (we handle the arrayness separately by creating an IntConstant for the array specification)
    TSmartPointer<TypeSpecifierNode> spTypeSpecifierNode;
    CHK(TypeSpecifierNode::CreateNodeFromType(this, pExprType, &spTypeSpecifierNode));

    // The fully specified type has no qualifier - it is just a local variable.
    TSmartPointer<ParseTreeNode> spFullySpecifiedType;
    CHK(RefCounted<FullySpecifiedTypeNode>::Create(this, NO_QUALIFIER, spTypeSpecifierNode, /*out*/spFullySpecifiedType));

    // If the type is an array type, we'll need an extra node to specify this in the InitDeclaratorListNode.
    TSmartPointer<IntConstantNode> spIntConstant;
    const bool fIsArrayType = pExprType->IsArrayType();
    if (fIsArrayType)
    {
        CHK(RefCounted<IntConstantNode>::Create(this, pExprType->AsArrayType()->GetElementCount(), /*out*/spIntConstant));
    }

    // Generated nodes do not need a location for error reporting; we're past the point of verification now.
    TSmartPointer<InitDeclaratorListNode> spInitDecl;
    CHK(RefCounted<InitDeclaratorListNode>::Create(
        this,
        spFullySpecifiedType, 
        spVariableIdent,
        s_nullLocation,
        pInitializer,
        spIntConstant,
        /*out*/spInitDecl
        ));

    *ppInitDeclaratorListNode = spInitDecl.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateAssignmentStatement
//
//  Synopsis:   Given variable identifier info and an expression, generates
//              a binaryoperator (with the assignment operator) that assigns
//              to a VariableIdentifierNode (LHS) from the expression (RHS).
//              The binaryoperator node is then used as an expression statement
//              in a statement list node of a single statement.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::GenerateAssignmentStatement(
    __in ParseTreeNode* pExpr,                                          // Expression to assign from
    __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
    __deref_out ParseTreeNode** ppAssignStatement                       // Generated assignment statement
    )
{
    CHK_START;

    TSmartPointer<ParseTreeNode> spVariableIdent;
    CHK(pPlaceholderVariable->Clone(&spVariableIdent));

    TSmartPointer<BinaryOperatorNode> spBinaryOperator;
    CHK(RefCounted<BinaryOperatorNode>::Create(this, spVariableIdent, pExpr, s_nullLocation, EQUAL, /*out*/spBinaryOperator));

    // The type of this assignment may be an array type, which is normally rejected by
    // BinaryOperatorNode at verification time. However, this must be allowed if we're
    // translating ternary operators with expression array type and the translated HLSL 
    // will compile - tell the operator to allow this.
    spBinaryOperator->SetAllowArrayAssignment();

    TSmartPointer<ExpressionStatementNode> spExprStmt;
    CHK(RefCounted<ExpressionStatementNode>::Create(this, spBinaryOperator, /*out*/spExprStmt));

    *ppAssignStatement = spExprStmt.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ConvertShortCircuitToSelection
//
//  Synopsis:   Converts expressions that need to short circuit to selection
//              statement (i.e. if/else). Used to ensure HLSL only executes
//              the 'taken' branch of an expression.
//
//              Conditional expressions (i.e. ternary) are converted as thus:
//
//                  (test) ? true : false
//
//              is converted to
//
//                  if (test) { tmp = true; } else { tmp = false; }
//
//              where tmp is described by pInfo.
//
//              Logical expressions (&& and ||) are converted as using the 
//              following equivalences:
//
//                  A && B -> if (A) { tmp = B; } else { tmp = false; }
//                  A || B -> if (A) { tmp = true; } else { tmp = B; }
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::ConvertShortCircuitToSelection(
    __in ParseTreeNode* pExpr,                                          // Short circuit expression to convert
    __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
    __deref_out ParseTreeNode** ppSelectionConverted                    // Created SelectionStatement node
    )
{
    CHK_START;

    TSmartPointer<ParseTreeNode> spTestExpr;
    TSmartPointer<ParseTreeNode> spTrueExpr;
    TSmartPointer<ParseTreeNode> spFalseExpr;

    if (pExpr->GetParseNodeType() == ParseNodeType::conditionalExpression)
    {
        ConditionalExpressionNode* pCondExpr = pExpr->GetAs<ConditionalExpressionNode>();

        // Extract the false and true branches, and the test expression in backwards order
        // (extracting is a destructive operation which moves the indices).
        CHK_VERIFY(SUCCEEDED(pCondExpr->ExtractChild(2, &spFalseExpr)));
        CHK_VERIFY(SUCCEEDED(pCondExpr->ExtractChild(1, &spTrueExpr)));
        CHK_VERIFY(SUCCEEDED(pCondExpr->ExtractChild(0, &spTestExpr)));
    }
    else if (pExpr->GetParseNodeType() == ParseNodeType::binaryOperator)
    {
        BinaryOperatorNode* pBinaryExpr = pExpr->GetAs<BinaryOperatorNode>();

        // We only support these two operators here
        Assert(pBinaryExpr->GetOperator() == AND_OP || pBinaryExpr->GetOperator() == OR_OP);

        // Pull the left and right side out - use the left side of the expression as the test
        TSmartPointer<ParseTreeNode> spRight;
        CHK_VERIFY(SUCCEEDED(pBinaryExpr->ExtractChild(1, &spRight)));
        CHK_VERIFY(SUCCEEDED(pBinaryExpr->ExtractChild(0, &spTestExpr)));

        // Per the function header comment && and || are converted as using the 
        // following equivalences:
        //     A && B -> if (A) { tmp = B; } else { tmp = false; }
        //     A || B -> if (A) { tmp = true; } else { tmp = B; }
        // Here we need to generate the bool constant for the simple
        // 'tmp' assignments above. This constant value is 'true' for || and 
        // 'false' for &&.
        const bool fBoolConstantValue = (pBinaryExpr->GetOperator() == OR_OP);
        TSmartPointer<BoolConstantNode> spConstantNode;
        CHK(RefCounted<BoolConstantNode>::Create(
            this,
            fBoolConstantValue,
            /*out*/spConstantNode
            ));

        if (pBinaryExpr->GetOperator() == AND_OP)
        {
            spTrueExpr = spRight;
            spFalseExpr = spConstantNode;
        }
        else
        {
            spTrueExpr = spConstantNode;
            spFalseExpr = spRight;
        }
    }

    CHK(CreateSelectionFromParts(spTestExpr, spTrueExpr, spFalseExpr, pPlaceholderVariable, ppSelectionConverted));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateSelectionFromParts
//
//  Synopsis:   Create a selection (if / else) from three expressions and
//              a temporary variable.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::CreateSelectionFromParts(
    __in ParseTreeNode* pTestExpr,                                      // Expression to test in if
    __in ParseTreeNode* pTrueExpr,                                      // What to do if true
    __in ParseTreeNode* pFalseExpr,                                     // What to do if false
    __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
    __deref_out ParseTreeNode** ppSelectionConverted                    // Created SelectionStatement node
    )
{
    CHK_START;

    // Next, assign both the true and false expressions to the passed in placeholder variable,
    // and put the statements into a statement list. If there's a ternary inside either branch
    // of this ternary we'll  need an insertion point.
    TSmartPointer<ParseTreeNode> spTrueStmt;
    CHK(GenerateAssignmentStatement(pTrueExpr, pPlaceholderVariable, &spTrueStmt));
    TSmartPointer<StatementListNode> spTrueStmtList;
    CHK(RefCounted<StatementListNode>::Create(this, spTrueStmt, /*out*/spTrueStmtList));

    TSmartPointer<ParseTreeNode> spFalseStmt;
    CHK(GenerateAssignmentStatement(pFalseExpr, pPlaceholderVariable, &spFalseStmt));
    TSmartPointer<StatementListNode> spFalseStmtList;
    CHK(RefCounted<StatementListNode>::Create(this, spFalseStmt, /*out*/spFalseStmtList));

    // For both true/false statements created above, create a scope around them
    // which is required by the structure of SelectionRestStatementNode
    TSmartPointer<ParseTreeNode> spTrueScopeStmt;
    CHK(RefCounted<ScopeStatementNode>::Create(this, spTrueStmtList, /*out*/spTrueScopeStmt));

    TSmartPointer<ParseTreeNode> spFalseScopeStmt;
    CHK(RefCounted<ScopeStatementNode>::Create(this, spFalseStmtList, /*out*/spFalseScopeStmt));

    TSmartPointer<ParseTreeNode> spSelectionRest;
    CHK(RefCounted<SelectionRestStatementNode>::Create(this, spTrueScopeStmt, spFalseScopeStmt, /*out*/spSelectionRest));

    TSmartPointer<ParseTreeNode> spSelectionStatement;
    CHK(RefCounted<SelectionStatementNode>::Create(this, pTestExpr, spSelectionRest, s_nullLocation, /*out*/spSelectionStatement));

    *ppSelectionConverted = spSelectionStatement.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateShortCircuitExpressions
//
//  Synopsis:   Iterates through all the short circuit expressions
//              that were verified and translates them to selection statements
//              (if else). Along the way it will lift expressions that
//              must execute before the ternary into expression statements of
//              their own. 
//
//              Also handles the case where the expression is in a
//              declaration (splits each declarator entry into it's own
//              declaration). Potentially inserts the expressions into a
//              function is the ternary is in a global declaration since we
//              cannot execute code directly.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateShortCircuitExpressions()
{
    CHK_START;

    while (_aryShortCircuitExprs.GetCount() > 0)
    {
        TSmartPointer<ParseTreeNode> spCondExpr = _aryShortCircuitExprs[0];
        _aryShortCircuitExprs.RemoveAt(0);

        // This array will hold every expression that occurs 'before' (i.e. must be executed
        // prior to) the current short circuit expression. This is in reverse order to simplify
        // adding to it while walking up the tree.
        CModernArray<TSmartPointer<ParseTreeNode>> aryOrderedExpressionsToMove;

        // Add the short circuit expression as the last expression to be moved.
        CHK(aryOrderedExpressionsToMove.Add(spCondExpr));

        ParseTreeNode* pChild = spCondExpr;
        CollectionNode* pParent = pChild->GetParent();

        while (pParent != nullptr)
        {
            if (IsValidExpressionInsertionPoint(pParent))
            {
                // At this point we've gathered all the nodes that must be executed before the
                // short circuit expression we're concerned with. We want to have the gathered
                // ordered expressions execute before the branch of the short circuit expression
                // so that they executed before it.
                CHK(MoveExpressionsToInsertionPoint(pParent, pChild, aryOrderedExpressionsToMove));

                // Once the expressions have successfully been moved, we're done with this 
                break;
            }

            // As we're walking up the tree, some GLSL nodes require their expression
            // children to be executed in order, left-to-right. If we encounter one of
            // these nodes, we'll ensure that we either modify the parse tree to keep
            // that invariant, or we'll gather the expressions so that we can ensure they
            // execute before the ternary expression that we are planning on moving.
            if (ParseTreeNode::RequiresOrderedExpressionExecution(pParent))
            {
                // Variable declarations are handled separately - each entry has
                // the effect of declaring the variable identifier (along with whatever
                // side effects are caused by the initializer expression). Because of this
                // we will actually split the declaration into multiple statements.
                if (pParent->GetParseNodeType() == ParseNodeType::initDeclaratorList)
                {
                    CHK(FixupDeclarationForShortCircuiting(pParent->GetAs<InitDeclaratorListNode>(), pChild));
                }
                else
                {
                    // Non-declarations requiring ordered execution are added to our list of expressions to move
                    CHK(ParseTreeNode::GatherPreviousSiblingExpressions(pParent, pChild, aryOrderedExpressionsToMove));
                }
            }

            pChild = pParent;
            pParent = pParent->GetParent();
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsValidExpressionInsertionPoint
//
//  Synopsis:   Returns true when we've encountered a parent node that we
//              can determine an insertion point from. Because we move all
//              non-const global initializers into a statement list inside
//              a function, only statementLists are valid insertion points.
//
//+----------------------------------------------------------------------------
bool CGLSLParser::IsValidExpressionInsertionPoint(__in CollectionNode* pParent)
{
    return pParent->GetParseNodeType() == ParseNodeType::statementList;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateFunctionDeclarationAndDefinition
//
//  Synopsis:   To ensure correctness, we must execute global initializer
//              expressions after the entry point and shader input variables
//              have been properly initialized.
//              Creates a function with a void return type and no parameters,
//              and an empty function body. This function can be called via
//              the returned symbol index as appropriate.
//              This is the mechanism we use to initialize globals that have
//              initialization expressions that must execute after the entry
//              point has initialized shader input variables (that initialization
//              happens via boilerplate code - see WriteEntryPointBegin)
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::GenerateFunctionDeclarationAndDefinition(
    __deref_out FunctionPrototypeDeclarationNode** ppFunctionDeclaration,   // Generated function declaration node
    __deref_out FunctionDefinitionNode** ppFunctionDefinition,              // Generated function definition node
    __out int *piFunctionIdent,                                             // Generated function identifier symbol that defines the generated function
    __deref_out CollectionNode** ppStatementList                            // After this function is generated, the parent where to insert statements under
    )
{
    CHK_START;

    // Generate a unique function identifier that we'll used for both the declaration, definition, and function call.
    int iFunctionIdent;
    CHK(GenerateIdentifierSymbol(&iFunctionIdent));

    // Generate the function declaration. A function declaration only needs a function prototype.
    TSmartPointer<FunctionPrototypeNode> spFunctionPrototype;
    CHK(GenerateNoArgsFunctionPrototype(iFunctionIdent, &spFunctionPrototype));

    TSmartPointer<FunctionPrototypeDeclarationNode> spFunctionPrototypeDecl;
    CHK(RefCounted<FunctionPrototypeDeclarationNode>::Create(this, spFunctionPrototype, s_nullLocation, /*out*/spFunctionPrototypeDecl));

    // Also generate a function definition. A function definition contains a function prototype (same as function
    // declaration), but also has a compound statement as a child that represents the function body.
    // This function body is simply an empty statement list ('{ }') that we will later add to.
    TSmartPointer<FunctionPrototypeNode> spFunctionPrototypeForDefinition;
    CHK(GenerateNoArgsFunctionPrototype(iFunctionIdent, &spFunctionPrototypeForDefinition));

    // Generate an empty function body
    TSmartPointer<StatementListNode> spStatementList;
    CHK(RefCounted<StatementListNode>::Create(this, /*out*/spStatementList));

    TSmartPointer<CompoundStatementNode> spCompoundStatementNoNewScope;
    CHK(RefCounted<CompoundStatementNode>::Create(this, spStatementList, /*fDefinesScope*/false, /*out*/spCompoundStatementNoNewScope));

    // Now that we have the prototype and body, we can create the function definition. When verified, it will add
    // the identifier to the global scope so it can be called.
    TSmartPointer<FunctionDefinitionNode> spFunctionDefinition;
    CHK(RefCounted<FunctionDefinitionNode>::Create(this, spFunctionPrototypeForDefinition, spCompoundStatementNoNewScope, s_nullLocation, /*out*/spFunctionDefinition));

    *ppFunctionDeclaration = spFunctionPrototypeDecl.Extract();
    *ppFunctionDefinition = spFunctionDefinition.Extract();
    *piFunctionIdent = iFunctionIdent;
    *ppStatementList = spStatementList.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GenerateNoArgsFunctionPrototype
//
//  Synopsis:   Creates the boilerplate to tie together a return type,
//              identifier, and no parameters into a function prototype
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::GenerateNoArgsFunctionPrototype(
    int iSymbolIndex,                                                   // Symbol to use to generate a function identifier for the prototype
    __deref_out FunctionPrototypeNode** ppFunctionPrototype             // Created prototype
    )
{
    CHK_START;

    // Function prototype requires the following:
    //      Return type - FullySpecifiedTypeNode, we can just use type void as no one needs a result from the function
    //      Name - A generated FunctionIdentifierNode based on the passed in symbol index.
    //      Parameters - No parameters necessary; the function will be operating on variables in the global scope.
    //      Other declaration glue - In order for consistency with parser generated function definitions, we have
    //              intermediate nodes such as FunctionHeaderNode and FunctionPrototypeNode

    // Generate return type as a FullySpecifiedType node of basic type 'void' with no qualifier
    TSmartPointer<GLSLType> spVoidType;
    CHK(GLSLType::CreateFromBasicTypeToken(VOID_TOK, &spVoidType));

    TSmartPointer<TypeSpecifierNode> spTypeSpecifierNode;
    CHK(TypeSpecifierNode::CreateNodeFromType(this, spVoidType, &spTypeSpecifierNode));

    TSmartPointer<FullySpecifiedTypeNode> spFullySpecifiedType;
    CHK(RefCounted<FullySpecifiedTypeNode>::Create(this, NO_QUALIFIER, spTypeSpecifierNode, /*out*/spFullySpecifiedType));

    // Generate the identifier node from the passed in symbol
    TSmartPointer<FunctionIdentifierNode> spFunctionIdentifier;
    CHK(GenerateIdentifierNodeFromSymbol(iSymbolIndex, &spFunctionIdentifier));

    // Create the rest of the glue up to the prototype
    TSmartPointer<FunctionHeaderNode> spFunctionHeader;
    CHK(RefCounted<FunctionHeaderNode>::Create(this, spFullySpecifiedType, spFunctionIdentifier, /*out*/spFunctionHeader));

    TSmartPointer<FunctionHeaderWithParametersNode> spFunctionHeaderWithParams;
    CHK(RefCounted<FunctionHeaderWithParametersNode>::Create(this, spFunctionHeader, /*parameter_declaration*/nullptr, /*out*/spFunctionHeaderWithParams));

    TSmartPointer<FunctionPrototypeNode> spFunctionPrototype;
    CHK(RefCounted<FunctionPrototypeNode>::Create(this, spFunctionHeaderWithParams, /*out*/spFunctionPrototype));

    *ppFunctionPrototype = spFunctionPrototype.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   FixupDeclarationForShortCircuiting
//
//  Synopsis:   Declarations that have short-circuit expressions in one (or
//              more) initialization expressions must be handled specially.
//              The short-circuit expression needs to inject if/else code into a
//              statement list, but just putting the if/else before the entire
//              declaration may not be a valid transform since the expression
//              should be allowed to use a variable declared by an 
//              InitDeclaratorListEntryNode that precedes it. To work around
//              this, if we detect a short-circuit expression in the non-0th
//              InitDeclListEntry, we'll break up the declaration into multiple
//              declarations so that we can inject the code at just the right
//              spot (just before the declaration that contains the short-circuit
//              expression). As an example, this method will turn something like this:
//
//                  int a = 0, b = 1, c = (b = 2, (b == 0) ? a = 1 : a = 2), d[2];
//
//              into:
//
//                  int a = 0, b = 1;
//                  int c = (b = 2, (b == 0) ? a = 1 : a = 2), d[2];
//
//              The ternary and the ordered expression before it (b = 2) can then
//              be moved to just befor the 'int c' declaration.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::FixupDeclarationForShortCircuiting(__in InitDeclaratorListNode* pInitDeclList, __in ParseTreeNode* pChild)
{
    CHK_START;

    Assert(pChild->GetParseNodeType() == ParseNodeType::initDeclaratorListEntry);

    UINT uChildEntryOriginalIndex;
    CHK_VERIFY(SUCCEEDED(pInitDeclList->GetChildIndex(pChild, &uChildEntryOriginalIndex)));

    AssertSz(uChildEntryOriginalIndex != 0, "First child of initdecllist should always be a fully specified type, which should not have any expression children (much less short-circuit ones)");

    // See if the expression is coming up through a non-0th entry child. If it is the first one, we already
    // are guaranteed a valid insertion point to inject code.
    if (uChildEntryOriginalIndex > 1)
    {
        // We're going to split these entries into 2 separate declarations,
        // one for each of the identifiers declared before the shortcircuit expression,
        // and one with the remaining, beginning with the entry containing the short ciruit expression.
        // This will allow us to inject an if/else statement between the two declarations to ensure
        // we don't re-order execution of the code.

        // Both declarations must have the same type - clone it from the original
        FullySpecifiedTypeNode* pOriginalFullType = pInitDeclList->GetFullySpecifiedTypeNode();
        TSmartPointer<ParseTreeNode> spClonedFullType;
        CHK(pOriginalFullType->Clone(&spClonedFullType));

        TSmartPointer<InitDeclaratorListNode> spNewInitDeclList;
        CHK(RefCounted<InitDeclaratorListNode>::Create(this, /*out*/spNewInitDeclList));
        CHK(spNewInitDeclList->AppendChild(spClonedFullType));

        // The new InitDeclaratorListNode must be attached to the tree before verification (type specifier will attempt to
        // compute precision which assumes it occurs in a scope which is true if it's in the tree).
        // We want to insert the new list, which will eventually contain the declarations that occur before the
        // ternary, just before the InitDeclaratorListNode we're modifying.
        UINT uInitDeclListIndex;
        CHK_VERIFY(SUCCEEDED(pInitDeclList->GetParent()->GetChildIndex(pInitDeclList, &uInitDeclListIndex)));
        CHK(pInitDeclList->GetParent()->InsertChild(spNewInitDeclList, uInitDeclListIndex));

        CHK_VERIFY(SUCCEEDED(spNewInitDeclList->VerifyNode()));

        // The entries are already verified, so we just append them to the list node.
        // Grab all the entries that occur before the one that contains a short-circuit expression
        for (UINT i = 0; i < uChildEntryOriginalIndex - 1; i++)
        {
            // We must append them in order to keep variable scoping correct. This means
            // we'll extract the first entry (child1 - remember child0 is the fully specified type)
            // and append it to the new list.
            TSmartPointer<ParseTreeNode> spEntryChild;
            CHK(pInitDeclList->ExtractChild(1, &spEntryChild));
            CHK(spNewInitDeclList->AppendChild(spEntryChild));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   MoveExpressionsToInsertionPoint
//
//  Synopsis:   Move and translate all nodes in aryOrderedExpressionsToMove to
//              the specified insertion point (a child of pStatementInsertionParent, 
//              inserted before pStatementInsertionChild). This is used when translating
//              short cirtuit expressions so that the expressions are executed
//              in order and just before the child branch executes.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::MoveExpressionsToInsertionPoint(
    __in CollectionNode* pStatementInsertionParent,                     // Found parent to insert to  
    __in ParseTreeNode* pStatementInsertionChild,                       // Child branch walked up to pParent
    const CModernArray<TSmartPointer<ParseTreeNode>>& aryOrderedExpressionsToMove   // List of nodes to move
    )
{
    CHK_START;

    Assert(IsValidExpressionInsertionPoint(pStatementInsertionParent));

    for (UINT i = aryOrderedExpressionsToMove.GetCount(); i > 0; i--)
    {
        const TSmartPointer<ParseTreeNode>& spExpr = aryOrderedExpressionsToMove[i - 1];

        TSmartPointer<GLSLType> spExprType;
        CHK_VERIFY(SUCCEEDED(spExpr->GetExpressionType(&spExprType)));

        // Void function expressions must be handled separately, since we cannot
        // generate declarations to assign their results to - we just need to move
        // where they are called.
        if (!spExprType->IsTypeOrArrayOfType(VOID_TOK))
        {
            // First step is to generate a placeholder declaration with variable identifier 
            // to hold the result of the expression that is going to be moved.
            TSmartPointer<InitDeclaratorListNode> spInitDeclaratorListNode;
            CHK(GeneratePlaceholderDeclaration(spExprType, /*pInitializer*/nullptr, &spInitDeclaratorListNode));
            AssertSz(spInitDeclaratorListNode->GetIdentifierCount() == 1, "We should not generate multiple placeholder variables...");

            // Insert it first into the proper insertion location as described by the declaration parent/child pair.
            CHK(pStatementInsertionParent->InsertBefore(spInitDeclaratorListNode, pStatementInsertionChild));

            // Now that it is attached, it can be verified which will allow subsequent code to see the declared variable.
            CHK_VERIFY(SUCCEEDED(spInitDeclaratorListNode->VerifyNode()));

            VariableIdentifierNode* pPlaceholderVariable = spInitDeclaratorListNode->GetIdentifier(0);

            // Extract the expression from it's original location, remembering where it belongs (uOriginalExpressionIndex)
            UINT uOriginalExpressionIndex;
            CollectionNode* pExprParentOriginal = spExpr->GetParent();
            CHK_VERIFY(SUCCEEDED(pExprParentOriginal->GetChildIndex(spExpr, &uOriginalExpressionIndex)));
            CHK_VERIFY(SUCCEEDED(pExprParentOriginal->ExtractChild(spExpr)));

            // Now that it has been extracted, we can convert it into something that can 
            // be moved to the insertion point
            TSmartPointer<ParseTreeNode> spStatementToInsert;
            CHK(ConvertExpressionToStatement(spExpr, pPlaceholderVariable, &spStatementToInsert));

            // Insert the converted statement in the location specified by pStatementInsertionParent/Child
            CHK(pStatementInsertionParent->InsertBefore(spStatementToInsert, pStatementInsertionChild));
            CHK_VERIFY(SUCCEEDED(spStatementToInsert->VerifyNode()));

            // Now put the identifier that holds the resultant value back into the original location
            TSmartPointer<ParseTreeNode> spVariableIdent;
            CHK(pPlaceholderVariable->Clone(&spVariableIdent));
            CHK(pExprParentOriginal->InsertChild(spVariableIdent, uOriginalExpressionIndex));
            CHK_VERIFY(SUCCEEDED(spVariableIdent->VerifyNode()));
        }
        else
        {
            // For something that is type 'void' just move it by removing from its parent,
            // then inserting it as an expression statement in the appropriate location
            // in the statement list. Since we only gather expressions from expression lists
            // and function call arguments, and we know you can't have a void typed argument,
            // the parent must be an expression list.
            CollectionNode* pExprParentOriginal = spExpr->GetParent();
            Assert(pExprParentOriginal->GetParseNodeType() == ParseNodeType::expressionList);
            CHK_VERIFY(SUCCEEDED(pExprParentOriginal->ExtractChild(spExpr)));

            TSmartPointer<ExpressionStatementNode> spExprStmt;
            CHK(RefCounted<ExpressionStatementNode>::Create(this, spExpr, /*out*/spExprStmt));

            // Insert the converted statement as a child of the parent statement list.
            CHK(pStatementInsertionParent->InsertBefore(spExprStmt, pStatementInsertionChild));
            CHK_VERIFY(SUCCEEDED(spExprStmt->VerifyNode()));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   ConvertExpressionToStatement
//
//  Synopsis:   Helper function to convert a parse tree node expression into
//              a statement, based on the type of expression that is passed.
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::ConvertExpressionToStatement(
    __in ParseTreeNode* pExpr,                                          // Expression node to convert
    __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
    __deref_out ParseTreeNode** ppStatementToInsert                     // Generated expression statement
    )
{
    CHK_START;

    if (pExpr->IsShortCircuitExpression())
    {
        CHK(ConvertShortCircuitToSelection(pExpr, pPlaceholderVariable, ppStatementToInsert));
    }
    else 
    {
        // Non short circuit expression will create an assignment statement that assigns
        // into the generated variable from the declaration above.
        CHK(GenerateAssignmentStatement(pExpr, pPlaceholderVariable, ppStatementToInsert));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateStructDeclarations
//
//  Synopsis:   Searches all declarations for ones that contained struct
//              specifiers in their type specifier. The found struct specifiers 
//              (a.k.a. struct type declarations) are lifted to the global
//              scope. This is done because typename constructors are not
//              supported in HLSL, and thus we have to generate a function
//              that acts as a constructor. But functions have to be in the
//              global scope so we must perform this transformation.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateStructDeclarations()
{
    CHK_START;

    StructSpecifierCollectionNode* pStructSpecifierCollection = _spRootNode->GetStructSpecifierCollection();
    // Loop over the all the declarations that were verified.
    for (UINT i = 0; i < _aryDeclarations.GetCount(); i++)
    {
        InitDeclaratorListNode* pInitDeclList = _aryDeclarations[i];

        // If the declaration's TypeSpecifierNode descendant defined a struct type
        // we'll lift this to the global scope.
        ParseTreeNode* pTypeChild = pInitDeclList->GetTypeSpecifierDescendant()->GetChild(0);
        if (pTypeChild->GetParseNodeType() == ParseNodeType::structSpecifier)
        {
            TSmartPointer<StructSpecifierNode> spStructSpecifier(pTypeChild->GetAs<StructSpecifierNode>());
            
            // Remove the struct specifier from its original location and move it to the global collection.
            CollectionNode* pOriginalParent = spStructSpecifier->GetParent();
            CHK(pOriginalParent->ExtractChild(spStructSpecifier));

            // Append to the end to keep the ordering in the global collection the same in which the types are declared.
            CHK(pStructSpecifierCollection->AppendChild(spStructSpecifier));

            // We removed the struct specifier from its place in the tree. In order to
            // continue to output the typename for the declaration, we'll move the 
            // typename identifier from the struct specifier to be a child of 
            // the TypeSpecifierNode (original parent).
            TSmartPointer<TypeNameIdentifierNode> spTypeNameIdentifierNode(spStructSpecifier->GetTypeNameIdentifierChild());
            if (spTypeNameIdentifierNode != nullptr)
            {
                // Remove the typename identifier from the struct specifier. This should never fail
                // since we just got the node from the child array.
                CHK_VERIFY(SUCCEEDED(spStructSpecifier->ExtractChild(spTypeNameIdentifierNode)));

                // Restore the child structure of the StructSpecifierNode
                // by putting a nullptr back as its first child. Since it is verified,
                // it has everything it needs to output the typename; this nullptr
                // child ensures child ordering invariants hold true. This should never 
                // fail as the underlying storage hasn't shrunk and will have enough space for this pointer.
                CHK_VERIFY(SUCCEEDED(spStructSpecifier->InsertChild(nullptr, 0)));
            }
            else
            {
                // No TypeNameIdentifierNode child means this is an anonymous struct. We'll need to whip up
                // a typename identifier to stand in place of the anonymous struct specifier in order to
                // output the lifted type in the declaration.
                TSmartPointer<CTypeNameIdentifierInfo> spInfoAnonymous;
                CHK_VERIFY(SUCCEEDED(spStructSpecifier->GetTypeNameIdentifierInfo(&spInfoAnonymous)));
                CHK(RefCounted<TypeNameIdentifierNode>::Create(this, spInfoAnonymous, /*out*/spTypeNameIdentifierNode));
            }

            CHK(pOriginalParent->AppendChild(spTypeNameIdentifierNode));
            CHK_VERIFY(SUCCEEDED(spTypeNameIdentifierNode->VerifyNode()));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateSamplers
//
//  Synopsis:   Samplers in GLSL need to be converted to a sampler + texture
//              combination in HLSL. We do this by making new nodes that
//              correspond to the new things and adding them to a tree node
//              at the top.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateSamplers()
{
    CHK_START;

    // First collect the global variables that are samplers
    CModernArray<TSmartPointer<InitDeclaratorListNode>> arySamplers;

    // Gather all of the uniform samplers from the declarations
    for (UINT i = 0; i < _aryDeclarations.GetCount(); i++)
    {
        if (_aryDeclarations[i]->IsGLSLSampler() && _aryDeclarations[i]->GetTypeQualifier() == UNIFORM)
        {
            CHK(arySamplers.Add(_aryDeclarations[i]));
        }
    }

    // Move all of the samplers to the sampler collection
    SamplerCollectionNode* pSamplerCollection = _spRootNode->GetSamplerCollection();

    UINT uIndex = 0;
    for (UINT i = 0; i < arySamplers.GetCount(); i++)
    {
        // Move the original sampler to the sampler collection
        CollectionNode* pSamplerParent = arySamplers[i]->GetParent();
        CHK(pSamplerParent->ExtractChild(arySamplers[i]));

        CHK(pSamplerCollection->AppendChild(arySamplers[i]));

        // Make the texture clone
        CHK(CloneSamplerAsTexture(arySamplers[i], &uIndex));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CloneSamplerAsTexture
//
//  Synopsis:   Take a tree node that represents a sampler declaration, and
//              add a clone of it that represents a texture declaration after
//              it.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::CloneSamplerAsTexture(
    __in ParseTreeNode* pSamplerNode,                           // Node that represents a sampler to make a texture clone for
    __inout UINT* puSamplerCounter                              // Counter to pass down to cloning wrapper
    )
{
    CHK_START;

    // Make copies of the sampler declarations for the texture declaration
    TSmartPointer<ParseTreeNode> spTextureNode;
    CHK(pSamplerNode->Clone(&spTextureNode));

    // Figure out what the original node is - we have to specialize a bit depending
    // on the type of the original node. We specialize with a wrapper class to avoid
    // if statements everywhere.
    TSmartMemory<CSamplerNodeWrapper> spWrapper;
    if (pSamplerNode->GetParseNodeType() == ParseNodeType::initDeclaratorList)
    {
        spWrapper = new CDeclarationSamplerNodeWrapper(pSamplerNode, spTextureNode);
    }
    else
    {
        spWrapper = new CParameterSamplerNodeWrapper(pSamplerNode, spTextureNode);
    }

    // Rename the cloned nodes to the new names
    for (UINT id = 0; id < spWrapper->GetIdentifierCount(); id++)
    {
        // Grab the GLSL identifier
        VariableIdentifierNode* pGLSLIdentifier = spWrapper->GetIdentifierNode(id);

        // We need the type of the sampler, which will have both the dimension for
        // and array and the actual type. We get it from the variable if we can,
        // or from the declaration if not. An anonymous declaration cannot be
        // an array, so getting it from the declaration is OK for anon things.
        TSmartPointer<GLSLType> spType;

        // An anonymous variable won't have an identifier node
        if (pGLSLIdentifier != nullptr)
        {
            TSmartPointer<CVariableIdentifierInfo> spGLSLSamplerInfo;
            CHK(pGLSLIdentifier->GetVariableIdentifierInfo(&spGLSLSamplerInfo));
            
            // Make the generic names and semantics for the samplers and textures
            CMutableString<char> samplerName, textureName;
            CMutableString<char> samplerSem, textureSem;
            CHK(spWrapper->CreateNames(
                spGLSLSamplerInfo->GetHLSLName(0),
                samplerName,
                textureName,
                samplerSem,
                textureSem,
                *puSamplerCounter
                ));

            // Set the HLSL names for the identifier to be the sampler and texture - when outputting
            // references to the GLSL sampler, one of these will be used instead.
            CHK(spGLSLSamplerInfo->SetHLSLName(0, samplerName, samplerSem));
            CHK(spGLSLSamplerInfo->SetHLSLName(1, textureName, textureSem));

            // Get the type from the variable info
            spType = spGLSLSamplerInfo->UseType();
        }
        else
        {
            // Grab the type of the variable from the wrapper if we don't have an identifier
            CHK(spWrapper->GetType(&spType));
        }

        // Move index along by number of slots used
        (*puSamplerCounter) += static_cast<UINT>(spType->GetElementCount());
    }

    // Get the index of the sampler
    UINT uIndex;
    CollectionNode* pSamplerParent = pSamplerNode->GetParent();
    CHK(pSamplerParent->GetChildIndex(pSamplerNode, &uIndex));

    // Add the clone to this collection after the sampler
    CHK(pSamplerParent->InsertChild(spTextureNode, uIndex + 1));

    // Make sure that the texture declaration uses the texture name, semantic, etc
    CHK(spTextureNode->SetHLSLNameIndex(1));

    // This node was added out of band from the normal type verification process, so do that manually
    CHK(spTextureNode->VerifyNode());

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   TranslateInputs
//
//  Synopsis:   Variable declarators that represent shader inputs and outputs
//              need to be identified and translated separately in this step.
//
//              All declarators register during the parsing stage, so this is
//              a simple matter of iterating over them and identifying those
//              which need this special treatment.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::TranslateInputs(__in CMemoryStream* pOutput)
{
    CHK_START;

    for (UINT i = 0; i < _aryDeclarations.GetCount(); i++)
    {
        int qualifier = _aryDeclarations[i]->GetTypeQualifier();

        if (qualifier == ATTRIBUTE || qualifier == VARYING)
        {
            // Find the new parent of the declaration
            IOStructType::Enum structType = qualifier == ATTRIBUTE ? IOStructType::Attribute : IOStructType::Varying;
            IOStructNode* pStructNode = _spRootNode->GetIOStruct(structType);
            CHKB(pStructNode != nullptr);

            // Find the old parent
            ParseTreeNode* pOldParent = _aryDeclarations[i]->GetParent();
            CHKB(pOldParent->IsCollectionNode());
            CollectionNode* pOldParentCN = static_cast<CollectionNode*>(pOldParent);

            // Extract and reparent
            CHK(pOldParentCN->ExtractChild(_aryDeclarations[i]));
            CHK(pStructNode->AppendChild(_aryDeclarations[i]));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyInputs
//
//  Synopsis:   Verify that none of the input variable counts are exceeded.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::VerifyInputs()
{
    CHK_START;

    UINT uAttribCount = 0;
    UINT uVaryingCount = 0;
    UINT uUniformCount = 0;

    for (UINT i = 0; i < _aryDeclarations.GetCount(); i++)
    {
        UINT uVectorCount = _aryDeclarations[i]->GetVectorCount();

        switch(_aryDeclarations[i]->GetTypeQualifier())
        {
        case UNIFORM:
            if (FAILED(UIntAdd(uUniformCount, uVectorCount, &uUniformCount)))
            {
                // Continue and throw appropriate errors based on shader type.
                uUniformCount = UINT_MAX;
            }
            break;

        case ATTRIBUTE:
            if (FAILED(UIntAdd(uAttribCount, uVectorCount, &uAttribCount)))
            {
                // Continue and throw appropriate errors based on shader type.
                uAttribCount = UINT_MAX;
            }
            break;

        case VARYING:
            if (FAILED(UIntAdd(uVaryingCount, uVectorCount, &uVaryingCount)))
            {
                // Continue and throw appropriate errors based on shader type.
                uVaryingCount = UINT_MAX;
            }
            break;
        }
    }

    const WebGLFeatureLevelData& data = WebGLFeatureLevelData::GetData(_glFeatureLevel);
    // Make sure that the number of these variables is not in excess
    if (_shaderType == GLSLShaderType::Vertex)
    {
        if (uUniformCount > data._uVertexUniformVectors)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXUNIFORMEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (uVaryingCount > data._uVaryingVectors)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXVARYINGEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (uAttribCount > WEBGL_MAX_VERTEX_ATTRIBUTES)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXATTRIBUTEEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    if (_shaderType == GLSLShaderType::Fragment)
    {
        if (uUniformCount > data._uFragmentUniformVectors)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXUNIFORMEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (uVaryingCount > data._uVaryingVectors)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXVARYINGEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }

        if (uAttribCount > 0)
        {
            CHK(LogError(nullptr, E_GLSLERROR_MAXATTRIBUTEEXCEEDED, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetRootNode
//
//  Synopsis:   Called from the parser when the tree is completed.
//
//-----------------------------------------------------------------------------
void CGLSLParser::SetRootNode(__in ParseTreeNode* pRoot)
{
    _spRootNode = pRoot->GetAs<TranslationUnitCollectionNode>();
}

//+----------------------------------------------------------------------------
//
//  Function:   EnsureSymbolIndex
//
//  Synopsis:   Called to get the index of the symbol - it is added if need be.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::EnsureSymbolIndex(__in_z char* pszSymbol, __out int* pIndex)
{
    return _spSymbolTable->EnsureSymbolIndex(pszSymbol, pIndex);
}

//+----------------------------------------------------------------------------
//
//  Function:   DumpTree
//
//  Synopsis:   Called to dump the tree for debugging or testing.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::DumpTree()
{
    CHK_START;

    CHKB(_spRootNode != nullptr);

    TSmartPointer<CMemoryStream> spStream;
    CHK(RefCounted<CMemoryStream>::Create(/*out*/spStream));
    CHK(_spRootNode->DumpTree(spStream));

    CMutableString<char> spDump;

    CHK(spStream->ExtractString(spDump));

#ifdef DBG
    ::OutputDebugStringA(spDump);
#endif

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     LogError
//
//  Synopsis:   Called to log an error (usually through verification).
//
//              This function may fail, but only because of memory allocation
//              failures. In this case, the rest of the code needs to still
//              at least follow a path of knowing that errors occured.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::LogError(
    __in_opt const YYLTYPE *pLocation,                          // Where the error occured
    HRESULT hrCode,                                             // The code for the error
    __in_z_opt const char* pszErrorData                         // Optional string data
    )
{
    CHK_START;

    // Always record that we have an error, even if the rest fails this is important
    // for the rest of the error handling and logging code.
    _fErrors = true;

    if (pLocation == nullptr)
    {
        pLocation = &s_nullLocation;
    }

    CHK(_spConverted->AddError(pLocation->first_line, pLocation->first_column, hrCode, pszErrorData));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     OnOutputHLSL
//
//  Synopsis:   We have a text node that is inserted right after the input
//              and output structure definitions. This is called back to
//              insert the text.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::OnOutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    if (GetWriteBoilerPlate())
    {
        const WebGLFeatureLevelData& data = WebGLFeatureLevelData::GetData(_glFeatureLevel);

        // Constants
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxVertexAttribs = %d;\n", WEBGL_MAX_VERTEX_ATTRIBUTES));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxVertexUniformVectors = %d;\n", data._uVertexUniformVectors));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxVaryingVectors = %d;\n", data._uVaryingVectors));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxVertexTextureImageUnits = %d;\n", data._uVertexSamplerCount));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxCombinedTextureImageUnits = %d;\n", data._uTotalSamplerCount));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxTextureImageUnits = %d;\n", data._uFragmentSamplerCount));
        CHK(pOutput->WriteFormat(128, "static const int gl_MaxFragmentUniformVectors = %d;\n", data._uFragmentUniformVectors));
        CHK(pOutput->WriteString("static const int gl_MaxDrawBuffers = 1;\n"));

        // Both shaders have the PS input - it is the output of the vertex shader
        CHK(pOutput->WriteString("static PSInput psInput;\n"));

        if (GetShaderType() == GLSLShaderType::Vertex)
        {
            // Vertex shader has VS input structure
            CHK(pOutput->WriteString("static VSInput vsInput;\n"));

            // We need to declare a variable that represents the flipped position
            CHK(pOutput->WriteString("static float4 flippedPosition = float4(0.0, 0.0, 0.0, 1.0);\n"));

            // We need to declare a global that is used by both the point size and frag coord code
            // It contains the viewport width/2 and height/2 in x and y, respectively,
            // and viewportX + width/2 and viewportY + height/2 in z and w, respectively
            // (see CDXRenderTarget3D::PrepareConstantBufferData of emulation variables).
            CHK(pOutput->WriteString("float4 viewportHalfDim;\n"));

            if (IsFeatureUsed(FeatureUsedFlags::glPointSize))
            {                    
                CHK(pOutput->WriteString("static float gl_PointSize = 0.0;\n"));
                CHK(pOutput->WriteString(
                    "float2 AdjustPositionByPointSize(in float4 pos, in VSInput vsInput)\n"
                    "{\n"
                    // According to the spec, negative values for point size are undefined.  We'll
                    // play it safe and turn negative values to 1.0
                    "float pointSize = max(gl_PointSize, 1.0);\n"

                    // According to the spec, point rasterization needs to produce a fragment
                    // for each framebuffer pixels whose center lies inside the square who's side
                    // is equal to gl_PointSize.
                    // The units of point size are in drawingBuffer pixels: [0, viewportSize]. The position 
                    // is in homogeneous coordinates [-w, w]. In distance terms, that gives [0,2*w].
                    // To linearly interpolate between the two, we solve for the equation of a line
                    // y - y1 = ((y2-y1)/(x2-x1))*(x - x1)
                    // y - 0 = (2*w/viewportSize)*(x-0)
                    // y = (2*w/viewportSize)*(x)
                    // Here, y is the length of the square we need to render.  Since the passed in point (pos)
                    // lies in the center of the square, we need to offset the point by gl_PointSize / 2.  
                    // Plugging in gl_PointSize/2 as x (above), we get y = gl_PointSize*w/viewportSize.  Y is now 
                    // the distance we need to move the point left or right of the center. We assign this value to
                    // distanceFactor in the code below.
                    // vsInput.instanceInput has values of -1 or 1, depending on which corner of the square we're
                    // on.  If we multiply the it by the distanceFactor*pointSize, this will move the vertex in the 
                    // correct direction.  If the web developer elects to draw with triangles instead of points, 
                    // the render target will ensure that instanceInfo is filled with zeros.  Hence, the vertex
                    // will not move at all since (pos + (0 * ABunchOfStuff)) equals pos.
                    // Also, remember that viewportHalfDim.xy have half the size values, so multiply those by
                    // two in the calculation below.
                    "float2 distanceFactor = float2(pos.w / (viewportHalfDim.x *2), pos.w / (viewportHalfDim.y *2));\n"
                    "return float2(pos.xy + (vsInput.instanceInfo.xy * distanceFactor * pointSize));\n"
                    "}\n"
                    ));
            }
        }

        if (IsFeatureUsed(FeatureUsedFlags::glFragCoord) || IsFeatureUsed(FeatureUsedFlags::glDepthRange))
        {
            // Declare the depth range variable that is set by the constant buffer setup
            CHK(pOutput->WriteString("float4 depthRange;\n"));
        }

        if (GetShaderType() == GLSLShaderType::Fragment)
        {
            // Fragment shader has PS output structure
            CHK(pOutput->WriteString("static PSOutput psOutput;\n"));

            if (IsFeatureUsed(FeatureUsedFlags::glFragCoord))
            {                    
                // Declare the fragment coordinate variable - this needs to be a global
                CHK(pOutput->WriteString("static float4 gl_FragCoord;\n"));
            }

            if (IsFeatureUsed(FeatureUsedFlags::glFrontFacing))
            {
                // Declare the front facing variable - this needs to be a global
                CHK(pOutput->WriteString("static bool gl_FrontFacing;\n"));
            }

            // The gl_PointCoord variable is written to at the beginning of the pixel shader. 
            if (IsFeatureUsed(FeatureUsedFlags::glPointCoord))
            {
                CHK(pOutput->WriteString("static float2 gl_PointCoord = float2(0.0, 1.0);\n"));
            }

            if (IsFeatureUsed(FeatureUsedFlags::glFragCoord))
            {
                CHK(pOutput->WriteString("float4 viewportHalfDim;\n"));
            }
        }

        // Other things (like intrinsic function simulators) go here
        if (IsFeatureUsed(FeatureUsedFlags::GLSLsign))
        {
            CHK(pOutput->WriteString("float GLSLsign(float x) { return float(sign(x)); }\n"));
            CHK(pOutput->WriteString("float2 GLSLsign(float2 x) { return float2(sign(x)); }\n"));
            CHK(pOutput->WriteString("float3 GLSLsign(float3 x) { return float3(sign(x)); }\n"));
            CHK(pOutput->WriteString("float4 GLSLsign(float4 x) { return float4(sign(x)); }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLmod))
        {
            CHK(pOutput->WriteString("float GLSLmod(float x, float y) { return (x - y * floor(x / y)); }\n"));
            CHK(pOutput->WriteString("float2 GLSLmod(float2 x, float2 y) { return (x - y * floor(x / y)); }\n"));
            CHK(pOutput->WriteString("float3 GLSLmod(float3 x, float3 y) { return (x - y * floor(x / y)); }\n"));
            CHK(pOutput->WriteString("float4 GLSLmod(float4 x, float4 y) { return (x - y * floor(x / y)); }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLmatrixCompMult))
        {
            CHK(pOutput->WriteString("float2x2 GLSLmatrixCompMult(float2x2 x, float2x2 y) { return x * y; }\n"));
            CHK(pOutput->WriteString("float3x3 GLSLmatrixCompMult(float3x3 x, float3x3 y) { return x * y; }\n"));
            CHK(pOutput->WriteString("float4x4 GLSLmatrixCompMult(float4x4 x, float4x4 y) { return x * y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLlessThan))
        {
            CHK(pOutput->WriteString("bool2 GLSLlessThan(float2 x, float2 y) { return x < y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLlessThan(float3 x, float3 y) { return x < y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLlessThan(float4 x, float4 y) { return x < y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLlessThan(int2 x, int2 y) { return x < y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLlessThan(int3 x, int3 y) { return x < y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLlessThan(int4 x, int4 y) { return x < y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLlessThanEqual))
        {
            CHK(pOutput->WriteString("bool2 GLSLlessThanEqual(float2 x, float2 y) { return x <= y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLlessThanEqual(float3 x, float3 y) { return x <= y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLlessThanEqual(float4 x, float4 y) { return x <= y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLlessThanEqual(int2 x, int2 y) { return x <= y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLlessThanEqual(int3 x, int3 y) { return x <= y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLlessThanEqual(int4 x, int4 y) { return x <= y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLgreaterThan))
        {
            CHK(pOutput->WriteString("bool2 GLSLgreaterThan(float2 x, float2 y) { return x > y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLgreaterThan(float3 x, float3 y) { return x > y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLgreaterThan(float4 x, float4 y) { return x > y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLgreaterThan(int2 x, int2 y) { return x > y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLgreaterThan(int3 x, int3 y) { return x > y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLgreaterThan(int4 x, int4 y) { return x > y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLgreaterThanEqual))
        {
            CHK(pOutput->WriteString("bool2 GLSLgreaterThanEqual(float2 x, float2 y) { return x >= y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLgreaterThanEqual(float3 x, float3 y) { return x >= y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLgreaterThanEqual(float4 x, float4 y) { return x >= y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLgreaterThanEqual(int2 x, int2 y) { return x >= y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLgreaterThanEqual(int3 x, int3 y) { return x >= y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLgreaterThanEqual(int4 x, int4 y) { return x >= y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLequal))
        {
            CHK(pOutput->WriteString("bool2 GLSLequal(float2 x, float2 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLequal(float3 x, float3 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLequal(float4 x, float4 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLequal(int2 x, int2 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLequal(int3 x, int3 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLequal(int4 x, int4 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLequal(bool2 x, bool2 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLequal(bool3 x, bool3 y) { return x == y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLequal(bool4 x, bool4 y) { return x == y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLnotEqual))
        {
            CHK(pOutput->WriteString("bool2 GLSLnotEqual(float2 x, float2 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLnotEqual(float3 x, float3 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLnotEqual(float4 x, float4 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLnotEqual(int2 x, int2 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLnotEqual(int3 x, int3 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLnotEqual(int4 x, int4 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool2 GLSLnotEqual(bool2 x, bool2 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLnotEqual(bool3 x, bool3 y) { return x != y; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLnotEqual(bool4 x, bool4 y) { return x != y; }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLnot))
        {
            CHK(pOutput->WriteString("bool2 GLSLnot(bool2 x) { return !x; }\n"));
            CHK(pOutput->WriteString("bool3 GLSLnot(bool3 x) { return !x; }\n"));
            CHK(pOutput->WriteString("bool4 GLSLnot(bool4 x) { return !x; }\n"));
        }

        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2D, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DBias, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DLod, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProj, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProj_1, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProjBias, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProjBias_1, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProjLod, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtexture2DProjLod_1, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtextureCube, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtextureCubeBias, pOutput));
        CHK(WriteTextureFunction(FeatureUsedFlags::GLSLtextureCubeLod, pOutput));

        if (IsFeatureUsed(FeatureUsedFlags::GLSLmatrixFromScalar))
        {
            CHK(pOutput->WriteString("float2x2 GLSLmatrix2FromScalar(float x) { return float2x2(x, 0, 0, x); }\n"));
            CHK(pOutput->WriteString("float3x3 GLSLmatrix3FromScalar(float x) { return float3x3(x, 0, 0, 0, x, 0, 0, 0, x); }\n"));
            CHK(pOutput->WriteString("float4x4 GLSLmatrix4FromScalar(float x) { return float4x4(x, 0, 0, 0, 0, x, 0, 0, 0, 0, x, 0, 0, 0, 0, x); }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLmatrixFromMatrix))
        {
            // Create 2x2
            CHK(pOutput->WriteString("float2x2 GLSLmatrix2FromMatrix3(float3x3 x) { return float2x2(x[0].xy, x[1].xy); }\n"));
            CHK(pOutput->WriteString("float2x2 GLSLmatrix2FromMatrix4(float4x4 x) { return float2x2(x[0].xy, x[1].xy); }\n"));

            // Create 3x3
            CHK(pOutput->WriteString("float3x3 GLSLmatrix3FromMatrix2(float2x2 x) { return float3x3(x[0], 0, x[1], 0, 0, 0, 1); }\n"));
            CHK(pOutput->WriteString("float3x3 GLSLmatrix3FromMatrix4(float4x4 x) { return float3x3(x[0].xyz, x[1].xyz, x[2].xyz); }\n"));

            // Create 4x4
            CHK(pOutput->WriteString("float4x4 GLSLmatrix4FromMatrix2(float2x2 x) { return float4x4(x[0].xy, 0, 0, x[1].xy, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1); }\n"));
            CHK(pOutput->WriteString("float4x4 GLSLmatrix4FromMatrix3(float3x3 x) { return float4x4(x[0].xyz, 0, x[1].xyz, 0, x[2].xyz, 0, 0, 0, 0, 1); }\n"));
        }                                                                                             

        if (IsFeatureUsed(FeatureUsedFlags::GLSLvectorFromMatrix))
        {
            CHK(pOutput->WriteString("float4 GLSLvectorFromMatrix(float2x2 x) { return float4(x[0], x[1]); }\n"));
        }

        if (IsFeatureUsed(FeatureUsedFlags::GLSLfwidth))
        {
            CHK(pOutput->WriteString("float GLSLfwidth(float x) { return abs(ddx(x)) + abs(ddy(x)); }\n"));
            CHK(pOutput->WriteString("float2 GLSLfwidth(float2 x) { return abs(ddx(x)) + abs(ddy(x)); }\n"));
            CHK(pOutput->WriteString("float3 GLSLfwidth(float3 x) { return abs(ddx(x)) + abs(ddy(x)); }\n"));
            CHK(pOutput->WriteString("float4 GLSLfwidth(float4 x) { return abs(ddx(x)) + abs(ddy(x)); }\n"));
        }

        // The gl_DepthRangeParameters struct is available regardless of whether or not
        // the gl_DepthRange variable/glDepthRange feature is used. Always output this struct definition.
        CHK(pOutput->WriteString(
            "struct gl_DepthRangeParameters\n"
            "{\n"
            "float near;\n"
            "float far;\n"
            "float diff;\n"
            "};\n"
            ));

        if (IsFeatureUsed(FeatureUsedFlags::glDepthRange))
        {
            // Output the gl_DepthRange variable if it is used in this shader.
            CHK(pOutput->WriteString(
                "static gl_DepthRangeParameters gl_DepthRange;\n"
                ));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Struct:     TextureFunctionInfo
//
//  Synopsis:   Struct used by WriteTextureFunction to store a table of
//              functions.
//
//-----------------------------------------------------------------------------
struct TextureFunctionInfo
{
    FeatureUsedFlags::Enum _enum;
    bool _fBias;
    bool _fLod;
    const char* _pszName;
    const char* _pszTextureType;
    const char* _pszCoordType;
    const char* _pszSampleFragFunction;
    const char* _pszSampleExpression;
};

//+----------------------------------------------------------------------------
//
//  Member:     WriteTextureFunction
//
//  Synopsis:   Write out the given texture function from the feature used
//              enum code. All of the functions look very similar, they just
//              vary argument counts and types.
//
//              This function takes care of outputting the function 
//              conditionally.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::WriteTextureFunction(
    FeatureUsedFlags::Enum featureUsed,                         // Feature used code for function
    __in IStringStream* pOutput                                 // Where to write the code
    )
{
    CHK_START;

    static const TextureFunctionInfo rgFunctionInfo[] = {
            /* name                                     bias    lod     functionname                texture coord       hlsl frag       expr */
        {   FeatureUsedFlags::GLSLtexture2D,            false,  false,  "GLSLtexture2D",            "2D",   "float2",   "Sample",       "coord" },
        {   FeatureUsedFlags::GLSLtexture2DBias,        true,   false,  "GLSLtexture2DBias",        "2D",   "float2",   "SampleBias",   "coord" },
        {   FeatureUsedFlags::GLSLtexture2DLod,         false,  true,   "GLSLtexture2DLod",         "2D",   "float2",   nullptr,        "coord" },
        {   FeatureUsedFlags::GLSLtexture2DProj,        false,  false,  "GLSLtexture2DProj",        "2D",   "float3",   "Sample",       "coord.xy / coord.z" },
        {   FeatureUsedFlags::GLSLtexture2DProj_1,      false,  false,  "GLSLtexture2DProj",        "2D",   "float4",   "Sample",       "coord.xy / coord.w" },
        {   FeatureUsedFlags::GLSLtexture2DProjBias,    true,   false,  "GLSLtexture2DProjBias",    "2D",   "float3",   "SampleBias",   "coord.xy / coord.z" },
        {   FeatureUsedFlags::GLSLtexture2DProjBias_1,  true,   false,  "GLSLtexture2DProjBias",    "2D",   "float4",   "SampleBias",   "coord.xy / coord.w" },
        {   FeatureUsedFlags::GLSLtexture2DProjLod,     false,  true,   "GLSLtexture2DProjLod",     "2D",   "float3",   nullptr,        "coord.xy / coord.z" },
        {   FeatureUsedFlags::GLSLtexture2DProjLod_1,   false,  true,   "GLSLtexture2DProjLod",     "2D",   "float4",   nullptr,        "coord.xy / coord.w" },
        {   FeatureUsedFlags::GLSLtextureCube,          false,  false,  "GLSLtextureCube",          "Cube", "float3",   "Sample",       "coord" },
        {   FeatureUsedFlags::GLSLtextureCubeBias,      true,   false,  "GLSLtextureCubeBias",      "Cube", "float3",   "SampleBias",   "coord" },
        {   FeatureUsedFlags::GLSLtextureCubeLod,       false,  true,   "GLSLtextureCubeLod",       "Cube", "float3",   nullptr,        "coord" },
    };

    if (IsFeatureUsed(featureUsed))
    {
        for (UINT i = 0; i < ARRAYSIZE(rgFunctionInfo); i++)
        {
            if (rgFunctionInfo[i]._enum == featureUsed)
            {
                if (rgFunctionInfo[i]._fBias)
                {
                    AssertSz(!rgFunctionInfo[i]._fLod, "Bias and Lod cannot be combined with texture functions");

                    if (_shaderType == GLSLShaderType::Fragment)
                    {
                        CHK(pOutput->WriteFormat(
                            256,
                            "float4 %s(SamplerState s, Texture%s<float4> t, %s coord, float bias) { return t.%s(s, %s, bias); }\n",
                            rgFunctionInfo[i]._pszName,
                            rgFunctionInfo[i]._pszTextureType,
                            rgFunctionInfo[i]._pszCoordType,
                            rgFunctionInfo[i]._pszSampleFragFunction,
                            rgFunctionInfo[i]._pszSampleExpression
                            ));
                    }
                    else
                    {
                        AssertSz(false, "Vertex shaders should not request bias version of functions");
                    }
                }
                else if (rgFunctionInfo[i]._fLod)
                {
                    if (_shaderType == GLSLShaderType::Vertex)
                    {
                        CHK(pOutput->WriteFormat(
                            256,
                            "float4 %s(SamplerState s, Texture%s<float4> t, %s coord, float lod) { return t.SampleLevel(s, %s, lod); }\n",
                            rgFunctionInfo[i]._pszName,
                            rgFunctionInfo[i]._pszTextureType,
                            rgFunctionInfo[i]._pszCoordType,
                            rgFunctionInfo[i]._pszSampleExpression
                            ));
                    }
                    else
                    {
                        AssertSz(false, "Fragment shaders should not request lod version of functions");
                    }
                }
                else
                {
                    if (_shaderType == GLSLShaderType::Vertex)
                    {
                        CHK(pOutput->WriteFormat(
                            256,
                            "float4 %s(SamplerState s, Texture%s<float4> t, %s coord) { return t.SampleLevel(s, %s, 0.0); }\n",
                            rgFunctionInfo[i]._pszName,
                            rgFunctionInfo[i]._pszTextureType,
                            rgFunctionInfo[i]._pszCoordType,
                            rgFunctionInfo[i]._pszSampleExpression
                            ));
                    }
                    else
                    {
                        CHK(pOutput->WriteFormat(
                            256,
                            "float4 %s(SamplerState s, Texture%s<float4> t, %s coord) { return t.%s(s, %s); }\n",
                            rgFunctionInfo[i]._pszName,
                            rgFunctionInfo[i]._pszTextureType,
                            rgFunctionInfo[i]._pszCoordType,
                            rgFunctionInfo[i]._pszSampleFragFunction,
                            rgFunctionInfo[i]._pszSampleExpression
                            ));
                    }
                }
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     WriteEntryPointBegin
//
//  Synopsis:   Write out the code that we have at the beginning of each
//              entry point.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::WriteEntryPointBegin(
    __in IStringStream* pOutput                                 // Where to write the code
    )
{
    CHK_START;

    if (GetWriteBoilerPlate())
    {
        if (GetShaderType() == GLSLShaderType::Vertex)
        {
            CHK(pOutput->WriteString("vsInput = vsInputArg;\n"));
        }

        if (GetShaderType() == GLSLShaderType::Fragment)
        {
            CHK(pOutput->WriteString("psInput = psInputArg;\n"));

            if (IsFeatureUsed(FeatureUsedFlags::glFragCoord))
            {
                // Store the inverse w value, we'll use it a few times in calculations below.
                CHK(pOutput->WriteString("float wInverse = 1.0 / psInput.fragCoord.w;\n"));
                if (_glFeatureLevel >= WebGLFeatureLevel::Level_10)
                {
                    // In feature level 10+ (real shader model 4) we can just use SV_Position's calculation of the x and y
                    // frag coord values. Since everything in the pixel shader is 'right side up' (i.e. we've executed the
                    // y-flip upon vertex shader output, which will be undone with a transform on the canvas image),
                    // we do not have to adjust for y-flipping.
                    CHK(pOutput->WriteString("gl_FragCoord.xy = psInput.position.xy;\n"));
                }
                else
                {
                    // In feature levels < 10 we must calculate this ourselves. Window coordinates in WebGL are defined as
                    //
                    //      (xw, yx) = (px/2 * xd + ox, py/2 * yd + oy)
                    //
                    // where px is viewport width, py is viewport height, ox is viewport x origin (x offset + width/2), and 
                    // oy is viewport y origin (y offset + height/2).
                    // Also, xd and yd are normalized device coordinates which are given by
                    //
                    //      (xd, yd) = (xc / wc, yc / wc)
                    //
                    // where xc and yc are clip coordinates. Clip coordinates are the coordinates output from the vertex shader.
                    // To derive xd and yd, we must perform the perspective divide by multiplying by wInverse.
                    //
                    // The emulation variable sets viewportHalfDim x and y to px and py, respectively
                    // and z and w to ox and oy, respectively. Use these to calculate the fragcoord.
                    CHK(pOutput->WriteString("gl_FragCoord.x = viewportHalfDim.x * (psInput.fragCoord.x * wInverse) + viewportHalfDim.z;\n"));
                    CHK(pOutput->WriteString("gl_FragCoord.y = viewportHalfDim.y * (psInput.fragCoord.y * wInverse) + viewportHalfDim.w;\n"));
                }

                // gl_FragCoord corresponds to window coordinates, so we calculate the z value using
                // the equation given in the OpenGL ES spec; namely
                //
                //      zw = (((far - near) / 2) * zd) + ((near + far) / 2)
                //
                // The depthRange constants are set by the render target prior to drawing as the following:
                //      depthRange.x = (far - near) / 2
                //      depthRange.y = (near + far) / 2
                //      depthRange.z = near
                //      depthRange.w = far
                // These were chosen to minimize the number of instructions in the pixel shader and are constant
                // for a given draw pass (see DXRenderTarget3D.cxx). With the values in depthRange, the equation reduces to
                //
                //      zw = (depthRange.x * zd) + depthRange.y
                //
                // where zd is the z-value after perspective divide (i.e. input.z / input.w).
                CHK(pOutput->WriteString("gl_FragCoord.z = (depthRange.x * psInput.fragCoord.z * wInverse) + depthRange.y;\n"));

                // gl_FragCoord.w is just the inverse of the w value at the given pixel.
                CHK(pOutput->WriteString("gl_FragCoord.w = wInverse;\n"));
            }

            if (IsFeatureUsed(FeatureUsedFlags::glFrontFacing))
            {
                CHK(pOutput->WriteString("gl_FrontFacing = psInput.frontFacing;\n"));
            }
            
            // We need to make sure that the fragment color is cleared on entry, because GLSL specs that it is zeroed and
            // HLSL requires that it be touched.
            CHK(pOutput->WriteString("psOutput.fragColor[0] = float4(0.0, 0.0, 0.0, 0.0);\n"));

            // If we are using gl_PointCoord, make sure to initialize it.  CGLSLIOStructInfo::OutputHLSL is responsible for 
            // ensuring that ReadPointCoordFromPSInput is present and either returns a default value or grabs its from PSInput.
            if (IsFeatureUsed(FeatureUsedFlags::glPointCoord))
            {
                CHK(pOutput->WriteString("gl_PointCoord = ReadPointCoordFromPSInput(psInput);\n"));
            }
        }

        if (IsFeatureUsed(FeatureUsedFlags::glDepthRange))
        {
            CHK(pOutput->WriteString(
                "gl_DepthRange.near = depthRange.z;\n"
                "gl_DepthRange.far = depthRange.w;\n"
                "gl_DepthRange.diff = depthRange.x * 2.0;\n"
                ));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     WriteEntryPointEnd
//
//  Synopsis:   Write out the code we have at the end of each entry point.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::WriteEntryPointEnd(
    __in IStringStream* pOutput                                 // Where to write the code
    )
{
    CHK_START;

    if (GetWriteBoilerPlate())
    {
        if (GetShaderType() == GLSLShaderType::Vertex)
        {
            if (IsFeatureUsed(FeatureUsedFlags::glPointSize))
            {
                // If we're using gl_PointSize, we need to adjust the position by the point size.  
                // AdjustPositionByPointSize is a helper method responsible for doing that.  
                // Similarly, we need to pass down the point coord information in psInput.pointCoord.
                // The pixel shader can choose to ignore it if it chooses.  
                CHK(pOutput->WriteString(
                    "flippedPosition.xy = AdjustPositionByPointSize(flippedPosition, vsInput);\n"
                    "WritePointCoordToPSInput(psInput, vsInput.instanceInfo.zw);\n"
                    ));
            }

            // WebGL needs content rendered upside down relative to D3D because the texture
            // coordinates are upside down relative to D3D. Reads / writes to gl_Position
            // have been directed into a flipped version of the variable, so output the
            // real thing now.
            CHK(pOutput->WriteString("psInput.position.xw = flippedPosition.xw;\n"));

            CHK(pOutput->WriteString("psInput.position.y = -flippedPosition.y;\n"));

            // Scale the psInput.position.z value from [-w, w] to [0, w]. Anything outside
            // [0, w] will be clipped at the rasterizer stage.
            CHK(pOutput->WriteString("psInput.position.z = (flippedPosition.z + flippedPosition.w) / 2;\n"));

            // Before we return from a vertex shader, we have to calculate the fragment
            // coordinate for the pixel shader. Since we can have multiple return
            // points we need to put this code in all of them.
            //
            // We must calculate the frag coord from the flipped position, in order
            // for it to report self-consistent results.
            //
            // We call out to a function to calculate this, because we might decide
            // to noop this operation if the program has no gl_FragCoord usage. We
            // cannot know that yet, so we call a function here and in GLSLIOStructInfo
            // we generate the function that is actually called.
            CHK(pOutput->WriteString("WriteFragCoordToPSInput(psInput, flippedPosition);\n"));

            // Return the appropriate struct
            CHK(pOutput->WriteString("psInputOut = psInput;\n"));
        }

        if (GetShaderType() == GLSLShaderType::Fragment)
        {
            // Return the appropriate struct
            CHK(pOutput->WriteString("psOutputOut = psOutput;\n"));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     AddHLSLFunctionUsage
//
//  Synopsis:   We have seen usage of a function, and we are being notified
//              of the equivalent HLSL function to it.
//
//-----------------------------------------------------------------------------
void CGLSLParser::AddHLSLFunctionUsage(HLSLFunctions::Enum function)
{
    // Check that it has an equivalent function
    if (function != HLSLFunctions::count)
    {
        // Set flags for functions that have them
        if (function == HLSLFunctions::GLSLsign) { _uFeaturesUsed |= FeatureUsedFlags::GLSLsign; }
        if (function == HLSLFunctions::GLSLmod) { _uFeaturesUsed |= FeatureUsedFlags::GLSLmod; }
        if (function == HLSLFunctions::GLSLmatrixCompMult) { _uFeaturesUsed |= FeatureUsedFlags::GLSLmatrixCompMult; }
        if (function == HLSLFunctions::GLSLmatrixCompMult_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLmatrixCompMult; }
        if (function == HLSLFunctions::GLSLmatrixCompMult_2) { _uFeaturesUsed |= FeatureUsedFlags::GLSLmatrixCompMult; }
        if (function == HLSLFunctions::GLSLlessThan) { _uFeaturesUsed |= FeatureUsedFlags::GLSLlessThan; }
        if (function == HLSLFunctions::GLSLlessThan_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLlessThan; }
        if (function == HLSLFunctions::GLSLlessThanEqual) { _uFeaturesUsed |= FeatureUsedFlags::GLSLlessThanEqual; }
        if (function == HLSLFunctions::GLSLlessThanEqual_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLlessThanEqual; }
        if (function == HLSLFunctions::GLSLgreaterThan) { _uFeaturesUsed |= FeatureUsedFlags::GLSLgreaterThan; }
        if (function == HLSLFunctions::GLSLgreaterThan_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLgreaterThan; }
        if (function == HLSLFunctions::GLSLgreaterThanEqual) { _uFeaturesUsed |= FeatureUsedFlags::GLSLgreaterThanEqual; }
        if (function == HLSLFunctions::GLSLgreaterThanEqual_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLgreaterThanEqual; }
        if (function == HLSLFunctions::GLSLequal) { _uFeaturesUsed |= FeatureUsedFlags::GLSLequal; }
        if (function == HLSLFunctions::GLSLequal_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLequal; }
        if (function == HLSLFunctions::GLSLequal_2) { _uFeaturesUsed |= FeatureUsedFlags::GLSLequal; }
        if (function == HLSLFunctions::GLSLnotEqual) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnotEqual; }
        if (function == HLSLFunctions::GLSLnotEqual_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnotEqual; }
        if (function == HLSLFunctions::GLSLnotEqual_2) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnotEqual; }
        if (function == HLSLFunctions::GLSLnot) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnot; }
        if (function == HLSLFunctions::GLSLnot_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnot; }
        if (function == HLSLFunctions::GLSLnot_2) { _uFeaturesUsed |= FeatureUsedFlags::GLSLnot; }
        if (function == HLSLFunctions::GLSLtexture2D) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2D; }
        if (function == HLSLFunctions::GLSLtexture2DBias) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DBias; }
        if (function == HLSLFunctions::GLSLtexture2DLod) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DLod; }
        if (function == HLSLFunctions::GLSLtexture2DProj) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProj; }
        if (function == HLSLFunctions::GLSLtexture2DProj_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProj_1; }
        if (function == HLSLFunctions::GLSLtexture2DProjBias) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProjBias; }
        if (function == HLSLFunctions::GLSLtexture2DProjBias_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProjBias_1; }
        if (function == HLSLFunctions::GLSLtexture2DProjLod) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProjLod; }
        if (function == HLSLFunctions::GLSLtexture2DProjLod_1) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtexture2DProjLod_1; }
        if (function == HLSLFunctions::GLSLtextureCube) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtextureCube; }
        if (function == HLSLFunctions::GLSLtextureCubeBias) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtextureCubeBias; }
        if (function == HLSLFunctions::GLSLtextureCubeLod) { _uFeaturesUsed |= FeatureUsedFlags::GLSLtextureCubeLod; }
        if (function == HLSLFunctions::GLSLfwidth) { _uFeaturesUsed |= FeatureUsedFlags::GLSLfwidth; }
    }
}

//+----------------------------------------------------------------------------
//
//  Member:     AddSpecialVariableUsage
//
//  Synopsis:   We have seen usage of a special variable, and we are being 
//              notified of this.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLParser::AddSpecialVariableUsage(
    __in const YYLTYPE* pLocation,                              // Location of usage
    GLSLSpecialVariables::Enum variable                         // Special variable being used
    )
{
    CHK_START;

    // Check that it has an equivalent function
    if (variable != GLSLSpecialVariables::count)
    {
        // Set flags for variables that have them
        if (variable == GLSLSpecialVariables::glFrontFacing) 
        {
            // Somebody trying to use this feature on a 9 feature level has to be denied
            // because we have not implemented emulation for this yet.
            if (_glFeatureLevel < WebGLFeatureLevel::Level_10)
            {
                CHK(LogError(pLocation, E_GLSLERROR_INVALIDFRONTFACINGUSAGE, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }

            _uFeaturesUsed |= FeatureUsedFlags::glFrontFacing; 
        }
        else if (variable == GLSLSpecialVariables::glPointSize)
        {
            AssertSz(_shaderType == GLSLShaderType::Vertex, "glPointSize should not be able to be used in Fragment shader");

            _uFeaturesUsed |= FeatureUsedFlags::glPointSize;
        }            
        else if (variable == GLSLSpecialVariables::glPointCoord)
        {
            AssertSz(_shaderType == GLSLShaderType::Fragment, "glPointCoord should not be able to be used in Vertex shader");

            _uFeaturesUsed |= FeatureUsedFlags::glPointCoord;
        }
        else if (variable == GLSLSpecialVariables::glFragCoord)
        {
            AssertSz(_shaderType == GLSLShaderType::Fragment, "glFragCoord should not be able to be used in Vertex shader");

            _uFeaturesUsed |= FeatureUsedFlags::glFragCoord;
        }
        else if (variable == GLSLSpecialVariables::glDepthRange)
        {
            _uFeaturesUsed |= FeatureUsedFlags::glDepthRange;
        }
        else if (variable == GLSLSpecialVariables::glFragDepth)
        {
            _uFeaturesUsed |= FeatureUsedFlags::glFragDepth;
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Member:     AddFeatureUsage
//
//  Synopsis:   Add a used feature from the raw flag.
//
//-----------------------------------------------------------------------------
void CGLSLParser::AddFeatureUsage(FeatureUsedFlags::Enum feature)
{
    _uFeaturesUsed |= feature;
}


//+----------------------------------------------------------------------------
//
//  Function:   WrapSimpleStatementAsCompoundStatement
//
//  Synopsis:   Helper function for nodes that need to wrap a simple statement
//              in a compound statement so that we generate equivalent parse
//              trees for constructs like
//                  if (foo) x = 2;
//                  if (foo) { x = 2; }
//              We are required to output the braces when we inject code, but
//              further than that we may need to rely on the structure (namely
//              the statement list) in order to find valid points to insert
//              generated nodes for various translation tree transformation
//              operations that we perform (e.g. ternary and short-circuiting).
//
//+----------------------------------------------------------------------------
HRESULT CGLSLParser::WrapSimpleStatementAsCompoundStatement(
    __in ParseTreeNode* pSimpleStatement,                               // The simple statment to wrap
    bool fDefinesScope,                                                 // Whether the created compound statement defines a scope or not
    __deref_out CompoundStatementNode** ppCompoundStatement             // The created compound statement
    )
{
    CHK_START;

    // Wrap the simple statement in a statement list; CompoundStatementNode's child is 
    // required to be a statement list by the grammar.
    TSmartPointer<StatementListNode> spStatementList;
    CHK(RefCounted<StatementListNode>::Create(this, pSimpleStatement, /*out*/spStatementList));

    TSmartPointer<CompoundStatementNode> spCompoundStatement;
    CHK(RefCounted<CompoundStatementNode>::Create(this, spStatementList, fDefinesScope, /*out*/spCompoundStatement));

    *ppCompoundStatement = spCompoundStatement.Extract();
    
    CHK_RETURN;
}
