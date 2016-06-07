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

class CGLSLParser;

#include "ParseTreeNode.hxx"        /* Tree node is defined here */
#include "GLSL.tab.h"               /* Bison output has token definitions */
#include "GLSLParserGlobals.hxx"    /* This is where we define yyerror, yywrap etc */

#pragma warning(disable:4242 4100 4244 4018 4127 4505)

%}

%option reentrant
%option bison-bridge
%option yylineno
%option bison-locations
%option header-file = "lex.GLSL.h"
%option extra-type = "CGLSLParser*"
%option prefix = "GLSL"

oseq_opt                                            [0-7]*
hex-digit                                           [0-9a-fA-F]
glsl_hex                                            0[xX]{hex-digit}+
dseq                                                [[:digit:]]+
dseq_opt                                            [[:digit:]]*
frac                                                ({dseq_opt}"\."{dseq})|{dseq}"\."
exp                                                 [eE][+-]?{dseq}
glsl_float                                          ({frac}{exp}?)|({dseq}{exp})
ident                                               [a-zA-Z_][a-zA-Z0-9_]*

%%

"float"                                             { yylval->iType = FLOAT_TOK; return FLOAT_TOK; }
"vec2"                                              { yylval->iType = VEC2; return VEC2; }
"vec3"                                              { yylval->iType = VEC3; return VEC3; }
"vec4"                                              { yylval->iType = VEC4; return VEC4; }
"int"                                               { yylval->iType = INT_TOK; return INT_TOK; }
"ivec2"                                             { yylval->iType = IVEC2_TOK; return IVEC2_TOK; }
"ivec3"                                             { yylval->iType = IVEC3_TOK; return IVEC3_TOK; }
"ivec4"                                             { yylval->iType = IVEC4_TOK; return IVEC4_TOK; }
"bool"                                              { yylval->iType = BOOL_TOK; return BOOL_TOK; }
"bvec2"                                             { yylval->iType = BVEC2_TOK; return BVEC2_TOK; }
"bvec3"                                             { yylval->iType = BVEC3_TOK; return BVEC3_TOK; }
"bvec4"                                             { yylval->iType = BVEC4_TOK; return BVEC4_TOK; }
"mat2"                                              { yylval->iType = MAT2_TOK; return MAT2_TOK; }
"mat3"                                              { yylval->iType = MAT3_TOK; return MAT3_TOK; }
"mat4"                                              { yylval->iType = MAT4_TOK; return MAT4_TOK; }
"void"                                              { yylval->iType = VOID_TOK; return VOID_TOK; }
"sampler2D"                                         { yylval->iType = SAMPLER2D; return SAMPLER2D; }
"samplerCube"                                       { yylval->iType = SAMPLERCUBE; return SAMPLERCUBE; }
"attribute"                                         { yylval->iType = ATTRIBUTE; return ATTRIBUTE; }
"uniform"                                           { yylval->iType = UNIFORM; return UNIFORM; }
"varying"                                           { yylval->iType = VARYING; return VARYING; }
"const"                                             { yylval->iType = CONST_TOK; return CONST_TOK; }
"in"                                                { yylval->iType = IN_TOK; return IN_TOK; }
"out"                                               { yylval->iType = OUT_TOK; return OUT_TOK; }
"inout"                                             { yylval->iType = INOUT_TOK; return INOUT_TOK; }
"lowp"                                              { yylval->iType = LOW_PRECISION; return LOW_PRECISION; }
"mediump"                                           { yylval->iType = MEDIUM_PRECISION; return MEDIUM_PRECISION; }
"highp"                                             { yylval->iType = HIGH_PRECISION; return HIGH_PRECISION; }
"="                                                 { yylval->iType = EQUAL; return EQUAL; }
">>="                                               { yylval->iType = RIGHT_ASSIGN; return RIGHT_ASSIGN; }
"<<="                                               { yylval->iType = LEFT_ASSIGN; return LEFT_ASSIGN; }
"*="                                                { yylval->iType = MUL_ASSIGN; return MUL_ASSIGN; }
"-="                                                { yylval->iType = SUB_ASSIGN; return SUB_ASSIGN; }
"+="                                                { yylval->iType = ADD_ASSIGN; return ADD_ASSIGN; }
"|="                                                { yylval->iType = OR_ASSIGN ; return OR_ASSIGN ; }
"%="                                                { yylval->iType = MOD_ASSIGN; return MOD_ASSIGN; }
"^="                                                { yylval->iType = XOR_ASSIGN; return XOR_ASSIGN; }
"/="                                                { yylval->iType = DIV_ASSIGN; return DIV_ASSIGN; }
"&="                                                { yylval->iType = AND_ASSIGN; return AND_ASSIGN; }
"precision"                                         { return PRECISION; }
","                                                 { return COMMA; }
"{"                                                 { return LEFT_BRACE; }
"}"                                                 { return RIGHT_BRACE; }
"("                                                 { return LEFT_PAREN; }
")"                                                 { return RIGHT_PAREN; }
"["                                                 { return LEFT_BRACKET; }
"]"                                                 { return RIGHT_BRACKET; }
"<"                                                 { yylval->iType = LEFT_ANGLE; return LEFT_ANGLE; }
">"                                                 { yylval->iType = RIGHT_ANGLE; return RIGHT_ANGLE; }
"<="                                                { yylval->iType = LE_OP; return LE_OP; }
">="                                                { yylval->iType = GE_OP; return GE_OP; }
";"                                                 { return SEMICOLON; }
"*"                                                 { return STAR; }
"/"                                                 { return SLASH; }
"?"                                                 { return QUESTION; }
":"                                                 { return COLON; }
"++"                                                { return INC_OP; }
"--"                                                { return DEC_OP; }
"+"                                                 { yylval->iType = PLUS; return PLUS; }
"-"                                                 { yylval->iType = DASH; return DASH; }
"!"                                                 { yylval->iType = BANG; return BANG; }
"~"                                                 { yylval->iType = TILDE; return TILDE; }
"&&"                                                { yylval->iType = AND_OP; return AND_OP; }
"||"                                                { yylval->iType = OR_OP; return OR_OP; }
"^^"                                                { yylval->iType = XOR_OP; return XOR_OP; }
"=="                                                { return EQ_OP; }
"!="                                                { return NE_OP; }
"\."                                                { return DOT; }
"if"                                                { return IF_TOK; }
"else"                                              { return ELSE_TOK; }
"return"                                            { return RETURN_TOK; }
"break"                                             { return BREAK_TOK; }
"discard"                                           { return DISCARD_TOK; }
"continue"                                          { return CONTINUE_TOK; }
"for"                                               { return FOR_TOK; }
"struct"                                            { return STRUCT_TOK; }

