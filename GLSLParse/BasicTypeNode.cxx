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
#include "BasicTypeNode.hxx"
#include "IStringStream.hxx"

MtDefine(BasicTypeNode, CGLSLParser, "BasicTypeNode");

//+----------------------------------------------------------------------------
//
//  Function:   Constructor
//
//-----------------------------------------------------------------------------
BasicTypeNode::BasicTypeNode() : 
    _basicType(0),
    _uHLSLNameIndex(0)
{
}

//+----------------------------------------------------------------------------
//
//  Function:   Initialize
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::Initialize(
    __in CGLSLParser* pParser,                  // The parser that owns the tree
    int basicType                               // The type stored in the node
    )
{
    ParseTreeNode::Initialize(pParser);

    _basicType = basicType;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   InitializeAsClone
//
//  Synopsis:   Clone the basic type
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::InitializeAsClone(__in ParseTreeNode* pOriginal)
{
    BasicTypeNode* pOriginalBasicTypeNode = pOriginal->GetAs<BasicTypeNode>();

    _basicType = pOriginalBasicTypeNode->_basicType;

    return S_OK;
}

//+----------------------------------------------------------------------------
//
//  Function:   VerifySelf
//
//  Synopsis:   Create the GLSL type that represents the basic type
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::VerifySelf()
{
    return GLSLType::CreateFromBasicTypeToken(_basicType, &_spType);
}

//+----------------------------------------------------------------------------
//
//  Function:   OutputHLSL
//
//  Synopsis:   Output the HLSL name for the basic type
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::OutputHLSL(__in IStringStream* pOutput)
{
    CHK_START;

    const BasicGLSLTypeInfo* pInfo;
    CHK(BasicGLSLTypeInfo::GetInfo(_basicType, &pInfo));

    const char* pszName = nullptr;
    switch (_uHLSLNameIndex)
    {
    case 0:
        pszName = pInfo->_pszHLSLName;
        break;

    case 1:
        pszName = pInfo->_pszAltHLSLName;
        break;

    default:
        Assert(false);
        CHK(E_UNEXPECTED);
    }

    // Write out the equivalent HLSL name of the type
    CHK(pOutput->WriteString(pszName));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetDumpString
//
//  Synopsis:   Output the basic type name into the dump string
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::GetDumpString(__in IStringStream* pOutput)
{
    CHK_START;

    const BasicGLSLTypeInfo* pInfo;
    CHK(BasicGLSLTypeInfo::GetInfo(_basicType, &pInfo));
    const char* pszTypeName = (pInfo->_pszGLSLName != nullptr) ? pInfo->_pszGLSLName : "unknown";

    CHK(pOutput->WriteFormat(1024, "BasicType type=%s", pszTypeName));

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   SetHLSLNameIndex
//
//  Synopsis:   When texture samplers are used in GLSL, the corresponding HLSL
//              code needs to create a sampler and texture for each GLSL
//              sampler.
//
//              When defining parameters to functions, both the sampler and
//              texture need to be defined. This is done by outputting the
//              parameter twice, and changing the HLSL name index.
//
//              If the index given is out of range for this type, then
//              this is a no-op. It will only affect types with the
//              large enough number of HLSL names, so that expressions with
//              non-sampler identifiers will still work.
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::SetHLSLNameIndex(UINT uIndex)
{
    CHK_START;

    const BasicGLSLTypeInfo* pInfo;
    CHK(BasicGLSLTypeInfo::GetInfo(_basicType, &pInfo));

    // If the index is not zero, this only does something if the
    // type has an alternate and the new index is one.
    if (uIndex != 0)
    {
        if (uIndex == 1 && pInfo->_pszAltHLSLName != nullptr)
        {
            _uHLSLNameIndex = uIndex;
        }
    }
    else
    {
        _uHLSLNameIndex = uIndex;
    }

    CHK_RETURN;
}

//+----------------------------------------------------------------------------
//
//  Function:   GetType
//
//  Synopsis:   Return the verified type
//
//-----------------------------------------------------------------------------
HRESULT BasicTypeNode::GetType(__deref_out GLSLType** ppType) const
{
    AssertSz(IsVerified() && _spType != nullptr, "No one should ask for the type before the node is verified");

    _spType.CopyTo(ppType);

    return S_OK;
}
