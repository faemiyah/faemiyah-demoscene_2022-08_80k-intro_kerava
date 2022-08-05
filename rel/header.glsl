precision highp float;

#if defined(USE_LD)
uniform int debug_mode;
#endif

vec3 depth_encode(float op)
{
    float lin_z = op * 65535.99609375;
    float lo = lin_z - floor(lin_z);
    lin_z = (lin_z - lo) / 256.0;
    float mid = lin_z - floor(lin_z);
    float hi = (lin_z - mid) / 255.0;
    return vec3(hi, vec2(mid, lo) * 1.003921568627451);
}

float color_encode(float color, float angle)
{
    float ret = min(angle, 1.0) * 0.2470588;
    if(color >= 0.75)
    {
        // Should be 0.7529412
        return ret + 0.7547795;
    }
    if(color >= 0.5)
    {
        // Should be 0.5019608
        return ret + 0.5037990;
    }
    if(color >= 0.25)
    {
        // Should be 0.2509804
        return ret + 0.2528187;
    }
    return ret;
}

float sample_stipple(float ss, float cmp)
{
    if(cmp >= ss)
    {
        return 1.0;
    }
    float ss_border = ss - 0.2;
    if(cmp >= ss_border)
    {
        return (cmp - ss_border) / 0.2;
    }
    return 0.0;
}
