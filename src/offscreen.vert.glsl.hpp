#ifndef __g_shader_vertex_offscreen_header__
#define __g_shader_vertex_offscreen_header__
static const char *g_shader_vertex_offscreen = ""
#if defined(DNLOAD_USE_LD)
"offscreen.vert.glsl"
#else
"attribute vec3 r;"
"attribute vec3 o;"
"uniform mat4 g;"
"uniform mat4 f;"
"uniform mat3 l;"
"uniform vec3 m;"
"varying vec3 e;"
"varying vec3 a;"
"void main()"
"{"
"e=l*o,a=m-(f*vec4(r,1)).rgb,gl_Position=g*vec4(r,1);"
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
static const char* g_shader_vertex_offscreen_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"position"
#else
"r"
#endif
"";
static const char* g_shader_vertex_offscreen_attribute_normal DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"normal"
#else
"o"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"combined_transform"
#else
"g"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"modelview_transform"
#else
"f"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_normal_transform DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"normal_transform"
#else
"l"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_camera_position DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"camera_position"
#else
"m"
#endif
"";
#endif
