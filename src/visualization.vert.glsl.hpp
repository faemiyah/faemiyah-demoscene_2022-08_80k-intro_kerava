#ifndef __g_shader_vertex_visualization_header__
#define __g_shader_vertex_visualization_header__
static const char *g_shader_vertex_visualization = ""
#if defined(USE_LD)
"visualization.vert.glsl"
#else
"attribute vec3 H;"
"uniform mat4 t;"
"void main()"
"{"
"gl_Position=t*vec4(H,1);"
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
static const char* g_shader_vertex_visualization_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"position"
#else
"H"
#endif
"";
static const char* g_shader_vertex_visualization_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"combined_transform"
#else
"t"
#endif
"";
#endif
