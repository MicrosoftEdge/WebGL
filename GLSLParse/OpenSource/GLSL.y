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
}

%{



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

%}

/* Types */
%token NO_TYPE                  /* Token to represent when there is no type information */
%token <iType> FLOAT_TOK 
%token <iType> VEC2
%token <iType> VEC3
%token <iType> VEC4
%token <iType> INT_TOK 
%token <iType> IVEC2_TOK 
%token <iType> IVEC3_TOK 
%token <iType> IVEC4_TOK 
%token <iType> BOOL_TOK
%token <iType> BVEC2_TOK
%token <iType> BVEC3_TOK
%token <iType> BVEC4_TOK
%token <iType> MAT2_TOK
%token <iType> MAT3_TOK
%token <iType> MAT4_TOK
%token <iType> VOID_TOK
%token <iType> SAMPLER2D
%token <iType> SAMPLERCUBE
%token GENTYPE                  /* Token to represent GLSL gentype */
%token VECTYPE                  /* Token to represent float vector types */
%token IVECTYPE                 /* Token to represent int vector types */
%token BVECTYPE                 /* Token to represent bool vector types */
%token SCALAR_NUMERIC_TYPE      /* Token to represent scalar numeric types */
%token NUMERIC_TYPE             /* Token to represent all numeric types */
%token SIMPLE_TYPE              /* Token to represent all numeric types and boolean */
%token LEFT_EXPR_TYPE           /* Token to represent return type same as LHS */

/* Qualifiers */
%token <iType> NO_QUALIFIER     /* Token to represent when there is no qualifier specified */
%token <iType> ATTRIBUTE
%token <iType> UNIFORM 
%token <iType> VARYING
%token <iType> HLSL_UNIFORM     /* Qualifier for implementation detail uniforms */
%token <iType> CONST_TOK
%token <iType> EMPTY_TOK        /* Token to represent empty where needed in the grammar */
%token <iType> IN_TOK
%token <iType> OUT_TOK
%token <iType> INOUT_TOK

/* Precision */
%token NO_PRECISION             /* Token to represent no precision specified */
%token <iType> LOW_PRECISION
%token <iType> MEDIUM_PRECISION
%token <iType> HIGH_PRECISION
%token <iType> PRECISION

/* Operators */
%token INVALID_OP               /* Token to represent an invalid operator */
%token <iType> EQUAL
%token <iType> MUL_ASSIGN
%token <iType> DIV_ASSIGN
%token <iType> MOD_ASSIGN
%token <iType> ADD_ASSIGN
%token <iType> SUB_ASSIGN
%token <iType> RIGHT_ASSIGN
%token <iType> LEFT_ASSIGN
%token <iType> AND_ASSIGN
%token <iType> OR_ASSIGN 
%token <iType> XOR_ASSIGN
%token <iType> AND_OP
%token <iType> XOR_OP
%token <iType> OR_OP

/* Punctuation */
%token SEMICOLON 
%token RIGHT_PAREN 
%token LEFT_PAREN 
%token LEFT_BRACE 
%token RIGHT_BRACE
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token <iType> LEFT_ANGLE
%token <iType> RIGHT_ANGLE
%token <iType> LE_OP
%token <iType> GE_OP
%token COMMA
%token STAR
%token SLASH
%token QUESTION
%token COLON
%token INC_OP
%token DEC_OP
%token <iType> PLUS
%token <iType> DASH
%token EQ_OP
%token NE_OP
%token DOT
%token <iType> BANG
%token <iType> TILDE

/* Other keywords (flow control etc) */
%token IF_TOK
%token ELSE_TOK
%token RETURN_TOK
%token FOR_TOK
%token BREAK_TOK
%token DISCARD_TOK
%token CONTINUE_TOK

/* Constants */
%token <fConstant> TRUE_TOK
%token <fConstant> FALSE_TOK

/* Structs */
%token STRUCT_TOK

/* Tokens unsupported by the grammar right now, but part of the language */
%token UNSUPPORTED_TOKEN

/* Terminals with values stored in the union */
%token <iSymbolIndex> IDENTIFIER 
%token <iIntConstant> INTCONSTANT
%token <doubleConstant> DOUBLECONSTANT

