uniform sampler2D tex0;
uniform sampler2D tex1;

FSIN vec4 v_color;
FSIN vec4 v_reflcolor;
FSIN vec2 v_tex0;
FSIN vec2 v_tex1;
FSIN float v_fog;

void
main(void)
{
	vec4 pass1 = v_color*texture(tex0, vec2(v_tex0.x, 1.0-v_tex0.y));
	vec3 envmap = texture(tex1, vec2(v_tex1.x, 1.0-v_tex1.y)).rgb;
	pass1.rgb = mix(pass1.rgb, envmap, v_reflcolor.a);
	pass1.rgb = mix(u_fogColor.rgb, pass1.rgb, v_fog);
//	pass1.rgb += v_reflcolor.rgb * v_fog;

	vec3 pass2 = v_reflcolor.rgb * v_fog;

	vec4 color;
	color.rgb = pass1.rgb*pass1.a + pass2;
	color.a = pass1.a;

//	color.rgb = mix(u_fogColor.rgb, color.rgb, v_fog);
	DoAlphaTest(color.a);

	FRAGCOLOR(color);
}
