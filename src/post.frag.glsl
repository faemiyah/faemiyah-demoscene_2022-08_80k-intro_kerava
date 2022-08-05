#define VGL_DISABLE_DEPTH_TEXTURE

uniform sampler2D color;
#if !defined(VGL_DISABLE_DEPTH_TEXTURE)
uniform sampler2D depth;
#endif
uniform sampler2D stipple;

varying vec2 tex;
varying vec2 tex1;
varying vec2 tex2;
varying vec2 tex3;
varying vec2 tex4;
varying vec2 stex;

#if 0
float linearize(float op)
{
#if 0
    const float zNear = projection_range.x / projection_range.y;
    const float zFar = 1.0; // projection_range.y / projection_range.y
#else
    // Only ratio is important.
    const float zNear = 0.2 / 256.0;
    const float zFar = 1.0;
#endif
    return zNear * zFar / (zFar - op * (zFar - zNear));
}
#endif

float nonlinearize(float op)
{
#if 0
    const float zNear = projection_range.x / projection_range.y;
    const float zFar = 1.0; // projection_range.y / projection_range.y
#else
    // Only ratio is important.
    const float zNear = 0.2 / 256.0;
    const float zFar = 1.0;
#endif
    return (zFar - (zNear * zFar / op)) / (zFar - zNear);
}

float depth_decode(vec3 op)
{
    return (op.x * 65280.0 + op.y * 255.0 + op.z) / 65536.0;
}

vec4 color_decode(float op)
{
    const float inv_mul = 4.047619;
    if(op >= 0.7529412)
    {
        return vec4(1.0, 1.0, 1.0, (op - 0.7529412) * inv_mul);
    }
    if(op >= 0.5019608)
    {
        return vec4(0.8, 1.0, 0.6, (op - 0.5019608) * inv_mul);
    }
    if(op >= 0.2509804)
    {
        return vec4(0.5, 0.9, 1.0, (op - 0.2509804) * inv_mul);
    }
    return vec4(vec3(0.0), op * inv_mul);
}

void main()
{
#if defined(USE_LD)
    if(debug_mode != 0)
    {
        vec3 col = texture2D(color, tex).rgb;
        gl_FragColor = vec4(vec3(0.5) + col * 0.5, 1.0);
        return;
    }
#endif
    float ss = texture2D(stipple, stex).r + 0.001;

    vec4 raw = texture2D(color, tex);
    vec4 cc0 = color_decode(raw.a);
    float stipple_aa = sample_stipple(ss, cc0.w * cc0.w * cc0.w);

    float lindd = depth_decode(raw.rgb);
#if defined(VGL_DISABLE_DEPTH_TEXTURE)
    float dd0 = nonlinearize(lindd);
    float dd1 = abs(nonlinearize(depth_decode(texture2D(color, tex1).rgb)) - dd0);
    float dd2 = abs(nonlinearize(depth_decode(texture2D(color, tex2).rgb)) - dd0);
    float dd3 = abs(nonlinearize(depth_decode(texture2D(color, tex3).rgb)) - dd0);
    float dd4 = abs(nonlinearize(depth_decode(texture2D(color, tex4).rgb)) - dd0);
#else
    float dd0 = texture2D(depth, tex).r;
    float dd1 = abs(texture2D(depth, tex1).r - dd0);
    float dd2 = abs(texture2D(depth, tex2).r - dd0);
    float dd3 = abs(texture2D(depth, tex3).r - dd0);
    float dd4 = abs(texture2D(depth, tex4).r - dd0);
#endif
    float maxdd = max(max(max(dd1, dd2), dd3), dd4);
    float diffdd = smoothstep(0.0, 0.00033, maxdd);

    float inv_lindd = 1.0 - lindd;
    vec3 col = mix(stipple_aa * vec3(0.7, 0.5, 0.0) + vec3(0.11, 0.0, 0.0), vec3(0.21, 0.0, 0.0), 1.0 - inv_lindd * inv_lindd);

    if(cc0.g > 0.0)
    {
        vec3 stipple_cc = sample_stipple(ss, cc0.g * 0.7) * cc0.rgb;
        gl_FragColor = vec4(max(col, stipple_cc), 1.0);
    }
    else
    {
        float stipple_dd = sample_stipple(ss, diffdd);
        gl_FragColor = vec4(max(col, vec3(stipple_dd)), 1.0);
    }
    //gl_FragColor = vec4(lindd);
}
