struct VS_out {
	float4 Position		: POSITION;
	float2 TexCoord0	: TEXCOORD0;
	float2 TexCoord1	: TEXCOORD1;
	float4 Color		: COLOR0;
};

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 main(VS_out input) : COLOR
{
	float4 color = input.Color;
	color *= tex2D(tex0, input.TexCoord0.xy);
	color *= tex2D(tex1, input.TexCoord1.xy);
	return color;
}
