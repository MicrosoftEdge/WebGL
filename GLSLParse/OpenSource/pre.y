%require "2.4.2"

%code top
{
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
}

%code requires
{
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

class CGLSLPreParser;                   /* Flex needs this forward defined */

}

%{

#include <stdio.h>
#include <stdlib.h>
#define _STDLIB_H                       /* Bison output needs this defined or it thinks stdlib was not included */

#include "Pre.tab.h"                    /* We need this to include the lexer header */
#include "lex.Pre.h"                    /* We need this for the scanner type definitions */
#include "GLSLPreParserGlobals.hxx"     /* This is where we define yyerror, yywrap etc */
#include "GLSLPreParser.hxx"            /* We call methods on the parser from here */
#include "GLSLPreMacroDefinition.hxx"   /* We call methods on the definition from here */
#include "GLSLMacro.hxx"                /* For CHK_YY et al */

#pragma warning(disable:4242 4244 4127 4702 4701 4065)

%}

%union
{
    int iSymbolIndex;                   /* Index of symbol in symbol table */
    int exprValue;                      /* Expression value */
    const char* pszTokenText;           /* Text of token */
    int defIndex;                       /* Definition index */
};

%token <iSymbolIndex> IDENT_TOK
%token <iSymbolIndex> TEXT_TOK
%token <iSymbolIndex> DEC_CONST_TOK
%token <iSymbolIndex> OCT_CONST_TOK
%token <iSymbolIndex> HEX_CONST_TOK
%token <iSymbolIndex> FLOAT_CONST_TOK
%token <defIndex> MACRO_DEFINITION
%token NO_SYMBOL                        /* Used in places where a symbol is needed but cannot be provided */

%token VERSION_TOK
%token IFDEF_TOK
%token IFNDEF_TOK
%token IF_TOK
%token ELIF_TOK
%token DEFINED_TOK
%token UNDEF_TOK
%token NOOUTPUT_IF_TOK
%token NOOUTPUT_ELIF_TOK
%token EXTENSION_TOK
%token LEFT_PAREN
%token RIGHT_PAREN
%token ELSE_TOK
%token ENDIF_TOK
%token DEFINE_TOK
%token PRAGMA_TOK
%token LINE_TOK
%token ERROR_TOK
%token NEWLINE_TOK
%token LEFT_ANGLE
%token RIGHT_ANGLE
%token LE_OP
%token GE_OP
%token AND_OP
%token OR_OP
%token PLUS
%token BANG
%token DASH
%token TILDA
%token AMPERSAND
%token CARET
%token VERTICAL_BAR
%token EQ_OP
%token NE_OP
%token LEFT_SHIFT_OP
%token RIGHT_SHIFT_OP
%token STAR
%token SLASH
%token PERCENT
%token COLON
%token RIGHT_ASSIGN 
%token LEFT_ASSIGN
%token MUL_ASSIGN 
%token SUB_ASSIGN 
%token ADD_ASSIGN 
%token OR_ASSIGN
%token MOD_ASSIGN 
%token XOR_ASSIGN 
%token DIV_ASSIGN 
%token AND_ASSIGN 
%token XOR_OP 
%token INC_OP 
%token DEC_OP 

%type <exprValue> expression
%type <exprValue> defined_expression
%type <exprValue> relational_expression
%type <exprValue> unary_expression
%type <exprValue> primary_expression
%type <exprValue> logical_and_expression
%type <exprValue> logical_or_expression
%type <exprValue> equality_expression
%type <exprValue> additive_expression
%type <exprValue> multiplicative_expression
%type <exprValue> bitwise_excl_or_expression
%type <exprValue> bitwise_incl_or_expression
%type <exprValue> bitwise_and_expression
%type <exprValue> bitwise_shift_expression
%type <defIndex> directive_token_list
%type <defIndex> definition_token_list
%type <defIndex> define_with_args
%type <pszTokenText> any_dir_token

%error-verbose
%locations

%defines
%name-prefix "GLSLPre"
%define api.pure
%parse-param { yyscan_t scanner }
%lex-param { yyscan_t scanner }

%%

/* The top of the grammar is a list of <preprocess_unit>s */

preprocess_unit_list:
        preprocess_unit
    |   preprocess_unit_list preprocess_unit
    ;

/* A <preprocess_unit> is either a nondirective line or some kind of preprocessor
   statement. */

preprocess_unit:
        nondirective_line
    |   statement
    ;

/* A nondirective line either has a newline without any tokens, or a list of
   non directive tokens followed by a newline. */

nondirective_line:
        NEWLINE_TOK
    ;