%type <spRootNode> translation_unit
%type <spTreeNode> type_specifier
%type <spTreeNode> type_specifier_no_prec
%type <spTreeNode> fully_specified_type
%type <spTreeNode> single_declaration
%type <spTreeNode> init_declarator_list
%type <spTreeNode> declaration
%type <spTreeNode> external_declaration
%type <spTreeNode> function_definition
%type <spTreeNode> function_prototype
%type <spTreeNode> compound_statement_no_new_scope
%type <spTreeNode> compound_statement_with_scope
%type <spTreeNode> function_declarator
%type <spTreeNode> function_header
%type <spTreeNode> function_header_with_parameters
%type <spTreeNode> parameter_declaration
%type <spTreeNode> parameter_declarator
%type <spTreeNode> statement_list
%type <spTreeNode> statement_no_new_scope
%type <spTreeNode> simple_statement
%type <spTreeNode> declaration_statement
%type <spTreeNode> expression_statement
%type <spTreeNode> expression
%type <spTreeNode> assignment_expression
%type <spTreeNode> conditional_expression
%type <spTreeNode> logical_and_expression
%type <spTreeNode> logical_or_expression
%type <spTreeNode> logical_xor_expression
%type <spTreeNode> inclusive_or_expression
%type <spTreeNode> and_expression
%type <spTreeNode> equality_expression
%type <spTreeNode> relational_expression
%type <spTreeNode> shift_expression
%type <spTreeNode> additive_expression
%type <spTreeNode> multiplicative_expression
%type <spTreeNode> unary_expression
%type <spTreeNode> postfix_expression
%type <spTreeNode> primary_expression
%type <spTreeNode> variable_identifier
%type <spTreeNode> function_call
%type <spTreeNode> function_call_header
%type <spTreeNode> function_call_header_no_parameters
%type <spTreeNode> function_call_header_with_parameters
%type <spTreeNode> function_call_generic
%type <spTreeNode> function_identifier
%type <spTreeNode> parameter_type_specifier
%type <spTreeNode> constructor_identifier
%type <spTreeNode> variable_identifier_node
%type <spTreeNode> field_selection
%type <spTreeNode> selection_rest_statement
%type <spTreeNode> selection_statement
%type <spTreeNode> statement_with_scope
%type <spTreeNode> initializer
%type <spTreeNode> jump_statement
%type <spTreeNode> constant_expression
%type <spTreeNode> integer_expression
%type <spTreeNode> condition
%type <spTreeNode> iteration_statement
%type <spTreeNode> for_init_statement
%type <spTreeNode> for_rest_statement
%type <spTreeNode> function_identifier_node
%type <spTreeNode> iteration_statement_no_new_scope
%type <spTreeNode> struct_specifier
%type <spTreeNode> struct_declaration_list
%type <spTreeNode> struct_declaration
%type <spTreeNode> struct_declarator_list
%type <spTreeNode> struct_declarator
%type <spTreeNode> typename_identifier_node

%type <iType> type_qualifier
%type <iType> assignment_operator
%type <iType> parameter_qualifier
%type <iType> precision_qualifier
%type <iType> unary_operator
%type <iType> basic_type

%type <fConstant> bool_constant

// We have a dangling else shift / reduce conflict that we cannot resolve with
// associativity because of how the C-like grammar works in GLSL. So we expect
// exactly one shift-reduce conflict.
%expect 1

%error-verbose
%locations

%defines
%name-prefix "GLSL"
%define api.pure
%parse-param { yyscan_t scanner }
%lex-param { yyscan_t scanner }
%no-lines

// Note that RefCounted::Create uses a reference to pointer as the last
// parameter so that we don't end up having a confusing syntax like &$$ written
// in the grammar.

%%

translation_unit:
        external_declaration                                { CHK_YY(RefCounted<TranslationUnitCollectionNode>::Create(GLSLget_extra(scanner), $1, $$)); GLSLget_extra(scanner)->SetRootNode($$); }
    |   translation_unit external_declaration               { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $2)); }
    ;

variable_identifier:
        variable_identifier_node                            { $$ = $1; }
    ;

