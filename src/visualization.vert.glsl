attribute vec3 position;

uniform mat4 combined_transform;

void main()
{
    gl_Position = combined_transform * vec4(position, 1.0);
}
