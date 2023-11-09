#ifndef __g_shader_vertex_post_header__
#define __g_shader_vertex_post_header__
static const char *g_shader_vertex_post = ""
#if defined(USE_LD)
"post.vert.glsl"
#else
"attribute vec3 H;"
"attribute vec2 A;"
"uniform vec4 f;"
"varying vec2 t;"
"varying vec2 Y;"
"varying vec2 E;"
"varying vec2 Q;"
"varying vec2 P;"
"varying vec2 D;"
"vec2 L(float D,float H)"
"{"
"return vec2(cos(H),sin(H))*D;"
"}"
"void main()"
"{"
"t=A;"
"float t=.8,x=.31,C=.7853982;"
"Y=A+L(t,x)*f.st,E=A+L(t,x+C)*f.st,Q=A+L(t,x+C*2.)*f.st,P=A+L(t,x+C*3.)*f.st,D=A*f.pq,gl_Position=vec4(H,1.);"
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
"A"
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