primary_expression:
        variable_identifier                                 { $$ = $1; }
    |   INTCONSTANT                                         { CHK_YY(RefCounted<IntConstantNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   DOUBLECONSTANT                                      { CHK_YY(RefCounted<FloatConstantNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   bool_constant                                       { CHK_YY(RefCounted<BoolConstantNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   LEFT_PAREN expression RIGHT_PAREN                   { CHK_YY(RefCounted<ParenExpressionNode>::Create(GLSLget_extra(scanner), $2, $$)); }
    ;

postfix_expression:
        primary_expression                                  { $$ = $1; }
    |   postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET
                                                            { CHK_YY(RefCounted<IndexSelectionNode>::Create(GLSLget_extra(scanner), $1, @1, $3, $$)); }
    |   function_call                                       { $$ = $1; }
    |   postfix_expression DOT field_selection              { CHK_YY(RefCounted<FieldSelectionNode>::Create(GLSLget_extra(scanner), $1, $3, @3, $$)); }
    |   postfix_expression INC_OP                           { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), INC_OP, UnaryOperatorType::Post, $1, @1, $$)); }
    |   postfix_expression DEC_OP                           { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), DEC_OP, UnaryOperatorType::Post, $1, @1, $$)); }
    ;

integer_expression:
        expression                                          { $$ = $1; }

function_call:
        function_call_generic                               { $$ = $1; }
    ;

function_call_generic:
        function_call_header_with_parameters RIGHT_PAREN    { CHK_YY(RefCounted<FunctionCallGenericNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   function_call_header_no_parameters RIGHT_PAREN      { CHK_YY(RefCounted<FunctionCallGenericNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

function_call_header_no_parameters:
        function_call_header VOID_TOK                       { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), $1, nullptr, @$, $$)); }
    |   function_call_header                                { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), $1, nullptr, @$, $$)); }
    ;

function_call_header_with_parameters:
        function_call_header assignment_expression          { CHK_YY(RefCounted<FunctionCallHeaderWithParametersNode>::Create(GLSLget_extra(scanner), $1, $2, @$, $$)); }
    |   function_call_header_with_parameters COMMA assignment_expression
                                                            { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $3)); }
    ;

function_call_header:
        function_identifier LEFT_PAREN                      { CHK_YY(RefCounted<FunctionCallHeaderNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

/* In the GLES grammar, TYPE_NAME is usually covered under constructor_identifier
   However, from the tokenization perspective, TYPE_NAME is equivalent to
   IDENTIFIER. Because we can't tokenize TYPE_NAME and IDENTIFIER differently,
   following the specified grammar would make our grammar ambiguous. Instead,
   IDENTIFIER will create a FunctionCallIdentifierNode which begins as
   an ambiguous IDENTIFIER. When we verify the parse tree, we'll have enough
   context to say whether the identifier is a typename identifier or function
   identifier, at which point we'll create the appropriate node. */
function_identifier:
        constructor_identifier                              { $$ = $1; }
    |   IDENTIFIER                                          { CHK_YY(RefCounted<FunctionCallIdentifierNode>::Create(GLSLget_extra(scanner), $1, @1, $$)); }
    ;

/* The grammar calls for another list of types here rather than basic_type, because the list of constructors is
   different to the list of types. But we can avoid maintaining two lists and give more meaningful errors in constructor
   usage at translation time by just using type_specifier here. Additionally the additional list in the grammar
   has TYPE_NAME, however, we cannot distinguish between TYPE_NAME and IDENTIFIER so adding that would end
   up making our grammar ambiguous. TYPE_NAME will be handled (in combination with IDENTIFIER) in the 
   function_identifer production in a subsequent change */
constructor_identifier:
        basic_type                                          { CHK_YY(RefCounted<BasicTypeNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

unary_expression:
        postfix_expression                                  { $$ = $1; }
    |   unary_operator unary_expression                     { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), $1, UnaryOperatorType::Pre, $2, @2, $$)); }
    |   INC_OP unary_expression                             { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), INC_OP, UnaryOperatorType::Pre, $2, @2, $$)); }
    |   DEC_OP unary_expression                             { CHK_YY(RefCounted<UnaryOperatorNode>::Create(GLSLget_extra(scanner), DEC_OP, UnaryOperatorType::Pre, $2, @2, $$)); }
    ;

unary_operator:
        PLUS
    |   DASH
    |   BANG
    |   TILDE
    ;

multiplicative_expression:
        unary_expression                                    { $$ = $1; }
    |   multiplicative_expression STAR unary_expression     { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, STAR, $$)); }
    |   multiplicative_expression SLASH unary_expression    { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, SLASH, $$)); }
    ;

