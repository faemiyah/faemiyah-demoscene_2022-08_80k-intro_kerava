uniform float color;

void main()
{
#if defined(USE_LD)
    if(debug_mode != 0)
    {
        gl_FragColor = vec4(vec3(0.5), 1.0);
        return;
    }
#endif

    gl_FragColor = vec4(vec3(0.0), color_encode(color, 0.0));
}
