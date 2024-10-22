#pragma once
#include <d3d11.h>
#define DeleteObject(object)   if((object)  != nullptr)  { delete object; object = nullptr; }
#define DeleteObjects(objects) if((objects) != nullptr)  { delete[] objects; objects = nullptr; }
#define ReleaseObject(object)  if((object)  != nullptr)  { object->Release(); object = nullptr; }
#define ZeroMem(object)  ZeroMemory(&object, sizeof(object));

template<class T>
class ConstantBuffer
{
public:
	ConstantBuffer(const ConstantBuffer<T>& rhs) = default;
	ConstantBuffer<T>& operator=(const ConstantBuffer<T>& rhs) = default;

	ID3D11Buffer* mBuffer;
	bool mInitialized;

public:
	ConstantBuffer() {}
	void ShutDown();
	~ConstantBuffer()
	{
		//ReleaseObject(mBuffer);
	};

	T Data;

	ID3D11Buffer* Buffer()const
	{
		return mBuffer;
	}

	HRESULT Initialize(ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;

		hr = device->CreateBuffer(&desc, 0, &mBuffer);
		mInitialized = true;
		return hr;
	}

	void ApplyChanges(ID3D11DeviceContext* dc)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		dc->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &Data, sizeof(T));
		dc->Unmap(mBuffer, 0);
	}
};

template <class T>
void ConstantBuffer<T>::ShutDown()
{
	ReleaseObject(mBuffer);
}