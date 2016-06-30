#pragma once
#include <d3d11.h>
#include "../CommonIncludes.h"

class IndexBufferWrapper
{
public:
	IndexBufferWrapper();
	virtual ~IndexBufferWrapper();

	//Core
	void Initialize(ID3D11Device* pDevice);
	void CreateBuffer(size_t count, const void* data = nullptr);
	void BindBuffer(UINT offset = 0);
	void Destroy();

	//Accessors & Mutators
	inline ID3D11Buffer* GetIndices() { return mIndexBuffer; }
	inline void SetStride(UINT stride) { mStride = stride; }
	inline UINT GetStride() { return mStride; }

	UINT AddIndices(const UINT* indices, UINT numIndicesToAdd);
private:
	IndexBufferWrapper(const IndexBufferWrapper&) = default;
	IndexBufferWrapper &operator=(const IndexBufferWrapper&) = default;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mIndexBuffer;
	UINT mStride;
	UINT mBufferSize;
	DXGI_FORMAT mBufferFormat;
};

inline IndexBufferWrapper::IndexBufferWrapper() : mIndexBuffer(nullptr), mStride(2), mBufferSize(0)
{

}

inline IndexBufferWrapper::~IndexBufferWrapper()
{
	Destroy();
}

inline void IndexBufferWrapper::Initialize(ID3D11Device* pDevice)
{
	mDevice = pDevice;
	mDevice->GetImmediateContext(&mDeviceContext);
}

inline void IndexBufferWrapper::CreateBuffer(size_t count, const void* data)
{
	assert(count != 0);
	mBufferSize = static_cast<UINT>(count);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = static_cast<UINT>(mBufferSize * mStride);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = NULL;
	bufferDesc.MiscFlags = NULL;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = NULL;
	initData.SysMemSlicePitch = NULL;
	mDevice->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
}

inline void IndexBufferWrapper::BindBuffer(UINT offset)
{
	assert(mIndexBuffer != nullptr);
	mBufferFormat = (mStride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	mDeviceContext->IASetIndexBuffer(mIndexBuffer, mBufferFormat, offset);
}

inline UINT IndexBufferWrapper::AddIndices(const UINT* indices, UINT numIndicesToAdd)
{
	size_t startingIndex;

	//First Indices To Add
	if (!mIndexBuffer)
	{
		startingIndex = 0;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = sizeof(UINT) * numIndicesToAdd;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		mDevice->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
	}
	else
	{
		D3D11_BUFFER_DESC ibd;

		mIndexBuffer->GetDesc(&ibd);
		D3D11_SUBRESOURCE_DATA iinitData;
		UINT oldBufferSize = ibd.ByteWidth / sizeof(UINT);
		UINT newBufferSize = oldBufferSize + numIndicesToAdd;

		iinitData.pSysMem = new UINT[newBufferSize];
		memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, indices, sizeof(UINT) * numIndicesToAdd);
		ibd.ByteWidth += sizeof(UINT) * numIndicesToAdd;

		ID3D11Buffer* newIndexBuffer;
		mDevice->CreateBuffer(&ibd, &iinitData, &newIndexBuffer);
		mDeviceContext->CopySubresourceRegion(newIndexBuffer, 0, 0, 0, 0, mIndexBuffer, 0, 0);
		ReleaseObject(mIndexBuffer);

		mIndexBuffer = newIndexBuffer;

		delete[] iinitData.pSysMem;
		startingIndex = oldBufferSize;
	}
	return static_cast<UINT>(startingIndex);
}

inline void IndexBufferWrapper::Destroy()
{
	//ReleaseObject(mDevice);
	//ReleaseObject(mDeviceContext);
	//ReleaseObject(mIndexBuffer);
}