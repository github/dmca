const char *matfx_env_vert_src =
"uniform mat4 u_texMatrix;\n"

"VSIN(ATTRIB_POS)	vec3 in_pos;\n"

"VSOUT vec4 v_color;\n"
"VSOUT vec2 v_tex0;\n"
"VSOUT vec2 v_tex1;\n"
"VSOUT float v_fog;\n"

"void\n"
"main(void)\n"
"{\n"
"	vec4 Vertex = u_world * vec4(in_pos, 1.0);\n"
"	gl_Position = u_proj * u_view * Vertex;\n"
"	vec3 Normal = mat3(u_world) * in_normal;\n"

"	v_tex0 = in_tex0;\n"
"	v_tex1 = (u_texMatrix * vec4(Normal, 1.0)).xy;\n"

"	v_color = in_color;\n"
"	v_color.rgb += u_ambLight.rgb*surfAmbient;\n"
"	v_color.rgb += DoDynamicLight(Vertex.xyz, Normal)*surfDiffuse;\n"
"	v_color = clamp(v_color, 0.0, 1.0);\n"
"	v_color *= u_matColor;\n"

"	v_fog = DoFog(gl_Position.w);\n"
"}\n"
;
const char *matfx_env_frag_src =
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"

"uniform vec2 u_fxparams;\n"
"uniform vec4 u_colorClamp;\n"

"#define shininess (u_fxparams.x)\n"
"#define disableFBA (u_fxparams.y)\n"

"FSIN vec4 v_color;\n"
"FSIN vec2 v_tex0;\n"
"FSIN vec2 v_tex1;\n"
"FSIN float v_fog;\n"

"void\n"
"main(void)\n"
"{\n"
"	vec4 pass1 = v_color;\n"
"	vec4 envColor = max(pass1, u_colorClamp);\n"
"	pass1 *= texture(tex0, vec2(v_tex0.x, 1.0-v_tex0.y));\n"

"	vec4 pass2 = envColor*shininess*texture(tex1, vec2(v_tex1.x, 1.0-v_tex1.y));\n"

"	pass1.rgb = mix(u_fogColor.rgb, pass1.rgb, v_fog);\n"
"	pass2.rgb = mix(vec3(0.0, 0.0, 0.0), pass2.rgb, v_fog);\n"

"	float fba = max(pass1.a, disableFBA);\n"
"	vec4 color;\n"
"	color.rgb = pass1.rgb*pass1.a + pass2.rgb*fba;\n"
"	color.a = pass1.a;\n"

"	DoAlphaTest(color.a);\n"

"	FRAGCOLOR(color);\n"
"}\n"
;
