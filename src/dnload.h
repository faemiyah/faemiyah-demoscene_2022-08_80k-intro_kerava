#ifndef DNLOAD_H
#define DNLOAD_H

/// \file
/// \brief Dynamic loader header stub.
///
/// This file was automatically generated by 'dnload.py'.

#if defined(WIN32)
/// \cond
#define _USE_MATH_DEFINES
#define NOMINMAX
/// \endcond
#else
/// \cond
#define GL_GLEXT_PROTOTYPES
/// \endcond
#endif

#if defined(__cplusplus)
#include <cstdint>
#else
#include <stdint.h>
#endif

#if defined(__cplusplus)
#include <cstdio>
#include <cstdlib>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

#include "fftw3.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#if defined(__cplusplus)
#include <cmath>
#else
#include <math.h>
#endif

#if defined(DNLOAD_VIDEOCORE)
#include "bcm_host.h"
#include "EGL/egl.h"
#endif

#if defined(USE_LD)
#if defined(WIN32)
#include "windows.h"
#include "GL/glew.h"
#include "GL/glu.h"
#elif defined(__APPLE__)
#include "GL/glew.h"
#include <OpenGL/glu.h>
#else
#if defined(DNLOAD_GLESV2)
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#else
#include "GL/glew.h"
#include "GL/glu.h"
#endif
#endif
#else
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#else
#if defined(DNLOAD_GLESV2)
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#else
#include "GL/gl.h"
#include "GL/glext.h"
#include "GL/glu.h"
#endif
#endif
#endif

#include "opus.h"

#if defined(USE_LD)
#include "gnu_rand.h"
#else
#define DNLOAD_RAND_IMPLEMENTATION_BSD 0
#define DNLOAD_RAND_IMPLEMENTATION_GNU 1
#if defined(__FreeBSD__)
#if !DNLOAD_RAND_IMPLEMENTATION_BSD
#include "gnu_rand.h"
#include "gnu_rand.c"
#else
#define bsd_rand rand
#define bsd_srand srand
#endif
#elif defined(__linux__)
#if !DNLOAD_RAND_IMPLEMENTATION_GNU
#include "gnu_rand.h"
#include "gnu_rand.c"
#else
#define gnu_rand rand
#define gnu_srand srand
#endif
#endif
#endif

#include "SDL.h"
#if defined(SDL_INIT_EVERYTHING) && defined(__APPLE__)
#define DNLOAD_MAIN SDL_main
#else
#define DNLOAD_MAIN main
#endif

/// Macro stringification helper (adds indirection).
#define DNLOAD_MACRO_STR_HELPER(op) #op
/// Macro stringification.
#define DNLOAD_MACRO_STR(op) DNLOAD_MACRO_STR_HELPER(op)

#if defined(DNLOAD_GLESV2)
/// Apientry definition (OpenGL ES 2.0).
#define DNLOAD_APIENTRY GL_APIENTRY
#else
/// Apientry definition (OpenGL).
#define DNLOAD_APIENTRY GLAPIENTRY
#endif

#if (defined(_LP64) && _LP64) || (defined(__LP64__) && __LP64__)
/// Size of pointer in bytes (64-bit).
#define DNLOAD_POINTER_SIZE 8
#else
/// Size of pointer in bytes (32-bit).
#define DNLOAD_POINTER_SIZE 4
#endif

