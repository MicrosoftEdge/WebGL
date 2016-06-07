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

#include "MutableString.hxx"

#include "CharTraits.hxx"

template <typename T>
CMutableString<T>::CMutableString()
{
    _uStringLength = 0;
}

// Constructor with minimum capacity
template <typename T>
CMutableString<T>::CMutableString(size_t minimumCapacity)
{
    this->SetInitialSize(minimumCapacity);
}

// Clears contents and ensures the string capacity is at least the given size.
template <typename T>
void CMutableString<T>::SetInitialSize(size_t uLength)
{
    // Ensure enough room for string of given size
    _rgCharArray.EnsureCapacity(CheckedMath::SizeT_To_UInt32(CheckedMath::SizeT_Add(uLength, 1)));
    _rgCharArray.Resize(1);

    // String is now zero length
    _uStringLength = 0;
    _rgCharArray[0] = '\0';
}

// Set to the value of the given C language string.
template <typename T>
HRESULT CMutableString<T>::Set(_In_opt_z_ const T* pszSetString)
{
    if (pszSetString == nullptr)
    {
        // Make sure we look like nullptr if given it
        _rgCharArray.RemoveAll();
        _uStringLength = 0;
        return S_OK;
    }
    else
    {
        // Whatever is allocated, act like we are zero sized and append
        _uStringLength = 0;
        return Append(pszSetString);
    }
}

// Concatenate the given string to this string.
template <typename T>
HRESULT CMutableString<T>::Append(_In_count_(uAppendLength) const T* pszAppendString, size_t uAppendLength)
{
    // Check that the new capacity will not cause an overflow
    size_t newCapacity = CheckedMath::SizeT_Add(CheckedMath::SizeT_Add(_uStringLength, uAppendLength), 1);

    // Ensure our array is large enough for the new required capacity
    _rgCharArray.EnsureCapacity(CheckedMath::SizeT_To_UInt32(newCapacity));

    // Copy the string and re-null-terminate - if we had anything allocated, even a zero length string, then
    // we will copy at the beginning, which we want. If nothing was allocated, then we have ensured enough
    // capacity and we still get what we want.
    T* pCurrentStart = _rgCharArray.GetData();
    HRESULT hr = CharTraits<T>::StringCchCopyN(&pCurrentStart[_uStringLength],
                                               _rgCharArray.GetCapacity() - _uStringLength,
                                               pszAppendString,
                                               uAppendLength);

    if (SUCCEEDED(hr))
    {
        // Adjust our internal view of the string size
        _uStringLength += uAppendLength;
    }

    return hr;
}

// Concatenate the given string to this string.
template <typename T>
HRESULT CMutableString<T>::Append(_In_z_ const T* pszAppendString)
{
    // Find out the size of the append string
    size_t appendLength;
    CharTraits<T>::StringCchLength(pszAppendString, STRSAFE_MAX_CCH, &appendLength);

    // Call the length provided version of Append
    return Append(pszAppendString, appendLength);
}

// Concatenate the given string to this string.
template <typename T>
HRESULT CMutableString<T>::Append(const CMutableString<T> &appendString)
{
    return Append(appendString, appendString.GetLength());
}

// Replace the string with the given format string and subsitution.
template <typename T>
HRESULT CMutableString<T>::Format(size_t uMax, _In_z_ const T* pszFormatString, ...)
{
    va_list args;
    va_start(args, pszFormatString);

    // Make sure we have enough space for the maximum permitted size
    _rgCharArray.EnsureCapacity(CheckedMath::SizeT_To_UInt32(uMax));

    // Do the format
    HRESULT hr = CharTraits<T>::StringCchVPrintf(_rgCharArray.GetData(), uMax, pszFormatString, args);

    if (FAILED(hr))
    {
        return hr;
    }

    // Find out the new size
    CharTraits<T>::StringCchLength(_rgCharArray.GetData(), uMax, &_uStringLength);

    return S_OK;
}

// Cast to non-const C char pointer.
template <typename T>
CMutableString<T>::operator T*()
{
    return _rgCharArray.GetData();
}

// Cast to const C char pointer.
template <typename T>
CMutableString<T>::operator const T*() const
{
    return _rgCharArray.GetData();
}

// Return the length of the string (not including null terminator).
template <typename T>
size_t CMutableString<T>::GetLength() const
{
    return _uStringLength;
}

template <typename T>
bool CMutableString<T>::operator==(const CMutableString<T> &other) const
{
    if (_uStringLength == other._uStringLength)
    {
        return (CharTraits<T>::Compare(_rgCharArray.GetData(), other._rgCharArray.GetData()) == 0);
    }

    return false;
}

template <typename T>
bool CMutableString<T>::operator!=(const CMutableString<T> &other) const
{
    if (_uStringLength == other._uStringLength)
    {
        return (CharTraits<T>::Compare(_rgCharArray.GetData(), other._rgCharArray.GetData()) != 0);
    }

    return true;
}

template CMutableString<char>;
template CMutableString<wchar_t>;
