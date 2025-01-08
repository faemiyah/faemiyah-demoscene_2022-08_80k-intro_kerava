#ifndef __g_shader_fragment_post_header__
#define __g_shader_fragment_post_header__
static const char *g_shader_fragment_post = ""
#if defined(DNLOAD_USE_LD)
"post.frag.glsl"
#else
"uniform sampler2D v;"
"uniform sampler2D u;"
"varying vec2 t;"
"varying vec2 g;"
"varying vec2 f;"
"varying vec2 l;"
"varying vec2 m;"
"varying vec2 e;"
"float a(float a)"
"{"
"float e=.000781,r=1;"
"return(r-e*r/a)/(r-e);"
"}"
"float o(vec3 r)"
"{"
"return(r.r*65280.+r.g*255.+r.b)/65536.;"
"}"
"vec4 b(float r)"
"{"
"float e=4.047619;"
"if(r>=.752941)return vec4(1,1,1,(r-.752941)*e);"
"if(r>=.501961)return vec4(.8,1,.6,(r-.501961)*e);"
"if(r>=.25098)return vec4(.5,.9,1,(r-.25098)*e);"
"return vec4(vec3(0),r*e);"
"}"
"void main()"
"{"
"float i=texture2D(u,e).r+.001;"
"vec4 n=texture2D(v,t),r=b(n.a);"
"float u=c(i,r.a*r.a*r.a),t=o(n.rgb),e=a(t),b=abs(a(o(texture2D(v,g).rgb))-e),g=abs(a(o(texture2D(v,f).rgb))-e),f=abs(a(o(texture2D(v,l).rgb))-e),l=abs(a(o(texture2D(v,m).rgb))-e),o=max(max(max(b,g),f),l),m=smoothstep(.0,.00033,o),a=1.-t;"
"vec3 v=mix(u*vec3(.7,.5,0)+vec3(.11,0,0),vec3(.21,0,0),1.-a*a);"
"if(r.g>.0)"
"{"
"vec3 e=c(i,r.g*.7)*r.rgb;"
"gl_FragColor=vec4(max(v,e),1);"
"}"
"else"
"{"
"float r=c(i,m);"
"gl_FragColor=vec4(max(v,vec3(r)),1);"
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
#if defined(DNLOAD_USE_LD)
"color"
#else
"v"
#endif
"";
static const char* g_shader_fragment_post_uniform_stipple DNLOAD_RENAME_UNUSED = ""
#if defined(DNLOAD_USE_LD)
"stipple"
#else
"u"
#endif
"";
#endif
