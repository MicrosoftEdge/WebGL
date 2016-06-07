%{
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

#include <stdlib.h>
#include <io.h>

#define fileno _fileno
#define isatty _isatty

class CGLSLPreParser;

#include "pre.tab.h"                /* Bison output has token definitions */
#include "GLSLPreParserGlobals.hxx" /* This is where we define yyerror, yywrap etc */
#include "GLSLPreParamList.hxx"     /* Macro parameter list, used for definitions and invocations */
#include "GLSLMacro.hxx"            /* CHK_PRE_FLEX comes from here */
#include "GLSLPreParser.hxx"        /* We talk to the parser from here */
#include "RefCounted.hxx"           /* Objects are created with this from here */

#pragma warning(disable:4242 4100 4244 4018 4127 4505)

%}

%option reentrant
%option bison-bridge
%option yylineno
%option bison-locations
%option stack
%option outfile = "lex.pre.c"
%option header-file = "lex.pre.h"
%option extra-type = "CGLSLPreParser*"
%option prefix = "GLSLPre"

/* Condition for output being disabled. This happens when an #if* directive
   runs and the condition is false. */
%x NOOUTPUT

/* Conditions for being inside directives. Most of the time, directives will
   expand macros when they are encountered (EXPAND_DIR_COND) but in some cases
   the macro should not be expanded (NOEXPAND_DIR_COND).
   
   #define directives have their own state (DEF_DIR_COND) because they can have 
   parameters in them. There are three additional states for defining macros:
   
   * DEF_DIR_PARAM_P_COND for looking for the paren around the parameters
   * DEF_DIR_PARAM_I_COND for looking for a parameter identifier
   * DEF_DIR_PARAM_C_COND for looking for a parameter separator */
%x EXPAND_DIR_COND
%x NOEXPAND_DIR_COND
%x DEF_DIR_COND
%x DEF_DIR_PARAM_P_COND
%x DEF_DIR_PARAM_I_COND
%x DEF_DIR_PARAM_S_COND

/* Conditions for function macro expansion - P_X_PARAM_COND is the condition
   looking for the open paren, and X_PARAM_COND is the condition collecting
   parameters. */
%x P_X_PARAM_COND
%x X_PARAM_COND

/* Conditions for paren expansion - INSIDE_PAREN_CONDITION is the condition
   of being inside a paren while processing text. */
%x INSIDE_PAREN_COND

/* Exclusive condition for C++ style comments */
%x CPP_COMMENT_COND

/* Exclusive condition for C style comments */
%x C_COMMENT_COND

/* These should be the same as the GLSL lexer */
oseq_opt                                                                                [0-7]*
hex-digit                                                                               [0-9a-fA-F]
glsl_hex                                                                                0[xX]{hex-digit}+
dseq                                                                                    [[:digit:]]+
dseq_opt                                                                                [[:digit:]]*
frac                                                                                    ({dseq_opt}"\."{dseq})|{dseq}"\."
exp                                                                                     [eE][+-]?{dseq}
glsl_float                                                                              ({frac}{exp}?)|({dseq}{exp})
ident                                                                                   [a-zA-Z_][a-zA-Z0-9_]*

/* These are unique to the preprocessor */
space_opt                                                                               [[:space:]]*
linespace                                                                               [[:space:]]{-}[\n]
linespace_opt                                                                           {linespace}*
begindir                                                                                [ \t]*"#"[ \t]*


%%

%{
/* Newlines have very important semantics that always apply because:
   
   * Directives are always limited to a single line
   * C++ style comments end on a newline
   * The preprocessor aims to not change line numbers from the source

   Therefore, processing newlines involves popping off the comment state if need be,
   stopping the directive processing if need be, letting the parser object record the
   newline and returning a token that can be used to verify directive syntax. 
   
   Any directive that has not finished processing is now in error. */
%}
<*>\n                                                                                   {
    if (YY_START == CPP_COMMENT_COND)
    {
        yy_pop_state(yyscanner);
    }

    if (YY_START == DEF_DIR_PARAM_I_COND)
    {
        GLSLPreerror(yylloc, yyscanner, "Syntax error, define directive must finish on a single line");
    }
    
    if (CGLSLPreParser::IsDirectiveCondition(YY_START))
    { 
        yy_pop_state(yyscanner);
    } 
    
    CHK_PRE_FLEX(yyextra->PreProcessNewline()); 
    return NEWLINE_TOK; 
}