#if !defined(USE_LD)
/// Error string for when assembler exit procedure is not available.
#define DNLOAD_ASM_EXIT_ERROR "no assembler exit procedure defined for current operating system or architecture"
/// Perform exit syscall in assembler.
static void asm_exit(void)
{
#if defined(DNLOAD_NO_DEBUGGER_TRAP)
#if defined(__x86_64__)
#if defined(__FreeBSD__)
    asm("syscall" : /* no output */ : "a"(1) : /* no clobber */);
#elif defined(__linux__)
    asm("syscall" : /* no output */ : "a"(60) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__i386__)
#if defined(__FreeBSD__) || defined(__linux__)
    asm("int $0x80" : /* no output */ : "a"(1) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__aarch64__)
#if defined(__linux__)
    register int x8 asm("x8") = 93;
    asm("svc #0" : /* no output */ : "r"(x8) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__arm__)
#if defined(__linux__)
    register int r7 asm("r7") = 1;
    asm("swi #0" : /* no output */ : "r"(r7) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#else
#if defined(__x86_64__) || defined(__i386__)
    asm("int $0x3" : /* no output */ : /* no input */ : /* no clobber */);
#elif defined(__aarch64__)
    asm("brk #1000" : /* no output */ : /* no input */ : /* no clobber */);
#elif defined(__arm__)
    asm(".inst 0xdeff" : /* no output */ : /* no input */ : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#endif
    __builtin_unreachable();
}
#endif

#if defined(USE_LD)
/// \cond
#define dnload_glGenerateMipmap glGenerateMipmap
#define dnload_SDL_CondSignal SDL_CondSignal
#define dnload_glLinkProgram glLinkProgram
#define dnload_glBindBuffer glBindBuffer
#define dnload_glBufferSubData glBufferSubData
#define dnload_SDL_CondWait SDL_CondWait
#define dnload_fmodf fmodf
#define dnload_glFramebufferTexture2D glFramebufferTexture2D
#define dnload_qsort qsort
#define dnload_sinf sinf
#define dnload_SDL_GL_SetAttribute SDL_GL_SetAttribute
#define dnload_fftw_execute fftw_execute
#define dnload_glClear glClear
#define dnload_glUniform1fv glUniform1fv
#define dnload_glUniform1iv glUniform1iv
#define dnload_glUniform2fv glUniform2fv
#define dnload_glUniform3fv glUniform3fv
#define dnload_glUniform4fv glUniform4fv
#define dnload_glGetUniformLocation glGetUniformLocation
#define dnload_SDL_GL_SwapWindow SDL_GL_SwapWindow
#define dnload_SDL_PauseAudio SDL_PauseAudio
#define dnload_SDL_Delay SDL_Delay
#define dnload_FT_Set_Pixel_Sizes FT_Set_Pixel_Sizes
#define dnload_FT_New_Face FT_New_Face
#define dnload_glAttachShader glAttachShader
#define dnload_glClearDepthf glClearDepthf
#define dnload_cosf cosf
#define dnload_SDL_UnlockMutex SDL_UnlockMutex
#define dnload_glGenBuffers glGenBuffers
#define dnload_sqrtf sqrtf
#define dnload_tanf tanf
#define dnload_SDL_GetThreadID SDL_GetThreadID
#define dnload_SDL_OpenAudio SDL_OpenAudio
#define dnload_SDL_CreateWindow SDL_CreateWindow
#define dnload_glUniformMatrix2fv glUniformMatrix2fv
#define dnload_glUniformMatrix3fv glUniformMatrix3fv
#define dnload_glUniformMatrix4fv glUniformMatrix4fv
#define dnload_glBindRenderbuffer glBindRenderbuffer
#define dnload_SDL_CondBroadcast SDL_CondBroadcast
#define dnload_fftw_destroy_plan fftw_destroy_plan
#define dnload_SDL_WaitThread SDL_WaitThread
#define dnload_glDrawElements glDrawElements
#define dnload_glDisableVertexAttribArray glDisableVertexAttribArray
#define dnload_SDL_PollEvent SDL_PollEvent
#define dnload_glDeleteShader glDeleteShader
#define dnload_glDepthMask glDepthMask
#define dnload_glCreateShader glCreateShader
#define dnload_srand gnu_srand
#define dnload_SDL_DestroyMutex SDL_DestroyMutex
#define dnload_SDL_Init SDL_Init
#define dnload_log2f log2f
#define dnload_glColorMask glColorMask
#define dnload_SDL_LockMutex SDL_LockMutex
#define dnload_FT_Init_FreeType FT_Init_FreeType
#define dnload_glCreateProgram glCreateProgram
#define dnload_glGenRenderbuffers glGenRenderbuffers
#define dnload_SDL_Quit SDL_Quit
#define dnload_SDL_ThreadID SDL_ThreadID
#define dnload_SDL_CreateThread SDL_CreateThread
#define dnload_FT_Render_Glyph FT_Render_Glyph
#define dnload_glClearColor glClearColor
#define dnload_SDL_CreateCond SDL_CreateCond
#define dnload_powf powf
#define dnload_glBindTexture glBindTexture
#define dnload_expf expf
#define dnload_glBufferData glBufferData
#define dnload_glGenTextures glGenTextures
#define dnload_SDL_DestroyCond SDL_DestroyCond
#define dnload_opus_decode_float opus_decode_float
#define dnload_glBindFramebuffer glBindFramebuffer
#define dnload_glTexImage2D glTexImage2D
#define dnload_FT_Get_Char_Index FT_Get_Char_Index
#define dnload_glDeleteBuffers glDeleteBuffers
#define dnload_glGenFramebuffers glGenFramebuffers
#define dnload_realloc realloc
#define dnload_glDisable glDisable
#define dnload_glBlendFuncSeparate glBlendFuncSeparate
#define dnload_fftw_plan_r2r_1d fftw_plan_r2r_1d
#define dnload_SDL_ShowCursor SDL_ShowCursor
#define dnload_glDeleteProgram glDeleteProgram
#define dnload_free free
#define dnload_glVertexAttribPointer glVertexAttribPointer
#define dnload_glCompileShader glCompileShader
#define dnload_glShaderSource glShaderSource
#define dnload_glDepthFunc glDepthFunc
#define dnload_glRenderbufferStorage glRenderbufferStorage
#define dnload_SDL_CreateMutex SDL_CreateMutex
#define dnload_glUseProgram glUseProgram
#define dnload_roundf roundf
#define dnload_glGetAttribLocation glGetAttribLocation
#define dnload_SDL_GetTicks SDL_GetTicks
#define dnload_exp2f exp2f
#define dnload_glPolygonOffset glPolygonOffset
#define dnload_glActiveTexture glActiveTexture
#define dnload_logf logf
#define dnload_FT_Load_Glyph FT_Load_Glyph
#define dnload_SDL_GL_CreateContext SDL_GL_CreateContext
#define dnload_glTexParameterf glTexParameterf
#define dnload_glTexParameteri glTexParameteri
#define dnload_floorf floorf
#define dnload_glCullFace glCullFace
#define dnload_opus_decoder_create opus_decoder_create
#define dnload_lrintf lrintf
#define dnload_rand gnu_rand
#define dnload_glEnableVertexAttribArray glEnableVertexAttribArray
#define dnload_glFramebufferRenderbuffer glFramebufferRenderbuffer
#define dnload_glViewport glViewport
#define dnload_glEnable glEnable
/// \endcond
#else
/// \cond
#define dnload_glGenerateMipmap g_symbol_table.df_glGenerateMipmap
#define dnload_SDL_CondSignal g_symbol_table.df_SDL_CondSignal
#define dnload_glLinkProgram g_symbol_table.df_glLinkProgram
#define dnload_glBindBuffer g_symbol_table.df_glBindBuffer
#define dnload_glBufferSubData g_symbol_table.df_glBufferSubData
#define dnload_SDL_CondWait g_symbol_table.df_SDL_CondWait
#define dnload_fmodf g_symbol_table.df_fmodf
#define dnload_glFramebufferTexture2D g_symbol_table.df_glFramebufferTexture2D
#define dnload_qsort g_symbol_table.df_qsort
#define dnload_sinf g_symbol_table.df_sinf
#define dnload_SDL_GL_SetAttribute g_symbol_table.df_SDL_GL_SetAttribute
#define dnload_fftw_execute g_symbol_table.df_fftw_execute
#define dnload_glClear g_symbol_table.df_glClear
#define dnload_glUniform1fv g_symbol_table.df_glUniform1fv
#define dnload_glUniform1iv g_symbol_table.df_glUniform1iv
#define dnload_glUniform2fv g_symbol_table.df_glUniform2fv
#define dnload_glUniform3fv g_symbol_table.df_glUniform3fv
#define dnload_glUniform4fv g_symbol_table.df_glUniform4fv
#define dnload_glGetUniformLocation g_symbol_table.df_glGetUniformLocation
#define dnload_SDL_GL_SwapWindow g_symbol_table.df_SDL_GL_SwapWindow
#define dnload_SDL_PauseAudio g_symbol_table.df_SDL_PauseAudio
#define dnload_SDL_Delay g_symbol_table.df_SDL_Delay
#define dnload_FT_Set_Pixel_Sizes g_symbol_table.df_FT_Set_Pixel_Sizes
#define dnload_FT_New_Face g_symbol_table.df_FT_New_Face
#define dnload_glAttachShader g_symbol_table.df_glAttachShader
#define dnload_glClearDepthf g_symbol_table.df_glClearDepthf
#define dnload_cosf g_symbol_table.df_cosf
#define dnload_SDL_UnlockMutex g_symbol_table.df_SDL_UnlockMutex
#define dnload_glGenBuffers g_symbol_table.df_glGenBuffers
#define dnload_sqrtf sqrtf
#define dnload_tanf g_symbol_table.df_tanf
#define dnload_SDL_GetThreadID g_symbol_table.df_SDL_GetThreadID
#define dnload_SDL_OpenAudio g_symbol_table.df_SDL_OpenAudio
#define dnload_SDL_CreateWindow g_symbol_table.df_SDL_CreateWindow
#define dnload_glUniformMatrix2fv g_symbol_table.df_glUniformMatrix2fv
#define dnload_glUniformMatrix3fv g_symbol_table.df_glUniformMatrix3fv
#define dnload_glUniformMatrix4fv g_symbol_table.df_glUniformMatrix4fv
#define dnload_glBindRenderbuffer g_symbol_table.df_glBindRenderbuffer
#define dnload_SDL_CondBroadcast g_symbol_table.df_SDL_CondBroadcast
#define dnload_fftw_destroy_plan g_symbol_table.df_fftw_destroy_plan
#define dnload_SDL_WaitThread g_symbol_table.df_SDL_WaitThread
#define dnload_glDrawElements g_symbol_table.df_glDrawElements
#define dnload_glDisableVertexAttribArray g_symbol_table.df_glDisableVertexAttribArray
#define dnload_SDL_PollEvent g_symbol_table.df_SDL_PollEvent
#define dnload_glDeleteShader g_symbol_table.df_glDeleteShader
#define dnload_glDepthMask g_symbol_table.df_glDepthMask
#define dnload_glCreateShader g_symbol_table.df_glCreateShader
#define dnload_srand g_symbol_table.df_srand
#define dnload_SDL_DestroyMutex g_symbol_table.df_SDL_DestroyMutex
#define dnload_SDL_Init g_symbol_table.df_SDL_Init
#define dnload_log2f g_symbol_table.df_log2f
#define dnload_glColorMask g_symbol_table.df_glColorMask
#define dnload_SDL_LockMutex g_symbol_table.df_SDL_LockMutex
#define dnload_FT_Init_FreeType g_symbol_table.df_FT_Init_FreeType
#define dnload_glCreateProgram g_symbol_table.df_glCreateProgram
#define dnload_glGenRenderbuffers g_symbol_table.df_glGenRenderbuffers
#define dnload_SDL_Quit g_symbol_table.df_SDL_Quit
#define dnload_SDL_ThreadID g_symbol_table.df_SDL_ThreadID
#define dnload_SDL_CreateThread g_symbol_table.df_SDL_CreateThread
#define dnload_FT_Render_Glyph g_symbol_table.df_FT_Render_Glyph
#define dnload_glClearColor g_symbol_table.df_glClearColor
#define dnload_SDL_CreateCond g_symbol_table.df_SDL_CreateCond
#define dnload_powf g_symbol_table.df_powf
#define dnload_glBindTexture g_symbol_table.df_glBindTexture
#define dnload_expf g_symbol_table.df_expf
#define dnload_glBufferData g_symbol_table.df_glBufferData
#define dnload_glGenTextures g_symbol_table.df_glGenTextures
#define dnload_SDL_DestroyCond g_symbol_table.df_SDL_DestroyCond
#define dnload_opus_decode_float g_symbol_table.df_opus_decode_float
#define dnload_glBindFramebuffer g_symbol_table.df_glBindFramebuffer
#define dnload_glTexImage2D g_symbol_table.df_glTexImage2D
#define dnload_FT_Get_Char_Index g_symbol_table.df_FT_Get_Char_Index
#define dnload_glDeleteBuffers g_symbol_table.df_glDeleteBuffers
#define dnload_glGenFramebuffers g_symbol_table.df_glGenFramebuffers
#define dnload_realloc g_symbol_table.df_realloc
#define dnload_glDisable g_symbol_table.df_glDisable
#define dnload_glBlendFuncSeparate g_symbol_table.df_glBlendFuncSeparate
#define dnload_fftw_plan_r2r_1d g_symbol_table.df_fftw_plan_r2r_1d
#define dnload_SDL_ShowCursor g_symbol_table.df_SDL_ShowCursor
#define dnload_glDeleteProgram g_symbol_table.df_glDeleteProgram
#define dnload_free g_symbol_table.df_free
#define dnload_glVertexAttribPointer g_symbol_table.df_glVertexAttribPointer
#define dnload_glCompileShader g_symbol_table.df_glCompileShader
#define dnload_glShaderSource g_symbol_table.df_glShaderSource
#define dnload_glDepthFunc g_symbol_table.df_glDepthFunc
#define dnload_glRenderbufferStorage g_symbol_table.df_glRenderbufferStorage
#define dnload_SDL_CreateMutex g_symbol_table.df_SDL_CreateMutex
#define dnload_glUseProgram g_symbol_table.df_glUseProgram
#define dnload_roundf g_symbol_table.df_roundf
#define dnload_glGetAttribLocation g_symbol_table.df_glGetAttribLocation
#define dnload_SDL_GetTicks g_symbol_table.df_SDL_GetTicks
#define dnload_exp2f g_symbol_table.df_exp2f
#define dnload_glPolygonOffset g_symbol_table.df_glPolygonOffset
#define dnload_glActiveTexture g_symbol_table.df_glActiveTexture
#define dnload_logf g_symbol_table.df_logf
#define dnload_FT_Load_Glyph g_symbol_table.df_FT_Load_Glyph
#define dnload_SDL_GL_CreateContext g_symbol_table.df_SDL_GL_CreateContext
#define dnload_glTexParameterf g_symbol_table.df_glTexParameterf
#define dnload_glTexParameteri g_symbol_table.df_glTexParameteri
#define dnload_floorf g_symbol_table.df_floorf
#define dnload_glCullFace g_symbol_table.df_glCullFace
#define dnload_opus_decoder_create g_symbol_table.df_opus_decoder_create
#define dnload_lrintf g_symbol_table.df_lrintf
#define dnload_rand g_symbol_table.df_rand
#define dnload_glEnableVertexAttribArray g_symbol_table.df_glEnableVertexAttribArray
#define dnload_glFramebufferRenderbuffer g_symbol_table.df_glFramebufferRenderbuffer
#define dnload_glViewport g_symbol_table.df_glViewport
#define dnload_glEnable g_symbol_table.df_glEnable
/// \endcond
/// Symbol table structure.
///
/// Contains all the symbols required for dynamic linking.
static struct SymbolTableStruct
{
    void (DNLOAD_APIENTRY *df_glGenerateMipmap)(GLenum);
    int (*df_SDL_CondSignal)(SDL_cond*);
    void (DNLOAD_APIENTRY *df_glLinkProgram)(GLuint);
    void (DNLOAD_APIENTRY *df_glBindBuffer)(GLenum, GLuint);
    void (DNLOAD_APIENTRY *df_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const void*);
    int (*df_SDL_CondWait)(SDL_cond*, SDL_mutex*);
    float (*df_fmodf)(float, float);
    void (DNLOAD_APIENTRY *df_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);
    void (*df_qsort)(void*, size_t, size_t, int (*)(const void*, const void*));
    float (*df_sinf)(float);
    int (*df_SDL_GL_SetAttribute)(SDL_GLattr, int);
    void (*df_fftw_execute)(const fftw_plan);
    void (DNLOAD_APIENTRY *df_glClear)(GLbitfield);
    void (DNLOAD_APIENTRY *df_glUniform1fv)(GLint, GLsizei, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glUniform1iv)(GLint, GLsizei, const GLint*);
    void (DNLOAD_APIENTRY *df_glUniform2fv)(GLint, GLsizei, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glUniform3fv)(GLint, GLsizei, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glUniform4fv)(GLint, GLsizei, const GLfloat*);
    GLint (DNLOAD_APIENTRY *df_glGetUniformLocation)(GLuint, const GLchar*);
    void (*df_SDL_GL_SwapWindow)(SDL_Window*);
    void (*df_SDL_PauseAudio)(int);
    void (*df_SDL_Delay)(Uint32);
    FT_Error (*df_FT_Set_Pixel_Sizes)(FT_Face, FT_UInt, FT_UInt);
    FT_Error (*df_FT_New_Face)(FT_Library, const char*, FT_Long, FT_Face*);
    void (DNLOAD_APIENTRY *df_glAttachShader)(GLuint, GLuint);
    void (DNLOAD_APIENTRY *df_glClearDepthf)(GLfloat);
    float (*df_cosf)(float);
    int (*df_SDL_UnlockMutex)(SDL_mutex*);
    void (DNLOAD_APIENTRY *df_glGenBuffers)(GLsizei, GLuint*);
    float (*df_tanf)(float);
    SDL_threadID (*df_SDL_GetThreadID)(SDL_Thread*);
    int (*df_SDL_OpenAudio)(SDL_AudioSpec*, SDL_AudioSpec*);
    SDL_Window* (*df_SDL_CreateWindow)(const char*, int, int, int, int, Uint32);
    void (DNLOAD_APIENTRY *df_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
    void (DNLOAD_APIENTRY *df_glBindRenderbuffer)(GLenum, GLuint);
    int (*df_SDL_CondBroadcast)(SDL_cond*);
    void (*df_fftw_destroy_plan)(fftw_plan);
    void (*df_SDL_WaitThread)(SDL_Thread*, int*);
    void (DNLOAD_APIENTRY *df_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid*);
    void (DNLOAD_APIENTRY *df_glDisableVertexAttribArray)(GLuint);
    int (*df_SDL_PollEvent)(SDL_Event*);
    void (DNLOAD_APIENTRY *df_glDeleteShader)(GLuint);
    void (DNLOAD_APIENTRY *df_glDepthMask)(GLboolean);
    GLuint (DNLOAD_APIENTRY *df_glCreateShader)(GLenum);
    void (*df_srand)(unsigned int);
    void (*df_SDL_DestroyMutex)(SDL_mutex*);
    int (*df_SDL_Init)(Uint32);
    float (*df_log2f)(float);
    void (DNLOAD_APIENTRY *df_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean);
    int (*df_SDL_LockMutex)(SDL_mutex*);
    FT_Error (*df_FT_Init_FreeType)(FT_Library*);
    GLuint (DNLOAD_APIENTRY *df_glCreateProgram)(void);
    void (DNLOAD_APIENTRY *df_glGenRenderbuffers)(GLsizei, GLuint*);
    void (*df_SDL_Quit)(void);
    SDL_threadID (*df_SDL_ThreadID)(void);
    SDL_Thread* (*df_SDL_CreateThread)(int (*)(void*), const char*, void*);
    FT_Error (*df_FT_Render_Glyph)(FT_GlyphSlot, FT_Render_Mode);
    void (DNLOAD_APIENTRY *df_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
    SDL_cond* (*df_SDL_CreateCond)(void);
    float (*df_powf)(float, float);
    void (DNLOAD_APIENTRY *df_glBindTexture)(GLenum, GLuint);
    float (*df_expf)(float);
    void (DNLOAD_APIENTRY *df_glBufferData)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
    void (DNLOAD_APIENTRY *df_glGenTextures)(GLsizei, GLuint*);
    void (*df_SDL_DestroyCond)(SDL_cond*);
    int (*df_opus_decode_float)(OpusDecoder*, const unsigned char*, opus_int32, float*, int, int);
    void (DNLOAD_APIENTRY *df_glBindFramebuffer)(GLenum, GLuint);
    void (DNLOAD_APIENTRY *df_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    FT_UInt (*df_FT_Get_Char_Index)(FT_Face, FT_ULong);
    void (DNLOAD_APIENTRY *df_glDeleteBuffers)(GLsizei, const GLuint*);
    void (DNLOAD_APIENTRY *df_glGenFramebuffers)(GLsizei, GLuint*);
    void* (*df_realloc)(void*, size_t);
    void (DNLOAD_APIENTRY *df_glDisable)(GLenum);
    void (DNLOAD_APIENTRY *df_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum);
    fftw_plan (*df_fftw_plan_r2r_1d)(int, double*, double*, fftw_r2r_kind, unsigned);
    int (*df_SDL_ShowCursor)(int);
    void (DNLOAD_APIENTRY *df_glDeleteProgram)(GLuint);
    void (*df_free)(void*);
    void (DNLOAD_APIENTRY *df_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
    void (DNLOAD_APIENTRY *df_glCompileShader)(GLuint);
    void (DNLOAD_APIENTRY *df_glShaderSource)(GLuint, GLsizei, const GLchar**, const GLint*);
    void (DNLOAD_APIENTRY *df_glDepthFunc)(GLenum);
    void (DNLOAD_APIENTRY *df_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei);
    SDL_mutex* (*df_SDL_CreateMutex)(void);
    void (DNLOAD_APIENTRY *df_glUseProgram)(GLuint);
    float (*df_roundf)(float);
    GLint (DNLOAD_APIENTRY *df_glGetAttribLocation)(GLuint, const GLchar*);
    uint32_t (*df_SDL_GetTicks)(void);
    float (*df_exp2f)(float);
    void (DNLOAD_APIENTRY *df_glPolygonOffset)(GLfloat, GLfloat);
    void (DNLOAD_APIENTRY *df_glActiveTexture)(GLenum);
    float (*df_logf)(float);
    FT_Error (*df_FT_Load_Glyph)(FT_Face, FT_UInt, FT_Int32);
    SDL_GLContext (*df_SDL_GL_CreateContext)(SDL_Window*);
    void (DNLOAD_APIENTRY *df_glTexParameterf)(GLenum, GLenum, GLfloat);
    void (DNLOAD_APIENTRY *df_glTexParameteri)(GLenum, GLenum, GLint);
    float (*df_floorf)(float);
    void (DNLOAD_APIENTRY *df_glCullFace)(GLenum);
    OpusDecoder* (*df_opus_decoder_create)(opus_int32, int, int*);
    long (*df_lrintf)(float);
    int (*df_rand)(void);
    void (DNLOAD_APIENTRY *df_glEnableVertexAttribArray)(GLuint);
    void (DNLOAD_APIENTRY *df_glFramebufferRenderbuffer)(GLenum, GLenum, GLint, GLuint);
    void (DNLOAD_APIENTRY *df_glViewport)(GLint, GLint, GLsizei, GLsizei);
    void (DNLOAD_APIENTRY *df_glEnable)(GLenum);
} g_symbol_table =
{
    (void (DNLOAD_APIENTRY *)(GLenum))0x11741122,
    (int (*)(SDL_cond*))0x11f4f60e,
    (void (DNLOAD_APIENTRY *)(GLuint))0x133a35c5,
    (void (DNLOAD_APIENTRY *)(GLenum, GLuint))0x15aa3ae2,
    (void (DNLOAD_APIENTRY *)(GLenum, GLintptr, GLsizeiptr, const void*))0x164ac985,
    (int (*)(SDL_cond*, SDL_mutex*))0x167a16bb,
    (float (*)(float, float))0x16a18daa,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLenum, GLuint, GLint))0x18781f65,
    (void (*)(void*, size_t, size_t, int (*)(const void*, const void*)))0x19008aaf,
    (float (*)(float))0x1ab23d2e,
    (int (*)(SDL_GLattr, int))0x1da21ab0,
    (void (*)(const fftw_plan))0x1e9bcf9,
    (void (DNLOAD_APIENTRY *)(GLbitfield))0x1fd92088,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, const GLfloat*))0x21383ab2,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, const GLint*))0x213b3b6f,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, const GLfloat*))0x21b64a33,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, const GLfloat*))0x223459b4,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, const GLfloat*))0x22b26935,
    (GLint (DNLOAD_APIENTRY *)(GLuint, const GLchar*))0x25c12218,
    (void (*)(SDL_Window*))0x295bfb59,
    (void (*)(int))0x29f14a4,
    (void (*)(Uint32))0x2ccbf01f,
    (FT_Error (*)(FT_Face, FT_UInt, FT_UInt))0x2e0eeab,
    (FT_Error (*)(FT_Library, const char*, FT_Long, FT_Face*))0x2f7e33ed,
    (void (DNLOAD_APIENTRY *)(GLuint, GLuint))0x30b3cfcf,
    (void (DNLOAD_APIENTRY *)(GLfloat))0x338598ab,
    (float (*)(float))0x353e8f7f,
    (int (*)(SDL_mutex*))0x3a574477,
    (void (DNLOAD_APIENTRY *)(GLsizei, GLuint*))0x3dce2328,
    (float (*)(float))0x454891e5,
    (SDL_threadID (*)(SDL_Thread*))0x45904f57,
    (int (*)(SDL_AudioSpec*, SDL_AudioSpec*))0x46fd70c8,
    (SDL_Window* (*)(const char*, int, int, int, int, Uint32))0x4fbea370,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, GLboolean, const GLfloat*))0x502ac1d2,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, GLboolean, const GLfloat*))0x50a8d153,
    (void (DNLOAD_APIENTRY *)(GLint, GLsizei, GLboolean, const GLfloat*))0x5126e0d4,
    (void (DNLOAD_APIENTRY *)(GLenum, GLuint))0x53a3ca18,
    (int (*)(SDL_cond*))0x5e41dcdb,
    (void (*)(fftw_plan))0x5f9b574a,
    (void (*)(SDL_Thread*, int*))0x62469d23,
    (void (DNLOAD_APIENTRY *)(GLenum, GLsizei, GLenum, const GLvoid*))0x64074f40,
    (void (DNLOAD_APIENTRY *)(GLuint))0x647c0b08,
    (int (*)(SDL_Event*))0x64949d97,
    (void (DNLOAD_APIENTRY *)(GLuint))0x686f65b5,
    (void (DNLOAD_APIENTRY *)(GLboolean))0x695018a,
    (GLuint (DNLOAD_APIENTRY *)(GLenum))0x6b4ffac6,
    (void (*)(unsigned int))0x6b699dd8,
    (void (*)(SDL_mutex*))0x6dda9ec9,
    (int (*)(Uint32))0x70d6574,
    (float (*)(float))0x716e0d8,
    (void (DNLOAD_APIENTRY *)(GLboolean, GLboolean, GLboolean, GLboolean))0x71de4aca,
    (int (*)(SDL_mutex*))0x72b4ef70,
    (FT_Error (*)(FT_Library*))0x773710a4,
    (GLuint (DNLOAD_APIENTRY *)(void))0x78721c3,
    (void (DNLOAD_APIENTRY *)(GLsizei, GLuint*))0x7c824ef2,
    (void (*)(void))0x7eb657f3,
    (SDL_threadID (*)(void))0x7eb690c9,
    (SDL_Thread* (*)(int (*)(void*), const char*, void*))0x83d86faa,
    (FT_Error (*)(FT_GlyphSlot, FT_Render_Mode))0x88a089d4,
    (void (DNLOAD_APIENTRY *)(GLfloat, GLfloat, GLfloat, GLfloat))0x8c118fbb,
    (SDL_cond* (*)(void))0x8ebee9c2,
    (float (*)(float, float))0x921b2a2e,
    (void (DNLOAD_APIENTRY *)(GLenum, GLuint))0x95e43fb9,
    (float (*)(float))0x96b7bbc9,
    (void (DNLOAD_APIENTRY *)(GLenum, GLsizeiptr, const GLvoid*, GLenum))0x9aa49d4f,
    (void (DNLOAD_APIENTRY *)(GLsizei, GLuint*))0x9bdd4fa3,
    (void (*)(SDL_cond*))0x9c453778,
    (int (*)(OpusDecoder*, const unsigned char*, opus_int32, float*, int, int))0x9c84190b,
    (void (DNLOAD_APIENTRY *)(GLenum, GLuint))0xa0fdff6b,
    (void (DNLOAD_APIENTRY *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))0xa259532b,
    (FT_UInt (*)(FT_Face, FT_ULong))0xb1177d43,
    (void (DNLOAD_APIENTRY *)(GLsizei, const GLuint*))0xb1319e23,
    (void (DNLOAD_APIENTRY *)(GLsizei, GLuint*))0xb1503371,
    (void* (*)(void*, size_t))0xb1ae4962,
    (void (DNLOAD_APIENTRY *)(GLenum))0xb5f7c43,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLenum, GLenum))0xb82574f3,
    (fftw_plan (*)(int, double*, double*, fftw_r2r_kind, unsigned))0xb8531c5a,
    (int (*)(int))0xb88bf697,
    (void (DNLOAD_APIENTRY *)(GLuint))0xbd317294,
    (void (*)(void*))0xc23f2ccc,
    (void (DNLOAD_APIENTRY *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))0xc443174a,
    (void (DNLOAD_APIENTRY *)(GLuint))0xc5165dd3,
    (void (DNLOAD_APIENTRY *)(GLuint, GLsizei, const GLchar**, const GLint*))0xc609c385,
    (void (DNLOAD_APIENTRY *)(GLenum))0xcab98122,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLsizei, GLsizei))0xcbd90e40,
    (SDL_mutex* (*)(void))0xcc177eff,
    (void (DNLOAD_APIENTRY *)(GLuint))0xcc55bb62,
    (float (*)(float))0xcd6ca938,
    (GLint (DNLOAD_APIENTRY *)(GLuint, const GLchar*))0xceb27dd0,
    (uint32_t (*)(void))0xd1d0b104,
    (float (*)(float))0xd2cc2a11,
    (void (DNLOAD_APIENTRY *)(GLfloat, GLfloat))0xd77292a8,
    (void (DNLOAD_APIENTRY *)(GLenum))0xd7d4d450,
    (float (*)(float))0xd7efe342,
    (FT_Error (*)(FT_Face, FT_UInt, FT_Int32))0xdb48d8e4,
    (SDL_GLContext (*)(SDL_Window*))0xdba45bd,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLfloat))0xdefef0bf,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLint))0xdefef0c2,
    (float (*)(float))0xe0f62bba,
    (void (DNLOAD_APIENTRY *)(GLenum))0xe379fd94,
    (OpusDecoder* (*)(opus_int32, int, int*))0xe3ffda57,
    (long (*)(float))0xe5e5b9bd,
    (int (*)(void))0xe83af065,
    (void (DNLOAD_APIENTRY *)(GLuint))0xe9e99723,
    (void (DNLOAD_APIENTRY *)(GLenum, GLenum, GLint, GLuint))0xea8c7dfe,
    (void (DNLOAD_APIENTRY *)(GLint, GLint, GLsizei, GLsizei))0xecca892b,
    (void (DNLOAD_APIENTRY *)(GLenum))0xf1854d68,
};
#endif

