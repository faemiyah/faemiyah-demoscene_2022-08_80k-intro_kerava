#ifndef __g_shader_vertex_post_header__
#define __g_shader_vertex_post_header__
static const char *g_shader_vertex_post = ""
#if defined(USE_LD)
"post.vert.glsl"
#else
"attribute vec3 H;"
"attribute vec2 L;"
"uniform vec4 f;"
"varying vec2 A;"
"varying vec2 E;"
"varying vec2 P;"
"varying vec2 Y;"
"varying vec2 Q;"
"varying vec2 D;"
"vec2 t(float D,float H)"
"{"
"return vec2(cos(H),sin(H))*D;"
"}"
"void main()"
"{"
"A=L;"
"float A=.8,C=.31,x=.7853982;"
"E=L+t(A,C)*f.st,P=L+t(A,C+x)*f.st,Y=L+t(A,C+x*2.)*f.st,Q=L+t(A,C+x*3.)*f.st,D=L*f.pq,gl_Position=vec4(H,1);"
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
#if defined(USE_LD)
"position"
#else
"H"
#endif
"";
static const char* g_shader_vertex_post_attribute_texcoord DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"texcoord"
#else
"L"
#endif
"";
static const char* g_shader_vertex_post_uniform_scoords DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"scoords"
#else
"f"
#endif
"";
#endif
