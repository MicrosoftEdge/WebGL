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

class CMemoryStream;

const int E_GLSLERROR_KNOWNERROR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                       1);    // Used to communicate that a known error occured
const int E_GLSLERROR_INTERNALERROR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                    2);    // Code for errors not covered by anything else
const int E_GLSLERROR_SHADERTOOLONG = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                    3);    // Shader too long error
const int E_GLSLERROR_SYNTAXERROR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                      4);    // Code used for syntax errors in parsing
const int E_GLSLERROR_UNDECLAREDIDENTIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             5);    // Undeclared identifier error
const int E_GLSLERROR_INVALIDARGUMENTS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 6);    // Invalid arguments error
const int E_GLSLERROR_INVALIDINDEXTARGET = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               7);    // Indexing something that you cannot index
const int E_GLSLERROR_INDEXOUTOFRANGE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                  8);    // Index out of range error
const int E_GLSLERROR_INVALIDINDEX = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                     9);    // Index wrong type error
const int E_GLSLERROR_NONCONSTINDEX = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                    10);   // Index not a constant error
const int E_GLSLERROR_INVALIDSAMPLER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                   11);   // Argument not a sampler error
const int E_GLSLERROR_INVALIDMACRONAME = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 12);   // Invalid macro name
const int E_GLSLERROR_INCOMPATIBLETYPES = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                13);   // Incompatible types in expression error
const int E_GLSLERROR_INVALIDIFEXPRESSION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,              14);   // Invalid if expression error
const int E_GLSLERROR_DIVIDEORMODBYZERO = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                15);   // Divide or mod by zero error
const int E_GLSLERROR_PREINVALIDMACROPARAMCOUNT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        16);   // Invalid preprocessor parameter count on macro
const int E_GLSLERROR_MAXUNIFORMEXCEEDED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               17);   // Uniform variable count exceeded
const int E_GLSLERROR_MAXATTRIBUTEEXCEEDED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             18);   // Attribute variable count exceeded
const int E_GLSLERROR_MAXVARYINGEXCEEDED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               19);   // Varying variable count exceeded
const int E_GLSLERROR_SHADERCOMPLEXITY = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 20);   // Shader complexity error
const int E_GLSLERROR_IDENTIFIERALREADYDECLARED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        21);   // Identifier defined already error
const int E_GLSLERROR_INVALIDCHARACTER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 22);   // Invalid character error
const int E_GLSLERROR_INVALIDFORLOOPINIT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               23);   // Invalid initializer in for loop
const int E_GLSLERROR_INVALIDFORLOOPCOND = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               24);   // Invalid condition in for loop
const int E_GLSLERROR_INVALIDFORLOOPITER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               25);   // Invalid iteration in for loop
const int E_GLSLERROR_INVALIDFORLOOPINDEXWRITE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         26);   // Invalid write to loop variable
const int E_GLSLERROR_INVALIDIDENTIFIERNAME = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            27);   // Invalid identifier name error
const int E_GLSLERROR_PREMAXTOKEN = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                      28);   // Token exceeds max length error
const int E_GLSLERROR_INVALIDARRAYQUALIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            29);   // Invalid qualifier on array
const int E_GLSLERROR_INCOMPATIBLERETURN = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               30);   // Incompatible return type
const int E_GLSLERROR_INVALIDSAMPLERQUALIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          31);   // Invalid qualifier on sampler
const int E_GLSLERROR_INVALIDMATCTORTYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               32);   // Invalid type passed to matrix constructor
const int E_GLSLERROR_INVALIDTENSORCTORCOMPONENTTYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,   33);   // Invalid type passed to component wise matrix constructor
const int E_GLSLERROR_INVALIDTENSORCTORCOMPONENTCOUNT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,  34);   // Invalid count of components passed to matrix constructor
const int E_GLSLERROR_INVALIDLVALUE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                    35);   // Invalid Lvalue in assignment
const int E_GLSLERROR_TOOMANYSWIZZLECOMPONENTS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         36);   // Too many components provided for swizzle
const int E_GLSLERROR_SWIZZLECOMPONENTSDIFFERENT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,       37);   // Swizzle components provided are in different sets
const int E_GLSLERROR_INVALIDSWIZZLECOMPONENT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          38);   // Invalid swizzle component
const int E_GLSLERROR_SWIZZLECOMPONENTOUTOFRANGE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,       39);   // Swizzle component out of range
const int E_GLSLERROR_INVALIDFRONTFACINGUSAGE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          40);   // Using gl_FrontFacing on FL 9 hardware
const int E_GLSLERROR_CONSTREQUIRESINITIALIZATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,      41);   // Variables declared as const require initialization
const int E_GLSLERROR_INVALIDINITQUALIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             42);   // Variables declared as uniform, attribute, or varying qualifier cannot be initialized
const int E_GLSLERROR_INVALIDINPUTDATATYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             43);   // Variables declared as attribute or varying require an underlying component type of float
const int E_GLSLERROR_INVALIDCONSTRUCTORARG = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            44);   // Invalid constructor argument
const int E_GLSLERROR_INVALIDPARAMTYPEQUALIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        45);   // Invalid type qualifier on parameter
const int E_GLSLERROR_ARRAYDECLARATORNOTCONST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          46);   // Declarators declared as array types require a constant expression for the arraysize
const int E_GLSLERROR_ARRAYSIZELEQZERO = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 47);   // Array size constants must be >= 0
const int E_GLSLERROR_INVALIDTYPEQUALIFIERFORSTRUCT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,    48);   // Invalid type qualifier for structs (can't be varying or attribute)
const int E_GLSLERROR_INVALIDSTRUCTFIELDNAME = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,           49);   // Invalid type qualifier for structs (can't be varying or attribute)
const int E_GLSLERROR_INVALIDTYPEFORFIELDSELECTION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,     50);   // Invalid type on LHS of field selection
const int E_GLSLERROR_SAMPLERNOTALLOWEDFORSTRUCTS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,      51);   // Sampler types are not supported as struct fields
const int E_GLSLERROR_MACRODEFINITIONNOTEQUAL = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          52);   // Macros must be redefined the same
const int E_GLSLERROR_LOOPINDEXOUTPARAM = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                53);   // Using loop index as out parameter
const int E_GLSLERROR_TYPENOTCONSTRUCTABLE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             54);   // Type is not allowed to be constructed
const int E_GLSLERROR_UNDECLAREDTYPENAME = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,               55);   // Type name hasn't been declared
const int E_GLSLERROR_NESTEDSTRUCTSINVALID = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             56);   // Structs can't be nested
const int E_GLSLERROR_UNDEFINEDFUNCTION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                57);   // Function not defined
const int E_GLSLERROR_FUNCTIONREDEFINITION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             58);   // Function redfined
const int E_GLSLERROR_FUNCTIONREDECLARATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            59);   // Function redeclared
const int E_GLSLERROR_INVALIDVECCTOR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                   60);   // Invalid argument to construct vector
const int E_GLSLERROR_INVALIDTYPESFORSTRUCTCTOR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        61);   // Structs constructors require exactly matching types
const int E_GLSLERROR_CUSTOM = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                           62);   // Custom error
const int E_GLSLERROR_INVALIDCONTINUELOCATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          63);   // Continue statements must occur inside of a loop
const int E_GLSLERROR_CALLMAIN = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                         64);   // Cannot call main
const int E_GLSLERROR_INVALIDNONGLOBALQUALIFIER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        65);   // Invalid qualifier on non-global
const int E_GLSLERROR_REDEFINEMAIN = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                     66);   // Cannot redefine main
const int E_GLSLERROR_RESERVEDOPERATOR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 67);   // Cannot use reserved operators
const int E_GLSLERROR_TERNARYCONDITIONALNOTBOOL = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,        68);   // Ternary conditional operator must be boolean
const int E_GLSLERROR_TERNARYCONDITIONALEXPRNE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         69);   // Ternary conditional operator must have equal expressions
const int E_GLSLERROR_INVALIDBREAKLOCATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             70);   // Invalid location for break statement
const int E_GLSLERROR_INVALIDDISCARDLOCATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,           71);   // Invalid location for discard statement
const int E_GLSLERROR_NONCONSTINITIALIZER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,              72);   // Non-const initializer
const int E_GLSLERROR_FUNCOVERLOADONRETURNTYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         73);   // Function overloaded only on return type
const int E_GLSLERROR_KNOWNFUNCTIONDECLAREDORDEFINED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,   74);   // A known function was re-declared or re-defined
const int E_GLSLERROR_FUNCTIONMISMATCHEDQUALIFIERS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,     75);   // A function was defined with mismatching qualifiers from it's declaration
const int E_GLSLERROR_ARRAYSIZENOTINT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                  76);   // Array size must be an integer constant expression
const int E_GLSLERROR_CANNOTCALCARRAYSIZE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,              77);   // Array size unable to be calculated
const int E_GLSLERROR_WRONGVERSION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                     78);   // Wrong version used
const int E_GLSLERROR_UNEXPECTEDVERSION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                79);   // Version in wrong place
const int E_GLSLERROR_UNARYOPERATORNOTDEFINEDFORTYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,   80);   // Unary operator not defined for type
const int E_GLSLERROR_STRUCTDECLINPARAMETER = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            81);   // Struct type declaration in function parameter not allowed
const int E_GLSLERROR_MAXSTRUCTNESTINGEXCEEDED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         82);   // Struct types exceeded maximum nesting level
const int E_GLSLERROR_OPUNDEFINEDFORSTRUCTS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,            83);   // Illegal operator for struct types
const int E_GLSLERROR_OPUNDEFINEDFORTYPESWITHARRAYS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,    84);   // Operator not defined for a type that contains an array type.
const int E_GLSLERROR_UNKNOWNEXTENSION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 85);   // Unknown extension used
const int E_GLSLERROR_INVALIDEXTENSIONBEHAVIOR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         86);   // Invalid behavior specified for extension
const int E_GLSLERROR_REQUIREDEXTENSIONUNSUPPORTED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,     87);   // A required extension is not supported
const int E_GLSLERROR_PREDIRECTIVENOTFIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             88);   // Directive not first thing on line
const int E_GLSLERROR_LOCALFUNCTIONDECLARATION = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         89);   // Function declarations cannot be local
const int E_GLSLERROR_VARIABLETYPEVOID = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 90);   // Variables of type void are not allowed
const int E_GLSLERROR_MULTIPLEPARAMSWITHVOID = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,           91);   // 'void' parameters are only valid as single parameter
const int E_GLSLERROR_WRONGOPERANDTYPES = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                92);   // Operands of certain types are not allowed
const int E_GLSLERROR_INVALIDTYPEFORPRECISIONDECL = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,      93);   // Not all types are allowed in a precision declaration
const int E_GLSLERROR_NOPRECISIONSPECIFIED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,             94);   // Precision must be specified for certain types
const int E_GLSLERROR_MAXFUNCTIONDEPTHEXCEEDED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,         95);   // Max function depth nesting level exceeded
const int E_GLSLERROR_CTOR_REQUIRES_ARGUMENTS = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,          96);   // Constructor requires arguments
const int E_GLSLERROR_PRECISIONSPECIFIEDFORSTRUCT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,      97);   // Struct types cannot have precision qualifiers specified
const int E_GLSLERROR_PRECISIONNOTALLOWEDFORTYPE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,       98);   // Certain basic types cannot have precision qualifiers specified
const int E_GLSLERROR_CANNOTUNROLLLOOP = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF,                 99);   // Loops that cannot have termination determined or are too complex for D3D9 hardware

struct GLSLErrorInfo
{
    HRESULT _hrCode;                    // The error code this info is for
    int _dataCount;                     // The number of things to substitute into the format
    const char* _pszErrorFormat;        // The error format string
};

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLError
//
//  Synopsis:   Class to encapsulate an error found while parsing or translating
//              GLSL into HLSL.
//
//------------------------------------------------------------------------------
class CGLSLError : public IUnknown
{
public:
    HRESULT GetCode() const { return _hrCode; }
    int GetLine() const { return _line; }
    int GetColumn() const { return _column; }
    const char* GetText() const { return _text; }

    HRESULT WriteLog(__in CMemoryStream *pLogStream);

protected:
    CGLSLError() {}
    
    HRESULT Initialize(
        int line,                       // The location that the error occured
        int column,
        HRESULT hrCode,                 // The error code for the error
        __in_z_opt const char* pszData  // The optional data to use making a message for the error
        );

    HRESULT Initialize(
        HRESULT hrCode                  // The error code for the error
        );

private:
    HRESULT _hrCode;                    // The error code that we log for this error
    int _line;                          // The line the error occurred on
    int _column;                        // The column the error occurred on
    CSmartSTR _text;                    // The text for the error

    static const GLSLErrorInfo s_errorInfo[];
};
