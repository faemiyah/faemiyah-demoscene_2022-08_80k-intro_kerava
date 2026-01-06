#ifndef __g_shader_fragment_font_header__
#define __g_shader_fragment_font_header__
static const char *g_shader_fragment_font = ""
#if defined(DNLOAD_USE_LD)
"font.frag.glsl"
#else
"uniform vec2 v;"
"uniform sampler2D o;"
"varying vec3 e;"
"varying vec3 a;"
"varying vec2 t;"
"void main()"
"{"
"vec3 r=normalize(e);"
"float e=texture2D(o,t).r;"
"if(e<=.3)discard;"
"vec3 t=normalize(a);"
"float a=v.g-v.r;"
"gl_FragColor=vec4(n(gl_FragCoord.b/gl_FragCoord.a/a),i(.8,1.-dot(t,r)));"
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
#if defined(DNLOAD_USE_LD)
"projection_range"
#else
"v"
#endif
"";
static const char* g_shader_fragment_font_uniform_glyph DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"glyph"
#else
"o"
#endif
"";
#endif
