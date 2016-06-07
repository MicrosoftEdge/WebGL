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
#include "PreComp.hxx"
#include "GLSLError.hxx"
#include "MemoryStream.hxx"
#include "GLSL.tab.h"

//+----------------------------------------------------------------------------
//
//  Mapping of errors to error strings. Since these go into the shader log,
//  they are not localized like other WebGL errors.
//
//-----------------------------------------------------------------------------
const GLSLErrorInfo CGLSLError::s_errorInfo[] = {
    { E_GLSLERROR_KNOWNERROR,                       0,  "" },
    { E_GLSLERROR_INTERNALERROR,                    0, "Internal compiler error" },
    { E_GLSLERROR_SHADERTOOLONG,                    1, "Compiler memory error - shader exceeds %s bytes" },
    { E_GLSLERROR_SYNTAXERROR,                      1, "%s" },
    { E_GLSLERROR_UNDECLAREDIDENTIFIER,             1, "Undeclared identifier '%s'" },
    { E_GLSLERROR_INVALIDARGUMENTS,                 1, "Invalid arguments passed to function '%s'" },
    { E_GLSLERROR_INVALIDINDEXTARGET,               0, "Postfix expression cannot be indexed" },
    { E_GLSLERROR_INDEXOUTOFRANGE,                  0, "Index out of range" },
    { E_GLSLERROR_INVALIDINDEX,                     0, "Incompatible index expression. For non-uniforms, the index must be an expression formed of the loop_index and integer constants. For uniforms, the index must be an integer constant." },
    { E_GLSLERROR_NONCONSTINDEX,                    0, "Index must be a constant" },
    { E_GLSLERROR_INVALIDSAMPLER,                   1, "Argument '%s' is not a sampler" },
    { E_GLSLERROR_INVALIDMACRONAME,                 0, "Invalid macro name - cannot start with 'GL_' or contain '__'" },
    { E_GLSLERROR_INCOMPATIBLETYPES,                0, "Incompatible types in expression" },
    { E_GLSLERROR_INVALIDIFEXPRESSION,              0, "Expression in if statement does not evaluate to a boolean" },
    { E_GLSLERROR_DIVIDEORMODBYZERO,                0, "Divide or mod by zero in constant expression" },
    { E_GLSLERROR_PREINVALIDMACROPARAMCOUNT,        0, "Invalid parameter count for macro" },
    { E_GLSLERROR_MAXUNIFORMEXCEEDED,               0, "Maximum uniform vector count exceeded" },
    { E_GLSLERROR_MAXATTRIBUTEEXCEEDED,             0, "Maximum attribute vector count exceeded" },
    { E_GLSLERROR_MAXVARYINGEXCEEDED,               0, "Maximum varying vector count exceeded" },
    { E_GLSLERROR_SHADERCOMPLEXITY,                 0, "Maximum shader complexity exceeded" },
    { E_GLSLERROR_IDENTIFIERALREADYDECLARED,        0, "Identifier already declared" },
    { E_GLSLERROR_INVALIDCHARACTER,                 0, "Invalid character used outside of comment" },
    { E_GLSLERROR_INVALIDFORLOOPINIT,               0, "Invalid initializer in for loop, needs to be a single variable of type float or int and initialized to a constant" },
    { E_GLSLERROR_INVALIDFORLOOPCOND,               0, "Invalid condition in for loop, needs to be in form loop_index { > | >= | < | <= | == | != } constant" },
    { E_GLSLERROR_INVALIDFORLOOPITER,               0, "Invalid iteration in for loop, needs to be in form { --loop_index | ++loop_index | loop_index++ | loop_index-- | loop_index+=constant | loop_index-=constant }" },
    { E_GLSLERROR_INVALIDFORLOOPINDEXWRITE,         0, "Invalid modification to loop index inside loop body" },
    { E_GLSLERROR_INVALIDIDENTIFIERNAME,            0, "Invalid identifier name - cannot start with 'gl_', 'webgl_', '_webgl_' or contain '__'" },
    { E_GLSLERROR_PREMAXTOKEN,                      0, "Token exceeds maximum length" },
    { E_GLSLERROR_INVALIDARRAYQUALIFIER,            0, "Invalid qualifier on array - cannot make arrays of attribute or const variables" },
    { E_GLSLERROR_INCOMPATIBLERETURN,               0, "Incompatible type used for return expression" },
    { E_GLSLERROR_INVALIDSAMPLERQUALIFIER,          0, "Invalid qualifier on sampler variable declaration - must be uniform" },
    { E_GLSLERROR_INVALIDMATCTORTYPE,               0, "Invalid type passed to matrix constructor - arguments must be a matrix, or a scalar / vector of float / int / bool" },
    { E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE,   0, "Invalid type passed to componentwise vector or matrix constructor - arguments must be a scalar / vector of float / int / bool" },
    { E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT,  0, "Invalid argument count in componentwise vector or matrix constructor - total components passed must equal vector or matrix size" },
    { E_GLSLERROR_INVALIDLVALUE,                    0, "Invalid expression on left of assignment expression" },
    { E_GLSLERROR_TOOMANYSWIZZLECOMPONENTS,         0, "Invalid swizzle in field selection - swizzle component count must be equal or less than max vector size (4)" },
    { E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT,       0, "Invalid swizzle in field selection - swizzle components must be all from same set (xyzw, rgba or stpq)" },
    { E_GLSLERROR_INVALIDSWIZZLECOMPONENT,          0, "Invalid swizzle component in field selection - must be from a valid GLSL set (xyzw, rgba or stpq)" },
    { E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE,       0, "Swizzle component out of range - must select a component that exists in the vector" },
    { E_GLSLERROR_INVALIDFRONTFACINGUSAGE,          0, "This hardware is unable to support gl_FrontFacing" },
    { E_GLSLERROR_CONSTREQUIRESINITIALIZATION,      0, "Const variable requires initialization" },
    { E_GLSLERROR_INVALIDINITQUALIFIER,             0, "Variables declared with uniform, attribute, or varying qualifier cannot be initialized" },
    { E_GLSLERROR_INVALIDINPUTDATATYPE,             0, "Attribute and varying variables cannot have bool, int, or struct type" },
    { E_GLSLERROR_INVALIDCONSTRUCTORARG,            0, "Invalid argument passed to constructor - argument must be a basic GLSL type" },
    { E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER,        0, "Invalid type qualifier for function parameter - only const on in parameters is allowed" },
    { E_GLSLERROR_ARRAYDECLARATORNOTCONST,          0, "Array declarator requires a constant expression" },
    { E_GLSLERROR_ARRAYSIZELEQZERO,                 0, "Array was declared with size less than or equal to zero" },
    { E_GLSLERROR_INVALIDTYPEQUALIFIERFORSTRUCT,    0, "Type qualifiers 'uniform' and 'attribute' are invalid for structs" },
    { E_GLSLERROR_INVALIDSTRUCTFIELDNAME,           0, "Invalid field name for struct type" },
    { E_GLSLERROR_INVALIDTYPEFORFIELDSELECTION,     0, "Invalid type for left hand side of field selection" },
    { E_GLSLERROR_SAMPLERNOTALLOWEDFORSTRUCTS,      0, "Samplers are not allowed in structs" },
    { E_GLSLERROR_MACRODEFINITIONNOTEQUAL,          0, "Macros must be redefined the same as original definition" },
    { E_GLSLERROR_LOOPINDEXOUTPARAM,                0, "Invalid loop index expression passed as out / inout parameter" },
    { E_GLSLERROR_TYPENOTCONSTRUCTABLE,             0, "Type cannot be used as a constructor" },
    { E_GLSLERROR_UNDECLAREDTYPENAME,               1, "Undeclared type '%s'" },
    { E_GLSLERROR_NESTEDSTRUCTSINVALID,             0, "Embedded struct declarations are not allowed" },
    { E_GLSLERROR_UNDEFINEDFUNCTION,                1, "Function '%s' is declared and used but not defined" },
    { E_GLSLERROR_FUNCTIONREDEFINITION,             0, "Function redefinition not allowed" },
    { E_GLSLERROR_FUNCTIONREDECLARATION,            0, "Function redeclaration not allowed" },
    { E_GLSLERROR_INVALIDVECCTOR,                   0, "Invalid single argument to vector constructor - must be a scalar type, or another vector, or a 2x2 matrix" },
    { E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR,        0, "Struct constructor arguments' types do not match the struct's field types" },
    { E_GLSLERROR_CUSTOM,                           1, "Error: %s" },
    { E_GLSLERROR_INVALIDCONTINUELOCATION,          0, "Invalid location for continue statment - must be inside of a loop" },
    { E_GLSLERROR_CALLMAIN,                         0, "Cannot call main" },
    { E_GLSLERROR_INVALIDNONGLOBALQUALIFIER,        0, "Invalid qualifier on non-global variable - non-global variables can be const but cannot be varying, attribute or invariant" },
    { E_GLSLERROR_REDEFINEMAIN,                     0, "Cannot redefine main or define main with incorrect signature" },
    { E_GLSLERROR_RESERVEDOPERATOR,                 0, "Cannot use reserved operators such as '~', '%=', '>>=', '<<=', '&=', '|=', or '^='" },
    { E_GLSLERROR_TERNARYCONDITIONALNOTBOOL,        0, "Ternary conditional operator must have boolean expression for test condition" },
    { E_GLSLERROR_TERNARYCONDITIONALEXPRNE,         0, "Ternary conditional operator must have two expressions of equal types after test condition" },
    { E_GLSLERROR_INVALIDBREAKLOCATION,             0, "Invalid location for break statement - break statements must be inside a loop" },
    { E_GLSLERROR_INVALIDDISCARDLOCATION,           0, "Invalid location for discard statement - discard statements must be inside a fragment shader" },
    { E_GLSLERROR_NONCONSTINITIALIZER,              0, "Initializer for const variable must initialize to a constant value" },
    { E_GLSLERROR_FUNCOVERLOADONRETURNTYPE,         0, "Functions cannot be overloaded on return type" },
    { E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED,   0, "Known functions cannot be re-declared or re-defined" }, 
    { E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS,     0, "Function header definition parameter qualifiers must match declaration parameter qualifiers" },
    { E_GLSLERROR_ARRAYSIZENOTINT,                  0, "Array size must be an integer constant expression" },
    { E_GLSLERROR_CANNOTCALCARRAYSIZE,              0, "Array size expression too complex" },
    { E_GLSLERROR_WRONGVERSION,                     0, "#version directive must specify 100 for version" },
    { E_GLSLERROR_UNEXPECTEDVERSION,                0, "#version directive can only be preceded by whitespace or comments" },
    { E_GLSLERROR_UNARYOPERATORNOTDEFINEDFORTYPE,   0, "Unary operator not defined for type" },
    { E_GLSLERROR_STRUCTDECLINPARAMETER,            0, "Struct declarations are disallowed in function parameter declarators" },
    { E_GLSLERROR_MAXSTRUCTNESTINGEXCEEDED,         0, "Struct type declaration exceeds maximum nesting level of 4" },
    { E_GLSLERROR_OPUNDEFINEDFORSTRUCTS,            0, "Operator not defined for struct types" },
    { E_GLSLERROR_OPUNDEFINEDFORTYPESWITHARRAYS,    0, "Operator not defined for user-defined types that contain array types" },
    { E_GLSLERROR_UNKNOWNEXTENSION,                 1, "Unknown extension: %s" },
    { E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR,         0, "Invalid behavior specified for extension - behavior must be require, warn, enable or disable for regular extensions, or warn or disable for 'all'" },
    { E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED,     1, "Required extension %s is not supported" },
    { E_GLSLERROR_PREDIRECTIVENOTFIRST,             0, "Preprocessor directives can only be preceded by whitespace on a line" },
    { E_GLSLERROR_LOCALFUNCTIONDECLARATION,         0, "Function declarations cannot be local" },
    { E_GLSLERROR_VARIABLETYPEVOID,                 0, "Variable declared as type void is not allowed" },
    { E_GLSLERROR_MULTIPLEPARAMSWITHVOID,           0, "'void' is an invalid parameter type unless used as '(void)'" },
    { E_GLSLERROR_WRONGOPERANDTYPES,                0, "Operator not defined for type" },
    { E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL,      0, "Invalid type for precision declaration, only int, float, sampler2D, and samplerCube are allowed" },
    { E_GLSLERROR_NOPRECISIONSPECIFIED,             1, "No precision specified for type '%s'" },
    { E_GLSLERROR_MAXFUNCTIONDEPTHEXCEEDED,         0, "Max function nesting level exceeded" },
    { E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS,          0, "Constructor call requires at least one argument" },
    { E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT,      0, "Struct types cannot have a precision qualifier specified" },
    { E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE,       1, "Precision qualifier not allowed to be specified for type '%s'" },
    { E_GLSLERROR_CANNOTUNROLLLOOP,                 0, "Loop is too dynamic for this hardware" },
};

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CGLSLError::Initialize(
    HRESULT hrCode                  // The error code for the error
    )
{
    return Initialize(0, 0, hrCode, nullptr);
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT CGLSLError::Initialize(
    int line,                       // The location that the error occured
    int column,
    HRESULT hrCode,                 // The error code for the error
    __in_z_opt const char* pszData  // The optional data to use making a message for the error
    )
{
    CHK_START;

    _hrCode = hrCode;
    _line = line;
    _column = column;

    int errorIndex = HRESULT_CODE(hrCode) - 1;
    CHKB(errorIndex >= 0 && errorIndex < ARRAYSIZE(s_errorInfo));

    // Table consistency
    Assert(s_errorInfo[errorIndex]._hrCode == hrCode);

    char szFormatted[256];
    switch(s_errorInfo[errorIndex]._dataCount)
    {
    case 0:
        CHK(::StringCchPrintfA(szFormatted, ARRAYSIZE(szFormatted), s_errorInfo[errorIndex]._pszErrorFormat));
        break;

    case 1:
        Assert(pszData != nullptr);
        CHKB(pszData != nullptr);
        CHK(::StringCchPrintfA(szFormatted, ARRAYSIZE(szFormatted), s_errorInfo[errorIndex]._pszErrorFormat, pszData));
        break;

    case 2:
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    CHK(_text.Set(szFormatted));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   WriteLong
//
//  Synopsis:   Write out a string representation of the error to the given
//              logging stream.
//
//-----------------------------------------------------------------------------
HRESULT CGLSLError::WriteLog(__in CMemoryStream* pLogStream)
{
    UINT uLength = ::strlen(_text);
    
    return pLogStream->WriteFormat(128 + uLength, "(%d, %d): %s\n", _line, _column, static_cast<char*>(_text));
}
