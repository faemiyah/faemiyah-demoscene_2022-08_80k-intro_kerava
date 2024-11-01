#ifndef __g_shader_fragment_visualization_header__
#define __g_shader_fragment_visualization_header__
static const char *g_shader_fragment_visualization = ""
#if defined(USE_LD)
"visualization.frag.glsl"
#else
"uniform float g;"
"void main()"
"{"
"gl_FragColor=vec4(vec3(.0),i(g,.0));"
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
static const char* g_shader_fragment_visualization_uniform_color DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"color"
#else
"g"
#endif
"";
#endif
