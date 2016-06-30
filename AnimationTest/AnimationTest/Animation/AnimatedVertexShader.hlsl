#include "SharedDefines.hlsl"

VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	//Calc It
	float4x4 skinningMatrix = (float4x4)0;
	skinningMatrix = Input.BoneWeights.x * gBoneTransforms[Input.BonesIndices.x];
	skinningMatrix += Input.BoneWeights.y * gBoneTransforms[Input.BonesIndices.y];
	skinningMatrix += Input.BoneWeights.z * gBoneTransforms[Input.BonesIndices.z];
	skinningMatrix += Input.BoneWeights.w * gBoneTransforms[Input.BonesIndices.w];

	//Apply It
	float4 vAnimatedPosition = mul(float4(Input.inPositionL, 1.0f), skinningMatrix);
	float4 vAnimatedTangent = mul(float4(Input.inTangent, 0.0f), skinningMatrix);
	float4 vAnimatedNormal = mul(float4(Input.inNormal, 0.0f), skinningMatrix);

	//Transform Between Coordinate Systems
	OUT.oPositionHClip = mul(mul(vAnimatedPosition, gObjectWorldMatrix), gViewProjectionMatrix);
	OUT.oPositionWorld = mul(vAnimatedPosition, gObjectWorldMatrix);
	OUT.oNormal = mul(vAnimatedNormal.xyz, (float3x3)gInvTransposeObjectToWorld);
	OUT.oTangent = mul(vAnimatedTangent.xyz, (float3x3)gInvTransposeObjectToWorld);
	OUT.oBiTangent = cross(OUT.oNormal, OUT.oTangent);


	//Build TBN Matrix
	float3x3 rotation = float3x3(OUT.oTangent, OUT.oBiTangent, OUT.oNormal);

	//LightVec 2 TextureSpace via TBN
	float3 temp_lightDir0 = normalize(gLightPosition - vAnimatedPosition);
	temp_lightDir0 = normalize(mul(rotation, temp_lightDir0));
	OUT.oLightVector = temp_lightDir0;

	//CameraVec 2 TextureSpace via TBN
	float3 eyeDir = normalize(gCameraPosition - vAnimatedPosition);
	eyeDir = normalize(mul(rotation, gCameraPosition));
	OUT.oHalfAngle = OUT.oLightVector + eyeDir;

	//Pass-Through
	OUT.oUVs = Input.inUVs;

	return OUT;
}