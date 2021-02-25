uniform mat4 u_boneMatrices[64];

uniform vec3 u_viewVec;
uniform vec4 u_rampStart;
uniform vec4 u_rampEnd;
uniform vec3 u_rimData;

VSIN(ATTRIB_POS)	vec3 in_pos;

VSOUT vec4 v_color;
VSOUT vec2 v_tex0;
VSOUT float v_fog;

void
main(void)
{
	vec3 SkinVertex = vec3(0.0, 0.0, 0.0);
	vec3 SkinNormal = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < 4; i++){
		SkinVertex += (u_boneMatrices[int(in_indices[i])] * vec4(in_pos, 1.0)).xyz * in_weights[i];
		SkinNormal += (mat3(u_boneMatrices[int(in_indices[i])]) * in_normal) * in_weights[i];
	}

	vec4 Vertex = u_world * vec4(SkinVertex, 1.0);
	gl_Position = u_proj * u_view * Vertex;
	vec3 Normal = mat3(u_world) * SkinNormal;

	v_tex0 = in_tex0;

	v_color = in_color;
	v_color.rgb += u_ambLight.rgb*surfAmbient;
	v_color.rgb += DoDynamicLight(Vertex.xyz, Normal)*surfDiffuse;

	// rim light
	float f = u_rimData.x - u_rimData.y*dot(Normal, u_viewVec);
	vec4 rimlight = clamp(mix(u_rampEnd, u_rampStart, f)*u_rimData.z, 0.0, 1.0);
	v_color.rgb += rimlight.rgb;

	v_color = clamp(v_color, 0.0, 1.0);
	v_color *= u_matColor;

	v_fog = DoFog(gl_Position.z);
}