"true"                                              { yylval->fConstant = true; return TRUE_TOK; }
"false"                                             { yylval->fConstant = false; return FALSE_TOK; }

"invariant"                                         { return UNSUPPORTED_TOKEN; }
"do"                                                { return UNSUPPORTED_TOKEN; }
"while"                                             { return UNSUPPORTED_TOKEN; }
"asm"                                               { return UNSUPPORTED_TOKEN; }
"class"                                             { return UNSUPPORTED_TOKEN; }
"union"                                             { return UNSUPPORTED_TOKEN; }
"enum"                                              { return UNSUPPORTED_TOKEN; }
"typedef"                                           { return UNSUPPORTED_TOKEN; }
"template"                                          { return UNSUPPORTED_TOKEN; }
"this"                                              { return UNSUPPORTED_TOKEN; }
"packed"                                            { return UNSUPPORTED_TOKEN; }
"goto"                                              { return UNSUPPORTED_TOKEN; }
"switch"                                            { return UNSUPPORTED_TOKEN; }
"default"                                           { return UNSUPPORTED_TOKEN; }
"inline"                                            { return UNSUPPORTED_TOKEN; }
"noinline"                                          { return UNSUPPORTED_TOKEN; }
"volatile"                                          { return UNSUPPORTED_TOKEN; }
"public"                                            { return UNSUPPORTED_TOKEN; }
"static"                                            { return UNSUPPORTED_TOKEN; }
"extern"                                            { return UNSUPPORTED_TOKEN; }
"external"                                          { return UNSUPPORTED_TOKEN; }
"interface"                                         { return UNSUPPORTED_TOKEN; }
"flat"                                              { return UNSUPPORTED_TOKEN; }
"long"                                              { return UNSUPPORTED_TOKEN; }
"short"                                             { return UNSUPPORTED_TOKEN; }
"double"                                            { return UNSUPPORTED_TOKEN; }
"half"                                              { return UNSUPPORTED_TOKEN; }
"fixed"                                             { return UNSUPPORTED_TOKEN; }
"unsigned"                                          { return UNSUPPORTED_TOKEN; }
"superp"                                            { return UNSUPPORTED_TOKEN; }
"input"                                             { return UNSUPPORTED_TOKEN; }
"output"                                            { return UNSUPPORTED_TOKEN; }
"hvec2"                                             { return UNSUPPORTED_TOKEN; }
"hvec3"                                             { return UNSUPPORTED_TOKEN; }
"hvec4"                                             { return UNSUPPORTED_TOKEN; }
"dvec2"                                             { return UNSUPPORTED_TOKEN; }
"dvec3"                                             { return UNSUPPORTED_TOKEN; }
"dvec4"                                             { return UNSUPPORTED_TOKEN; }
"fvec2"                                             { return UNSUPPORTED_TOKEN; }
"fvec3"                                             { return UNSUPPORTED_TOKEN; }
"fvec4"                                             { return UNSUPPORTED_TOKEN; }
"sampler1D"                                         { return UNSUPPORTED_TOKEN; }
"sampler3D"                                         { return UNSUPPORTED_TOKEN; }
"sampler1DShadow"                                   { return UNSUPPORTED_TOKEN; }
"sampler2DShadow"                                   { return UNSUPPORTED_TOKEN; }
"sampler2DRect"                                     { return UNSUPPORTED_TOKEN; }
"sampler3DRect"                                     { return UNSUPPORTED_TOKEN; }
"sampler2DRectShadow"                               { return UNSUPPORTED_TOKEN; }
"sizeof"                                            { return UNSUPPORTED_TOKEN; }
"cast"                                              { return UNSUPPORTED_TOKEN; }
"namespace"                                         { return UNSUPPORTED_TOKEN; }
"using"                                             { return UNSUPPORTED_TOKEN; }

0{oseq_opt}                                         { yylval->iIntConstant = ::strtol(yytext, nullptr, 8); return INTCONSTANT; }
{glsl_hex}                                          { yylval->iIntConstant = ::strtol(yytext, nullptr, 16); return INTCONSTANT; }
[1-9]{dseq_opt}                                     { yylval->iIntConstant = ::atoi(yytext); return INTCONSTANT; }
{glsl_float}                                        { yylval->doubleConstant = ::atof(yytext); return DOUBLECONSTANT; }

{ident}                                             { if (FAILED(GLSLEnsureSymbolIndex(yyextra, yytext, &yylval->iSymbolIndex))) { GLSLerror(yylloc, yyscanner, "Internal compiler error"); } return IDENTIFIER; }
\n                                                  { GLSLRecordNewline(yyextra); }
[ \t\r]+                                            ;

%{
/* The proprocessor should have removed all invalid characters already */
%}
.                                                   { GLSLerror(yylloc, yyscanner, "Invalid character"); }

%%
