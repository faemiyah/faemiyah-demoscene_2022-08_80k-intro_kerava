#ifndef __g_shader_fragment_post_header__
#define __g_shader_fragment_post_header__
static const char *g_shader_fragment_post = ""
#if defined(USE_LD)
"post.frag.glsl"
#else
"uniform sampler2D L;"
"uniform sampler2D T;"
"varying vec2 A;"
"varying vec2 E;"
"varying vec2 P;"
"varying vec2 Y;"
"varying vec2 Q;"
"varying vec2 D;"
"float f(float f)"
"{"
"float D=.000781,H=1;"
"return(H-D*H/f)/(H-D);"
"}"
"float t(vec3 H)"
"{"
"return(H.s*65280.+H.t*255.+H.p)/65536.;"
"}"
"vec4 X(float H)"
"{"
"float D=4.047619;"
"if(H>=.752941)return vec4(1,1,1,(H-.752941)*D);"
"if(H>=.501961)return vec4(.8,1,.6,(H-.501961)*D);"
"if(H>=.25098)return vec4(.5,.9,1,(H-.25098)*D);"
"return vec4(vec3(0),H*D);"
"}"
"void main()"
"{"
"float C=texture2D(T,D).s+.001;"
"vec4 I=texture2D(L,A),H=X(I.q);"
"float T=x(C,H.q*H.q*H.q),A=t(I.stp),D=f(A),X=abs(f(t(texture2D(L,E).stp))-D),E=abs(f(t(texture2D(L,P).stp))-D),P=abs(f(t(texture2D(L,Y).stp))-D),Y=abs(f(t(texture2D(L,Q).stp))-D),t=max(max(max(X,E),P),Y),Q=smoothstep(.0,.00033,t),f=1.-A;"
"vec3 L=mix(T*vec3(.7,.5,0)+vec3(.11,0,0),vec3(.21,0,0),1.-f*f);"
"if(H.t>.0)"
"{"
"vec3 D=x(C,H.t*.7)*H.stp;"
"gl_FragColor=vec4(max(L,D),1);"
"}"
"else"
"{"
"float H=x(C,Q);"
"gl_FragColor=vec4(max(L,vec3(H)),1);"
"}"
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
static const char* g_shader_fragment_post_uniform_color DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"color"
#else
"L"
#endif
"";
static const char* g_shader_fragment_post_uniform_stipple DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"stipple"
#else
"T"
#endif
"";
#endif
