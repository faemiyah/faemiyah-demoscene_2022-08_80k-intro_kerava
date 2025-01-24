uniform vec2 projection_range;

varying vec3 nor;
varying vec3 cpos_minus_rpos;

void main()
{
    vec3 snorm = normalize(nor);

#if defined(DNLOAD_USE_LD)
    if(debug_mode != 0)
    {
        gl_FragColor = vec4(0.5 + 0.5 * snorm, 1.0);
        return;
    }
#endif

    vec3 fwd = normalize(cpos_minus_rpos);
    float depth_range = projection_range.y - projection_range.x;
    gl_FragColor = vec4(depth_encode(gl_FragCoord.z / gl_FragCoord.w / depth_range), color_encode(0.0, 1.0 - dot(fwd, snorm)));
}
