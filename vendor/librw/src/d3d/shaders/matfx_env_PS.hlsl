struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
	float2 TexCoord1	: TEXCOORD1;
	float4 Color		: COLOR0;
};

sampler2D diffTex : register(s0);
sampler2D envTex : register(s1);

float4 fogColor : register(c0);

float4 fxparams : register(c1);
float4 colorClamp : register(c2);

#define shininess (fxparams.x)
#define disableFBA (fxparams.y)

float4 main(VS_out input) : COLOR
{
	float4 pass1 = input.Color;
	float4 envColor = max(pass1, colorClamp);
#ifdef TEX
	pass1 *= tex2D(diffTex, input.TexCoord0.xy);
#endif

	float4 pass2 = envColor*shininess*tex2D(envTex, input.TexCoord1.xy);

	pass1.rgb = lerp(fogColor.rgb, pass1.rgb, input.TexCoord0.z);
	pass2.rgb = lerp(float3(0.0, 0.0, 0.0), pass2.rgb, input.TexCoord0.z);

	// We simulate drawing this in two passes.
	// First pass with standard blending, second with addition
	// We premultiply alpha so render state should be one.
	// For FB alpha rendering assume that diffuse alpha (pass1.a) was
	// written to framebuffer, so just multiply pass2 by it as well then.
	float fba = max(pass1.a, disableFBA);
	float4 color;
	color.rgb = pass1.rgb*pass1.a + pass2.rgb*fba;
	color.a = pass1.a;

	return color;
}
