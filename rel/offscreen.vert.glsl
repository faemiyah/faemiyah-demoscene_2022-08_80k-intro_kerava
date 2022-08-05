attribute vec3 position;
attribute vec3 normal;

uniform mat4 combined_transform;
uniform mat4 modelview_transform;
uniform mat3 normal_transform;
uniform vec3 camera_position;

varying vec3 nor;
varying vec3 cpos_minus_rpos;

void main()
{
    nor = normal_transform * normal;
    cpos_minus_rpos = camera_position - (modelview_transform * vec4(position, 1.0)).xyz;
    gl_Position = combined_transform * vec4(position, 1.0);
}
