attribute vec3 position;
attribute vec3 normal;
attribute vec4 bone_weight;
attribute vec4 bone_ref;

uniform mat4 combined_transform;
uniform mat4 modelview_transform;
uniform mat3 normal_transform;
uniform mat4 skeleton[64];
uniform vec3 camera_position;

varying vec3 nor;
varying vec3 cpos_minus_rpos;

void main()
{
    ivec4 iref = ivec4(bone_ref);
    mat4 bone_transform = skeleton[iref.x] * bone_weight.x + skeleton[iref.y] * bone_weight.y + skeleton[iref.z] * bone_weight.z;
    vec3 lpos = (bone_transform * vec4(position, 1.0)).xyz;
#if defined(DNLOAD_USE_LD)
    vec4 col0 = bone_transform[0];
    vec4 col1 = bone_transform[1];
    vec4 col2 = bone_transform[2];
    vec4 col3 = bone_transform[3];
    vec3 lnor = mat3(col0.xy, col0.z, col1.xy, col1.z, col2.xy, col2.z) * normal;
#else
    vec3 lnor = mat3(bone_transform) * normal;
#endif

    nor = normal_transform * lnor;
    cpos_minus_rpos = camera_position - (modelview_transform * vec4(lpos, 1.0)).xyz;
    gl_Position = combined_transform * vec4(lpos, 1.0);
}
