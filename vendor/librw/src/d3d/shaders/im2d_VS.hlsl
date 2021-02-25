#include "standardConstants.h"

struct VS_in
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORD0;
	float4 Color		: COLOR0;
};

struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
	float4 Color		: COLOR0;
};

float4 xform : register(c41);

VS_out main(in VS_in input)
{
	VS_out output;

	output.Position = input.Position;
	output.Position.xy = output.Position.xy * xform.xy + xform.zw;
	output.TexCoord0.z = clamp((output.Position.w - fogEnd)*fogRange, fogDisable, 1.0);
	output.Position.xyz *= output.Position.w;
	output.Color = input.Color;
	output.TexCoord0.xy = input.TexCoord;

	return output;
}
