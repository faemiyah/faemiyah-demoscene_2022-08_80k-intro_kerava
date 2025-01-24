uniform vec2 projection_range;
uniform sampler2D glyph;

varying vec3 nor;
varying vec3 cpos_minus_rpos;
varying vec2 tex;

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

    float gstr = texture2D(glyph, tex).r;
    if(gstr <= 0.3)
    {
        discard;
    }

    vec3 fwd = normalize(cpos_minus_rpos);
    float depth_range = projection_range.y - projection_range.x;
    gl_FragColor = vec4(depth_encode(gl_FragCoord.z / gl_FragCoord.w / depth_range), color_encode(0.8, 1.0 - dot(fwd, snorm)));
}