#if defined(USE_LD)
/// \cond
#define dnload()
/// \endcond
#else
/// SDBM hash function.
///
/// \param op String to hash.
/// \return Full hash.
static uint32_t dnload_hash(const uint8_t *op)
{
    uint32_t ret = 0;
    for(;;)
    {
        uint32_t cc = *op++;
        if(!cc)
        {
            return ret;
        }
        ret = ret * 65599 + cc;
    }
}
#if defined(__FreeBSD__)
#include <sys/link_elf.h>
#elif defined(__linux__)
#include <link.h>
#else
#error "no elf header location known for current platform"
#endif
#if (8 == DNLOAD_POINTER_SIZE)
/// Elf header type.
typedef Elf64_Ehdr dnload_elf_ehdr_t;
/// Elf program header type.
typedef Elf64_Phdr dnload_elf_phdr_t;
/// Elf dynamic structure type.
typedef Elf64_Dyn dnload_elf_dyn_t;
/// Elf symbol table entry type.
typedef Elf64_Sym dnload_elf_sym_t;
/// Elf dynamic structure tag type.
typedef Elf64_Sxword dnload_elf_tag_t;
#else
/// Elf header type.
typedef Elf32_Ehdr dnload_elf_ehdr_t;
/// Elf program header type.
typedef Elf32_Phdr dnload_elf_phdr_t;
/// Elf dynamic structure type.
typedef Elf32_Dyn dnload_elf_dyn_t;
/// Elf symbol table entry type.
typedef Elf32_Sym dnload_elf_sym_t;
/// Elf dynamic structure tag type.
typedef Elf32_Sword dnload_elf_tag_t;
#endif
/// Get dynamic section element by tag.
///
/// \param dyn Dynamic section.
/// \param tag Tag to look for.
/// \return Pointer to dynamic element.
static const dnload_elf_dyn_t* elf_get_dynamic_element_by_tag(const void *dyn, dnload_elf_tag_t tag)
{
    const dnload_elf_dyn_t *dynamic = (const dnload_elf_dyn_t*)dyn;
    do {
        ++dynamic; // First entry in PT_DYNAMIC is probably nothing important.
    } while(dynamic->d_tag != tag);
    return dynamic;
}
#if defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS) || defined(DNLOAD_SAFE_SYMTAB_HANDLING)
/// Get the address associated with given tag in a dynamic section.
///
/// \param dyn Dynamic section.
/// \param tag Tag to look for.
/// \return Address matching given tag.
static const void* elf_get_dynamic_address_by_tag(const void *dyn, dnload_elf_tag_t tag)
{
    const dnload_elf_dyn_t *dynamic = elf_get_dynamic_element_by_tag(dyn, tag);
    return (const void*)dynamic->d_un.d_ptr;
}
#endif
#if !defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS)
/// Link map address, fixed location in ELF headers.
extern const struct r_debug *dynamic_r_debug __attribute__((aligned(1)));
#endif
/// Get the program link map.
///
/// \return Link map struct.
static const struct link_map* elf_get_link_map()
{
#if defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS)
    // ELF header is in a fixed location in memory.
    const void* ELF_BASE_ADDRESS = (const void*)(
#if defined(__arm__)
            0x10000
#elif defined(__i386__)
            0x2000000
#else
#if (8 != DNLOAD_POINTER_SIZE)
#error "no base address known for current platform"
#endif
            0x400000
#endif
            );
    // First program header is located directly afterwards.
    const dnload_elf_ehdr_t *ehdr = (const dnload_elf_ehdr_t*)ELF_BASE_ADDRESS;
    const dnload_elf_phdr_t *phdr = (const dnload_elf_phdr_t*)((size_t)ehdr + (size_t)ehdr->e_phoff);
    do {
        ++phdr; // Dynamic header is probably never first in PHDR list.
    } while(phdr->p_type != PT_DYNAMIC);
    // Find the debug entry in the dynamic header array.
    {
        const struct r_debug *debug = (const struct r_debug*)elf_get_dynamic_address_by_tag((const void*)phdr->p_vaddr, DT_DEBUG);
        return debug->r_map;
    }
#else
    return dynamic_r_debug->r_map;
#endif
}
/// Return pointer from link map address.
///
/// \param lmap Link map.
/// \param ptr Pointer in this link map.
static const void* elf_transform_dynamic_address(const struct link_map *lmap, const void *ptr)
{
#if defined(__linux__)
    // Addresses may also be absolute.
    if(ptr >= (void*)(size_t)lmap->l_addr)
    {
        return ptr;
    }
#endif
    return (uint8_t*)ptr + (size_t)lmap->l_addr;
}
#if defined(DNLOAD_SAFE_SYMTAB_HANDLING)
/// Get address of one dynamic section corresponding to given library.
///
/// \param lmap Link map.
/// \param tag Tag to look for.
/// \return Pointer to given section or NULL.
static const void* elf_get_library_dynamic_section(const struct link_map *lmap, dnload_elf_tag_t tag)
{
    const void* ptr = elf_get_dynamic_address_by_tag((const dnload_elf_dyn_t*)(lmap->l_ld), tag);
    return elf_transform_dynamic_address(lmap, ptr);
}
#endif
/// Find a symbol in any of the link maps.
///
/// Should a symbol with name matching the given hash not be present, this function will happily continue until
/// we crash. Size-minimal code has no room for error checking.
///
/// \param hash Hash of the function name string.
/// \return Symbol found.
static void* dnload_find_symbol(uint32_t hash)
{
    const struct link_map* lmap = elf_get_link_map();
#if defined(__linux__) && (8 == DNLOAD_POINTER_SIZE)
    // On 64-bit Linux, the second entry is not usable.
    lmap = lmap->l_next;
#endif
    for(;;)
    {
        // First entry is this object itself, safe to advance first.
        lmap = lmap->l_next;
        {
#if defined(DNLOAD_SAFE_SYMTAB_HANDLING)
            const dnload_elf_sym_t* symtab = (const dnload_elf_sym_t*)elf_get_library_dynamic_section(lmap, DT_SYMTAB);
            const char* strtab = (char*)elf_get_library_dynamic_section(lmap, DT_STRTAB);
            const dnload_elf_sym_t* symtab_end = (const dnload_elf_sym_t*)strtab;
            // If the section immediately following SYMTAB is not STRTAB, it may be something else.
            {
                const dnload_elf_sym_t *potential_end = (const dnload_elf_sym_t*)elf_get_library_dynamic_section(lmap, DT_VERSYM);
                if(potential_end < symtab_end)
                {
                    symtab_end = potential_end;
                }
            }
#else
            // Assume DT_SYMTAB dynamic entry immediately follows DT_STRTAB dynamic entry.
            // Assume DT_STRTAB memory block immediately follows DT_SYMTAB dynamic entry.
            const dnload_elf_dyn_t *dynamic = elf_get_dynamic_element_by_tag(lmap->l_ld, DT_STRTAB);
            const char* strtab = (const char*)elf_transform_dynamic_address(lmap, (const void*)(dynamic->d_un.d_ptr));
            const dnload_elf_sym_t *symtab_end = (const dnload_elf_sym_t*)strtab;
            ++dynamic;
            const dnload_elf_sym_t *symtab = (const dnload_elf_sym_t*)elf_transform_dynamic_address(lmap, (const void*)(dynamic->d_un.d_ptr));
#endif
            for(const dnload_elf_sym_t *sym = symtab; (sym < symtab_end); ++sym)
            {
                const char *name = strtab + sym->st_name;
                if(dnload_hash((const uint8_t*)name) == hash)
                {
                    void* ret_addr = (void*)((const uint8_t*)sym->st_value + (size_t)lmap->l_addr);
#if defined(__linux__) && (defined(__aarch64__) || defined(__i386__) || defined(__x86_64__))
                    // On Linux and various architectures, need to check for IFUNC.
                    if((sym->st_info & 15) == STT_GNU_IFUNC)
                    {
                        ret_addr = ((void*(*)())ret_addr)();
                    }
#endif
                    return ret_addr;
                }
            }
        }
    }
}
/// Perform init.
///
/// Import by hash - style.
static void dnload(void)
{
    unsigned ii;
    for(ii = 0; (107 > ii); ++ii)
    {
        void **iter = ((void**)&g_symbol_table) + ii;
        *iter = dnload_find_symbol(*(uint32_t*)iter);
    }
}
#endif

#if defined(__clang__)
/// Visibility declaration for symbols that require it (clang).
#define DNLOAD_VISIBILITY __attribute__((visibility("default")))
#else
/// Visibility declaration for symbols that require it (gcc).
#define DNLOAD_VISIBILITY __attribute__((externally_visible,visibility("default")))
#endif

#if !defined(USE_LD)
#if defined(__cplusplus)
extern "C"
{
#endif
/// Program entry point.
void _start() DNLOAD_VISIBILITY;
#if defined(__cplusplus)
}
#endif
#endif
#endif

