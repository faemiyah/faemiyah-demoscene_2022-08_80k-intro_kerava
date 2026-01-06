#ifndef __g_shader_vertex_post_header__
#define __g_shader_vertex_post_header__
static const char *g_shader_vertex_post = ""
#if defined(DNLOAD_USE_LD)
"post.vert.glsl"
#else
"attribute vec3 r;"
"attribute vec2 v;"
"uniform vec4 a;"
"varying vec2 t;"
"varying vec2 g;"
"varying vec2 f;"
"varying vec2 l;"
"varying vec2 m;"
"varying vec2 e;"
"vec2 o(float e,float r)"
"{"
"return vec2(cos(r),sin(r))*e;"
"}"
"void main()"
"{"
"t=v;"
"float t=.8,i=.31,c=.7853982;"
"g=v+o(t,i)*a.rg,f=v+o(t,i+c)*a.rg,l=v+o(t,i+c*2.)*a.rg,m=v+o(t,i+c*3.)*a.rg,e=v*a.ba,gl_Position=vec4(r,1);"
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
static const char* g_shader_vertex_post_attribute_position DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"position"
#else
"r"
#endif
"";
static const char* g_shader_vertex_post_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"texcoord"
#else
"v"
#endif
"";
static const char* g_shader_vertex_post_uniform_scoords DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"scoords"
#else
"a"
#endif
"";
#endif
