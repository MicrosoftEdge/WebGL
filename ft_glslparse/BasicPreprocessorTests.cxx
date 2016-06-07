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
﻿#include "headers.hxx"
#include "BasicPreprocessorTests.hxx"
#include "GLSLPreProcess.hxx"
#include "GLSLStringParserInput.hxx"
#include "MemoryStream.hxx"
#include "ParserTestUtils.hxx"
#include "RefCounted.hxx"
#include "TestErrorSink.hxx"
#include "GLSLLineMap.hxx"
#include "GLSLExtensionState.hxx"
#include "GLSLTranslateOptions.hxx"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace ft_glslparse
{
    void BasicPreprocessorTests::VersionTests()
    {
        // The most basic case
        TestPreprocessorInput("#version 100\n",                                                     "\n");

        // Bigger and smaller versions should fail
        TestPreprocessorNegative("#version 90\n", E_GLSLERROR_WRONGVERSION);
        TestPreprocessorNegative("#version 110\n", E_GLSLERROR_WRONGVERSION);

        // Hex and octal versions don't work
        TestPreprocessorNegative("#version 0x1\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#version 01\n", E_GLSLERROR_SYNTAXERROR);

        // Don't mess up the syntax
        TestPreprocessorNegative("#version 100 100\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#version 100.0\n", E_GLSLERROR_SYNTAXERROR);

        // No statements before
        TestPreprocessorNegative("#define FOO\n#version 100\n", E_GLSLERROR_UNEXPECTEDVERSION);
        TestPreprocessorNegative("#ifdef FOO\n#endif\n#version 100\n", E_GLSLERROR_UNEXPECTEDVERSION);
        TestPreprocessorNegative("#ifdef FOO\n#version 100\n#endif\n", E_GLSLERROR_UNEXPECTEDVERSION);
        TestPreprocessorNegative("#\n#version 100\n", E_GLSLERROR_UNEXPECTEDVERSION);

        // Whitespace and comments OK before version
        TestPreprocessorInput("\t  \t\n#version 100\n",                                             "\t  \t\n\n");
        TestPreprocessorInput("\n#version 100\n",                                                   "\n\n");
        TestPreprocessorInput("/*comment*/\n#version 100\n",                                        "           \n\n");
        TestPreprocessorInput("\n\n\n#version 100\n",                                               "\n\n\n\n");

        // Text not OK
        TestPreprocessorNegative("Hi I am text\n#version 100\n", E_GLSLERROR_UNEXPECTEDVERSION);
    }

    void BasicPreprocessorTests::BasicDirectiveTests()
    {
        // Directives can start with spaces / tabs, and have spaces / tabs before the word
        TestPreprocessorInput(" #define FOO\n",                                                     "\n");
        TestPreprocessorInput("# define FOO\n",                                                     "\n");
        TestPreprocessorInput(" # define FOO\n",                                                    "\n");

        TestPreprocessorInput("\t#define FOO\n",                                                    "\n");
        TestPreprocessorInput("#\tdefine FOO\n",                                                    "\n");
        TestPreprocessorInput("\t#\tdefine FOO\n",                                                  "\n");

        TestPreprocessorInput("\t #define FOO\n",                                                   "\n");
        TestPreprocessorInput("# \tdefine FOO\n",                                                   "\n");
        TestPreprocessorInput("\t # \tdefine FOO\n",                                                "\n");

        // Non-whitespace not OK (apart from comments)
        TestPreprocessorNegative("aaaa#define FOO\n", E_GLSLERROR_PREDIRECTIVENOTFIRST);
        TestPreprocessorNegative(" \t a/**/aa  #define FOO\n", E_GLSLERROR_PREDIRECTIVENOTFIRST);
        TestPreprocessorNegative(" \t aaa#define FOO\n", E_GLSLERROR_PREDIRECTIVENOTFIRST);
        TestPreprocessorNegative("aa/**/a \t #define FOO\n", E_GLSLERROR_PREDIRECTIVENOTFIRST);

        TestPreprocessorInput(" /* comment */  #define FOO\n",                                     "              \n");

        // Directives with no name can be used as a comment line
        TestPreprocessorInput("#\n",                                                                "\n");
        TestPreprocessorInput(" \t#\t \t \n",                                                       "\n");
    }

    void BasicPreprocessorTests::IfDefTests()
    {
        // Test last line with newline
        TestPreprocessorInput("#ifdef FOO\nBlah\n#endif\nBAR\n",                                    "\n\n\nBAR\n");
        TestPreprocessorInput("#ifdef GL_ES\nBlah\n#endif\nBAR\n",                                  "\nBlah\n\nBAR\n");

        // Test blank line
        TestPreprocessorInput("#ifdef FOO\nBlah\n\n#endif\nBAR\n",                                  "\n\n\n\nBAR\n");
        TestPreprocessorInput("#ifdef GL_ES\nBlah\n\n#endif\nBAR\n",                                "\nBlah\n\n\nBAR\n");

        // Test combinations of having no non-directive lines - these should hit all of the combinations
        // in the .y file. Try to keep them in the same order.
        TestPreprocessorInput("#ifdef GL_ES\n#endif\n",                                             "\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nFoo\n#endif\n",                                        "\nFoo\n\n");
        TestPreprocessorInput("#ifdef GL_ES\n#else\n#endif\n",                                      "\n\n\n");
        TestPreprocessorInput("#ifdef GL_ES\n#else\nBar\n#endif\n",                                 "\n\n\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nFoo\n#else\n#endif\n",                                 "\nFoo\n\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nFoo\n#else\nBar\n#endif\n",                            "\nFoo\n\n\n\n");

        // Test nesting of defined inside of defined, with some else conditions
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#ifdef GL_ES\nLine2\n#endif\nLine3\n#endif\n",  "\nLine1\n\nLine2\n\nLine3\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#ifdef GL_ES\nLine2\n#else\nLine3\n#endif\nLine4\n#else\nLine5\n#endif\n",  
                                                                                                    "\nLine1\n\nLine2\n\n\n\nLine4\n\n\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#else\nLine2\n#ifdef GL_ES\nLine3\n#else\nLine4\n#endif\nLine5\n#endif\n",  
                                                                                                    "\nLine1\n\n\n\n\n\n\n\n\n\n");

        // Test nesting of undefined inside of defined, with some else conditions
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#ifdef FOO\nLine2\n#endif\nLine3\n#endif\n",    "\nLine1\n\n\n\nLine3\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#ifdef FOO\nLine2\n#else\nLine3\n#endif\nLine4\n#else\nLine5\n#endif\n",    
                                                                                                    "\nLine1\n\n\n\nLine3\n\nLine4\n\n\n\n");
        TestPreprocessorInput("#ifdef GL_ES\nLine1\n#else\nLine2\n#ifdef FOO\nLine3\n#else\nLine4\n#endif\nLine5\n#endif\n",    
                                                                                                    "\nLine1\n\n\n\n\n\n\n\n\n\n");

        // Test nesting of defined inside of undefined, with some else conditions
        TestPreprocessorInput("#ifdef FOO\nLine1\n#ifdef GL_ES\nLine2\n#endif\nLine3\n#endif\n",    "\n\n\n\n\n\n\n");
        TestPreprocessorInput("#ifdef FOO\nLine1\n#ifdef GL_ES\nLine2\n#else\nLine3\n#endif\nLine4\n#else\nLine5\n#endif\n",    
                                                                                                    "\n\n\n\n\n\n\n\n\nLine5\n\n");
        TestPreprocessorInput("#ifdef FOO\nLine1\n#else\nLine2\n#ifdef GL_ES\nLine3\n#else\nLine4\n#endif\nLine5\n#endif\n",    
                                                                                                    "\n\n\nLine2\n\nLine3\n\n\n\nLine5\n\n");

        // Test nesting of undefined inside of undefined, with some else conditions
        TestPreprocessorInput("#ifdef FOO\nLine1\n#ifdef BAR\nLine2\n#endif\nLine3\n#endif\n",      "\n\n\n\n\n\n\n");
        TestPreprocessorInput("#ifdef FOO\nLine1\n#ifdef BAR\nLine2\n#else\nLine3\n#endif\nLine4\n#else\nLine5\n#endif\n",      
                                                                                                    "\n\n\n\n\n\n\n\n\nLine5\n\n");
        TestPreprocessorInput("#ifdef FOO\nLine1\n#else\nLine2\n#ifdef BAR\nLine3\n#else\nLine4\n#endif\nLine5\n#endif\n",      
                                                                                                    "\n\n\nLine2\n\n\n\nLine4\n\nLine5\n\n");

        // Missing identifier
        TestPreprocessorNegative("#ifdef\nLine1\n#endif", E_GLSLERROR_SYNTAXERROR);

        // Missing endif
        TestPreprocessorNegative("#ifdef GL_ES\nLine1\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#ifdef FOO\nLine1\n", E_GLSLERROR_SYNTAXERROR);

        // Missing nested endif
        TestPreprocessorNegative("#ifdef GL_ES\nLine1\n#ifdef FOO\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#ifdef FOO\nLine1\n#ifdef GL_ES\n#endif\n", E_GLSLERROR_SYNTAXERROR);

        // Directives inside paren started elsewhere
        TestPreprocessorInput("void foo(int a,\n#ifdef GL_ES\nint thisDoes,\n#endif\nint c) {}\n",  "void foo(int a,\n\nint thisDoes,\n\nint c) {}\n");
        TestPreprocessorInput("void foo(int a,\n#ifdef THISDOESNT\nint thisDoesnt,\n#endif\nint c) {}\n", 
                                                                                                    "void foo(int a,\n\n\n\nint c) {}\n");
        
        TestPreprocessorInput("void foo(int a,\n#if defined(GL_ES)\nint thisDoes,\n#endif\nint c) {}\n",  
                                                                                                    "void foo(int a,\n\nint thisDoes,\n\nint c) {}\n");
        TestPreprocessorInput("void foo(int a,\n#if defined(THISDOESNT)\nint thisDoesnt,\n#endif\nint c) {}\n", 
                                                                                                    "void foo(int a,\n\n\n\nint c) {}\n");

        TestPreprocessorInput("(\n#define X(a) (a+1)\n)\nX(2)\n",                                   "(\n\n)\n( 2 + 1 )\n");
    }

    void BasicPreprocessorTests::IfNDefTests()
    {
        TestPreprocessorInput("#ifndef FOO\nBlah\n#endif\nBAR\n",                                    "\nBlah\n\nBAR\n");
        TestPreprocessorInput("#ifndef GL_ES\nBlah\n#endif\nBAR\n",                                  "\n\n\nBAR\n");
    }

    void BasicPreprocessorTests::IfDefinedTests()
    {
        // Test syntax with paren
        TestPreprocessorInput("#if defined(FOO)\nBlah\n#endif\nBAR\n",                               "\n\n\nBAR\n");
        TestPreprocessorInput("#if defined(GL_ES)\nBlah\n#endif\nBAR\n",                             "\nBlah\n\nBAR\n");

        // Test syntax without paren
        TestPreprocessorInput("#if defined FOO\nBlah\n#endif\nBAR\n",                               "\n\n\nBAR\n");
        TestPreprocessorInput("#if defined GL_ES\nBlah\n#endif\nBAR\n",                             "\nBlah\n\nBAR\n");
    }

    void BasicPreprocessorTests::ElifTests()
    {
        // Test a single elif with each of the kinds of statement it can be used with
        TestPreprocessorInput("#if defined(FOO)\nBlah\n#elif defined(GL_ES)\nBAR\n#endif\n",        "\n\n\nBAR\n\n");
        TestPreprocessorInput("#ifdef FOO\nBlah\n#elif defined(GL_ES)\nBAR\n#endif\n",              "\n\n\nBAR\n\n");
        TestPreprocessorInput("#ifndef GL_ES\nBlah\n#elif defined(GL_ES)\nBAR\n#endif\n",           "\n\n\nBAR\n\n");

        // Test multiple elif with no else statement
        TestPreprocessorInput("#define FOO\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n",
                                                                                                    "\n\nFoo\n\n\n\n\n\n");
        TestPreprocessorInput("#define BAR\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n",
                                                                                                    "\n\n\n\nBar\n\n\n\n");
        TestPreprocessorInput("#define BAZ\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n",
                                                                                                    "\n\n\n\n\n\nBaz\n\n");

        // Test multiple elif with else statement
        TestPreprocessorInput("#define FOO\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#else\nelse\n#endif\n",
                                                                                                    "\n\nFoo\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#define BAR\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#else\nelse\n#endif\n",
                                                                                                    "\n\n\n\nBar\n\n\n\n\n\n");
        TestPreprocessorInput("#define BAZ\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#else\nelse\n#endif\n",
                                                                                                    "\n\n\n\n\n\nBaz\n\n\n\n");
        TestPreprocessorInput("#define ELSE\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#else\nelse\n#endif\n",
                                                                                                    "\n\n\n\n\n\n\n\nelse\n\n");

        // Nest the some of these conditions in another condition
        TestPreprocessorInput("#ifdef NEVER\n#if defined(FOO)\nBlah\n#elif defined(GL_ES)\nBAR\n#endif\n#endif\n",
                                                                                                    "\n\n\n\n\n\n\n");
        TestPreprocessorInput("#ifdef NEVER\n#define FOO\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n#endif\n",
                                                                                                    "\n\n\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#ifdef NEVER\n#define BAR\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n#endif\n",
                                                                                                    "\n\n\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#ifdef NEVER\n#define BAZ\n#ifdef FOO\nFoo\n#elif defined(BAR)\nBar\n#elif defined(BAZ)\nBaz\n#endif\n#endif\n",
                                                                                                    "\n\n\n\n\n\n\n\n\n\n");

        // elif with no condition that is not taken should not care about syntax
        TestPreprocessorInput("#ifdef GL_ES\n#elif\n#endif\n",                                      "\n\n\n");

        // elif with no condition that is taken should about syntax
        TestPreprocessorNegative("#ifndef GL_ES\n#elif\n#endif\n", E_GLSLERROR_SYNTAXERROR);

        // No elif without if
        TestPreprocessorNegative("#elif defined(GL_ES)\nBAR\n#endif\n", E_GLSLERROR_SYNTAXERROR);

        // No elif after else
        TestPreprocessorNegative("#ifdef FOO\nFoo\n#else\nBar\n#elif defined(BAR)\nBar\n#endif\n", E_GLSLERROR_SYNTAXERROR);

        // elif with or without same condition as the original if
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#endif\n",                               "\n\n\n\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#endif\n",                               "\n\n\nFoo2\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#endif\n",                               "\nFoo1\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#endif\n",                               "\nFoo1\n\n\n\n");

        // And with else added
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#else\nElse\n#endif\n",                  "\n\n\n\n\nElse\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#else\nElse\n#endif\n",                  "\n\n\nFoo2\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#else\nElse\n#endif\n",                  "\nFoo1\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#else\nElse\n#endif\n",                  "\nFoo1\n\n\n\n\n\n");

        // Now with 3 conditions, so that we test that various patterns work as expected
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#elif 0\nFoo3\n#endif\n",                "\n\n\n\n\n\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#elif 1\nFoo3\n#endif\n",                "\n\n\n\n\nFoo3\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#elif 0\nFoo3\n#endif\n",                "\n\n\nFoo2\n\n\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#elif 1\nFoo3\n#endif\n",                "\n\n\nFoo2\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#elif 0\nFoo3\n#endif\n",                "\nFoo1\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#elif 1\nFoo3\n#endif\n",                "\nFoo1\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#elif 0\nFoo3\n#endif\n",                "\nFoo1\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#elif 1\nFoo3\n#endif\n",                "\nFoo1\n\n\n\n\n\n");

        // Also with if
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#elif 0\nFoo3\n#else\nElse\n#endif\n",   "\n\n\n\n\n\n\nElse\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 0\nFoo2\n#elif 1\nFoo3\n#else\nElse\n#endif\n",   "\n\n\n\n\nFoo3\n\n\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#elif 0\nFoo3\n#else\nElse\n#endif\n",   "\n\n\nFoo2\n\n\n\n\n\n");
        TestPreprocessorInput("#if 0\nFoo1\n#elif 1\nFoo2\n#elif 1\nFoo3\n#else\nElse\n#endif\n",   "\n\n\nFoo2\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#elif 0\nFoo3\n#else\nElse\n#endif\n",   "\nFoo1\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 0\nFoo2\n#elif 1\nFoo3\n#else\nElse\n#endif\n",   "\nFoo1\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#elif 0\nFoo3\n#else\nElse\n#endif\n",   "\nFoo1\n\n\n\n\n\n\n\n");
        TestPreprocessorInput("#if 1\nFoo1\n#elif 1\nFoo2\n#elif 1\nFoo3\n#else\nElse\n#endif\n",   "\nFoo1\n\n\n\n\n\n\n\n");
    }

    void BasicPreprocessorTests::IfMultiplicationTests()
    {
        TestPreprocessorInput("#define FOO 2\n#define BAZ 3\n#if FOO * BAZ == 6\nBAR\n#endif\n",    "\n\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 8\n#define BAZ 2\n#if FOO / BAZ == 4\nBAR\n#endif\n",    "\n\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 7\n#define BAZ 3\n#if FOO % BAZ == 1\nBAR\n#endif\n",    "\n\n\nBAR\n\n");

        TestPreprocessorNegative("#define FOO 1\n#if FOO / 0 == 1\nBAR\n#endif\n", E_GLSLERROR_DIVIDEORMODBYZERO);
        TestPreprocessorNegative("#define FOO 1\n#if FOO % 0 == 1\nBAR\n#endif\n", E_GLSLERROR_DIVIDEORMODBYZERO);
    }

    void BasicPreprocessorTests::IfAdditionTests()
    {
        TestPreprocessorInput("#define FOO 1\n#define BAZ 2\n#if FOO + BAZ == 3\nBAR\n#endif\n",    "\n\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 3\n#define BAZ 2\n#if FOO - BAZ == 1\nBAR\n#endif\n",    "\n\n\nBAR\n\n");

        // Take the opportunity to test hex and octal constants
        TestPreprocessorInput("#define FOO 0xF\n#define BAZ 010\n#if FOO + BAZ == 23\nBAR\n#endif\n",
                                                                                                    "\n\n\nBAR\n\n");
    }

    void BasicPreprocessorTests::IfShiftTests()
    {
        TestPreprocessorInput("#define FOO 1\n#if FOO << 2 == 4\nBAR\n#endif\n",                    "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAZ 3\n#if FOO << BAZ == 8\nBAR\n#endif\n",   "\n\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 16\n#if FOO >> 2 == 4\nBAR\n#endif\n",                   "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 32\n#define BAZ 3\n#if FOO >> BAZ == 4\nBAR\n#endif\n",  "\n\n\nBAR\n\n");
    }

    void BasicPreprocessorTests::IfRelationalTests()
    {
        // Test all of the operator combinations
        TestPreprocessorInput("#define FOO 1\n#if FOO < 2\nBAR\n#endif\n",                          "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#if 2 > FOO\nBAR\n#endif\n",                          "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#if FOO <= 1\nBAR\n#endif\n",                         "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#if 1 >= FOO\nBAR\n#endif\n",                         "\n\nBAR\n\n");
    }

    void BasicPreprocessorTests::IfEqualityTests()
    {
        // Test both operators succeeding and failing
        TestPreprocessorInput("#define FOO 1\n#if FOO == 1\nBAR\n#endif\n",                         "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#if FOO == 2\nBAR\n#endif\n",                         "\n\n\n\n");

        TestPreprocessorInput("#define FOO 1\n#if FOO != 0\nBAR\n#endif\n",                         "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 1\n#if FOO != 1\nBAR\n#endif\n",                         "\n\n\n\n");
    }

    void BasicPreprocessorTests::IfLogicalTests()
    {
        // Logical AND - both must be defined
        TestPreprocessorInput("#define FOO 1\n#define BAR 2\n#if FOO && BAR\nBAZ\n#endif\n",        "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 2\n#if FOO && BAR\nBAZ\n#endif\n",        "\n\n\n\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 0\n#if FOO && BAR\nBAZ\n#endif\n",        "\n\n\n\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 0\n#if FOO && BAR\nBAZ\n#endif\n",        "\n\n\n\n\n");

        // Logical OR - only 1 need be defined
        TestPreprocessorInput("#define FOO 1\n#define BAR 2\n#if FOO || BAR\nBAZ\n#endif\n",        "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 2\n#if FOO || BAR\nBAZ\n#endif\n",        "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 0\n#if FOO || BAR\nBAZ\n#endif\n",        "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 0\n#if FOO || BAR\nBAZ\n#endif\n",        "\n\n\n\n\n");
    }

    void BasicPreprocessorTests::IfUnaryTests()
    {
        TestPreprocessorInput("#define FOO 1\n#if -FOO < 0\nBAZ\n#endif\n",                         "\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#if -FOO > 0\nBAZ\n#endif\n",                         "\n\n\n\n");

        TestPreprocessorInput("#define FOO 1\n#if +FOO > 0\nBAZ\n#endif\n",                         "\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#if +FOO < 0\nBAZ\n#endif\n",                         "\n\n\n\n");

        TestPreprocessorInput("#define FOO 0\n#if !FOO\nBAZ\n#endif\n",                             "\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#if !FOO\nBAZ\n#endif\n",                             "\n\n\n\n");

        // This will work when equality is added
        TestPreprocessorInput("#define FOO 0\n#if ~FOO == -1\nBAZ\n#endif\n",                       "\n\nBAZ\n\n");
    }

    void BasicPreprocessorTests::IfBitwiseTests()
    {
        // Bitwise OR
        TestPreprocessorInput("#define FOO 0\n#define BAR 0\n#if (FOO | BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 1\n#if (FOO | BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 0\n#if (FOO | BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 1\n#if (FOO | BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");

        // Bitwise XOR
        TestPreprocessorInput("#define FOO 0\n#define BAR 0\n#if (FOO ^ BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 1\n#if (FOO ^ BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 0\n#if (FOO ^ BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 1\n#if (FOO ^ BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");

        // Bitwise AND
        TestPreprocessorInput("#define FOO 0\n#define BAR 0\n#if (FOO & BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 0\n#define BAR 1\n#if (FOO & BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 0\n#if (FOO & BAR) == 0\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
        TestPreprocessorInput("#define FOO 1\n#define BAR 1\n#if (FOO & BAR) == 1\nBAZ\n#endif\n",    "\n\n\nBAZ\n\n");
    }

    void BasicPreprocessorTests::IfPrecedenceTests()
    {
        /* AND is higher precedence than OR, so if we have:

           A = 0
           B = 0
           C = 1

           Then (A && B) || C == 1, and A && (B || C) == 0

           Since && is higher than ||, with no paren we should end up with a result of 1 */
        TestPreprocessorInput("#define A 0\n#define B 0\n#define C 1\n#if A && B || C\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\nTRUE\n\n");
        TestPreprocessorInput("#define A 0\n#define B 0\n#define C 1\n#if C || A && B\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\nTRUE\n\n");

        /* Comparison is higher than AND, so:

           A = 0
           B = 1
           C = 1

           (A && B) < C == 1, but A && (B < C) == 0

           Since < is higher than &&, without paren we should end up with a result of 0 */
        TestPreprocessorInput("#define A 0\n#define B 1\n#define C 1\n#if A && B < C\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\n\n\n");
        TestPreprocessorInput("#define A 0\n#define B 1\n#define C 1\n#if B < C && A\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\n\n\n");

        /* Bitwise AND is higher than bitwise OR, so:

           A = 0
           B = 1
           C = 1

           (A & B) | C == 1, but A & (B | C) == 0

           Since & is higher than |, without paren we should end up with a result of 1 */
        TestPreprocessorInput("#define A 0\n#define B 1\n#define C 1\n#if A & B | C\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\nTRUE\n\n");
        TestPreprocessorInput("#define A 0\n#define B 1\n#define C 1\n#if B | C & A\nTRUE\n#endif\n",
                                                                                                    "\n\n\n\nTRUE\n\n");

    }

    void BasicPreprocessorTests::ExpressionTests()
    {
        // Defined should return 1 when things are defined and 0 otherwise
        TestPreprocessorInput("#define FOO 1\n#if FOO <= defined(FOO)\nBAR\n#endif\n",              "\n\nBAR\n\n");
        TestPreprocessorInput("#define FOO 0\n#if FOO <= defined(NOTFOO)\nBAR\n#endif\n",           "\n\nBAR\n\n");

        // Undefined identifiers should generate a syntax error
        TestPreprocessorNegative("#if NOTDEFINED > 1\nBAR\n#endif\n", E_GLSLERROR_SYNTAXERROR);

        // But not inside disabled output
        TestPreprocessorInput("#ifdef NOTDEFINED\n#if NOTDEFINED > 1\nBAR\n#endif\n#endif\n",       "\n\n\n\n\n");

        // Lookup of macros should not cause issues when output is disabled
        TestPreprocessorInput("#define A 1\n#define B 0\n#if B\nNo\n#if A\nNo\n#endif\n#endif\n",   "\n\n\n\n\n\n\n\n");

        // We have operator tokens defined in the grammar that should never be used in a preprocessor expression
        TestPreprocessorNegative("#define X 1\n#if X >>= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X <<= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X *= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X -= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X += 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X |= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X %= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X ^= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X /= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X &= 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X ^^ 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X++\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define X 1\n#if X--\n#endif\n", E_GLSLERROR_SYNTAXERROR);
    }

    void BasicPreprocessorTests::FunctionMacroTests()
    {
        // Define a simple function macro that uses a known pp operator
        TestPreprocessorInput("#define ADD(x, y) x + y\n",                                          "\n");

        // Define with no substitution
        TestPreprocessorInput("#define FOO(x)\n#ifdef FOO\nBAR\n#endif\n",                          "\n\nBAR\n\n");

        // Basic substitution of various types of single token, including non-operator tokens
        TestPreprocessorInput("#define X(x) Y+x\nX(a)\n",                                           "\nY + a\n");
        TestPreprocessorInput("#define FOO(x) x[1]\nFOO(a)\n",                                      "\na [ 1 ]\n");
        TestPreprocessorInput("#define FOO(x) BAR+x\nFOO(a)\n",                                     "\nBAR + a\n");
        TestPreprocessorInput("#define FOO(x) 1+x\nFOO(a)\n",                                       "\n1 + a\n");
        TestPreprocessorInput("#define FOO(x) 1.0+x\nFOO(a)\n",                                     "\n1.0 + a\n");
        TestPreprocessorInput("#define FOO(x) x:x\nFOO(1)\n",                                       "\n1 : 1\n");

        // Basic substitution of various types of two tokens, including non-operator tokens
        TestPreprocessorInput("#define X(x, y) Y+x+y\nX(a, b)\n",                                   "\nY + a +  b\n");
        TestPreprocessorInput("#define FOO(x, y) x[y]\nFOO(a, b)\n",                                "\na [  b ]\n");
        TestPreprocessorInput("#define FOO(x, y) BAR+x+y\nFOO(a, b)\n",                             "\nBAR + a +  b\n");
        TestPreprocessorInput("#define FOO(x, y) 1+x[ y ]\nFOO(a, b)\n",                             "\n1 + a [  b ]\n");
        TestPreprocessorInput("#define FOO(x, y) 1.0+x - y\nFOO(a, b)\n",                           "\n1.0 + a -  b\n");

        // Whitespace in arguments is preserved
        TestPreprocessorInput("#define X(x, y) (x)(y)\nX(\ta ,b\t)\n",                              "\n( \ta  ) ( b\t )\n");

        // Test basic macro expansion
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO(foo)\n",                     "\n( 1 + foo )\n");

        // Test expanding with a C++ style comment in the middle
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO//\n(foo)\n",                 "\n\n( 1 + foo )\n");
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO(//\nfoo)\n",                 "\n\n( 1 + foo )\n");
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO(\nfoo//\n)\n",               "\n\n\n( 1 + foo )\n");

        // Test expanding with a C comment in the middle
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO/*comment*/(foo)\n",          "\n( 1 + foo )\n");
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO(/*comment*/foo)\n",          "\n( 1 + foo )\n");
        TestPreprocessorInput("#define TEST_MACRO(x) (1+x)\nTEST_MACRO(foo/*comment*/)\n",          "\n( 1 + foo )\n");

        // Define a function macro and reference another regular macro
        TestPreprocessorInput("#define X Y\n#define F(x) (x + X)\nF(1)\n",                          "\n\n( 1 + Y )\n");
        TestPreprocessorInput("#define F(x) (x + X)\n#define X Y\nF(1)\n",                          "\n\n( 1 + Y )\n");

        // Define 2 function macros and call them inside each other
        TestPreprocessorInput("#define F(x) (x+1)\n#define G(param) param+2\nF(G(arg))\n",          "\n\n( arg + 2 + 1 )\n");

        // Define a single arg macro, and have a GLSL function call in it
        TestPreprocessorInput("#define F(x) x+1\nF(bar(1.0));\n",                                   "\nbar(1.0) + 1;\n");
        TestPreprocessorInput("#define F(x) x+1\nF(bar(1.0, 2.0));\n",                              "\nbar(1.0, 2.0) + 1;\n");    

        // Define a multi arg macro, and have a GLSL function call in it
        TestPreprocessorInput("#define F(x, y) x + y\nF(foo(1.0), bar(2));\n",                      "\nfoo(1.0) +  bar(2);\n");
        TestPreprocessorInput("#define F(x, y) x + y\nF(foo(1.0, 2.0), bar(2, 1));\n",              "\nfoo(1.0, 2.0) +  bar(2, 1);\n");

        // Macros without arguments do not read arguments passed to them
        TestPreprocessorInput("#define FOO a\nFOO(1)\n",                                            "\na(1)\n");

        // You cannot start a macro and have a newline in the parameters
        TestPreprocessorNegative("#define FOO(x,\n", E_GLSLERROR_SYNTAXERROR);

        // You cannot use non-identifiers as parameters
        TestPreprocessorNegative("#define FOO(1)\n", E_GLSLERROR_SYNTAXERROR);

        // You cannot skip out on having a parameter
        TestPreprocessorNegative("#define FOO()\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define FOO(a,)\n", E_GLSLERROR_SYNTAXERROR);

        // You cannot define a macro with parameters and call it without parameters - this can hit at the end
        // of the file or in the middle so test both paths.
        TestPreprocessorNegative("#define FOO(x) x\nFOO\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define FOO(x) x\nFOO\nA", E_GLSLERROR_SYNTAXERROR);

        // You cannot define a macro with parameters and call it with incorrect parameter count
        TestPreprocessorNegative("#define FOO(x) x\nFOO(1,2)\n", E_GLSLERROR_PREINVALIDMACROPARAMCOUNT);
    }

    void BasicPreprocessorTests::DefineTests()
    {
        // Builtin defines
        TestPreprocessorInput("GL_ES\n",                                                            "1\n");
        TestPreprocessorInput("__VERSION__\n",                                                      "100\n");

        // Define with no substitution
        TestPreprocessorInput("#define FOO\nFOO\n",                                                 "\n \n");
        TestPreprocessorInput("#define FOO\n#ifdef FOO\nBAR\n#endif\n",                             "\n\nBAR\n\n");

        // Basic substitution of various types of single token
        TestPreprocessorInput("#define X Y\nX\n",                                                   "\nY\n");
        TestPreprocessorInput("#define FOO BAR\nFOO\n",                                             "\nBAR\n");
        TestPreprocessorInput("#define FOO 1\nFOO\n",                                               "\n1\n");
        TestPreprocessorInput("#define FOO 1.0\nFOO\n",                                             "\n1.0\n");

        // Basic substitution of various types of multiple token
        TestPreprocessorInput("#define X Y Z\nX\n",                                                 "\nY Z\n");
        TestPreprocessorInput("#define FOO BAR BAZ\nFOO\n",                                         "\nBAR BAZ\n");
        TestPreprocessorInput("#define FOO 1 2\nFOO\n",                                             "\n1 2\n");
        TestPreprocessorInput("#define FOO 1.0 2.0e1\nFOO\n",                                       "\n1.0 2.0e1\n");

        // Multiple spaces become a single space when replacing multi token definitions
        TestPreprocessorInput("#define X Y    Z\nX\n",                                              "\nY Z\n");

        // Definition of a macro should be able to include another macro
        TestPreprocessorInput("#define FOO X\n#define BAR FOO\nFOO BAR\n",                          "\n\nX X\n");

        // Macros should expand and make expressions properly
        TestPreprocessorInput("#define FOO -1\n#if FOO < 0\nBAR\n#endif\n",                         "\n\nBAR\n\n");

        // Inside an ifdef
        TestPreprocessorInput("#ifdef FOO\n#define BAR 1\n#endif\nBAR\n",                           "\n\n\nBAR\n");
        TestPreprocessorInput("#ifdef GL_ES\n#define BAR 1\n#endif\nBAR\n",                         "\n\n\n1\n");

        // Cannot redefine constants
        TestPreprocessorNegative("#define 1 BAR\n1\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define 1.0 BAR\n1.0\n", E_GLSLERROR_SYNTAXERROR);

        // Definitions need a token and comments should work while looking for that token
        TestPreprocessorNegative("#define //\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#define /* */\n", E_GLSLERROR_SYNTAXERROR);

        // Illegal tokens should not be allowed
        TestPreprocessorNegative("#define GL_foo\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define GL_foo 1\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define GL_foo(x)\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define GL_foo(x) x+x\n", E_GLSLERROR_INVALIDMACRONAME);

        TestPreprocessorNegative("#define foo__baz\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define foo__baz 1\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define foo__baz(x)\n", E_GLSLERROR_INVALIDMACRONAME);
        TestPreprocessorNegative("#define foo__baz(x) x+x\n", E_GLSLERROR_INVALIDMACRONAME);

        // Test recursive macros - they should not expand a reference to themselves, so after one expansion
        // they will just output their name again.
        TestPreprocessorInput("#define M1 a+M1+a\nM1\n",                                            "\na + M1 + a\n");
        TestPreprocessorInput("#define M1(x) x+M1(x)+x\nM1(a)\n",                                   "\na + M1 ( a ) + a\n");

        // Test recursion with two macros that reference each other recursively. We should see two expansions here -
        // the first expansion will end up referencing the outer macro, and then will stop in the inner expansion.
        TestPreprocessorInput("#define X Y+1\n#define Y X*2\nX\n",                                  "\n\nX * 2 + 1\n");
        TestPreprocessorInput("#define X(x) Y(a)+x\n#define Y(x) X(b)*x\nX(a)\n",                   "\n\nX ( b ) *  a  + a\n");

        // Now make a macro that references itself and something else that references it. This will also expand twice,
        // but it requires the code checking for the recursion to check all of the macro names on the stack.
        TestPreprocessorInput("#define X Y+X\n#define Y X*Y\nX\n",                                  "\n\nX * Y + X\n");
        TestPreprocessorInput("#define X(x) Y(x)+X(b)\n#define Y(x) X(x)*Y(b)\nX(1)\n",             "\n\nX (  1  ) * Y ( b ) + X ( b )\n");

        // Assign operators that are part of a definition should be output as one token
        TestPreprocessorInput("#define X x >>= x\nX\n",                                             "\nx >>= x\n");
        TestPreprocessorInput("#define X x <<= x\nX\n",                                             "\nx <<= x\n");
        TestPreprocessorInput("#define X x *= x\nX\n",                                              "\nx *= x\n");
        TestPreprocessorInput("#define X x -= x\nX\n",                                              "\nx -= x\n");
        TestPreprocessorInput("#define X x += x\nX\n",                                              "\nx += x\n");
        TestPreprocessorInput("#define X x |= x\nX\n",                                              "\nx |= x\n");
        TestPreprocessorInput("#define X x %= x\nX\n",                                              "\nx %= x\n");
        TestPreprocessorInput("#define X x ^= x\nX\n",                                              "\nx ^= x\n");
        TestPreprocessorInput("#define X x /= x\nX\n",                                              "\nx /= x\n");
        TestPreprocessorInput("#define X x &= x\nX\n",                                              "\nx &= x\n");
        TestPreprocessorInput("#define X x ^^ x\nX\n",                                              "\nx ^^ x\n");
        TestPreprocessorInput("#define X x++\nX\n",                                                 "\nx ++\n");
        TestPreprocessorInput("#define X x--\nX\n",                                                 "\nx --\n");
        TestPreprocessorInput("#define X ++x\nX\n",                                                 "\n++ x\n");
        TestPreprocessorInput("#define X --x\nX\n",                                                 "\n-- x\n");
    }

    void BasicPreprocessorTests::RedefineTests()
    {
        // Define something with nothing, and then redefine in various ways
        TestPreprocessorInput("#define A\nA\n#define A\nA\n",                                       "\n \n\n \n");
        TestPreprocessorNegative("#define A\nA\n#define A 1\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);                    // Different value
        TestPreprocessorNegative("#define A\nA\n#define A(x)\nA(1)\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);                // Different parameter count
        TestPreprocessorNegative("#define A\nA\n#define A(x) x\nA(1)\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);              // Different parameter count and tokens

        // Define something with simple subsitution, and then redefine in various ways
        TestPreprocessorInput("#define A 1\nA\n#define A 1\nA\n",                                   "\n1\n\n1\n");          // Same
        TestPreprocessorNegative("#define A 1\nA\n#define A\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);                    // No value
        TestPreprocessorNegative("#define A 1\nA\n#define A 2\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);                  // Different value
        TestPreprocessorNegative("#define A 1\nA\n#define A(x)\nA(2)\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);              // Different parameter count
        TestPreprocessorNegative("#define A 1\nA\n#define A(x) x\nA(2)\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);            // Different parameter count and tokens

        // Define something with parameters and no subsitution, and then with something else in 4 forms
        TestPreprocessorInput("#define A(x)\nA(1)\n#define A(x)\nA(1)\n",                           "\n\n\n\n");            // Same
        TestPreprocessorNegative("#define A(x)\nA(1)\n#define A\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);                // No value
        TestPreprocessorNegative("#define A(x)\nA(1)\n#define A(x) x\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);           // Same parameters different tokens
        TestPreprocessorNegative("#define A(x)\nA(1)\n#define A(x, y)\nA(1, 2)\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);    // Different parameter count

        // Define something with parameters and subsitution, and then with something else in 4 forms
        TestPreprocessorInput("#define A(x) x\nA(1)\n#define A(x) x\nA(1)\n",                       "\n1\n\n1\n");          // Same
        TestPreprocessorNegative("#define A(x) x\nA(1)\n#define A\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);              // No value
        TestPreprocessorNegative("#define A(x) x\nA(1)\n#define A(x) x+x\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);       // Different tokens
        TestPreprocessorNegative("#define A(x) x\nA(1)\n#define A(x, y) x+y\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);    // Different parameters
        TestPreprocessorNegative("#define A(x) x\nA(1)\n#define A(y) y\nA\n", E_GLSLERROR_MACRODEFINITIONNOTEQUAL);         // Different names
    }

    void BasicPreprocessorTests::UndefineTests()
    {
        // Define with no substitution then undefine
        TestPreprocessorInput("#define FOO\nFOO\n#undef FOO\nFOO\n",                                "\n \n\nFOO\n");
        TestPreprocessorInput("#define FOO\n#ifdef FOO\nBAR\n#endif\n#undef FOO\n#ifdef FOO\nBAR\n#endif\n",
                                                                                                    "\n\nBAR\n\n\n\n\n\n");

        // Define with param then undefine
        TestPreprocessorInput("#define FOO(x) x+1\nFOO(2)\n#undef FOO\nFOO(2)\n",                   "\n2 + 1\n\nFOO(2)\n");
        TestPreprocessorInput("#define FOO(x) x+1\n#ifdef FOO\nBAR\n#endif\n#undef FOO\n#ifdef FOO\nBAR\n#endif\n",
                                                                                                    "\n\nBAR\n\n\n\n\n\n");

        // Undefine of undefined thing should work
        TestPreprocessorInput("#undef FOO\n",                                                       "\n");

        // Need a thing to undefine
        TestPreprocessorNegative("#undef\n", E_GLSLERROR_SYNTAXERROR);

        // Compound define, before and after undefine
        TestPreprocessorInput("#define X\n#ifdef X\nFOO\n#undef X\n#ifdef X\nBAR\n#endif\n#endif\n","\n\nFOO\n\n\n\n\n\n");
    }

    void BasicPreprocessorTests::CommentTests()
    {
        // Replacement should not work in a comment
        TestPreprocessorInput("//GL_ES\n",                                                          "\n");
        TestPreprocessorInput("/*GL_ES*/\n",                                                        "         \n");

        // C comment inside C++ comment
        TestPreprocessorInput("// /* GL_ES */\n",                                                   "\n");

        // Start of C comment ignored, but end not
        TestPreprocessorInput("// /* GL_ES\n*/\n",                                                  "\n*/\n");

        // Nested C comments make failing input
        TestPreprocessorInput("/* /* Inside */ */\n",                                               "                */\n");

        // Comment should not define something
        TestPreprocessorInput("//#define FOO BAR\nFOO\n",                                           "\nFOO\n");
        TestPreprocessorInput("/*#define FOO BAR*/\nFOO\n",                                         "                   \nFOO\n");

        // Comment should work inside a directive
        TestPreprocessorInput("#define FOO BA//R\nFOO\n",                                           "\nBA\n");
        TestPreprocessorInput("#define FOO BA/*R*/\nFOO\n",                                         "\nBA\n");

        // Multiline comments starting and finishing in various places
        TestPreprocessorInput("FOO/*BAR*/\n",                                                       "FOO       \n");
        TestPreprocessorInput("FOO/*BAR\nBAZ*/FUZZ\n",                                              "FOO     \n     FUZZ\n");
        TestPreprocessorInput("#define FOO BAR/*\nFOO BAR*/FOO BAR\n",                              "\n         BAR BAR\n");
        TestPreprocessorInput("#define FOO BAR/*\nFOO*/\n",                                         "\n     \n");
        TestPreprocessorInput("#define FOO BAR/*\nFOO*/#define XX YY\n",                            "\n     \n");
        TestPreprocessorInput("FOO/*BAR\nBAZ*/#define X Y\n",                                       "FOO     \n     \n");
        TestPreprocessorInput("#ifdef GL_ES\n/* Start\nEnd */\n#endif\n",                           "\n        \n      \n\n");

        // C comments inside macro calls and paren
        TestPreprocessorInput("#define F(x) x+1\nF/**/(bar(1.0, 2.0));\n",                          "\nbar(1.0, 2.0) + 1;\n");
        TestPreprocessorInput("#define F(x) x+1\nF(/**/bar(1.0, 2.0));\n",                          "\nbar(1.0, 2.0) + 1;\n");
        TestPreprocessorInput("#define F(x) x+1\nF(bar(1.0, /**/2.0));\n",                          "\nbar(1.0, 2.0) + 1;\n");

        // C++ comments inside macro calls and paren
        TestPreprocessorInput("#define F(x) x+1\nF// Comment\n(bar(1.0, 2.0));\n",                  "\n\nbar(1.0, 2.0) + 1;\n");
        TestPreprocessorInput("#define F(x) x+1\nF(// Comment\nbar(1.0, 2.0));\n",                  "\n\nbar(1.0, 2.0) + 1;\n");
        TestPreprocessorInput("#define F(x) x+1\nF(bar(1.0, // Comment\n2.0));\n",                  "\n\nbar(1.0, 2.0) + 1;\n");

        // Comments should start correctly inside of directives in output disabled code
        TestPreprocessorInput("#ifdef NOTDEF\n#define blah//blahblah\n#endif\n",                    "\n\n\n");
        TestPreprocessorInput("#ifdef NOTDEF\n#define blah/*blahblah*/\n#endif\n",                  "\n\n\n");
        TestPreprocessorInput("#ifdef NOTDEF\n#define blah/*blahblah\nblah*/#endif\n",              "\n\n      \n");
    }

    void BasicPreprocessorTests::DisabledOutputTests()
    {
        // Expressions should not error out with undefined macros when output is disabled
        TestPreprocessorNegative("#if NOTDEF > 1\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#ifdef NOTDEF\n#else\n#if NOTDEF > 1\n#endif\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorInput("#ifdef NOTDEF\n#if NOTDEF > 1\n#endif\n#endif\n",                    "\n\n\n\n");

        // .. or care about expression syntax
        TestPreprocessorNegative("#if GL_ES >\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#ifdef NOTDEF\n#else\n#if GL_ES >\n#endif\n#endif", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorInput("#ifdef NOTDEF\n#if GL_ES >\n#endif\n#endif\n",                       "\n\n\n\n");

        // .. even with elif (note that elif has to be the taken branch for this test to work)
        TestPreprocessorNegative("#ifndef GL_ES\n#elif NOTDEF\n#endif\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorInput("#ifdef NOTDEF\n#ifndef GL_ES\n#elif NOTDEF\n#endif\n#endif\n",       "\n\n\n\n\n");
    }

    void BasicPreprocessorTests::PragmaTests()
    {
        // All pragmas should be ignored, no matter what they have in them - try many different tokens
        TestPreprocessorInput("#pragma FOO BAR\n",                                                  "\n");
        TestPreprocessorInput("#pragma FOO (BAR || BAZ)\n",                                         "\n");
        TestPreprocessorInput("#pragma ifdef (endif || if)\n",                                      "\n");
        TestPreprocessorInput("#pragma defined < 1.0e-4\n",                                         "\n");
        TestPreprocessorInput("#pragma !1 == !2\n",                                                 "\n");
    }

    void BasicPreprocessorTests::CustomErrorTests()
    {
        // The most basic tests of this
        TestPreprocessorNegative("#error\n", E_GLSLERROR_CUSTOM, "Error: ");
        TestPreprocessorNegative("#error test error\n", E_GLSLERROR_CUSTOM, "Error: test error");

        // Right now we don't preserve whitespace
        TestPreprocessorNegative("#error test\terror\n", E_GLSLERROR_CUSTOM, "Error: test error");
        TestPreprocessorNegative("#error test   error\n", E_GLSLERROR_CUSTOM, "Error: test error");

        // You should be able to comment in the error
        TestPreprocessorNegative("#error test error/*not in error*/\n", E_GLSLERROR_CUSTOM, "Error: test error");
        TestPreprocessorNegative("#error test/*not in error*/error\n", E_GLSLERROR_CUSTOM, "Error: test error");
        TestPreprocessorNegative("#error test error// not in error\n", E_GLSLERROR_CUSTOM, "Error: test error");

        // Errors should not happen unless the output is active
        TestPreprocessorInput("#ifdef NOTDEF\n#error This should not happen\n#endif\n",             "\n\n\n");
        TestPreprocessorInput("#ifndef NOTDEF\n#else\n#error This should not happen\n#endif\n",     "\n\n\n\n");
    }

    void BasicPreprocessorTests::ExtensionTests()
    {
        // Try out warn and disable for all
        TestPreprocessorInput("#extension all : warn\n",                                                 "\n");
        TestPreprocessorInput("#extension all : disable\n",                                              "\n");

        // Enable and require don't work for all
        TestPreprocessorNegative("#extension all : enable\n", E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR);
        TestPreprocessorNegative("#extension all : require\n", E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR);

        // Unknown behavior gives an error
        TestPreprocessorNegative("#extension all : unknown\n", E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR);

        // Unknown extensions give an error for require, but not on anything else
        TestPreprocessorNegative("#extension unknown : require\n", E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED);
        TestPreprocessorInput("#extension unknown : enable\n",                                          "\n"); 
        TestPreprocessorInput("#extension unknown : warn\n",                                            "\n");
        TestPreprocessorInput("#extension unknown : disable\n",                                         "\n");

        // The above error cases should not give an error if it occurs where the output is not active
        TestPreprocessorInput("#ifdef NOTDEF\n#extension all : enable\n#endif\n",                       "\n\n\n");
        TestPreprocessorInput("#ifdef NOTDEF\n#extension all : require\n#endif\n",                      "\n\n\n");
        TestPreprocessorInput("#ifdef NOTDEF\n#extension all : unknown\n#endif\n",                      "\n\n\n");
        TestPreprocessorInput("#ifdef NOTDEF\n#extension unknown : require\n#endif\n",                  "\n\n\n");

        // Test known extensions
        TestPreprocessorInput("#extension GL_OES_standard_derivatives : require\n",                     "\n");
    }

    void BasicPreprocessorTests::TokenLimitTests()
    {
        TestPreprocessorLargeTokenNegative("#define %s 1\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("#define %s(x) x\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("#define Foo(%s) bar\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("#if defined(%s)\n#endif\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("%s\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("#pragma %s\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIdentifierToken);
        TestPreprocessorLargeTokenNegative("%s\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIntToken);
        TestPreprocessorLargeTokenNegative("%s\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantFloatToken);
        TestPreprocessorLargeTokenNegative("#if GL_ES == %s\n#endif\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantIntToken);
        TestPreprocessorLargeTokenNegative("#if GL_ES == %s\n#endif\n", E_GLSLERROR_PREMAXTOKEN, &CreateGiantFloatToken);
    }

    void BasicPreprocessorTests::CharacterSetTests()
    {
        // Invalid characters in a comment are allowed
        TestPreprocessorInput("/* $$$$ */\n",                                                       "          \n");

        // Invalid characters outside of a comment are not allowed
        TestPreprocessorNegative("void foo() { int $$$$; }", E_GLSLERROR_INVALIDCHARACTER);
    }

    void BasicPreprocessorTests::LineMacroTests()
    {
        // The most basic cases
        TestPreprocessorInput("__LINE__\n",                                                         "1\n");
        TestPreprocessorInput("\n__LINE__\n",                                                       "\n2\n");
        TestPreprocessorInput("__FILE__\n",                                                         "0\n");
        TestPreprocessorInput("\n__FILE__\n",                                                       "\n0\n");

        // Now use #line
        TestPreprocessorInput("#line 100\n__LINE__\n",                                              "\n100\n");
        TestPreprocessorInput("__LINE__\n#line 100\n__LINE__\n",                                    "1\n\n100\n");
        TestPreprocessorInput("#line 1 234\n__FILE__\n#line 2 123\n__FILE__",                       "\n234\n\n123");

        // Preprocessor errors should have the right location
        TestPreprocessorNegative("#line 100\n#define 1 BAR\n", E_GLSLERROR_SYNTAXERROR, 100);

        // Line must be a decimal
        TestPreprocessorNegative("#line 010\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#line 0x1\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#line 0 010\n", E_GLSLERROR_SYNTAXERROR);
        TestPreprocessorNegative("#line 0 0x1\n", E_GLSLERROR_SYNTAXERROR);
    }

    void BasicPreprocessorTests::TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, int lineNumber, const char* pszErrorExpected)
    {
        UINT inputSize = ::strlen(pszInput);

        TSmartPointer<CGLSLStringParserInput> spInput;
        VERIFY_SUCCEEDED(RefCounted<CGLSLStringParserInput>::Create(pszInput, inputSize, /*out*/spInput));        

        TSmartPointer<CTestErrorSink> spErrorSink;
        VERIFY_SUCCEEDED(RefCounted<CTestErrorSink>::Create(/*out*/spErrorSink));

        TSmartPointer<CMemoryStream> spStream;
        TSmartPointer<CGLSLLineMap> spLineMap;
        TSmartPointer<CGLSLExtensionState> spExtensionState;
        VERIFY_FAILED(::GLSLPreprocess(spInput, spErrorSink, GLSLTranslateOptions::EnableStandardDerivatives, GLSLShaderType::Fragment, &spStream, &spLineMap, &spExtensionState));

        // The first error should be the one provided
        VERIFY_IS_TRUE(spErrorSink->GetErrorCount() > 0);
        if (spErrorSink->GetErrorCount() > 0)
        {
            VERIFY_ARE_EQUAL(spErrorSink->UseError(0)->GetCode(), hrExpected);

            if (lineNumber != -1)
            {
                VERIFY_ARE_EQUAL(spErrorSink->UseError(0)->GetLine(), lineNumber);
            }

            if (pszErrorExpected != nullptr)
            {
                VERIFY_ARE_EQUAL(::strcmp(spErrorSink->UseError(0)->GetText(), pszErrorExpected), 0);
            }
        }
    }

    void BasicPreprocessorTests::TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, int lineNumber)
    {
        TestPreprocessorNegative(pszInput, hrExpected, lineNumber, nullptr);
    }

    void BasicPreprocessorTests::TestPreprocessorNegative(char* pszInput, HRESULT hrExpected, const char* pszErrorExpected)
    {
        TestPreprocessorNegative(pszInput, hrExpected, -1, pszErrorExpected);
    }

    void BasicPreprocessorTests::TestPreprocessorNegative(char* pszInput, HRESULT hrExpected)
    {
        TestPreprocessorNegative(pszInput, hrExpected, -1);
    }

    void BasicPreprocessorTests::TestPreprocessorLargeTokenNegative(char* pszInput, HRESULT hrExpected, PFNCreateToken pfnCreateToken)
    {
        CMutableString<char> strBigIdent;

        // Create the given token
        VERIFY_SUCCEEDED((*pfnCreateToken)(/*inout*/strBigIdent));

        // Format into the given string
        CMutableString<char> strFormatted;
        VERIFY_SUCCEEDED(strFormatted.Set(pszInput));
        VERIFY_SUCCEEDED(strFormatted.Format(1024, pszInput, static_cast<const char *>(strBigIdent)));

        TestPreprocessorNegative(static_cast<char*>(strFormatted), hrExpected);
    }

    void BasicPreprocessorTests::TestPreprocessorInput(char* pszInput, const char* pszExpected)
    {
        UINT inputSize = ::strlen(pszInput);

        TSmartPointer<CGLSLStringParserInput> spInput;
        VERIFY_SUCCEEDED(RefCounted<CGLSLStringParserInput>::Create(pszInput, inputSize, /*out*/spInput));        

        TSmartPointer<CTestErrorSink> spErrorSink;
        VERIFY_SUCCEEDED(RefCounted<CTestErrorSink>::Create(/*out*/spErrorSink));

        TSmartPointer<CMemoryStream> spStream;
        TSmartPointer<CGLSLLineMap> spLineMap;
        TSmartPointer<CGLSLExtensionState> spExtensionState;
        HRESULT hr = ::GLSLPreprocess(spInput, spErrorSink, GLSLTranslateOptions::EnableStandardDerivatives, GLSLShaderType::Fragment, &spStream, &spLineMap, &spExtensionState);
        VERIFY_SUCCEEDED(hr);
        if (hr == S_OK)
        {
            CMutableString<char> spOutput;
            VERIFY_SUCCEEDED(spStream->ExtractString(spOutput));

            int firstDiff;
            VERIFY_IS_TRUE(CompareOutputs(pszExpected, spOutput, &firstDiff));
            VERIFY_ARE_EQUAL(firstDiff, -1);
            if (firstDiff != -1)
            {
                WCHAR szConverted[2048];
                szConverted[0] = L'\0';
                int length = ::MultiByteToWideChar(CP_UTF8, 0, spOutput, spOutput.GetLength(), szConverted, ARRAYSIZE(szConverted));
                if (length != 0)
                {
                    if (length < ARRAYSIZE(szConverted))
                    {
                        szConverted[length] = L'\0';
                        Log::Comment(L"actual:\n");
                        Log::Comment(szConverted);
                        Log::Comment(L"end\n");
                    }
                    else
                    {
                        Log::Error(L"MultiByteToWideChar required a buffer larger than 2048! Can't output actual result");
                    }
                }
                else
                {
                    Log::Error(L"Failed to convert translated shadercode to wide char");
                }
            }
        }
    }
}
