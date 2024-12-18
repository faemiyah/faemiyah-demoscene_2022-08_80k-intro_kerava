#ifndef __g_shader_fragment_font_header__
#define __g_shader_fragment_font_header__
static const char *g_shader_fragment_font = ""
#if defined(USE_LD)
"font.frag.glsl"
#else
"uniform vec2 L;"
"uniform sampler2D t;"
"varying vec3 D;"
"varying vec3 f;"
"varying vec2 A;"
"void main()"
"{"
"vec3 H=normalize(D);"
"float D=texture2D(t,A).s;"
"if(D<=.3)discard;"
"vec3 A=normalize(f);"
"float f=L.t-L.s;"
"gl_FragColor=vec4(I(gl_FragCoord.p/gl_FragCoord.q/f),C(.8,1.-dot(A,H)));"
"}"
#endif
"";
#if !defined(DNLOAD_RENAME_UNUSED)
#if defined(__GNUC__)
#define DNLOAD_RENAME_UNUSED __attribute__((unused))
#else
#define DNLOAD_RENAME_UNUSED
#endif
#endif
static const char* g_shader_fragment_font_uniform_projection_range DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"projection_range"
#else
"L"
#endif
"";
static const char* g_shader_fragment_font_uniform_glyph DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph"
#else
"t"
#endif
"";
#endif
