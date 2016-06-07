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

class CGLSLPreParser;

#include "pre.tab.h"
#include "GLSLSymbolTable.hxx"
#include "GLSLError.hxx"
#include "SimpleStack.hxx"
#include "IParserInput.hxx"
#include "GLSLError.hxx"
#include "IErrorSink.hxx"
#include "GLSLPreMacroDefinitionCollection.hxx"
#include "GLSLPreExpandBuffer.hxx"
#include "GLSLPreParamList.hxx"
#include "GLSLLineMap.hxx"
#include "GLSLPreTokenList.hxx"
#include "GLSLExtensionState.hxx"
#include "GLSLShaderType.hxx"

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLPreParser
//
//  Synopsis:   Class to encapsulate the generated preprocessing scanner and 
//              the associated input / output.
//
//------------------------------------------------------------------------------
class CGLSLPreParser
{
public:
    CGLSLPreParser();

    HRESULT Initialize(
        __in IParserInput* pInput,                                          // Input to preprocess
        __in IErrorSink* pErrorSink,                                        // Where to store errors if you have them
        UINT uOptions,                                                      // Translation options
        GLSLShaderType::Enum shaderType                                     // Type of shader
        );

    // Functions called from the generated parser stack
    void NotifyError(__in YYLTYPE* pLocation, __in_z_opt const char* error);
    void UpdateLocation(__in YYLTYPE* pLocation, int lineNo, int tokenLength);

    HRESULT PreVersion(
        int versionSymbol,                                                  // Symbol index for version
        const YYLTYPE& location                                             // Location of directive
        );

    HRESULT PreIfDef(int iIdSymbol);
    HRESULT PreIfNDef(int iIdSymbol);
    HRESULT PreIf(bool fExpr);
    HRESULT PreElif(bool fExpr);
    HRESULT PreElse();
    HRESULT PreEndIf();
    HRESULT PreError(int tokenList, const YYLTYPE& pLocation);
    
    HRESULT PreExtension(
        int nameToken,                                                      // Name of extension
        int behaviorToken,                                                  // Setting of extension
        const YYLTYPE& location                                             // Location of the define statement
        );

    HRESULT AddLineMapping(
        __in_z const char* pszNewLogical                                    // The new logical line number for the next line
        );

    HRESULT AddLineAndFileMapping(
        __in_z const char* pszNewLogicalLine,                               // The new logical line number for the next line
        __in_z const char* pszNewLogicalFile                                // The new logical file number
        );

    HRESULT PreDefine(
        int iToken,                                                         // Token that specifies the macro being defined
        int defIndex,                                                       // Index of the definition of what this define is (what replaces the token)
        const YYLTYPE& pLocation                                            // Location of the define statement
        );

    HRESULT PreUndefine(
        int iToken,                                                         // Token that specifies the macro being undefined
        const YYLTYPE& pLocation                                            // Location of the undefine statement
        );

    HRESULT GetMacroDefinition(
        __in_z char* pszText,                                               // Text to test for being a macro
        __deref_out CGLSLPreMacroDefinition** ppDefinition                  // Definition of that macro
        );

    HRESULT PreProcessNewline();
    void SetCommentCondition(int condition);
    int GetCommentCondition();
    HRESULT PreProcessCommentText(int length);
    bool IsDefined(int token);
    const char* GetSymbolText(int index) { return _spSymbolTable->NameFromIndex(index); }
    IParserInput* UseInput() { return _spInput; }
    CMemoryStream* UseOutput() { return _spOutput; }
    CGLSLLineMap* UseLineMap() { return _spLineMap; }
    CGLSLExtensionState* UseExtensionState() const { return _spExtensionState; }

    HRESULT EnsureSymbolIndex(
        __in_z char* pszName,                                               // Name of symbol
        int length,                                                         // Length of symbol
        __in YYLTYPE* pLocation,                                            // Location of symbol
        __out int* pIndex                                                   // Index of symbol in table
        );

    HRESULT VerifyTokenLength(
        int length,                                                         // Length of text
        __in YYLTYPE* pLocation                                             // Location of text
        );

    HRESULT ProcessTextToken(
        __in_z char* pszText,                                               // Token to process
        int length,                                                         // Length of token
        __in YYLTYPE* pLocation                                             // Location of token
        );

    HRESULT CreateDefinition(
        __in_z_opt const char* pszText,                                     // Optional token to start the definition
        __out int& defIndex                                                 // Returned index of definition
        );

    HRESULT AddDefinitionToken(int defIndex, __in_z const char* pszText);

    HRESULT CreateTokenList(
        __in_z const char* pszText,                                         // Token to start the list
        __out int& index                                                    // Returned index of list
        );

    HRESULT AddTokenToList(int index, __in_z const char* pszText);

    HRESULT PushBufferState(
        __in YYLTYPE* pLocation,                                            // Location of macro being expanded
        __in_opt CGLSLPreMacroDefinition* pDefinition,                      // The macro being expanded
        void* oldState                                                      // The buffer to push (YY_CURRENT_BUFFER)
        );

