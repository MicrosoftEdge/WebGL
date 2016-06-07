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
#include "GLSLPreParser.hxx"
#include "MemoryStream.hxx"
#include "RefCounted.hxx"
#include "GLSLPreExpandBuffer.hxx"
#include "GLSLUnicodeConverter.hxx"
#include "GLSLTranslateOptions.hxx"
#include "WebGLConstants.hxx"
#pragma warning(disable:28718)
#define YY_HEADER_EXPORT_START_CONDITIONS
#include "lex.pre.h"

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
CGLSLPreParser::CGLSLPreParser() : 
    _scanner(nullptr),
    _realLine(1),
    _logicalLine(1),
    _logicalFile(0),
    _column(1),
    _fErrors(false),
    _commentCondition(INITIAL),
    _lineSymbol(-1),
    _fProcessedStatement(false),
    _fNonWhitespaceOnLine(false)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//  Synopsis:   Initialize the tokenizer and make our copy of the input.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::Initialize(
    __in IParserInput* pInput,                                          // Input to preprocess
    __in IErrorSink* pErrorSink,                                        // Where to store errors if you have them
    UINT uOptions,                                                      // Translation options
    GLSLShaderType::Enum shaderType                                     // Type of shader
    )
{
    CHK_START;

    _spInput = pInput;
    _spErrorSink = pErrorSink;

    // Set up the symbol table
    CHK(RefCounted<CGLSLSymbolTable>::Create(/*out*/_spSymbolTable));
    
    // Create a line map object
    CHK(RefCounted<CGLSLLineMap>::Create(/*out*/_spLineMap));

    // Add builtin definitions
    CHK(AddDefinition("GL_ES", "1", nullptr));
    CHK(AddDefinition("__VERSION__", "100", nullptr));
    CHK(AddDefinition("__LINE__", "0", &_lineSymbol));
    CHK(AddDefinition("__FILE__", "0", &_fileSymbol));
    CHK(AddDefinition("GL_FRAGMENT_PRECISION_HIGH", "1", nullptr));

    // Add extension information
    CHK(RefCounted<CGLSLExtensionState>::Create(/*out*/_spExtensionState));
    CHK(_spExtensionState->AddEntry(
        GLSLExtension::all, 
        true,                               // all as an entry is supported
        GLSLExtensionBehavior::disable      // Spec says that disable all is initial starting point
        ));

    if (shaderType == GLSLShaderType::Fragment)
    {
        bool fEnabled = (uOptions & GLSLTranslateOptions::EnableStandardDerivatives) != 0;

        // Derivatives only in fragment shader
        CHK(_spExtensionState->AddEntry(
            GLSLExtension::GL_OES_standard_derivatives,
            fEnabled,
            GLSLExtensionBehavior::notSet
            ));

        if (fEnabled)
        {
            CHK(AddDefinition("GL_OES_standard_derivatives", "1", nullptr));
        }

        fEnabled = (uOptions & GLSLTranslateOptions::EnableFragDepth) != 0;

        // Fragment depth only in fragment shader
        CHK(_spExtensionState->AddEntry(
            GLSLExtension::GL_EXT_frag_depth,
            fEnabled,
            GLSLExtensionBehavior::notSet
            ));

        if (fEnabled)
        {
            CHK(AddDefinition("GL_EXT_frag_depth", "1", nullptr));
        }
    }

    // Create our output stream
    CHK(RefCounted<CMemoryStream>::Create(/*out*/_spOutput));

    // Initialize our allow stack to the initial state, which is passing code through
    _conditionState._fAllowOutput = true;

    // Create and kick off the scanner
    GLSLPrelex_init(&_scanner);
    GLSLPreset_extra(this, _scanner);
    GLSLPreparse(_scanner); 

    // If we have more than one buffer left, then we should delete it. The macro
    // expansion did not unroll naturally and the extra created buffers should be
    // deleted.
    while (SUCCEEDED(PopBufferState())) {}

    GLSLPrelex_destroy(_scanner);

    _scanner = nullptr;

    // No errors allowed
    CHKB(!_fErrors);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   LogError
//
//  Synopsis:   Called to log specific errors when found during preprocessing.
//
//              We set a boolean and we add to a list, because if we hit
//              an out of memory error logging the error, we don't want to
//              think that we had no errors.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::LogError(
    __in_opt const YYLTYPE *pLocation,                          // Where the error occured
    HRESULT hrCode,                                             // The code for the error
    __in_z_opt const char* pszErrorData                         // Optional string data
    )
{
    _fErrors = true;

    return _spErrorSink->AddError(pLocation->first_line, pLocation->first_column, hrCode, pszErrorData);
}

