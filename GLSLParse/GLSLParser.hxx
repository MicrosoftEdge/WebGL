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

class CGLSLParser;

#include "ParseTreeNode.hxx"
#include "GLSLSymbolTable.hxx"
#include "GLSLIdentifierTable.hxx"
#include "GLSLShaderType.hxx"
#include "GLSLTranslateOptions.hxx"
#include "GLSLError.hxx"
#include "GLSLConvertedShader.hxx"
#include "IParserInput.hxx"
#include "TextNode.hxx"
#include "IStringStream.hxx"
#include "GLSLIOStructInfo.hxx"
#include "GLSLLineMap.hxx"
#include "GLSLExtensionState.hxx"
#include "WebGLFeatureLevel.hxx"
#include "GLSL.tab.h"
#include "InitDeclaratorListNode.hxx"
#include "TranslationUnitNode.hxx"

int GLSLparse(__in void* YYPARSE_PARAM);

class VariableIdentifierNode;
class CompoundStatementNode;
class FunctionPrototypeDeclarationNode;
class FunctionPrototypeNode;

//+-----------------------------------------------------------------------------
//
//  Enum:       FeatureUsedFlags
//
//  Synopsis:   Flags enum for features that are not always used - the parser
//              uses this to trim down the translation.
//
//------------------------------------------------------------------------------
namespace FeatureUsedFlags
{
    enum Enum : UINT
    {
        None                    = 0,
        GLSLsign                = 0x00000001,
        GLSLmod                 = 0x00000002,
        GLSLmatrixCompMult      = 0x00000004,
        GLSLlessThan            = 0x00000008,
        GLSLlessThanEqual       = 0x00000010,
        GLSLgreaterThan         = 0x00000020,
        GLSLgreaterThanEqual    = 0x00000040,
        GLSLequal               = 0x00000080,
        GLSLnotEqual            = 0x00000100,
        GLSLnot                 = 0x00000200,
        GLSLtexture2D           = 0x00000400,
        GLSLtexture2DBias       = 0x00000800,
        GLSLtexture2DLod        = 0x00001000,
        GLSLtexture2DProj       = 0x00002000,
        GLSLtexture2DProj_1     = 0x00004000,
        GLSLtexture2DProjBias   = 0x00008000,
        GLSLtexture2DProjBias_1 = 0x00010000,
        GLSLtexture2DProjLod    = 0x00020000,
        GLSLtexture2DProjLod_1  = 0x00040000,
        GLSLtextureCube         = 0x00080000,
        GLSLtextureCubeBias     = 0x00100000,
        GLSLtextureCubeLod      = 0x00200000,
        glFrontFacing           = 0x00400000,
        GLSLmatrixFromScalar    = 0x00800000,
        GLSLmatrixFromMatrix    = 0x01000000,
        GLSLvectorFromMatrix    = 0x02000000,
        glPointSize             = 0x04000000,
        glPointCoord            = 0x08000000,
        GLSLfwidth              = 0x10000000,
        glFragCoord             = 0x20000000,
        glDepthRange            = 0x40000000,
        glFragDepth             = 0x80000000,
    };
}

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLParser
//
//  Synopsis:   Class to encapsulate the generated scanner and the associated
//              input / output.
//
//              This is not meant to be consumed from outside of the lib - 
//              use the GLSLTranslate function rather than this directly.
//
//------------------------------------------------------------------------------
class CGLSLParser : public ITextNodeProvider
{
public:
    CGLSLParser();

    HRESULT Initialize(
        __in BSTR bstrInput,                                                // Input unicode text of shader
        GLSLShaderType::Enum shaderType,                                    // Type of shader to translate
        UINT uOptions,                                                      // Translation options
        WebGLFeatureLevel glFeatureLevel                                    // Feature level to translate for
        );

    HRESULT Translate(
        __deref_out CGLSLConvertedShader** ppConvertedShader                // Converted shader object
        );

    // Functions called from the generated parser stack
    HRESULT EnsureSymbolIndex(__in_z char* pszSymbol, __out int* pIndex);
    void SetRootNode(__in ParseTreeNode* pRoot);
    void NotifyError(__in YYLTYPE* pLocation, __in_z const char* error);
    void UpdateLocation(__in YYLTYPE* pLocation, int lineNo, int tokenLength);
    void RecordNewline();

    // Functions called from the parse tree
    HRESULT AddDeclaratorList(
        InitDeclaratorListNode* pNewDecl                                    // The declarator to add
        );

    HRESULT AddShortCircuitExpression(
        __in ParseTreeNode* pExpr                                           // The expression to add
        );

    void SetEntryPointNode(__in FunctionDefinitionNode* pEntryPoint);
    void SetHasNonConstGlobalInitializer() { _fHasNonConstGlobalInitializers = true; }

    HRESULT LogError(
        __in_opt const YYLTYPE *pLocation,                                  // Where the error occured
        HRESULT hrCode,                                                     // The code for the error
        __in_z_opt const char* pszErrorData                                 // Optional string data
        );

