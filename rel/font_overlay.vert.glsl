attribute vec3 position;
attribute vec2 texcoord;

uniform mat4 modelview_transform;
uniform vec3 glyph_position;
uniform vec4 glyph_quad;
uniform vec4 scoords;

varying vec2 tex;
varying vec2 stex;

void main()
{
    tex = texcoord;
    vec3 pos = vec3(position.xy * glyph_quad.zw + glyph_quad.xy, position.z) + glyph_position;
    gl_Position = modelview_transform * vec4(pos, 1.0);
    stex = gl_Position.xy * scoords.zw * 0.5;
}