    HRESULT PopBufferState();

    HRESULT PushMacroParam(__in CGLSLPreMacroDefinition* pDefinition);

    HRESULT PushMacroParam(
        const char* pszIdentifier,                                          // Identifier of macro
        int length,                                                         // Length of macro identifier
        __in YYLTYPE* pLocation                                             // Location of identifier
        );

    HRESULT NextMacroParam();
    HRESULT PopMacroParam(
        __deref_out CGLSLPreMacroDefinition** ppDefinition,                 // The definition that is popped off
        __out_opt int* pDefIndex,                                           // The index of the popped off definition
        __deref_opt_out CGLSLPreParamList** ppParamList                     // The parameters that are popped off
        );

    static bool IsDirectiveCondition(int cond);
    bool IsOutputAllowed() const { return _conditionState._fAllowOutput; }
    bool IsConditionMet() const { return _conditionState._fConditionMet; }
    bool IsPreviousOutputAllowed() const;

    HRESULT CheckNonZero(int val, const YYLTYPE& pLocation);

    HRESULT LogError(
        __in_opt const YYLTYPE *pLocation,                                  // Where the error occured
        HRESULT hrCode,                                                     // The code for the error
        __in_z_opt const char* pszErrorData                                 // Optional string data
        );

    void ProcessStatement() { _fProcessedStatement = true; }
    HRESULT VerifyWhitespaceOnly(__in YYLTYPE* pLocation);

private:
    HRESULT PreIfImpl(bool fVal);

    HRESULT AddDefinition(
        __in_z const char* pszToken,                                        // Macro identifier
        __in_z const char* pszValue,                                        // First token in macro value
        __out_opt int* pSymbolIndex                                         // Symbol index of macro identifier
        );

    HRESULT VerifyMacroName(int iToken);

private:
    struct BufferState
    {
        void* _oldState;                                                    // The state before we pushed
        void* _newState;                                                    // The state after we pushed
        TSmartPointer<CGLSLPreExpandBuffer> _spBuffer;                      // The buffer being processed on this entry
        TSmartPointer<CGLSLPreMacroDefinition> _spDefinition;               // The definition being expanded on this entry
    };

    struct ParamState
    {
        TSmartPointer<CGLSLPreMacroDefinition> _spDefinition;               // The macro definition we are collecting parameters for
        TSmartPointer<CGLSLPreParamList> _spParamList;                      // The parameter collection
        int _defIndex;                                                      // The index of the macro definition, only valid for macro definition
    };

    struct ConditionState
    {
        ConditionState()
        {
            _fAllowOutput = false;
            _fConditionMet = false;
        }

        bool _fAllowOutput;                                                 // Indicates if code from the input is allowed to the output
        bool _fConditionMet;                                                // Indicates if the condition in an if statement was met in the current scope
    };

private:
    // Bison / flex integration
    void* _scanner;                                                         // The scanner that the parser is using - define with void* to avoid header issues
    int _realLine;                                                          // The line counter used while parsing
    int _logicalLine;                                                       // The calculated line (after effect of #line)
    int _logicalFile;                                                       // The calculated file 'name' (in GLSL this is an integer) after effect of #line
    int _lineSymbol;                                                        // Symbol index for __LINE__
    int _fileSymbol;                                                        // Symbol index for __FILE__
    int _column;                                                            // The column counter used while parsing
    bool _fErrors;                                                          // Whether errors are found
    TSmartPointer<IErrorSink> _spErrorSink;                                 // Where to store errors if you have them

    TSmartPointer<IParserInput> _spInput;                                   // Input string
    TSmartPointer<CMemoryStream> _spOutput;                                 // Output stream

    ConditionState _conditionState;                                         // Current condition state
    int _commentCondition;                                                  // Condition in lexer for C comment to return to when comment ends
    CSimpleStack<ConditionState> _conditionStack;                           // Stack of state that represents if code should be passed through to output
    TSmartPointer<CGLSLSymbolTable> _spSymbolTable;                         // The symbol table
    CGLSLPreMacroDefinitionCollection _rgDefinitions;                       // The macro definitions
    CModernArray<TSmartPointer<CGLSLPreTokenList>> _aryTokenLists;          // The collection of token lists
    CSimpleStack<BufferState> _bufferStack;                                 // Stack of buffers - expanding macros moves deeper into the stack
    CSimpleStack<ParamState> _paramStack;                                   // Stack of collected macro parameters - calling macros moves deeper into the stack
    TSmartPointer<CGLSLLineMap> _spLineMap;                                 // Line mapping
    TSmartPointer<CGLSLExtensionState> _spExtensionState;                   // Extension state
    bool _fProcessedStatement;                                              // Flag to indicate if a statement has been processed
    bool _fNonWhitespaceOnLine;                                             // Flag to indicate that a line has processed non-whitespace as text on the current line
};

int GLSLPreparse(__in void* YYPARSE_PARAM);