/* any_dir_token is any token that you can see while in a directive state */
any_dir_token:
        DEFINED_TOK                                                                 { $$ = "defined"; }
    |   LEFT_PAREN                                                                  { $$ = "("; }
    |   RIGHT_PAREN                                                                 { $$ = ")"; }
    |   LEFT_ANGLE                                                                  { $$ = "<"; }
    |   RIGHT_ANGLE                                                                 { $$ = ">"; }
    |   LE_OP                                                                       { $$ = "<="; }
    |   GE_OP                                                                       { $$ = ">="; }
    |   AND_OP                                                                      { $$ = "&&"; }
    |   OR_OP                                                                       { $$ = "||"; }
    |   PLUS                                                                        { $$ = "+"; }
    |   DASH                                                                        { $$ = "-"; }
    |   BANG                                                                        { $$ = "!"; }
    |   TILDA                                                                       { $$ = "~"; }
    |   AMPERSAND                                                                   { $$ = "&"; }
    |   CARET                                                                       { $$ = "^"; }
    |   VERTICAL_BAR                                                                { $$ = "|"; }
    |   EQ_OP                                                                       { $$ = "=="; }
    |   NE_OP                                                                       { $$ = "!="; }
    |   LEFT_SHIFT_OP                                                               { $$ = "<<"; }
    |   RIGHT_SHIFT_OP                                                              { $$ = ">>"; }
    |   STAR                                                                        { $$ = "*"; }
    |   SLASH                                                                       { $$ = "/"; }
    |   PERCENT                                                                     { $$ = "%"; }
    |   COLON                                                                       { $$ = ":"; }
    |   RIGHT_ASSIGN                                                                { $$ = ">>="; }
    |   LEFT_ASSIGN                                                                 { $$ = "<<="; }
    |   MUL_ASSIGN                                                                  { $$ = "*="; }
    |   SUB_ASSIGN                                                                  { $$ = "-="; }
    |   ADD_ASSIGN                                                                  { $$ = "+="; }
    |   OR_ASSIGN                                                                   { $$ = "|="; }
    |   MOD_ASSIGN                                                                  { $$ = "%="; }
    |   XOR_ASSIGN                                                                  { $$ = "^="; }
    |   DIV_ASSIGN                                                                  { $$ = "/="; }
    |   AND_ASSIGN                                                                  { $$ = "&="; }
    |   XOR_OP                                                                      { $$ = "^^"; }
    |   INC_OP                                                                      { $$ = "++"; }
    |   DEC_OP                                                                      { $$ = "--"; }
    |   FLOAT_CONST_TOK                                                             { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    |   DEC_CONST_TOK                                                               { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    |   OCT_CONST_TOK                                                               { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    |   HEX_CONST_TOK                                                               { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    |   TEXT_TOK                                                                    { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    |   IDENT_TOK                                                                   { $$ = GLSLPreget_extra(scanner)->GetSymbolText($1); }
    ;

/* A statement is a set of preprocessor directives that need to go together
   in some way to make sense. Some of them can optionally have a list of
   <preprocess_unit>s in them, which means that processing recursively
   keeps going inside of them. */

statement:
    /* Selection without elif or else */
        if_opt_list endif_statement

    /* Selection with combinations of elif and else */
    |   if_opt_list else_opt_list endif_statement
    |   if_opt_list elif_opt_list_list endif_statement
    |   if_opt_list elif_opt_list_list else_opt_list endif_statement

    /* Other statements */
    |   version_statement                                                           { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   define_statement                                                            { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   undef_statement                                                             { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   pragma_statement                                                            { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   line_statement                                                              { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   error_statement                                                             { GLSLPreget_extra(scanner)->ProcessStatement(); }
    |   extension_statement                                                         { GLSLPreget_extra(scanner)->ProcessStatement(); }
    ;

if_opt_list:
        if_definition_statement
    |   if_definition_statement preprocess_unit_list
    ;

elif_opt_list_list:
        elif_opt_list
    |   elif_opt_list_list elif_opt_list
    ;

elif_opt_list:
        elif_statement
    |   elif_statement preprocess_unit_list
    ;

else_opt_list:
        else_statement
    |   else_statement preprocess_unit_list
    ;

if_definition_statement:
        ifdef_statement
    |   ifndef_statement
    |   if_statement
    |   disabled_if_statement
    ;

disabled_if_statement:
        NOOUTPUT_IF_TOK NEWLINE_TOK                                                 { CHK_YY(GLSLPreget_extra(scanner)->PreIf(0)); }
    ;

if_statement:
        IF_TOK expression NEWLINE_TOK                                               { CHK_YY(GLSLPreget_extra(scanner)->PreIf($2 != 0)); }
    ;

elif_statement:
        ELIF_TOK expression NEWLINE_TOK                                             { CHK_YY(GLSLPreget_extra(scanner)->PreElif($2 != 0)); }
    |   NOOUTPUT_ELIF_TOK NEWLINE_TOK                                               { CHK_YY(GLSLPreget_extra(scanner)->PreElif(0)); }
    ;

defined_expression:
        DEFINED_TOK LEFT_PAREN IDENT_TOK RIGHT_PAREN                                { $$ = GLSLPreget_extra(scanner)->IsDefined($3) ? 1 : 0; }
    |   DEFINED_TOK IDENT_TOK                                                       { $$ = GLSLPreget_extra(scanner)->IsDefined($2) ? 1 : 0; }
    ;

primary_expression:
        DEC_CONST_TOK                                                               { $$ = ::atoi(GLSLPreget_extra(scanner)->GetSymbolText($1)); }
    |   OCT_CONST_TOK                                                               { $$ = ::strtol(GLSLPreget_extra(scanner)->GetSymbolText($1), nullptr, 8); }
    |   HEX_CONST_TOK                                                               { $$ = ::strtol(GLSLPreget_extra(scanner)->GetSymbolText($1), nullptr, 16); }
    |   LEFT_PAREN expression RIGHT_PAREN                                           { $$ = $2; }
    ;

unary_expression:
        primary_expression                                                          { $$ = $1; }
    |   defined_expression                                                          { $$ = $1; }
    |   PLUS unary_expression                                                       { $$ = +($2); }
    |   DASH unary_expression                                                       { $$ = -($2); }
    |   TILDA unary_expression                                                      { $$ = ~($2); }
    |   BANG unary_expression                                                       { $$ = !($2); }
    ;

multiplicative_expression:
        unary_expression                                                            { $$ = $1; }
    |   multiplicative_expression STAR unary_expression                             { $$ = $1 * $3; }
    |   multiplicative_expression SLASH unary_expression                            { if (SUCCEEDED(GLSLPreget_extra(scanner)->CheckNonZero($3, @$))) { $$ = $1 / $3; } else { yyerror(&yylloc, scanner, nullptr); } }
    |   multiplicative_expression PERCENT unary_expression                          { if (SUCCEEDED(GLSLPreget_extra(scanner)->CheckNonZero($3, @$))) { $$ = $1 % $3; } else { yyerror(&yylloc, scanner, nullptr); } }
    ;

additive_expression:
        multiplicative_expression                                                   { $$ = $1; }
    |   additive_expression PLUS multiplicative_expression                          { $$ = $1 + $3; }
    |   additive_expression DASH multiplicative_expression                          { $$ = $1 - $3; }
    ;

bitwise_shift_expression:
        additive_expression                                                         { $$ = $1; }
    |   bitwise_shift_expression LEFT_SHIFT_OP additive_expression                  { $$ = $1 << $3; }
    |   bitwise_shift_expression RIGHT_SHIFT_OP additive_expression                 { $$ = $1 >> $3; }
    ;

relational_expression:
        bitwise_shift_expression                                                    { $$ = $1; }
    |   relational_expression LEFT_ANGLE bitwise_shift_expression                   { $$ = ($1 < $3) ? 1 : 0; }
    |   relational_expression RIGHT_ANGLE bitwise_shift_expression                  { $$ = ($1 > $3) ? 1 : 0; }
    |   relational_expression LE_OP bitwise_shift_expression                        { $$ = ($1 <= $3) ? 1 : 0; }
    |   relational_expression GE_OP bitwise_shift_expression                        { $$ = ($1 >= $3) ? 1 : 0; }
    ;

equality_expression:
        relational_expression                                                       { $$ = $1; }
    |   equality_expression EQ_OP relational_expression                             { $$ = ($1 == $3) ? 1 : 0; }
    |   equality_expression NE_OP relational_expression                             { $$ = ($1 != $3) ? 1 : 0; }
    ;

bitwise_and_expression:
        equality_expression                                                         { $$ = $1; }
    |   bitwise_and_expression AMPERSAND equality_expression                        { $$ = ($1 & $3); }
    ;

bitwise_excl_or_expression:
        bitwise_and_expression                                                      { $$ = $1; }
    |   bitwise_excl_or_expression CARET bitwise_and_expression                     { $$ = ($1 ^ $3); }
    ;

bitwise_incl_or_expression:
        bitwise_excl_or_expression                                                  { $$ = $1; }
    |   bitwise_incl_or_expression VERTICAL_BAR bitwise_excl_or_expression          { $$ = ($1 | $3); }
    ;

logical_and_expression:
        bitwise_incl_or_expression                                                  { $$ = $1; }
    |   logical_and_expression AND_OP bitwise_incl_or_expression                    { $$ = (($1 != 0) && ($3 != 0)) ? 1 : 0; }
    ;   

logical_or_expression:
        logical_and_expression                                                      { $$ = $1; }
    |   logical_or_expression OR_OP logical_and_expression                          { $$ = (($1 != 0) || ($3 != 0)) ? 1 : 0; }
    ;

expression:
        logical_or_expression                                                       { $$ = $1; }
    ;   

ifdef_statement:
        IFDEF_TOK IDENT_TOK NEWLINE_TOK                                             { CHK_YY(GLSLPreget_extra(scanner)->PreIfDef($2)); }
    ;

ifndef_statement:
        IFNDEF_TOK IDENT_TOK NEWLINE_TOK                                            { CHK_YY(GLSLPreget_extra(scanner)->PreIfNDef($2)); }
    ;

else_statement:
        ELSE_TOK NEWLINE_TOK                                                        { CHK_YY(GLSLPreget_extra(scanner)->PreElse()); }
    ;

endif_statement:
        ENDIF_TOK NEWLINE_TOK                                                       { CHK_YY(GLSLPreget_extra(scanner)->PreEndIf()); }
    ;

version_statement:
        VERSION_TOK DEC_CONST_TOK NEWLINE_TOK                                       { CHK_YY(GLSLPreget_extra(scanner)->PreVersion($2, @$)); }
    ;

define_statement:
        DEFINE_TOK IDENT_TOK NEWLINE_TOK                                            { CHK_YY(GLSLPreget_extra(scanner)->PreDefine($2, -1, @$)); }
    |   DEFINE_TOK IDENT_TOK definition_token_list NEWLINE_TOK                      { CHK_YY(GLSLPreget_extra(scanner)->PreDefine($2, $3, @$)); }
    |   DEFINE_TOK MACRO_DEFINITION NEWLINE_TOK                                     { CHK_YY(GLSLPreget_extra(scanner)->PreDefine(-1, $2, @$)); }
    |   DEFINE_TOK define_with_args NEWLINE_TOK                                     { CHK_YY(GLSLPreget_extra(scanner)->PreDefine(-1, $2, @$)); }
    ;

definition_token_list:
        any_dir_token                                                               { CHK_YY(GLSLPreget_extra(scanner)->CreateDefinition($1, $$)); }
    |   definition_token_list any_dir_token                                         { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken($1, $2)); $$ = $1; }
    ;

define_with_args:
        MACRO_DEFINITION any_dir_token                                              { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken($1, $2)); $$ = $1; }
    |   define_with_args any_dir_token                                              { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken($1, $2)); $$ = $1; }
    ;

undef_statement:
        UNDEF_TOK IDENT_TOK NEWLINE_TOK                                             { CHK_YY(GLSLPreget_extra(scanner)->PreUndefine($2, @$)); }
    ;

pragma_statement:
        PRAGMA_TOK NEWLINE_TOK                                                      { /*no-op*/ }
    |   PRAGMA_TOK directive_token_list NEWLINE_TOK                                 { /*no-op*/ }
    ;

line_statement:
        LINE_TOK DEC_CONST_TOK NEWLINE_TOK                                          { CHK_YY(GLSLPreget_extra(scanner)->AddLineMapping(GLSLPreget_extra(scanner)->GetSymbolText($2))); }
    |   LINE_TOK DEC_CONST_TOK DEC_CONST_TOK NEWLINE_TOK                            { CHK_YY(GLSLPreget_extra(scanner)->AddLineAndFileMapping(GLSLPreget_extra(scanner)->GetSymbolText($2), GLSLPreget_extra(scanner)->GetSymbolText($3))); }
    ;

error_statement:
        ERROR_TOK NEWLINE_TOK                                                       { CHK_YY(GLSLPreget_extra(scanner)->PreError(-1, @$)); }
    |   ERROR_TOK directive_token_list NEWLINE_TOK                                  { CHK_YY(GLSLPreget_extra(scanner)->PreError($2, @$)); }
    ;

directive_token_list:
        any_dir_token                                                               { CHK_YY(GLSLPreget_extra(scanner)->CreateTokenList($1, $$)); }
    |   directive_token_list any_dir_token                                          { CHK_YY(GLSLPreget_extra(scanner)->AddTokenToList($1, $2)); $$ = $1; }
    ;

extension_statement:
        EXTENSION_TOK IDENT_TOK COLON IDENT_TOK NEWLINE_TOK                         { CHK_YY(GLSLPreget_extra(scanner)->PreExtension($2, $4, @$)); }
    ;

%%
