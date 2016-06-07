/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 1685 of yacc.c  */
#line 19 "pre.y"

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




/* Line 1685 of yacc.c  */
#line 58 "pre.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT_TOK = 258,
     TEXT_TOK = 259,
     DEC_CONST_TOK = 260,
     OCT_CONST_TOK = 261,
     HEX_CONST_TOK = 262,
     FLOAT_CONST_TOK = 263,
     MACRO_DEFINITION = 264,
     NO_SYMBOL = 265,
     VERSION_TOK = 266,
     IFDEF_TOK = 267,
     IFNDEF_TOK = 268,
     IF_TOK = 269,
     ELIF_TOK = 270,
     DEFINED_TOK = 271,
     UNDEF_TOK = 272,
     NOOUTPUT_IF_TOK = 273,
     NOOUTPUT_ELIF_TOK = 274,
     EXTENSION_TOK = 275,
     LEFT_PAREN = 276,
     RIGHT_PAREN = 277,
     ELSE_TOK = 278,
     ENDIF_TOK = 279,
     DEFINE_TOK = 280,
     PRAGMA_TOK = 281,
     LINE_TOK = 282,
     ERROR_TOK = 283,
     NEWLINE_TOK = 284,
     LEFT_ANGLE = 285,
     RIGHT_ANGLE = 286,
     LE_OP = 287,
     GE_OP = 288,
     AND_OP = 289,
     OR_OP = 290,
     PLUS = 291,
     BANG = 292,
     DASH = 293,
     TILDA = 294,
     AMPERSAND = 295,
     CARET = 296,
     VERTICAL_BAR = 297,
     EQ_OP = 298,
     NE_OP = 299,
     LEFT_SHIFT_OP = 300,
     RIGHT_SHIFT_OP = 301,
     STAR = 302,
     SLASH = 303,
     PERCENT = 304,
     COLON = 305,
     RIGHT_ASSIGN = 306,
     LEFT_ASSIGN = 307,
     MUL_ASSIGN = 308,
     SUB_ASSIGN = 309,
     ADD_ASSIGN = 310,
     OR_ASSIGN = 311,
     MOD_ASSIGN = 312,
     XOR_ASSIGN = 313,
     DIV_ASSIGN = 314,
     AND_ASSIGN = 315,
     XOR_OP = 316,
     INC_OP = 317,
     DEC_OP = 318
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 54 "pre.y"

    int iSymbolIndex;                   /* Index of symbol in symbol table */
    int exprValue;                      /* Expression value */
    const char* pszTokenText;           /* Text of token */
    int defIndex;                       /* Definition index */



/* Line 1685 of yacc.c  */
#line 147 "pre.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



