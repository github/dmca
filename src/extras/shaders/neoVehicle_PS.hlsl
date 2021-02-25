struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
	float2 TexCoord1	: TEXCOORD1;
	float4 Color		: COLOR0;
	float4 ReflColor	: COLOR1;
};

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 fogColor : register(c0);

float4 main(VS_out input) : COLOR
{
	float4 pass1 = input.Color;
//#ifdef TEX
	pass1 *= tex2D(tex0, input.TexCoord0.xy);
//#endif
	float3 envmap = tex2D(tex1, input.TexCoord1).rgb;
	pass1.rgb = lerp(pass1.rgb, envmap, input.ReflColor.a);
//	pass1.rgb = envmap;
//	pass1.rgb *= input.ReflColor.a;
	pass1.rgb = lerp(fogColor.rgb, pass1.rgb, input.TexCoord0.z);
//	pass1.rgb += input.ReflColor.rgb * input.TexCoord0.z;

	float3 pass2 = input.ReflColor.rgb*input.TexCoord0.z;

	float4 color;
	color.rgb = pass1.rgb*pass1.a + pass2;
	color.a = pass1.a;

	return color;
}
