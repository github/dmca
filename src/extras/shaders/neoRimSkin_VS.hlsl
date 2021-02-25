#include "standardConstants.h"

float4x3 boneMatrices[64] : register(c41);

struct VS_in
{
	float4 Position		: POSITION;
	float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD0;
	float4 Prelight		: COLOR0;
	float4 Weights		: BLENDWEIGHT;
	int4 Indices		: BLENDINDICES;
};

struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;	// also fog
	float4 Color		: COLOR0;
};

float3	    viewVec     : register(c233);
float4      rampStart   : register(c234);
float4      rampEnd     : register(c235);
float3      rimData     : register(c236);

VS_out main(in VS_in input)
{
	VS_out output;

	int j;
	float3 SkinVertex = float3(0.0, 0.0, 0.0);
	float3 SkinNormal = float3(0.0, 0.0, 0.0);
	for(j = 0; j < 4; j++){
		SkinVertex += mul(input.Position, boneMatrices[input.Indices[j]]).xyz * input.Weights[j];
		SkinNormal += mul(input.Normal, (float3x3)boneMatrices[input.Indices[j]]).xyz * input.Weights[j];
	}

	output.Position = mul(combinedMat, float4(SkinVertex, 1.0));
	float3 Vertex = mul(worldMat, float4(SkinVertex, 1.0)).xyz;
	float3 Normal = mul(normalMat, SkinNormal);

	output.TexCoord0.xy = input.TexCoord;

	output.Color = input.Prelight;
	output.Color.rgb += ambientLight.rgb * surfAmbient;

	int i;
//#ifdef DIRECTIONALS
	for(i = 0; i < numDirLights; i++)
		output.Color.xyz += DoDirLight(lights[i+firstDirLight], Normal)*surfDiffuse;
//#endif
//#ifdef POINTLIGHTS
//	for(i = 0; i < numPointLights; i++)
//		output.Color.xyz += DoPointLight(lights[i+firstPointLight], Vertex.xyz, Normal)*surfDiffuse;
//#endif
//#ifdef SPOTLIGHTS
//	for(i = 0; i < numSpotLights; i++)
//		output.Color.xyz += DoSpotLight(lights[i+firstSpotLight], Vertex.xyz, Normal)*surfDiffuse;
//#endif

	// rim light
	float f = rimData.x - rimData.y*dot(Normal, viewVec);
	float4 rimlight = saturate(lerp(rampEnd, rampStart, f)*rimData.z);
	output.Color.xyz += rimlight.xyz;

	// PS2 clamps before material color
	output.Color = clamp(output.Color, 0.0, 1.0);
	output.Color *= matCol;

	output.TexCoord0.z = clamp((output.Position.w - fogEnd)*fogRange, fogDisable, 1.0);

	return output;
}
