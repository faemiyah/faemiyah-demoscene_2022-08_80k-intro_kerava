#ifndef __g_shader_fragment_skeleton_header__
#define __g_shader_fragment_skeleton_header__
static const char *g_shader_fragment_skeleton = ""
#if defined(USE_LD)
"skeleton.frag.glsl"
#else
"uniform vec2 t;"
"varying vec3 D;"
"varying vec3 f;"
"void main()"
"{"
"vec3 H=normalize(D),D=normalize(f);"
"float f=t.t-t.s;"
"gl_FragColor=vec4(I(gl_FragCoord.p/gl_FragCoord.q/f),x(.0,1.-dot(D,H)));"
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
static const char* g_shader_fragment_skeleton_uniform_projection_range DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"projection_range"
#else
"t"
#endif
"";
#endif
