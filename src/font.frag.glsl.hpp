#ifndef __g_shader_fragment_font_header__
#define __g_shader_fragment_font_header__
static const char *g_shader_fragment_font = ""
#if defined(USE_LD)
"font.frag.glsl"
#else
"uniform vec2 A;"
"uniform sampler2D L;"
"varying vec3 D;"
"varying vec3 f;"
"varying vec2 t;"
"void main()"
"{"
"vec3 H=normalize(D);"
"float D=texture2D(L,t).s;"
"if(D<=.3)discard;"
"vec3 t=normalize(f);"
"float f=A.t-A.s;"
"gl_FragColor=vec4(I(gl_FragCoord.p/gl_FragCoord.q/f),C(.8,1.-dot(t,H)));"
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
"A"
#endif
"";
static const char* g_shader_fragment_font_uniform_glyph DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph"
#else
"L"
#endif
"";
#endif
