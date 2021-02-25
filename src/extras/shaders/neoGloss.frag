uniform sampler2D tex0;

uniform vec4 u_reflProps;

#define glossMult (u_reflProps.x)

FSIN vec3 v_normal;
FSIN vec3 v_light;
FSIN vec2 v_tex0;
FSIN float v_fog;

void
main(void)
{
	vec4 color = texture(tex0, vec2(v_tex0.x, 1.0-v_tex0.y));
	vec3 n = 2.0*v_normal-1.0;            // unpack
	vec3 v = 2.0*v_light-1.0;             //

	float s = dot(n, v);
	color = s*s*s*s*s*s*s*s*color*v_fog*glossMult;

	DoAlphaTest(color.a);

	FRAGCOLOR(color);
}