%{
/* Characters not valid for GLSL are replaced with '$' during conversion
   from unicode. If we see these characters outside of a comment we will
   end up here.
   
   The position of this rule is chosen so that subsequent rules can match
   to '.' to handle error conditions within themselves. This rule will
   override them because it comes first.
   */
%}
<INITIAL,NOOUTPUT,INSIDE_PAREN_COND,EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND,DEF_DIR_PARAM_I_COND,DEF_DIR_PARAM_S_COND,X_PARAM_COND,P_X_PARAM_COND>"\$"    {
                                                                                        CHK_PRE_FLEX(yyextra->LogError(yylloc, E_GLSLERROR_INVALIDCHARACTER, nullptr)); }

%{
/* Directives always start with the same pattern, and they push the appropriate condition onto the stack. This 
   way they do not lose the current context with regard to parenthesis. */
%}
<INITIAL,INSIDE_PAREN_COND>{begindir}"version"                                          { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return VERSION_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"ifdef"                                            { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return IFDEF_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"ifndef"                                           { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return IFNDEF_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"else"                                             { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return ELSE_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"endif"                                            { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(EXPAND_DIR_COND, yyscanner); return ENDIF_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"if"                                               { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); if (yyextra->IsOutputAllowed()) { yy_push_state(EXPAND_DIR_COND, yyscanner); return IF_TOK; } else { yy_push_state(NOOUTPUT, yyscanner); return NOOUTPUT_IF_TOK; } }
<INITIAL,INSIDE_PAREN_COND>{begindir}"elif"                                             { 
    CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); 
    if (yyextra->IsPreviousOutputAllowed() && !yyextra->IsConditionMet()) 
    {
        yy_push_state(EXPAND_DIR_COND, yyscanner); 
        return ELIF_TOK; 
    } 
    else 
    { 
        yy_push_state(NOOUTPUT, yyscanner); 
        return NOOUTPUT_ELIF_TOK; 
    } 
}
<INITIAL,INSIDE_PAREN_COND>{begindir}"pragma"                                           { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(EXPAND_DIR_COND, yyscanner); return PRAGMA_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"define"                                           { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(DEF_DIR_COND, yyscanner); return DEFINE_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"undef"                                            { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return UNDEF_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"line"                                             { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(EXPAND_DIR_COND, yyscanner); return LINE_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"error"                                            { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return ERROR_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"extension"                                        { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yy_push_state(NOEXPAND_DIR_COND, yyscanner); return EXTENSION_TOK; }
<INITIAL,INSIDE_PAREN_COND>{begindir}"\n"                                               { CHK_PRE_FLEX(yyextra->VerifyWhitespaceOnly(yylloc)); yyextra->ProcessStatement(); yyless(yyleng - 1); }

%{
/* When starting a directive condition, an identifier is read and it cannot be expanded
   because this is how a new identifier is defined. If it is immediately followed by
   a '(' then it is a macro with parameters, otherwise it is not.

   When we find a macro identifier, we go into the no expand condition because
   we want to expand macros when they are encountered, not when defining other macros.

   Anything other than white space or newlines after trying to define a directive is
   cause for an error. 
   
   PushMacroParam and EnsureSymbolIndex both ensure that tokens do not exceed the
   256 maximum length.

   */
%}
<DEF_DIR_COND>{ident}|({ident}"(")                                                      {
    if (yytext[yyleng - 1] == '(')
    {
        yy_push_state(DEF_DIR_PARAM_P_COND, yyscanner);
        yyless(yyleng - 1);

        CHK_PRE_FLEX(yyextra->PushMacroParam(yytext, yyleng, yylloc));
    }
    else
    {
        BEGIN(NOEXPAND_DIR_COND); 
        
        CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); 
        
        return IDENT_TOK;   
    }
}

<DEF_DIR_COND>^[[:space:]]                                                              { GLSLPreerror(yylloc, yyscanner, "Syntax error, expected identifier after define directive"); }

%{
/* We have a condition we push for the paren around the macro arguments. Since we always
   should be hitting the yyless call above when handling an identifier followed by a
   paren, we should never have a problem encountering this paren. 
   
   This condition always pushes the identifier search condition. */
%}
<DEF_DIR_PARAM_P_COND>"("                                                               { yy_push_state(DEF_DIR_PARAM_I_COND, yyscanner); }
<DEF_DIR_PARAM_P_COND>.                                                                 { GLSLPreerror(yylloc, yyscanner, "Internal compiler error"); }

%{
/* While searching for an identifier, anything other than spaces is ignored, and the
   identifier triggers the search for a comma or paren. 

   ProcessTextToken ensures that tokens will fit in the 256 limit.
*/
%}
<DEF_DIR_PARAM_I_COND>{ident}                                                           { BEGIN(DEF_DIR_PARAM_S_COND); CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }
<DEF_DIR_PARAM_I_COND>{linespace}*                                                      ;
<DEF_DIR_PARAM_I_COND>.                                                                 { GLSLPreerror(yylloc, yyscanner, "Syntax error, expected identifier for macro parameter"); }

%{
/* While we are processing parameters to macros, a comma will move to the next parameter. */
%}
<DEF_DIR_PARAM_S_COND>","                                                               { BEGIN(DEF_DIR_PARAM_I_COND); CHK_PRE_FLEX(yyextra->NextMacroParam()); }

%{
/* The closing paren ends the define directive parameter collection. Both the parameter
   conditions are popped off the condition stack, and the parameters are set on the
   macro. A token is returned for the parser to use with further processing. */
%}
<DEF_DIR_PARAM_S_COND>")"                                                               { 
    yy_pop_state(yyscanner);
    yy_pop_state(yyscanner);
    
    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    TSmartPointer<CGLSLPreParamList> spParamList;
    CHK_PRE_FLEX(yyextra->PopMacroParam(&spDefinition, &yylval->defIndex, &spParamList));

    spDefinition->SetParameters(spParamList);

    BEGIN(NOEXPAND_DIR_COND);

    return MACRO_DEFINITION;
}

%{
/* Anything else we see other than spaces are an error. */
%}
<DEF_DIR_PARAM_S_COND>{linespace}*                                                      ;
<DEF_DIR_PARAM_S_COND>.                                                                 { GLSLPreerror(yylloc, yyscanner, "Syntax error, expected ',' or ')'"); }

%{
/* The if directive contains many operators that are defined here. They return tokens
   and the parser for the preprocessor will verify that they are only used in the right
   place. */
%}
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"("                                                  { return LEFT_PAREN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>")"                                                  { return RIGHT_PAREN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"<"                                                  { return LEFT_ANGLE; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>">"                                                  { return RIGHT_ANGLE; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"<="                                                 { return LE_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>">="                                                 { return GE_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"&&"                                                 { return AND_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"||"                                                 { return OR_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"+"                                                  { return PLUS; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"-"                                                  { return DASH; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"!"                                                  { return BANG; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"~"                                                  { return TILDA; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"&"                                                  { return AMPERSAND; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"^"                                                  { return CARET; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"|"                                                  { return VERTICAL_BAR; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"=="                                                 { return EQ_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"!="                                                 { return NE_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"<<"                                                 { return LEFT_SHIFT_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>">>"                                                 { return RIGHT_SHIFT_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"*"                                                  { return STAR; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"/"                                                  { return SLASH; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"%"                                                  { return PERCENT; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>":"                                                  { return COLON; }
%{
/*  The tokens below are added for completeness with GLSL. If we don't
    have these listed here, they will tokenize as multiple tokens, since each
    character by itself is specified above. Multiple tokens get output 
    with a space between them, leading to GLSL parsing errors */
%}
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>">>="                                                { return RIGHT_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"<<="                                                { return LEFT_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"*="                                                 { return MUL_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"-="                                                 { return SUB_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"+="                                                 { return ADD_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"|="                                                 { return OR_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"%="                                                 { return MOD_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"^="                                                 { return XOR_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"/="                                                 { return DIV_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"&="                                                 { return AND_ASSIGN; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"^^"                                                 { return XOR_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"++"                                                 { return INC_OP; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND>"--"                                                 { return DEC_OP; }

%{
/* The defined operator temporarily puts things into a no-expand state (because it needs
   the unexpanded identifier). */
%}
<EXPAND_DIR_COND>"defined"                                                              { BEGIN(NOEXPAND_DIR_COND); return DEFINED_TOK; }

%{
/* Identifier handling when not expanding is very straightforward - the identifier
   is ensured in the symbol table and a token is returned. Since non-expanded identifiers
   are always only needed by themselves (and never in sequence) the expand directive
   condition is set here as well. */
%}
<NOEXPAND_DIR_COND>{ident}                                                              { BEGIN(EXPAND_DIR_COND); CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return IDENT_TOK; }

%{
/* Identifier handling while in "regular" text processing can mean expanding the
   macro if the identifier is a macro identifier. In this case, we either go into
   a state of looking for the parameter list or we just expand the buffer out
   and keep processing.

   Identifiers that do not match existing macros are just passed as text to the
   next stage. 

   EnsureSymbolIndex and ProcessTextToken check the 256 char limit.
*/
%}
<INITIAL,INSIDE_PAREN_COND,EXPAND_DIR_COND,X_PARAM_COND>{ident}                         {
    TSmartPointer<CGLSLPreMacroDefinition> spDefinition;
    if(SUCCEEDED(yyextra->GetMacroDefinition(yytext, &spDefinition))) 
    {
        if (spDefinition->HasParameters())
        {
            yy_push_state(P_X_PARAM_COND, yyscanner);

            CHK_PRE_FLEX(yyextra->PushMacroParam(spDefinition));
        }
        else
        {
            CHK_PRE_FLEX(yyextra->PushBufferState(yylloc, spDefinition, YY_CURRENT_BUFFER));
        }
    } 
    else 
    {
        if (YY_START == EXPAND_DIR_COND)
        {
            CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex));
            return IDENT_TOK; 
        }
        else
        {
            CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc));
        }
    }
}

%{
/* Once we start looking for arguments, we need to see an opening to an argument
   list or we have hit an error. */
%}
<P_X_PARAM_COND>{space_opt}"("                                                          { yy_push_state(X_PARAM_COND, yyscanner); }
<P_X_PARAM_COND>.                                                                       { GLSLPreerror(yylloc, yyscanner, "Expected macro argument list"); }

%{
/* Commas delimit arguments passed to macros. */
%}
<X_PARAM_COND>","                                                                       { CHK_PRE_FLEX(yyextra->NextMacroParam()); }

%{
/* Once we have collected all of the arguments, we pop off the states and get the
   parser to coordinate expanding the macro and pushing the new buffer. */
%}
<X_PARAM_COND>")"                                                                       { 
    yy_pop_state(yyscanner);
    yy_pop_state(yyscanner);
    
    CHK_PRE_FLEX(yyextra->PushBufferState(yylloc, nullptr, YY_CURRENT_BUFFER));
}

%{
/* EOF means that we have finished a buffer - we need to pop it off and keep going
   in the original one we were in. If we run out of buffers to pop then we are done. 
   
   Some states are not allowed to be terminated in. These cause an error. */
%}
<*><<EOF>>                                                                              { 
    if (FAILED(yyextra->PopBufferState()))
    {
        if (YY_START == P_X_PARAM_COND)
        {
            GLSLPreerror(yylloc, yyscanner, "Expected macro argument list");
        }

        yyterminate();
    }
}

%{
/* While in preprocessor conditions, make tokens for the parser when we encounter
   floats, ints and everything else other than whitespace and '#'. */
%}
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>0                                       { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return DEC_CONST_TOK; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>0{oseq_opt}                             { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return OCT_CONST_TOK; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>{glsl_hex}                              { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return HEX_CONST_TOK; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>[1-9]{dseq_opt}                         { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return DEC_CONST_TOK; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>{glsl_float}                            { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return FLOAT_CONST_TOK; }
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>{linespace}*                            ;
<EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND>[^#\n]                                  { CHK_PRE_FLEX(yyextra->EnsureSymbolIndex(yytext, yyleng, yylloc, &yylval->iSymbolIndex)); return TEXT_TOK; }

%{
/* When we encounter paren, we push a condition for it on the condition stack
   so that the code we are preprocessing does not have commas interpreted in
   the preprocessor. This allows functions to be called inside of macros. 
   
   Apart from the condition change, the paren is still processed as text. */
%}
<INITIAL,X_PARAM_COND,INSIDE_PAREN_COND>"("                                             { yy_push_state(INSIDE_PAREN_COND, yyscanner); CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }

%{
/* Once the paren close, back to the last condition. */
%}
<INSIDE_PAREN_COND>")"                                                                  { yy_pop_state(yyscanner); CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }

%{
/* While not in preprocessor conditions, numbers and other things are just text */
%}
<INITIAL,X_PARAM_COND,INSIDE_PAREN_COND>{glsl_float}                                    { CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }
<INITIAL,X_PARAM_COND,INSIDE_PAREN_COND>{dseq}                                          { CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }
<INITIAL,X_PARAM_COND,INSIDE_PAREN_COND>[^#\n]                                          { CHK_PRE_FLEX(yyextra->ProcessTextToken(yytext, yyleng, yylloc)); }

%{
/* CPP comments push a state that is reset by newline and eat everything. */
%}
<INITIAL,NOOUTPUT,INSIDE_PAREN_COND,EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND,DEF_DIR_PARAM_I_COND,DEF_DIR_PARAM_S_COND,X_PARAM_COND,P_X_PARAM_COND>"//"    { 
    yy_push_state(CPP_COMMENT_COND, yyscanner); 
}
<CPP_COMMENT_COND>[^\n]+                                                                ;

%{
/* C comments push a state that is reset by a particular end condition. Spaces
   replace text (so that column positions are not affected in source) and newlines
   are still pushed through. */
%}
<INITIAL,NOOUTPUT,INSIDE_PAREN_COND,EXPAND_DIR_COND,NOEXPAND_DIR_COND,DEF_DIR_COND,DEF_DIR_PARAM_I_COND,DEF_DIR_PARAM_S_COND,X_PARAM_COND,P_X_PARAM_COND>"/*"    { 
    yyextra->SetCommentCondition(YY_START); yy_push_state(C_COMMENT_COND, yyscanner); CHK_PRE_FLEX(yyextra->PreProcessCommentText(yyleng)); 
}
<C_COMMENT_COND>[^*\n]*                                                                 { CHK_PRE_FLEX(yyextra->PreProcessCommentText(yyleng)); }
<C_COMMENT_COND>"*"+[^*/\n]*                                                            { CHK_PRE_FLEX(yyextra->PreProcessCommentText(yyleng)); }
<C_COMMENT_COND>"*"+"/"                                                                 { yy_pop_state(yyscanner); BEGIN(yyextra->GetCommentCondition()); CHK_PRE_FLEX(yyextra->PreProcessCommentText(yyleng)); }

%{
/* While output is disabled, we basically eat tokens until the newline turns up.  This
   is last in the file because we want comments to take priority. */
%}
<NOOUTPUT>[^\n]+                                                                        ; 
    
%{
/* We should be processing all valid characters and invalid characters
   up to this point. So ending up here means that our lexer is ill formed. */
%}
.                                                                                       { GLSLPreerror(yylloc, yyscanner, "Internal compiler error"); }

%%
