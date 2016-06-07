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

class CGLSLParser;
class TranslationUnitCollectionNode;
class ParseTreeNode;

/* Our ParseTreeNode objects are reference counted objects. In order to manage
   their lifetimes in a sane manner, we must use TSmartPointer. Using an
   RAII class is not possible in a union (e.g. using the %union bison semantic)
   so we declare the struct on our own here in the %code requires section. The
   'if defined' preprocessor code is copied from what bison normally generates
   around the %union in order to prevent multiple definitions of the same type */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
struct YYSTYPE 
{
    int iSymbolIndex;                                           /* Symbol index for identifiers */
    int iType;                                                  /* Type index for identifiers */
    int iIntConstant;                                           /* An integer constant */
    double doubleConstant;                                      /* A double constant */
    bool fConstant;                                             /* A boolean constant */
    TSmartPointer<ParseTreeNode> spTreeNode;                    /* Tree node that is not the root for other expressions */
    TSmartPointer<TranslationUnitCollectionNode> spRootNode;    /* Tree node that is set on the root */
};
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NO_TYPE = 258,
     FLOAT_TOK = 259,
     VEC2 = 260,
     VEC3 = 261,
     VEC4 = 262,
     INT_TOK = 263,
     IVEC2_TOK = 264,
     IVEC3_TOK = 265,
     IVEC4_TOK = 266,
     BOOL_TOK = 267,
     BVEC2_TOK = 268,
     BVEC3_TOK = 269,
     BVEC4_TOK = 270,
     MAT2_TOK = 271,
     MAT3_TOK = 272,
     MAT4_TOK = 273,
     VOID_TOK = 274,
     SAMPLER2D = 275,
     SAMPLERCUBE = 276,
     GENTYPE = 277,
     VECTYPE = 278,
     IVECTYPE = 279,
     BVECTYPE = 280,
     SCALAR_NUMERIC_TYPE = 281,
     NUMERIC_TYPE = 282,
     SIMPLE_TYPE = 283,
     LEFT_EXPR_TYPE = 284,
     NO_QUALIFIER = 285,
     ATTRIBUTE = 286,
     UNIFORM = 287,
     VARYING = 288,
     HLSL_UNIFORM = 289,
     CONST_TOK = 290,
     EMPTY_TOK = 291,
     IN_TOK = 292,
     OUT_TOK = 293,
     INOUT_TOK = 294,
     NO_PRECISION = 295,
     LOW_PRECISION = 296,
     MEDIUM_PRECISION = 297,
     HIGH_PRECISION = 298,
     PRECISION = 299,
     INVALID_OP = 300,
     EQUAL = 301,
     MUL_ASSIGN = 302,
     DIV_ASSIGN = 303,
     MOD_ASSIGN = 304,
     ADD_ASSIGN = 305,
     SUB_ASSIGN = 306,
     RIGHT_ASSIGN = 307,
     LEFT_ASSIGN = 308,
     AND_ASSIGN = 309,
     OR_ASSIGN = 310,
     XOR_ASSIGN = 311,
     AND_OP = 312,
     XOR_OP = 313,
     OR_OP = 314,
     SEMICOLON = 315,
     RIGHT_PAREN = 316,
     LEFT_PAREN = 317,
     LEFT_BRACE = 318,
     RIGHT_BRACE = 319,
     LEFT_BRACKET = 320,
     RIGHT_BRACKET = 321,
     LEFT_ANGLE = 322,
     RIGHT_ANGLE = 323,
     LE_OP = 324,
     GE_OP = 325,
     COMMA = 326,
     STAR = 327,
     SLASH = 328,
     QUESTION = 329,
     COLON = 330,
     INC_OP = 331,
     DEC_OP = 332,
     PLUS = 333,
     DASH = 334,
     EQ_OP = 335,
     NE_OP = 336,
     DOT = 337,
     BANG = 338,
     TILDE = 339,
     IF_TOK = 340,
     ELSE_TOK = 341,
     RETURN_TOK = 342,
     FOR_TOK = 343,
     BREAK_TOK = 344,
     DISCARD_TOK = 345,
     CONTINUE_TOK = 346,
     TRUE_TOK = 347,
     FALSE_TOK = 348,
     STRUCT_TOK = 349,
     UNSUPPORTED_TOKEN = 350,
     IDENTIFIER = 351,
     INTCONSTANT = 352,
     DOUBLECONSTANT = 353
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

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



