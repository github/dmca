#include "standardConstants.h"

struct VS_in
{
	float4 Position         : POSITION;
	float2 TexCoord         : TEXCOORD0;
};

struct VS_out
{
	float4 Position         : POSITION;
	float3 TexCoord0        : TEXCOORD0;
	float3 Normal           : COLOR0;
	float3 Light            : COLOR1;
};

float3 eye : register(c41);

VS_out main(in VS_in input)
{
	VS_out output;

	output.Position = mul(combinedMat, input.Position);
	float3 Vertex = mul(worldMat, input.Position).xyz;
	output.TexCoord0.xy = input.TexCoord;

	float3 viewVec = normalize(eye - Vertex);
	float3 Light = normalize(viewVec - lights[0].direction.xyz);
	output.Normal = 0.5*(1.0 + float3(0.0, 0.0, 1.0));    // compress
	output.Light  = 0.5*(1.0 + Light);                    //

	output.TexCoord0.z = clamp((output.Position.w - fogEnd)*fogRange, fogDisable, 1.0);

	return output;
}