additive_expression:
        multiplicative_expression                           { $$ = $1; }
    |   additive_expression PLUS multiplicative_expression  { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, PLUS, $$)); }
    |   additive_expression DASH multiplicative_expression  { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, DASH, $$)); }
    ;

shift_expression:
        additive_expression                                 { $$ = $1; }
    ;

relational_expression:
        shift_expression                                    { $$ = $1; }
    |   relational_expression LEFT_ANGLE shift_expression   { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, LEFT_ANGLE, $$)); }
    |   relational_expression RIGHT_ANGLE shift_expression  { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, RIGHT_ANGLE, $$)); }
    |   relational_expression LE_OP shift_expression        { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, LE_OP, $$)); }
    |   relational_expression GE_OP shift_expression        { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, GE_OP, $$)); }
    ;

equality_expression:
        relational_expression                               { $$ = $1; }
    |   equality_expression EQ_OP relational_expression     { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, EQ_OP, $$)); }
    |   equality_expression NE_OP relational_expression     { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, NE_OP, $$)); }
    ;

and_expression:
        equality_expression                                 { $$ = $1; }
    ;

inclusive_or_expression:
        and_expression                                      { $$ = $1; }
    ;

logical_and_expression:
        inclusive_or_expression                             { $$ = $1; }
    |   logical_and_expression AND_OP inclusive_or_expression  
                                                            { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, AND_OP, $$)); }
    ;   

logical_xor_expression:
        logical_and_expression                              { $$ = $1; }
    |   logical_xor_expression XOR_OP logical_and_expression 
                                                            { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, XOR_OP, $$)); }
    ;

logical_or_expression:
        logical_xor_expression                              { $$ = $1; }
    |   logical_or_expression OR_OP logical_xor_expression  { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, OR_OP, $$)); }
    ;

conditional_expression:
        logical_or_expression                               { $$ = $1; }
    |   logical_or_expression QUESTION expression COLON assignment_expression
                                                            { CHK_YY(RefCounted<ConditionalExpressionNode>::Create(GLSLget_extra(scanner), $1, $3, $5, @$, $$)); }
    ;

assignment_expression:
        conditional_expression                              { $$ = $1; }
    |   unary_expression assignment_operator assignment_expression 
                                                            { CHK_YY(RefCounted<BinaryOperatorNode>::Create(GLSLget_extra(scanner), $1, $3, @$, $2, $$)); }
    ;

assignment_operator:
        EQUAL
    |   MUL_ASSIGN
    |   DIV_ASSIGN
    |   MOD_ASSIGN
    |   ADD_ASSIGN
    |   SUB_ASSIGN
    |   LEFT_ASSIGN
    |   RIGHT_ASSIGN
    |   AND_ASSIGN
    |   XOR_ASSIGN
    |   OR_ASSIGN
    ;

expression:
        assignment_expression                               { $$ = $1; }
    |   expression COMMA assignment_expression              { CHK_YY(ExpressionListNode::CreateOrAppend(GLSLget_extra(scanner), $1, $3, &$$)); }
    ;

constant_expression:
        conditional_expression                              { $$ = $1; }
    ;

declaration:
        function_prototype SEMICOLON                        { CHK_YY(RefCounted<FunctionPrototypeDeclarationNode>::Create(GLSLget_extra(scanner), $1, @$, $$)); }
    |   init_declarator_list SEMICOLON                      { $$ = $1; }
    |   PRECISION precision_qualifier type_specifier_no_prec SEMICOLON
                                                            { CHK_YY(RefCounted<PrecisionDeclarationNode>::Create(GLSLget_extra(scanner), $2, $3, @$, $$)); }
    ;

function_prototype:        
        function_declarator RIGHT_PAREN                     { CHK_YY(RefCounted<FunctionPrototypeNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

function_declarator:        
        function_header                                     { CHK_YY(RefCounted<FunctionHeaderWithParametersNode>::Create(GLSLget_extra(scanner), $1, nullptr, $$)); }
    |   function_header_with_parameters                     { $$ = $1; }
    ;

function_header_with_parameters:
        function_header parameter_declaration               { CHK_YY(RefCounted<FunctionHeaderWithParametersNode>::Create(GLSLget_extra(scanner), $1, $2, $$)); }
    |   function_header_with_parameters COMMA parameter_declaration
                                                            { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $3)); }
    ;

