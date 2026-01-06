#ifndef __g_shader_fragment_skeleton_header__
#define __g_shader_fragment_skeleton_header__
static const char *g_shader_fragment_skeleton = ""
#if defined(DNLOAD_USE_LD)
"skeleton.frag.glsl"
#else
"uniform vec2 t;"
"varying vec3 e;"
"varying vec3 a;"
"void main()"
"{"
"vec3 r=normalize(e),e=normalize(a);"
"float a=t.g-t.r;"
"gl_FragColor=vec4(n(gl_FragCoord.b/gl_FragCoord.a/a),i(0,1.-dot(e,r)));"
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
#if defined(DNLOAD_USE_LD)
"projection_range"
#else
"t"
#endif
"";
#endif