    int GenerateScopeId() { return _currentScopeId++; }
    int GenerateIdentifierId() { return _generatedIdentifierId++; }

    GLSLShaderType::Enum GetShaderType() const { return _shaderType; }
    bool GetWriteInputs() const { return _fWriteInputs; }
    bool GetWriteBoilerPlate() const { return _fWriteBoilerPlate; }
    WebGLFeatureLevel GetFeatureLevel() const { return _glFeatureLevel; }

    CGLSLSymbolTable* UseSymbolTable() { return _spSymbolTable; }
    CGLSLIdentifierTable* UseIdentifierTable() { return _spIdTable; }
    CGLSLExtensionState* UseExtensionState() { return _spExtensionState; }
    ParseTreeNode* UseRootNode() { return _spRootNode; }
    IParserInput* UseInput() { return _spInput; }

    HRESULT WriteEntryPointBegin(
        __in IStringStream* pOutput                                         // Where to write the code
        );

    HRESULT WriteEntryPointEnd(
        __in IStringStream* pOutput                                         // Where to write the code
        );

    void AddHLSLFunctionUsage(HLSLFunctions::Enum function);
    void AddFeatureUsage(FeatureUsedFlags::Enum feature);
    
    HRESULT AddSpecialVariableUsage(
        __in const YYLTYPE* pLocation,                                      // Location of usage
        GLSLSpecialVariables::Enum variable                                 // Special variable being used
        );

    void SetVaryingStructInfo(__in CGLSLIOStructInfo* pInfo) { Assert(_spVaryingStructInfo == nullptr); _spVaryingStructInfo = pInfo; }

    HRESULT CloneSamplerAsTexture(
        __in ParseTreeNode* pSamplerNode,                                   // Node that represents a sampler to make a texture clone for
        __inout UINT* puSamplerCounter                                      // Counter to pass down to cloning wrapper
        );

    UINT GetFeaturesUsed() const { return _uFeaturesUsed; }
    static bool IsFeatureUsed(UINT uFlags, FeatureUsedFlags::Enum feature) { return (uFlags & feature) == feature; }

    // ITextNodeProvider implementation
    HRESULT OnOutputHLSL(__in IStringStream* pOutput) override;

    HRESULT WrapSimpleStatementAsCompoundStatement(
        __in ParseTreeNode* pSimpleStatement,                               // The simple statment to wrap
        bool fDefinesScope,                                                 // Whether the created compound statement defines a scope or not
        __deref_out CompoundStatementNode** ppCompoundStatement             // The created compound statement
        );

private:
    HRESULT TranslateGlobalDeclarations();
    HRESULT DetermineEntryPoint(__deref_out_opt FunctionDefinitionNode** ppEntryPoint);
    HRESULT MoveDeclarationsBeforeEntryPoint(__in const FunctionDefinitionNode* pEntryPoint);
    HRESULT MoveGlobalInitializersIntoStatementList(__in CollectionNode* pStatementList);
    HRESULT AddFunctionCallToEntryPointBegin(
        const FunctionDefinitionNode* pEntryPoint,                          // The entry point function definition where the function call statement should be added
        int iFunctionIdent                                                  // The identifier of the function to call
        );
    HRESULT TranslateStructDeclarations();
    HRESULT TranslateShortCircuitExpressions();
    HRESULT TranslateSamplers();
    HRESULT TranslateInputs(__in CMemoryStream* pOutput);
    HRESULT VerifyInputs();

    HRESULT TranslateTree(
        bool fWriteInputs,                                                  // Flag used in testing to suppress input structures from output
        __deref_out CMemoryStream** ppConverted                             // Stream with converted HLSL
        );

    bool IsFeatureUsed(FeatureUsedFlags::Enum feature) const { return IsFeatureUsed(_uFeaturesUsed, feature); }

    HRESULT WriteTextureFunction(
        FeatureUsedFlags::Enum featureUsed,                                 // Feature used code for function
        __in IStringStream* pOutput                                         // Where to write the code
        );

    virtual HRESULT DumpTree();

    static bool IsValidExpressionInsertionPoint(__in CollectionNode* pParent);

    HRESULT GenerateFunctionDeclarationAndDefinition(
        __deref_out FunctionPrototypeDeclarationNode** ppFunctionDeclaration,   // Generated function declaration node
        __deref_out FunctionDefinitionNode** ppFunctionDefinition,              // Generated function definition node
        __out int *piFunctionIdent,                                             // Generated function identifier symbol that defines the generated function
        __deref_out CollectionNode** ppStatementList                            // After this function is generated, the statement list parent to insert statements inside the function definition
        );

    HRESULT CGLSLParser::GenerateNoArgsFunctionPrototype(
        int iSymbolIndex,                                                   // Symbol to use to generate a function identifier for the prototype
        __deref_out FunctionPrototypeNode** ppFunctionPrototype             // Created prototype
        );