//+----------------------------------------------------------------------------
//
//  Function:   NotifyError
//
//  Synopsis:   Method called when yyerror happens. Used to log the error.
//
//-----------------------------------------------------------------------------
void CGLSLPreParser::NotifyError(__in YYLTYPE* pLocation, __in_z_opt const char* pszError)
{
    if (pszError == nullptr)
    {
        pszError = "Internal preprocessor error";
    }

    LogError(pLocation, E_GLSLERROR_SYNTAXERROR, pszError);
}

//+----------------------------------------------------------------------------
//
//  Function:   UpdateLocation
//
//  Synopsis:   Called to pass along location information.
//
//-----------------------------------------------------------------------------
void CGLSLPreParser::UpdateLocation(__in YYLTYPE* pLocation, int lineNo, int tokenLength)
{
    pLocation->first_line = pLocation->last_line = _logicalLine;
    pLocation->first_column = _column;    
    pLocation->last_column = _column + tokenLength - 1;

    _column += tokenLength;
}

//+----------------------------------------------------------------------------
//
//  Function:   ProcessTextToken
//
//  Synopsis:   Called to process tokens that have not been deemed to be part
//              of directing the preprocessor of what to do - these will end
//              up accumulating somewhere and eventually form part of the
//              output.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::ProcessTextToken(
    __in_z char* pszText,                                               // Token to process
    int length,                                                         // Length of token
    __in YYLTYPE* pLocation                                             // Location of token
    )
{
    CHK_START;

    // Restrict token length
    CHK(VerifyTokenLength(length, pLocation));

    if (!_fNonWhitespaceOnLine)
    {
        // Check if any non-whitespace was added
        for (int i = 0; i < length; i++)
        {
            if (!CGLSLUnicodeConverter::IsWhitespaceChar(pszText[i]))
            {
                _fNonWhitespaceOnLine = true;
                break;
            }
        }
    }

    if (_conditionState._fAllowOutput)
    {
        if (_paramStack.Size() == 0)
        {
            CHK(_spOutput->WriteString(pszText));
        }
        else
        {
            ParamState topState;
            CHK(_paramStack.Top(topState));            
            CHK(topState._spParamList->AddToken(pszText));
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   EnsureSymbolIndex
//
//  Synopsis:   Wrapper for symbol table function that checks length.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::EnsureSymbolIndex(
    __in_z char* pszName,                                               // Name of symbol
    int length,                                                         // Length of symbol
    __in YYLTYPE* pLocation,                                            // Location of symbol
    __out int* pIndex                                                   // Index of symbol in table
    )
{
    CHK_START;

    // Restrict token length
    CHK(VerifyTokenLength(length, pLocation));
    CHK(_spSymbolTable->EnsureSymbolIndex(pszName, pIndex));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyTokenLength
//
//  Synopsis:   Verify that the token is not too long.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::VerifyTokenLength(
    int length,                                                         // Length of text
    __in YYLTYPE* pLocation                                             // Location of text
    )
{
    CHK_START;

    // Restrict token length
    if (length > MAX_GLSL_TOKEN_SIZE)
    {
        CHK(LogError(pLocation, E_GLSLERROR_PREMAXTOKEN, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);             
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetMacroDefinition
//
//  Synopsis:   When an identifier token is encountered outside of a directive,
//              it needs to be expanded if it maps to a macro definition.
//
//              This method tests if the given identifier text maps to a macro.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::GetMacroDefinition(
    __in_z char* pszText,                                               // Text to test for being a macro
    __deref_out CGLSLPreMacroDefinition** ppDefinition                  // Definition of that macro
    )
{
    CHK_START;

    // See if we have a symbol - no symbol means no definition
    int symIndex;
    CHK(_spSymbolTable->LookupSymbolIndex(pszText, &symIndex));

    // See if we have a definition
    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    CHK(_rgDefinitions.FindDefinition(symIndex, /*index*/nullptr, &spDefinition));

    // The line macro might be stale - update it
    int identifierIndex = spDefinition->GetIdentifierIndex();
    if (identifierIndex == _lineSymbol || identifierIndex == _fileSymbol)
    {
        CHK(spDefinition->SetToNumber((identifierIndex == _lineSymbol) ? _logicalLine : _logicalFile));
    }

    // If this definition is the one we are expanding, then pretend we could
    // not find it. We need to check all the way up the chain because if you
    // are two definitions deep, neither should expand themselves.
    for (UINT i = 0; i < _bufferStack.Size(); i++)
    {
        if (_bufferStack.ItemRef(i)._spDefinition->GetIdentifierIndex() == symIndex)
        {
            CHK(E_INVALIDARG);
        }
    }

    (*ppDefinition) = spDefinition.Extract();

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreProcessNewline
//
//  Synopsis:   Called in all conditions when a newline is encountered. The
//              column counter is reset and the output is kept in sync with
//              regard to line count.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreProcessNewline()
{
    CHK_START;

    CHK(_spOutput->WriteChar('\n'));

    // Reset column counter and increment line counter
    _realLine++;
    _logicalLine++;
    _column = 1;

    // _commentCondition is the condition that the lexer will return to when
    // a C comment (/* */) ends. If we are preprocessing a newline, it means
    // that any directive that was being processed has finished and when the
    // comment ends, processing must resume at the initial condition.
    //
    // We only reset the comment condition for the case where we started inside
    // of a directive. If we start inside of any other kind of condition, we
    // must not modify states when we are done because we have a stack to
    // deal with.
    if (IsDirectiveCondition(_commentCondition))
    {
        _commentCondition = INITIAL;
    }

    // Reset non-whitespace flag since it is a per line flag
    _fNonWhitespaceOnLine = false;

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsDirectiveCondition
//
//  Synopsis:   Returns true if the given lexer condition is one of the
//              directive conditions.
//
//-----------------------------------------------------------------------------
bool CGLSLPreParser::IsDirectiveCondition(int cond)
{
    return (
        cond == EXPAND_DIR_COND || 
        cond == NOEXPAND_DIR_COND || 
        cond == DEF_DIR_COND || 
        cond == DEF_DIR_PARAM_P_COND || 
        cond == DEF_DIR_PARAM_I_COND ||
        cond == DEF_DIR_PARAM_S_COND ||
        cond == NOOUTPUT
        );
}

//+----------------------------------------------------------------------------
//
//  Function:   PreProcessCommentText
//
//  Synopsis:   Called when C style comments are processed - the length
//              indicates how many spaces to output to the stream.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreProcessCommentText(int length)
{
    CHK_START;

    // We eat spaces while collecting parameters for macro expansion
    // since we only preserve spaces to have accurate error positions, and
    // expanding a macro ruins the error positions anyway.
    //
    // If there is a parameter stack, then we are processing parameters
    // and we can give up on doing anything about comments.
    if (_paramStack.Size() == 0)
    {
        // Don't output replacement spaces on a directive line if a comment started
        // there. We want directive lines to be blank lines in the output. If the
        // comment started on a directive condition it means that a comment
        // has started and it is still on a directive line, and therefore we can
        // eat these spaces.
        if (!IsDirectiveCondition(_commentCondition))
        {
            for (int i = 0; i < length; i++)
            {
                CHK(_spOutput->WriteChar(' '));
            }
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreIfImpl
//
//  Synopsis:   Contains the meat of the work to do ifdef / ifndef / if
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreIfImpl(bool fVal)
{
    CHK_START;

    // Set the flag that a statement is being processed - conditional statements
    // set through this path rather than using ProcessStatement() because they
    // recursively have other statements inside. If conditional statements were
    // to use ProcessStatement(), then the code would run after they process and 
    // not before, and it would not work correctly. Hence it is done here.
    _fProcessedStatement = true;

    // Push the old allow state to the stack so that we can restore it on the #endif
    // for this directive. Even if we are not currently allowing code, we still have
    // to have matching ifdef / endif statements.
    //
    // This is needed because later on, if an #else directive is encountered, we
    // need to know what the state was before the #ifdef was encountered.
    CHK(_conditionStack.Push(_conditionState));

    // Reset whether the condition is met until we see it met somewhere
    _conditionState._fConditionMet = false;

    // If we are allowing code right now, then this new ifdef can alter our ability
    // to allow output and we process it. If we are not allowing output right now, 
    // then another define cannot change that.
    if (_conditionState._fAllowOutput)
    {
        // Whether we allow code to flow through depends on if the entry is defined
        _conditionState._fAllowOutput =  fVal;

        if (fVal)
        {
            // If the value is met, then we found a true condition
            _conditionState._fConditionMet = true;
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreIf
//
//  Synopsis:   If directives have an expression after them. We evaluate the
//              expression and change the output allow appropriately.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreIf(bool fExpr)
{
    // based on the value of the expression, do the logic
    return PreIfImpl(fExpr);
}

//+----------------------------------------------------------------------------
//
//  Function:   IsPreviousOutputAllowed
//
//  Synopsis:   Returns if the output was allowed before the current state
//              was pushed.
//
//-----------------------------------------------------------------------------
bool CGLSLPreParser::IsPreviousOutputAllowed() const
{
    ConditionState previousState;
    if (SUCCEEDED(_conditionStack.Top(/*out*/previousState)))
    {
        return previousState._fAllowOutput;
    }
    else
    {
        // Incorrect usage of directives (like just starting your file with
        // #elif) can cause this path to be hit. We are not really accurate
        // with our return value here, but there will be a syntax error
        // anyway.
        return false;
    }
}

//+----------------------------------------------------------------------------
//
//  Function:   PreElif
//
//  Synopsis:   Elif does the same thing as else + if, with the obvious caveat
//              that an extra endif is not required.
//
//              This makes it very similar to else, except instead of looking
//              at the previous allow value to determine what to do, it also
//              looks at the expression.
//
//              Care must be taken to not go into multiple blocks - for 
//              example, this code:
//
//              #if 1
//              Line1
//              #elif 1
//              Line2
//              #endif
//
//              Should not print both Line1 and Line2, even though the
//              condition is met both times.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreElif(bool fExpr)
{
    CHK_START;

    ConditionState previousState;
    
    // Get the top of the stack - this will tell us what the value of 
    // _conditionState._fAllowOutput was when the ifdef was done.
    //
    // If previousState._fAllowOutput is true, then output was enabled when 
    // the ifdef statement was encountered. So we need to evaluate the 
    // expression and look at the current condition to set what we can do.
    //
    // If previousState._fAllowOutput is false, it means that the corresponding 
    // if for this else was itself inside of a disallowed code block and 
    // therefore the else block, just like the if block it is related to, 
    // should do nothing.
    CHK(_conditionStack.Top(/*out*/previousState));

    if (previousState._fAllowOutput)
    {
        // We 'go into the branch' if we did not go into a previous
        // one and the expression evaluates to true.
        bool fConditionMet = (!_conditionState._fConditionMet && fExpr);

        // Allow output if the condition is met
        _conditionState._fAllowOutput = fConditionMet;

        if (fConditionMet)
        {
            // If the condition is met, then we found a true condition
            _conditionState._fConditionMet = true;
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreIfDef
//
//  Synopsis:   Ifdef directives have a symbol after them. This symbol is
//              checked for existing, and the allow stack pushes the 
//              appropriate flag.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreIfDef(int iIdSymbol)
{
    return PreIfImpl(IsDefined(iIdSymbol));
}

//+----------------------------------------------------------------------------
//
//  Function:   PreIfNDef
//
//  Synopsis:   Ifndef directives have a symbol after them. This symbol is
//              checked for existing, and the allow stack pushes the 
//              appropriate flag.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreIfNDef(int iIdSymbol)
{
    return PreIfImpl(!IsDefined(iIdSymbol));
}

//+----------------------------------------------------------------------------
//
//  Function:   PreElse
//
//  Synopsis:   Reverses the condition of the allow stack. This should have the
//              same effect as popping the stack and immediately pushing the
//              reverse condition as before.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreElse()
{
    CHK_START;

    ConditionState previousState;
        
    // Get the top of the stack - this will tell us what the value of 
    // _conditionState._fAllowOutput was when the ifdef was done.
    //
    // If previousState._fAllowOutput is true, then output was enabled when 
    // the ifdef statement was encountered. So we either need to go from on to 
    // off or off to on.
    //
    // If previousState._fAllowOutput is false, it means that the corresponding 
    // if for this else was itself inside of a disallowed code block and 
    // therefore the elseblock, just like the if block it is related to, should 
    // do nothing.
    CHK(_conditionStack.Top(/*out*/previousState));

    if (previousState._fAllowOutput)
    {
        _conditionState._fAllowOutput = !_conditionState._fConditionMet;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreEndDef
//
//  Synopsis:   Pops the allow stack.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreEndIf()
{
    // Restore the output allow state
    _conditionStack.Pop(/*out*/_conditionState);

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreDefine
//
//  Synopsis:   Adds a definition to the preprocessor, given a symbol table
//              token index and a definition table index. All of the define
//              statement generations end up calling this, so at the point
//              that this is called, all of the information to do define
//              or redefine a macro is there.
//
//              The token index can be -1, which means that the macro name
//              has already been set when the definition was created. This
//              occurs for macros with parameters.
//
//              The definition index can be -1, which means that a definition
//              with no tokens is created. This is perfectly legal.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreDefine(
    int iToken,                                                         // Token that specifies the macro being defined
    int defIndex,                                                       // Index of the definition of what this define is (what replaces the token)
    const YYLTYPE& pLocation                                            // Location of the define statement
    )
{
    CHK_START;

    // Don't use the definition if the output is disabled
    if (_conditionState._fAllowOutput)
    {
        TSmartPointer<CGLSLPreMacroDefinition> spDefinition;

        if (iToken != -1)
        {
            // We have an identifier for the macro but we might not have a definition
            if (defIndex == -1)
            {
                // Create a definition to set the token on
                CHK(_rgDefinitions.CreateAndAddDefinition(this, /*index*/nullptr, &spDefinition));

                // Make sure that at least a space is used in this case
                CHK(spDefinition->AddToken(" "));
            }
            else
            {
                // Set the identifier on the existing definition
                CHK(_rgDefinitions.GetDefinition(defIndex, &spDefinition));
            }

            spDefinition->SetIdentifierIndex(iToken);
        }
        else
        {
            // If there is no token, then the definition needs to be valid
            CHKB(defIndex != -1);
            CHK(_rgDefinitions.GetDefinition(defIndex, &spDefinition));

            // The identifier index needs to be set on this definition by now
            iToken = spDefinition->GetIdentifierIndex();
            CHKB(iToken != -1);
        }

        // We should have a valid token by this point
        Assert(iToken != -1);

        // Now see if this macro was already defined
        int existingIndex;
        TSmartPointer<CGLSLPreMacroDefinition> spExisting;
        if (SUCCEEDED(_rgDefinitions.FindDefinition(iToken, &existingIndex, &spExisting)))
        {
            // An existing finalized definition has been found - the new one
            // has to have a matching definition.
            if (!spExisting->IsEqual(spDefinition))
            {
                CHK(_spErrorSink->AddError(pLocation.first_line, pLocation.first_column, E_GLSLERROR_MACRODEFINITIONNOTEQUAL, nullptr));
                CHK(E_GLSLERROR_KNOWNERROR);
            }
            
            // Remove the new one and leave the old one
            _rgDefinitions.RemoveDefinitionFromIndex(existingIndex);
        }

        // Now we can finalize what we have
        spDefinition->Finalize();

        // Make sure the macro name is legal
        if (FAILED(VerifyMacroName(iToken)))
        {
            CHK(_spErrorSink->AddError(pLocation.first_line, pLocation.first_column, E_GLSLERROR_INVALIDMACRONAME, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreUndefine
//
//  Synopsis:   Removes a definition from the preprocessor, given a symbol table
//              token index.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreUndefine(
    int iToken,                                                         // Token that specifies the macro being undefined
    const YYLTYPE& pLocation                                            // Location of the undefine statement
    )
{
    // Don't remove the definition if the output is disabled
    if (_conditionState._fAllowOutput)
    {
        // Undef of a thing that is not defined is perfectly legal, so no error condition here
        _rgDefinitions.RemoveDefinitionFromIdentifier(iToken);
    }

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetCommentCondition
//
//  Synopsis:   Set the condition that a C comment started in. Since they can
//              span lines, the condition that they return to might need to
//              change. The preparser keeps track of that.
//
//-----------------------------------------------------------------------------
void CGLSLPreParser::SetCommentCondition(int condition)
{
    _commentCondition = condition;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetCommentCondition
//
//  Synopsis:   Get the condition that a C comment should return to. This will
//              be the value that SetCommentCondition last set, unless there
//              was a newline encountered, in which case it will return to
//              INITIAL.
//
//-----------------------------------------------------------------------------
int CGLSLPreParser::GetCommentCondition()
{
    return _commentCondition;
}

//+----------------------------------------------------------------------------
//
//  Function:   IsDefined
//
//  Synopsis:   Return true if the given token is defined.
//
//-----------------------------------------------------------------------------
bool CGLSLPreParser::IsDefined(int token)
{
    return SUCCEEDED(_rgDefinitions.FindDefinition(token, nullptr, nullptr));
}

//+----------------------------------------------------------------------------
//
//  Function:   AddDefinition
//
//  Synopsis:   Add a definition from two strings - used to add builtin
//              definitions.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::AddDefinition(
    __in_z const char* pszToken,                                        // Macro identifier
    __in_z const char* pszValue,                                        // First token in macro value
    __out_opt int* pSymbolIndex                                         // Symbol index of macro identifier
    )
{
    CHK_START;

    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    CHK(_rgDefinitions.CreateAndAddDefinition(this, nullptr, &spDefinition));
    CHK(spDefinition->AddToken(pszValue));

    int token;
    CHK(_spSymbolTable->EnsureSymbolIndex(pszToken, &token));
    spDefinition->SetIdentifierIndex(token);
    spDefinition->Finalize();

    if (pSymbolIndex != nullptr)
    {
        (*pSymbolIndex) = token;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateDefinition
//
//  Synopsis:   Create a definition, and add the given text if it is not
//              nullptr as the first token. This is the form called by
//              the production in the grammar file.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::CreateDefinition(
    __in_z_opt const char* pszText,                                 // Optional token to start the definition
    __out int& defIndex                                             // Returned index of definition
    )
{
    CHK_START;

    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    CHK(_rgDefinitions.CreateAndAddDefinition(this, &defIndex, &spDefinition));

    // If somebody provided a starting token, then add it
    if (pszText != nullptr)
    {
        CHK(spDefinition->AddToken(pszText));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddDefinitionToken
//
//  Synopsis:   Add the given token to the given definition.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::AddDefinitionToken(int defIndex, __in_z const char* pszText)
{
    CHK_START;

    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    CHK(_rgDefinitions.GetDefinition(defIndex, &spDefinition));
    CHK(spDefinition->AddToken(pszText));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PushBufferState
//
//  Synopsis:   Push the given scan buffer onto the stack, and use the currently
//              loaded up macro parameters (if any) to create a buffer to
//              recommence scanning with.
//
//              This function needs to be declared with a void parameter
//              because of a circular reference problem with headers.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PushBufferState(
    __in YYLTYPE* pLocation,                                            // Location of macro being expanded
    __in_opt CGLSLPreMacroDefinition* pDefinition,                      // The macro being expanded
    void* oldState                                                      // The buffer to push (YY_CURRENT_BUFFER)
    )
{
    CHK_START;

    TSmartPointer<CGLSLPreMacroDefinition> spPopDefinition;
    TSmartPointer<CGLSLPreParamList> spParamList;

    // If we don't have a definition, it is because we are closing off the parameter
    // list. We need to pop the parameter list off and get the definition from there
    if (pDefinition == nullptr)
    {
        CHK(PopMacroParam(&spPopDefinition, nullptr, &spParamList));

        pDefinition = spPopDefinition;
    }

    TSmartPointer<CGLSLPreExpandBuffer> spNewBuffer;
    CHK(RefCounted<CGLSLPreExpandBuffer>::Create(pLocation, pDefinition, spParamList, /*out*/spNewBuffer));

    YY_BUFFER_STATE newState = GLSLPre_scan_string(spNewBuffer->GetBufferString(), _scanner);

    // Set up the new state
    BufferState newEntry;
    newEntry._oldState = oldState;
    newEntry._newState = newState;
    newEntry._spBuffer = spNewBuffer;
    newEntry._spDefinition = pDefinition;

    // Push it onto the stack
    CHK(_bufferStack.Push(newEntry));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PopBufferState
//
//  Synopsis:   Pop the given buffer from the stack.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PopBufferState()        
{
    CHK_START;

    BufferState popState;
    CHK(_bufferStack.Pop(/*out*/popState));

    GLSLPre_delete_buffer(static_cast<YY_BUFFER_STATE>(popState._newState), _scanner);
    GLSLPre_switch_to_buffer(static_cast<YY_BUFFER_STATE>(popState._oldState), _scanner); 

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PushMacroParam
//
//  Synopsis:   Push a new macro parameter context for calling a macro and
//              expanding it.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PushMacroParam(__in CGLSLPreMacroDefinition* pDefinition) 
{
    CHK_START;

    // Create the new entry - use the given definition and create a new param list
    ParamState newState;
    newState._spDefinition = pDefinition;
    newState._defIndex = -1;
    CHK(RefCounted<CGLSLPreParamList>::Create(/*out*/newState._spParamList));

    // Push onto the stack
    CHK(_paramStack.Push(newState));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PushMacroParam
//
//  Synopsis:   Push a new macro parameter context for defining the parameters
//              of a macro.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PushMacroParam(
    const char* pszIdentifier,                                          // Identifier of macro
    int length,                                                         // Length of macro identifier
    __in YYLTYPE* pLocation                                             // Location of identifier
    ) 
{
    CHK_START;

    // Restrict token length
    CHK(VerifyTokenLength(length, pLocation));

    // We need to convert the identifier into a symbol table index
    int idIndex;
    CHK(_spSymbolTable->EnsureSymbolIndex(pszIdentifier, &idIndex));

    // Set up the new parameter state by creating a new definition
    ParamState newState;
    CHK(_rgDefinitions.CreateAndAddDefinition(
        this, 
        &newState._defIndex, 
        &newState._spDefinition
        ));
    newState._spDefinition->SetIdentifierIndex(idIndex);

    // Create the parameter list
    CHK(RefCounted<CGLSLPreParamList>::Create(/*out*/newState._spParamList));

    // Push onto the stack
    CHK(_paramStack.Push(newState));

    CHK_RETURN;
}


//+----------------------------------------------------------------------------
//
//  Function:   NextMacroParam
//
//  Synopsis:   Move onto the next parameter - do this by adding another
//              parameter to the macro.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::NextMacroParam() 
{
    CHK_START;

    // Cannot move to next param unless you are in a param. This indicates
    // a bug because our Flex state should never allow this.
    Assert(_paramStack.Size() != 0);
    CHKB(_paramStack.Size() != 0);

    // Add a parameter to the macro
    ParamState topState;
    CHK(_paramStack.Top(/*out*/topState));
    CHK(topState._spParamList->AddParameter());

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PopMacroParam
//
//  Synopsis:   Pop the parameter context.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PopMacroParam(
    __deref_out CGLSLPreMacroDefinition** ppDefinition,                 // The definition that is popped off
    __out_opt int* pDefIndex,                                           // The index of the popped off definition
    __deref_opt_out CGLSLPreParamList** ppParamList                     // The parameters that are popped off
    ) 
{
    CHK_START;

    ParamState popState;

    CHK(_paramStack.Pop(/*out*/popState));
    (*ppDefinition) = popState._spDefinition.Extract();
    (*ppParamList) = popState._spParamList.Extract();

    if (pDefIndex != nullptr)
    {
        (*pDefIndex) = popState._defIndex;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CheckNonZero
//
//  Synopsis:   Check if a value is zero - used for divide and mod in expressions.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::CheckNonZero(int val, const YYLTYPE& pLocation)
{
    CHK_START;

    if (val == 0)
    {
        CHK(_spErrorSink->AddError(pLocation.first_line, pLocation.first_column, E_GLSLERROR_DIVIDEORMODBYZERO, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   AddLineMapping
//
//  Synopsis:   Add a line mapping (used for #line directives).
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::AddLineMapping(
    __in_z const char* pszNewLogical                                    // The new logical line number for the next line
    )
{
    // The newline has been encountered and processed already, so set the logical line
    _logicalLine = ::atoi(pszNewLogical);

    return _spLineMap->AddEntry(_realLine, _logicalLine);
}

//+----------------------------------------------------------------------------
//
//  Function:   AddLineAndFileMapping
//
//  Synopsis:   Add a line and file mapping (called from #line directives, affects
//              expansion of __LINE__ and __FILE__ ).
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::AddLineAndFileMapping(
    __in_z const char* pszNewLogicalLine,                                // The new logical line number for the next line
    __in_z const char* pszNewLogicalFile                                 // The new logical file number
    )
{
    CHK_START;

    CHK(AddLineMapping(pszNewLogicalLine));

    _logicalFile = ::atoi(pszNewLogicalFile);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyMacroName
//
//  Synopsis:   Verify that a macro name is legal
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::VerifyMacroName(int iToken)
{
    CHK_START;

    const char* pszText = _spSymbolTable->NameFromIndex(iToken);

    CHKB(pszText != nullptr);

    // Cannot start with this prefix
    CHKB(::strncmp("GL_", pszText, 3) != 0);

    // Consecutive underscores are reserved
    CHKB(::strstr(pszText, "__") == nullptr);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateTokenList
//
//  Synopsis:   Create a token list with a starting token.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::CreateTokenList(
    __in_z const char* pszText,                                         // Token to start the list
    __out int& index                                                    // Returned index of list
    )
{
    CHK_START;

    TSmartPointer<CGLSLPreTokenList> spNewList;
    CHK(RefCounted<CGLSLPreTokenList>::Create(/*out*/spNewList));
    CHK(_aryTokenLists.Add(spNewList));
    CHK(spNewList->AddToken(pszText));

    index = static_cast<int>(_aryTokenLists.GetCount() - 1);

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   CreateTokenList
//
//  Synopsis:   Create a token list with a starting token.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::AddTokenToList(int index, __in_z const char* pszText)
{
    CHK_START;

    CHK_VERIFY(index >= 0 && index < static_cast<int>(_aryTokenLists.GetCount()));

    CHK(_aryTokenLists[index]->AddToken(pszText));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreError
//
//  Synopsis:   Log a user defined error.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreError(int tokenList, const YYLTYPE& location)
{
    CHK_START;

    // Don't report an error unless output is enabled
    if (_conditionState._fAllowOutput)
    {
        CMutableString<char> errorString;

        // Construct the error string from the token list, if one was given
        if (tokenList != -1)
        {
            CHK_VERIFY(tokenList >= 0 && tokenList < static_cast<int>(_aryTokenLists.GetCount()));
            CHK(_aryTokenLists[tokenList]->ConvertToString(errorString));
        }
        else
        {
            // We need a string for the custom error
            CHK(errorString.Append(""));
        }

        // Report the error
        CHK(LogError(&location, E_GLSLERROR_CUSTOM, errorString));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreVersion
//
//  Synopsis:   Check the version
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreVersion(
    int versionSymbol,                                                  // Symbol index for version
    const YYLTYPE& location                                             // Location of directive
    )
{
    CHK_START;

    if (_fProcessedStatement)
    {
        CHK(LogError(&location, E_GLSLERROR_UNEXPECTEDVERSION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    // We know that statements have not been processed, but we might have still allowed
    // some non-whitespace output. Check that we only have whitespace.
    CMutableString<char> spCurrentOutput;
    CHK(_spOutput->ExtractString(/*out*/spCurrentOutput));

    for (UINT i = 0; i < spCurrentOutput.GetLength(); i++)
    {
        if (!CGLSLUnicodeConverter::IsWhitespaceChar(spCurrentOutput[i]))
        {
            CHK(LogError(&location, E_GLSLERROR_UNEXPECTEDVERSION, nullptr));
            CHK(E_GLSLERROR_KNOWNERROR);
        }
    }

    const char* pszVersionText = _spSymbolTable->NameFromIndex(versionSymbol);
    int version = ::atoi(pszVersionText);

    // This needs to be exactly 100
    if (version != 100)
    {
        CHK(LogError(&location, E_GLSLERROR_WRONGVERSION, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   PreExtension
//
//  Synopsis:   Define an extension
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::PreExtension(
    int nameToken,                                                      // Name of extension
    int behaviorToken,                                                  // Setting of extension
    const YYLTYPE& location                                             // Location of the define statement
    )
{
    CHK_START;

    // We only modify extension state if we're in an area where we're performing output,
    // otherwise extensions (along with everything else) are ignored for preprocessing.
    if (IsOutputAllowed())
    {
        const char* pszName = _spSymbolTable->NameFromIndex(nameToken);
        const char* pszBehavior = _spSymbolTable->NameFromIndex(behaviorToken);

        CHK(_spExtensionState->AdjustState(pszName, pszBehavior, location, _spErrorSink));
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifyWhitespaceOnly
//
//  Synopsis:   Called when a statement is being processed to verify that no
//              non-whitespace text preceded on the line.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLPreParser::VerifyWhitespaceOnly(__in YYLTYPE* pLocation)
{ 
    CHK_START;

    if (_fNonWhitespaceOnLine)
    {
        CHK(LogError(pLocation, E_GLSLERROR_PREDIRECTIVENOTFIRST, nullptr));
        CHK(E_GLSLERROR_KNOWNERROR);
    }

    CHK_RETURN;
}
