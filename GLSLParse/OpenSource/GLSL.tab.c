/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* "%code top" blocks.  */


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


/* Substitute the variable and function names.  */
#define yyparse         GLSLparse
#define yylex           GLSLlex
#define yyerror         GLSLerror
#define yylval          GLSLlval
#define yychar          GLSLchar
#define yydebug         GLSLdebug
#define yynerrs         GLSLnerrs
#define yylloc          GLSLlloc

/* Copy the first part of user declarations.  */





#include <stdio.h>
#include <stdlib.h>
#define _STDLIB_H                   /* Bison output needs this defined or it thinks stdlib was not included */

#include "ParseTreeNode.hxx"        /* Tree node is defined here */
#include "GLSL.tab.h"               /* We need this to include the lexer header */
#include "lex.GLSL.h"               /* We need this for the scanner type definitions */
#include "GLSLParserGlobals.hxx"    /* This is where we define yyerror, yywrap etc */
#include "GLSLParser.hxx"           /* We call methods on the parser from here */
#include "ParseTree.hxx"            /* This includes all of the various kinds of tree nodes */
#include "GLSLMacro.hxx"            /* For CHK_YY et al */

#pragma warning(disable:4242 4244 4127 4702 4701 4065)




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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


/* Copy the second part of user declarations.  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  50
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1458

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  99
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  76
/* YYNRULES -- Number of rules.  */
#define YYNRULES  192
/* YYNRULES -- Number of states.  */
#define YYNSTATES  284

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   353

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    18,
      22,    24,    29,    31,    35,    38,    41,    43,    45,    48,
      51,    54,    56,    59,    63,    66,    68,    70,    72,    74,
      77,    80,    83,    85,    87,    89,    91,    93,    97,   101,
     103,   107,   111,   113,   115,   119,   123,   127,   131,   133,
     137,   141,   143,   145,   147,   151,   153,   157,   159,   163,
     165,   171,   173,   177,   179,   181,   183,   185,   187,   189,
     191,   193,   195,   197,   199,   201,   205,   207,   210,   213,
     218,   221,   223,   225,   228,   232,   236,   239,   245,   249,
     252,   256,   259,   260,   262,   264,   266,   268,   273,   275,
     279,   286,   292,   294,   297,   300,   306,   311,   313,   316,
     318,   320,   322,   324,   326,   329,   331,   333,   335,   337,
     339,   341,   343,   345,   347,   349,   351,   353,   355,   357,
     359,   361,   363,   365,   367,   369,   371,   376,   382,   384,
     387,   391,   393,   397,   399,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   422,   424,   426,   428,   431,   435,
     437,   439,   442,   446,   448,   451,   453,   455,   458,   464,
     468,   470,   472,   479,   481,   483,   486,   490,   493,   495,
     498,   501,   505,   508,   511,   513,   515,   518,   520,   522,
     524,   526,   528
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     100,     0,    -1,   168,    -1,   100,   168,    -1,   172,    -1,
     101,    -1,    97,    -1,    98,    -1,   171,    -1,    62,   127,
      61,    -1,   102,    -1,   103,    65,   104,    66,    -1,   105,
      -1,   103,    82,   170,    -1,   103,    76,    -1,   103,    77,
      -1,   127,    -1,   106,    -1,   108,    61,    -1,   107,    61,
      -1,   109,    19,    -1,   109,    -1,   109,   125,    -1,   108,
      71,   125,    -1,   110,    62,    -1,   111,    -1,    96,    -1,
     144,    -1,   103,    -1,   113,   112,    -1,    76,   112,    -1,
      77,   112,    -1,    78,    -1,    79,    -1,    83,    -1,    84,
      -1,   112,    -1,   114,    72,   112,    -1,   114,    73,   112,
      -1,   114,    -1,   115,    78,   114,    -1,   115,    79,   114,
      -1,   115,    -1,   116,    -1,   117,    67,   116,    -1,   117,
      68,   116,    -1,   117,    69,   116,    -1,   117,    70,   116,
      -1,   117,    -1,   118,    80,   117,    -1,   118,    81,   117,
      -1,   118,    -1,   119,    -1,   120,    -1,   121,    57,   120,
      -1,   121,    -1,   122,    58,   121,    -1,   122,    -1,   123,
      59,   122,    -1,   123,    -1,   123,    74,   127,    75,   125,
      -1,   124,    -1,   112,   126,   125,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    53,
      -1,    52,    -1,    54,    -1,    56,    -1,    55,    -1,   125,
      -1,   127,    71,   125,    -1,   124,    -1,   130,    60,    -1,
     138,    60,    -1,    44,   150,   143,    60,    -1,   131,    61,
      -1,   133,    -1,   132,    -1,   133,   135,    -1,   132,    71,
     135,    -1,   140,   173,    62,    -1,   142,   172,    -1,   142,
     172,    65,   128,    66,    -1,   141,   136,   134,    -1,   136,
     134,    -1,   141,   136,   137,    -1,   136,   137,    -1,    -1,
      37,    -1,    38,    -1,    39,    -1,   142,    -1,   142,    65,
     128,    66,    -1,   139,    -1,   138,    71,   172,    -1,   138,
      71,   172,    65,   128,    66,    -1,   138,    71,   172,    46,
     151,    -1,   145,    -1,   141,   145,    -1,   140,   172,    -1,
     140,   172,    65,   128,    66,    -1,   140,   172,    46,   151,
      -1,   142,    -1,   141,   142,    -1,    35,    -1,    31,    -1,
      32,    -1,    33,    -1,   143,    -1,   150,   143,    -1,   144,
      -1,   174,    -1,   145,    -1,     4,    -1,     5,    -1,     6,
      -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,    16,
      -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,    21,
      -1,    94,    63,   146,    64,    -1,    94,   174,    63,   146,
      64,    -1,   147,    -1,   146,   147,    -1,   142,   148,    60,
      -1,   149,    -1,   148,    71,   149,    -1,   172,    -1,   172,
      65,   128,    66,    -1,    43,    -1,    42,    -1,    41,    -1,
     125,    -1,   129,    -1,   155,    -1,   154,    -1,   152,    -1,
     160,    -1,   161,    -1,   164,    -1,   167,    -1,    63,    64,
      -1,    63,   158,    64,    -1,   157,    -1,   154,    -1,    63,
      64,    -1,    63,   158,    64,    -1,   153,    -1,   158,   153,
      -1,   153,    -1,    60,    -1,   127,    60,    -1,    85,    62,
     127,    61,   162,    -1,   156,    86,   156,    -1,   156,    -1,
     127,    -1,    88,    62,   165,   166,    61,   159,    -1,   160,
      -1,   152,    -1,   163,    60,    -1,   163,    60,   127,    -1,
      60,   127,    -1,    60,    -1,    89,    60,    -1,    87,    60,
      -1,    87,   127,    60,    -1,    90,    60,    -1,    91,    60,
      -1,   169,    -1,   129,    -1,   130,   157,    -1,   172,    -1,
      92,    -1,    93,    -1,    96,    -1,    96,    -1,    96,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   294,   294,   295,   299,   303,   304,   305,   306,   307,
     311,   312,   314,   315,   316,   317,   321,   324,   328,   329,
     333,   334,   338,   339,   344,   356,   357,   367,   371,   372,
     373,   374,   378,   379,   380,   381,   385,   386,   387,   391,
     392,   393,   397,   401,   402,   403,   404,   405,   409,   410,
     411,   415,   419,   423,   424,   429,   430,   435,   436,   440,
     441,   446,   447,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   462,   466,   467,   471,   475,   476,   477,
     482,   486,   487,   491,   492,   497,   502,   503,   508,   510,
     511,   513,   517,   518,   519,   520,   524,   525,   530,   531,
     532,   534,   546,   547,   548,   549,   551,   556,   557,   561,
     562,   563,   564,   568,   569,   573,   574,   575,   579,   580,
     581,   582,   583,   584,   585,   586,   587,   588,   589,   590,
     591,   592,   593,   594,   595,   596,   600,   602,   607,   608,
     612,   616,   617,   621,   622,   627,   628,   629,   633,   637,
     641,   642,   646,   647,   648,   649,   650,   654,   655,   659,
     660,   664,   665,   669,   670,   674,   678,   679,   683,   688,
     689,   693,   697,   702,   703,   707,   708,   709,   710,   714,
     715,   716,   717,   718,   722,   723,   727,   733,   736,   737,
     741,   745,   749
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NO_TYPE", "FLOAT_TOK", "VEC2", "VEC3",
  "VEC4", "INT_TOK", "IVEC2_TOK", "IVEC3_TOK", "IVEC4_TOK", "BOOL_TOK",
  "BVEC2_TOK", "BVEC3_TOK", "BVEC4_TOK", "MAT2_TOK", "MAT3_TOK",
  "MAT4_TOK", "VOID_TOK", "SAMPLER2D", "SAMPLERCUBE", "GENTYPE", "VECTYPE",
  "IVECTYPE", "BVECTYPE", "SCALAR_NUMERIC_TYPE", "NUMERIC_TYPE",
  "SIMPLE_TYPE", "LEFT_EXPR_TYPE", "NO_QUALIFIER", "ATTRIBUTE", "UNIFORM",
  "VARYING", "HLSL_UNIFORM", "CONST_TOK", "EMPTY_TOK", "IN_TOK", "OUT_TOK",
  "INOUT_TOK", "NO_PRECISION", "LOW_PRECISION", "MEDIUM_PRECISION",
  "HIGH_PRECISION", "PRECISION", "INVALID_OP", "EQUAL", "MUL_ASSIGN",
  "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN", "SUB_ASSIGN", "RIGHT_ASSIGN",
  "LEFT_ASSIGN", "AND_ASSIGN", "OR_ASSIGN", "XOR_ASSIGN", "AND_OP",
  "XOR_OP", "OR_OP", "SEMICOLON", "RIGHT_PAREN", "LEFT_PAREN",
  "LEFT_BRACE", "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET",
  "LEFT_ANGLE", "RIGHT_ANGLE", "LE_OP", "GE_OP", "COMMA", "STAR", "SLASH",
  "QUESTION", "COLON", "INC_OP", "DEC_OP", "PLUS", "DASH", "EQ_OP",
  "NE_OP", "DOT", "BANG", "TILDE", "IF_TOK", "ELSE_TOK", "RETURN_TOK",
  "FOR_TOK", "BREAK_TOK", "DISCARD_TOK", "CONTINUE_TOK", "TRUE_TOK",
  "FALSE_TOK", "STRUCT_TOK", "UNSUPPORTED_TOKEN", "IDENTIFIER",
  "INTCONSTANT", "DOUBLECONSTANT", "$accept", "translation_unit",
  "variable_identifier", "primary_expression", "postfix_expression",
  "integer_expression", "function_call", "function_call_generic",
  "function_call_header_no_parameters",
  "function_call_header_with_parameters", "function_call_header",
  "function_identifier", "constructor_identifier", "unary_expression",
  "unary_operator", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "inclusive_or_expression", "logical_and_expression",
  "logical_xor_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration", "function_prototype",
  "function_declarator", "function_header_with_parameters",
  "function_header", "parameter_declarator", "parameter_declaration",
  "parameter_qualifier", "parameter_type_specifier",
  "init_declarator_list", "single_declaration", "fully_specified_type",
  "type_qualifier", "type_specifier", "type_specifier_no_prec",
  "basic_type", "struct_specifier", "struct_declaration_list",
  "struct_declaration", "struct_declarator_list", "struct_declarator",
  "precision_qualifier", "initializer", "declaration_statement",
  "statement_no_new_scope", "simple_statement",
  "compound_statement_with_scope", "statement_with_scope",
  "compound_statement_no_new_scope", "statement_list",
  "iteration_statement_no_new_scope", "expression_statement",
  "selection_statement", "selection_rest_statement", "condition",
  "iteration_statement", "for_init_statement", "for_rest_statement",
  "jump_statement", "external_declaration", "function_definition",
  "field_selection", "bool_constant", "variable_identifier_node",
  "function_identifier_node", "typename_identifier_node", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    99,   100,   100,   101,   102,   102,   102,   102,   102,
     103,   103,   103,   103,   103,   103,   104,   105,   106,   106,
     107,   107,   108,   108,   109,   110,   110,   111,   112,   112,
     112,   112,   113,   113,   113,   113,   114,   114,   114,   115,
     115,   115,   116,   117,   117,   117,   117,   117,   118,   118,
     118,   119,   120,   121,   121,   122,   122,   123,   123,   124,
     124,   125,   125,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   127,   127,   128,   129,   129,   129,
     130,   131,   131,   132,   132,   133,   134,   134,   135,   135,
     135,   135,   136,   136,   136,   136,   137,   137,   138,   138,
     138,   138,   139,   139,   139,   139,   139,   140,   140,   141,
     141,   141,   141,   142,   142,   143,   143,   143,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   145,   145,   146,   146,
     147,   148,   148,   149,   149,   150,   150,   150,   151,   152,
     153,   153,   154,   154,   154,   154,   154,   155,   155,   156,
     156,   157,   157,   158,   158,   159,   160,   160,   161,   162,
     162,   163,   164,   165,   165,   166,   166,   166,   166,   167,
     167,   167,   167,   167,   168,   168,   169,   170,   171,   171,
     172,   173,   174
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     3,
       1,     4,     1,     3,     2,     2,     1,     1,     2,     2,
       2,     1,     2,     3,     2,     1,     1,     1,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     3,     1,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     1,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     2,     4,
       2,     1,     1,     2,     3,     3,     2,     5,     3,     2,
       3,     2,     0,     1,     1,     1,     1,     4,     1,     3,
       6,     5,     1,     2,     2,     5,     4,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     5,     1,     2,
       3,     1,     3,     1,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     1,
       1,     2,     3,     1,     2,     1,     1,     2,     5,     3,
       1,     1,     6,     1,     1,     2,     3,     2,     1,     2,
       2,     3,     2,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   110,
     111,   112,   109,   147,   146,   145,     0,     0,   192,     0,
     185,     0,     0,    82,    92,     0,    98,     0,     0,   107,
     113,   115,   102,     0,     2,   184,   116,     0,     0,     0,
       1,     3,    77,     0,   186,    80,    92,    93,    94,    95,
      83,     0,    92,    78,     0,   190,   104,     0,   108,   103,
     114,   117,     0,     0,     0,   138,     0,   166,     0,     0,
     161,     0,     0,    32,    33,    34,    35,     0,     0,     0,
       0,     0,     0,   188,   189,   190,     6,     7,     5,    10,
      28,    12,    17,     0,     0,    21,     0,    25,    36,     0,
      39,    42,    43,    48,    51,    52,    53,    55,    57,    59,
      61,    74,     0,   149,     0,    27,   152,   163,   151,   150,
       0,   153,   154,   155,   156,     8,     4,    84,    89,    91,
      96,     0,   190,    99,     0,     0,    85,    79,     0,   141,
     143,   136,   139,     0,   190,     0,    27,   157,     0,    30,
      31,     0,   180,     0,     0,   179,   182,   183,     0,    14,
      15,     0,    19,    18,     0,    20,    22,    24,    63,    64,
      65,    66,    67,    68,    70,    69,    71,    73,    72,     0,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   167,     0,   162,   164,     0,
      86,    88,    90,     0,     0,   148,   106,    36,    76,     0,
     140,     0,     0,   137,     9,   158,     0,   181,   174,   173,
       0,     0,    16,    13,   187,    23,    62,    37,    38,    40,
      41,    44,    45,    46,    47,    49,    50,    54,    56,    58,
       0,    75,     0,     0,   101,     0,   105,   142,     0,     0,
     178,   171,     0,     0,    11,     0,    97,     0,   100,   144,
     160,   170,   159,   168,   177,   175,     0,    60,    87,     0,
     176,   165,   172,   169
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    29,    98,    99,   100,   231,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   189,   122,   219,
     123,   124,    32,    33,    34,   138,    60,    61,   139,    35,
      36,    37,    38,    39,    40,   156,    42,    74,    75,   148,
     149,    43,   216,   126,   127,   128,   129,   271,   272,   130,
     282,   131,   132,   273,   262,   133,   230,   263,   134,    44,
      45,   233,   135,   136,    67,    46
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -226
static const yytype_int16 yypact[] =
{
    1192,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,    66,   -47,  -226,   196,
    -226,    34,   -52,   -16,   134,    -1,  -226,   -69,  1344,  -226,
    -226,  -226,   -39,  1362,  -226,  -226,  -226,  1362,  1344,    18,
    -226,  -226,  -226,   289,  -226,  -226,   187,  -226,  -226,  -226,
    -226,  1344,   118,  -226,     9,    -2,   -33,    64,  -226,   -39,
    -226,  -226,    69,     9,  1233,  -226,  1344,  -226,  1074,   384,
    -226,  1074,  1074,  -226,  -226,  -226,  -226,    71,   954,    75,
      82,    83,    92,  -226,  -226,   -54,  -226,  -226,  -226,  -226,
       1,  -226,  -226,    98,    28,  1097,   100,  -226,   195,  1074,
     -10,   -61,  -226,    78,    31,  -226,  -226,   103,   105,   -44,
    -226,  -226,     4,  -226,   108,    74,  -226,  -226,  -226,  -226,
     479,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
     -55,  1344,  -226,   -26,  1074,  1074,  -226,  -226,    20,  -226,
     109,  -226,  -226,  1251,   119,    29,  -226,  -226,   574,  -226,
    -226,  1074,  -226,    27,   859,  -226,  -226,  -226,  1074,  -226,
    -226,     9,  -226,  -226,  1074,   120,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  1074,
    -226,  1074,  1074,  1074,  1074,  1074,  1074,  1074,  1074,  1074,
    1074,  1074,  1074,  1074,  1074,  -226,  1074,  -226,  -226,  1074,
     115,  -226,  -226,  1074,  1074,  -226,  -226,  -226,  -226,   117,
    -226,     9,  1074,  -226,  -226,  -226,    35,  -226,  -226,  -226,
     979,   121,   122,  -226,  -226,  -226,  -226,  -226,  -226,   -10,
     -10,  -226,  -226,  -226,  -226,    78,    78,  -226,   103,   105,
     -50,  -226,   123,  1074,  -226,   124,  -226,  -226,   125,   669,
    1074,   122,   128,   131,  -226,  1074,  -226,   132,  -226,  -226,
    -226,   111,  -226,  -226,   122,  1074,   764,  -226,  -226,   669,
     122,  -226,  -226,  -226
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,   -78,  -226,   -91,  -226,   -19,   -60,  -226,
    -226,    -7,    19,    30,  -226,  -121,   -79,  -226,   -76,  -203,
      22,    23,  -226,  -226,  -226,    89,   167,   170,    93,  -226,
    -226,  -226,   -20,     8,    25,     0,    -3,   159,   -67,  -226,
      15,   226,    40,    90,  -125,  -225,  -226,   -24,   225,   178,
    -226,    94,  -226,  -226,  -226,  -226,  -226,  -226,  -226,   231,
    -226,  -226,  -226,   -36,  -226,   234
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -193
static const yytype_int16 yytable[] =
{
      41,    66,   155,   159,   160,   208,   252,   152,   -26,    55,
     209,   255,   163,   144,    62,   203,    48,   193,   194,   258,
     213,   206,    30,    31,   218,   265,   176,    65,   143,    41,
     204,   190,   145,   208,   270,    69,    62,   150,    41,   214,
      71,   142,  -192,    41,    71,    71,    68,    41,    41,    28,
     267,    30,    31,   125,   270,    56,    73,  -117,    71,    63,
    -191,    41,   191,   192,   205,   215,   168,   217,    70,   140,
      64,    71,    72,    71,    41,   206,    41,   169,   170,   125,
     220,    76,    73,   171,    73,   226,   152,   227,   218,   173,
     224,   221,   232,   218,    52,   235,   259,    53,   206,   174,
     206,   218,   239,   240,   210,   142,   206,    23,    24,    25,
     236,   199,   200,   237,   238,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   146,   251,   250,   147,
     125,   217,   218,   161,   215,   234,   217,   164,    71,   245,
     246,    41,   165,   166,   217,   195,   196,   197,   198,   140,
      71,   281,   167,    41,   261,    57,    58,    59,   125,   172,
     201,    73,   177,   202,   125,    19,    20,    21,    52,    22,
    -115,    57,    58,    59,   222,   217,   241,   242,   243,   244,
     253,   -26,  -133,   256,   274,   150,   277,   264,   275,   266,
     268,   269,   276,   206,   247,   -81,    50,   279,   278,   280,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,   248,    22,   137,    57,    58,    59,    19,    20,    21,
     211,    22,   141,   249,   212,   153,   257,    23,    24,    25,
      26,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,    47,   254,   228,   283,    54,   158,   229,   125,
      51,    49,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   125,     0,     0,   125,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    28,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,     0,     0,     0,     0,     0,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    78,    79,    80,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,    83,    84,     0,
       0,     0,    85,    86,    87,     0,    88,    89,    90,    91,
      92,    93,    94,    27,     0,    95,    96,    97,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
       0,     0,     0,     0,     0,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    77,     0,    78,    79,   157,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,    83,    84,     0,     0,     0,    85,    86,    87,
       0,    88,    89,    90,    91,    92,    93,    94,    27,     0,
      95,    96,    97,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,     0,     0,     0,     0,     0,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    78,    79,   207,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,    83,    84,     0,
       0,     0,    85,    86,    87,     0,    88,    89,    90,    91,
      92,    93,    94,    27,     0,    95,    96,    97,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
       0,     0,     0,     0,     0,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    77,     0,    78,    79,   225,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,    83,    84,     0,     0,     0,    85,    86,    87,
       0,    88,    89,    90,    91,    92,    93,    94,    27,     0,
      95,    96,    97,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,     0,     0,     0,     0,     0,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    78,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,    83,    84,     0,
       0,     0,    85,    86,    87,     0,    88,    89,    90,    91,
      92,    93,    94,    27,     0,    95,    96,    97,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
       0,     0,     0,     0,     0,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    77,     0,    78,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,    83,    84,     0,     0,     0,    85,    86,    87,
       0,    88,    89,    90,    91,    92,    93,    94,    27,     0,
      95,    96,    97,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,     0,     0,     0,     0,     0,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    78,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,    83,    84,     0,
       0,     0,    85,    86,     0,     0,     0,     0,     0,     0,
       0,    93,    94,    27,     0,    95,    96,    97,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,     0,    78,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,    83,    84,     0,     0,     0,    85,    86,   260,
       0,    78,     0,     0,     0,     0,    93,    94,     0,     0,
     154,    96,    97,     0,     0,    81,    82,    83,    84,     0,
       0,     0,    85,    86,     0,     0,     0,     0,     0,     0,
       0,    93,    94,     0,     0,   154,    96,    97,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,   175,    17,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    78,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,    83,    84,     0,     0,     0,    85,    86,    78,
       0,     0,     0,     0,     0,     0,    93,    94,     0,     0,
     154,    96,    97,    81,    82,    83,    84,     0,     0,     0,
      85,    86,     0,     0,     0,     0,     0,     0,     0,    93,
      94,     0,     0,   154,    96,    97,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,    20,    21,     0,    22,     0,     0,
       0,     0,     0,    23,    24,    25,    26,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     0,    23,    24,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    27,     0,    28,     0,
       0,     0,    23,    24,    25,     0,     0,   151,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   223,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     0,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    27,     0,    28
};

static const yytype_int16 yycheck[] =
{
       0,    37,    78,    81,    82,   130,   209,    74,    62,    61,
      65,   214,    88,    46,    34,    59,    63,    78,    79,   222,
      46,    71,     0,     0,   145,    75,   105,    96,    64,    29,
      74,   109,    65,   158,   259,    38,    56,    73,    38,    65,
      43,    96,    96,    43,    47,    48,    38,    47,    48,    96,
     253,    29,    29,    53,   279,    71,    48,    96,    61,    60,
      62,    61,    72,    73,    60,   144,    65,   145,    43,    61,
      71,    74,    47,    76,    74,    71,    76,    76,    77,    79,
      60,    63,    74,    82,    76,   161,   153,    60,   209,    61,
      61,    71,   168,   214,    60,   174,    61,    63,    71,    71,
      71,   222,   193,   194,   140,    96,    71,    41,    42,    43,
     189,    80,    81,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,    62,   206,   204,    60,
     130,   209,   253,    62,   213,   171,   214,    62,   141,   199,
     200,   141,    60,    60,   222,    67,    68,    69,    70,   141,
     153,   276,    60,   153,   230,    37,    38,    39,   158,    61,
      57,   153,    62,    58,   164,    31,    32,    33,    60,    35,
      96,    37,    38,    39,    65,   253,   195,   196,   197,   198,
      65,    62,    62,    66,   260,   221,   265,    66,    60,    66,
      66,    66,    61,    71,   201,    61,     0,    86,    66,   275,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    31,    32,
      33,   202,    35,    56,    37,    38,    39,    31,    32,    33,
     141,    35,    62,   203,   141,    76,   221,    41,    42,    43,
      44,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    26,   213,   164,   279,    31,    79,   164,   259,
      29,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   276,    -1,    -1,   279,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    96,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79,    -1,
      -1,    -1,    83,    84,    85,    -1,    87,    88,    89,    90,
      91,    92,    93,    94,    -1,    96,    97,    98,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    98,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79,    -1,
      -1,    -1,    83,    84,    85,    -1,    87,    88,    89,    90,
      91,    92,    93,    94,    -1,    96,    97,    98,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    98,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79,    -1,
      -1,    -1,    83,    84,    85,    -1,    87,    88,    89,    90,
      91,    92,    93,    94,    -1,    96,    97,    98,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    98,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79,    -1,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    93,    94,    -1,    96,    97,    98,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    -1,    -1,    -1,    83,    84,    60,
      -1,    62,    -1,    -1,    -1,    -1,    92,    93,    -1,    -1,
      96,    97,    98,    -1,    -1,    76,    77,    78,    79,    -1,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    93,    -1,    -1,    96,    97,    98,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    78,    79,    -1,    -1,    -1,    83,    84,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    92,    93,    -1,    -1,
      96,    97,    98,    76,    77,    78,    79,    -1,    -1,    -1,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    92,
      93,    -1,    -1,    96,    97,    98,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    32,    33,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    44,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    -1,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    96,    -1,
      -1,    -1,    41,    42,    43,    -1,    -1,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    96,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    96,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    41,    42,    43,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    96
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    31,
      32,    33,    35,    41,    42,    43,    44,    94,    96,   100,
     129,   130,   131,   132,   133,   138,   139,   140,   141,   142,
     143,   144,   145,   150,   168,   169,   174,   150,    63,   174,
       0,   168,    60,    63,   157,    61,    71,    37,    38,    39,
     135,   136,   141,    60,    71,    96,   172,   173,   142,   145,
     143,   145,   143,   142,   146,   147,    63,    60,    62,    63,
      64,    76,    77,    78,    79,    83,    84,    85,    87,    88,
      89,    90,    91,    92,    93,    96,    97,    98,   101,   102,
     103,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   127,   129,   130,   144,   152,   153,   154,   155,
     158,   160,   161,   164,   167,   171,   172,   135,   134,   137,
     142,   136,    96,   172,    46,    65,    62,    60,   148,   149,
     172,    64,   147,   146,    96,   127,   144,    64,   158,   112,
     112,    62,    60,   127,    62,    60,    60,    60,    65,    76,
      77,    82,    61,    61,    71,    19,   125,    62,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,   126,
     112,    72,    73,    78,    79,    67,    68,    69,    70,    80,
      81,    57,    58,    59,    74,    60,    71,    64,   153,    65,
     172,   134,   137,    46,    65,   125,   151,   112,   124,   128,
      60,    71,    65,    64,    61,    64,   127,    60,   152,   160,
     165,   104,   127,   170,   172,   125,   125,   112,   112,   114,
     114,   116,   116,   116,   116,   117,   117,   120,   121,   122,
     127,   125,   128,    65,   151,   128,    66,   149,   128,    61,
      60,   127,   163,   166,    66,    75,    66,   128,    66,    66,
     154,   156,   157,   162,   127,    60,    61,   125,    66,    86,
     127,   153,   159,   156
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    yyscan_t scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    yyscan_t scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, yyscan_t scanner)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, scanner)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    yyscan_t scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, scanner); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, yyscan_t scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    yyscan_t scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (yyscan_t scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (yyscan_t scanner)
#else
int
yyparse (scanner)
    yyscan_t scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    { CHK_YY(RefCounted<TranslationUnitCollectionNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spRootNode))); GLSLget_extra(scanner)->SetRootNode((yyval.spRootNode)); ;}
    break;

  case 3:

    { (yyval.spRootNode) = (yyvsp[(1) - (2)].spRootNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (2)].spRootNode), (yyvsp[(2) - (2)].spTreeNode))); ;}
    break;

  case 4:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 5:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 6:

    { CHK_YY(RefCounted<IntConstantNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iIntConstant), (yyval.spTreeNode))); ;}
    break;

  case 7:

    { CHK_YY(RefCounted<FloatConstantNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].doubleConstant), (yyval.spTreeNode))); ;}
    break;

  case 8:

    { CHK_YY(RefCounted<BoolConstantNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].fConstant), (yyval.spTreeNode))); ;}
    break;

  case 9:

    { CHK_YY(RefCounted<ParenExpressionNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 10:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 11:

    { CHK_YY(RefCounted<IndexSelectionNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (4)].spTreeNode), (yylsp[(1) - (4)]), (yyvsp[(3) - (4)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 12:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 13:

    { CHK_YY(RefCounted<FieldSelectionNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yylsp[(3) - (3)]), (yyval.spTreeNode))); ;}
    break;

  case 14:

    { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), INC_OP, UnaryOperatorType::Post, (yyvsp[(1) - (2)].spTreeNode), (yylsp[(1) - (2)]), (yyval.spTreeNode))); ;}
    break;

  case 15:

    { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), DEC_OP, UnaryOperatorType::Post, (yyvsp[(1) - (2)].spTreeNode), (yylsp[(1) - (2)]), (yyval.spTreeNode))); ;}
    break;

  case 16:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 17:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 18:

    { CHK_YY(RefCounted<FunctionCallGenericNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 19:

    { CHK_YY(RefCounted<FunctionCallGenericNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 20:

    { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), nullptr, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 21:

    { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), nullptr, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 22:

    { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 23:

    { (yyval.spTreeNode) = (yyvsp[(1) - (3)].spTreeNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode))); ;}
    break;

  case 24:

    { CHK_YY(RefCounted<FunctionCallHeaderNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 25:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 26:

    { CHK_YY(RefCounted<FunctionCallIdentifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iSymbolIndex), (yylsp[(1) - (1)]), (yyval.spTreeNode))); ;}
    break;

  case 27:

    { CHK_YY(RefCounted<BasicTypeNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iType), (yyval.spTreeNode))); ;}
    break;

  case 28:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 29:

    { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].iType), UnaryOperatorType::Pre, (yyvsp[(2) - (2)].spTreeNode), (yylsp[(2) - (2)]), (yyval.spTreeNode))); ;}
    break;

  case 30:

    { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), INC_OP, UnaryOperatorType::Pre, (yyvsp[(2) - (2)].spTreeNode), (yylsp[(2) - (2)]), (yyval.spTreeNode))); ;}
    break;

  case 31:

    { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), DEC_OP, UnaryOperatorType::Pre, (yyvsp[(2) - (2)].spTreeNode), (yylsp[(2) - (2)]), (yyval.spTreeNode))); ;}
    break;

  case 36:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 37:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), STAR, (yyval.spTreeNode))); ;}
    break;

  case 38:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), SLASH, (yyval.spTreeNode))); ;}
    break;

  case 39:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 40:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), PLUS, (yyval.spTreeNode))); ;}
    break;

  case 41:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), DASH, (yyval.spTreeNode))); ;}
    break;

  case 42:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 43:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 44:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), LEFT_ANGLE, (yyval.spTreeNode))); ;}
    break;

  case 45:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), RIGHT_ANGLE, (yyval.spTreeNode))); ;}
    break;

  case 46:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), LE_OP, (yyval.spTreeNode))); ;}
    break;

  case 47:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), GE_OP, (yyval.spTreeNode))); ;}
    break;

  case 48:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 49:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), EQ_OP, (yyval.spTreeNode))); ;}
    break;

  case 50:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), NE_OP, (yyval.spTreeNode))); ;}
    break;

  case 51:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 52:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 53:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 54:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), AND_OP, (yyval.spTreeNode))); ;}
    break;

  case 55:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 56:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), XOR_OP, (yyval.spTreeNode))); ;}
    break;

  case 57:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 58:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), OR_OP, (yyval.spTreeNode))); ;}
    break;

  case 59:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 60:

    { CHK_YY(RefCounted<ConditionalExpressionNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (5)].spTreeNode), (yyvsp[(3) - (5)].spTreeNode), (yyvsp[(5) - (5)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 61:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 62:

    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyloc), (yyvsp[(2) - (3)].iType), (yyval.spTreeNode))); ;}
    break;

  case 74:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 75:

    { CHK_YY(ExpressionListNode::CreateOrAppend(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), &(yyval.spTreeNode))); ;}
    break;

  case 76:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 77:

    { CHK_YY(RefCounted<FunctionPrototypeDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 78:

    { (yyval.spTreeNode) = (yyvsp[(1) - (2)].spTreeNode); ;}
    break;

  case 79:

    { CHK_YY(RefCounted<PrecisionDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (4)].iType), (yyvsp[(3) - (4)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 80:

    { CHK_YY(RefCounted<FunctionPrototypeNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 81:

    { CHK_YY(RefCounted<FunctionHeaderWithParametersNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 82:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 83:

    { CHK_YY(RefCounted<FunctionHeaderWithParametersNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 84:

    { (yyval.spTreeNode) = (yyvsp[(1) - (3)].spTreeNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode))); ;}
    break;

  case 85:

    { CHK_YY(RefCounted<FunctionHeaderNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(2) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 86:

    { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode), nullptr, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 87:

    { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (5)].spTreeNode), (yyvsp[(2) - (5)].spTreeNode), (yyvsp[(4) - (5)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 88:

    { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].iType), (yyvsp[(3) - (3)].spTreeNode), (yyvsp[(1) - (3)].iType), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 89:

    { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].iType), (yyvsp[(2) - (2)].spTreeNode), NO_QUALIFIER, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 90:

    { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].iType), (yyvsp[(3) - (3)].spTreeNode), (yyvsp[(1) - (3)].iType), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 91:

    { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].iType), (yyvsp[(2) - (2)].spTreeNode), NO_QUALIFIER, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 92:

    { (yyval.iType) = EMPTY_TOK; ;}
    break;

  case 96:

    { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), nullptr, nullptr, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 97:

    { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (4)].spTreeNode), nullptr, (yyvsp[(3) - (4)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 98:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 99:

    { (yyval.spTreeNode) = (yyvsp[(1) - (3)].spTreeNode); CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>((yyvsp[(1) - (3)].spTreeNode), GLSLget_extra(scanner), (yyvsp[(3) - (3)].spTreeNode), nullptr, nullptr)); ;}
    break;

  case 100:

    { (yyval.spTreeNode) = (yyvsp[(1) - (6)].spTreeNode); CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>((yyvsp[(1) - (6)].spTreeNode), GLSLget_extra(scanner), (yyvsp[(3) - (6)].spTreeNode), nullptr, (yyvsp[(5) - (6)].spTreeNode))); ;}
    break;

  case 101:

    { (yyval.spTreeNode) = (yyvsp[(1) - (5)].spTreeNode); CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>((yyvsp[(1) - (5)].spTreeNode), GLSLget_extra(scanner), (yyvsp[(3) - (5)].spTreeNode), (yyvsp[(5) - (5)].spTreeNode), nullptr)); ;}
    break;

  case 102:

    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 103:

    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (2)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 104:

    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode), (yyloc), nullptr, nullptr, (yyval.spTreeNode))); ;}
    break;

  case 105:

    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (5)].spTreeNode), (yyvsp[(2) - (5)].spTreeNode), (yyloc), nullptr, (yyvsp[(4) - (5)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 106:

    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (4)].spTreeNode), (yyvsp[(2) - (4)].spTreeNode), (yyloc), (yyvsp[(4) - (4)].spTreeNode), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 107:

    { CHK_YY(RefCounted<FullySpecifiedTypeNode>::Create(GLSLget_extra(scanner), NO_QUALIFIER, (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 108:

    { CHK_YY(RefCounted<FullySpecifiedTypeNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].iType), (yyvsp[(2) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 113:

    { CHK_YY(RefCounted<TypeSpecifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), NO_PRECISION, (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 114:

    { CHK_YY(RefCounted<TypeSpecifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (2)].spTreeNode), (yyvsp[(1) - (2)].iType), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 115:

    { CHK_YY(RefCounted<BasicTypeNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iType), (yyval.spTreeNode))); ;}
    break;

  case 116:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 117:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 136:

    { CHK_YY(RefCounted<StructSpecifierNode>::Create(GLSLget_extra(scanner), (yylsp[(1) - (4)]), nullptr, (yyvsp[(3) - (4)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 137:

    { CHK_YY(RefCounted<StructSpecifierNode>::Create(GLSLget_extra(scanner), (yylsp[(1) - (5)]), (yyvsp[(2) - (5)].spTreeNode), (yyvsp[(4) - (5)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 138:

    { CHK_YY(RefCounted<StructDeclarationListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 139:

    { (yyval.spTreeNode) = (yyvsp[(1) - (2)].spTreeNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode))); ;}
    break;

  case 140:

    { CHK_YY(RefCounted<StructDeclarationNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(2) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 141:

    { CHK_YY(RefCounted<StructDeclaratorListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 142:

    { (yyval.spTreeNode) = (yyvsp[(1) - (3)].spTreeNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode))); ;}
    break;

  case 143:

    { CHK_YY(RefCounted<StructDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 144:

    { CHK_YY(RefCounted<StructDeclaratorNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (4)].spTreeNode), (yyvsp[(3) - (4)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 148:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 149:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 150:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 151:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 152:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 153:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 154:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 155:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 156:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 157:

    { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), nullptr, true, (yyval.spTreeNode))); ;}
    break;

  case 158:

    { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].spTreeNode), true, (yyval.spTreeNode))); ;}
    break;

  case 159:

    { CHK_YY(RefCounted<ScopeStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 160:

    { CHK_YY(RefCounted<ScopeStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 161:

    { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), nullptr, false, (yyval.spTreeNode))); ;}
    break;

  case 162:

    { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].spTreeNode), false, (yyval.spTreeNode))); ;}
    break;

  case 163:

    { CHK_YY(RefCounted<StatementListNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 164:

    { (yyval.spTreeNode) = (yyvsp[(1) - (2)].spTreeNode); CHK_YY(CollectionNode::AppendChild((yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode))); ;}
    break;

  case 165:

    { CHK_YY(RefCounted<IterationStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 166:

    { CHK_YY(RefCounted<ExpressionStatementNode>::Create(GLSLget_extra(scanner), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 167:

    { CHK_YY(RefCounted<ExpressionStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 168:

    { CHK_YY(RefCounted<SelectionStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(3) - (5)].spTreeNode), (yyvsp[(5) - (5)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 169:

    { CHK_YY(RefCounted<SelectionRestStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 170:

    { CHK_YY(RefCounted<SelectionRestStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].spTreeNode), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 171:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 172:

    { CHK_YY(RefCounted<ForStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(3) - (6)].spTreeNode), (yyvsp[(4) - (6)].spTreeNode), (yyvsp[(6) - (6)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 173:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 174:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 175:

    { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 176:

    { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (3)].spTreeNode), (yyvsp[(3) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 177:

    { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), nullptr, (yyvsp[(2) - (2)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 178:

    { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), nullptr, nullptr, (yyval.spTreeNode))); ;}
    break;

  case 179:

    { CHK_YY(RefCounted<BreakStatementNode>::Create(GLSLget_extra(scanner), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 180:

    { CHK_YY(RefCounted<ReturnStatementNode>::Create(GLSLget_extra(scanner), nullptr, (yyval.spTreeNode))); ;}
    break;

  case 181:

    { CHK_YY(RefCounted<ReturnStatementNode>::Create(GLSLget_extra(scanner), (yyvsp[(2) - (3)].spTreeNode), (yyval.spTreeNode))); ;}
    break;

  case 182:

    { CHK_YY(RefCounted<DiscardStatementNode>::Create(GLSLget_extra(scanner), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 183:

    { CHK_YY(RefCounted<ContinueStatementNode>::Create(GLSLget_extra(scanner), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 184:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 185:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 186:

    { CHK_YY(RefCounted<FunctionDefinitionNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (2)].spTreeNode), (yyvsp[(2) - (2)].spTreeNode), (yyloc), (yyval.spTreeNode))); ;}
    break;

  case 187:

    { (yyval.spTreeNode) = (yyvsp[(1) - (1)].spTreeNode); ;}
    break;

  case 190:

    { CHK_YY(RefCounted<VariableIdentifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iSymbolIndex), (yylsp[(1) - (1)]), (yyval.spTreeNode))); ;}
    break;

  case 191:

    { CHK_YY(RefCounted<FunctionIdentifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iSymbolIndex), (yylsp[(1) - (1)]), (yyval.spTreeNode))); ;}
    break;

  case 192:

    { CHK_YY(RefCounted<TypeNameIdentifierNode>::Create(GLSLget_extra(scanner), (yyvsp[(1) - (1)].iSymbolIndex), (yylsp[(1) - (1)]), (yyval.spTreeNode))); ;}
    break;



      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, scanner, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, scanner, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, scanner);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, scanner);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}






