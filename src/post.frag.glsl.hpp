#ifndef __g_shader_fragment_post_header__
#define __g_shader_fragment_post_header__
static const char *g_shader_fragment_post = ""
#if defined(USE_LD)
"post.frag.glsl"
#else
"uniform sampler2D A;"
"uniform sampler2D P;"
"varying vec2 t;"
"varying vec2 Q;"
"varying vec2 E;"
"varying vec2 Y;"
"varying vec2 T;"
"varying vec2 D;"
"float f(float f)"
"{"
"float D=.000781,H=1;"
"return(H-D*H/f)/(H-D);"
"}"
"float L(vec3 H)"
"{"
"return(H.s*65280.+H.t*255.+H.p)/65536.;"
"}"
"vec4 X(float H)"
"{"
"float D=4.047619;"
"if(H>=.752941)return vec4(1.,1.,1.,(H-.752941)*D);"
"if(H>=.501961)return vec4(.8,1.,.6,(H-.501961)*D);"
"if(H>=.25098)return vec4(.5,.9,1.,(H-.25098)*D);"
"return vec4(vec3(0),H*D);"
"}"
"void main()"
"{"
"float C=texture2D(P,D).s+.001;"
"vec4 I=texture2D(A,t),H=X(I.q);"
"float P=x(C,H.q*H.q*H.q),t=L(I.stp),D=f(t),X=abs(f(L(texture2D(A,Q).stp))-D),Q=abs(f(L(texture2D(A,E).stp))-D),E=abs(f(L(texture2D(A,Y).stp))-D),Y=abs(f(L(texture2D(A,T).stp))-D),L=max(max(max(X,Q),E),Y),T=smoothstep(.0,.00033,L),f=1.-t;"
"vec3 A=mix(P*vec3(.7,.5,.0)+vec3(.11,.0,.0),vec3(.21,.0,.0),1.-f*f);"
"if(H.t>.0)"
"{"
"vec3 D=x(C,H.t*.7)*H.stp;"
"gl_FragColor=vec4(max(A,D),1.);"
"}"
"else"
"{"
"float H=x(C,T);"
"gl_FragColor=vec4(max(A,vec3(H)),1.);"
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
"A"
#endif
"";
static const char* g_shader_fragment_post_uniform_stipple DNLOAD_RENAME_UNUSED = ""
#if defined(USE_LD)
"stipple"
#else
"P"
#endif
"";
#endif
