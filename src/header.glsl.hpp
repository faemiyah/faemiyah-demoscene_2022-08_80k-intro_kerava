#ifndef __g_shader_header_header__
#define __g_shader_header_header__
static const char *g_shader_header = ""
#if defined(USE_LD)
"header.glsl"
#else
"vec3 I(float t)"
"{"
"float H=t*65535.1,D=H-floor(H);"
"H=(H-D)/256.;"
"float f=H-floor(H),A=(H-f)/255.;"
"return vec3(A,vec2(f,D)*1.0039216);"
"}"
"float x(float D,float f)"
"{"
"float H=min(f,1.)*.247059;"
"if(D>=.75)return H+.75478;"
"if(D>=.5)return H+.503799;"
"if(D>=.25)return H+.252819;"
"return H;"
"}"
"float C(float D,float H)"
"{"
"if(H>=D)return 1;"
"float f=D-.2;"
"if(H>=f)return(H-f)/.2;"
"return 0;"
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
#endif
