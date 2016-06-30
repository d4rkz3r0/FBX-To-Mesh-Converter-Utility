#pragma once
#include "../CommonIncludes.h"

struct AnimationVertex
{
	AnimationVertex() :
		Position(0.0f, 0.0f, 0.0f),
		TextureCoordinates(0.0f, 0.0f),
		Normal(0.0f, 0.0f, 0.0f),
		Tangent(0.0f, 0.0f, 0.0f),
		BoneIndices(0.0f, 0.0f, 0.0f, 0.0f),
		BoneWeights(0.0f, 0.0f, 0.0f, 0.0f)
	{
		ZeroMem(*this);
	}

	AnimationVertex(const XMFLOAT3& position, const XMFLOAT2& textureCoordinates, const XMFLOAT3& normal, const XMFLOAT3& tangent, XMFLOAT4& boneIndices, const XMFLOAT4& boneWeights) :
		Position(position),
		TextureCoordinates(textureCoordinates),
		Normal(normal),
		Tangent(tangent),
		BoneIndices(boneIndices),
		BoneWeights(boneWeights) { }

	XMFLOAT3 Position;
	XMFLOAT2 TextureCoordinates;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT4 BoneIndices;
	XMFLOAT4 BoneWeights;
};