    HRESULT FixupDeclarationForShortCircuiting(__in InitDeclaratorListNode* pInitDeclList, __in ParseTreeNode* pChild);

    HRESULT MoveExpressionsToInsertionPoint(
        __in CollectionNode* pParent,                                       // Found parent to insert to  
        __in ParseTreeNode* pChild,                                         // Child branch walked up to pParent
        const CModernArray<TSmartPointer<ParseTreeNode>>& aryOrderedExpressionsToMove   // List of nodes to move
        );

    HRESULT ConvertExpressionToStatement(
        __in ParseTreeNode* pExpr,                                          // Expression node to convert
        __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
        __deref_out ParseTreeNode** ppStatementToInsert                     // Generated expression statement
        );

    template <typename T>
    HRESULT GenerateIdentifierNode(__deref_out T** ppIdentifier);
    template <typename T>
    HRESULT GenerateIdentifierNodeFromSymbol(int iSymbolIndex, __deref_out T** ppIdentifier);
    HRESULT GenerateIdentifierSymbol(__out int* piIdentifier);

    HRESULT GeneratePlaceholderDeclaration(
        __in GLSLType* pExprType,                                           // Type of an expression for which a placeholder variable needs to be generated
        __in_opt ParseTreeNode* pInitializer,                               // Optional initializer to init the placeholder variable with
        __deref_out InitDeclaratorListNode** ppInitDeclaratorListNode       // The generated declaration. Must be verified after attaching to the tree for the variable to be considered as declared and in scope
        );

    HRESULT GenerateAssignmentStatement(
        __in ParseTreeNode* pExpr,                                          // Expression to assign from
        __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
        __deref_out ParseTreeNode** ppAssignStatement                       // Generated assignment statement
        );

    HRESULT ConvertShortCircuitToSelection(
        __in ParseTreeNode* pExpr,                                          // Short circuit expression to convert
        __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
        __deref_out ParseTreeNode** ppSelectionConverted                    // Created SelectionStatement node
        );

    HRESULT CreateSelectionFromParts(
        __in ParseTreeNode* pTestExpr,                                      // Expression to test in if
        __in ParseTreeNode* pTrueExpr,                                      // What to do if true
        __in ParseTreeNode* pFalseExpr,                                     // What to do if false
        __in VariableIdentifierNode* pPlaceholderVariable,                  // Variable used to assign the result of the expression into
        __deref_out ParseTreeNode** ppSelectionConverted                    // Created SelectionStatement node
        );

private:
    // Bison / flex integration
    GLSLShaderType::Enum _shaderType;                                       // Indicates if this is a vertex or pixel shader
    int _realLine;                                                          // The line counter used while parsing
    int _logicalLine;                                                       // The calculated line (after effect of #line)
    int _column;                                                            // The column counter used while parsing
    bool _fErrors;                                                          // Whether errors are found

    // Input / output
    TSmartPointer<IParserInput> _spInput;                                   // The input to the parser
    TSmartPointer<CGLSLLineMap> _spLineMap;                                 // The line map from the preprocessor
    TSmartPointer<CGLSLExtensionState> _spExtensionState;                   // Extension state from the preprocessor
    TSmartPointer<CGLSLConvertedShader> _spConverted;                       // The converted shader

    // Options
    bool _fWriteInputs;                                                     // Whether to write HLSL inputs into conversion
    bool _fWriteBoilerPlate;                                                // Whether to output boilerplate code such as function wrappers and special variable calculation
    bool _fHasNonConstGlobalInitializers;                                   // Whether there are one or more non-const initializer expressions for global declarations

    // Translation
    TSmartPointer<TranslationUnitCollectionNode> _spRootNode;               // The root node of the parse tree
    TSmartPointer<FunctionDefinitionNode> _spEntryPoint;                    // The node that represents the function definition of 'main()'
    TSmartPointer<CGLSLSymbolTable> _spSymbolTable;                         // The symbol table
    TSmartPointer<CGLSLIdentifierTable> _spIdTable;                         // The identifier table
    CModernArray<TSmartPointer<InitDeclaratorListNode>> _aryDeclarations;   // All of the variable declarations that have been found
    CModernArray<TSmartPointer<ParseTreeNode>> _aryShortCircuitExprs;       // All of the short circuit expressions that have been found
    TSmartPointer<CGLSLIOStructInfo> _spVaryingStructInfo;                  // Varying struct info
    int _currentScopeId;                                                    // The current scope id
    UINT _generatedIdentifierId;                                            // Unique id for generating identifiers
    UINT _uFeaturesUsed;                                                    // Indicates what optional features were used in verification
    WebGLFeatureLevel _glFeatureLevel;                                      // Feature level we're translating for

    static const UINT s_uMaxShaderSize;                                     // Maximum size of input to GLSL parser
    static const char* s_pszMaxShaderSizeString;                            // Max size as string
    static const YYLTYPE s_nullLocation;                                    // Placeholder location for generated parse tree nodes
};
