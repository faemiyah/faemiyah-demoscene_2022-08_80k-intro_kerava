attribute vec3 position;
attribute vec2 texcoord;

uniform vec4 scoords;

varying vec2 tex;
varying vec2 tex1;
varying vec2 tex2;
varying vec2 tex3;
varying vec2 tex4;
varying vec2 stex;

vec2 rcoords(float len, float rot)
{
    return vec2(cos(rot), sin(rot)) * len;
}

void main()
{
    tex = texcoord;

    // Rotate offset coordinates slightly, since we do it in vertex shader, it doesn't cost anything.
    // Rotation has been chosen pretty much at random
    const float diff = 0.8;
    const float angle = 0.31;
    const float pi4 = 0.7853982;
    tex1 = texcoord + rcoords(diff, angle) * scoords.xy;
    tex2 = texcoord + rcoords(diff, angle + pi4) * scoords.xy;
    tex3 = texcoord + rcoords(diff, angle + pi4 * 2.0) * scoords.xy;
    tex4 = texcoord + rcoords(diff, angle + pi4 * 3.0) * scoords.xy;
    stex = texcoord * scoords.zw;
    gl_Position=vec4(position, 1.0);
}
