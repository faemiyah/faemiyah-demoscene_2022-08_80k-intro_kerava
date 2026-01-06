#ifndef __g_shader_header_header__
#define __g_shader_header_header__
static const char *g_shader_header = ""
#if defined(DNLOAD_USE_LD)
"header.glsl"
#else
"vec3 n(float v)"
"{"
"float r=v*65535.1,e=r-floor(r);"
"r=(r-e)/256.;"
"float a=r-floor(r),t=(r-a)/255.;"
"return vec3(t,vec2(a,e)*1.0039216);"
"}"
"float i(float e,float a)"
"{"
"float r=min(a,1)*.247059;"
"if(e>=.75)return r+.75478;"
"if(e>=.5)return r+.503799;"
"if(e>=.25)return r+.252819;"
"return r;"
"}"
"float c(float e,float r)"
"{"
"if(r>=e)return 1;"
"float a=e-.2;"
"if(r>=a)return(r-a)/.2;"
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
