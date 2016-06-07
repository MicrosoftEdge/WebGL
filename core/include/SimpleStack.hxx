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

//+-----------------------------------------------------------------------------
//
//  Class:      CSimpleStack
//
//  Synopsis:   Implements a simple stack based on CModernArray.
//
//------------------------------------------------------------------------------
template<typename TItem, typename TElementTraits = CDefaultTraits< TItem > >
class CSimpleStack
{
public:
    HRESULT_VOID Push(const TItem& item)
    {
        _aryItem.Add(item);
        return S_OK_VOID;
    }

    HRESULT Pop(__out TItem& item)
    {
        CHK_START;

        CHKB(!IsEmpty());
        UINT uCount = _aryItem.GetCount();
        item = _aryItem[uCount-1];
        _aryItem.RemoveAt(uCount - 1);

        CHK_RETURN;
    }

    bool IsEmpty() const
    {
        return _aryItem.GetCount() == 0;
    }

    HRESULT Top(__out TItem& item) const
    {
        CHK_START;

        CHKB(!IsEmpty());
        UINT uCount = _aryItem.GetCount();
        item = _aryItem[uCount-1];

        CHK_RETURN;
    }

    UINT Find(
        const TItem& item                      // Element to find
        ) const
    {
        return _aryItem.Find(item);
    }

    UINT Size() const { return _aryItem.GetCount(); }

    TItem& operator[](UINT nIndex)
    {
        return _aryItem[nIndex];
    }

    TItem Item(UINT nIndex) const
    {
        Assert(IsLegalIndex(nIndex));
        return _aryItem[nIndex];
    }

    const TItem &ItemRef(UINT nIndex) const
    {
        Assert(IsLegalIndex(nIndex));
        return _aryItem[nIndex];
    }

    HRESULT_VOID EnsureCapacity(UINT cCapacityDesired)
    {
        _aryItem.EnsureCapacity(cCapacityDesired);
        return S_OK_VOID;
    }

private:
    bool IsLegalIndex(UINT uIndex) const
    {
        UINT uCount = _aryItem.GetCount();
        if (uIndex < uCount && uCount != 0)
        {
            return true;
        }
        return false;        
    }

private:
    // Array of TItem
    CModernArray<TItem, TElementTraits> _aryItem;
};
