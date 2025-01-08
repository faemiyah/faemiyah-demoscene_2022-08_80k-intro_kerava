#ifndef __g_shader_vertex_font_overlay_header__
#define __g_shader_vertex_font_overlay_header__
static const char *g_shader_vertex_font_overlay = ""
#if defined(DNLOAD_USE_LD)
"font_overlay.vert.glsl"
#else
"attribute vec3 r;"
"attribute vec2 v;"
"uniform mat4 g;"
"uniform vec3 f;"
"uniform vec4 o;"
"uniform vec4 l;"
"varying vec2 t;"
"varying vec2 e;"
"void main()"
"{"
"t=v;"
"vec3 a=vec3(r.rg*o.ba+o.rg,r.b)+f;"
"gl_Position=g*vec4(a,1),e=gl_Position.rg*l.ba*.5;"
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
#if defined(DNLOAD_USE_LD)
"position"
#else
"r"
#endif
"";
static const char* g_shader_vertex_font_overlay_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"texcoord"
#else
"v"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_modelview_transform DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"modelview_transform"
#else
"g"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_glyph_position DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"glyph_position"
#else
"f"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_glyph_quad DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"glyph_quad"
#else
"o"
#endif
"";
static const char* g_shader_vertex_font_overlay_uniform_scoords DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"scoords"
#else
"l"
#endif
"";
#endif
