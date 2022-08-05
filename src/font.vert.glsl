attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 combined_transform;
uniform mat4 modelview_transform;
uniform mat3 normal_transform;
uniform vec3 camera_position;
uniform vec3 glyph_position;
uniform vec4 glyph_quad;

varying vec3 nor;
varying vec3 cpos_minus_rpos;
varying vec2 tex;

void main()
{
    nor = normal_transform * normal;
    tex = texcoord;
    vec3 pos = vec3(position.xy * glyph_quad.zw + glyph_quad.xy, position.z) + glyph_position;
    cpos_minus_rpos = camera_position - (modelview_transform * vec4(pos, 1.0)).xyz;
    gl_Position = combined_transform * vec4(pos, 1.0);
}
