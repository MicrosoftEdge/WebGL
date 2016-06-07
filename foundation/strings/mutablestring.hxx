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

#include <foundation/collections.hxx>
#include <foundation/runtime.hxx>

// String class built on CModernArray to provide similar resizing algorithms.
template <typename T>
class CMutableString final
{
public:
    CMutableString();

    // Constructor with minimum capacity
    CMutableString(size_t minimumCapacity);

    // Clears contents and ensures the string capacity is at least the given size.
    void SetInitialSize(size_t uLength);

    // Set to the value of the given C language string.
    HRESULT Set(_In_opt_z_ const T* pszSetString);

    // Concatenate the given string to this string.
    HRESULT Append(_In_count_(uAppendLength) const T* pszAppendString, size_t uAppendLength);

    // Concatenate the given string to this string.
    HRESULT Append(_In_z_ const T* pszAppendString);

    // Concatenate the given string to this string.
    HRESULT Append(const CMutableString<T> &appendString);

    // Replace the string with the given format string and subsitution.
    HRESULT Format(size_t uMax, _In_z_ const T* pszFormatString, ...);

    // Cast to non-const C char pointer.
    operator T*();

    // Cast to const C char pointer.
    operator const T*() const;

    // Return the length of the string (not including null terminator).
    size_t GetLength() const;

    bool operator==(const CMutableString<T> &other) const;
    bool operator!=(const CMutableString<T> &other) const;

private:
    CModernArray<T> _rgCharArray;   // Underlying array of characters
    size_t _uStringLength;          // Length of string (excluding null terminator); underlying array could be larger
};
