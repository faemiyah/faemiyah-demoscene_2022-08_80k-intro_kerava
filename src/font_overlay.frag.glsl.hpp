#ifndef __g_shader_fragment_font_overlay_header__
#define __g_shader_fragment_font_overlay_header__
static const char *g_shader_fragment_font_overlay = ""
#if defined(USE_LD)
"font_overlay.frag.glsl"
#else
"uniform sampler2D P;"
"uniform sampler2D T;"
"varying vec2 t;"
"varying vec2 D;"
"void main()"
"{"
"float H=texture2D(P,t).s,f=texture2D(T,D).s+.001;"
"gl_FragColor=vec4(C(f,H*.6));"
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
static const char* g_shader_fragment_font_overlay_uniform_glyph DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph"
#else
"P"
#endif
"";
static const char* g_shader_fragment_font_overlay_uniform_stipple DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"stipple"
#else
"T"
#endif
"";
#endif
