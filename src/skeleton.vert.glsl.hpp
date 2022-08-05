#ifndef __g_shader_vertex_skeleton_header__
#define __g_shader_vertex_skeleton_header__
static const char *g_shader_vertex_skeleton = ""
#if defined(USE_LD)
"skeleton.vert.glsl"
#else
"attribute vec3 r;"
"attribute vec3 o;"
"attribute vec4 t;"
"attribute vec4 g;"
"uniform mat4 f;"
"uniform mat4 l;"
"uniform mat3 m;"
"uniform mat4 v[64];"
"uniform vec3 u;"
"varying vec3 e;"
"varying vec3 a;"
"void main()"
"{"
"ivec4 i=ivec4(g);"
"mat4 c=v[i.r]*t.r+v[i.g]*t.g+v[i.b]*t.b;"
"vec3 v=(c*vec4(r,1.)).rgb,r=mat3(c)*o;"
"e=m*r,a=u-(l*vec4(v,1.)).rgb,gl_Position=f*vec4(v,1.);"
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
static const char* g_shader_vertex_skeleton_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"position"
#else
"r"
#endif
"";
static const char* g_shader_vertex_skeleton_attribute_normal DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal"
#else
"o"
#endif
"";
static const char* g_shader_vertex_skeleton_attribute_bone_weight DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"bone_weight"
#else
"t"
#endif
"";
static const char* g_shader_vertex_skeleton_attribute_bone_ref DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"bone_ref"
#else
"g"
#endif
"";
static const char* g_shader_vertex_skeleton_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"combined_transform"
#else
"f"
#endif
"";
static const char* g_shader_vertex_skeleton_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"modelview_transform"
#else
"l"
#endif
"";
static const char* g_shader_vertex_skeleton_uniform_normal_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal_transform"
#else
"m"
#endif
"";
static const char* g_shader_vertex_skeleton_uniform_skeleton DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"skeleton"
#else
"v"
#endif
"";
static const char* g_shader_vertex_skeleton_uniform_camera_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"camera_position"
#else
"u"
#endif
"";
#endif
