#ifndef __g_shader_vertex_font_header__
#define __g_shader_vertex_font_header__
static const char *g_shader_vertex_font = ""
#if defined(USE_LD)
"font.vert.glsl"
#else
"attribute vec3 r;"
"attribute vec3 o;"
"attribute vec2 v;"
"uniform mat4 f;"
"uniform mat4 l;"
"uniform mat3 m;"
"uniform vec3 u;"
"uniform vec3 b;"
"uniform vec4 g;"
"varying vec3 e;"
"varying vec3 a;"
"varying vec2 t;"
"void main()"
"{"
"e=m*o,t=v;"
"vec3 e=vec3(r.rg*g.ba+g.rg,r.b)+b;"
"a=u-(l*vec4(e,1.)).rgb,gl_Position=f*vec4(e,1.);"
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
static const char* g_shader_vertex_font_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"position"
#else
"r"
#endif
"";
static const char* g_shader_vertex_font_attribute_normal DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal"
#else
"o"
#endif
"";
static const char* g_shader_vertex_font_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"texcoord"
#else
"v"
#endif
"";
static const char* g_shader_vertex_font_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"combined_transform"
#else
"f"
#endif
"";
static const char* g_shader_vertex_font_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"modelview_transform"
#else
"l"
#endif
"";
static const char* g_shader_vertex_font_uniform_normal_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal_transform"
#else
"m"
#endif
"";
static const char* g_shader_vertex_font_uniform_camera_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"camera_position"
#else
"u"
#endif
"";
static const char* g_shader_vertex_font_uniform_glyph_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_position"
#else
"b"
#endif
"";
static const char* g_shader_vertex_font_uniform_glyph_quad DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_quad"
#else
"g"
#endif
"";
#endif
