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

// All of the parse tree subtypes are included here so that the generated parser can get to them
// and they can be expanded without editing the .y file and rerunning bison.

#include "BasicTypeNode.hxx"
#include "BinaryOperatorNode.hxx"
#include "BoolConstantNode.hxx"
#include "BreakStatementNode.hxx"
#include "CompoundStatementNode.hxx"
#include "ConditionalExpressionNode.hxx"
#include "ContinueStatementNode.hxx"
#include "DiscardStatementNode.hxx"
#include "ExpressionListNode.hxx"
#include "ExpressionStatementNode.hxx"
#include "FieldSelectionNode.hxx"
#include "FloatConstantNode.hxx"
#include "FullySpecifiedTypeNode.hxx"
#include "FunctionCallGenericNode.hxx"
#include "FunctionCallHeaderNode.hxx"
#include "FunctionCallHeaderWithParametersNode.hxx"
#include "FunctionCallIdentifierNode.hxx"
#include "FunctionDefinitionNode.hxx"
#include "FunctionHeaderNode.hxx"
#include "FunctionHeaderWithParametersNode.hxx"
#include "FunctionIdentifierNode.hxx"
#include "FunctionPrototypeNode.hxx"
#include "FunctionPrototypeDeclarationNode.hxx"
#include "IndexSelectionNode.hxx"
#include "InitDeclaratorListNode.hxx"
#include "InitDeclaratorListEntryNode.hxx"
#include "IntConstantNode.hxx"
#include "IterationStatementNode.hxx"
#include "ParameterDeclarationNode.hxx"
#include "ParameterDeclaratorNode.hxx"
#include "ParenExpressionNode.hxx"
#include "PrecisionDeclarationNode.hxx"
#include "ReturnStatementNode.hxx"
#include "ScopeStatementNode.hxx"
#include "SelectionStatementNode.hxx"
#include "SelectionRestStatementNode.hxx"
#include "StatementListNode.hxx"
#include "StructSpecifierNode.hxx"
#include "StructDeclarationListNode.hxx"
#include "StructDeclarationNode.hxx"
#include "StructDeclaratorListNode.hxx"
#include "StructDeclaratorNode.hxx"
#include "VariableIdentifierNode.hxx"
#include "TranslationUnitNode.hxx"
#include "TypeNameIdentifierNode.hxx"
#include "TypeSpecifierNode.hxx"
#include "UnaryOperatorNode.hxx"
#include "ForStatementNode.hxx"
#include "ForRestStatementNode.hxx"