function_header:        
        fully_specified_type function_identifier_node LEFT_PAREN          
                                                            { CHK_YY(RefCounted<FunctionHeaderNode>::Create(GLSLget_extra(scanner), $1, $2, $$)); }
    ;

parameter_declarator:
        type_specifier variable_identifier_node               { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), $1, $2, nullptr, @$, $$)); }
    |   type_specifier variable_identifier_node LEFT_BRACKET constant_expression RIGHT_BRACKET
                                                            { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), $1, $2, $4, @$, $$)); }
    ;

parameter_declaration:
        type_qualifier parameter_qualifier parameter_declarator
                                                            { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), $2, $3, $1, @$, $$)); } 
    |   parameter_qualifier parameter_declarator            { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), $1, $2, NO_QUALIFIER, @$, $$)); }
    |   type_qualifier parameter_qualifier parameter_type_specifier
                                                            { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), $2, $3, $1, @$, $$)); } 
    |   parameter_qualifier parameter_type_specifier        { CHK_YY(RefCounted<ParameterDeclarationNode>::Create(GLSLget_extra(scanner), $1, $2, NO_QUALIFIER, @$, $$)); }
    ;

parameter_qualifier:
        /* empty */                                         { $$ = EMPTY_TOK; }
    |   IN_TOK
    |   OUT_TOK
    |   INOUT_TOK
    ;

parameter_type_specifier:
        type_specifier                                      { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), $1, nullptr, nullptr, @$, $$)); }
    |   type_specifier LEFT_BRACKET constant_expression RIGHT_BRACKET
                                                            { CHK_YY(RefCounted<ParameterDeclaratorNode>::Create(GLSLget_extra(scanner), $1, nullptr, $3, @$, $$)); }
    ;

init_declarator_list:        
        single_declaration                                  { $$ = $1; }
    |   init_declarator_list COMMA variable_identifier_node { $$ = $1; CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>($1, GLSLget_extra(scanner), $3, nullptr, nullptr)); }
    |   init_declarator_list COMMA variable_identifier_node LEFT_BRACKET constant_expression RIGHT_BRACKET
                                                            { $$ = $1; CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>($1, GLSLget_extra(scanner), $3, nullptr, $5)); }
    |   init_declarator_list COMMA variable_identifier_node EQUAL initializer
                                                            { $$ = $1; CHK_YY(CollectionNode::CreateAppendChild<InitDeclaratorListEntryNode>($1, GLSLget_extra(scanner), $3, $5, nullptr)); }
    ;

/*
 * TODO: The grammar specifies fully_specified_type as the first production for single_declaration.
 * We differ here because having fully_specified_type causes ambiguity with expression_statement (both are valid
 * productions for IDENTIFIER SEMICOLON). Since struct_specifier is the only useful form to have here
 * along with 'type_qualifier struct_specifier' (the others have no effect on the program) we're only 
 * adding that piece right now to allow a user to declare a type without immediately having a variable of that type.
 */
single_declaration:
        struct_specifier                                    { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), $1, @$, $$)); }
    |   type_qualifier struct_specifier                     { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), $2, @$, $$)); }
    |   fully_specified_type variable_identifier_node       { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), $1, $2, @$, nullptr, nullptr, $$)); }
    |   fully_specified_type variable_identifier_node LEFT_BRACKET constant_expression RIGHT_BRACKET
                                                            { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), $1, $2, @$, nullptr, $4, $$)); }
    |   fully_specified_type variable_identifier_node EQUAL initializer        
                                                            { CHK_YY(RefCounted<InitDeclaratorListNode>::Create(GLSLget_extra(scanner), $1, $2, @$, $4, nullptr, $$)); }
    ;

fully_specified_type:        
        type_specifier                                      { CHK_YY(RefCounted<FullySpecifiedTypeNode>::Create(GLSLget_extra(scanner), NO_QUALIFIER, $1, $$)); }
    |   type_qualifier type_specifier                       { CHK_YY(RefCounted<FullySpecifiedTypeNode>::Create(GLSLget_extra(scanner), $1, $2, $$)); }
    ;

type_qualifier:
        CONST_TOK
    |   ATTRIBUTE
    |   UNIFORM
    |   VARYING
    ;   

