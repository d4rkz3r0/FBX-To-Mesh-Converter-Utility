#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "VertexLibrary.h"
#include "../CommonIncludes.h"
using namespace std;

template <typename VertexType>
class VertexBufferWrapper
{
public:
	VertexBufferWrapper() = default;
	virtual ~VertexBufferWrapper();

	//Core
	void Initialize(ID3D11Device* pDevice);
	void CreateBuffer(size_t count, size_t vertStride, const void* data = nullptr);
	void BindBuffer(UINT bindSlot = 0);
	void Destroy();

	//Accessors & Mutators
	inline ID3D11Buffer* const GetVertices() { return mVertexBuffer; }
	UINT AddVerts(const VertexType* vertices, UINT numVertsToAdd);
	UINT GetStride() const { return sizeof(AnimationVertex); }
	UINT GetOffset() const { return 0; }

private:
	VertexBufferWrapper(const VertexBufferWrapper&) = default;
	VertexBufferWrapper &operator=(const VertexBufferWrapper&) = default;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
	UINT mStride;
	UINT mBufferSize;
};

template <typename VertexType>
UINT VertexBufferWrapper<VertexType>::AddVerts(const VertexType* vertices, UINT numVertsToAdd)
{
	size_t startingVertIndex;

	//First Vertices To Add
	if (!mVertexBuffer)
	{
		startingVertIndex = 0;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth = sizeof(VertexType) * numVertsToAdd;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;
		mDevice->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);
	}
	else
	{
		D3D11_BUFFER_DESC ibd;

		mVertexBuffer->GetDesc(&ibd);
		D3D11_SUBRESOURCE_DATA iinitData;
		UINT oldBufferSize = ibd.ByteWidth / sizeof(VertexType);
		UINT newBufferSize = oldBufferSize + numVertsToAdd;

		iinitData.pSysMem = new VertexType[newBufferSize];
		memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, vertices, sizeof(VertexType) * numVertsToAdd);
		ibd.ByteWidth += sizeof(VertexType) * numVertsToAdd;

		ID3D11Buffer* newVertexBuffer;
		Renderer::mD3DDevice->CreateBuffer(&ibd, &iinitData, &newVertexBuffer);
		Renderer::mD3DContext->CopySubresourceRegion(newVertexBuffer, 0, 0, 0, 0, mVertexBuffer, 0, 0);
		ReleaseObject(mVertexBuffer);

		mVertexBuffer = newVertexBuffer;

		delete[] iinitData.pSysMem;
		startingVertIndex = oldBufferSize;
	}
	return static_cast<UINT>(startingVertIndex);
}

template <typename VertexType>
void VertexBufferWrapper<VertexType>::BindBuffer(UINT bindSlot)
{
	assert(mVertexBuffer != nullptr);
	UINT offset = GetOffset();
	mDeviceContext->IASetVertexBuffers(bindSlot, 1, &mVertexBuffer, &mStride, &offset);
}

template <typename VertexType>
void VertexBufferWrapper<VertexType>::Destroy()
{
	//	ReleaseObject(mDevice);
	//	ReleaseObject(mDeviceContext);
	//	ReleaseObject(mVertexBuffer);
}


template <typename VertexType>
VertexBufferWrapper<VertexType>::~VertexBufferWrapper()
{
	Destroy();
}

template <typename VertexType>
void VertexBufferWrapper<VertexType>::Initialize(ID3D11Device* pDevice)
{
	mDevice = pDevice;
	mDevice->GetImmediateContext(&mDeviceContext);
}

template <typename VertexType>
void VertexBufferWrapper<VertexType>::CreateBuffer(size_t count, size_t vertStride, const void* data)
{
	assert(count != 0);
	assert(vertStride != 0);
	mBufferSize = static_cast<UINT>(count);
	mStride = static_cast<UINT>(vertStride);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(mBufferSize * mStride);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = NULL;
	bufferDesc.MiscFlags = NULL;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = NULL;
	initData.SysMemSlicePitch = NULL;
	mDevice->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
}