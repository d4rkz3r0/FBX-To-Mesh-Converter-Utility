#pragma once
#include <DirectXMath.h>

struct basic_vert
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 uv;
	DirectX::XMFLOAT4 normal;
};

struct animated_vert
{
	DirectX::XMFLOAT4	pos;
	DirectX::XMFLOAT4	uv;
	DirectX::XMFLOAT4	normal;

	int32_t			bones[4] = { -1, -1, -1, -1 };
	DirectX::XMFLOAT4	weights = { 0.0f, 0.0f, 0.0f, 0.0f };
};