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
#include "BasicGLSLTests.hxx"
#include "GLSLTranslate.hxx"
#include "GLSLIdentifierTable.hxx"
#include "GLSLUnicodeConverter.hxx"
#include "GLSLConvertedShader.hxx"
#include "ParserTestUtils.hxx"
#include "GLSLTranslateOptions.hxx"
#include "WebGLFeatureLevel.hxx"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace ft_glslparse
{
    void BasicGLSLTests::ShaderVerificationTests()
    {
        // Construct a big shader to test rejection based on size
        CMutableString<wchar_t> strBigShader(128000);

        for (int i = 0; i < 16000; i++)
        {
            if (FAILED(strBigShader.Append(L"\n\n\n\n\n\n\n\n\n")))
            {
                VERIFY_FAILED(E_FAIL, L"Cannot allocate big shader string");
            }
        }

        VERIFY_SUCCEEDED(strBigShader.Append(L"void foo() { int bar; }\n"));

        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, strBigShader, E_GLSLERROR_SHADERTOOLONG);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strBigShader, E_GLSLERROR_SHADERTOOLONG);
    }

    void BasicGLSLTests::BasicDeclarationTests()
    {
        // Test a basic declaration
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar; }",                                                                         "void fn_0_0()\n{\nint var_1_1=0;\n}\n");

        // Test multiple identifiers
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar, baz, bob; }",                                                               "void fn_0_0()\n{\nint var_1_1=0, var_1_2=0, var_1_3=0;\n}\n");

        // Test identifier with same name as intrinsic
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int distance; }",                                                                    "void fn_0_0()\n{\nint vark_1_29=0;\n}\n");

        // Test initializer on a single declaration
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 1; }",                                                                     "void fn_0_0()\n{\nint var_1_1=1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");

        // Test initializer on a single declaration in a list
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 1, baz, bob; }",                                                           "void fn_0_0()\n{\nint var_1_1=1, var_1_2=0, var_1_3=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar, baz = 1, bob; }",                                                           "void fn_0_0()\n{\nint var_1_1=0, var_1_2=1, var_1_3=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar, baz, bob = 1; }",                                                           "void fn_0_0()\n{\nint var_1_1=0, var_1_2=0, var_1_3=1;\n}\n");

        // Test initializer on all declarations in a list
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 1, baz = 2, bob = 3; }",                                                   "void fn_0_0()\n{\nint var_1_1=1, var_1_2=2, var_1_3=3;\n}\n");

        // Test initializer with expression list
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar; float baz; bar = (baz = 2.0, 1); }",                                        "void fn_0_0()\n{\nint var_1_1=0;\nfloat var_1_2=0.0;\nvar_1_1=(var_1_2=2.000000e+000,1);\n}\n");

        // Initializer with RHS being another identifier
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar; float baz = bar; }",                                                      "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=var_1_1;\n}\n");

        // Array expression must be constant
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int i = (false) ? 1 : 2; int j[i];", E_GLSLERROR_ARRAYDECLARATORNOTCONST);

        // ... Which includes constant constructors
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[int(5.0)]; }",                                                               "void fn_0_0()\n{\nint var_1_1[5]=(int[5])0;\n}\n");

        // Arrays can't be declared with <= 0 size
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int j[0];", E_GLSLERROR_ARRAYSIZELEQZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int j[-1];", E_GLSLERROR_ARRAYSIZELEQZERO);

        // void typed variables are not allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { void v[3]; }", E_GLSLERROR_VARIABLETYPEVOID);
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   VariableQualifierDeclarationTests
    //
    //  Synopsis:   Tests declarations of variables with both precision and
    //              type qualifiers.
    //
    //-----------------------------------------------------------------------------
    void BasicGLSLTests::VariableQualifierDeclarationTests()
    {
        // Make sure const works with both global and local
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { const float bar = 0.0; }",                                                           "void fn_0_0()\n{\nconst float var_1_1=0.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"const float bar = 0.0;",                                                                          "static const float var_0_0=0.000000e+000;\n");

        // Make sure const works with a decl list
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { const float bar = 0.0, baz = 1.0, qux = 2.0; }",                                      "void fn_0_0()\n{\nconst float var_1_1=0.000000e+000, var_1_2=1.000000e+000, var_1_3=2.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"const float bar = 0.0, foo = 1.0, baz = 2.0;",                                                     "static const float var_0_0=0.000000e+000, var_0_1=1.000000e+000, var_0_2=2.000000e+000;\n");

        // ... But not with arrays
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const float bar[1]; }", E_GLSLERROR_INVALIDARRAYQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const float bar[1];", E_GLSLERROR_INVALIDARRAYQUALIFIER);

        // ... And not without an initializer on every variable
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const float bar; }", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const float bar;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const lowp float bar;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const float foo, bar = 0.0;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const float foo = 0.0, bar;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const float foo, bar = 0.0, baz;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);

        // Make sure we can init const things with other const things

        // Test both global and local for this case while short circuiting is getting working
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"void foo() { const bool x = true && true && true; }",                                                   "void fn_0_0()\n{\nbool var_1_5=false;\nbool var_1_6=false;\nif (true){\nvar_1_6=true;\n}\nelse\n{\nvar_1_6=false;\n}\nif (var_1_6){\nvar_1_5=true;\n}\nelse\n{\nvar_1_5=false;\n}\nconst bool var_1_1=var_1_5;\n}\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"bool a; bool x = a && true && true; void foo() {}",                                                    "static bool var_0_0=false;\nstatic bool var_0_1=false;\nvoid fn_0_6();\nvoid fn_0_2()\n{\nfn_0_6();\n}\nvoid fn_0_6()\n{\nbool var_2_7=false;\nbool var_2_8=false;\nif (var_0_0){\nvar_2_8=true;\n}\nelse\n{\nvar_2_8=false;\n}\nif (var_2_8){\nvar_2_7=true;\n}\nelse\n{\nvar_2_7=false;\n}\nvar_0_1=var_2_7;\n}\n");

        // Short-circuits and conditionals that are part of a global const declaration initialization expression
        // don't need to be short cirtuited as there are no side effects
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const bool x = true && true && true; void foo() {}",                                                    "static const bool var_0_0=true&&true&&true;\nvoid fn_0_1()\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const bool x = true ? false : true; void foo() {}",                                                     "static const bool var_0_0=true?false:true;\nvoid fn_0_1()\n{\n}\n");

        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 0; const int y = x;",                                                                     "static const int var_0_0=0;\nstatic const int var_0_1=var_0_0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 0; const int y = -x;",                                                                    "static const int var_0_0=0;\nstatic const int var_0_1=(-var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 0; const int y = (x + x);",                                                               "static const int var_0_0=0;\nstatic const int var_0_1=(var_0_0+var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const float x = 0.0; const float y = (x + x);",                                                         "static const float var_0_0=0.000000e+000;\nstatic const float var_0_1=(var_0_0+var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const bool x = false; const bool y = !x;",                                                              "static const bool var_0_0=false;\nstatic const bool var_0_1=(!var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const ivec2 x = ivec2(0, 0); const int y = x.x;",                                                       "static const int2 var_0_0=int2(0,0);\nstatic const int var_0_1=var_0_0.x;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const ivec2 x = ivec2(0, 0); const int y = -x.x;",                                                      "static const int2 var_0_0=int2(0,0);\nstatic const int var_0_1=(-var_0_0.x);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const ivec2 x = ivec2(0, 0); const int y = x[0];",                                                      "static const int2 var_0_0=int2(0,0);\nstatic const int var_0_1=var_0_0[0];\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const ivec2 x = ivec2(1, 1); const int y = x[0] * x[1];",                                               "static const int2 var_0_0=int2(1,1);\nstatic const int var_0_1=var_0_0[0]*var_0_0[1];\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 0; const int y = (x, x);",                                                                "static const int var_0_0=0;\nstatic const int var_0_1=(var_0_0,var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"int nc; const int x = 0; const int y = (nc, x);",                                                       "static int var_0_0=0;\nstatic const int var_0_1=0;\nstatic const int var_0_2=(var_0_0,var_0_1);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 0; const int y = int(x);",                                                                "static const int var_0_0=0;\nstatic const int var_0_1=int(var_0_0);\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"struct structDef {int bar;}; const structDef foo = structDef(1); const int y = foo.bar;",               "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\nstatic const typename_0_0 var_0_2=ctor_typename_0_0(1);\nstatic const int var_0_3=var_0_2.var_1_1;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const bool f = true; const int a = 1, b = 2; const int y = (f) ? a : b;",                               "static const bool var_0_0=true;\nstatic const int var_0_1=1, var_0_2=2;\nstatic const int var_0_3=(var_0_0)?var_0_1:var_0_2;\n");

        // Test making arrays with the size from those const things
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 1; const int y = x; int a[y];",                                                           "static const int var_0_0=1;\nstatic const int var_0_1=var_0_0;\nstatic int var_0_2[1]=(int[1])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = -1; const int y = -x; int a[y];",                                                         "static const int var_0_0=(-1);\nstatic const int var_0_1=(-var_0_0);\nstatic int var_0_2[1]=(int[1])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 1; const int y = (x + x); int a[y];",                                                     "static const int var_0_0=1;\nstatic const int var_0_1=(var_0_0+var_0_0);\nstatic int var_0_2[2]=(int[2])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const float x = 1.0; const float y = (x + x); int a[int(y)];",                                          "static const float var_0_0=1.000000e+000;\nstatic const float var_0_1=(var_0_0+var_0_0);\nstatic int var_0_2[2]=(int[2])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 1; const int y = (x, x); int a[y];",                                                      "static const int var_0_0=1;\nstatic const int var_0_1=(var_0_0,var_0_0);\nstatic int var_0_2[1]=(int[1])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"int nc; const int x = 1; const int y = (nc, x); int a[y];",                                             "static int var_0_0=0;\nstatic const int var_0_1=1;\nstatic const int var_0_2=(var_0_0,var_0_1);\nstatic int var_0_3[1]=(int[1])0;\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"const int x = 1; const int y = int(x); int a[y];",                                                      "static const int var_0_0=1;\nstatic const int var_0_1=int(var_0_0);\nstatic int var_0_2[1]=(int[1])0;\n");

        // Test that doing binary expressions with non-int and non-float do not assert
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const int i = (true == false);", E_GLSLERROR_INCOMPATIBLETYPES);

        // Some constant expressions cannot be evaluated to make arrays - try anyway to exercise the code
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const bool x = false; const bool y = !x; int a[int(y)];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const bool x = true; const float y = float(x); int a[int(y)];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int a[ivec2(1, 1).x];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int a[int(true)];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int a[int(!true)];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int a[int(bool(true))];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const ivec2 x = ivec2(1, 1); const int y = x.x; int a[y];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const ivec2 x = ivec2(1, 1); const int y = x[0]; int a[y];", E_GLSLERROR_CANNOTCALCARRAYSIZE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct structDef {int bar;}; const structDef foo = structDef(1); const int y = foo.bar; int a[y];", E_GLSLERROR_CANNOTCALCARRAYSIZE);

        // Make sure that assigning non-const things to const things does not work, and try various expressions to stress this
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x = 0; const int y = x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x = 0; const int y = -x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x = 0; const int y = (x + x);", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float x = 0.0; const float y = (x + x);", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool x = false; const bool y = !x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x[2]; const int y = x[0];", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"ivec2 x; const int y = x.x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"ivec2 x; const int y = x[0];", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x; const int y = x, x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x() { return 0; } const int y = x();", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x; const int y = int(x);", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct structDef {int bar;}; structDef foo = structDef(1); const int y = foo.bar;", E_GLSLERROR_NONCONSTINITIALIZER);  
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x; const int y = (true) ? 1 : x;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int x; const int y = (true) ? x : 1;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool f = true; const int y = (f) ? 1 : 0;", E_GLSLERROR_NONCONSTINITIALIZER);

        // Function params with const are not const expressions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const int x) { const int y = x; }", E_GLSLERROR_NONCONSTINITIALIZER);

        // Make sure precision is accepted with both global and local - currently this does not output any HLSL equivalent precision
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { lowp float bar; }",                                                                  "void fn_0_0()\n{\nfloat var_1_1=0.0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"lowp float bar;",                                                                                 "static float var_0_0=0.0;\n");

        // Make sure precision and const are accepted with both global and local
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { const lowp float bar = 0.0; }",                                                      "void fn_0_0()\n{\nconst float var_1_1=0.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"const lowp float bar = 0.0;",                                                                     "static const float var_0_0=0.000000e+000;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"const lowp float foo = 0.0, bar = 1.0, baz = 2.0;",                                               "static const float var_0_0=0.000000e+000, var_0_1=1.000000e+000, var_0_2=2.000000e+000;\n");

        // Make sure precision and const are not accepted in the wrong order
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { lowp const float bar = 0.0; }", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"lowp const float bar = 0.0;", E_GLSLERROR_SYNTAXERROR);

        // Make sure you cannot init an float with an int or vice versa
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a = 1; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a = 1.0; }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Make sure you cannot make locals that are attributes or varying
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { varying float a; }", E_GLSLERROR_INVALIDNONGLOBALQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { attribute float a; }", E_GLSLERROR_INVALIDNONGLOBALQUALIFIER);

        // Make sure you cannot initialize variables that are uniform, varying, or attribute
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"attribute vec3 attr1 = vec3(0.0);", E_GLSLERROR_INVALIDINITQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform float uniFloat = 0.0;", E_GLSLERROR_INVALIDINITQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"varying vec2 varyVec = vec2(0.0);", E_GLSLERROR_INVALIDINITQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform lowp float uniFloat = 0.0;", E_GLSLERROR_INVALIDINITQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying lowp vec2 varyVec = vec2(0.0);", E_GLSLERROR_INVALIDINITQUALIFIER);

        // Varyings can only be of types that are made up of float components. Positive tests with HLSL verification are in VaryingDeclarationTests
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying int var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying ivec2 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying ivec3 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying ivec3 var[2];", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying ivec4 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying bool var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying bool var[3];", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying bvec2 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying bvec3 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying bvec4 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);

        // Attributes also can only be of types that are made up of float components. Positive tests with HLSL verification are in VaryingDeclarationTests
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute int var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute ivec2 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute ivec3 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute ivec3 var[2];", E_GLSLERROR_INVALIDARRAYQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute ivec4 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute bool var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute bool var[3];", E_GLSLERROR_INVALIDARRAYQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute bvec2 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute bvec3 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute bvec4 var;", E_GLSLERROR_INVALIDINPUTDATATYPE);
    }

    void BasicGLSLTests::LValueTests()
    {
        // Test things that should be valid lValues - start with regular variables
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x; x = 1.0; }",                                                                "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x; x *= 1.0; }",                                                               "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1*=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x; x++; }",                                                                    "void fn_0_0()\n{\nfloat var_1_1=0.0;\n(var_1_1++);\n}\n");

        // Arrays that have indexer
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x[1]; x[0] = 1.0; }",                                                          "void fn_0_0()\n{\nfloat var_1_1[1]=(float[1])0;\nvar_1_1[0]=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x[1]; x[0] += 1.0; }",                                                         "void fn_0_0()\n{\nfloat var_1_1[1]=(float[1])0;\nvar_1_1[0]+=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x[1]; x[0]--; }",                                                              "void fn_0_0()\n{\nfloat var_1_1[1]=(float[1])0;\n(var_1_1[0]--);\n}\n");

        // Swizzles without repeats
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.x = 1.0; }",                                                               "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.x=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.y = 1.0; }",                                                               "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.y=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; ++x.x; }",                                                                   "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\n(++var_1_1.x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.xy = vec2(1.0, 1.0); }",                                                   "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.xy=float2(1.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.x /= 1.0; }",                                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.x/=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.y -= 1.0; }",                                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.y-=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; x.xy *= vec2(1.0, 1.0); }",                                                  "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1.xy*=float2(1.000000e+000,1.000000e+000);\n}\n");

        // Assignments are not l-values, but you can chain them since the grammar defines precedence to essentially make the AST for something like
        //      x = y = z;
        // look like
        //      x = (y = z);
        // where y = z is actually an r-value
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x, y; x = y = 1.0; }",                                                         "void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nvar_1_1=var_1_2=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float x, y; x = y *= 1.0; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nvar_1_1=var_1_2*=1.000000e+000;\n}\n");

        // Make sure assignment used as l-value is marked as an error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float x; (x = 1.0)++; }", E_GLSLERROR_INVALIDLVALUE);

        // Now make sure things that should not be valid are flagged - start with const variables and constants
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const float x = 0.0; x = 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { 2.0 = 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const float x = 0.0; x += 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { 2.0 *= 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const float x = 0.0; x++; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const float x) { x = 1.0; }", E_GLSLERROR_INVALIDLVALUE);
 
        // Now test swizzle with repeats
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.xx = vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.yy = vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.zz = vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.ww = vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.xx *= vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.yy /= vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.zz += vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; x.ww -= vec2(1.0, 1.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 x; --x.xx; }", E_GLSLERROR_INVALIDLVALUE);

        // Function call
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float test() { return 0.0; } void foo() { test() = 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float test() { return 0.0; } void foo() { test() *= 1.0; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float test() { return 0.0; } void foo() { test()--; }", E_GLSLERROR_INVALIDLVALUE);

        // Assignment expression that is not straight assignment
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float x, y; x *= 2.0 = y; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float x, y; x *= 2.0 *= y; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float x; (x *= 2.0)++; }", E_GLSLERROR_INVALIDLVALUE);

        // Don't write to readonly builtin variables
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void main() { gl_FrontFacing = false; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void main() { gl_PointCoord = vec2(0.0, 0.0); }", E_GLSLERROR_INVALIDLVALUE);

        // Varying values should be read-only in the fragment shader
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying lowp vec3 foo; void main() { foo = vec3(0.0); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying lowp vec3 foo; void main() { foo.x = 1.0; }", E_GLSLERROR_INVALIDLVALUE);

        // Uniforms are not valid lval's
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform bool foo; void main() { foo = true; }", E_GLSLERROR_INVALIDLVALUE);

        // Attributes are not valid lval's
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"attribute vec3 foo; void main() { foo.z = 1.0; }", E_GLSLERROR_INVALIDLVALUE);

        // Out parameters have to be lvalues
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void outfunc(out float ret) { ret = 1.0; } void foo() { const float x = 0.0; outfunc(x); }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void outfunc(inout float ret) { ret = 1.0; } void foo() { const float x = 0.0; outfunc(x); }", E_GLSLERROR_INVALIDLVALUE);
    }

    void BasicGLSLTests::DefaultInitTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar; }",                                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 bar; }",                                                                        "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 bar; }",                                                                        "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 bar; }",                                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar; }",                                                                         "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 bar; }",                                                                       "void fn_0_0()\n{\nint2 var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 bar; }",                                                                       "void fn_0_0()\n{\nint3 var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 bar; }",                                                                       "void fn_0_0()\n{\nint4 var_1_1=0;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bool bar; }",                                                                        "void fn_0_0()\n{\nbool var_1_1=false;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 bar; }",                                                                       "void fn_0_0()\n{\nbool2 var_1_1=false;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 bar; }",                                                                       "void fn_0_0()\n{\nbool3 var_1_1=false;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 bar; }",                                                                       "void fn_0_0()\n{\nbool4 var_1_1=false;\n}\n");
    }

    void BasicGLSLTests::IntLiteralTests()
    {
        // Things that should work - decimal
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0; }",                                                                     "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 1; }",                                                                     "void fn_0_0()\n{\nint var_1_1=1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 10; }",                                                                    "void fn_0_0()\n{\nint var_1_1=10;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 10000; }",                                                                 "void fn_0_0()\n{\nint var_1_1=10000;\n}\n");

        // Things that should work - octal
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0; }",                                                                     "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 01; }",                                                                    "void fn_0_0()\n{\nint var_1_1=1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 010; }",                                                                   "void fn_0_0()\n{\nint var_1_1=8;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0100; }",                                                                  "void fn_0_0()\n{\nint var_1_1=64;\n}\n");

        // Things that should work - hex
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0x0; }",                                                                   "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0x1; }",                                                                   "void fn_0_0()\n{\nint var_1_1=1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0x10; }",                                                                  "void fn_0_0()\n{\nint var_1_1=16;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar = 0x100; }",                                                                 "void fn_0_0()\n{\nint var_1_1=256;\n}\n");

        // Things that should not - octal literals cannot have 8 and 9 in them
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int bar = 08; }", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int bar = 09; }", E_GLSLERROR_SYNTAXERROR);
    }

    void BasicGLSLTests::FloatLiteralTests()
    {
        // Test non-exponent literals
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.; }",                                                                  "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = .1; }",                                                                  "void fn_0_0()\n{\nfloat var_1_1=1.000000e-001;\n}\n");

        // Test exponent literals with no decimal point (both cases of 'e', both signs and no sign)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1e1; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1e+1; }",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1e-1; }",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e-001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1E1; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1E+1; }",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1E-1; }",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e-001;\n}\n");

        // Test large exponent
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 10000000e-11; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=1.000000e-004;\n}\n");

        // Test exponent literals with a decimal point (lower case e, no sign)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.e1;}",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = .1e1;}",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0e1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 0.1e1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");

        // Test exponent literals with a decimal point (upper case e, no sign)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.E1;}",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = .1E1;}",                                                                 "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0E1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 0.1E1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");

        // Test exponent literals with a decimal point (lower case e, - sign)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.e-1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e-001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = .1e-1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e-002;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0e-1;}",                                                               "void fn_0_0()\n{\nfloat var_1_1=1.000000e-001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 0.1e-1;}",                                                               "void fn_0_0()\n{\nfloat var_1_1=1.000000e-002;\n}\n");

        // Test exponent literals with a decimal point (upper case e, + sign)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.E+1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = .1E+1;}",                                                                "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 1.0E+1;}",                                                               "void fn_0_0()\n{\nfloat var_1_1=1.000000e+001;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float bar = 0.1E+1;}",                                                               "void fn_0_0()\n{\nfloat var_1_1=1.000000e+000;\n}\n");

        // Values from conformance that are interesting
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float posInRange = 4611686018427387903.;",                                                  "static float var_0_0=4.611686e+018;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float posOutRange = 4611686018427387905.;",                                                 "static float var_0_0=4.611686e+018;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float posHuge = 1E100;",                                                                    "static float var_0_0=1.000000e+100;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float negInRange = -4611686018427387903.;",                                                 "static float var_0_0=(-4.611686e+018);\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float negOutRange = -4611686018427387905.;",                                                "static float var_0_0=(-4.611686e+018);\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"highp float negHuge = -1E100;",                                                                   "static float var_0_0=(-1.000000e+100);\n");
    }

    void BasicGLSLTests::ArrayDeclarationTests()
    {
        // Some simple declarations, including in lists
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[1]; }",                                                                      "void fn_0_0()\n{\nint var_1_1[1]=(int[1])0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float foo, bar[2]; }",                                                               "void fn_0_0()\n{\nfloat var_1_0=0.0, var_1_1[2]=(float[2])0;\n}\n");

        // Some expressions in the declarations
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[2-1]; }",                                                                    "void fn_0_0()\n{\nint var_1_1[1]=(int[1])0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[2+1]; }",                                                                    "void fn_0_0()\n{\nint var_1_1[3]=(int[3])0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[2*3]; }",                                                                    "void fn_0_0()\n{\nint var_1_1[6]=(int[6])0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[8/3]; }",                                                                    "void fn_0_0()\n{\nint var_1_1[2]=(int[2])0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[2+(3-2)]; }",                                                                "void fn_0_0()\n{\nint var_1_1[3]=(int[3])0;\n}\n");

        // We know samplers are special, so try these variations with samplers too
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2-1];",                                                                 "SamplerState sampler_0[1]:register(s[0]);\nTexture2D<float4> texture_0[1]:register(t[0]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2+1];",                                                                 "SamplerState sampler_0[3]:register(s[0]);\nTexture2D<float4> texture_0[3]:register(t[0]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2*3];",                                                                 "SamplerState sampler_0[6]:register(s[0]);\nTexture2D<float4> texture_0[6]:register(t[0]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[8/3];",                                                                 "SamplerState sampler_0[2]:register(s[0]);\nTexture2D<float4> texture_0[2]:register(t[0]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2+(3-2)];",                                                             "SamplerState sampler_0[3]:register(s[0]);\nTexture2D<float4> texture_0[3]:register(t[0]);\n");

        // Test using indices
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int bar[3]; int baz = bar[0]; }",                                                    "void fn_0_0()\n{\nint var_1_1[3]=(int[3])0;\nint var_1_2=var_1_1[0];\n}\n");

        // Arrays in the function parameters
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int bar[1]) {}",                                                                         "void fn_0_0(int var_1_1[1])\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int [1]) {}",                                                                            "void fn_0_0(int anonarg_0[1])\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int bar[2*3]) {}",                                                                       "void fn_0_0(int var_1_1[6])\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int bar[2]) { int baz = bar[1]; }",                                                      "void fn_0_0(int var_1_1[2])\n{\nint var_1_2=var_1_1[1];\n}\n");

        // Negative or zero function parameter arrays
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int bar[-1]) {}", E_GLSLERROR_ARRAYSIZELEQZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int bar[0]) {}", E_GLSLERROR_ARRAYSIZELEQZERO);

        // Call a function with array parameters
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int bar[2]) { } void baz() { int boo[2]; foo(boo); }",                                   "void fn_0_0(int var_1_1[2])\n{\n}\nvoid fn_0_2()\n{\nint var_2_3[2]=(int[2])0;\nfn_0_0(var_2_3);\n}\n");

        // Call a function with wrong size array parameters
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int bar[2]) { } void baz() { int boo[3]; foo(boo); }", E_GLSLERROR_INVALIDARGUMENTS);

        // Out of range index gives error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int bar[3]; int baz = bar[4]; }", E_GLSLERROR_INDEXOUTOFRANGE);

        // Multi-dimension not supported
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo[1][2];", E_GLSLERROR_SYNTAXERROR);

        // Not all expressions are allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo[2 > 1 ? 8 : 9];", E_GLSLERROR_CANNOTCALCARRAYSIZE);

        // You cannot index things that are not indexable
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo() { int bar; return bar[0]; }", E_GLSLERROR_INVALIDINDEXTARGET);

        // Test that we've detected the maximum number of uniforms has been reached.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform float _fl1[10000];", E_GLSLERROR_MAXUNIFORMEXCEEDED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform float _fl1[2147483647]; uniform float _fl2[2147483647]; uniform float _flOverflowTo1[2];", E_GLSLERROR_MAXUNIFORMEXCEEDED);

        // Run the same test, but this time in a declaration list and overflow as part of the list count
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform float _fl1[2147483647], _fl2[2147483647], _flOverflowTo1[2];", E_GLSLERROR_MAXUNIFORMEXCEEDED);

        // Test that we've detected the maximum number of varyings has been reached.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"varying float _fl1[10000]; void foo() { _fl1[0]; }", E_GLSLERROR_MAXVARYINGEXCEEDED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"varying float _fl1[2147483647]; varying float _fl2[2147483647]; varying float _flOverflowTo1[2]; void foo() { _fl1[0] + _fl2[0] + _flOverflowTo1[0]; }", E_GLSLERROR_MAXVARYINGEXCEEDED);

        // Run the same test on varyings, but this time in a declaration list and overflow as part of the list count
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying lowp float _fl1[2147483647], _fl2[2147483647], _flOverflowTo1[2]; void foo() { _fl1[0] + _fl2[0] + _flOverflowTo1[0]; }", E_GLSLERROR_MAXVARYINGEXCEEDED);

        // Unused varyings are not statically counted against the max
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"varying mediump float _fl1[2147483647], _fl2[2147483647], _flOverflowTo1[2];",                    "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\n");

        // Divide by zero should be a nice error, not a crash
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo[1 / 0];", E_GLSLERROR_DIVIDEORMODBYZERO);

        // Integer overflow a negative signed integer
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo[-2147483648 / -1];", E_GLSLERROR_DIVIDEORMODBYZERO);
    }

    void BasicGLSLTests::ScalarConstructorTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nbool2 var_1_1=false;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nbool3 var_1_1=false;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nbool4 var_1_1=false;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nint2 var_1_1=0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nint3 var_1_1=0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 x; float f = float(x); }",                                                     "void fn_0_0()\n{\nint4 var_1_1=0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat var_1_2=float((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat var_1_2=float((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat var_1_2=float((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; float f = float(x); }",                                                      "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat var_1_2=float((var_1_1)._m00);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nbool2 var_1_1=false;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nbool3 var_1_1=false;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nbool4 var_1_1=false;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nint2 var_1_1=0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nint3 var_1_1=0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 x; int i = int(x); }",                                                         "void fn_0_0()\n{\nint4 var_1_1=0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nint var_1_2=int((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nint var_1_2=int((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nint var_1_2=int((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; int i = int(x); }",                                                          "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nint var_1_2=int((var_1_1)._m00);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nbool2 var_1_1=false;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nbool3 var_1_1=false;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nbool4 var_1_1=false;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nint2 var_1_1=0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nint3 var_1_1=0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 x; bool i = bool(x); }",                                                       "void fn_0_0()\n{\nint4 var_1_1=0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nbool var_1_2=bool((var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nbool var_1_2=bool((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nbool var_1_2=bool((var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; bool i = bool(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nbool var_1_2=bool((var_1_1)._m00);\n}\n");

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float f = float(1, 2); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i = int(1, 2); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bool b = bool(1, 2); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform sampler2D s; void foo() { float f = float(s); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform samplerCube s; void foo() { int i = int(s);}", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform samplerCube s; void foo() { bool b = int(s);}", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bool b = bool();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i = int();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float f = float();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { sampler2D s = sampler2D(); }", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { samplerCube s = samplerCube(); }", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
    }

    void BasicGLSLTests::VectorConstructorTests()
    {
        // Pass varying types to a constructor
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { lowp vec4 v = vec4(1.0, 1., .0, 1); }",                                              "void fn_0_0()\n{\nfloat4 var_1_1=float4(1.000000e+000,1.000000e+000,0.000000e+000,1);\n}\n");

        // Test passing single component to vector constructor
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 v = vec2(1.0); }",                                                              "void fn_0_0()\n{\nfloat2 var_1_1=float2((1.000000e+000).xx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 v = vec3(1.0); }",                                                              "void fn_0_0()\n{\nfloat3 var_1_1=float3((1.000000e+000).xxx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 v = vec4(1.0); }",                                                              "void fn_0_0()\n{\nfloat4 var_1_1=float4((1.000000e+000).xxxx);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 v = ivec2(1); }",                                                              "void fn_0_0()\n{\nint2 var_1_1=int2((1).xx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 v = ivec3(1); }",                                                              "void fn_0_0()\n{\nint3 var_1_1=int3((1).xxx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 v = ivec4(1); }",                                                              "void fn_0_0()\n{\nint4 var_1_1=int4((1).xxxx);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 v = bvec2(true); }",                                                           "void fn_0_0()\n{\nbool2 var_1_1=bool2((true).xx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 v = bvec3(true); }",                                                           "void fn_0_0()\n{\nbool3 var_1_1=bool3((true).xxx);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 v = bvec4(true); }",                                                           "void fn_0_0()\n{\nbool4 var_1_1=bool4((true).xxxx);\n}\n");

        // Test passing larger vector to smaller one
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 v = vec3(vec4(1.0)); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=float3((float4((1.000000e+000).xxxx)).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 v = vec2(vec3(1.0)); }",                                                        "void fn_0_0()\n{\nfloat2 var_1_1=float2((float3((1.000000e+000).xxx)).xy);\n}\n");

        // Construct from the same type
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec2 y = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=float2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec3 y = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=float3(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec4 y = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=float4(var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec2 x; ivec2 y = ivec2(x); }",                                                     "void fn_0_0()\n{\nint2 var_1_1=0;\nint2 var_1_2=int2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 x; ivec3 y = ivec3(x); }",                                                     "void fn_0_0()\n{\nint3 var_1_1=0;\nint3 var_1_2=int3(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec4 x; ivec4 y = ivec4(x); }",                                                     "void fn_0_0()\n{\nint4 var_1_1=0;\nint4 var_1_2=int4(var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; bvec2 y = bvec2(x); }",                                                     "void fn_0_0()\n{\nbool2 var_1_1=false;\nbool2 var_1_2=bool2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec3 x; bvec3 y = bvec3(x); }",                                                     "void fn_0_0()\n{\nbool3 var_1_1=false;\nbool3 var_1_2=bool3(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 x; bvec4 y = bvec4(x); }",                                                     "void fn_0_0()\n{\nbool4 var_1_1=false;\nbool4 var_1_2=bool4(var_1_1);\n}\n");

        // Construct from matrix
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec4 y = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat4 var_1_2=GLSLvectorFromMatrix(var_1_1);\n}\n");

        // Matrix as constructor paramter to vector
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec4 v = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4 var_1_2=float4((var_1_1)._m00_m01_m02_m10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec4 v = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=float4((var_1_1)._m00_m01_m02_m03);\n}\n");


        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m02);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m02);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");

        // Various cases constructing from components
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec3 y = vec3(x, 1.0); }",                                                   "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat3 var_1_2=float3(var_1_1,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec3 y = vec3(1.0, x); }",                                                   "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat3 var_1_2=float3(1.000000e+000,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; vec3 y = vec3(1.0, x); }",                                                  "void fn_0_0()\n{\nbool2 var_1_1=false;\nfloat3 var_1_2=float3(1.000000e+000,var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec4 y = vec4(x, 1.0, 1.0); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=float4(var_1_1,1.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec4 y = vec4(1.0, x, 1.0); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=float4(1.000000e+000,var_1_1,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec4 y = vec4(1.0, 1.0, x); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=float4(1.000000e+000,1.000000e+000,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec2 x; vec4 y = vec4(1.0, 1.0, x); }",                                             "void fn_0_0()\n{\nbool2 var_1_1=false;\nfloat4 var_1_2=float4(1.000000e+000,1.000000e+000,var_1_1);\n}\n");

        // Pass too many components
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec3 v = vec3(x, x); }",                                                     "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat3 var_1_2=float3(var_1_1,(var_1_1).x);\n}\n");

        // Convert from float vector to boolean vector
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; bvec2 y = bvec2(x); }",                                                      "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nbool2 var_1_2=bool2(var_1_1);\n}\n");

        // Constructors are specified in our grammar as basic_type or IDENTIFIER; ensure precision is not allowed as part of a constructor
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 v = mediump vec4(0.0); }", E_GLSLERROR_SYNTAXERROR);

        // void, sampler2D, and samplerCube cannot be constructed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { return void(); }", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { return void(0); }", E_GLSLERROR_TYPENOTCONSTRUCTABLE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { sampler2D s = sampler2D(0); }", E_GLSLERROR_TYPENOTCONSTRUCTABLE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { samplerCube s = samplerCube(0); }", E_GLSLERROR_TYPENOTCONSTRUCTABLE);

        // Struct and array types cannot be vector or tensor constructor args.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; } s; vec2 v = vec2(s); }", E_GLSLERROR_INVALIDVECCTOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; } s; vec2 v = vec2(1, s); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i[2]; ivec2 v = ivec2(i); }", E_GLSLERROR_INVALIDVECCTOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i[1]; ivec2 v = ivec2(1, i); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        // Test truncated args to vec2
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec2 v = vec2(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat2 var_1_2=float2((var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec2 v = vec2(1, x); }",                                                     "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat2 var_1_2=float2(1,(var_1_1)._m00);\n}\n");

        // Test truncated args to vec3
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec3 v = vec3(1, x); }",                                                     "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=float3(1,(var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec3 v = vec3(1, x); }",                                                     "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat3 var_1_2=float3(1,(var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec3 v = vec3(1, x); }",                                                     "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat3 var_1_2=float3(1,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m02);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec3 v = vec3(1, x); }",                                                     "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat3 var_1_2=float3(1,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec3 v = vec3(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat3 var_1_2=float3((var_1_1)._m00_m01_m02);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec3 v = vec3(1, x); }",                                                     "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat3 var_1_2=float3(1,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec3 v = vec3(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat3 var_1_2=float3(1,2,(var_1_1)._m00);\n}\n");

        // Test truncated args to vec4
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec4 v = vec4(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,(var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec4 v = vec4(1, x); }",                                                     "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,(var_1_1).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec4 v = vec4(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,(var_1_1).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec4 v = vec4(1, x); }",                                                     "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat4 var_1_2=float4(1,(var_1_1)._m00_m01_m10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec4 v = vec4(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec4 v = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4 var_1_2=float4((var_1_1)._m00_m01_m02_m10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec4 v = vec4(1, x); }",                                                     "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4 var_1_2=float4(1,(var_1_1)._m00_m01_m02);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec4 v = vec4(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1)._m00);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec4 v = vec4(x); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=float4((var_1_1)._m00_m01_m02_m03);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec4 v = vec4(1, x); }",                                                     "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,(var_1_1)._m00_m01_m02);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec4 v = vec4(1, 2, x); }",                                                  "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,(var_1_1)._m00_m01);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 x; vec4 v = vec4(1, 2, 3, x); }",                                               "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=float4(1,2,3,(var_1_1)._m00);\n}\n");

        // Samplers not allowed as vector constructor arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform sampler2D s; void foo() { vec4 v = vec4(0, 0, 0, s); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform samplerCube s; void foo() { vec4 v = vec4(0, 0, 0, s);}", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bvec2 v = bvec2();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bvec3 v = bvec3();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bvec4 v = bvec4();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { ivec2 v = ivec2();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { ivec3 v = ivec3();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { ivec4 v = ivec4();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 v = vec2();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec3 v = vec3();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 v = vec4();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
    }

    void BasicGLSLTests::MatrixConstructorTests()
    {
        // Test construction from a single scalar
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(1); }",                                                              "void fn_0_0()\n{\nfloat2x2 var_1_1=GLSLmatrix2FromScalar(float(1));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(1.0); }",                                                            "void fn_0_0()\n{\nfloat2x2 var_1_1=GLSLmatrix2FromScalar(float(1.000000e+000));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(true); }",                                                           "void fn_0_0()\n{\nfloat2x2 var_1_1=GLSLmatrix2FromScalar(float(true));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 mat = mat3(1); }",                                                              "void fn_0_0()\n{\nfloat3x3 var_1_1=GLSLmatrix3FromScalar(float(1));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 mat = mat4(1); }",                                                              "void fn_0_0()\n{\nfloat4x4 var_1_1=GLSLmatrix4FromScalar(float(1));\n}\n");

        // Test valid inputs to a matrix constructor with scalars
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(1.0, 2.0, 3.0, 4.0); }",                                             "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1.000000e+000,2.000000e+000,3.000000e+000,4.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(1, 2, 3, 4); }",                                                     "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,2,3,4);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 mat = mat2(1.0, 2, 3.0, 4); }",                                                 "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1.000000e+000,2,3.000000e+000,4);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 mat = mat3(1, 2, 3, 4, 5, 6, 7, 8, 9); }",                                      "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,2,3,4,5,6,7,8,9);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 mat = mat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16); }",          "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);\n}\n");

        // Test with equal sized matrix
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 a; mat2 b = mat2(a); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat2x2 var_1_2=float2x2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 a; mat3 b = mat3(a); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat3x3 var_1_2=float3x3(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 a; mat4 b = mat4(a); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4x4 var_1_2=float4x4(var_1_1);\n}\n");

        // Test with different sized matrix
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 a; mat2 b = mat2(a); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat2x2 var_1_2=GLSLmatrix2FromMatrix3(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 a; mat2 b = mat2(a); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat2x2 var_1_2=GLSLmatrix2FromMatrix4(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 a; mat3 b = mat3(a); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat3x3 var_1_2=GLSLmatrix3FromMatrix2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 a; mat3 b = mat3(a); }",                                                        "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat3x3 var_1_2=GLSLmatrix3FromMatrix4(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 a; mat4 b = mat4(a); }",                                                        "void fn_0_0()\n{\nfloat2x2 var_1_1=0.0;\nfloat4x4 var_1_2=GLSLmatrix4FromMatrix2(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 a; mat4 b = mat4(a); }",                                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=0.0;\nfloat4x4 var_1_2=GLSLmatrix4FromMatrix3(var_1_1);\n}\n");

        // Test with right sized vectors
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec2 b; mat2 m = mat2(a, b); }",                                             "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nfloat2x2 var_1_3=float2x2(var_1_1,var_1_2);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; vec3 b; vec3 c; mat3 m = mat3(a, b, c); }",                                  "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=0.0;\nfloat3 var_1_3=0.0;\nfloat3x3 var_1_4=float3x3(var_1_1,var_1_2,var_1_3);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; vec4 b; vec4 c; vec4 d; mat4 m = mat4(a, b, c, d); }",                       "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nfloat4 var_1_3=0.0;\nfloat4 var_1_4=0.0;\nfloat4x4 var_1_5=float4x4(var_1_1,var_1_2,var_1_3,var_1_4);\n}\n");

        // Test mixing vector and float
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; mat2 m = mat2(a, 1.0, 1.0); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2x2 var_1_2=float2x2(var_1_1,1.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; mat2 m = mat2(1.0, a, 1.0); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2x2 var_1_2=float2x2(1.000000e+000,var_1_1,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; mat2 m = mat2(1.0, 1.0, a); }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2x2 var_1_2=float2x2(1.000000e+000,1.000000e+000,var_1_1);\n}\n");

        // Cannot pass multiple matrix arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat2 a; mat4 mat = mat4(a, a, a, a); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        // Test invalid component count with scalars
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat2 mat = mat2(1.0, 2.0, 3.0); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat3 mat = mat3(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat4 mat = mat4(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);

        // Test invalid component count with vectors
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec3 a; vec3 b; vec3 c; mat2 m = mat2(a, b, c); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);

        // Test truncation of matrix ctor last arg
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; vec3 b; mat2 m = mat2(a, b); }",                                             "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=0.0;\nfloat2x2 var_1_3=float2x2(var_1_1,(var_1_2).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec3 b; mat2 m = mat2(a, b); }",                                             "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat3 var_1_2=0.0;\nfloat2x2 var_1_3=float2x2(var_1_1,(var_1_2).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; vec2 b; mat2 m = mat2(a, b); }",                                             "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nfloat2x2 var_1_3=float2x2(var_1_1,(var_1_2).x);\n}\n");

        // Test invalid component count with vectors and scalars
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; mat2 mat = mat2(a, 3.0); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat3 mat = mat3(a, a, a, 3.0); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);

        // Test invalid vector arg count
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; mat2 m = mat2(a); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a, b, c; mat2 m = mat2(a, b, c); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec3 a, b; mat3 m = mat3(a, b); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec3 a, b, c, d; mat3 m = mat3(a, b, c, d); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a, b, c; mat4 m = mat4(a, b, c); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a, b, c, d, e; mat4 m = mat4(a, b, c, d, e); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT);

        // Struct and array types cannot be used in matrix constructors
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; } s; mat2 v = mat2(s); }", E_GLSLERROR_INVALIDMATCTORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; } s; mat2 v = mat2(1, s, 0, 1); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i[2]; mat2 v = mat2(i); }", E_GLSLERROR_INVALIDMATCTORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i[2]; mat2 v = mat2(1, i, 0, 1); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        // Test truncation of various vectors as matrix ctor args
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, vec2(0), vec2(0)); }",                                              "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,float2((0).xx),(float2((0).xx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(vec2(0), vec3(0)); }",                                                 "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(float2((0).xx),(float3((0).xxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, vec2(0), vec3(0)); }",                                              "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,float2((0).xx),(float3((0).xxx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, 2, vec3(0)); }",                                                    "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,2,(float3((0).xxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(vec2(0), vec4(0)); }",                                                 "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(float2((0).xx),(float4((0).xxxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, vec2(0), vec4(0)); }",                                              "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,float2((0).xx),(float4((0).xxxx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, vec4(0)); }",                                                       "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,(float4((0).xxxx)).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, 2, vec4(0)); }",                                                    "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,2,(float4((0).xxxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 m = mat2(1, 2, 3, vec4(0)); }",                                                 "void fn_0_0()\n{\nfloat2x2 var_1_1=float2x2(1,2,3,(float4((0).xxxx)).x);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(1, 2, vec3(0), vec3(0), vec2(0)); }",                                  "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,2,float3((0).xxx),float3((0).xxx),(float2((0).xx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(1, vec3(0), vec3(0), vec3(0)); }",                                     "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,float3((0).xxx),float3((0).xxx),(float3((0).xxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(1, 2, vec3(0), vec3(0), vec3(0)); }",                                  "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,2,float3((0).xxx),float3((0).xxx),(float3((0).xxx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(vec3(0), vec3(0), vec4(0)); }",                                        "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(float3((0).xxx),float3((0).xxx),(float4((0).xxxx)).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(1, vec3(0), vec3(0), vec4(0)); }",                                     "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,float3((0).xxx),float3((0).xxx),(float4((0).xxxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat3 m = mat3(1, 2, vec3(0), vec3(0), vec4(0)); }",                                  "void fn_0_0()\n{\nfloat3x3 var_1_1=float3x3(1,2,float3((0).xxx),float3((0).xxx),(float4((0).xxxx)).x);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, 2, 3, vec4(0), vec4(0), vec4(0), vec2(0)); }",                      "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,3,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float2((0).xx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, 2, vec4(0), vec4(0), vec4(0), vec3(0)); }",                         "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float3((0).xxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, 2, 3, vec4(0), vec4(0), vec4(0), vec3(0)); }",                      "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,3,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float3((0).xxx)).x);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, vec4(0), vec4(0), vec4(0), vec4(0)); }",                            "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float4((0).xxxx)).xyz);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, 2, vec4(0), vec4(0), vec4(0), vec4(0)); }",                         "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float4((0).xxxx)).xy);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m = mat4(1, 2, 3, vec4(0), vec4(0), vec4(0), vec4(0)); }",                      "void fn_0_0()\n{\nfloat4x4 var_1_1=float4x4(1,2,3,float4((0).xxxx),float4((0).xxxx),float4((0).xxxx),(float4((0).xxxx)).x);\n}\n");

        // Samplers not allowed as matrix constructor arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform sampler2D s; void foo() { mat2 v = mat2(0, 0, 0, s); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform samplerCube s; void foo() { mat2 v = mat2(0, 0, 0, s);}", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        // Cannot use matrix arguments to a matrix constructor if there are > 1 arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat3 m = mat3(vec3(0), vec2(0), mat2(1)); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat4 m = mat4(vec4(0), vec4(0), mat2(1), mat2(1)); }", E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat2 m = mat2();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat3 m = mat3();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { mat4 m = mat4();}", E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS);
    }

    void BasicGLSLTests::ErrorCleanupTests()
    {
        // Test that errors don't leak tree nodes
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int int;", E_GLSLERROR_SYNTAXERROR);
    }

    void BasicGLSLTests::InputDeclarationTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float alpha; attribute float beta; void foo() { alpha+beta; }",                         "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_0:attr_0_sem;\n  float attr_1:attr_1_sem;\n};\nvoid fn_0_2()\n{\nvsInput.attr_0+vsInput.attr_1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float beta; attribute float alpha; void foo() { beta+alpha; }",                         "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_1:attr_1_sem;\n  float attr_0:attr_0_sem;\n};\nvoid fn_0_2()\n{\nvsInput.attr_1+vsInput.attr_0;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float alpha, beta; void foo() { alpha+beta; }",                                         "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_0:attr_0_sem;\n  float attr_1:attr_1_sem;\n};\nvoid fn_0_2()\n{\nvsInput.attr_0+vsInput.attr_1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float beta, alpha; void foo() { alpha+beta; }",                                         "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_1:attr_1_sem;\n  float attr_0:attr_0_sem;\n};\nvoid fn_0_2()\n{\nvsInput.attr_0+vsInput.attr_1;\n}\n");

        // Ensure unused attributes are not output in the VSInput struct and are not counted towards the max number.
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute vec4 used0; attribute mat4 un1, un2, un3, un4, un5, un6; void foo() { used0; }",        "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float4 attr_6:attr_6_sem;\n};\nvoid fn_0_7()\n{\nvsInput.attr_6;\n}\n");

        // Test a basic non-array declaration of an attribute
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float foo; void fn() { foo; }",                                                         "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_0:attr_0_sem;\n};\nvoid fn_0_1()\n{\nvsInput.attr_0;\n}\n");

        // No attribute arrays are allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"attribute float foo[2];", E_GLSLERROR_INVALIDARRAYQUALIFIER);

        // Test a uniform declared between two attributes
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute float foo; uniform float bar; attribute float baz; void fn() { foo; baz; }",            "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float attr_1:attr_1_sem;\n  float attr_0:attr_0_sem;\n};\nfloat var_0_1=0.0;\nvoid fn_0_3()\n{\nvsInput.attr_1;\nvsInput.attr_0;\n}\n");


        // Test a basic varying - use it so that it will be output
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"precision mediump float; varying float foo; void main() { float bar = foo; }",                    "struct PSInput\n{\n  float vary_0:vary_0_sem;\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\nfloat var_1_1=psInput.vary_0;\n}\n");

        // Test varyings of the same type declared with commas - use them so that they will be output
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"precision mediump float; varying vec3 a, b, g; void main() { vec3 bar = vec3(a.r, b.g, g.b); }",  "struct PSInput\n{\n  float3 vary_0:vary_0_sem;\n  float3 vary_1:vary_1_sem;\n  float3 vary_2:vary_2_sem;\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\nfloat3 var_1_3=float3(psInput.vary_0.x,psInput.vary_1.y,psInput.vary_2.z);\n}\n");
    }

    void BasicGLSLTests::FragColorTests()
    {
        // Assign a varying to a frag color
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"precision mediump float; varying vec4 foo; void main() { gl_FragColor = foo; }",                  "struct PSInput\n{\n  float4 vary_0:vary_0_sem;\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=psInput.vary_0;\n}\n");

        // Simple assignment of a vector
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"void main() { gl_FragColor = vec4(1, 1, 1, 1); }",                                                "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=float4(1,1,1,1);\n}\n");
    }

    void BasicGLSLTests::NonInputDeclarationTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo(int) { }",                                                                              "float fn_0_0(int anonarg_0)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo(int bar) { }",                                                                          "float fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo(int bar, int baz) { }",                                                                 "float fn_0_0(int var_1_1,int var_1_2)\n{\n}\n");
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo(int bar, int bar) { }", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Test a regular sampler and cube
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler;",                                                                      "SamplerState sampler_0:register(s[0]);\nTexture2D<float4> texture_0:register(t[0]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform samplerCube sampler;",                                                                    "SamplerState sampler_0:register(s[0]);\nTextureCube<float4> texture_0:register(t[0]);\n");

        // Test anonymous samplers (should not work)
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform sampler2D;", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform samplerCube;", E_GLSLERROR_SYNTAXERROR);

        // Test two samplers in a single declaration
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D s1, s2;",                                                                       "SamplerState sampler_0:register(s[0]), sampler_1:register(s[1]);\nTexture2D<float4> texture_0:register(t[0]), texture_1:register(t[1]);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform samplerCube s1, s2;",                                                                     "SamplerState sampler_0:register(s[0]), sampler_1:register(s[1]);\nTextureCube<float4> texture_0:register(t[0]), texture_1:register(t[1]);\n");

        // Test an array of samplers
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2];",                                                                   "SamplerState sampler_0[2]:register(s[0]);\nTexture2D<float4> texture_0[2]:register(t[0]);\n");

        // Test an array followed by a non-array - the semantics should be packed correctly
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2], next;",                                                             "SamplerState sampler_0[2]:register(s[0]), sampler_2:register(s[2]);\nTexture2D<float4> texture_0[2]:register(t[0]), texture_2:register(t[2]);\n");

        // No samplers as attribute, varying, or regular variables
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying sampler2D sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"varying samplerCube sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute sampler2D sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"attribute samplerCube sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"sampler2D sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"sampler2D sampler[2];", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"samplerCube sampler;", E_GLSLERROR_INVALIDSAMPLERQUALIFIER);

        // Samplers also can't be const variables, but we detect other errors before getting to that point
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"const sampler2D sampler;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"const samplerCube sampler;", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform sampler2D foo; const sampler2D sampler = foo;", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform samplerCube foo; const samplerCube sampler = foo;", E_GLSLERROR_NONCONSTINITIALIZER);

        // Test a uniform boolean
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool foo; void bar() { bool baz; if (foo) { baz = true; } else { baz = foo; } }",         "bool var_0_0=false;\nvoid fn_0_1()\n{\nbool var_3_2=false;\nif (var_0_0){\nvar_3_2=true;\n}\nelse\n{\nvar_3_2=var_0_0;\n}\n}\n");
    }

    void BasicGLSLTests::ScopingDeclarationTests()
    {
        // Try naming the variables all the same - the parser should correctly figure out the scope for them
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; float foo; float bar() { float foo; if (foo == 1.0) { float foo; foo = 2.0; } }", "static float var_0_0=0.0;\nfloat fn_0_1()\n{\nfloat var_2_0=0.0;\nif (var_2_0==1.000000e+000){\nfloat var_1_0=0.0;\nvar_1_0=2.000000e+000;\n}\n}\n");

        // Now name them all differently and the output should be almost the same
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; float foo1; float bar() { float foo2; if (foo2 == 1.0) { float foo3; foo3 = 2.0; } }", "static float var_0_0=0.0;\nfloat fn_0_1()\n{\nfloat var_2_2=0.0;\nif (var_2_2==1.000000e+000){\nfloat var_1_3=0.0;\nvar_1_3=2.000000e+000;\n}\n}\n");
    }

    void BasicGLSLTests::Texture2DTests()
    {
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler; void main() { gl_FragColor = texture2D(sampler, vec2(0.0, 0.0)); }",   "SamplerState sampler_0:register(s[0]);\nTexture2D<float4> texture_0:register(t[0]);\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=GLSLtexture2D(sampler_0,texture_0,float2(0.000000e+000,0.000000e+000));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D sampler[2]; void main() { gl_FragColor = texture2D(sampler[1], vec2(0.0, 0.0)); }",   
                                                                                                                                                                "SamplerState sampler_0[2]:register(s[0]);\nTexture2D<float4> texture_0[2]:register(t[0]);\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=GLSLtexture2D(sampler_0[1],texture_0[1],float2(0.000000e+000,0.000000e+000));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform samplerCube sampler; void main() { gl_FragColor = textureCube(sampler, vec3(0.0, 0.0, 0.0)); }",   
                                                                                                                                                                "SamplerState sampler_0:register(s[0]);\nTextureCube<float4> texture_0:register(t[0]);\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=GLSLtextureCube(sampler_0,texture_0,float3(0.000000e+000,0.000000e+000,0.000000e+000));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"uniform samplerCube sampler[2]; void main() { gl_FragColor = textureCube(sampler[1], vec3(0.0, 0.0, 0.0)); }",   
                                                                                                                                                                "SamplerState sampler_0[2]:register(s[0]);\nTextureCube<float4> texture_0[2]:register(t[0]);\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=GLSLtextureCube(sampler_0[1],texture_0[1],float3(0.000000e+000,0.000000e+000,0.000000e+000));\n}\n");

        // Make sure we can parse the variations of the texture functions for fragment shaders
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2D(s, vec2(0, 0)); }",                                "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2D(var_1_1,var_1_1_tex,float2(0,0));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2D(s, vec2(0, 0), 0.0); }",                           "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DBias(var_1_1,var_1_1_tex,float2(0,0),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2DProj(s, vec3(0, 0, 1)); }",                         "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProj(var_1_1,var_1_1_tex,float3(0,0,1));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2DProj(s, vec4(0, 0, 1, 0)); }",                      "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProj(var_1_1,var_1_1_tex,float4(0,0,1,0));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2DProj(s, vec3(0, 0, 1), 0.0); }",                    "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProjBias(var_1_1,var_1_1_tex,float3(0,0,1),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D s) { return texture2DProj(s, vec4(0, 0, 1, 0), 0.0); }",                 "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProjBias(var_1_1,var_1_1_tex,float4(0,0,1,0),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(samplerCube s) { return textureCube(s, vec3(0, 0, 0)); }",                         "float4 fn_0_0(SamplerState var_1_1,TextureCube<float4> var_1_1_tex)\n{\nreturn GLSLtextureCube(var_1_1,var_1_1_tex,float3(0,0,0));\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(samplerCube s) { return textureCube(s, vec3(0, 0, 0), 0.0); }",                    "float4 fn_0_0(SamplerState var_1_1,TextureCube<float4> var_1_1_tex)\n{\nreturn GLSLtextureCubeBias(var_1_1,var_1_1_tex,float3(0,0,0),0.000000e+000);\n}\n");

        // Anonymous variables for samplers should be allowed
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D) { return vec4(0, 0, 0, 0); }",                                          "float4 fn_0_0(SamplerState anonarg_0,Texture2D<float4> anonarg_1)\n{\nreturn float4(0,0,0,0);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(samplerCube) { return vec4(0, 0, 0, 0); }",                                        "float4 fn_0_0(SamplerState anonarg_0,TextureCube<float4> anonarg_1)\n{\nreturn float4(0,0,0,0);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(sampler2D) { return vec4(0, 0, 0, 0); }",                                          "float4 fn_0_0(SamplerState anonarg_0,Texture2D<float4> anonarg_1)\n{\nreturn float4(0,0,0,0);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec4 Test(samplerCube) { return vec4(0, 0, 0, 0); }",                                        "float4 fn_0_0(SamplerState anonarg_0,TextureCube<float4> anonarg_1)\n{\nreturn float4(0,0,0,0);\n}\n");

        // Lod functions don't work for fragment shaders
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"highp vec4 Test(sampler2D s) { return texture2DLod(s, vec2(0, 0), 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"highp vec4 Test(sampler2D s) { return texture2DProjLod(s, vec3(0, 0, 1), 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"highp vec4 Test(sampler2D s) { return texture2DProjLod(s, vec4(0, 0, 1, 0), 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"highp vec4 Test(samplerCube s) { return textureCubeLod(s, vec3(0, 0, 0), 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // Test the same variations with vertex shader - bias functions will not work
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2D(s, vec2(0, 0)); }",                                     "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2D(var_1_1,var_1_1_tex,float2(0,0));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2DLod(s, vec2(0, 0), 0.0); }",                             "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DLod(var_1_1,var_1_1_tex,float2(0,0),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2DProj(s, vec3(0, 0, 1)); }",                              "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProj(var_1_1,var_1_1_tex,float3(0,0,1));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2DProj(s, vec4(0, 0, 1, 0)); }",                           "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProj(var_1_1,var_1_1_tex,float4(0,0,1,0));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2DProjLod(s, vec3(0, 0, 1), 0.0); }",                      "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProjLod(var_1_1,var_1_1_tex,float3(0,0,1),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(sampler2D s) { return texture2DProjLod(s, vec4(0, 0, 1, 0), 0.0); }",                   "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2DProjLod(var_1_1,var_1_1_tex,float4(0,0,1,0),0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(samplerCube s) { return textureCube(s, vec3(0, 0, 0)); }",                              "float4 fn_0_0(SamplerState var_1_1,TextureCube<float4> var_1_1_tex)\n{\nreturn GLSLtextureCube(var_1_1,var_1_1_tex,float3(0,0,0));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 Test(samplerCube s) { return textureCubeLod(s, vec3(0, 0, 0), 0.0); }",                      "float4 fn_0_0(SamplerState var_1_1,TextureCube<float4> var_1_1_tex)\n{\nreturn GLSLtextureCubeLod(var_1_1,var_1_1_tex,float3(0,0,0),0.000000e+000);\n}\n");

        // Verify error message for the vertex shaders calling the bias functions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 Test(sampler2D s) { return texture2D(s, vec2(0, 0), 0.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 Test(sampler2D s) { return texture2DProj(s, vec3(0, 0, 1), 0.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 Test(sampler2D s) { return texture2DProj(s, vec4(0, 0, 1, 0), 0.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 Test(samplerCube s) { return textureCube(s, vec3(0, 0, 0), 0.0); }", E_GLSLERROR_INVALIDARGUMENTS);
    }

    void BasicGLSLTests::SelectionTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bool a; if (a) { a = false; } }",                                                    "void fn_0_0()\n{\nbool var_2_1=false;\nif (var_2_1){\nvar_2_1=false;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bool a; if (a == true) { a = false; } }",                                            "void fn_0_0()\n{\nbool var_2_1=false;\nif (var_2_1==true){\nvar_2_1=false;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a, b; if (a == 1) { b = 2; } }",                                                 "void fn_0_0()\n{\nint var_2_1=0, var_2_2=0;\nif (var_2_1==1){\nvar_2_2=2;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a, b; if (a == vec2(1,1)) { b = vec2(2,2); } }",                                "void fn_0_0()\n{\nfloat2 var_2_1=0.0, var_2_2=0.0;\nif (all(var_2_1==float2(1,1))){\nvar_2_2=float2(2,2);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat2 a, b; if (a == mat2(1,1,1,1)) { b = mat2(2,2,2,2); } }",                        "void fn_0_0()\n{\nfloat2x2 var_2_1=0.0, var_2_2=0.0;\nif (all(var_2_1==float2x2(1,1,1,1))){\nvar_2_2=float2x2(2,2,2,2);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a, b; if (a == 1.0) { b = 2.0; } }",                                           "void fn_0_0()\n{\nfloat var_2_1=0.0, var_2_2=0.0;\nif (var_2_1==1.000000e+000){\nvar_2_2=2.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a, b; if (a == 1.0) { b = 2.0; } else { b = 3.0; } }",                         "void fn_0_0()\n{\nfloat var_3_1=0.0, var_3_2=0.0;\nif (var_3_1==1.000000e+000){\nvar_3_2=2.000000e+000;\n}\nelse\n{\nvar_3_2=3.000000e+000;\n}\n}\n");

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a = 1; if (a) { a = 2; } }\n", E_GLSLERROR_INVALIDIFEXPRESSION);

        // Struct and array types are not allowed in selection if expressions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; bool foo() { S s; if (s) { return true; } else { return false; } }", E_GLSLERROR_INVALIDIFEXPRESSION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool foo() { bool b[2]; if (b) { return true; } else { return false; } }", E_GLSLERROR_INVALIDIFEXPRESSION);
    }

    void BasicGLSLTests::ReturnTests()
    {
        // Test return with no expression
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { return; }" ,                                                                         "void fn_0_0()\n{\nreturn;\n}\n");

        // Test return with a simple expression
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo() { return 1.0; }" ,                                                                    "float fn_0_0()\n{\nreturn 1.000000e+000;\n}\n");

        // Test return with something more complex
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo() { float a; return (a + 1.0); }" ,                                                     "float fn_0_0()\n{\nfloat var_1_1=0.0;\nreturn (var_1_1+1.000000e+000);\n}\n");

        // Test returning the wrong type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; return (a + 1.0); }", E_GLSLERROR_INCOMPATIBLERETURN);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 foo() { float a; return (a + 1.0); }", E_GLSLERROR_INCOMPATIBLERETURN);
    }

    void BasicGLSLTests::LogicalOperatorTests()
    {
        // Test a simple case with each of the operators
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a = 1.0; float b = 2.0; if (a == 0.5 || b == 0.6) { a = 2.0; } }",             "void fn_0_0()\n{\nfloat var_2_1=1.000000e+000;\nfloat var_2_2=2.000000e+000;\nbool var_2_6=false;\nif (var_2_1==5.000000e-001){\nvar_2_6=true;\n}\nelse\n{\nvar_2_6=var_2_2==6.000000e-001;\n}\nif (var_2_6){\nvar_2_1=2.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a = 1.0; float b = 2.0; if (a == 0.5 && b == 0.6) { a = 2.0; } }",             "void fn_0_0()\n{\nfloat var_2_1=1.000000e+000;\nfloat var_2_2=2.000000e+000;\nbool var_2_6=false;\nif (var_2_1==5.000000e-001){\nvar_2_6=var_2_2==6.000000e-001;\n}\nelse\n{\nvar_2_6=false;\n}\nif (var_2_6){\nvar_2_1=2.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a = 1.0; float b = 2.0; if (a == 0.5 ^^ b == 0.6) { a = 2.0; } }",             "void fn_0_0()\n{\nfloat var_2_1=1.000000e+000;\nfloat var_2_2=2.000000e+000;\nif (((var_2_1==5.000000e-001)!=(var_2_2==6.000000e-001))){\nvar_2_1=2.000000e+000;\n}\n}\n");

        // Make sure that non boolean inputs fail
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b; if (a || b) { a = 1.0; } }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b; if (a && b) { a = 1.0; } }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b; if (a ^^ b) { a = 1.0; } }", E_GLSLERROR_INCOMPATIBLETYPES);
    }

    void BasicGLSLTests::ExpressionExpandScalarTests()
    {
        // Test all of the operators where this should work
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; c = a + b; }",                                            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_3=var_1_1+var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = a + b; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=var_1_1+(var_1_2).xx;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = b + a; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=(var_1_2).xx+var_1_1;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a += b; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1+=var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; a += b; }",                                                         "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1+=(var_1_2).xx;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; c = a - b; }",                                            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_3=var_1_1-var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = a - b; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=var_1_1-(var_1_2).xx;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = b - a; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=(var_1_2).xx-var_1_1;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a -= b; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1-=var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; a -= b; }",                                                         "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1-=(var_1_2).xx;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; c = a * b; }",                                            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_3=var_1_1*var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = a * b; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=var_1_1*var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = b * a; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=var_1_2*var_1_1;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a *= b; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1*=var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; a *= b; }",                                                         "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1*=var_1_2;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; c = a / b; }",                                            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_3=var_1_1/var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = a / b; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=var_1_1/var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; vec2 c; c = b / a; }",                                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat2 var_1_3=0.0;\nvar_1_3=(var_1_2).xx/var_1_1;\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a /= b; }",                                                        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1/=var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b; a /= b; }",                                                         "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1/=var_1_2;\n}\n");

        // Operations that should not work
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; float b; a = a * b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec3 foo; int bar; vec3 baz = foo * bar; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; float b; float c; c = b / a; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; float b; b += a; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; float b; b -= a; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; float b; b *= a; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; float b; b /= a; }", E_GLSLERROR_INCOMPATIBLETYPES);
    }

    void BasicGLSLTests::ExpressionTests()
    {
        // Test mathematic expressions
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1, s2; vec2 v1, v2; mat4 m1, m2; s1 = s1 + s2; v1 = v1 + v2; m1 = m1 + m2; }","void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nfloat2 var_1_3=0.0, var_1_4=0.0;\nfloat4x4 var_1_5=0.0, var_1_6=0.0;\nvar_1_1=var_1_1+var_1_2;\nvar_1_3=var_1_3+var_1_4;\nvar_1_5=var_1_5+var_1_6;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1, s2; vec2 v1, v2; mat4 m1, m2; s1 = s1 - s2; v1 = v1 - v2; m1 = m1 - m2; }","void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nfloat2 var_1_3=0.0, var_1_4=0.0;\nfloat4x4 var_1_5=0.0, var_1_6=0.0;\nvar_1_1=var_1_1-var_1_2;\nvar_1_3=var_1_3-var_1_4;\nvar_1_5=var_1_5-var_1_6;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1, s2; vec2 v1, v2; mat4 m1, m2; s1 = s1 / s2; v1 = v1 / v2; m1 = m1 / m2; }","void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nfloat2 var_1_3=0.0, var_1_4=0.0;\nfloat4x4 var_1_5=0.0, var_1_6=0.0;\nvar_1_1=var_1_1/var_1_2;\nvar_1_3=var_1_3/var_1_4;\nvar_1_5=var_1_5/var_1_6;\n}\n");

        // Various multiplies that should work
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1, s2; s1 = s1 * s2; }",                                                      "void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0;\nvar_1_1=var_1_1*var_1_2;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 m1; vec4 v1; v1 = m1 * v1; }",                                                  "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_2=mul(var_1_2,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1; vec4 v1; v1 = s1 * v1; }",                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_2=var_1_1*var_1_2;\n}\n");

        // Combinations of operators with paren
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float s1, s2, s3; s1 = (s2 + s3) * s1; }",                                           "void fn_0_0()\n{\nfloat var_1_1=0.0, var_1_2=0.0, var_1_3=0.0;\nvar_1_1=(var_1_2+var_1_3)*var_1_1;\n}\n");

        // Test expressions with swizzling
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; vec2 b; float c; b = a.xy * a.zw; c = b.x * a.x; }",                         "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_2=var_1_1.xy*var_1_1.zw;\nvar_1_3=var_1_2.x*var_1_1.x;\n}\n");

        // Divide by zero in an expression should be an error 
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; a[1 / 0] = 0.4; }", E_GLSLERROR_DIVIDEORMODBYZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a = 5 / 0; int b; b=a; }", E_GLSLERROR_DIVIDEORMODBYZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a = 5.0 / 0.0; float b; b=a; }", E_GLSLERROR_DIVIDEORMODBYZERO);

        // Test assignment methods
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a=1.0; }",                                                                  "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a+=1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1+=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a-=1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1-=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a/=1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1/=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a*=1.0; }",                                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1*=1.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mat4 a; mat4 b; b*=a; }",                                                            "void fn_0_0()\n{\nfloat4x4 var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\n(var_1_2=mul(var_1_1,var_1_2));\n}\n");

        // Test incompatible types with assignment methods
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a=b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a+=b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a-=b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a/=b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a+=b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; vec2 b; a*=b; }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Make sure reserved assignment operators do not work
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a>>=1; }", E_GLSLERROR_RESERVEDOPERATOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a<<=1; }", E_GLSLERROR_RESERVEDOPERATOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a%=1; }", E_GLSLERROR_RESERVEDOPERATOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a&=1; }", E_GLSLERROR_RESERVEDOPERATOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a|=1; }", E_GLSLERROR_RESERVEDOPERATOR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a; a^=1; }", E_GLSLERROR_RESERVEDOPERATOR);

        // Binary operators on sampler types should flag a known error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(sampler2D samp[2]) { samp[0]+samp[1]; }", E_GLSLERROR_WRONGOPERANDTYPES);

        // Binary operators on void types should flag a known error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void bar() {} void foo() {} void baz() { foo() == bar(); }", E_GLSLERROR_WRONGOPERANDTYPES);
    }

    void BasicGLSLTests::IndexingTests()
    {
        // Test indexing of matrices, vectors and arrays with constants
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; mat4 b; a = b[0]; }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nvar_1_1=var_1_2[0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; mat4 b; a = b[3]; }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nvar_1_1=var_1_2[3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; mat4 b; a = b[0][0]; }",                                                    "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nvar_1_1=var_1_2[0][0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; mat4 b; a = b[3][3]; }",                                                    "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nvar_1_1=var_1_2[3][3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec4 b; a = b[0]; }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_1=var_1_2[0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec4 b; a = b[3]; }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_1=var_1_2[3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; a = b[0]; }",                                                   "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2[4]=(float[4])0;\nvar_1_1=var_1_2[0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; a = b[3]; }",                                                   "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2[4]=(float[4])0;\nvar_1_1=var_1_2[3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bvec4 bar; bar[3] = true; }",                                                        "void fn_0_0()\n{\nbool4 var_1_1=false;\nvar_1_1[3]=true;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { ivec3 bar; bar[2] = 4; }",                                                           "void fn_0_0()\n{\nint3 var_1_1=0;\nvar_1_1[2]=4;\n}\n");

        // Test indexing with a constant expression
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { const ivec2 a = ivec2(1,1); int b[3]; b[a.x]; }",                                    "void fn_0_0()\n{\nconst int2 var_1_1=int2(1,1);\nint var_1_2[3]=(int[3])0;\nvar_1_2[var_1_1.x];\n}\n");

        // Test indexing of uniform vectors and arrays with index expressions - should work in vertex shader
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform vec4 b; void foo() { float a; int c = 0; a = b[c]; }",                                    "float4 var_0_0=0.0;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nint var_1_3=0;\nvar_1_2=var_0_0[var_1_3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform vec4 b; void foo() { float a; int c = 5; a = b[c]; }",                                    "float4 var_0_0=0.0;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nint var_1_3=5;\nvar_1_2=var_0_0[var_1_3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform float b[4]; void foo() { float a; int c = 0; a = b[c]; }",                                "float var_0_0[4]=(float[4])0;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nint var_1_3=0;\nvar_1_2=var_0_0[var_1_3];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform float b[4]; void foo() { float a; int c = 5; a = b[c]; }",                                "float var_0_0[4]=(float[4])0;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nint var_1_3=5;\nvar_1_2=var_0_0[var_1_3];\n}\n");

        // But not in Fragment shaders
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"precision mediump float; uniform vec4 b; void foo() { float a; int c = 0; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"precision mediump float; uniform vec4 b; void foo() { float a; int c = 5; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"precision mediump float; uniform float b[4]; void foo() { float a; int c = 0; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"precision mediump float; uniform float b[4]; void foo() { float a; int c = 5; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);

        // Test indexing of nonuniform vectors and arrays with expressions - this should fail
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; int c = 0; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; int c = 5; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; int c = 0; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; int c = 5; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);

        // ... Unless the expression is constant
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec4 b; const int c = 0; a = b[c]; }",                                      "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4 var_1_2=0.0;\nconst int var_1_3=0;\nvar_1_1=var_1_2[0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; const int c = 0; a = b[c]; }",                                  "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2[4]=(float[4])0;\nconst int var_1_3=0;\nvar_1_1=var_1_2[0];\n}\n");
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; const int c = 5; a = b[c]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; const int c = 5; a = b[c]; }", E_GLSLERROR_INDEXOUTOFRANGE);

        // ... Or they use loop variables
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec4 b; for (int c = 0 ; c < 1; c++) { a = b[c]; } }",                      "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat4 var_2_2=0.0;\n[unroll] for (int var_1_3=0;\nvar_1_3<1;(var_1_3++))\n{\nvar_2_1=var_2_2[var_1_3];\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; for (int c = 0 ; c < 1; c++) { a = b[c]; } }",                  "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2[4]=(float[4])0;\n[unroll] for (int var_1_3=0;\nvar_1_3<1;(var_1_3++))\n{\nvar_2_1=var_2_2[var_1_3];\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; mat4 b; for (int c = 0; c < 4; c++) { a += b[c][c]; } }",                   "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat4x4 var_2_2=0.0;\n[unroll] for (int var_1_3=0;\nvar_1_3<4;(var_1_3++))\n{\nvar_2_1+=var_2_2[var_1_3][var_1_3];\n}\n}\n");

        // Indexing with loop variables currently allows out of range access (as do other browsers)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec4 b; for (int c = 0 ; c < 10; c++) { a = b[c]; } }",                      "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat4 var_2_2=0.0;\n[unroll] for (int var_1_3=0;\nvar_1_3<10;(var_1_3++))\n{\nvar_2_1=var_2_2[var_1_3];\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; for (int c = 0 ; c < 10; c++) { a = b[c]; } }",                  "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2[4]=(float[4])0;\n[unroll] for (int var_1_3=0;\nvar_1_3<10;(var_1_3++))\n{\nvar_2_1=var_2_2[var_1_3];\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; mat4 b; for (int c = 0; c < 10; c++) { a += b[c][c]; } }",                   "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat4x4 var_2_2=0.0;\n[unroll] for (int var_1_3=0;\nvar_1_3<10;(var_1_3++))\n{\nvar_2_1+=var_2_2[var_1_3][var_1_3];\n}\n}\n");

        // ... Which includes loop variables used in int constructors
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; for (int c = 0; c < 1; c++) { a = b[int(c)]; } }",              "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2[4]=(float[4])0;\n[unroll] for (int var_1_3=0;\nvar_1_3<1;(var_1_3++))\n{\nvar_2_1=var_2_2[int(var_1_3)];\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b[4]; for (float c = 0.0; c < 1.0; c++) { a = b[int(c)]; } }",        "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2[4]=(float[4])0;\n[unroll] for (float var_1_3=0.000000e+000;\nvar_1_3<1.000000e+000;(var_1_3++))\n{\nvar_2_1=var_2_2[int(var_1_3)];\n}\n}\n");

        // Indexing of matrices with expressions should not work
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat4 b; int c = 0; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat4 b; int c = 5; a = b[c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; mat4 b; int c = 0; a = b[c][c]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; mat4 b; int c = 5; a = b[c][c]; }", E_GLSLERROR_INVALIDINDEX);

        // Test out of range index fails on matrices, vectors and arrays
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat4 b; a = b[4]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat4 b; a = b[-1]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; mat4 b; a = b[4][4]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; mat4 b; a = b[-1][-1]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; a = b[4]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; a = b[-1]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; a = b[4]; }", E_GLSLERROR_INDEXOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; a = b[-1]; }", E_GLSLERROR_INDEXOUTOFRANGE);

        // Test float index fails
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; mat4 b; a = b[a]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; vec4 b; a = b[a]; }", E_GLSLERROR_INVALIDINDEX);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b[4]; a = b[a]; }", E_GLSLERROR_INVALIDINDEX);

        // Test expressions with vector and matrix indexing, include lValues with an index
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec2 b; float c; c = a * b[0] * b[1]; }",                                   "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat2 var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_3=var_1_1*var_1_2[0]*var_1_2[1];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; vec2 b; b[0] = a * b[1]; }",                                                "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat2 var_1_2=0.0;\nvar_1_2[0]=var_1_1*var_1_2[1];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; mat4 b; mat4 c; a = b[0] + c[0]; }",                                         "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nfloat4x4 var_1_3=0.0;\nvar_1_1=var_1_2[0]+var_1_3[0];\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; mat4 b; b[1] = b[0] + a; }",                                                 "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nvar_1_2[1]=var_1_2[0]+var_1_1;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; mat4 b; mat4 c; a = b[0][1] + c[0][1]; }",                                  "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat4x4 var_1_2=0.0;\nfloat4x4 var_1_3=0.0;\nvar_1_1=var_1_2[0][1]+var_1_3[0][1];\n}\n");
    }

    void BasicGLSLTests::ExpressionListTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a+=1.0, a+=2.0; }",                                                         "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1+=1.000000e+000,var_1_1+=2.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a+=1.0, a+=2.0, a+=3.0; }",                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1+=1.000000e+000,var_1_1+=2.000000e+000,var_1_1+=3.000000e+000;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a+=1.0, a+=2.0, a+=3.0, a+=4.0; }",                                         "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1+=1.000000e+000,var_1_1+=2.000000e+000,var_1_1+=3.000000e+000,var_1_1+=4.000000e+000;\n}\n");
    }

    void BasicGLSLTests::KnownFunctionTests()
    {
        // Trig functions
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = 1.0 * sin(a); }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000*sin(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = 1.0 * cos(a); }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000*cos(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = 1.0 * tan(a); }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000*tan(var_1_1);\n}\n");

        // Min / Max
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a = min(a, b); }",                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1=min(var_1_1,var_1_2);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a = max(a, b); }",                                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1=max(var_1_1,var_1_2);\n}\n");

        // Clamp in various forms (all float, all vector, vector with float clamps)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a = clamp(b, 0.0, 1.0); }",                                        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1=clamp(var_1_2,0.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec2 b; a = clamp(b, vec2(0.0), vec2(1.0)); }",                              "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nvar_1_1=clamp(var_1_2,float2((0.000000e+000).xx),float2((1.000000e+000).xx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec2 b; a = clamp(b, 0.0, 1.0); }",                                          "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nvar_1_1=clamp(var_1_2,(0.000000e+000).xx,(1.000000e+000).xx);\n}\n");

        // Blenders
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; a = smoothstep(a, b, c); }",                              "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_1=smoothstep(var_1_1,var_1_2,var_1_3);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c; a = mix(a, b, c); }",                                     "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_3=0.0;\nvar_1_1=lerp(var_1_1,var_1_2,var_1_3);\n}\n");

        // Logarithmic / exponential
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = 1.0 * pow(a, 2.0); }",                                                  "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000*pow(var_1_1,2.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; a = vec2(1.0) * pow(a, vec2(2.0)); }",                                       "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1=float2((1.000000e+000).xx)*pow(var_1_1,float2((2.000000e+000).xx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = 1.0 * log(a); }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000*log(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = sqrt(a); }",                                                            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=sqrt(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; a = sqrt(a); }",                                                             "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1=sqrt(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; a = sqrt(a); }",                                                             "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nvar_1_1=sqrt(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; a = sqrt(a); }",                                                             "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nvar_1_1=sqrt(var_1_1);\n}\n");

        // Vector operators
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b = dot(a, a); }",                                                    "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=dot(var_1_1,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b = dot(a, a); }",                                                     "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=dot(var_1_1,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; float b = dot(a, a); }",                                                     "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat var_1_2=dot(var_1_1,var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; float b = dot(a, a); }",                                                     "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat var_1_2=dot(var_1_1,var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; a = normalize(a); }",                                                       "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=normalize(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; a = normalize(a); }",                                                        "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nvar_1_1=normalize(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; a = normalize(a); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nvar_1_1=normalize(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; a = normalize(a); }",                                                        "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nvar_1_1=normalize(var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b = length(a); }",                                                    "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=length(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; float b = length(a); }",                                                     "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat var_1_2=length(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; float b = length(a); }",                                                     "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat var_1_2=length(var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; float b = length(a); }",                                                     "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat var_1_2=length(var_1_1);\n}\n");

        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a = reflect(a, b); }",                                             "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nvar_1_1=reflect(var_1_1,var_1_2);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec2 b; a = reflect(a, b); }",                                               "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=0.0;\nvar_1_1=reflect(var_1_1,var_1_2);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 a; vec3 b; a = reflect(a, b); }",                                               "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=0.0;\nvar_1_1=reflect(var_1_1,var_1_2);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec4 a; vec4 b; a = reflect(a, b); }",                                               "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_1=reflect(var_1_1,var_1_2);\n}\n");

        // Other intrinsics
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 r = refract(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), 0.5); }",                 "void fn_0_0()\n{\nfloat3 var_1_1=refract(float3(1.000000e+000,0.000000e+000,0.000000e+000),float3(0.000000e+000,1.000000e+000,0.000000e+000),5.000000e-001);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 r = radians(vec2(45.0)); }",                                                    "void fn_0_0()\n{\nfloat2 var_1_1=radians(float2((4.500000e+001).xx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 f = fract(vec3(2.5)); }",                                                       "void fn_0_0()\n{\nfloat3 var_1_1=frac(float3((2.500000e+000).xxx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 c = cross(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0)); }",                        "void fn_0_0()\n{\nfloat3 var_1_1=cross(float3(1.000000e+000,0.000000e+000,0.000000e+000),float3(0.000000e+000,1.000000e+000,0.000000e+000));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a = abs(vec2(45.0)); }",                                                        "void fn_0_0()\n{\nfloat2 var_1_1=abs(float2((4.500000e+001).xx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 c = acos(vec3(2.5)); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=acos(float3((2.500000e+000).xxx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 s = asin(vec3(2.5)); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=asin(float3((2.500000e+000).xxx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 f = floor(vec3(2.5)); }",                                                       "void fn_0_0()\n{\nfloat3 var_1_1=floor(float3((2.500000e+000).xxx));\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec3 e = exp2(vec3(2.5)); }",                                                        "void fn_0_0()\n{\nfloat3 var_1_1=exp2(float3((2.500000e+000).xxx));\n}\n");

        // Reflect and dot need two arguments that are the same
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 a; vec3 b; a = reflect(a, b); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float res; vec4 a; vec3 b; res = dot(a, b); }", E_GLSLERROR_INVALIDARGUMENTS);

        // Length and dot need to return a float
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec3 b; a = length(b); }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec3 b; a = dot(b, b); }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Normalize and others need to return the same type that they take
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec3 b; a = normalize(b); }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec3 b; a = sqrt(b); }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Known functions are overloadable (exp2 is genType, so mat3 doesn't match)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"mat3 exp2(mat3 m) { return m; }",                                                                 "float3x3 fn_kn_0_12(float3x3 var_1_0)\n{\nreturn var_1_0;\n}\n");

        // invalid gentype should cause lookup of 'radians' and 'lerp' function call to fail lookup (no matching overload)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i = radians(3); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bool f = mix(true, false, false); }", E_GLSLERROR_INVALIDARGUMENTS);

        // Test all flavors of known functions to ensure you cannot overload on return type, or redefine/redeclare a known function
        // Single GENTYPE argument (sin), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(float f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(float f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec2 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec2 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec3 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec3 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec4 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec4 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void sin(vec4 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // Single GENTYPE argument (sin), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float sin(float f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float sin(float f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 sin(vec2 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 sin(vec2 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 sin(vec3 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 sin(vec3 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 sin(vec4 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 sin(vec4 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // Two GENTYPE arguments (pow), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(float f1, float f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(float f1, float f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec2 f1, vec2 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec2 f1, vec2 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec3 f1, vec3 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec3 f1, vec3 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec4 f1, vec4 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void pow(vec4 f1, vec4 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // Two GENTYPE arguments (pow), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float pow(float f1, float f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float pow(float f1, float f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 pow(vec2 f1, vec2 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 pow(vec2 f1, vec2 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 pow(vec3 f1, vec3 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 pow(vec3 f1, vec3 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 pow(vec4 f1, vec4 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 pow(vec4 f1, vec4 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // Three GENTYPE arguments (clamp), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(float f1, float f2, float f3) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(float f1, float f2, float f3);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec2 f1, vec2 f2, vec2 f3) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec2 f1, vec2 f2, vec2 f3);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec3 f1, vec3 f2, vec3 f3) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec3 f1, vec3 f2, vec3 f3);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec4 f1, vec4 f2, vec4 f3) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void clamp(vec4 f1, vec4 f2, vec4 f3);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // Three GENTYPE arguments (clamp), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float clamp(float f1, float f2, float f3) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float clamp(float f1, float f2, float f3);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 clamp(vec2 f1, vec2 f2, vec2 f3) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 clamp(vec2 f1, vec2 f2, vec2 f3);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 clamp(vec3 f1, vec3 f2, vec3 f3) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 clamp(vec3 f1, vec3 f2, vec3 f3);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 clamp(vec4 f1, vec4 f2, vec4 f3) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 clamp(vec4 f1, vec4 f2, vec4 f3);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // Concrete type + GENTYPE arguments (step), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f1, float f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f1, float f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec2 v) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec2 v);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec3 v) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec3 v);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec4 v) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void step(float f, vec4 v);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // Concrete type + GENTYPE arguments (step), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float step(float f1, float f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float step(float f1, float f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 step(float f, vec2 v) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec2 step(float f, vec2 v);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 step(float f, vec3 v) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 step(float f, vec3 v);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 step(float f, vec4 v) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 step(float f, vec4 v);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec4 step(out float f, vec4 v);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // concrete types (cross), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"mat4 cross(vec3 a, vec3 b) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"mat4 cross(vec3 a, vec3 b);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // concrete types (cross), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 cross(vec3 a, vec3 b) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"vec3 cross(vec3 a, vec3 b);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // CompareFloatVector, CompareIntVector, and CompareBoolVector, redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec2 f1, vec2 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec2 f1, vec2 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec3 f1, vec3 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec3 f1, vec3 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec4 f1, vec4 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(vec4 f1, vec4 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec2 f1, ivec2 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec2 f1, ivec2 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec3 f1, ivec3 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec3 f1, ivec3 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec4 f1, ivec4 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(ivec4 f1, ivec4 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec2 f1, bvec2 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec2 f1, bvec2 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec3 f1, bvec3 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec3 f1, bvec3 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec4 f1, bvec4 f2) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void equal(bvec4 f1, bvec4 f2);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // CompareFloatVector, CompareIntVector, and CompareBoolVector, redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(vec2 f1, vec2 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(vec2 f1, vec2 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(vec3 f1, vec3 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(vec3 f1, vec3 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(vec4 f1, vec4 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(vec4 f1, vec4 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(ivec2 f1, ivec2 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(ivec2 f1, ivec2 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(ivec3 f1, ivec3 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(ivec3 f1, ivec3 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(ivec4 f1, ivec4 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(ivec4 f1, ivec4 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(bvec2 f1, bvec2 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec2 equal(bvec2 f1, bvec2 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(bvec3 f1, bvec3 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec3 equal(bvec3 f1, bvec3 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(bvec4 f1, bvec4 f2) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(bvec4 f1, bvec4 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bvec4 equal(inout bvec4 f1, bvec4 f2);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);

        // TestBoolVector (any), redeclare/redefine different return type
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec2 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec2 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec3 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec3 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec4 f) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void any(bvec4 f);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // TestBoolVector (any), redeclare/redefine with correct signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec2 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec2 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec3 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec3 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec4 f) { }", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool any(bvec4 f);", E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED);
    }
    
    void BasicGLSLTests::Stage0Tests()
    {
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"precision mediump float; void main(void) { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }", "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=float4(1.000000e+000,1.000000e+000,1.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     0,                                          L"attribute vec3 aVertexPosition; uniform mat4 uMVMatrix; uniform mat4 uPMatrix; void main(void) { gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0); }", "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float3 attr_0:attr_0_sem;\n};\nfloat4x4 var_0_1=0.0;\nfloat4x4 var_0_2=0.0;\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nflippedPosition=mul(float4(vsInput.attr_0,1.000000e+000),mul(var_0_1,var_0_2));\n}\n");
    }

    void BasicGLSLTests::CommentTests()
    {
        // Make sure a comment at end of line does the right thing
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float a; // A float\n//float b;\nfloat c;",                                                       "static float var_0_0=0.0;\nstatic float var_0_1=0.0;\n");

        // Make sure a comment with illegal characters works
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float a; // 鰤はまち\n//float b;\nfloat c;",                                                       "static float var_0_0=0.0;\nstatic float var_0_1=0.0;\n");
    }

    void BasicGLSLTests::BasicLexerTests()
    {
        // Test for some basic stuff in the legal character space
        CSmartBstr bstrInput;
        bstrInput.Set(L"ABCXYZabcxyz(){}_+*09%&|^<>");

        TSmartPointer<CMemoryStream> spConvertedStream;
        VERIFY_SUCCEEDED(CGLSLUnicodeConverter::ConvertToAscii(bstrInput, &spConvertedStream));

        CMutableString<char> spConverted;
        VERIFY_SUCCEEDED(spConvertedStream->ExtractString(spConverted));
        VERIFY_ARE_EQUAL(::strcmp(spConverted, "ABCXYZabcxyz(){}_+*09%&|^<>\n"), 0);

        // Make sure that random unicode stuff is converted to characters that we do not allow
        spConvertedStream.Release();
        bstrInput.Set(L"鰤はまち");
        VERIFY_SUCCEEDED(CGLSLUnicodeConverter::ConvertToAscii(bstrInput, &spConvertedStream));

        spConverted.SetInitialSize(1);
        VERIFY_SUCCEEDED(spConvertedStream->ExtractString(spConverted));
        VERIFY_ARE_EQUAL(::strcmp(spConverted, "$$$$\n"), 0);

        // Using random unicode outside of comments should fail
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { int 鰤はまち; }", E_GLSLERROR_INVALIDCHARACTER);
    }

    void BasicGLSLTests::UnaryOperatorTests()
    {
        // Test the operators like +, -, !
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b = -a; }",                                                           "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=(-var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b = +a; }",                                                           "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=(+var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { bool a; bool b = !a; }",                                                             "void fn_0_0()\n{\nbool var_1_1=false;\nbool var_1_2=(!var_1_1);\n}\n");

        // Test increment and decrement, both pre and post
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a = 1; int b; b = a++; }",                                                       "void fn_0_0()\n{\nint var_1_1=1;\nint var_1_2=0;\nvar_1_2=(var_1_1++);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a = 1; int b; b = a--; }",                                                       "void fn_0_0()\n{\nint var_1_1=1;\nint var_1_2=0;\nvar_1_2=(var_1_1--);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a = 1; int b; b = ++a; }",                                                       "void fn_0_0()\n{\nint var_1_1=1;\nint var_1_2=0;\nvar_1_2=(++var_1_1);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a = 1; int b; b = --a; }",                                                       "void fn_0_0()\n{\nint var_1_1=1;\nint var_1_2=0;\nvar_1_2=(--var_1_1);\n}\n");

        // Test correct typing of operators
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a = 1; float b; b = a++; }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Test that reserved things are not supported
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { bool a; bool b = ~a; }", E_GLSLERROR_RESERVEDOPERATOR);

        // Unary operators are not allowed on struct or array types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; } s; +s; }", E_GLSLERROR_UNARYOPERATORNOTDEFINEDFORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int a[2]; +a; }", E_GLSLERROR_UNARYOPERATORNOTDEFINEDFORTYPE);

        // Unary operators are also not allowed on sampler or void types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(sampler2D samp) { +samp; }", E_GLSLERROR_WRONGOPERANDTYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { } void bar() { +foo(); }", E_GLSLERROR_WRONGOPERANDTYPES);
    }

    void BasicGLSLTests::RelationalExpressionTests()
    {
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; if (a != b) a = 1.0; }",                                           "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2=0.0;\nif (var_2_1!=var_2_2){\nvar_2_1=1.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; if (a < b) a = 1.0; }",                                            "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2=0.0;\nif (var_2_1<var_2_2){\nvar_2_1=1.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; if (a > b) a = 1.0; }",                                            "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2=0.0;\nif (var_2_1>var_2_2){\nvar_2_1=1.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; if (a <= b) a = 1.0; }",                                           "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2=0.0;\nif (var_2_1<=var_2_2){\nvar_2_1=1.000000e+000;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; if (a >= b) a = 1.0; }",                                           "void fn_0_0()\n{\nfloat var_2_1=0.0;\nfloat var_2_2=0.0;\nif (var_2_1>=var_2_2){\nvar_2_1=1.000000e+000;\n}\n}\n");
    }

    void BasicGLSLTests::ConditionalExpressionTests()
    {
        // Basic assignment expression with conditional expression rhs
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; a = (b < 0.0) ? 1.0 : 2.0; }",                                     "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_6=0.0;\nif ((var_1_2<0.000000e+000)){\nvar_1_6=1.000000e+000;\n}\nelse\n{\nvar_1_6=2.000000e+000;\n}\nvar_1_1=var_1_6;\n}\n");

        // Assignment expression with conditional expression rhs that has side effects in each branch
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; bool c; a = (b < 0.0) ? (c = true, 1.0) : (c = false, 2.0); }",    "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nbool var_1_3=false;\nfloat var_1_7=0.0;\nif ((var_1_2<0.000000e+000)){\nvar_1_7=(var_1_3=true,1.000000e+000);\n}\nelse\n{\nvar_1_7=(var_1_3=false,2.000000e+000);\n}\nvar_1_1=var_1_7;\n}\n");

        // Initializer with conditional
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; float b; float c = (a < b) ? 1.0 : 0.0; }",                                 "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_7=0.0;\nif ((var_1_1<var_1_2)){\nvar_1_7=1.000000e+000;\n}\nelse\n{\nvar_1_7=0.000000e+000;\n}\nfloat var_1_3=var_1_7;\n}\n");

        // Conditional expression with test expression that is an expression list, and side effects in each branch
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float a; int b; bool c; a = (a = 1.0, b, c) ? (b = 1, 1.0) : (b = 2, 2.0); }",        "void fn_0_0()\n{\nfloat var_1_1=0.0;\nint var_1_2=0;\nbool var_1_3=false;\nfloat var_1_7=0.0;\nif ((var_1_1=1.000000e+000,var_1_2,var_1_3)){\nvar_1_7=(var_1_2=1,1.000000e+000);\n}\nelse\n{\nvar_1_7=(var_1_2=2,2.000000e+000);\n}\nvar_1_1=var_1_7;\n}\n");

        // Right-most expression in expression list gives the list its type. The first expression in a ternary operator must have type bool.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; int b; bool c; a = (c, a, b) ? (b = 1, 1.0) : (b = 2, 2.0); }", E_GLSLERROR_TERNARYCONDITIONALNOTBOOL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b; int c; a = b ? 1.0 : c; }", E_GLSLERROR_TERNARYCONDITIONALNOTBOOL);

        // You cannot have expressions of different types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a; float b; int c; a = (b < 0.0) ? 1.0 : c; }", E_GLSLERROR_TERNARYCONDITIONALEXPRNE);

        // Struct and array types are not allowed in conditional expressions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; bool foo() { S s; return (s) ? true : false; }", E_GLSLERROR_TERNARYCONDITIONALNOTBOOL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool foo() { bool b[2]; return (b) ? true : false; }", E_GLSLERROR_TERNARYCONDITIONALNOTBOOL);

        // From the GLSL spec: Except for parameters to texture lookup functions, array indexing, structure field selection, and
        // parentheses, samplers are not allowed to be operands in expressions. Samplers cannot be treated as lvalues
        // and cannot be used as out or inout function parameters.
        // This would be an interesting test case if allowed, since we'd have to generate temporary sampler/texture variables
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform sampler2D a, b; void foo() { texture2D((true) ? a : b, vec2(1)); }", E_GLSLERROR_WRONGOPERANDTYPES);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform sampler2D a[2], b[2]; void foo() { texture2D(((true) ? a : b)[1], vec2(1)); }", E_GLSLERROR_WRONGOPERANDTYPES);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform samplerCube a, b; void foo() { textureCube((true) ? a : b, vec3(1)); }", E_GLSLERROR_WRONGOPERANDTYPES);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"uniform samplerCube a[2], b[2]; void foo() { textureCube(((true) ? a : b)[0], vec3(1)); }", E_GLSLERROR_WRONGOPERANDTYPES);

        // void is also not allowed as a conditional true/false type
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo(){} void bar(){} void baz() { (true) ? foo() : bar(); }", E_GLSLERROR_WRONGOPERANDTYPES);

        // Expressions that occur in the same statement as a conditional should be moved.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float fl1; float fl2; fl2 = (fl1 = 2.0, (fl1 == 2.0) ? 0.0 : 1.0); }",               "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_6=0.0;\nvar_1_6=var_1_1=2.000000e+000;\nfloat var_1_7=0.0;\nif ((var_1_1==2.000000e+000)){\nvar_1_7=0.000000e+000;\n}\nelse\n{\nvar_1_7=1.000000e+000;\n}\nvar_1_2=(var_1_6,var_1_7);\n}\n");

        // Test nested ternary where an expression in expression list (c = true) must be executed before second ternary
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a; bool c; (a == 0) ? (c = true, c ? a = 4 : a = 2) : (c = false, 2); }",        "void fn_0_0()\n{\nint var_1_1=0;\nbool var_1_2=false;\nint var_1_6=0;\nif ((var_1_1==0)){\nbool var_2_7=false;\nvar_2_7=var_1_2=true;\nint var_2_8=0;\nif (var_1_2){\nvar_2_8=var_1_1=4;\n}\nelse\n{\nvar_2_8=var_1_1=2;\n}\nvar_1_6=(var_2_7,var_2_8);\n}\nelse\n{\nvar_1_6=(var_1_2=false,2);\n}\nvar_1_6;\n}\n");

        // Test a function call with side effects in a conditional branch
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int a; int foo() { return (a = 5); } void bar() { int b = 0; (b != a) ? foo() : b = -1; }",       "static int var_0_0=0;\nint fn_0_1()\n{\nreturn (var_0_0=5);\n}\nvoid fn_0_2()\n{\nint var_2_3=0;\nint var_2_7=0;\nif ((var_2_3!=var_0_0)){\nvar_2_7=fn_0_1();\n}\nelse\n{\nvar_2_7=var_2_3=(-1);\n}\nvar_2_7;\n}\n");

        // Test a function call with side effects executes before the next expression in a conditional branch
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int a; void foo() { a = 5; } void bar() { int b = 0; (b != a) ? (foo(), b = a) : b = -1; }",      "static int var_0_0=0;\nvoid fn_0_1()\n{\nvar_0_0=5;\n}\nvoid fn_0_2()\n{\nint var_2_3=0;\nint var_2_7=0;\nif ((var_2_3!=var_0_0)){\nvar_2_7=(fn_0_1(),var_2_3=var_0_0);\n}\nelse\n{\nvar_2_7=var_2_3=(-1);\n}\nvar_2_7;\n}\n");

        // Test a function call with side effects executes before the moved conditional branches
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int a; void foo() { a = 5; } void bar() { int b = 0; (foo(), (b != a) ? b = a : b = -1); }",      "static int var_0_0=0;\nvoid fn_0_1()\n{\nvar_0_0=5;\n}\nvoid fn_0_2()\n{\nint var_2_3=0;\nfn_0_1();\nint var_2_7=0;\nif ((var_2_3!=var_0_0)){\nvar_2_7=var_2_3=var_0_0;\n}\nelse\n{\nvar_2_7=var_2_3=(-1);\n}\n(var_2_7);\n}\n");

        // Test ternary as a function argument, as well as a previous function argument that must execute before ternary.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int a = 0; void foo(int x, int y) { a += x + y; } void bar() { foo(a = 2, (false) ? 3 : 4); }",   "static int var_0_0=0;\nvoid fn_0_1(int var_1_2,int var_1_3)\n{\nvar_0_0+=var_1_2+var_1_3;\n}\nvoid fn_0_4()\n{\nint var_2_8=0;\nvar_2_8=var_0_0=2;\nint var_2_9=0;\nif ((false)){\nvar_2_9=3;\n}\nelse\n{\nvar_2_9=4;\n}\nfn_0_1(var_2_8,var_2_9);\n}\n");

        // Test moving a sampler usage
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D a; void bar(vec4 v, int i) {} void foo() { bar(texture2D(a, vec2(1)), (true) ? 1 : 2); }", "SamplerState sampler_0:register(s[0]);\nTexture2D<float4> texture_0:register(t[0]);\nvoid fn_0_1(float4 var_1_2,int var_1_3)\n{\n}\nvoid fn_0_4()\n{\nfloat4 var_2_8=0.0;\nvar_2_8=GLSLtexture2D(sampler_0,texture_0,float2((1).xx));\nint var_2_9=0;\nif ((true)){\nvar_2_9=1;\n}\nelse\n{\nvar_2_9=2;\n}\nfn_0_1(var_2_8,var_2_9);\n}\n");

        // Test returning a conditional
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec4 foo() { float a; float b; a = (b < 0.0) ? 1.0 : 2.0; return (a == 2.0) ? vec4(1) : vec4(0); }", "float4 fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=0.0;\nfloat var_1_6=0.0;\nif ((var_1_2<0.000000e+000)){\nvar_1_6=1.000000e+000;\n}\nelse\n{\nvar_1_6=2.000000e+000;\n}\nvar_1_1=var_1_6;\nfloat4 var_1_7=0.0;\nif ((var_1_1==2.000000e+000)){\nvar_1_7=float4((1).xxxx);\n}\nelse\n{\nvar_1_7=float4((0).xxxx);\n}\nreturn var_1_7;\n}\n");

        // Ensure we correctly generate temporary struct variables
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { struct S {int i;} s1, s2; s2 = (s1 = S(2), (s1.i == 2) ? S(0) : S(1)); }",           "struct typename_2_1 {\n  int var_1_2;\n};\ntypename_2_1 ctor_typename_2_1(int var_1_2) {\ntypename_2_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nvoid fn_0_0()\n{\ntypename_2_1 var_2_3=(typename_2_1)0, var_2_4=(typename_2_1)0;\ntypename_2_1 var_2_8=(typename_2_1)0;\nvar_2_8=var_2_3=ctor_typename_2_1(2);\ntypename_2_1 var_2_9=(typename_2_1)0;\nif ((var_2_3.var_1_2==2)){\nvar_2_9=ctor_typename_2_1(0);\n}\nelse\n{\nvar_2_9=ctor_typename_2_1(1);\n}\nvar_2_4=(var_2_8,var_2_9);\n}\n");

        // Ensure we correctly generate temporary variables for arrays (must pass to function since op= is not defined for array types
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x[2], int y){} void bar() { int a[2],b[2]; foo((true) ? (a[0] = -1, a) : (a[0] = 1, b), a[0]); }", "void fn_0_0(int var_1_1[2],int var_1_2)\n{\n}\nvoid fn_0_3()\n{\nint var_2_4[2]=(int[2])0, var_2_5[2]=(int[2])0;\nint var_2_9[2]=(int[2])0;\nif ((true)){\nvar_2_9=(var_2_4[0]=(-1),var_2_4);\n}\nelse\n{\nvar_2_9=(var_2_4[0]=1,var_2_5);\n}\nfn_0_0(var_2_9,var_2_4[0]);\n}\n");

        // Ensure array index operations within ternary translate as expected (really no different from vanilla identifiers
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a[2], b[2]; a[0] = (b[1] == a[1]) ? (b[0] = -1) : (b[1] = -1, b[0]); }",         "void fn_0_0()\n{\nint var_1_1[2]=(int[2])0, var_1_2[2]=(int[2])0;\nint var_1_6=0;\nif ((var_1_2[1]==var_1_1[1])){\nvar_1_6=(var_1_2[0]=(-1));\n}\nelse\n{\nvar_1_6=(var_1_2[1]=(-1),var_1_2[0]);\n}\nvar_1_1[0]=var_1_6;\n}\n");

        // Ensure array index operations on the result of ternary translate as expected
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a[2], b[2]; a[0] = ((b[1] == a[1]) ? (b[0] = -1, b) : (b[1] = -1, a))[1]; }",    "void fn_0_0()\n{\nint var_1_1[2]=(int[2])0, var_1_2[2]=(int[2])0;\nint var_1_6[2]=(int[2])0;\nif ((var_1_2[1]==var_1_1[1])){\nvar_1_6=(var_1_2[0]=(-1),var_1_2);\n}\nelse\n{\nvar_1_6=(var_1_2[1]=(-1),var_1_1);\n}\nvar_1_1[0]=(var_1_6)[1];\n}\n");

        // But only if they are the same array size (otherwise types should not match
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { int a[2], b[3]; a[0] = ((b[1] == a[1]) ? (b[0] = -1, b) : (b[1] = -1, a))[1]; }", E_GLSLERROR_TERNARYCONDITIONALEXPRNE);

        // Ternary expressions in declarations will split the declaration to be able to insert if/else in the correct location.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; void foo() { float fl0, fl1 = (f) ? 0.0 : 1.0; }",                                "bool var_0_0=false;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nfloat var_1_7=0.0;\nif ((var_0_0)){\nvar_1_7=0.000000e+000;\n}\nelse\n{\nvar_1_7=1.000000e+000;\n}\nfloat var_1_3=var_1_7;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; void foo() { float fl0, fl1 = (f) ? 0.0 : 1.0, fl2 = (fl0 < fl1) ? 1.0 : 2.0; }", "bool var_0_0=false;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nfloat var_1_8=0.0;\nif ((var_0_0)){\nvar_1_8=0.000000e+000;\n}\nelse\n{\nvar_1_8=1.000000e+000;\n}\nfloat var_1_3=var_1_8;\nfloat var_1_9=0.0;\nif ((var_1_2<var_1_3)){\nvar_1_9=1.000000e+000;\n}\nelse\n{\nvar_1_9=2.000000e+000;\n}\nfloat var_1_4=var_1_9;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; void foo() { float fl0, fl1 = (f) ? 0.0 : 1.0, fl2, fl3 = (fl2 < fl1) ? 1.0 : 2.0; }", "bool var_0_0=false;\nvoid fn_0_1()\n{\nfloat var_1_2=0.0;\nfloat var_1_9=0.0;\nif ((var_0_0)){\nvar_1_9=0.000000e+000;\n}\nelse\n{\nvar_1_9=1.000000e+000;\n}\nfloat var_1_3=var_1_9, var_1_4=0.0;\nfloat var_1_10=0.0;\nif ((var_1_4<var_1_3)){\nvar_1_10=1.000000e+000;\n}\nelse\n{\nvar_1_10=2.000000e+000;\n}\nfloat var_1_5=var_1_10;\n}\n"); 

        // This should work fine with struct declarations as well.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { struct S { int a; int b; } svar1 = S(0, 1), svar2 = S(svar1.a, svar1.b > 0 ? 5 : 6); }", "struct typename_2_1 {\n  int var_1_2;\n  int var_1_3;\n};\ntypename_2_1 ctor_typename_2_1(int var_1_2, int var_1_3) {\ntypename_2_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nvar_localstruct.var_1_3=var_1_3;\nreturn var_localstruct;\n}\nvoid fn_0_0()\n{\ntypename_2_1 var_2_4=ctor_typename_2_1(0,1);\nint var_2_9=0;\nif (var_2_4.var_1_3>0){\nvar_2_9=5;\n}\nelse\n{\nvar_2_9=6;\n}\ntypename_2_1 var_2_5=ctor_typename_2_1(var_2_4.var_1_2,var_2_9);\n}\n");

        // Similar to above declaration tests, but in the global scope. Ternary in global declarations generate
        // a function that is called inline to execute the if/else
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; float fl = (f) ? 0.0 : 1.0; void foo() {}",                                                     "bool var_0_0=false;\nstatic float var_0_1=0.0;\nvoid fn_0_6();\nvoid fn_0_2()\n{\nfn_0_6();\n}\nvoid fn_0_6()\n{\nfloat var_2_7=0.0;\nif ((var_0_0)){\nvar_2_7=0.000000e+000;\n}\nelse\n{\nvar_2_7=1.000000e+000;\n}\nvar_0_1=var_2_7;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; float fl0, fl1 = (f) ? 0.0 : 1.0; void foo() {}",                                               "bool var_0_0=false;\nstatic float var_0_1=0.0, var_0_2=0.0;\nvoid fn_0_7();\nvoid fn_0_3()\n{\nfn_0_7();\n}\nvoid fn_0_7()\n{\nfloat var_2_8=0.0;\nif ((var_0_0)){\nvar_2_8=0.000000e+000;\n}\nelse\n{\nvar_2_8=1.000000e+000;\n}\nvar_0_2=var_2_8;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; float fl0, fl1 = (f) ? 0.0 : 1.0, fl2 = (fl0 < fl1) ? 1.0 : 2.0; void foo() {}",                "bool var_0_0=false;\nstatic float var_0_1=0.0, var_0_2=0.0, var_0_3=0.0;\nvoid fn_0_8();\nvoid fn_0_4()\n{\nfn_0_8();\n}\nvoid fn_0_8()\n{\nfloat var_2_9=0.0;\nif ((var_0_0)){\nvar_2_9=0.000000e+000;\n}\nelse\n{\nvar_2_9=1.000000e+000;\n}\nvar_0_2=var_2_9;\nfloat var_2_10=0.0;\nif ((var_0_1<var_0_2)){\nvar_2_10=1.000000e+000;\n}\nelse\n{\nvar_2_10=2.000000e+000;\n}\nvar_0_3=var_2_10;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; float fl0, fl1 = (f) ? 0.0 : 1.0, fl2, fl3 = (fl2 < fl1) ? 1.0 : 2.0; void foo() {}",           "bool var_0_0=false;\nstatic float var_0_1=0.0, var_0_2=0.0, var_0_3=0.0, var_0_4=0.0;\nvoid fn_0_9();\nvoid fn_0_5()\n{\nfn_0_9();\n}\nvoid fn_0_9()\n{\nfloat var_2_10=0.0;\nif ((var_0_0)){\nvar_2_10=0.000000e+000;\n}\nelse\n{\nvar_2_10=1.000000e+000;\n}\nvar_0_2=var_2_10;\nfloat var_2_11=0.0;\nif ((var_0_3<var_0_2)){\nvar_2_11=1.000000e+000;\n}\nelse\n{\nvar_2_11=2.000000e+000;\n}\nvar_0_4=var_2_11;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"struct S { int a; int b; } svar1 = S(0, 1), svar2 = S(svar1.a, svar1.b > 0 ? 5 : 6); void foo() {}",            "struct typename_0_0 {\n  int var_1_1;\n  int var_1_2;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1, int var_1_2) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nstatic typename_0_0 var_0_3=ctor_typename_0_0(0,1), var_0_4=(typename_0_0)0;\nvoid fn_0_9();\nvoid fn_0_5()\n{\nfn_0_9();\n}\nvoid fn_0_9()\n{\nint var_3_10=0;\nif (var_0_3.var_1_2>0){\nvar_3_10=5;\n}\nelse\n{\nvar_3_10=6;\n}\nvar_0_4=ctor_typename_0_0(var_0_3.var_1_1,var_3_10);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"struct S { int a; int b; } svar1 = S(0, 1), svar2 = (true) ? S(svar1.a, svar1.b > 0 ? 5 : 6) : S(0, 0); void foo() {}", "struct typename_0_0 {\n  int var_1_1;\n  int var_1_2;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1, int var_1_2) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nstatic typename_0_0 var_0_3=ctor_typename_0_0(0,1), var_0_4=(typename_0_0)0;\nvoid fn_0_9();\nvoid fn_0_5()\n{\nfn_0_9();\n}\nvoid fn_0_9()\n{\ntypename_0_0 var_3_10=(typename_0_0)0;\nif ((true)){\nint var_5_11=0;\nif (var_0_3.var_1_2>0){\nvar_5_11=5;\n}\nelse\n{\nvar_5_11=6;\n}\nvar_3_10=ctor_typename_0_0(var_0_3.var_1_1,var_5_11);\n}\nelse\n{\nvar_3_10=ctor_typename_0_0(0,0);\n}\nvar_0_4=var_3_10;\n}\n");

        // Test a global declaration with ternary, but no function in the shader. In this case,
        // we should just do a straight translation of the ternary. No code will execute so
        // we shouldn't jump through all the hoops to generate a function and ensure side effects
        // happen in order and are short-circuited.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool f; float fl = (f) ? 0.0 : 1.0;",                                                     "bool var_0_0=false;\nstatic float var_0_1=(var_0_0)?0.000000e+000:1.000000e+000;\n");

        // Ternary inside for loop should execute on every iteration...
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float f; for (int i = 0; i < 2; i++) { f = (i < 1) ? 0.0 : 1.0; } }",                "void fn_0_0()\n{\nfloat var_2_1=0.0;\n[unroll] for (int var_1_2=0;\nvar_1_2<2;(var_1_2++))\n{\nfloat var_1_6=0.0;\nif ((var_1_2<1)){\nvar_1_6=0.000000e+000;\n}\nelse\n{\nvar_1_6=1.000000e+000;\n}\nvar_2_1=var_1_6;\n}\n}\n");

        // ... whether or not there were explicit curly braces in the GLSL
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { float f; for (int i = 0; i < 2; i++) f = (i < 1) ? 0.0 : 1.0; }",                    "void fn_0_0()\n{\nfloat var_2_1=0.0;\n[unroll] for (int var_1_2=0;\nvar_1_2<2;(var_1_2++))\n{\nfloat var_1_6=0.0;\nif ((var_1_2<1)){\nvar_1_6=0.000000e+000;\n}\nelse\n{\nvar_1_6=1.000000e+000;\n}\nvar_2_1=var_1_6;\n}\n}\n");

        // Similar to the above case, ensure we correctly move ternary inside if/else branches that have no statement list
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform bool b; void foo() { float f; if (b) f = (!b) ? 1.0 : 0.0; else (b) ? 1.0 : 0.0; }",      "bool var_0_0=false;\nvoid fn_0_1()\n{\nfloat var_3_2=0.0;\nif (var_0_0){\nfloat var_1_6=0.0;\nif (((!var_0_0))){\nvar_1_6=1.000000e+000;\n}\nelse\n{\nvar_1_6=0.000000e+000;\n}\nvar_3_2=var_1_6;\n}\nelse\n{\nfloat var_2_7=0.0;\nif ((var_0_0)){\nvar_2_7=1.000000e+000;\n}\nelse\n{\nvar_2_7=0.000000e+000;\n}\nvar_2_7;\n}\n}\n");


        // Test combinations of ternary and short-circuiting logical && and ||
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo() { float a = 1.0, b = 2.0; return (a < 1.1 && b > 2.0) ? 5.0 : 10.0; }",               "float fn_0_0()\n{\nfloat var_1_1=1.000000e+000, var_1_2=2.000000e+000;\nfloat var_1_6=0.0;\nbool var_1_7=false;\nif (var_1_1<1.100000e+000){\nvar_1_7=var_1_2>2.000000e+000;\n}\nelse\n{\nvar_1_7=false;\n}\nif ((var_1_7)){\nvar_1_6=5.000000e+000;\n}\nelse\n{\nvar_1_6=1.000000e+001;\n}\nreturn var_1_6;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"float foo() { float a = 1.0, b = 2.0; return (a < 1.1 || b > 2.0) ? 5.0 : 10.0; }",               "float fn_0_0()\n{\nfloat var_1_1=1.000000e+000, var_1_2=2.000000e+000;\nfloat var_1_6=0.0;\nbool var_1_7=false;\nif (var_1_1<1.100000e+000){\nvar_1_7=true;\n}\nelse\n{\nvar_1_7=var_1_2>2.000000e+000;\n}\nif ((var_1_7)){\nvar_1_6=5.000000e+000;\n}\nelse\n{\nvar_1_6=1.000000e+001;\n}\nreturn var_1_6;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"bool foo() { bool a = true, b = false, c = true; return (c) ? (a && b) : (a || b); }",            "bool fn_0_0()\n{\nbool var_1_1=true, var_1_2=false, var_1_3=true;\nbool var_1_7=false;\nif ((var_1_3)){\nbool var_2_8=false;\nif (var_1_1){\nvar_2_8=var_1_2;\n}\nelse\n{\nvar_2_8=false;\n}\nvar_1_7=(var_2_8);\n}\nelse\n{\nbool var_3_9=false;\nif (var_1_1){\nvar_3_9=true;\n}\nelse\n{\nvar_3_9=var_1_2;\n}\nvar_1_7=(var_3_9);\n}\nreturn var_1_7;\n}\n");

        // Sampler types in ordered expressions do not need to move as they have no side effects.
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"uniform sampler2D u_samp; void foo(sampler2D samp, int a) {} void bar() { foo(u_samp, (true) ? 3 : 4); }", "SamplerState sampler_0:register(s[0]);\nTexture2D<float4> texture_0:register(t[0]);\nvoid fn_0_1(SamplerState var_1_2,Texture2D<float4> var_1_2_tex,int var_1_3)\n{\n}\nvoid fn_0_4()\n{\nint var_2_8=0;\nif ((true)){\nvar_2_8=3;\n}\nelse\n{\nvar_2_8=4;\n}\nfn_0_1(sampler_0,texture_0,var_2_8);\n}\n");
    }

    void BasicGLSLTests::PreprocessTests()
    {
        // Make sure that a valid define allows code
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() {\n#ifdef GL_ES\nlowp float a;\n#endif\na = 1.0; }",                                   "void fn_0_0()\n{\nfloat var_1_1=0.0;\nvar_1_1=1.000000e+000;\n}\n");

        // Make sure that an invalid define disallows code
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() {\n#ifdef FOO\nfloat a;\n#endif\na = 1.0; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
    }

    void BasicGLSLTests::FunctionTests()
    {
        // Function forward declarations should translate correctly, and not cause scope issues
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; vec2 ForwardFunction(float x);",                                         "float2 fn_0_0(float var_1_1);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; vec2 foo(float x); float x;",                                            "float2 fn_0_0(float var_1_1);\nstatic float var_0_1=0.0;\n");

        // main is a special function - make sure that we translate it correctly with both ways of declaring it
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void main() { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }",                                        "void main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=float4(1.000000e+000,1.000000e+000,1.000000e+000,1.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void main(void) { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }",                                    "void main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\npsOutput.fragColor[0]=float4(1.000000e+000,1.000000e+000,1.000000e+000,1.000000e+000);\n}\n");

        // Make sure main defined with wrong arguments or return types does not translate
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(int x) { }", E_GLSLERROR_REDEFINEMAIN);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(float x) {}", E_GLSLERROR_REDEFINEMAIN);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(int x, float y) { }", E_GLSLERROR_REDEFINEMAIN);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float main() { return 1.0; }", E_GLSLERROR_REDEFINEMAIN);

        // Define a function and call it
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { } void bar() { foo(); }",                                                            "void fn_0_0()\n{\n}\nvoid fn_0_1()\n{\nfn_0_0();\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; float foo() { return 1.0; } void bar() { float a; a = foo(); }",         "float fn_0_0()\n{\nreturn 1.000000e+000;\n}\nvoid fn_0_1()\n{\nfloat var_2_2=0.0;\nvar_2_2=fn_0_0();\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(mediump float x) { } void bar() { foo(1.0); }",                                          "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\nfn_0_0(1.000000e+000);\n}\n");

        // Call main
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void main(void) { main(void); }", E_GLSLERROR_CALLMAIN);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { main(void); }", E_GLSLERROR_CALLMAIN);

        // Define a function with a const argument, a precision argument, and a const precision argument
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; void foo(const float x) {}",                                             "void fn_0_0(const float var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(highp float x) {}",                                                                      "void fn_0_0(float var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(const highp float x) {}",                                                                "void fn_0_0(const float var_1_1)\n{\n}\n");

        // Test various parameter qualifiers
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {}",                                                                              "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(in int x) {}",                                                                           "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(out int x) {}",                                                                          "void fn_0_0(inout int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(inout int x) {}",                                                                        "void fn_0_0(inout int var_1_1)\n{\n}\n");

        // Call a complex out function
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; void foo(out float x) {} void bar() { vec2 v; foo(v.x); }",              "void fn_0_0(inout float var_1_1)\n{\n}\nvoid fn_0_2()\n{\nfloat2 var_2_3=0.0;\nfn_0_0(var_2_3.x);\n}\n");

        // Mix all of the above
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(const in lowp float x) {}",                                                              "void fn_0_0(const float var_1_1)\n{\n}\n");
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const out mediump float x) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const inout highp float x) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);

        // ... And without variable names
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo(const in lowp float) {}",                                                                "void fn_0_0(const float anonarg_0)\n{\n}\n");
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const out mediump float) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const inout highp float) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);

        // Const in arrays is allowed for no qualifier and in
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(const float x[1]) {}",                                                                   "void fn_0_0(const float var_1_1[1])\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(const in float x[1]) {}",                                                                "void fn_0_0(const float var_1_1[1])\n{\n}\n");

        // ... But not for out and inout
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const out float x[1]) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(const inout float x[1]) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);

        // Parameter declarators with arrays must have constant size
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int bar = (true) ? 1 : 0; void foo(float x[bar]) {}", E_GLSLERROR_ARRAYDECLARATORNOTCONST);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"const int bar = 0; void foo(float x[bar]) {}", E_GLSLERROR_ARRAYSIZELEQZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(float x[0]) {}", E_GLSLERROR_ARRAYSIZELEQZERO);
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"const int bar = 1; void foo(float x[bar]) {}",                                                    "static const int var_0_0=1;\nvoid fn_0_1(float var_1_2[1])\n{\n}\n");

        // Make sure you cannot have attribute or varying arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(varying float x) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(attribute float x) {}", E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER);

        // Call function with wrong arguments and make sure it fails
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { } void bar() { foo(1.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo() { return 1.0; } void bar() { int a; a = foo(); }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(float x) { } void bar() { foo(1.0, 1.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int x) { } void bar() { foo(1.0); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(float x) { } void bar() { foo(1); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(float x) { } void bar() { int a = 1; foo(a); }", E_GLSLERROR_INVALIDARGUMENTS);

        // Define a function that takes a sampler and uses it
        TestParserInput(GLSLShaderType::Fragment, GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; vec4 foo(sampler2D arg) { return texture2D(arg, vec2(0.0, 0.0)); }",       "float4 fn_0_0(SamplerState var_1_1,Texture2D<float4> var_1_1_tex)\n{\nreturn GLSLtexture2D(var_1_1,var_1_1_tex,float2(0.000000e+000,0.000000e+000));\n}\n");

        // Define a function that takes a sampler array and uses it
        TestParserInput(GLSLShaderType::Fragment, GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; vec4 foo(sampler2D arg[1]) { return texture2D(arg[0], vec2(0.0, 0.0)); }", "float4 fn_0_0(SamplerState var_1_1[1],Texture2D<float4> var_1_1_tex[1])\n{\nreturn GLSLtexture2D(var_1_1[0],var_1_1_tex[0],float2(0.000000e+000,0.000000e+000));\n}\n");

        // Forward declare a function and use it
        TestParserInput(GLSLShaderType::Fragment, GLSLTranslateOptions::DisableWriteInputs,   L"void foo(); void bar() { foo(); } void foo() {}",                                                   "void fn_0_0();\nvoid fn_0_1()\n{\nfn_0_0();\n}\nvoid fn_0_0()\n{\n}\n");

        // Forward declare a function and use it, but don't define it
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(); void bar() { foo(); }", E_GLSLERROR_UNDEFINEDFUNCTION);

        // Function overloading is allowed
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int); void foo(float);",                                                                 "void fn_0_0(int anonarg_0);\nvoid fn_0_0(float anonarg_1);\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int) {} void foo(float) {}",                                                             "void fn_0_0(int anonarg_0)\n{\n}\nvoid fn_0_0(float anonarg_1)\n{\n}\n");

        // Using names of builtin functions is allowed
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void sin(float x, float y) {}",                                                                   "void fn_kn_0_3(float var_1_0,float var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void sin(float x, float y) {} void foo() { sin(1.0, 1.0); }",                                     "void fn_kn_0_3(float var_1_0,float var_1_1)\n{\n}\nvoid fn_0_2()\n{\nfn_kn_0_3(1.000000e+000,1.000000e+000);\n}\n");

        // Duplicate definition of same overload is not allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int) {} void foo(int) {}", E_GLSLERROR_FUNCTIONREDEFINITION);

        // Duplicate forward definitions are not allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int); void foo(int);", E_GLSLERROR_FUNCTIONREDECLARATION);

        // Definition counts as declaration
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int) {} void foo(int);", E_GLSLERROR_FUNCTIONREDECLARATION);

        // Local function declarations are disallowed...
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int) { void bar(); }", E_GLSLERROR_LOCALFUNCTIONDECLARATION);

        // ... as are local function definitions (but this is enforced by the grammar)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int) { void bar() {} }", E_GLSLERROR_SYNTAXERROR);

        // Overloads on return type are not allowed. Test variations between declaration/definition
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo(int); void foo(int) {}", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo(int) { return 1; } void foo(int) {}", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo(int); void foo(int);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo(int) { return 1; } void foo(int);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; int foo(S s) { return 1; } void foo(S);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; int foo(S s) { return 1; } void foo(S) {}", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; int foo(S s); void foo(S s) { }", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; int foo(S s) { return 1; } void foo(S);", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; struct S2 { int i; }; S foo(); S2 foo();", E_GLSLERROR_FUNCOVERLOADONRETURNTYPE);

        // Qualifiers must match between functions with the same signature
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int i); void foo(out int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int i); void foo(inout int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(in int i); void foo(out int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(in int i); void foo(inout int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int i); void foo(int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int i); void foo(in int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int i); void foo(inout int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(inout int i); void foo(int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(inout int i); void foo(in int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(inout int i); void foo(out int i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; void foo(inout S i); void foo(out S i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int i; }; void foo(S i); void foo(out S i) {}", E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS);

        // Qualifiers match, but the function is already defined
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(inout int i) {} void foo(inout int i) {}", E_GLSLERROR_FUNCTIONREDEFINITION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(in int i) {} void foo(int i) {}", E_GLSLERROR_FUNCTIONREDEFINITION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int i) {} void foo(in int i) {}", E_GLSLERROR_FUNCTIONREDEFINITION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int i) {} void foo(out int i);", E_GLSLERROR_FUNCTIONREDECLARATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(in int i) {} void foo(int i);", E_GLSLERROR_FUNCTIONREDECLARATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int i) {} void foo(in int i);", E_GLSLERROR_FUNCTIONREDECLARATION);

        // Overloading on different types is, of course, allowed
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"void foo(in int i); void foo(inout float i) {}",                                                        "void fn_0_0(int var_2_1);\nvoid fn_0_0(inout float var_1_1)\n{\n}\n");

        // A parameter of type 'void' is allowed if it is the only parameter. Void parameters should not be output.
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"void foo(void);",                                                                                       "void fn_0_0();\n");
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"void foo(void) {}",                                                                                     "void fn_0_0()\n{\n}\n");
        
        // A void parameter cannot be named (variables of type void are not allowed)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(void vv);", E_GLSLERROR_VARIABLETYPEVOID);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(void vv) {}", E_GLSLERROR_VARIABLETYPEVOID);

        // 'void' parameter cannot occur with other parameters...
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int, void) {}", E_GLSLERROR_MULTIPLEPARAMSWITHVOID);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int, vec3, mat2, void, mat4) {}", E_GLSLERROR_MULTIPLEPARAMSWITHVOID);

        // ... however, a quirk in the GLSL reference compiler and other browsers does allow it as the first parameter, which is then removed.
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, L"void foo(void, int) {} void bar() { foo(3); }",                                                         "void fn_0_0(int anonarg_0)\n{\n}\nvoid fn_0_1()\n{\nfn_0_0(3);\n}\n");
    }

    void BasicGLSLTests::ForLoopTests()
    {
        // Test breaking and continuing out of the loop
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i++) { break; } }",                                           "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;(var_1_1++))\n{\nbreak;\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i++) { continue; } }",                                        "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;(var_1_1++))\n{\ncontinue;\n}\n}\n");

        // You cannot break or continue unless in a loop
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(void) { break; }", E_GLSLERROR_INVALIDBREAKLOCATION);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(void) { continue; }", E_GLSLERROR_INVALIDCONTINUELOCATION);

        // The first part of the for loop needs to be a declaration of an int or float
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i++) { } }",                                                  "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (lowp float i = 0.0; i < 1.0; i++) { } }",                                       "void fn_0_0()\n{\n[unroll] for (float var_1_1=0.000000e+000;\nvar_1_1<1.000000e+000;(var_1_1++))\n{\n}\n}\n");

        // ... So declaring outside and just assigning is not OK
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int i; for (i = 0; i < 1; i++) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // And it needs a single identifier, so declaring two things is not OK
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int j = 0, i = 0; i < 1; i++) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // And it needs to be initialized
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i; i < 1; i++) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // Need to use a float or an int, so a boolean is right out...
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (bool i = false; i; i = !i) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // ... as well as struct types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; }; for (S s = S(2); s.i < 4; s.i++) { } }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // Needs to be initialized to a constant value
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 1 + 1; i < 1; i++) { } }",                                              "void fn_0_0()\n{\n[unroll] for (int var_1_1=1+1;\nvar_1_1<1;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"const int x = 1; void foo() { for (int i = x; i < 1; i++) { } }",                                 "static const int var_0_0=1;\nvoid fn_0_1()\n{\n[unroll] for (int var_1_2=var_0_0;\nvar_1_2<1;(var_1_2++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; const float x = 1.0; void foo() { for (float i = x; i < 1.0; i++) { } }","static const float var_0_0=1.000000e+000;\nvoid fn_0_1()\n{\n[unroll] for (float var_1_2=var_0_0;\nvar_1_2<1.000000e+000;(var_1_2++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; const float x = 1.0; void foo() { for (float i = true ? x : 0.0; i < 1.0; i++) { } }", "static const float var_0_0=1.000000e+000;\nvoid fn_0_1()\n{\nfloat var_2_6=0.0;\nif (true){\nvar_2_6=var_0_0;\n}\nelse\n{\nvar_2_6=0.000000e+000;\n}\n[unroll] for (float var_1_2=var_2_6;\nvar_1_2<1.000000e+000;(var_1_2++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; const float x = 1.0, y = 2.0; void foo() { for (float i = (x == y) ? x : y; i < 1.0; i++) { } }", "static const float var_0_0=1.000000e+000, var_0_1=2.000000e+000;\nvoid fn_0_2()\n{\nfloat var_2_7=0.0;\nif ((var_0_0==var_0_1)){\nvar_2_7=var_0_0;\n}\nelse\n{\nvar_2_7=var_0_1;\n}\n[unroll] for (float var_1_3=var_2_7;\nvar_1_3<1.000000e+000;(var_1_3++))\n{\n}\n}\n");

        // ... So initializing to some other variable is not allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int x = 0; for (int i = x; i < 1; i++) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float x = 0.0; for (float i = x; i < 1.0; i++) {} }", E_GLSLERROR_INVALIDFORLOOPINIT);

        // Need to have a condition
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; ; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; ; ) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);

        // The condition needs to be a binary operator from a set list (< was already tested above)
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i <= 2; i++) { } }",                                                 "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<=2;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 2; i > 0; i--) { } }",                                                  "void fn_0_0()\n{\n[unroll] for (int var_1_1=2;\nvar_1_1>0;(var_1_1--))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 1; i >= 0; i--) { } }",                                                 "void fn_0_0()\n{\n[unroll] for (int var_1_1=1;\nvar_1_1>=0;(var_1_1--))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i != 1; i++) { } }",                                                 "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1!=1;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 1; i != 0; i--) { } }",                                                 "void fn_0_0()\n{\n[unroll] for (int var_1_1=1;\nvar_1_1!=0;(var_1_1--))\n{\n}\n}\n");

        // ... So just a random expression should fail
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);

        // The condition needs an identifier on the left
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; 1 > i; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);

        // The condition cannot use operators outside of comparison or equality
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i * 2; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);

        // The condition needs to have a constant expression on the right
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i <= 1+1; i++) { } }",                                               "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<=1+1;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"const int x = 2; void foo() { for (int i = 0; i <= x; i++) { } }",                                "static const int var_0_0=2;\nvoid fn_0_1()\n{\n[unroll] for (int var_1_2=0;\nvar_1_2<=var_0_0;(var_1_2++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision mediump float; const float x = 2.0; void foo() { for (float i = 0.0; i <= x; i++) { } }", "static const float var_0_0=2.000000e+000;\nvoid fn_0_1()\n{\n[unroll] for (float var_1_2=0.000000e+000;\nvar_1_2<=var_0_0;(var_1_2++))\n{\n}\n}\n");

        // ... So comparing to other variables is not OK
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int x = 1; for (int i = 0; i < x; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision mediump float; void foo() { float x = 1.0; for (float i = 0.0; i < x; i++) { } }", E_GLSLERROR_INVALIDFORLOOPCOND);

        // The iteration needs to only do basic increment or decrement of constants
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; ++i) { } }",                                                  "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;(++var_1_1))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 1; i > 0; --i) { } }",                                                  "void fn_0_0()\n{\n[unroll] for (int var_1_1=1;\nvar_1_1>0;(--var_1_1))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i+=1) { } }",                                                 "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;var_1_1+=1)\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 1; i >= 0; i-=1) { } }",                                                "void fn_0_0()\n{\n[unroll] for (int var_1_1=1;\nvar_1_1>=0;var_1_1-=1)\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i+=(1+1)) { } }",                                             "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;var_1_1+=(1+1))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"const int x = 1; void foo() { for (int i = 0; i < 1; i+=x) { } }",                                "static const int var_0_0=1;\nvoid fn_0_1()\n{\n[unroll] for (int var_1_2=0;\nvar_1_2<1;var_1_2+=var_0_0)\n{\n}\n}\n");

        // ... So other expressions are not valid
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; ) { } }", E_GLSLERROR_INVALIDFORLOOPITER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 10; i *= 2) {} }", E_GLSLERROR_INVALIDFORLOOPITER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int x = 10; for (int i = 0; i < 10; i += x) {} }", E_GLSLERROR_INVALIDFORLOOPITER);

        // The loop variable can be read from in the loop
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 1; i++) { int x = i; } }",                                       "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<1;(var_1_1++))\n{\nint var_1_2=var_1_1;\n}\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int x[2]; for (int i = 0; i < 1; i++) { int y = x[i]; } }",                          "void fn_0_0()\n{\nint var_2_1[2]=(int[2])0;\n[unroll] for (int var_1_2=0;\nvar_1_2<1;(var_1_2++))\n{\nint var_1_3=var_2_1[var_1_2];\n}\n}\n");

        // ... But not written to
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { i = 1; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { (i) = 1; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { i++; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { i+=2; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { int x; x = i = 1; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { int x; i = x = 1; } }", E_GLSLERROR_INVALIDFORLOOPINDEXWRITE);
        
        // ... Even using an out parameter with a variety of expressions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int x) { x = 1; } void bar() { for (int i = 0; i < 1; i++) { foo(i); } }", E_GLSLERROR_LOOPINDEXOUTPARAM);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(out int x) { x = 1; } void bar() { for (int i = 0; i < 1; i++) { foo((i)); } }", E_GLSLERROR_LOOPINDEXOUTPARAM);

        // Loops that have 64 iterations or less will get [unroll]. Any more than that and they will 
        // not have an attribute output (the HLSL compiler will do whatever is appropriate based on
        // the flags we send it and the target shader profile).
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 64; i++) { foo(i); } }",                      "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=0;\nvar_2_3<64;(var_2_3++))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 65; i++) { foo(i); } }",                      "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=0;\nvar_2_3<65;(var_2_3++))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 129; i += 2) { foo(i); } }",                  "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=0;\nvar_2_3<129;var_2_3+=2)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 130; i += 2) { foo(i); } }",                  "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=0;\nvar_2_3<130;var_2_3+=2)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 5; i < 200; i += 3) { foo(i); } }",                  "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=5;\nvar_2_3<200;var_2_3+=3)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 5; i < 199; i += 3) { foo(i); } }",                  "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=5;\nvar_2_3<199;var_2_3+=3)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i > 9812; i--) { foo(i); } }",                 "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=9876;\nvar_2_3>9812;(var_2_3--))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i > 9811; i--) { foo(i); } }",                 "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=9876;\nvar_2_3>9811;(var_2_3--))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i < 9747; i -= 2) { foo(i); } }",              "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=9876;\nvar_2_3<9747;var_2_3-=2)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i < 9746; i -= 2) { foo(i); } }",              "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=9876;\nvar_2_3<9746;var_2_3-=2)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i > 1882; i -= 123) { foo(i); } }",            "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=9876;\nvar_2_3>1882;var_2_3-=123)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 9876; i > 1881; i -= 123) { foo(i); } }",            "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=9876;\nvar_2_3>1881;var_2_3-=123)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 0.1; i < 65.0; i++) { foo(i); } }",              "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (float var_2_3=1.000000e-001;\nvar_2_3<6.500000e+001;(var_2_3++))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 0.1; i < 65.2; i++) { foo(i); } }",              "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (float var_2_3=1.000000e-001;\nvar_2_3<6.520000e+001;(var_2_3++))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 0.1; i < 129.1; i += 2.0) { foo(i); } }",        "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (float var_2_3=1.000000e-001;\nvar_2_3<1.291000e+002;var_2_3+=2.000000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 0.1; i < 130.1; i += 2.0) { foo(i); } }",        "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (float var_2_3=1.000000e-001;\nvar_2_3<1.301000e+002;var_2_3+=2.000000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 5.1; i < 200.1; i += 3.0) { foo(i); } }",        "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (float var_2_3=5.100000e+000;\nvar_2_3<2.001000e+002;var_2_3+=3.000000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 5.1; i < 200.1; i += 3.1) { foo(i); } }",        "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (float var_2_3=5.100000e+000;\nvar_2_3<2.001000e+002;var_2_3+=3.100000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 5.1; i < 199.1; i += 3.0) { foo(i); } }",        "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (float var_2_3=5.100000e+000;\nvar_2_3<1.991000e+002;var_2_3+=3.000000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");

        // We don't discrimate the sign of the operators in order to prevent us from 
        // submitting infinite loops to the HLSL compiler (which does reject them)
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 1; i--) { foo(i); } }",                       "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[unroll] for (int var_2_3=0;\nvar_2_3<1;(var_2_3--))\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 65; i--) { foo(i); } }",                      "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=0;\nvar_2_3<65;(var_2_3--))\n{\nfn_0_0(var_2_3);\n}\n}\n");

        // Since we don't propagate constant values through const ternary expressions or struct field selections,
        // we will not detect the following loop as having > 32 iterations and will mark it as [unroll]
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = (true) ? 0 : -1; i < 65; i++) { } }",                                   "void fn_0_0()\n{\nint var_2_5=0;\nif ((true)){\nvar_2_5=0;\n}\nelse\n{\nvar_2_5=(-1);\n}\n[unroll] for (int var_1_1=var_2_5;\nvar_1_1<65;(var_1_1++))\n{\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { const struct S { int a; } svar = S(0); for (int i = svar.a; i < 65; i++) { } }",     "struct typename_3_1 {\n  int var_1_2;\n};\ntypename_3_1 ctor_typename_3_1(int var_1_2) {\ntypename_3_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nvoid fn_0_0()\n{\nconst typename_3_1 var_3_3=ctor_typename_3_1(0);\n[unroll] for (int var_2_4=var_3_3.var_1_2;\nvar_2_4<65;(var_2_4++))\n{\n}\n}\n");

        // Zero iterations should work, even though they make very little sense
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int x) {} void bar() { for (int i = 0; i < 1; i += 0) { foo(i); } }",                    "void fn_0_0(int var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (int var_2_3=0;\nvar_2_3<1;var_2_3+=0)\n{\nfn_0_0(var_2_3);\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float x) {} void bar() { for (float i = 0.0; i < 1.0; i += 0.0) { foo(i); } }",          "void fn_0_0(float var_1_1)\n{\n}\nvoid fn_0_2()\n{\n[loop] for (float var_2_3=0.000000e+000;\nvar_2_3<1.000000e+000;var_2_3+=0.000000e+000)\n{\nfn_0_0(var_2_3);\n}\n}\n");
    }

    void BasicGLSLTests::ScopeTests()
    {
        // You should be able to declare the same variable in two loops because of unique scope
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { for (int i = 0; i < 2; i++) { } for (int i = 0; i < 2; i++) { } }",                  "void fn_0_0()\n{\n[unroll] for (int var_1_1=0;\nvar_1_1<2;(var_1_1++))\n{\n}\n[unroll] for (int var_2_1=0;\nvar_2_1<2;(var_2_1++))\n{\n}\n}\n");

        // You should not be able to redeclare a variable in the loop statement scope (with and without braces)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) { int i; } }", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { for (int i = 0; i < 1; i++) int i; }", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Function definition defines a scope too
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(float bar) { float bar; }", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Regular blocks define scope
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float bar) { { float bar; } }",                                                          "void fn_0_0(float var_1_1)\n{\n{\nfloat var_2_1=0.0;\n}\n}\n");

        // If with and without braces defines scope
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float bar) { if (true) float bar; }",                                                    "void fn_0_0(float var_2_1)\n{\nif (true){\nfloat var_1_1=0.0;\n}\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(float bar) { if (true) { float bar; } }",                                                "void fn_0_0(float var_2_1)\n{\nif (true){\nfloat var_1_1=0.0;\n}\n}\n");

        // Function and variable cannot share name in same scope
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() {} int foo;", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo; void foo() {}", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Test various scenarios to stress identifier lookup - start with a variable overriding a function name
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void f1() {} void foo() { int f1; int x = f1; }",                                                 "void fn_0_0()\n{\n}\nvoid fn_0_1()\n{\nint var_2_0=0;\nint var_2_2=var_2_0;\n}\n");

        // Treating a function name like a variable should not cause the identifier code to get confused
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void f1(int x) {} void foo() { int x = f1; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void f1(int x) {} void f1(float x) {} void foo() { int x = f1; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // Functions, structs, and variable identifiers all share the same namespace within a scope (p.28 of the GLSL spec)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo; float foo() { return 1.0; }", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo; struct foo { int bar; };", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo() { return 1.0; } struct foo { int bar; };", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo; float foo[5];", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo(); struct foo { int bar; };", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Only function identifiers that are part of the function header should skip the prototype scope.
        // Function id 'foo' should be in the global scope. Local int variable id 'foo' should be in the function prototype scope.
        // After the local int foo is declared, usages of the identifier 'foo' should find the variable (even if it is a function call).
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo() { int foo = 1; return foo(); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo(int foo) { return foo(1); }", E_GLSLERROR_INVALIDARGUMENTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo() { struct foo { int a; }; return foo(); }", E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR);

        // Ensure that the function name truly is not scoped by the prototype
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int foo(int foo) { return foo; }",                                                                "int fn_0_0(int var_1_0)\n{\nreturn var_1_0;\n}\n");

        // Variables and typenames in the global scope are allowed to have the same name as built-in functions
        TestParserInputDefault(
            L"struct distance { int a; }; void foo() { distance foo; int distance; foo.a = distance; }",
            "struct typenamek_0_29 {\n  int var_1_0;\n};\nvoid fn_0_1()\n{\ntypenamek_0_29 var_2_1=(typenamek_0_29)0;\nint vark_2_29=0;\nvar_2_1.var_1_0=vark_2_29;\n}\n"
            );
        TestParserInputDefault(
            L"int distance; void foo() { int bar = distance; struct distance { int a; } foo; foo.a = bar; }",
            "struct typenamek_2_29 {\n  int var_1_2;\n};\nstatic int vark_0_29=0;\nvoid fn_0_0()\n{\nint var_2_1=vark_0_29;\ntypenamek_2_29 var_2_0=(typenamek_2_29)0;\nvar_2_0.var_1_2=var_2_1;\n}\n"
            );

        // Make sure that typename and variables with built-in function name is found upon redeclaration
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct distance { int a; }; int distance;", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int distance; struct distance { int a; };", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int distance; void distance() {}", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct distance { int a; }; void distance() {}", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
    }

    void BasicGLSLTests::DiscardTests()
    {
        // Test valid usage
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void main(void) { discard; }",                                                                    "void main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\ndiscard;\n}\n");

        // Test invalid usage
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void main(void) { discard; }", E_GLSLERROR_INVALIDDISCARDLOCATION);
    }

    void BasicGLSLTests::SwizzleTests()
    {
        // Test basic swizzling operations with all three sets of indexers
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec4 b; b = a.xxyy; a = b.zw; }",                                            "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_2=var_1_1.xxyy;\nvar_1_1=var_1_2.zw;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec4 b; b = a.rrgg; a = b.ba; }",                                            "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_2=var_1_1.xxyy;\nvar_1_1=var_1_2.zw;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { vec2 a; vec4 b; b = a.sstt; a = b.pq; }",                                            "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat4 var_1_2=0.0;\nvar_1_2=var_1_1.xxyy;\nvar_1_1=var_1_2.zw;\n}\n");

        // Don't mix index types (like r and y)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.ry; }", E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.xs; }", E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.sg; }", E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT);

        // Don't index beyond where you can (vec2 has no z)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.yzzz; }", E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.baaa; }", E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; b = a.pqqq; }", E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE);

        // Don't try to do more than 4 components
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec2 a; vec4 b; a = b.xxxxx; }", E_GLSLERROR_TOOMANYSWIZZLECOMPONENTS);

        // Don't try to use illegal characters in field selection of vectors
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 b; b = b.cdef; }", E_GLSLERROR_INVALIDSWIZZLECOMPONENT);

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { vec4 b[2]; b.x = 1.0; }", E_GLSLERROR_INVALIDTYPEFORFIELDSELECTION);
    }

    void BasicGLSLTests::NegativeTests()
    {
        // Don't touch undeclared variables and functions
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { a = 1.0; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { a = bar(); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // Sin does not take 2 arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { float a = sin(a, a); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // Parameters of functions should not act like globals
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"float foo(float param) { } float bar() { param = 1.0; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
    }

    void BasicGLSLTests::SpecialVariableTests()
    {
        // Test what you can do in the vertex shader
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { gl_Position = vec4(0.0, 0.0, 0.0, 0.0); }",                                          "void fn_0_0()\n{\nflippedPosition=float4(0.000000e+000,0.000000e+000,0.000000e+000,0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { gl_PointSize = 2.0; }",                                                              "void fn_0_0()\n{\ngl_PointSize=2.000000e+000;\n}\n");

        // Test that these things do not work in the fragment shader
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { gl_Position = vec4(0.0, 0.0, 0.0, 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { gl_PointSize = 2.0; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // Test what you can do in the fragment shader
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); }",                                         "void fn_0_0()\n{\npsOutput.fragColor[0]=float4(0.000000e+000,0.000000e+000,0.000000e+000,0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { gl_FragData[0] = vec4(0.0, 0.0, 0.0, 0.0); }",                                       "void fn_0_0()\n{\npsOutput.fragColor[0]=float4(0.000000e+000,0.000000e+000,0.000000e+000,0.000000e+000);\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mediump vec4 bar = gl_FragCoord; }",                                                 "void fn_0_0()\n{\nfloat4 var_1_1=gl_FragCoord;\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { mediump vec2 bar = gl_PointCoord; }",                                                "void fn_0_0()\n{\nfloat2 var_1_1=gl_PointCoord;\n}\n");

        // FragData only has a single element
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { gl_FragData[1] = vec4(0.0, 0.0, 0.0, 0.0); }", E_GLSLERROR_INDEXOUTOFRANGE);

        // Fragment shader things do not work in the vertex shader
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_FragData[0] = vec4(0.0, 0.0, 0.0, 0.0); }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { lowp vec4 bar = gl_FragCoord; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { lowp vec2 bar = gl_PointCoord; }", E_GLSLERROR_UNDECLAREDIDENTIFIER);

        // These special variables should work the same in both stages
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexAttribs;",                                                                    "static int var_0_0=gl_MaxVertexAttribs;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexUniformVectors;",                                                             "static int var_0_0=gl_MaxVertexUniformVectors;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVaryingVectors;",                                                                   "static int var_0_0=gl_MaxVaryingVectors;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexTextureImageUnits;",                                                          "static int var_0_0=gl_MaxVertexTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxCombinedTextureImageUnits;",                                                        "static int var_0_0=gl_MaxCombinedTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxTextureImageUnits;",                                                                "static int var_0_0=gl_MaxTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxFragmentUniformVectors;",                                                           "static int var_0_0=gl_MaxFragmentUniformVectors;\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxDrawBuffers;",                                                                      "static int var_0_0=gl_MaxDrawBuffers;\n");

        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexAttribs;",                                                                    "static int var_0_0=gl_MaxVertexAttribs;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexUniformVectors;",                                                             "static int var_0_0=gl_MaxVertexUniformVectors;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVaryingVectors;",                                                                   "static int var_0_0=gl_MaxVaryingVectors;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxVertexTextureImageUnits;",                                                          "static int var_0_0=gl_MaxVertexTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxCombinedTextureImageUnits;",                                                        "static int var_0_0=gl_MaxCombinedTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxTextureImageUnits;",                                                                "static int var_0_0=gl_MaxTextureImageUnits;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxFragmentUniformVectors;",                                                           "static int var_0_0=gl_MaxFragmentUniformVectors;\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"int x = gl_MaxDrawBuffers;",                                                                      "static int var_0_0=gl_MaxDrawBuffers;\n");

        // We should always have access to gl_DepthRange
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"vec3 x = vec3(gl_DepthRange.near, gl_DepthRange.far, gl_DepthRange.diff);",                       "static float3 var_0_0=float3(gl_DepthRange.near,gl_DepthRange.far,gl_DepthRange.diff);\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"lowp vec3 x = vec3(gl_DepthRange.near, gl_DepthRange.far, gl_DepthRange.diff);",                       "static float3 var_0_0=float3(gl_DepthRange.near,gl_DepthRange.far,gl_DepthRange.diff);\n");

        // Should be able to compare it with itself
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L" void foo() { if (gl_DepthRange == gl_DepthRange) return; }",                                     "bool eq_gl_DepthRangeParameters(gl_DepthRangeParameters val1, gl_DepthRangeParameters val2) {\nreturn (val1.near==val2.near && val1.far==val2.far && val1.diff==val2.diff);\n}\nvoid fn_0_0()\n{\nif (eq_gl_DepthRangeParameters(gl_DepthRange,gl_DepthRange)){\nreturn;\n}\n}\n");

        // ... But we should not be able to write to it
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_DepthRange = 1.0;\n }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_DepthRange = float3(0, 0, 0);\n }", E_GLSLERROR_UNDECLAREDIDENTIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_DepthRange.near = 1.0;\n }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_DepthRange.far = 1.0;\n }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { gl_DepthRange.diff = 1.0;\n }", E_GLSLERROR_INVALIDLVALUE);

        // Should be able to make a struct of the same type as gl_DepthRange
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L" void foo() { gl_DepthRangeParameters x = gl_DepthRange; }",                                      "void fn_0_0()\n{\ngl_DepthRangeParameters var_1_1=gl_DepthRange;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L" void foo() { gl_DepthRangeParameters x; x = gl_DepthRange; }",                                   "void fn_0_0()\n{\ngl_DepthRangeParameters var_1_1=(gl_DepthRangeParameters)0;\nvar_1_1=gl_DepthRange;\n}\n");
    }

    void BasicGLSLTests::VariableNameTests()
    {
        // Try out some names that should definitely work for variables, parameters, functions
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int a; }",                                                                           "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int abc; }",                                                                         "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { int _a1b2c3; }",                                                                     "void fn_0_0()\n{\nint var_1_1=0;\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void a() { }",                                                                                    "void fn_0_0()\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void abc() { }",                                                                                  "void fn_0_0()\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void _a1b2c3() { }",                                                                              "void fn_0_0()\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int a) { }",                                                                             "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int abc) { }",                                                                           "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int _a1b2c3) { }",                                                                       "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int web) { }",                                                                           "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int webgl) { }",                                                                         "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int Webgl_1) { }",                                                                       "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int gL_i) { }",                                                                          "void fn_0_0(int var_1_1)\n{\n}\n");
        TestParserInput(GLSLShaderType::Vertex,     GLSLTranslateOptions::DisableWriteInputs,   L"void foo(int _webgL_f) { }",                                                                      "void fn_0_0(int var_1_1)\n{\n}\n");

        // Cannot start with a number
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int 1a;", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void 1foo() {}", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int 1a) {}", E_GLSLERROR_SYNTAXERROR);

        // Cannot use a keyword (even ones we don't support)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int class", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void class() {}", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int class) {}", E_GLSLERROR_SYNTAXERROR);

        // Cannot start with 'gl_' or 'webgl_' or '_webgl_'
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void gl_foo() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int gl_foo;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int gl_arg) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void webgl_foo() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int webgl_foo;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int webgl_arg) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void _webgl_foo() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int _webgl_foo;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int _webgl_arg) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);

        // No double underscores allowed anywhere
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void __foo() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int __foo;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int __arg) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void f__oo() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int f__oo;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int a__rg) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo__() {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int foo__;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo(int arg__) {}", E_GLSLERROR_INVALIDIDENTIFIERNAME);
    }

    void BasicGLSLTests::ErrorLine()
    {
        // Try no #line
        TestParserInputNegativeErrorLine(GLSLShaderType::Vertex, 0, L"int 1a;", E_GLSLERROR_SYNTAXERROR, 1);

        // Try a basic syntax error and set the line on the first line
        TestParserInputNegativeErrorLine(GLSLShaderType::Vertex, 0, L"#line 100\nint 1a;", E_GLSLERROR_SYNTAXERROR, 100);

        // Set it on a line that is not the first line
        TestParserInputNegativeErrorLine(GLSLShaderType::Vertex, 0, L"int foo;\n#line 100\nint 1a;", E_GLSLERROR_SYNTAXERROR, 100);

        // Set it twice
        TestParserInputNegativeErrorLine(GLSLShaderType::Vertex, 0, L"#line 50\nint foo;\n#line 100\nint 1a;", E_GLSLERROR_SYNTAXERROR, 100);
    }

    void BasicGLSLTests::FrontFacingTests()
    {
        // Basic shader with no front facing should not generate it, and should work with both feature levels
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"void main() {}",                                                                                  "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\n}\n");
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::ForceFeatureLevel9,   L"void main() {}",                                                                                  "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\n}\n");

        // Shader that uses front facing should generate it, unless feature level 9 is used
        TestParserInput(GLSLShaderType::Fragment,   0,                                          L"void main() { if (gl_FrontFacing) { gl_FragColor = vec4(1.0, 0, 0, 1); } }",                      "struct PSInput\n{\n  float4 position : SV_Position;\n  bool frontFacing : SV_IsFrontFace;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\nif (gl_FrontFacing){\npsOutput.fragColor[0]=float4(1.000000e+000,0,0,1);\n}\n}\n");
        TestParserInputNegativeError(GLSLShaderType::Fragment, GLSLTranslateOptions::ForceFeatureLevel9, L"void main() { if (gl_FrontFacing) { gl_FragColor = vec4(1.0, 0, 0, 1); } }", E_GLSLERROR_INVALIDFRONTFACINGUSAGE);
    }

    void BasicGLSLTests::DepthLimitTests()
    {
        // Vertex and fragment shaders have a depth limit of ParseTreeNode::s_uMaxTreeDepth. These limits are constant regardless of feature level.
        const UINT uMaxIfStatementDepth = 32;
        const UINT uMaxFunctionNestingLevel = ParseTreeNode::GetMaxFunctionNestingLevel();
        const UINT uMaxStringSize = sizeof(wchar_t) * max(uMaxIfStatementDepth, uMaxFunctionNestingLevel) * 32;

        {
            // Test nested 'if' statements to ensure we error out if we get too deep.
            // Approximate initial size of string at approximately 32 characters per line. Append() will enlarge as necessary.
            CMutableString<wchar_t> strNestedIf(uMaxStringSize);

            strNestedIf.Append(L"void main(void){");

            for (int i = 0; i < uMaxIfStatementDepth; i++)
            {
                strNestedIf.Append(L"if(true) {");
            }

            for (int i = 0; i < uMaxIfStatementDepth; i++)
            {
                strNestedIf.Append(L"}");
            }

            strNestedIf.Append(L"}");

            TestParserInputNegativeError(GLSLShaderType::Fragment, 0, strNestedIf, E_GLSLERROR_SHADERCOMPLEXITY);
            TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strNestedIf, E_GLSLERROR_SHADERCOMPLEXITY);
        }

        {
            // For-loops can also introduce depth
            const UINT uMaxForStatementDepth = 40;
            CMutableString<wchar_t> strNestedFor(uMaxStringSize);

            strNestedFor.Append(L"void main(void){");

            CMutableString<wchar_t> arrayIndexExpression(uMaxStringSize);
            wchar_t arrayIndexVariableName = L'a';

            for (int i = 0; i < uMaxForStatementDepth; i++)
            {
                arrayIndexExpression.Format(uMaxStringSize, L"for(int i%d = 0; i%d < 1; i%d++) {", arrayIndexVariableName, arrayIndexVariableName, arrayIndexVariableName);
                strNestedFor.Append(arrayIndexExpression);
                arrayIndexVariableName++;
            }

            for (int i = 0; i < uMaxForStatementDepth; i++)
            {
                strNestedFor.Append(L"}");
            }

            strNestedFor.Append(L"}");

            TestParserInputNegativeError(GLSLShaderType::Fragment, 0, strNestedFor, E_GLSLERROR_SHADERCOMPLEXITY);
            TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strNestedFor, E_GLSLERROR_SHADERCOMPLEXITY);
        }

        {
            // Test ternary operator depth as well
            const UINT uMaxTernaryStatementDepth = 64;
            CMutableString<wchar_t> strNestedTernary(uMaxStringSize);

            strNestedTernary.Append(L"void main(void){ int i = ");

            for (int i = 0; i < uMaxTernaryStatementDepth; i++)
            {
                strNestedTernary.Append(L"(true ?");
            }

            // Append literal to terminate nesting
            strNestedTernary.Append(L"1");

            for (int i = 0; i < uMaxTernaryStatementDepth; i++)
            {
                strNestedTernary.Append(L": 0 )");
            }

            strNestedTernary.Append(L";}");

            TestParserInputNegativeError(GLSLShaderType::Fragment, 0, strNestedTernary, E_GLSLERROR_SHADERCOMPLEXITY);
            TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strNestedTernary, E_GLSLERROR_SHADERCOMPLEXITY);
        }

        {
            // Basic test for function nesting depth error code (behavioral verification in 
            // ft_webglrender!TestShaderCompile::VerifyFunctionNesting)
            CMutableString<wchar_t> strNestedFunctionShader(uMaxStringSize);
            CMutableString<wchar_t> strCurrentFunctionName(uMaxStringSize);
            for (UINT i = 0; i < uMaxFunctionNestingLevel; i++)
            {
                if (i == 0)
                {
                    // Leaf function does nothing - each successive function will call the previous one
                    strCurrentFunctionName.Format(uMaxStringSize, L"void foo_%i() {}", i);
                }
                else
                {
                    strCurrentFunctionName.Format(uMaxStringSize, L"void foo_%i() { foo_%i(); }", i, i - 1);
                }
                strNestedFunctionShader.Append(strCurrentFunctionName);
            }

            // Main counts as a nesting level, so calling foo_{uMaxFunctionNestingLevel - 1} should put us over the limit.
            strCurrentFunctionName.Format(uMaxStringSize, L"void main() { foo_%i(); }", uMaxFunctionNestingLevel - 1);
            strNestedFunctionShader.Append(strCurrentFunctionName);

            TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strNestedFunctionShader, E_GLSLERROR_MAXFUNCTIONDEPTHEXCEEDED);
        }
    }
    
    // Test limits on individually declared variables. Limits on arrays are in BasicGLSLTests::ArrayDeclarationTests.
    void BasicGLSLTests::VariableCountLimit()
    {
        // Test limit on uniforms
        const UINT uMaxUniformVariables = 129;
        // Approximate initial size of string at approximately 32 characters per line. Append() will enlarge as necessary.
        const UINT uMaxStringLength = sizeof(wchar_t) * uMaxUniformVariables * 32;

        CMutableString<wchar_t> strUniformDeclaration(uMaxStringLength);
        CMutableString<wchar_t> strCurrentVariableName(uMaxStringLength);

        for (int i = 0; i < uMaxUniformVariables; i++)
        {
            strCurrentVariableName.Format(uMaxStringLength, L"uniform mat4 foo_%i;", i);
            strUniformDeclaration.Append(strCurrentVariableName);
        }

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strUniformDeclaration, E_GLSLERROR_MAXUNIFORMEXCEEDED);

        // Test limit on varyings
        const UINT uMaxVaryingVariables = 32;
        CMutableString<wchar_t> strVaryingDeclaration(uMaxStringLength);

        for (int i = 0; i < uMaxVaryingVariables; i++)
        {
            strCurrentVariableName.Format(uMaxStringLength, L"varying mat4 foo_%i;", i);
            strVaryingDeclaration.Append(strCurrentVariableName);
        }
        strVaryingDeclaration.Append(L"void foo() {");
        for (int i = 0; i < uMaxVaryingVariables; i++)
        {
            strCurrentVariableName.Format(uMaxStringLength, L"foo_%i;", i);
            strVaryingDeclaration.Append(strCurrentVariableName);
        }
        strVaryingDeclaration.Append(L"}");

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strVaryingDeclaration, E_GLSLERROR_MAXVARYINGEXCEEDED);
        
        // Test limit on attributes
        const UINT uMaxAttributeVariables = 32;
        CMutableString<wchar_t> strAttributeDeclaration(uMaxStringLength);

        for (int i = 0; i < uMaxAttributeVariables; i++)
        {
            strCurrentVariableName.Format(uMaxStringLength, L"attribute mat4 foo_%i;", i);
            strAttributeDeclaration.Append(strCurrentVariableName);
        }
        strAttributeDeclaration.Append(L"void foo() {");
        for (int i = 0; i < uMaxAttributeVariables; i++)
        {
            strCurrentVariableName.Format(uMaxStringLength, L"foo_%i;", i);
            strAttributeDeclaration.Append(strCurrentVariableName);
        }
        strAttributeDeclaration.Append(L"}");

        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, strAttributeDeclaration, E_GLSLERROR_MAXATTRIBUTEEXCEEDED);
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   StructTests
    //
    //  Synopsis:   Tests various language constructs as they relate to structs
    //
    //-----------------------------------------------------------------------------
    void BasicGLSLTests::StructTests()
    {
        // Test basic type declarations in anonymous struct
        TestParserInputDefault(
            L"void foo() {"
            L"  struct { ivec2 a; } svar;"
            L"  svar.a = ivec2(3);"
            L"}",
            "struct anontype_0 {\n  int2 var_1_1;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_1=int2((3).xx);\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L"  struct { bvec3 a; } svar;"
            L"  svar.a = bvec3(true);"
            L"}",
            "struct anontype_0 {\n  bool3 var_1_1;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_1=bool3((true).xxx);\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L"  struct { mat3 a; } svar;"
            L"  svar.a = mat3(1.0);"
            L"}",
            "struct anontype_0 {\n  float3x3 var_1_1;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_1=GLSLmatrix3FromScalar(float(1.000000e+000));\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L"  struct { mat4 a; } svar;"
            L"  svar.a = mat4(1.0);"
            L"}",
            "struct anontype_0 {\n  float4x4 var_1_1;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_1=GLSLmatrix4FromScalar(float(1.000000e+000));\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L"  struct { vec2 a; } svar;"
            L"  svar.a = vec2(0.0);"
            L"}",
            "struct anontype_0 {\n  float2 var_1_1;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_1=float2((0.000000e+000).xx);\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L"  struct bar { "
            L"    int   a, aa[2]; ivec2 b, bb[2]; ivec3 c, cc[2]; ivec4 d, dd[2];"
            L"    bool  e, ee[2]; bvec2 f, ff[2]; bvec3 g, gg[2]; bvec4 h, hh[2];"
            L"    float i, ii[2]; vec2  j, jj[2]; vec3  k, kk[2]; vec4  l, ll[2];"
            L"    mat2  m, mm[2]; mat3  n, nn[2]; mat4  o, oo[2];"
            L"  } svar;"
            L"  svar.jj[0] = vec2(0.0);"
            L"}",
            "struct typename_2_1 {\n  int var_1_2, var_1_3[2];\n  int2 var_1_4, var_1_5[2];\n  int3 var_1_6, var_1_7[2];\n  int4 var_1_8, var_1_9[2];\n  bool var_1_10, var_1_11[2];\n  bool2 var_1_12, var_1_13[2];\n  bool3 var_1_14, var_1_15[2];\n  bool4 var_1_16, var_1_17[2];\n  float var_1_18, var_1_19[2];\n  float2 var_1_20, var_1_21[2];\n  float3 var_1_22, var_1_23[2];\n  float4 var_1_24, var_1_25[2];\n  float2x2 var_1_26, var_1_27[2];\n  float3x3 var_1_28, var_1_29[2];\n  float4x4 var_1_30, var_1_31[2];\n};\nvoid fn_0_0()\n{\ntypename_2_1 var_2_32=(typename_2_1)0;\nvar_2_32.var_1_21[0]=float2((0.000000e+000).xx);\n}\n"
            );

        // Test declarator list with array and non-arrays of the same basic type.
        TestParserInputDefault(
            L"void foo() {"
            L"  const int zz = 10;"
            L"  struct { int a, b[1], c, d[zz]; } svar;"
            L"  svar.a = 0;"
            L"  svar.b[0] = 0;"
            L"  svar.c = 1;"
            L"  svar.d[8] = 8;"
            L"}",
            "struct anontype_0 {\n  int var_1_2, var_1_3[1], var_1_4, var_1_5[10];\n};\nvoid fn_0_0()\n{\nconst int var_2_1=10;\nanontype_0 var_2_6=(anontype_0)0;\nvar_2_6.var_1_2=0;\nvar_2_6.var_1_3[0]=0;\nvar_2_6.var_1_4=1;\nvar_2_6.var_1_5[8]=8;\n}\n"
            );

        // Ensure struct fields check scope appropriately:
        // 'a' exists in another scope - we should be able to use it as a struct field name
        TestParserInputDefault(
            L"float a; void foo() { int a; struct { vec4 a; } svar; svar.a = vec4(1.0); }",
            "struct anontype_0 {\n  float4 var_1_0;\n};\nstatic float var_0_0=0.0;\nvoid fn_0_1()\n{\nint var_2_0=0;\nanontype_0 var_2_2=(anontype_0)0;\nvar_2_2.var_1_0=float4((1.000000e+000).xxxx);\n}\n"
            );

        // Despite not ever being able to return the correct type, the grammar allows anonymous structs for return types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { bool b; } foo() { return true; }", E_GLSLERROR_INCOMPATIBLERETURN);

        // Simlarly you can't call a function with a struct declaration in the parameter. Since you can't call it, we flag a compile error.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"bool foo(struct S { bool b; } svar) { return svar.b; }", E_GLSLERROR_STRUCTDECLINPARAMETER);

        // We can't have const anonymous structs since they can't be initialized in GLSL
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const struct { bool b; } svar; }", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);

        // Const should make struct variables non-lvalue
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const struct S { bool b; } svar = S(true); svar.b = false; }", E_GLSLERROR_INVALIDLVALUE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const struct S { vec4 v; } svar = S(vec4(0.0)); svar.v.x = 1.0; }", E_GLSLERROR_INVALIDLVALUE);

        // Invalid type for field selection (svar.v is an array)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct { vec4 v[10]; } svar; svar.v.x = 1.0; }", E_GLSLERROR_INVALIDTYPEFORFIELDSELECTION);

        // You cannot have duplicate names in a declarator list or declaration list
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { vec4 v, w, v[2]; } svar;", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { vec4 v; vec3 v[2]; } svar;", E_GLSLERROR_IDENTIFIERALREADYDECLARED);

        // Array declarators must be const in structs
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform bool f; void foo() { int i = (f) ? 1 : 2; struct { int j[i]; } svar; }", E_GLSLERROR_ARRAYDECLARATORNOTCONST);

        // Array size must be > 0 to be valid declarator
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int i, j[0]; } svar;", E_GLSLERROR_ARRAYSIZELEQZERO);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int j[-1]; } svar;", E_GLSLERROR_ARRAYSIZELEQZERO);

        // 'attribute' and 'varying' are disallowed for struct types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"attribute struct { vec4 v; } svar;", E_GLSLERROR_INVALIDTYPEQUALIFIERFORSTRUCT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"varying struct { vec4 v; } svar;", E_GLSLERROR_INVALIDTYPEQUALIFIERFORSTRUCT);

        // 'uniform' structs are allowed
        TestParserInputDefault(
            L"uniform struct { vec4 v; } svar;",
            "struct anontype_0 {\n  float4 var_1_0;\n};\nanontype_0 var_0_1=(anontype_0)0;\n"
            );

        // ...but they do contribute to the max uniform count. This should safely put us over the limit
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"uniform struct { float f1, f2, f3, f4[10000]; } svar;", E_GLSLERROR_MAXUNIFORMEXCEEDED);

        // Ensure you cannot use a name that doesn't exist on the type for field selection
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int a; } svar; void foo() {svar.b = 1;}", E_GLSLERROR_INVALIDSTRUCTFIELDNAME);

        // Ensure type for field selection is correctly propagated
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int a; } svar; void foo() {svar.a = true;}", E_GLSLERROR_INCOMPATIBLETYPES);

        // void and array of void should be prohibited (no custom error as of now)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { void a; } svar;", E_GLSLERROR_VARIABLETYPEVOID);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { void a[2]; } svar;", E_GLSLERROR_VARIABLETYPEVOID);

        // Sampler types are not allowed in structs
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"struct { sampler2D samp; } svar;", E_GLSLERROR_SAMPLERNOTALLOWEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"struct { samplerCube samp; } svar;", E_GLSLERROR_SAMPLERNOTALLOWEDFORSTRUCTS);

        // Can't declare void typed members
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"struct { void a; } svar;", E_GLSLERROR_VARIABLETYPEVOID);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"struct { void a[2]; } svar;", E_GLSLERROR_VARIABLETYPEVOID);

        // Ensure you can't have illegal names for typenames
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct __foo { int bar; } svar;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct f__oo { int bar; } svar;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct gl_foo { int bar; } svar;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct webgl_foo { int bar; } svar;", E_GLSLERROR_INVALIDIDENTIFIERNAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct _webgl_foo { int bar; } svar;", E_GLSLERROR_INVALIDIDENTIFIERNAME);

        // These should be legal though
        TestParserInputDefault(
            L"struct web_foo { int bar; } svar;",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\n"
            );
        TestParserInputDefault(
            L"struct gL_foo { int bar; } svar;",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\n"
            );

        // Basic tests for named struct types
        // Simple declaration and field selection
        TestParserInputDefault(
            L"struct S { int x; }; void foo() { S s; s.x = 1; }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nvoid fn_0_2()\n{\ntypename_0_0 var_2_3=(typename_0_0)0;\nvar_2_3.var_1_1=1;\n}\n"
            );

        // Basic constructor then field assignment
        TestParserInputDefault(
            L"struct S { int x; }; void foo() { S s = S(10); s.x = 1; }",
            "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\nvoid fn_0_2()\n{\ntypename_0_0 var_2_3=ctor_typename_0_0(10);\nvar_2_3.var_1_1=1;\n}\n"
            );

        // Test case from the spec: if the typename is not defined in the current scope, we should
        // be able to use the typename for type declaration, variable name, and ctor. Once finished
        // the identifier should only refer to the variable.
        TestParserInputDefault(
            L"struct S { int S; }; void foo() { S S = S(10); S.S = 1; }",
            "struct typename_0_0 {\n  int var_1_0;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_0) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_0=var_1_0;\nreturn var_localstruct;\n}\nvoid fn_0_1()\n{\ntypename_0_0 var_2_0=ctor_typename_0_0(10);\nvar_2_0.var_1_0=1;\n}\n"
            );
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; void foo() { S S = S(10); S x; }", E_GLSLERROR_UNDECLAREDTYPENAME);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; void foo() { S S = S(10); S = S(1); }", E_GLSLERROR_INVALIDARGUMENTS);

        // Struct constructor types must match exactly in count and type
        // Too few arguments is an error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x, y; }; void foo() { S S = S(1); }", E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR);

        // Too many arguments is an error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; void foo() { S S = S(1, 2); }", E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR);

        // Types not matching is an error
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; void foo() { S S = S(true); }", E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR);

        // Constructor with nested structs and declarator field with array
        TestParserInputDefault(
            L"struct S1 { int a, b[2]; };"
            L"struct S2 { S1 s1; }; "
            L"struct S3 { S2 s2; }; "
            L"void foo() {"
            L"  int zz[2]; zz[1] = 5; "
            L"  S3 s3 = S3(S2(S1(3,zz)));"
            L"}",
            "struct typename_0_0 {\n  int var_1_1, var_1_2[2];\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1, int var_1_2[2]) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nstruct typename_0_3 {\n  typename_0_0 var_2_4;\n};\ntypename_0_3 ctor_typename_0_3(typename_0_0 var_2_4) {\ntypename_0_3 var_localstruct;\nvar_localstruct.var_2_4=var_2_4;\nreturn var_localstruct;\n}\nstruct typename_0_5 {\n  typename_0_3 var_3_6;\n};\ntypename_0_5 ctor_typename_0_5(typename_0_3 var_3_6) {\ntypename_0_5 var_localstruct;\nvar_localstruct.var_3_6=var_3_6;\nreturn var_localstruct;\n}\nvoid fn_0_7()\n{\nint var_4_8[2]=(int[2])0;\nvar_4_8[1]=5;\ntypename_0_5 var_4_9=ctor_typename_0_5(ctor_typename_0_3(ctor_typename_0_0(3,var_4_8)));\n}\n"
            );

        // Ensure structs get lifted to the global scope in source order. This test also ensure that the 'empty'
        // type specifier doesn't output anything either - only the lifted struct declarations are output.
        // Types that don't use their constructors shouldn't output anything either.
        TestParserInputDefault(
            L"void foo() { "
            L"  struct S1 { bvec3 a; }; { "
            L"    struct S1 { bvec3 a; }; { "
            L"      struct S1 { bvec3 a; }; { "
            L"        struct S1 { bvec3 a; }; struct S2 { ivec2 b; };"
            L"      } "
            L"      struct S2 { ivec2 b; };"
            L"    }"
            L"    struct S2 { ivec2 b; };"
            L"  }"
            L"  struct S2 { ivec2 b; };"
            L"}",
            "struct typename_9_1 {\n  bool3 var_1_2;\n};\nstruct typename_10_1 {\n  bool3 var_2_2;\n};\nstruct typename_11_1 {\n  bool3 var_3_2;\n};\nstruct typename_12_1 {\n  bool3 var_4_2;\n};\nstruct typename_12_3 {\n  int2 var_5_4;\n};\nstruct typename_11_3 {\n  int2 var_6_4;\n};\nstruct typename_10_3 {\n  int2 var_7_4;\n};\nstruct typename_9_3 {\n  int2 var_8_4;\n};\nvoid fn_0_0()\n{\n{\n{\n{\n}\n}\n}\n}\n"
            );

        // Struct ctor as parameter
        TestParserInputDefault(
            L"struct A { int a; };"
            L"void foo(A a) { a.a += 1; }"
            L"void bar() { foo(A(1)); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\nvoid fn_0_2(typename_0_0 var_2_1)\n{\nvar_2_1.var_1_1+=1;\n}\nvoid fn_0_3()\n{\nfn_0_2(ctor_typename_0_0(1));\n}\n"
            );

        // Struct ctor is not an lvalue (can't be used with out param)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0,
            L"struct A { int a; };"
            L"void foo(out A a) { a.a += 1; }"
            L"void bar() { foo(A(1)); }",
            E_GLSLERROR_INVALIDLVALUE
            );

        // Test struct type as function return value
        TestParserInputDefault(
            L"struct A { int a; };"
            L"A foo(int a) { return A(a); }"
            L"void bar() { A returnedVal = foo(4); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\ntypename_0_0 fn_0_2(int var_2_1)\n{\nreturn ctor_typename_0_0(var_2_1);\n}\nvoid fn_0_3()\n{\ntypename_0_0 var_3_4=fn_0_2(4);\n}\n"
            );

        // Name hiding - the typename of the innermost declared type should win
        TestParserInputDefault(
            L"struct S { int x; }; void foo() { struct S { int y; } ss; S s; s.y = 1; }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstruct typename_3_0 {\n  int var_2_3;\n};\nvoid fn_0_2()\n{\ntypename_3_0 var_3_4=(typename_3_0)0;\ntypename_3_0 var_3_5=(typename_3_0)0;\nvar_3_5.var_2_3=1;\n}\n"
            );

        // ...and thus not allow '.x' for field selection
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; }; void foo() { struct S { int y; } ss; S s; s.x = 1; }", E_GLSLERROR_INVALIDSTRUCTFIELDNAME);

        // Test function calls with struct type as normal and 'out' parameters. 
        TestParserInputDefault(
            L"struct A { int a; };"
            L"void foo(A a) { a.a += 1; }"
            L"void bar() { A b; foo(b); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nvoid fn_0_2(typename_0_0 var_2_1)\n{\nvar_2_1.var_1_1+=1;\n}\nvoid fn_0_3()\n{\ntypename_0_0 var_3_4=(typename_0_0)0;\nfn_0_2(var_3_4);\n}\n"
            );
        TestParserInputDefault(
            L"struct A { int a; };"
            L"void foo(out A a) { a.a += 1; }"
            L"void bar() { A b; foo(b); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nvoid fn_0_2(inout typename_0_0 var_2_1)\n{\nvar_2_1.var_1_1+=1;\n}\nvoid fn_0_3()\n{\ntypename_0_0 var_3_4=(typename_0_0)0;\nfn_0_2(var_3_4);\n}\n"
            );

        // Test struct type as function return value
        TestParserInputDefault(
            L"struct A { int a; };"
            L"A foo(int a) { A local; local.a = a; return local; }"
            L"void bar() { A returnedVal = foo(4); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 fn_0_2(int var_2_1)\n{\ntypename_0_0 var_2_3=(typename_0_0)0;\nvar_2_3.var_1_1=var_2_1;\nreturn var_2_3;\n}\nvoid fn_0_4()\n{\ntypename_0_0 var_3_5=fn_0_2(4);\n}\n"
            );

        // Test struct types used in struct declarations
        TestParserInputDefault(
            L"struct A { int a; } svar;"
            L"struct B { A a; float aa; } svar2;"
            L"struct C { B b; } svar3;"
            L"void foo() { svar3.b.a.a = svar2.a.a = svar.a = 0; }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstruct typename_0_3 {\n  typename_0_0 var_2_1;\n  float var_2_4;\n};\nstruct typename_0_6 {\n  typename_0_3 var_3_7;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\nstatic typename_0_3 var_0_5=(typename_0_3)0;\nstatic typename_0_6 var_0_8=(typename_0_6)0;\nvoid fn_0_9()\n{\nvar_0_8.var_3_7.var_2_1.var_1_1=var_0_5.var_2_1.var_1_1=var_0_2.var_1_1=0;\n}\n"
            );

        // Typename can be declared as long as there is no existing identifier with the same name defined in the nearest scope.
        TestParserInputDefault(
            L"void foo() {"
            L"  struct S { int s; };"
            L"  {   "
            L"    struct S { S s; };"
            L"    {   "
            L"      struct S { S s; } baz;"
            L"      baz.s.s.s = 1;"
            L"    }"
            L"  }"
            L"}",
            "struct typename_4_1 {\n  int var_1_2;\n};\nstruct typename_5_1 {\n  typename_4_1 var_2_2;\n};\nstruct typename_6_1 {\n  typename_5_1 var_3_2;\n};\nvoid fn_0_0()\n{\n{\n{\ntypename_6_1 var_6_3=(typename_6_1)0;\nvar_6_3.var_3_2.var_2_2.var_1_2=1;\n}\n}\n}\n"
            );

        // "All previously visible variables, types, constructors, or functions with that [struct type] name are hidden."
        //      variables:
        TestParserInputDefault(
            L"int foo; void bar() { struct foo { vec4 a; } bar; }",
            "struct typename_2_0 {\n  float4 var_1_2;\n};\nstatic int var_0_0=0;\nvoid fn_0_1()\n{\ntypename_2_0 var_2_1=(typename_2_0)0;\n}\n"
            );

        //      types:
        TestParserInputDefault(
            L"struct stype { int a; } svar; void foo() { struct stype { bool b; } svar;}",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstruct typename_3_0 {\n  bool var_2_4;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\nvoid fn_0_3()\n{\ntypename_3_0 var_3_2=(typename_3_0)0;\n}\n"
            );

        //      constructors:
        TestParserInputDefault(
            L"struct stype { int a; }; void foo() { struct stype { bool b; }; stype(true); }",
            "struct typename_0_0 {\n  int var_1_1;\n};\nstruct typename_3_0 {\n  bool var_2_3;\n};\ntypename_3_0 ctor_typename_3_0(bool var_2_3) {\ntypename_3_0 var_localstruct;\nvar_localstruct.var_2_3=var_2_3;\nreturn var_localstruct;\n}\nvoid fn_0_2()\n{\nctor_typename_3_0(true);\n}\n"
            );

        //      functions:
        // typename declaration correctly masks the function name (i.e. when we do have ctor support we'll already have the right identifier).
        TestParserInputDefault(
            L"int foo(int a) { return a + 1; } void bar() { int baz = foo(1); struct foo { int a; } svar = foo(1); }",
            "struct typename_3_0 {\n  int var_2_1;\n};\ntypename_3_0 ctor_typename_3_0(int var_2_1) {\ntypename_3_0 var_localstruct;\nvar_localstruct.var_2_1=var_2_1;\nreturn var_localstruct;\n}\nint fn_0_0(int var_1_1)\n{\nreturn var_1_1+1;\n}\nvoid fn_0_2()\n{\nint var_3_3=fn_0_0(1);\ntypename_3_0 var_3_4=ctor_typename_3_0(1);\n}\n"
            );

        // "Structures must have at least one member declaration."
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { } svar;", E_GLSLERROR_SYNTAXERROR);

        // Struct constructor call can't match with no arguments
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int a; } s = S();}", E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR);

        // "Member declarations may contain precision qualifiers..."
        TestParserInputDefault(
            L"struct { lowp float a;};",
            "struct anontype_0 {\n  float var_1_0;\n};\n"
            );

        TestParserInputDefault(
            L"struct { mediump float b;};",
            "struct anontype_0 {\n  float var_1_0;\n};\n"
            );

        TestParserInputDefault(
            L"struct { highp float c;};",
            "struct anontype_0 {\n  float var_1_0;\n};\n"
            );

        // "... but may not contain any other qualifiers"
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { const int foo; };", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { uniform int foo; };", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { attribute int foo; };", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { varying int foo; };", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { badqualifier int foo; } svar;", E_GLSLERROR_SYNTAXERROR);  

        // "Member types must already be defined (there are no forward references..."
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { svarB foo; } svarA; struct svarB { int bar; };", E_GLSLERROR_UNDECLAREDTYPENAME);

        // "...and no embedded definitions)."
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { struct { int bar; } svarB; } svarA;", E_GLSLERROR_NESTEDSTRUCTSINVALID);

        // "Member declarations cannot contain initializers."
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int foo = 5; } svar;", E_GLSLERROR_SYNTAXERROR);

        // "Member declarations can contain arrays."
        TestParserInputDefault(
            L"struct { "
            L"int a[5]; float b[5]; bool c[5];"
            L"vec2 d[5]; vec3 e[5]; vec4 f[5];"
            L"bvec2 j[5]; bvec3 k[5]; bvec4 z[5];"
            L"ivec2 g[5]; ivec3 h[5];ivec4 i[5];"
            L"mat2 jj[5]; mat3 kk[5]; mat4 l[5];"
            L"} a;",
            "struct anontype_0 {\n  int var_1_0[5];\n  float var_1_1[5];\n  bool var_1_2[5];\n  float2 var_1_3[5];\n  float3 var_1_4[5];\n  float4 var_1_5[5];\n  bool2 var_1_6[5];\n  bool3 var_1_7[5];\n  bool4 var_1_8[5];\n  int2 var_1_9[5];\n  int3 var_1_10[5];\n  int4 var_1_11[5];\n  float2x2 var_1_12[5];\n  float3x3 var_1_13[5];\n  float4x4 var_1_14[5];\n};\nstatic anontype_0 var_0_0=(anontype_0)0;\n"
            );

        // "Such arrays must have a size specified, and the size specified must be an integral constant that's greater than zero"
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int foo[]; };", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"int arrraySize = 10; struct { int foo[arrraySize + 1]; };", E_GLSLERROR_ARRAYDECLARATORNOTCONST);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int foo[0]; };", E_GLSLERROR_ARRAYSIZELEQZERO);

        // "Each level of structure has its own name space for names given in member declarators..."
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct { int foo; float foo;};", E_GLSLERROR_IDENTIFIERALREADYDECLARED);
        
        // "...such names need only be unique within that name space.
        TestParserInputDefault(
            L"struct { int a; } svarA; struct { int a; } svarb;",
            "struct anontype_0 {\n  int var_1_0;\n};\nstruct anontype_1 {\n  int var_2_0;\n};\nstatic anontype_0 var_0_1=(anontype_0)0;\nstatic anontype_1 var_0_2=(anontype_1)0;\n"
            );

        // "Anonymous structure declarators are not allowed"
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int x; } svar; struct T {S; int y;};", E_GLSLERROR_SYNTAXERROR);

        // const struct that contains an array (with ctor or assignment init).
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { int iArr[2]; struct S { int i[2]; }; const S s = S(iArr); }", E_GLSLERROR_NONCONSTINITIALIZER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i[2]; }; S nonConstS; const S s = nonConstS; }", E_GLSLERROR_NONCONSTINITIALIZER);

        // const struct initialization requires const ctor params
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i, j; }; int a, b; const S s = S(a, b); }", E_GLSLERROR_NONCONSTINITIALIZER);

        // const structs require an initializer
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; }; const S s; }", E_GLSLERROR_CONSTREQUIRESINITIALIZATION);

        // Ensure exceeding the max nesting level flags an error
        TestParserInputNegativeError(
            GLSLShaderType::Vertex,
            0,
            L"struct One { bool f; };"
            L"struct Two { One one[2]; };"
            L"struct Three { Two two[2]; };"
            L"struct Four { Three three[2]; };"
            L"struct Five { One one; Two two; Three three; Four four[2]; };",
            E_GLSLERROR_MAXSTRUCTNESTINGEXCEEDED
            );

        // Ensure we allow exactly the max nesting level.
        TestParserInputDefault(
            L"void foo() {"
            L"  struct One { bool f; };"
            L"  struct Two { One one[2]; };"
            L"  struct Three { Two two[2]; };"
            L"  struct Four { Three three[2]; };"
            L"  Four four[4];"
            L"}",
            "struct typename_5_1 {\n  bool var_1_2;\n};\nstruct typename_5_3 {\n  typename_5_1 var_2_4[2];\n};\nstruct typename_5_5 {\n  typename_5_3 var_3_6[2];\n};\nstruct typename_5_7 {\n  typename_5_5 var_4_8[2];\n};\nvoid fn_0_0()\n{\ntypename_5_7 var_5_9[4]=(typename_5_7[4])0;\n}\n"
            );

        // Const arrays of structs are not allowed
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { struct S { int i; }; const S s[3]; }", E_GLSLERROR_INVALIDARRAYQUALIFIER);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"void foo() { const struct S { int i; } s[3]; }", E_GLSLERROR_INVALIDARRAYQUALIFIER);

        // Test various initializers in a single declarator list
        TestParserInputDefault(
            L"void foo() { struct S { int i; }; S sOrig; S s0 = sOrig, s1 = S(1), s2[2], s3; }", 
            "struct typename_2_1 {\n  int var_1_2;\n};\ntypename_2_1 ctor_typename_2_1(int var_1_2) {\ntypename_2_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nvoid fn_0_0()\n{\ntypename_2_1 var_2_3=(typename_2_1)0;\ntypename_2_1 var_2_4=var_2_3, var_2_5=ctor_typename_2_1(1), var_2_6[2]=(typename_2_1[2])0, var_2_7=(typename_2_1)0;\n}\n"
            );

        // Basic tests for the three supported binary operators for struct types. Test these with
        // anonymous structs to ensure proper declaration lifting and replacement typename identifier node.
        TestParserInputDefault(
            L"void foo() { struct {int a; int b;} a, b; bool x = a == b; }",
            "struct anontype_0 {\n  int var_1_1;\n  int var_1_2;\n};\nbool eq_anontype_0(anontype_0 val1, anontype_0 val2) {\nreturn (val1.var_1_1==val2.var_1_1 && val1.var_1_2==val2.var_1_2);\n}\nvoid fn_0_0()\n{\nanontype_0 var_2_1=(anontype_0)0, var_2_2=(anontype_0)0;\nbool var_2_3=eq_anontype_0(var_2_1,var_2_2);\n}\n"
            );
        TestParserInputDefault(
            L"void foo() { struct {int a; int b;} a, b; bool x = (a != b); }",
            "struct anontype_0 {\n  int var_1_1;\n  int var_1_2;\n};\nbool eq_anontype_0(anontype_0 val1, anontype_0 val2) {\nreturn (val1.var_1_1==val2.var_1_1 && val1.var_1_2==val2.var_1_2);\n}\nvoid fn_0_0()\n{\nanontype_0 var_2_1=(anontype_0)0, var_2_2=(anontype_0)0;\nbool var_2_3=(!eq_anontype_0(var_2_1,var_2_2));\n}\n"
            );
        TestParserInputDefault(
            L"void foo() { struct {int a; int b;} a, b; a = b; }",
            "struct anontype_0 {\n  int var_1_1;\n  int var_1_2;\n};\nvoid fn_0_0()\n{\nanontype_0 var_2_1=(anontype_0)0, var_2_2=(anontype_0)0;\nvar_2_1=var_2_2;\n}\n"
            );

        // Ensure operators that are not valid for structs are flagged as such.
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a / b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a * b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a + b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a - b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a < b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a > b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a <= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a >= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {bool a;}; void foo() { S a, b; a && b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {bool a;}; void foo() { S a, b; a || b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {bool a;}; void foo() { S a, b; a ^^ b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a *= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a /= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a %= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a += b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a -= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a >>= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a <<= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a &= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a |= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a, b; a ^= b; }", E_GLSLERROR_OPUNDEFINEDFORSTRUCTS);

        // Struct types with array fields have no operators defined 
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a[2];}; void foo() { S a, b; a = b; }", E_GLSLERROR_OPUNDEFINEDFORTYPESWITHARRAYS);

        // Ensure incompatible types are flagged as such
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a; int b;      a == b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { S a; int b[2];   a == b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { int a; int b[2]; a == b; }", E_GLSLERROR_INCOMPATIBLETYPES);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { int a[2], b[2];  a == b; }", E_GLSLERROR_INCOMPATIBLETYPES);

        // Ensure struct assignment operator requires an lvalue
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;}; void foo() { const S s = S(1); s = S(2); }", E_GLSLERROR_INVALIDLVALUE);

        // Ensure struct assignment operator can assign from rvalue
        TestParserInputDefault(
            L"struct S {int a;}; void foo() { const S s1 = S(1); S s2 = s1; }",
            "struct typename_0_0 {\n  int var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(int var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\nvoid fn_0_2()\n{\nconst typename_0_0 var_2_3=ctor_typename_0_0(1);\ntypename_0_0 var_2_4=var_2_3;\n}\n"
            );

        // Types are only equivalent if they refer to the exact same type.
        TestParserInputNegativeError(
            GLSLShaderType::Vertex, 0,
            L"struct S {int a;};"
            L"void foo() {"
            L"  S a;"
            L"  {"
            L"    struct S { int a; };"
            L"    S b;"
            L"    a == b;"
            L"  }"
            L"}",
            E_GLSLERROR_INCOMPATIBLETYPES
            );
        TestParserInputNegativeError(
            GLSLShaderType::Vertex, 0,
            L"struct S {int a;};"
            L"void foo() {"
            L"  S a;"
            L"  {"
            L"    struct S { int a; };"
            L"    S b;"
            L"    a = b;"
            L"  }"
            L"}",
            E_GLSLERROR_INCOMPATIBLETYPES
            );

        // Exercise some of the more complex output combinations
        TestParserInputDefault(
            L"void foo() {"
            L" struct S {int a;};"
            L" S a, b, c;"
            L" int x;"
            L" x = (a.a = 1, b.a = 1, c = b).a;"
            L"}",
            "struct typename_2_1 {\n  int var_1_2;\n};\nvoid fn_0_0()\n{\ntypename_2_1 var_2_2=(typename_2_1)0, var_2_3=(typename_2_1)0, var_2_4=(typename_2_1)0;\nint var_2_5=0;\nvar_2_5=(var_2_2.var_1_2=1,var_2_3.var_1_2=1,var_2_4=var_2_3).var_1_2;\n}\n"
            );
        TestParserInputDefault(
            L"void main() { "
            L" struct S {int a;};"
            L" S a, b, c;"
            L" bool fx = (a.a = 1, b.a = 1, c = b) == (b.a = 2, a.a = 2, a = c);"
            L"}",
            "struct typename_2_0 {\n  int var_1_1;\n};\nbool eq_typename_2_0(typename_2_0 val1, typename_2_0 val2) {\nreturn (val1.var_1_1==val2.var_1_1);\n}\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\ntypename_2_0 var_2_1=(typename_2_0)0, var_2_2=(typename_2_0)0, var_2_3=(typename_2_0)0;\nbool var_2_4=eq_typename_2_0((var_2_1.var_1_1=1,var_2_2.var_1_1=1,var_2_3=var_2_2),(var_2_2.var_1_1=2,var_2_1.var_1_1=2,var_2_1=var_2_3));\n}\n"
            );
        TestParserInputDefault(
            L"void foo() {"
            L" struct S {int a;};"
            L" S a, b, c;"
            L" bool x;"
            L" x = (a.a = 1, b.a = 1, c != b) == true;"
            L"}",
            "struct typename_2_1 {\n  int var_1_2;\n};\nbool eq_typename_2_1(typename_2_1 val1, typename_2_1 val2) {\nreturn (val1.var_1_2==val2.var_1_2);\n}\nvoid fn_0_0()\n{\ntypename_2_1 var_2_2=(typename_2_1)0, var_2_3=(typename_2_1)0, var_2_4=(typename_2_1)0;\nbool var_2_5=false;\nvar_2_5=(var_2_2.var_1_2=1,var_2_3.var_1_2=1,!eq_typename_2_1(var_2_4,var_2_3))==true;\n}\n"
            );

        // Verify that nested structs are comparable and that used ctors are generated...
        TestParserInputDefault(
            L"bool foo() {"
            L" struct S1 {int a;};"
            L" struct S2 {S1 s1; float a, b;};"
            L" struct S3 {mat2 m; S2 s2; };"
            L" S3 a = S3(mat2(1), S2(S1(4), 1.0, 2.0)), b;"
            L" return a == b;"
            L"}",
            "struct typename_4_1 {\n  int var_1_2;\n};\ntypename_4_1 ctor_typename_4_1(int var_1_2) {\ntypename_4_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nbool eq_typename_4_1(typename_4_1 val1, typename_4_1 val2) {\nreturn (val1.var_1_2==val2.var_1_2);\n}\nstruct typename_4_3 {\n  typename_4_1 var_2_4;\n  float var_2_2, var_2_5;\n};\ntypename_4_3 ctor_typename_4_3(typename_4_1 var_2_4, float var_2_2, float var_2_5) {\ntypename_4_3 var_localstruct;\nvar_localstruct.var_2_4=var_2_4;\nvar_localstruct.var_2_2=var_2_2;\nvar_localstruct.var_2_5=var_2_5;\nreturn var_localstruct;\n}\nbool eq_typename_4_3(typename_4_3 val1, typename_4_3 val2) {\nreturn (eq_typename_4_1(val1.var_2_4,val2.var_2_4) && val1.var_2_2==val2.var_2_2 && val1.var_2_5==val2.var_2_5);\n}\nstruct typename_4_6 {\n  float2x2 var_3_7;\n  typename_4_3 var_3_8;\n};\ntypename_4_6 ctor_typename_4_6(float2x2 var_3_7, typename_4_3 var_3_8) {\ntypename_4_6 var_localstruct;\nvar_localstruct.var_3_7=var_3_7;\nvar_localstruct.var_3_8=var_3_8;\nreturn var_localstruct;\n}\nbool eq_typename_4_6(typename_4_6 val1, typename_4_6 val2) {\nreturn (all(val1.var_3_7==val2.var_3_7) && eq_typename_4_3(val1.var_3_8,val2.var_3_8));\n}\nbool fn_0_0()\n{\ntypename_4_6 var_4_2=ctor_typename_4_6(GLSLmatrix2FromScalar(float(1)),ctor_typename_4_3(ctor_typename_4_1(4),1.000000e+000,2.000000e+000)), var_4_5=(typename_4_6)0;\nreturn eq_typename_4_6(var_4_2,var_4_5);\n}\n"
            );
        TestParserInputDefault(
            L"bool foo() {"
            L" struct S1 {int a;};"
            L" struct S2 {S1 s1; float a, b;};"
            L" struct S3 {mat2 m; S2 s2; };"
            L" S3 a, b = S3(mat2(1), S2(S1(4), 1.0, 2.0));"
            L" return a != b;"
            L"}",
            "struct typename_4_1 {\n  int var_1_2;\n};\ntypename_4_1 ctor_typename_4_1(int var_1_2) {\ntypename_4_1 var_localstruct;\nvar_localstruct.var_1_2=var_1_2;\nreturn var_localstruct;\n}\nbool eq_typename_4_1(typename_4_1 val1, typename_4_1 val2) {\nreturn (val1.var_1_2==val2.var_1_2);\n}\nstruct typename_4_3 {\n  typename_4_1 var_2_4;\n  float var_2_2, var_2_5;\n};\ntypename_4_3 ctor_typename_4_3(typename_4_1 var_2_4, float var_2_2, float var_2_5) {\ntypename_4_3 var_localstruct;\nvar_localstruct.var_2_4=var_2_4;\nvar_localstruct.var_2_2=var_2_2;\nvar_localstruct.var_2_5=var_2_5;\nreturn var_localstruct;\n}\nbool eq_typename_4_3(typename_4_3 val1, typename_4_3 val2) {\nreturn (eq_typename_4_1(val1.var_2_4,val2.var_2_4) && val1.var_2_2==val2.var_2_2 && val1.var_2_5==val2.var_2_5);\n}\nstruct typename_4_6 {\n  float2x2 var_3_7;\n  typename_4_3 var_3_8;\n};\ntypename_4_6 ctor_typename_4_6(float2x2 var_3_7, typename_4_3 var_3_8) {\ntypename_4_6 var_localstruct;\nvar_localstruct.var_3_7=var_3_7;\nvar_localstruct.var_3_8=var_3_8;\nreturn var_localstruct;\n}\nbool eq_typename_4_6(typename_4_6 val1, typename_4_6 val2) {\nreturn (all(val1.var_3_7==val2.var_3_7) && eq_typename_4_3(val1.var_3_8,val2.var_3_8));\n}\nbool fn_0_0()\n{\ntypename_4_6 var_4_2=(typename_4_6)0, var_4_5=ctor_typename_4_6(GLSLmatrix2FromScalar(float(1)),ctor_typename_4_3(ctor_typename_4_1(4),1.000000e+000,2.000000e+000));\nreturn !eq_typename_4_6(var_4_2,var_4_5);\n}\n"
            );

        // ... and assignable
        TestParserInputDefault(
            L"void foo() {"
            L" struct S1 {int a;};"
            L" struct S2 {S1 s1; float a, b;};"
            L" struct S3 {mat2 m; S2 s2; };"
            L" S3 a, b;"
            L" a.s2.s1.a = 3;"
            L" a = b;"
            L"}",
            "struct typename_4_1 {\n  int var_1_2;\n};\nstruct typename_4_3 {\n  typename_4_1 var_2_4;\n  float var_2_2, var_2_5;\n};\nstruct typename_4_6 {\n  float2x2 var_3_7;\n  typename_4_3 var_3_8;\n};\nvoid fn_0_0()\n{\ntypename_4_6 var_4_2=(typename_4_6)0, var_4_5=(typename_4_6)0;\nvar_4_2.var_3_8.var_2_4.var_1_2=3;\nvar_4_2=var_4_5;\n}\n"
            );

        // Ensure equals function generation does the correct comparison for each basic type.
        TestParserInputDefault(
            L"bool foo() {"
            L"  struct S { "
            L"    int   a; ivec2 b; ivec3 c; ivec4 d;"
            L"    bool  e; bvec2 f; bvec3 g; bvec4 h;"
            L"    float i; vec2  j; vec2  k; vec2  l;"
            L"    mat2  m; mat3  n; mat4  o;"
            L"  } s1;"
            L"  S s2;"
            L"  return s1 == s2;"
            L"}",
            "struct typename_2_1 {\n  int var_1_2;\n  int2 var_1_3;\n  int3 var_1_4;\n  int4 var_1_5;\n  bool var_1_6;\n  bool2 var_1_7;\n  bool3 var_1_8;\n  bool4 var_1_9;\n  float var_1_10;\n  float2 var_1_11;\n  float2 var_1_12;\n  float2 var_1_13;\n  float2x2 var_1_14;\n  float3x3 var_1_15;\n  float4x4 var_1_16;\n};\nbool eq_typename_2_1(typename_2_1 val1, typename_2_1 val2) {\nreturn (val1.var_1_2==val2.var_1_2 && all(val1.var_1_3==val2.var_1_3) && all(val1.var_1_4==val2.var_1_4) && all(val1.var_1_5==val2.var_1_5) && val1.var_1_6==val2.var_1_6 && all(val1.var_1_7==val2.var_1_7) && all(val1.var_1_8==val2.var_1_8) && all(val1.var_1_9==val2.var_1_9) && val1.var_1_10==val2.var_1_10 && all(val1.var_1_11==val2.var_1_11) && all(val1.var_1_12==val2.var_1_12) && all(val1.var_1_13==val2.var_1_13) && all(val1.var_1_14==val2.var_1_14) && all(val1.var_1_15==val2.var_1_15) && all(val1.var_1_16==val2.var_1_16));\n}\nbool fn_0_0()\n{\ntypename_2_1 var_2_17=(typename_2_1)0;\ntypename_2_1 var_2_18=(typename_2_1)0;\nreturn eq_typename_2_1(var_2_17,var_2_18);\n}\n"
            );

        // You should be able to declare a struct_specifier with just a type_qualifier and no variable.
        // The type_qualifier should have no effect on the output
        TestParserInputDefault(
            L"const struct { float c;};",
            "struct anontype_0 {\n  float var_1_0;\n};\n"
            );
        TestParserInputDefault(
            L"const struct typename { float c; };"
            L"typename foo;"
            L"uniform typename bar;",
            "struct typename_0_0 {\n  float var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\ntypename_0_0 var_0_3=(typename_0_0)0;\n"
            );
        TestParserInputDefault(
            L"uniform struct typename { float c; };"
            L"typename foo;"
            L"uniform typename bar;",
            "struct typename_0_0 {\n  float var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\ntypename_0_0 var_0_3=(typename_0_0)0;\n"
            );
        TestParserInputDefault(
            L"varying struct typename { float c; };"
            L"typename foo;"
            L"uniform typename bar;",
            "struct typename_0_0 {\n  float var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\ntypename_0_0 var_0_3=(typename_0_0)0;\n"
            );
        TestParserInputDefault(
            L"attribute struct typename { float c; };"
            L"typename foo;"
            L"uniform typename bar;",
            "struct typename_0_0 {\n  float var_1_1;\n};\nstatic typename_0_0 var_0_2=(typename_0_0)0;\ntypename_0_0 var_0_3=(typename_0_0)0;\n"
            );
        TestParserInputDefault(
            L"const struct typename { float c; };"
            L"typename foo;"
            L"uniform typename bar;"
            L"void fn() { typename baz = typename(1.0); baz == bar; }",
            "struct typename_0_0 {\n  float var_1_1;\n};\ntypename_0_0 ctor_typename_0_0(float var_1_1) {\ntypename_0_0 var_localstruct;\nvar_localstruct.var_1_1=var_1_1;\nreturn var_localstruct;\n}\nbool eq_typename_0_0(typename_0_0 val1, typename_0_0 val2) {\nreturn (val1.var_1_1==val2.var_1_1);\n}\nstatic typename_0_0 var_0_2=(typename_0_0)0;\ntypename_0_0 var_0_3=(typename_0_0)0;\nvoid fn_0_4()\n{\ntypename_0_0 var_2_5=ctor_typename_0_0(1.000000e+000);\neq_typename_0_0(var_2_5,var_0_3);\n}\n"
            );

        // However type_specifier IDENT should still be a parse error for now
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;};" L"const S;", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;};" L"uniform S;", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;};" L"attribute S;", E_GLSLERROR_SYNTAXERROR);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S {int a;};" L"varying S;", E_GLSLERROR_SYNTAXERROR);
    }

    void BasicGLSLTests::DerivativeTests()
    {
        // Basic usage
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=ddx(var_1_1);\n}\n"
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { float x; float y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat var_1_1=0.0;\nfloat var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec2 x; vec2 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat2 var_1_1=0.0;\nfloat2 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec3 x; vec3 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec4 x; vec4 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat4 var_1_1=0.0;\nfloat4 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        // Macro should be enabled whenever extension is available
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,
            L"#ifdef GL_OES_standard_derivatives\nint x;\n#endif\nint y;",                                      
            "static int var_0_0=0;\nstatic int var_0_1=0;\n"
            );

        // ... and disabled when not
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs,
            L"#ifdef GL_OES_standard_derivatives\nint x;\n#endif\nint y;",
            "static int var_0_0=0;\n"
            );

        // Should not work without extension used
        TestParserInputNegativeError(
            GLSLShaderType::Fragment, 
            GLSLTranslateOptions::EnableStandardDerivatives, 
            L" precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }", 
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        // Should not work with vertex shader
        TestParserInputNegativeError(
            GLSLShaderType::Vertex, 
            GLSLTranslateOptions::EnableStandardDerivatives, 
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }", 
            E_GLSLERROR_UNKNOWNEXTENSION
            );

        TestParserInput(
            GLSLShaderType::Vertex,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,   
            L"#ifdef GL_OES_standard_derivatives\nint x;\n#endif\nint y;",                                        
            "static int var_0_0=0;\n"
            );
    }

    void BasicGLSLTests::FragDepthTests()
    {
        // Basic usage
        TestParserInput(
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableFragDepth,
            L"#extension GL_EXT_frag_depth : enable\nvoid foo() { gl_FragDepthEXT = 0.5; }",
            "void fn_0_0()\n{\npsOutput.fragDepth=5.000000e-001;\n}\n"
            );

        // Should not work without extension used
        TestParserInputNegativeError(
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::EnableFragDepth,
            L"void foo() { gl_FragDepthEXT = 0.5; }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        // Should not work with vertex shader
        TestParserInputNegativeError(
            GLSLShaderType::Vertex,
            GLSLTranslateOptions::EnableFragDepth,
            L"#extension GL_EXT_frag_depth : enable\nvoid foo() { gl_FragDepthEXT = 0.5; }",
            E_GLSLERROR_UNKNOWNEXTENSION
            );
    }

    void BasicGLSLTests::TestExtensions()
    {
        // Require should error out if extension is not available, and compile if it is
        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension GL_OES_standard_derivatives : require\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : require\n precision mediump float; void foo() { vec3 x; vec3 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        // Enable should not error out if extension is not available
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs,  
            L"#extension GL_OES_standard_derivatives : enable\nvoid foo() {}",
            "void fn_0_0()\n{\n}\n"
            );

        // But enable should still give errors if the extension is not available and you use it
        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        // And of course, enable should enable you to use the extension
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : enable\n precision mediump float; void foo() { vec3 x; vec3 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        // Warn behaves the same as enable for us right now since we have no warnings - test with all as well
        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs,  
            L"#extension GL_OES_standard_derivatives : warn\nvoid foo() {}",
            "void fn_0_0()\n{\n}\n"
            );

        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension GL_OES_standard_derivatives : warn\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension GL_OES_standard_derivatives : warn\n precision mediump float; void foo() { vec3 x; vec3 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs,  
            L"#extension all : warn\nvoid foo() {}",
            "void fn_0_0()\n{\n}\n"
            );

        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension all : warn\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        TestParserInput(
            GLSLShaderType::Fragment,   
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,  
            L"#extension all : warn\n precision mediump float; void foo() { vec3 x; vec3 y = fwidth(x); }",       
            "void fn_0_0()\n{\nfloat3 var_1_1=0.0;\nfloat3 var_1_2=GLSLfwidth(var_1_1);\n}\n"
            );

        // Disable extension should make extension functionality disabled, whether it is supported or not,
        // and with all or the extension.
        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension GL_OES_standard_derivatives : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,
            L"#extension GL_OES_standard_derivatives : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"#extension all : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        TestParserInputNegativeError(            
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,
            L"#extension all : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        // All should override previous changes to an individual extension
        TestParserInputNegativeError(
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,   
            L"#extension GL_OES_standard_derivatives : enable\n#extension all : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );

        // Doing all then doing something else should work as expected too
        TestParserInputNegativeError(
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs | GLSLTranslateOptions::EnableStandardDerivatives,   
            L"#extension all : warn\n#extension GL_OES_standard_derivatives : disable\n precision mediump float; void foo() { vec3 x; vec3 y = dFdx(x); }",
            E_GLSLERROR_UNDECLAREDIDENTIFIER
            );
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   PrecisionTests
    //
    //  Synopsis:   Test various precision declarations and their side-effects.
    //              Many of these tests are negative tests. Positive tests will
    //              be done elsewhere as the only spot we readback the identifier's
    //              precision is in shared uniform type checking.
    //  
    //+----------------------------------------------------------------------------
    void BasicGLSLTests::PrecisionTests()
    {
        // Test allowable precision qualifiers
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision lowp float; precision mediump int; precision highp float; precision mediump sampler2D; precision highp samplerCube;", "");

        // Ensure fragment float precision is propagated through various scopes
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"precision lowp float; void foo() { { for (int i = 0; i < 3; i++) { struct S { float x; } s; if (true) { vec2 y; } else { mat4 m; } } } }", "struct typename_4_2 {\n  float var_1_3;\n};\nvoid fn_0_0()\n{\n{\n[unroll] for (int var_4_1=0;\nvar_4_1<3;(var_4_1++))\n{\ntypename_4_2 var_4_4=(typename_4_2)0;\nif (true){\nfloat2 var_2_5=0.0;\n}\nelse\n{\nfloat4x4 var_3_6=0.0;\n}\n}\n}\n}\n");

        // Ensure fragment float precision is not propagated outside of its scope
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } vec2 x; }", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { if (true) precision lowp float; vec4 x; }", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { for (int i = 0; i < 2; i++) precision lowp float; mat3 x; }", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { precision lowp float; } void bar(float a);", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { precision lowp float; } void bar(float);", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { precision lowp float; } float bar();", E_GLSLERROR_NOPRECISIONSPECIFIED);

        // Verify precision required for function parameters and return types
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } } void bar(mat4 a) {}", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } } void bar(mat4) {}", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } } mat4 bar() { return mat4(0); }", E_GLSLERROR_NOPRECISIONSPECIFIED);

        // Verify that constructors don't require precision
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { { precision lowp float; } gl_FragColor = vec4(float(1), vec2(0.5), vec3(0)); }",             "void fn_0_0()\n{\n{\n}\npsOutput.fragColor[0]=float4(float(1),float2((5.000000e-001).xx),(float3((0).xxx)).x);\n}\n");

        // Verify precision required for struct members
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } struct S {vec3 x;}; }", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } struct S {vec3 x[2];}; }", E_GLSLERROR_NOPRECISIONSPECIFIED);
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs, L"void foo() { struct S { highp vec3 x;}; }",                                                                 "struct typename_2_1 {\n  float3 var_1_2;\n};\nvoid fn_0_0()\n{\n}\n");

        // Ensure arrays are not exempt
        TestParserInputNegativeError(GLSLShaderType::Fragment, 0, L"void foo() { { precision lowp float; } mat2 x[2]; }", E_GLSLERROR_NOPRECISIONSPECIFIED);

        // and that they pick up a declared precision
        TestParserInput(GLSLShaderType::Fragment,   GLSLTranslateOptions::DisableWriteInputs,   L"void foo() { precision lowp float; mat2 x[2]; }",                                                         "void fn_0_0()\n{\nfloat2x2 var_1_1[2]=(float2x2[2])0;\n}\n");

        // Verify that basic types that can't define precision are flagged as errors
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp void;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp bool;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp bvec2;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp bvec3;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp bvec4;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp ivec2;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp ivec3;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp ivec4;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp mat2;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp mat3;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp mat4;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);

        // Precision declaration with array type should be a syntax error (type_specifier_no_prec does not allow for array type, it comes from a declarator entry)
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp float[2];", E_GLSLERROR_SYNTAXERROR);

        // Verify user defined types are not allowed as well
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp struct { int a; };", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"precision lowp struct S { int a; };", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int a; }; precision lowp S;", E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL);

        // Precision qualifiers are only allowed for type specifiers of certain types
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"mediump struct S { int a; } s;", E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S { int a; }; lowp S s[2];", E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"struct S1 { int a; }; struct S2 { highp S1 s1[2]; };", E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"lowp bool b[2];", E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"lowp bvec2 b[2];", E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"lowp bvec3 b[2];", E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE);
        TestParserInputNegativeError(GLSLShaderType::Vertex, 0, L"lowp bvec4 b[2];", E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE);

        // Precision qualifiers are allowed for all other basic types
        TestParserInput(
            GLSLShaderType::Fragment,
            GLSLTranslateOptions::DisableWriteInputs,
            L"lowp float a1; "
            L"lowp vec2 a2; "
            L"lowp vec3 a3; "
            L"lowp vec4 a4; "
            L"lowp int a5; "
            L"lowp ivec2 a6; "
            L"lowp ivec3 a7; "
            L"lowp ivec4 a8; "
            L"lowp mat2 a9; "
            L"lowp mat3 a10; "
            L"lowp mat4 a11; "
            L"uniform lowp sampler2D a12; "
            L"uniform lowp samplerCube a13; ",
            "SamplerState sampler_0:register(s[0]);\nTexture2D<float4> texture_0:register(t[0]);\nSamplerState sampler_1:register(s[1]);\nTextureCube<float4> texture_1:register(t[1]);\nstatic float var_0_0=0.0;\nstatic float2 var_0_1=0.0;\nstatic float3 var_0_2=0.0;\nstatic float4 var_0_3=0.0;\nstatic int var_0_4=0;\nstatic int2 var_0_5=0;\nstatic int3 var_0_6=0;\nstatic int4 var_0_7=0;\nstatic float2x2 var_0_8=0.0;\nstatic float3x3 var_0_9=0.0;\nstatic float4x4 var_0_10=0.0;\n"
            );
    }

    //+----------------------------------------------------------------------------
    //
    //  Function:   GlobalDeclarationTests
    //
    //  Synopsis:   Tests our tranformation of global declarations. Because non-const
    //              globals that are initialized can depend on shader inputs, they
    //              must be initialized after those inputs are assigned which
    //              happens as the first statement in main()
    //
    //+----------------------------------------------------------------------------
    void BasicGLSLTests::GlobalDeclarationTests()
    {
        // Basic test to ensure we generate the init function declaration and definition in the
        // correct spot and that non-const global initializers are not moved
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void foo() {}"
            L"vec2 v3 = v2;"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_3()\n{\n}\nstatic float2 var_0_4=0.0;\nstatic const bool var_0_5=true;\nvoid fn_0_12();\nvoid fn_0_6()\n{\nfn_0_12();\n}\nstatic float2 var_0_7=0.0;\nvoid fn_0_12()\n{\nvar_0_1=var_0_0.xx;\nvar_0_4=var_0_1;\nvar_0_7=var_0_0.yy;\n}\n"
            );

        // Ensure the transformation work with multiple declarator entries even when not all are initialized
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void foo() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a, v3b = v2 = vec2(0.0), v3c;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_3()\n{\n}\nstatic const bool var_0_4=true;\nvoid fn_0_14();\nvoid fn_0_5()\n{\nfn_0_14();\n}\nstatic float2 var_0_6=0.0, var_0_7=0.0, var_0_8=0.0;\nstatic float2 var_0_9=0.0;\nvoid fn_0_14()\n{\nvar_0_1=var_0_0.xx;\nvar_0_7=var_0_1=float2((0.000000e+000).xx);\nvar_0_9=var_0_0.yy;\n}\n"
            );
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void foo() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a = v2, v3b = v2, v3c;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_3()\n{\n}\nstatic const bool var_0_4=true;\nvoid fn_0_14();\nvoid fn_0_5()\n{\nfn_0_14();\n}\nstatic float2 var_0_6=0.0, var_0_7=0.0, var_0_8=0.0;\nstatic float2 var_0_9=0.0;\nvoid fn_0_14()\n{\nvar_0_1=var_0_0.xx;\nvar_0_6=var_0_1;\nvar_0_7=var_0_1;\nvar_0_9=var_0_0.yy;\n}\n"
            );
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void foo() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a = v2, v3b, v3c = v1.zz;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_3()\n{\n}\nstatic const bool var_0_4=true;\nvoid fn_0_15();\nvoid fn_0_5()\n{\nfn_0_15();\n}\nstatic float2 var_0_6=0.0, var_0_7=0.0, var_0_8=0.0;\nstatic float2 var_0_10=0.0;\nvoid fn_0_15()\n{\nvar_0_1=var_0_0.xx;\nvar_0_6=var_0_1;\nvar_0_8=var_0_0.zz;\nvar_0_10=var_0_0.yy;\n}\n"
            );

        // Same set of tests as above, but with main() defined to ensure we inject the init function in the right spot in main
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void main() {}"
            L"vec2 v3 = v2;"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_11();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_11();\n}\nstatic float2 var_0_3=0.0;\nstatic const bool var_0_4=true;\nvoid fn_0_5()\n{\n}\nstatic float2 var_0_6=0.0;\nvoid fn_0_11()\n{\nvar_0_1=var_0_0.xx;\nvar_0_3=var_0_1;\nvar_0_6=var_0_0.yy;\n}\n"
            );
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void main() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a, v3b = v2, v3c;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_13();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_13();\n}\nstatic const bool var_0_3=true;\nvoid fn_0_4()\n{\n}\nstatic float2 var_0_5=0.0, var_0_6=0.0, var_0_7=0.0;\nstatic float2 var_0_8=0.0;\nvoid fn_0_13()\n{\nvar_0_1=var_0_0.xx;\nvar_0_6=var_0_1;\nvar_0_8=var_0_0.yy;\n}\n"
            );
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void main() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a = v2, v3b = v2, v3c;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_13();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_13();\n}\nstatic const bool var_0_3=true;\nvoid fn_0_4()\n{\n}\nstatic float2 var_0_5=0.0, var_0_6=0.0, var_0_7=0.0;\nstatic float2 var_0_8=0.0;\nvoid fn_0_13()\n{\nvar_0_1=var_0_0.xx;\nvar_0_5=var_0_1;\nvar_0_6=var_0_1;\nvar_0_8=var_0_0.yy;\n}\n"
            );
        TestParserInputDefault(
            L"vec4 v1 = vec4(1.0);"
            L"vec2 v2 = v1.xx;"
            L"void main() {}"
            L"const bool f = true;"
            L"void bar() {}"
            L"vec2 v3a = v2, v3b, v3c = v1.zz;"
            L"vec2 v4 = v1.yy;",
            "static float4 var_0_0=float4((1.000000e+000).xxxx);\nstatic float2 var_0_1=0.0;\nvoid fn_0_14();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_14();\n}\nstatic const bool var_0_3=true;\nvoid fn_0_4()\n{\n}\nstatic float2 var_0_5=0.0, var_0_6=0.0, var_0_7=0.0;\nstatic float2 var_0_9=0.0;\nvoid fn_0_14()\n{\nvar_0_1=var_0_0.xx;\nvar_0_5=var_0_1;\nvar_0_7=var_0_0.zz;\nvar_0_9=var_0_0.yy;\n}\n"
            );

        // Test that we init shader inputs after they've been assigned (both varying and attribute)
        TestParserInput(
            GLSLShaderType::Vertex,
            0,
            L"attribute vec4 att;"
            L"vec2 glob = vec2(att.x, att.z);"
            L"void main() {}",
            "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float4 attr_0:attr_0_sem;\n};\nstatic float2 var_0_1=0.0;\nvoid fn_0_7();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_7();\n}\nvoid fn_0_7()\n{\nvar_0_1=float2(vsInput.attr_0.x,vsInput.attr_0.z);\n}\n"
            );
        TestParserInput(
            GLSLShaderType::Fragment,
            0,
            L"precision mediump float;"
            L"varying vec4 vary;"
            L"vec2 glob = vec2(vary.x, vary.z);"
            L"void main() { gl_FragColor = vec4(glob.x, glob.y, 0.0, 1.0); }",
            "struct PSInput\n{\n  float4 vary_0:vary_0_sem;\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nstatic float2 var_0_1=0.0;\nvoid fn_0_8();\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\nfn_0_8();\npsOutput.fragColor[0]=float4(var_0_1.x,var_0_1.y,0.000000e+000,1.000000e+000);\n}\nvoid fn_0_8()\n{\nvar_0_1=float2(psInput.vary_0.x,psInput.vary_0.z);\n}\n"
            );


        // Test that we can't assign the uninitialized values by declaring and using after main()
        TestParserInput(
            GLSLShaderType::Vertex,
            0,
            L"attribute vec4 att;"
            L"vec2 fn();"
            L"void main() {"
            L"  vec2 localVec = fn();"
            L"}"
            L"vec2 glob = vec2(att.x, att.z);"
            L"vec2 fn() { return glob; }",
            "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float4 attr_0:attr_0_sem;\n};\nfloat2 fn_0_1();\nvoid fn_0_9();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_9();\nfloat2 var_1_2=fn_0_1();\n}\nstatic float2 var_0_3=0.0;\nfloat2 fn_0_1()\n{\nreturn var_0_3;\n}\nvoid fn_0_9()\n{\nvar_0_3=float2(vsInput.attr_0.x,vsInput.attr_0.z);\n}\n"
            );
        TestParserInput(
            GLSLShaderType::Fragment,
            0,
            L"precision mediump float;"
            L"varying vec4 vary;"
            L"vec2 fn();"
            L"void main() {"
            L"  vec2 localVec = fn();"
            L"  gl_FragColor = localVec.xxyy;"
            L"}"
            L"vec2 glob = vec2(vary.x, vary.z);"
            L"vec2 fn() { return glob; }",
            "struct PSInput\n{\n  float4 vary_0:vary_0_sem;\n  float4 position : SV_Position;\n};\nstruct PSOutput\n{\n  float4 fragColor[1] : SV_Target;\n};\nfloat2 fn_0_1();\nvoid fn_0_10();\nvoid main(in PSInput psInputArg, out PSOutput psOutputOut)\n{\nfn_0_10();\nfloat2 var_1_2=fn_0_1();\npsOutput.fragColor[0]=var_1_2.xxyy;\n}\nstatic float2 var_0_4=0.0;\nfloat2 fn_0_1()\n{\nreturn var_0_4;\n}\nvoid fn_0_10()\n{\nvar_0_4=float2(psInput.vary_0.x,psInput.vary_0.z);\n}\n"
            );


        // Verify that we don't insert the generated init function and call it if nothing is moved
        TestParserInputDefault(
            L"const vec2 v2 = vec2(1.0);"
            L"void main() {"
            L"}",
            "static const float2 var_0_0=float2((1.000000e+000).xx);\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\n}\n"
            );

        // Verify that the global init function is placed at the end of the shader where all variables are in scope
        TestParserInput(
            GLSLShaderType::Vertex,
            0,
            L"attribute vec2 att;"
            L"void main() {"
            L"}"
            L"const vec2 constV2 = vec2(1.0);"
            L"vec2 v2 = constV2 + att;",
            "struct PSInput\n{\n  float4 position : SV_Position;\n};\nstruct VSInput\n{\n  float2 attr_0:attr_0_sem;\n};\nvoid fn_0_6();\nvoid main(in VSInput vsInputArg, out PSInput psInputOut)\n{\nfn_0_6();\n}\nstatic const float2 var_0_1=float2((1.000000e+000).xx);\nstatic float2 var_0_2=0.0;\nvoid fn_0_6()\n{\nvar_0_2=var_0_1+vsInput.attr_0;\n}\n"
            );
    }

    void BasicGLSLTests::TestParserInput(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, const char* pszExpected)
    {
        CSmartBstr bstrText;
        bstrText.Set(pszInput);

        // These tests never output boiler plate code since it would make the baselines huge
        uOptions |= GLSLTranslateOptions::DisableBoilerPlate;

        TSmartPointer<CGLSLConvertedShader> spShader;
        VERIFY_SUCCEEDED(::GLSLTranslate(
            bstrText,
            shaderType,
            uOptions,
            WebGLFeatureLevel::Level_10,
            &spShader
            ));

        CMutableString<char> spConverted;
        if (FAILED(spShader->GetConvertedCodeWithParsedStructInfo(/*out*/spConverted)))
        {
            // Grab the log, it might be useful
            CSmartBstr bstrLog;
            VERIFY_SUCCEEDED(spShader->GetLog(&bstrLog));

            Log::Comment(bstrLog);
            Log::Error(L"Unexpected shader errors, test failed");
        }
        else
        {
            int firstDiff;
            VERIFY_IS_TRUE(CompareOutputs(pszExpected, spConverted, &firstDiff));
            VERIFY_ARE_EQUAL(firstDiff, -1);
            if (firstDiff != -1)
            {
                WCHAR szConverted[2048];
                szConverted[0] = L'\0';
                int length = ::MultiByteToWideChar(CP_UTF8, 0, spConverted, spConverted.GetLength(), szConverted, ARRAYSIZE(szConverted));
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

    //+----------------------------------------------------------------------------
    //
    //  Function:   TestParserInputDefault
    //
    //  Synopsis:   Tests parsing the input string in the Vertex shader without
    //              outputting write inputs.
    //
    //-----------------------------------------------------------------------------
    void BasicGLSLTests::TestParserInputDefault(__in const WCHAR* pszInput, __in const char* pszExpected)
    {
        TestParserInput(GLSLShaderType::Vertex, GLSLTranslateOptions::DisableWriteInputs, pszInput, pszExpected);
    }

    void BasicGLSLTests::TestParserInputNegativeError(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, HRESULT hrCode)
    {
        TestParserInputNegativeErrorLine(shaderType, uOptions, pszInput, hrCode, -1);
    }

    void BasicGLSLTests::TestParserInputNegativeErrorLine(GLSLShaderType::Enum shaderType, UINT uOptions, const WCHAR* pszInput, HRESULT hrCode, int line)
    {
        CSmartBstr bstrText;
        bstrText.Set(pszInput);

        TSmartPointer<CGLSLConvertedShader> spShader;
        VERIFY_SUCCEEDED(::GLSLTranslate(
            bstrText,
            shaderType,
            uOptions,
            WebGLFeatureLevel::Level_10,
            &spShader
            ));

        CMutableString<char> spConverted;
        VERIFY_FAILED(spShader->GetConvertedCodeWithParsedStructInfo(/*out*/spConverted));

        // The first error should be the one provided
        VERIFY_IS_TRUE(spShader->GetErrorCount() > 0);

        // If we have the error to check, then check it
        if (spShader->GetErrorCount() > 0)
        {
            VERIFY_ARE_EQUAL(spShader->UseError(0)->GetCode(), hrCode);

            if (line != -1)
            {
                VERIFY_ARE_EQUAL(spShader->UseError(0)->GetLine(), line);
            }
        }
    }
}