type_specifier:        
        type_specifier_no_prec                              { CHK_YY(RefCounted<TypeSpecifierNode>::Create(GLSLget_extra(scanner), $1, NO_PRECISION, @$, $$)); }
    |   precision_qualifier type_specifier_no_prec          { CHK_YY(RefCounted<TypeSpecifierNode>::Create(GLSLget_extra(scanner), $2, $1, @$, $$)); }
    ;

type_specifier_no_prec:
        basic_type                                          { CHK_YY(RefCounted<BasicTypeNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   typename_identifier_node                            { $$ = $1; }
    |   struct_specifier                                    { $$ = $1; }
    ;

basic_type:
        FLOAT_TOK
    |   VEC2
    |   VEC3
    |   VEC4
    |   INT_TOK
    |   IVEC2_TOK
    |   IVEC3_TOK
    |   IVEC4_TOK
    |   BOOL_TOK
    |   BVEC2_TOK
    |   BVEC3_TOK
    |   BVEC4_TOK
    |   MAT2_TOK
    |   MAT3_TOK
    |   MAT4_TOK
    |   VOID_TOK
    |   SAMPLER2D
    |   SAMPLERCUBE
    ;

struct_specifier:
        STRUCT_TOK LEFT_BRACE struct_declaration_list RIGHT_BRACE
                                                            { CHK_YY(RefCounted<StructSpecifierNode>::Create(GLSLget_extra(scanner), @1, nullptr, $3, $$)); }
    |   STRUCT_TOK typename_identifier_node LEFT_BRACE struct_declaration_list RIGHT_BRACE
                                                            { CHK_YY(RefCounted<StructSpecifierNode>::Create(GLSLget_extra(scanner), @1, $2, $4, $$)); }
    ;

struct_declaration_list:
        struct_declaration                                  { CHK_YY(RefCounted<StructDeclarationListNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   struct_declaration_list struct_declaration          { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $2)); }
    ;

struct_declaration:
        type_specifier struct_declarator_list SEMICOLON     { CHK_YY(RefCounted<StructDeclarationNode>::Create(GLSLget_extra(scanner), $1, $2, $$)); }
    ;

struct_declarator_list:
        struct_declarator                                   { CHK_YY(RefCounted<StructDeclaratorListNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   struct_declarator_list COMMA struct_declarator      { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $3)); }
    ;

struct_declarator:
        variable_identifier_node                            { CHK_YY(RefCounted<StructDeclaratorNode>::Create(GLSLget_extra(scanner), $1, nullptr, $$)); }
    |   variable_identifier_node LEFT_BRACKET constant_expression RIGHT_BRACKET
                                                            { CHK_YY(RefCounted<StructDeclaratorNode>::Create(GLSLget_extra(scanner), $1, $3, $$)); }
    ;

precision_qualifier:
        HIGH_PRECISION
    |   MEDIUM_PRECISION
    |   LOW_PRECISION
    ;

initializer:
        assignment_expression                               { $$ = $1; }
    ;

declaration_statement:
        declaration                                         { $$ = $1; }
    ;

statement_no_new_scope:
        compound_statement_with_scope                       { $$ = $1; }
    |   simple_statement                                    { $$ = $1; }
    ;

simple_statement:        
        declaration_statement                               { $$ = $1; }
    |   expression_statement                                { $$ = $1; }
    |   selection_statement                                 { $$ = $1; }
    |   iteration_statement                                 { $$ = $1; }
    |   jump_statement                                      { $$ = $1; }
    ;

compound_statement_with_scope:
        LEFT_BRACE RIGHT_BRACE                              { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), nullptr, true, $$)); }
    |   LEFT_BRACE statement_list RIGHT_BRACE               { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), $2, true, $$)); }
    ;

