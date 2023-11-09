#ifndef __g_shader_vertex_font_overlay_header__
#define __g_shader_vertex_font_overlay_header__
static const char *g_shader_vertex_font_overlay = ""
#if defined(USE_LD)
"font_overlay.vert.glsl"
#else
"attribute vec3 H;"
"attribute vec2 A;"
"uniform mat4 Y;"
"uniform vec3 E;"
"uniform vec4 L;"
"uniform vec4 Q;"
"varying vec2 t;"
"varying vec2 D;"
"void main()"
"{"
"t=A;"
"vec3 f=vec3(H.st*L.pq+L.st,H.p)+E;"
"gl_Position=Y*vec4(f,1.),D=gl_Position.st*Q.pq*.5;"
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
static const char* g_shader_vertex_font_overlay_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"position"
#else
"H"
#endif
"";
static const char* g_shader_vertex_font_overlay_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"texcoord"
#else
"A"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"modelview_transform"
#else
"Y"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_glyph_position DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_position"
#else
"E"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_glyph_quad DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"glyph_quad"
#else
"L"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_scoords DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"scoords"
#else
"Q"
#endif
"";
#endif
