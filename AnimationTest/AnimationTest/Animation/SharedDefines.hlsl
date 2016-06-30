//Macros
#define MAX_BONES 64

//Shader I/O Structs
struct VS_INPUT
{
	float3 inPositionL      : POSITION;
	float2 inUVs	        : TEXCOORD;
	float3 inNormal         : NORMAL;
	float3 inTangent        : TANGENT;
	float4 BonesIndices     : BONE;
	float4 BoneWeights      : BLENDWEIGHT;
};


struct VS_OUTPUT
{
	float4 oPositionHClip   : SV_POSITION;
	float4 oPositionWorld   : POSITION;
	float2 oUVs			    : TEXCOORD;
	float3 oNormal		    : NORMAL;
	float3 oTangent		    : TANGENT;
	float3 oBiTangent	    : BITANGENT;
	float3 oLightVector     : TEXCOORD1;
	float3 oHalfAngle   : TEXCOORD2;
};


struct PS_INPUT
{
	float4 inPositionHClip  : SV_POSITION;
	float4 inPositionWorld  : POSITION;
	float2 inUVs			: TEXCOORD;
	float3 inNormal		    : NORMAL;
	float3 inTangent		: TANGENT;
	float3 inBiTangent	    : BITANGENT;
	float3 inLightVector     : TEXCOORD1;
	float3 inHalfAngle   : TEXCOORD2;
};

//Shader Constant Buffer Structs
cbuffer CBufferPerObjectMatrixInfo : register(b0)
{
	float4x4 gObjectWorldMatrix;
	float4x4 gViewProjectionMatrix;
	float4x4 gInvTransposeObjectToWorld;
}


cbuffer CBufferPerObjectMaterialInfo : register(b1)
{
	float4 gObjectSpecularColor;
	float4 gObjectDiffuseColor;
	float4 gObjectAmbientColor;
	float4 gObjectEmissiveColor;
	float  gObjectSpecularPower;
	float  gPadding[3];
}


cbuffer CBufferPerObjectSkinningInfo : register(b2)
{
	float4x4 gBoneTransforms[MAX_BONES];
}


cbuffer CBufferPerFrame : register(b9)
{
	float4 gAmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	float4 gLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float3 gLightPosition = { 0.0f, 0.0f, 0.0f };
	float  gLightRadius = 10.0f;
	float3 gCameraPosition;
}


//Texture Maps
Texture2D gDiffuseTexture        : register(t0);
Texture2D gNormalTexture         : register(t1);


//Sampler State Objects
sampler gWrapSampler     : register(s0);