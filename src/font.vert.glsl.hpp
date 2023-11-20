#ifndef __g_shader_vertex_font_header__
#define __g_shader_vertex_font_header__
static const char *g_shader_vertex_font = ""
#if defined(USE_LD)
"font.vert.glsl"
#else
"attribute vec3 H;"
"attribute vec3 L;"
"attribute vec2 A;"
"uniform mat4 E;"
"uniform mat4 Y;"
"uniform mat3 T;"
"uniform vec3 P;"
"uniform vec3 X;"
"uniform vec4 Q;"
"varying vec3 D;"
"varying vec3 f;"
"varying vec2 t;"
"void main()"
"{"
"D=T*L,t=A;"
"vec3 D=vec3(H.st*Q.pq+Q.st,H.p)+X;"
"f=P-(Y*vec4(D,1.)).stp,gl_Position=E*vec4(D,1.);"
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
"H"
#endif
"";
static const char* g_shader_vertex_font_attribute_normal DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal"
#else
"L"
#endif
"";
static const char* g_shader_vertex_font_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"texcoord"
#else
"A"
#endif
"";
static const char* g_shader_vertex_font_uniform_combined_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"combined_transform"
#else
"E"
#endif
"";
static const char* g_shader_vertex_font_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"modelview_transform"
#else
"Y"
#endif
"";
static const char* g_shader_vertex_font_uniform_normal_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"normal_transform"
#else
"T"
#endif
"";
static const char* g_shader_vertex_font_uniform_camera_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"camera_position"
#else
"P"
#endif
"";
static const char* g_shader_vertex_font_uniform_glyph_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_position"
#else
"X"
#endif
"";
static const char* g_shader_vertex_font_uniform_glyph_quad DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_quad"
#else
"Q"
#endif
"";
#endif
