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

/* Line 171 of yacc.c  */
#line 4 "pre.y"

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



/* Line 171 of yacc.c  */
#line 86 "pre.tab.c"
/* Substitute the variable and function names.  */
#define yyparse         GLSLPreparse
#define yylex           GLSLPrelex
#define yyerror         GLSLPreerror
#define yylval          GLSLPrelval
#define yychar          GLSLPrechar
#define yydebug         GLSLPredebug
#define yynerrs         GLSLPrenerrs
#define yylloc          GLSLPrelloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 36 "pre.y"


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



/* Line 189 of yacc.c  */
#line 119 "pre.tab.c"

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

/* Line 209 of yacc.c  */
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


/* Line 209 of yacc.c  */
#line 162 "pre.tab.c"

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

/* Line 214 of yacc.c  */
#line 54 "pre.y"

    int iSymbolIndex;                   /* Index of symbol in symbol table */
    int exprValue;                      /* Expression value */
    const char* pszTokenText;           /* Text of token */
    int defIndex;                       /* Definition index */



/* Line 214 of yacc.c  */
#line 251 "pre.tab.c"
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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 276 "pre.tab.c"

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
#define YYFINAL  111
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   577

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  140
/* YYNRULES -- Number of states.  */
#define YYNSTATES  205

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

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
      55,    56,    57,    58,    59,    60,    61,    62,    63
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    84,    86,    88,    90,    92,    94,    96,    98,
     100,   103,   107,   111,   116,   118,   120,   122,   124,   126,
     128,   130,   132,   135,   137,   140,   142,   145,   147,   150,
     152,   154,   156,   158,   161,   165,   169,   172,   177,   180,
     182,   184,   186,   190,   192,   194,   197,   200,   203,   206,
     208,   212,   216,   220,   222,   226,   230,   232,   236,   240,
     242,   246,   250,   254,   258,   260,   264,   268,   270,   274,
     276,   280,   282,   286,   288,   292,   294,   298,   300,   304,
     308,   311,   314,   318,   322,   327,   331,   335,   337,   340,
     343,   346,   350,   353,   357,   361,   366,   369,   373,   375,
     378
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      65,     0,    -1,    66,    -1,    65,    66,    -1,    67,    -1,
      69,    -1,    29,    -1,    16,    -1,    21,    -1,    22,    -1,
      30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,
      35,    -1,    36,    -1,    38,    -1,    37,    -1,    39,    -1,
      40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      50,    -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,
      55,    -1,    56,    -1,    57,    -1,    58,    -1,    59,    -1,
      60,    -1,    61,    -1,    62,    -1,    63,    -1,     8,    -1,
       5,    -1,     6,    -1,     7,    -1,     4,    -1,     3,    -1,
      70,    95,    -1,    70,    73,    95,    -1,    70,    71,    95,
      -1,    70,    71,    73,    95,    -1,    96,    -1,    97,    -1,
     100,    -1,   101,    -1,   102,    -1,   103,    -1,   105,    -1,
      74,    -1,    74,    65,    -1,    72,    -1,    71,    72,    -1,
      77,    -1,    77,    65,    -1,    94,    -1,    94,    65,    -1,
      92,    -1,    93,    -1,    76,    -1,    75,    -1,    18,    29,
      -1,    14,    91,    29,    -1,    15,    91,    29,    -1,    19,
      29,    -1,    16,    21,     3,    22,    -1,    16,     3,    -1,
       5,    -1,     6,    -1,     7,    -1,    21,    91,    22,    -1,
      79,    -1,    78,    -1,    36,    80,    -1,    38,    80,    -1,
      39,    80,    -1,    37,    80,    -1,    80,    -1,    81,    47,
      80,    -1,    81,    48,    80,    -1,    81,    49,    80,    -1,
      81,    -1,    82,    36,    81,    -1,    82,    38,    81,    -1,
      82,    -1,    83,    45,    82,    -1,    83,    46,    82,    -1,
      83,    -1,    84,    30,    83,    -1,    84,    31,    83,    -1,
      84,    32,    83,    -1,    84,    33,    83,    -1,    84,    -1,
      85,    43,    84,    -1,    85,    44,    84,    -1,    85,    -1,
      86,    40,    85,    -1,    86,    -1,    87,    41,    86,    -1,
      87,    -1,    88,    42,    87,    -1,    88,    -1,    89,    34,
      88,    -1,    89,    -1,    90,    35,    89,    -1,    90,    -1,
      12,     3,    29,    -1,    13,     3,    29,    -1,    23,    29,
      -1,    24,    29,    -1,    11,     5,    29,    -1,    25,     3,
      29,    -1,    25,     3,    98,    29,    -1,    25,     9,    29,
      -1,    25,    99,    29,    -1,    68,    -1,    98,    68,    -1,
       9,    68,    -1,    99,    68,    -1,    17,     3,    29,    -1,
      26,    29,    -1,    26,   104,    29,    -1,    27,     5,    29,
      -1,    27,     5,     5,    29,    -1,    28,    29,    -1,    28,
     104,    29,    -1,    68,    -1,   104,    68,    -1,    20,     3,
      50,     3,    29,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   157,   157,   158,   165,   166,   173,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     230,   233,   234,   235,   238,   239,   240,   241,   242,   243,
     244,   248,   249,   253,   254,   258,   259,   263,   264,   268,
     269,   270,   271,   275,   279,   283,   284,   288,   289,   293,
     294,   295,   296,   300,   301,   302,   303,   304,   305,   309,
     310,   311,   312,   316,   317,   318,   322,   323,   324,   328,
     329,   330,   331,   332,   336,   337,   338,   342,   343,   347,
     348,   352,   353,   357,   358,   362,   363,   367,   371,   375,
     379,   383,   387,   391,   392,   393,   394,   398,   399,   403,
     404,   408,   412,   413,   417,   418,   422,   423,   427,   428,
     432
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT_TOK", "TEXT_TOK", "DEC_CONST_TOK",
  "OCT_CONST_TOK", "HEX_CONST_TOK", "FLOAT_CONST_TOK", "MACRO_DEFINITION",
  "NO_SYMBOL", "VERSION_TOK", "IFDEF_TOK", "IFNDEF_TOK", "IF_TOK",
  "ELIF_TOK", "DEFINED_TOK", "UNDEF_TOK", "NOOUTPUT_IF_TOK",
  "NOOUTPUT_ELIF_TOK", "EXTENSION_TOK", "LEFT_PAREN", "RIGHT_PAREN",
  "ELSE_TOK", "ENDIF_TOK", "DEFINE_TOK", "PRAGMA_TOK", "LINE_TOK",
  "ERROR_TOK", "NEWLINE_TOK", "LEFT_ANGLE", "RIGHT_ANGLE", "LE_OP",
  "GE_OP", "AND_OP", "OR_OP", "PLUS", "BANG", "DASH", "TILDA", "AMPERSAND",
  "CARET", "VERTICAL_BAR", "EQ_OP", "NE_OP", "LEFT_SHIFT_OP",
  "RIGHT_SHIFT_OP", "STAR", "SLASH", "PERCENT", "COLON", "RIGHT_ASSIGN",
  "LEFT_ASSIGN", "MUL_ASSIGN", "SUB_ASSIGN", "ADD_ASSIGN", "OR_ASSIGN",
  "MOD_ASSIGN", "XOR_ASSIGN", "DIV_ASSIGN", "AND_ASSIGN", "XOR_OP",
  "INC_OP", "DEC_OP", "$accept", "preprocess_unit_list", "preprocess_unit",
  "nondirective_line", "any_dir_token", "statement", "if_opt_list",
  "elif_opt_list_list", "elif_opt_list", "else_opt_list",
  "if_definition_statement", "disabled_if_statement", "if_statement",
  "elif_statement", "defined_expression", "primary_expression",
  "unary_expression", "multiplicative_expression", "additive_expression",
  "bitwise_shift_expression", "relational_expression",
  "equality_expression", "bitwise_and_expression",
  "bitwise_excl_or_expression", "bitwise_incl_or_expression",
  "logical_and_expression", "logical_or_expression", "expression",
  "ifdef_statement", "ifndef_statement", "else_statement",
  "endif_statement", "version_statement", "define_statement",
  "definition_token_list", "define_with_args", "undef_statement",
  "pragma_statement", "line_statement", "error_statement",
  "directive_token_list", "extension_statement", 0
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
     315,   316,   317,   318
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    64,    65,    65,    66,    66,    67,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    74,
      74,    74,    74,    75,    76,    77,    77,    78,    78,    79,
      79,    79,    79,    80,    80,    80,    80,    80,    80,    81,
      81,    81,    81,    82,    82,    82,    83,    83,    83,    84,
      84,    84,    84,    84,    85,    85,    85,    86,    86,    87,
      87,    88,    88,    89,    89,    90,    90,    91,    92,    93,
      94,    95,    96,    97,    97,    97,    97,    98,    98,    99,
      99,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     105
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     1,     2,     3,     3,     2,     4,     2,     1,
       1,     1,     3,     1,     1,     2,     2,     2,     2,     1,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       2,     2,     3,     3,     4,     3,     3,     1,     2,     2,
       2,     3,     2,     3,     3,     4,     2,     3,     1,     2,
       5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     6,     0,     2,     4,     5,     0,    61,    72,
      71,    69,    70,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,    79,    80,    81,     0,     0,     0,     0,
       0,     0,    84,    83,    89,    93,    96,    99,   104,   107,
     109,   111,   113,   115,   117,     0,     0,    73,     0,     0,
       0,     0,    49,    48,    45,    46,    47,    44,     7,     8,
       9,   132,    10,    11,    12,    13,    14,    15,    16,    18,
      17,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,   138,     0,     0,   136,
       0,     1,     3,     0,     0,     0,     0,     0,    63,     0,
      65,    67,    50,    62,   122,   118,   119,    78,     0,     0,
      85,    88,    86,    87,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    74,   131,     0,   123,   127,     0,   125,   129,
     126,   130,   133,   139,     0,   134,   137,     0,    76,   120,
     121,    64,     0,    52,    51,    66,    68,     0,    82,    90,
      91,    92,    94,    95,    97,    98,   100,   101,   102,   103,
     105,   106,   108,   110,   112,   114,   116,     0,   124,   128,
     135,    75,    53,    77,   140
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    14,    15,   106,    16,    17,   117,   118,   119,
      18,    19,    20,   120,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    21,    22,
     121,   122,    23,    24,   157,    61,    25,    26,    27,    28,
     107,    29
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -108
static const yytype_int16 yypact[] =
{
      49,     6,    16,    24,     8,    36,    28,    50,    19,    85,
      11,   148,  -108,    23,  -108,  -108,  -108,     2,    49,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
      30,    35,    39,  -108,  -108,  -108,    17,     8,     8,     8,
       8,     8,  -108,  -108,  -108,    32,    18,    38,     0,    42,
      47,    61,    66,    75,    78,   120,   121,  -108,   107,   209,
     270,   331,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,  -108,   392,    13,  -108,
     453,  -108,  -108,     8,   129,   130,   131,     2,  -108,   137,
      49,    49,  -108,    49,  -108,  -108,  -108,  -108,   162,   144,
    -108,  -108,  -108,  -108,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,  -108,  -108,   164,  -108,  -108,   514,  -108,  -108,
    -108,  -108,  -108,  -108,   139,  -108,  -108,   142,  -108,  -108,
    -108,  -108,   137,  -108,  -108,    49,    49,   150,  -108,  -108,
    -108,  -108,    32,    32,    18,    18,    38,    38,    38,    38,
       0,     0,    42,    47,    61,    66,    75,   145,  -108,  -108,
    -108,  -108,  -108,  -108,  -108
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -108,   -17,   -13,  -108,   -52,  -108,  -108,  -108,    56,    58,
    -108,  -108,  -108,  -108,  -108,  -108,   -36,   -43,   -28,   -71,
     -49,    29,    70,    71,    69,    72,  -108,   -31,  -108,  -108,
    -108,  -107,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
     210,  -108
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
     112,   123,   130,   131,   132,   133,   129,   156,   159,   161,
     173,    30,   174,    33,    34,    35,   108,   113,   164,    31,
     127,   114,    59,   111,    36,   115,   116,    32,    60,    37,
     141,   142,   143,   144,     1,     2,     3,     4,   128,    56,
       5,     6,   165,     7,    38,    39,    40,    41,     8,     9,
      10,    11,    12,    58,   137,   163,   138,    57,   163,   124,
       1,     2,     3,     4,   125,   202,     5,     6,   126,     7,
     186,   187,   188,   189,     8,     9,    10,    11,    12,   134,
     135,   136,   167,   139,   140,   145,   146,   147,    62,    63,
      64,    65,    66,    67,   182,   183,   190,   191,   179,   180,
     181,    68,   148,   175,   176,   199,    69,    70,   149,   150,
     112,   184,   185,   151,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   152,
     153,    62,    63,    64,    65,    66,    67,   154,   168,   169,
     170,   116,   112,   112,    68,   177,   178,   197,   200,    69,
      70,   201,   203,   171,   204,   172,   192,   109,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    62,    63,    64,    65,    66,    67,   193,   195,
     194,   110,     0,   196,     0,    68,     0,     0,     0,     0,
      69,    70,     0,     0,     0,     0,     0,     0,   155,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    62,    63,    64,    65,    66,    67,     0,
       0,     0,     0,     0,     0,     0,    68,     0,     0,     0,
       0,    69,    70,     0,     0,     0,     0,     0,     0,   158,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    62,    63,    64,    65,    66,    67,
       0,     0,     0,     0,     0,     0,     0,    68,     0,     0,
       0,     0,    69,    70,     0,     0,     0,     0,     0,     0,
     160,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    62,    63,    64,    65,    66,
      67,     0,     0,     0,     0,     0,     0,     0,    68,     0,
       0,     0,     0,    69,    70,     0,     0,     0,     0,     0,
       0,   162,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    62,    63,    64,    65,
      66,    67,     0,     0,     0,     0,     0,     0,     0,    68,
       0,     0,     0,     0,    69,    70,     0,     0,     0,     0,
       0,     0,   166,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,    62,    63,    64,
      65,    66,    67,     0,     0,     0,     0,     0,     0,     0,
      68,     0,     0,     0,     0,    69,    70,     0,     0,     0,
       0,     0,     0,   198,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105
};

