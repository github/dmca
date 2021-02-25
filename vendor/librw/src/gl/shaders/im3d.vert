VSIN(ATTRIB_POS)	vec3 in_pos;

VSOUT vec4 v_color;
VSOUT vec2 v_tex0;
VSOUT float v_fog;

void
main(void)
{
	vec4 Vertex = u_world * vec4(in_pos, 1.0);
	vec4 CamVertex = u_view * Vertex;
	gl_Position = u_proj * CamVertex;
	v_color = in_color;
	v_tex0 = in_tex0;
	v_fog = DoFog(gl_Position.w);
}
