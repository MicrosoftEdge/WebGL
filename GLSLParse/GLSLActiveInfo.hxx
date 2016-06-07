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

namespace GLConstants
{
    enum Type;
};

//+-----------------------------------------------------------------------------
//
//  Class:      CGLSLActiveInfo
//
//  Synopsis:   Contains a name, GLConstants::Type and array size. It
//              is an internal format of a WebGLActiveInfo, and is generated
//              as part of GLSL type reflection. Copy ctor and op= both do
//              not copy the string, which is required to be set directly 
//              (since it always requires modification after copying,
//              unless it stays as null/empty string).
//
//------------------------------------------------------------------------------
template <typename T>
class CGLSLActiveInfo
{
public:
    CGLSLActiveInfo<T>& operator=(const CGLSLActiveInfo<T>& other)
    {
        _spszName.Set(nullptr);
        _glType = other._glType;
        _uArraySize = other._uArraySize;
    }

    CGLSLActiveInfo(const CGLSLActiveInfo<T>& other)
    {
        _glType = other._glType;
        _uArraySize = other._uArraySize;
    }

    CGLSLActiveInfo() {}

    CMutableString<T>& UseName() { return _spszName; }
    const CMutableString<T>& UseName() const { return _spszName; }
    _Null_terminated_ const T* GetNameString() const { return static_cast<const T*>(_spszName); }

    void SetGLType(GLConstants::Type glType) { _glType = glType; }
    GLConstants::Type GetType() const { return _glType; }

    void SetArraySize(UINT uArraySize) { _uArraySize = uArraySize; }
    UINT GetArraySize() const { return _uArraySize; }

    HRESULT InitWithVariableName(__in PCSTR pszVariableName, const CGLSLActiveInfo<char>& other);

private:
    CMutableString<T> _spszName;                    // Name representation of an active variable
    GLConstants::Type _glType;                      // Type of active variable
    UINT _uArraySize;                               // Array size of active variable
};
