uniform sampler2D tex0;
uniform sampler2D tex1;

FSIN vec4 v_color;
FSIN vec2 v_tex0;
FSIN vec2 v_tex1;
FSIN float v_fog;

void
main(void)
{
	vec4 color;
	color = v_color*texture(tex0, vec2(v_tex0.x, 1.0-v_tex0.y));
	color *= texture(tex1, vec2(v_tex1.x, 1.0-v_tex1.y));

	FRAGCOLOR(color);
}