static const yytype_int16 yycheck[] =
{
      13,    18,    38,    39,    40,    41,    37,    59,    60,    61,
     117,     5,   119,     5,     6,     7,     5,    15,     5,     3,
       3,    19,     3,     0,    16,    23,    24,     3,     9,    21,
      30,    31,    32,    33,    11,    12,    13,    14,    21,     3,
      17,    18,    29,    20,    36,    37,    38,    39,    25,    26,
      27,    28,    29,     3,    36,   107,    38,    29,   110,    29,
      11,    12,    13,    14,    29,   172,    17,    18,    29,    20,
     141,   142,   143,   144,    25,    26,    27,    28,    29,    47,
      48,    49,   113,    45,    46,    43,    44,    40,     3,     4,
       5,     6,     7,     8,   137,   138,   145,   146,   134,   135,
     136,    16,    41,   120,   121,   157,    21,    22,    42,    34,
     123,   139,   140,    35,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    29,
      29,     3,     4,     5,     6,     7,     8,    50,    29,    29,
      29,    24,   175,   176,    16,     3,    22,     3,    29,    21,
      22,    29,    22,   117,    29,   117,   147,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,     3,     4,     5,     6,     7,     8,   148,   150,
     149,    11,    -1,   151,    -1,    16,    -1,    -1,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,    -1,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,     3,     4,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,
      -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,
      -1,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,     3,     4,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,
      -1,    -1,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    -1,    -1,    -1,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    12,    13,    14,    17,    18,    20,    25,    26,
      27,    28,    29,    65,    66,    67,    69,    70,    74,    75,
      76,    92,    93,    96,    97,   100,   101,   102,   103,   105,
       5,     3,     3,     5,     6,     7,    16,    21,    36,    37,
      38,    39,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,     3,    29,     3,     3,
       9,    99,     3,     4,     5,     6,     7,     8,    16,    21,
      22,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    68,   104,     5,    29,
     104,     0,    66,    15,    19,    23,    24,    71,    72,    73,
      77,    94,    95,    65,    29,    29,    29,     3,    21,    91,
      80,    80,    80,    80,    47,    48,    49,    36,    38,    45,
      46,    30,    31,    32,    33,    43,    44,    40,    41,    42,
      34,    35,    29,    29,    50,    29,    68,    98,    29,    68,
      29,    68,    29,    68,     5,    29,    29,    91,    29,    29,
      29,    72,    73,    95,    95,    65,    65,     3,    22,    80,
      80,    80,    81,    81,    82,    82,    83,    83,    83,    83,
      84,    84,    85,    86,    87,    88,    89,     3,    29,    68,
      29,    29,    95,    22,    29
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
        case 7:

/* Line 1464 of yacc.c  */
#line 178 "pre.y"
    { (yyval.pszTokenText) = "defined"; ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 179 "pre.y"
    { (yyval.pszTokenText) = "("; ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 180 "pre.y"
    { (yyval.pszTokenText) = ")"; ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 181 "pre.y"
    { (yyval.pszTokenText) = "<"; ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 182 "pre.y"
    { (yyval.pszTokenText) = ">"; ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 183 "pre.y"
    { (yyval.pszTokenText) = "<="; ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 184 "pre.y"
    { (yyval.pszTokenText) = ">="; ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 185 "pre.y"
    { (yyval.pszTokenText) = "&&"; ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 186 "pre.y"
    { (yyval.pszTokenText) = "||"; ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 187 "pre.y"
    { (yyval.pszTokenText) = "+"; ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 188 "pre.y"
    { (yyval.pszTokenText) = "-"; ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 189 "pre.y"
    { (yyval.pszTokenText) = "!"; ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 190 "pre.y"
    { (yyval.pszTokenText) = "~"; ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 191 "pre.y"
    { (yyval.pszTokenText) = "&"; ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 192 "pre.y"
    { (yyval.pszTokenText) = "^"; ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 193 "pre.y"
    { (yyval.pszTokenText) = "|"; ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 194 "pre.y"
    { (yyval.pszTokenText) = "=="; ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 195 "pre.y"
    { (yyval.pszTokenText) = "!="; ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 196 "pre.y"
    { (yyval.pszTokenText) = "<<"; ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 197 "pre.y"
    { (yyval.pszTokenText) = ">>"; ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 198 "pre.y"
    { (yyval.pszTokenText) = "*"; ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 199 "pre.y"
    { (yyval.pszTokenText) = "/"; ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 200 "pre.y"
    { (yyval.pszTokenText) = "%"; ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 201 "pre.y"
    { (yyval.pszTokenText) = ":"; ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 202 "pre.y"
    { (yyval.pszTokenText) = ">>="; ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 203 "pre.y"
    { (yyval.pszTokenText) = "<<="; ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 204 "pre.y"
    { (yyval.pszTokenText) = "*="; ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 205 "pre.y"
    { (yyval.pszTokenText) = "-="; ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 206 "pre.y"
    { (yyval.pszTokenText) = "+="; ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 207 "pre.y"
    { (yyval.pszTokenText) = "|="; ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 208 "pre.y"
    { (yyval.pszTokenText) = "%="; ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 209 "pre.y"
    { (yyval.pszTokenText) = "^="; ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 210 "pre.y"
    { (yyval.pszTokenText) = "/="; ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 211 "pre.y"
    { (yyval.pszTokenText) = "&="; ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 212 "pre.y"
    { (yyval.pszTokenText) = "^^"; ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 213 "pre.y"
    { (yyval.pszTokenText) = "++"; ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 214 "pre.y"
    { (yyval.pszTokenText) = "--"; ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 215 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 216 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 217 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 218 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 219 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 220 "pre.y"
    { (yyval.pszTokenText) = GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)); ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 238 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 239 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 240 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 241 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 242 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 243 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 244 "pre.y"
    { GLSLPreget_extra(scanner)->ProcessStatement(); ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 275 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreIf(0)); ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 279 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreIf((yyvsp[(2) - (3)].exprValue) != 0)); ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 283 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreElif((yyvsp[(2) - (3)].exprValue) != 0)); ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 284 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreElif(0)); ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 288 "pre.y"
    { (yyval.exprValue) = GLSLPreget_extra(scanner)->IsDefined((yyvsp[(3) - (4)].iSymbolIndex)) ? 1 : 0; ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 289 "pre.y"
    { (yyval.exprValue) = GLSLPreget_extra(scanner)->IsDefined((yyvsp[(2) - (2)].iSymbolIndex)) ? 1 : 0; ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 293 "pre.y"
    { (yyval.exprValue) = ::atoi(GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex))); ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 294 "pre.y"
    { (yyval.exprValue) = ::strtol(GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)), nullptr, 8); ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 295 "pre.y"
    { (yyval.exprValue) = ::strtol(GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(1) - (1)].iSymbolIndex)), nullptr, 16); ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 296 "pre.y"
    { (yyval.exprValue) = (yyvsp[(2) - (3)].exprValue); ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 300 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 301 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 302 "pre.y"
    { (yyval.exprValue) = +((yyvsp[(2) - (2)].exprValue)); ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 303 "pre.y"
    { (yyval.exprValue) = -((yyvsp[(2) - (2)].exprValue)); ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 304 "pre.y"
    { (yyval.exprValue) = ~((yyvsp[(2) - (2)].exprValue)); ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 305 "pre.y"
    { (yyval.exprValue) = !((yyvsp[(2) - (2)].exprValue)); ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 309 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 310 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) * (yyvsp[(3) - (3)].exprValue); ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 311 "pre.y"
    { if (SUCCEEDED(GLSLPreget_extra(scanner)->CheckNonZero((yyvsp[(3) - (3)].exprValue), (yyloc)))) { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) / (yyvsp[(3) - (3)].exprValue); } else { yyerror(&yylloc, scanner, nullptr); } ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 312 "pre.y"
    { if (SUCCEEDED(GLSLPreget_extra(scanner)->CheckNonZero((yyvsp[(3) - (3)].exprValue), (yyloc)))) { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) % (yyvsp[(3) - (3)].exprValue); } else { yyerror(&yylloc, scanner, nullptr); } ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 316 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 317 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) + (yyvsp[(3) - (3)].exprValue); ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 318 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) - (yyvsp[(3) - (3)].exprValue); ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 322 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 323 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) << (yyvsp[(3) - (3)].exprValue); ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 324 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (3)].exprValue) >> (yyvsp[(3) - (3)].exprValue); ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 328 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 329 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) < (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 330 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) > (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 331 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) <= (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 332 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) >= (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 336 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 337 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) == (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 338 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) != (yyvsp[(3) - (3)].exprValue)) ? 1 : 0; ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 342 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 343 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) & (yyvsp[(3) - (3)].exprValue)); ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 347 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 348 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) ^ (yyvsp[(3) - (3)].exprValue)); ;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 352 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 353 "pre.y"
    { (yyval.exprValue) = ((yyvsp[(1) - (3)].exprValue) | (yyvsp[(3) - (3)].exprValue)); ;}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 357 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 358 "pre.y"
    { (yyval.exprValue) = (((yyvsp[(1) - (3)].exprValue) != 0) && ((yyvsp[(3) - (3)].exprValue) != 0)) ? 1 : 0; ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 362 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 363 "pre.y"
    { (yyval.exprValue) = (((yyvsp[(1) - (3)].exprValue) != 0) || ((yyvsp[(3) - (3)].exprValue) != 0)) ? 1 : 0; ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 367 "pre.y"
    { (yyval.exprValue) = (yyvsp[(1) - (1)].exprValue); ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 371 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreIfDef((yyvsp[(2) - (3)].iSymbolIndex))); ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 375 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreIfNDef((yyvsp[(2) - (3)].iSymbolIndex))); ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 379 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreElse()); ;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 383 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreEndIf()); ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 387 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreVersion((yyvsp[(2) - (3)].iSymbolIndex), (yyloc))); ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 391 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreDefine((yyvsp[(2) - (3)].iSymbolIndex), -1, (yyloc))); ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 392 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreDefine((yyvsp[(2) - (4)].iSymbolIndex), (yyvsp[(3) - (4)].defIndex), (yyloc))); ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 393 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreDefine(-1, (yyvsp[(2) - (3)].defIndex), (yyloc))); ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 394 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreDefine(-1, (yyvsp[(2) - (3)].defIndex), (yyloc))); ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 398 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->CreateDefinition((yyvsp[(1) - (1)].pszTokenText), (yyval.defIndex))); ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 399 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken((yyvsp[(1) - (2)].defIndex), (yyvsp[(2) - (2)].pszTokenText))); (yyval.defIndex) = (yyvsp[(1) - (2)].defIndex); ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 403 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken((yyvsp[(1) - (2)].defIndex), (yyvsp[(2) - (2)].pszTokenText))); (yyval.defIndex) = (yyvsp[(1) - (2)].defIndex); ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 404 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddDefinitionToken((yyvsp[(1) - (2)].defIndex), (yyvsp[(2) - (2)].pszTokenText))); (yyval.defIndex) = (yyvsp[(1) - (2)].defIndex); ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 408 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreUndefine((yyvsp[(2) - (3)].iSymbolIndex), (yyloc))); ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 412 "pre.y"
    { /*no-op*/ ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 413 "pre.y"
    { /*no-op*/ ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 417 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddLineMapping(GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(2) - (3)].iSymbolIndex)))); ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 418 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddLineAndFileMapping(GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(2) - (4)].iSymbolIndex)), GLSLPreget_extra(scanner)->GetSymbolText((yyvsp[(3) - (4)].iSymbolIndex)))); ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 422 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreError(-1, (yyloc))); ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 423 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreError((yyvsp[(2) - (3)].defIndex), (yyloc))); ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 427 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->CreateTokenList((yyvsp[(1) - (1)].pszTokenText), (yyval.defIndex))); ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 428 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->AddTokenToList((yyvsp[(1) - (2)].defIndex), (yyvsp[(2) - (2)].pszTokenText))); (yyval.defIndex) = (yyvsp[(1) - (2)].defIndex); ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 432 "pre.y"
    { CHK_YY(GLSLPreget_extra(scanner)->PreExtension((yyvsp[(2) - (5)].iSymbolIndex), (yyvsp[(4) - (5)].iSymbolIndex), (yyloc))); ;}
    break;



/* Line 1464 of yacc.c  */
#line 2643 "pre.tab.c"
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



/* Line 1684 of yacc.c  */
#line 435 "pre.y"


