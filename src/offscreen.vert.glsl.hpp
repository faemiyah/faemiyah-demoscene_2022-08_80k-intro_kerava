#ifndef __g_shader_vertex_offscreen_header__
#define __g_shader_vertex_offscreen_header__
static const char *g_shader_vertex_offscreen = ""
#if defined(USE_LD)
"offscreen.vert.glsl"
#else
"attribute vec3 H;"
"attribute vec3 L;"
"uniform mat4 Q;"
"uniform mat4 E;"
"uniform mat3 Y;"
"uniform vec3 T;"
"varying vec3 D;"
"varying vec3 f;"
"void main()"
"{"
"D=Y*L,f=T-(E*vec4(H,1.)).stp,gl_Position=Q*vec4(H,1.);"
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
#if defined(USE_LD)
"position"
#else
"H"
#endif
"";
static const char* g_shader_vertex_offscreen_attribute_normal DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal"
#else
"L"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"combined_transform"
#else
"Q"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"modelview_transform"
#else
"E"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_normal_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal_transform"
#else
"Y"
#endif
"";
static const char* g_shader_vertex_offscreen_uniform_camera_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"camera_position"
#else
"T"
#endif
"";
#endif