statement_with_scope:
        compound_statement_no_new_scope                     { CHK_YY(RefCounted<ScopeStatementNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   simple_statement                                    { CHK_YY(RefCounted<ScopeStatementNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

compound_statement_no_new_scope:
        LEFT_BRACE RIGHT_BRACE                              { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), nullptr, false, $$)); }
    |   LEFT_BRACE statement_list RIGHT_BRACE               { CHK_YY(RefCounted<CompoundStatementNode>::Create(GLSLget_extra(scanner), $2, false, $$)); }
    ;

statement_list:        
        statement_no_new_scope                              { CHK_YY(RefCounted<StatementListNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    |   statement_list statement_no_new_scope               { $$ = $1; CHK_YY(CollectionNode::AppendChild($1, $2)); }
    ;

iteration_statement_no_new_scope:
        statement_no_new_scope                              { CHK_YY(RefCounted<IterationStatementNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

expression_statement:
        SEMICOLON                                           { CHK_YY(RefCounted<ExpressionStatementNode>::Create(GLSLget_extra(scanner), nullptr, $$)); }
    |   expression SEMICOLON                                { CHK_YY(RefCounted<ExpressionStatementNode>::Create(GLSLget_extra(scanner), $1, $$)); }
    ;

selection_statement:
        IF_TOK LEFT_PAREN expression RIGHT_PAREN selection_rest_statement    
                                                            { CHK_YY(RefCounted<SelectionStatementNode>::Create(GLSLget_extra(scanner), $3, $5, @$, $$)); }
    ;

selection_rest_statement:
        statement_with_scope ELSE_TOK statement_with_scope  { CHK_YY(RefCounted<SelectionRestStatementNode>::Create(GLSLget_extra(scanner), $1, $3, $$)); }
    |   statement_with_scope                                { CHK_YY(RefCounted<SelectionRestStatementNode>::Create(GLSLget_extra(scanner), $1, nullptr, $$)); }
    ;

condition:
        expression                                          { $$ = $1; }
    ;

iteration_statement:
        FOR_TOK LEFT_PAREN for_init_statement for_rest_statement RIGHT_PAREN iteration_statement_no_new_scope
                                                            { CHK_YY(RefCounted<ForStatementNode>::Create(GLSLget_extra(scanner), $3, $4, $6, @$, $$)); }
    ;

for_init_statement:
        expression_statement                                { $$ = $1; }
    |   declaration_statement                               { $$ = $1; }
    ;

for_rest_statement:
        condition SEMICOLON                                 { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), $1, nullptr, $$)); }
    |   condition SEMICOLON expression                      { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), $1, $3, $$)); }
    |   SEMICOLON expression                                { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), nullptr, $2, $$)); }
    |   SEMICOLON                                           { CHK_YY(RefCounted<ForRestStatementNode>::Create(GLSLget_extra(scanner), nullptr, nullptr, $$)); }
    ;

jump_statement:
        BREAK_TOK SEMICOLON                                 { CHK_YY(RefCounted<BreakStatementNode>::Create(GLSLget_extra(scanner), @$, $$)); }
    |   RETURN_TOK SEMICOLON                                { CHK_YY(RefCounted<ReturnStatementNode>::Create(GLSLget_extra(scanner), nullptr, $$)); }
    |   RETURN_TOK expression SEMICOLON                     { CHK_YY(RefCounted<ReturnStatementNode>::Create(GLSLget_extra(scanner), $2, $$)); }
    |   DISCARD_TOK SEMICOLON                               { CHK_YY(RefCounted<DiscardStatementNode>::Create(GLSLget_extra(scanner), @$, $$)); }
    |   CONTINUE_TOK SEMICOLON                              { CHK_YY(RefCounted<ContinueStatementNode>::Create(GLSLget_extra(scanner), @$, $$)); }
    ;

external_declaration:
        function_definition                                 { $$ = $1; }
    |   declaration                                         { $$ = $1; }
    ;

function_definition:
        function_prototype compound_statement_no_new_scope  { CHK_YY(RefCounted<FunctionDefinitionNode>::Create(GLSLget_extra(scanner), $1, $2, @$, $$)); }
    ;

/* Grammar not in the specification but added to complete the parser logic */

field_selection:
        variable_identifier_node                            { $$ = $1; }

bool_constant:
        TRUE_TOK
    |   FALSE_TOK
    ;

variable_identifier_node:
        IDENTIFIER                                          { CHK_YY(RefCounted<VariableIdentifierNode>::Create(GLSLget_extra(scanner), $1, @1, $$)); }
    ;

function_identifier_node:
        IDENTIFIER                                          { CHK_YY(RefCounted<FunctionIdentifierNode>::Create(GLSLget_extra(scanner), $1, @1, $$)); }
    ;

typename_identifier_node:
        IDENTIFIER                                          { CHK_YY(RefCounted<TypeNameIdentifierNode>::Create(GLSLget_extra(scanner), $1, @1, $$)); }
    ;

%%

