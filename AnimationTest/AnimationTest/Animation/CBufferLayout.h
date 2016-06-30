#pragma once
#include "../CommonIncludes.h"


#define CB_PER_OBJECT_MATRIX_REGISTER_SLOT 0
struct cbPerObjectMatrix
{
	cbPerObjectMatrix()
	{
		ZeroMemory(this, sizeof(this));
	}
	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewProjMatrix;
	XMFLOAT4X4 mInvTransWorldMatrix;
};


#define CB_PER_OBJECT_MATERIAL_REGISTER_SLOT 1
struct cbPerObjectMaterial
{
	cbPerObjectMaterial()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4   mSpecularColor;
	XMFLOAT4   mDiffuseColor;
	XMFLOAT4   mAmbientColor;
	XMFLOAT4   mEmissiveColor;
	float	   mSpecularPower;
	XMFLOAT3   mPadding;
};


#define CB_PER_OBJECT_SKINNING_REGISTER_SLOT 2
#define MAX_BONES 64
struct cbPerObjectSkinningData
{
	cbPerObjectSkinningData()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4X4 BoneTransforms[MAX_BONES];
};


#define CB_PER_FRAME_LIGHT_CAMERA_SLOT 9
struct cbPerFrameLightCameraData
{
	cbPerFrameLightCameraData()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	XMFLOAT4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 LightPosition = { 0.0f, 0.0f, 0.0f };
	float    LightRadius = 10.0f;
	XMFLOAT3 CameraPosition;
};