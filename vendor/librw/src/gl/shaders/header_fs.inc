const char *header_frag_src =
"#ifdef USE_UBOS\n"
"layout(std140) uniform State\n"
"{\n"
"	vec2 u_alphaRef;\n"
"	vec4  u_fogData;\n"
"	vec4  u_fogColor;\n"
"};\n"
"#else\n"
"uniform vec2 u_alphaRef;\n"

"uniform vec4  u_fogData;\n"
"uniform vec4  u_fogColor;\n"
"#endif\n"

"#define u_fogStart (u_fogData.x)\n"
"#define u_fogEnd (u_fogData.y)\n"
"#define u_fogRange (u_fogData.z)\n"
"#define u_fogDisable (u_fogData.w)\n"

"#ifndef GL2\n"
"out vec4 fragColor;\n"
"#endif\n"

"void DoAlphaTest(float a)\n"
"{\n"
"	if(a < u_alphaRef.x || a >= u_alphaRef.y)\n"
"		discard;\n"
"}\n"
;
