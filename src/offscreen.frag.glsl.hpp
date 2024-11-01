#ifndef __g_shader_fragment_offscreen_header__
#define __g_shader_fragment_offscreen_header__
static const char *g_shader_fragment_offscreen = ""
#if defined(USE_LD)
"offscreen.frag.glsl"
#else
"uniform vec2 v;"
"varying vec3 e;"
"varying vec3 a;"
"void main()"
"{"
"vec3 r=normalize(e),e=normalize(a);"
"float a=v.g-v.r;"
"gl_FragColor=vec4(n(gl_FragCoord.b/gl_FragCoord.a/a),i(.0,1.-dot(e,r)));"
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
static const char* g_shader_fragment_offscreen_uniform_projection_range DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"projection_range"
#else
"v"
#endif
"";
#endif
