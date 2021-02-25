sampler2D tex0 : register(s0);
float glossMult : register(c1);

struct VS_out
{
	float4 Position         : POSITION;
	float3 TexCoord0        : TEXCOORD0;
	float3 Normal           : COLOR0;
	float3 Light            : COLOR1;
};

float4 main(VS_out input) : COLOR
{
	float4 color = tex2D(tex0, input.TexCoord0.xy);
	float3 n = 2.0*input.Normal-1.0;            // unpack
	float3 v = 2.0*input.Light-1.0;             //

	float s = dot(n, v);
	return s*s*s*s*s*s*s*s*color*input.TexCoord0.z*glossMult;
}
