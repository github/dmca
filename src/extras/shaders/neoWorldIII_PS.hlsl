sampler2D Diffuse : register(s0);
sampler2D Light : register(s1);
float4 fogColor : register(c0);
float4 lm : register(c1);
 
struct PS_INPUT
{
	float4 Color	: COLOR0;
	float3 Tex0	: TEXCOORD0;
	float2 Tex1	: TEXCOORD1;
};

float4
main(PS_INPUT IN) : COLOR
{
	float4 t0 = tex2D(Diffuse, IN.Tex0.xy);
	float4 t1 = tex2D(Light, IN.Tex1);

	float4 col = t0*IN.Color*(1 + lm*(2*t1-1));
	col.a = IN.Color.a*t0.a*lm.a;

	col.rgb = lerp(fogColor.rgb, col.rgb, IN.Tex0.z);

	return col;
}
