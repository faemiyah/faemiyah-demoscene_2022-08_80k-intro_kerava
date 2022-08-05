uniform sampler2D glyph;
uniform sampler2D stipple;

varying vec2 tex;
varying vec2 stex;

void main()
{
    float gstr = texture2D(glyph, tex).r;
    float ss = texture2D(stipple, stex).r + 0.001;
    gl_FragColor = vec4(sample_stipple(ss, gstr * 0.6));
}
