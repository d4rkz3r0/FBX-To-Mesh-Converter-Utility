#include "SharedDefines.hlsl"

float4 main(PS_INPUT IN) : SV_Target
{
	//Init
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 base = gDiffuseTexture.Sample(gWrapSampler, IN.inUVs);
	float3 bump = gNormalTexture.Sample(gWrapSampler, IN.inUVs);
	float specularLevel = 0.7f;

	//Normalization
	float3 normLightVec = normalize(IN.inLightVector);
	float3 normHalfAngle = normalize(IN.inHalfAngle);

	//Lerp
	float bumpynessRatio = 0.75f;
	float3 smoothnessBase = { 0.5f, 0.5f, 1.0f };
	bump = lerp(smoothnessBase, bump, bumpynessRatio);
	bump = normalize((bump * 2.0f) - 1.0f);

	//1) Surface Normal Dot lightDir
	float4 n_dot_l = dot(bump, normLightVec);
	//2) Normal vec Dot Half angle
	float4 n_dot_h = dot(bump, normHalfAngle);

	//Combine Factors
	finalColor.rgb = (base * gAmbientColor) + (base * gLightColor * max(0, n_dot_l)) + (gObjectSpecularColor * specularLevel * pow(max(0, n_dot_h), gObjectSpecularPower));
	finalColor.a = gObjectAmbientColor.a;

	return finalColor;
}