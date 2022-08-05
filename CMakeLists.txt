cmake_minimum_required(VERSION 3.1)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")
set(CMAKE_SYSTEM_VERSION "10.0.18362.0" CACHE STRING INTERNAL FORCE)
project(kerava)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost("program_options" "wave")
find_fftw()
find_flac()
find_freetype()
find_ogg()
find_opus()
find_png()
find_sdl2()
find_threads()

check_mali()
if(MALI_FOUND)
    add_definitions(-DDNLOAD_MALI -DDNLOAD_GLESV2)
    set(EGL_LIBRARY "EGL")
    set(OPENGL_gl_LIBRARY "GLESv2")
else()
    find_opengl()
    find_glew()
endif()

output_flags("DEBUG" on)

add_definitions(-DUSE_LD)

include_directories("${PROJECT_SOURCE_DIR}/src")

set(VGL_ROOT "${PROJECT_SOURCE_DIR}/src/vgl")
include("${VGL_ROOT}/filelist.cmake")

add_executable("kerava"
    "src/dnload.h"
    "src/gnu_rand.c"
    "src/gnu_rand.h"
    "src/image_png.cpp"
    "src/image_png.hpp"
    "src/intro_data.hpp"
    "src/intro_state.hpp"
    "src/intro_world.hpp"
    "src/main.cpp"
    "src/mesh_sirkus_hevo_nen.hpp"
    "src/mesh_ukko.hpp"
    "src/header.glsl.hpp"
    "src/font.frag.glsl.hpp"
    "src/font.vert.glsl.hpp"
    "src/font_overlay.frag.glsl.hpp"
    "src/font_overlay.vert.glsl.hpp"
    "src/offscreen.frag.glsl.hpp"
    "src/offscreen.vert.glsl.hpp"
    "src/post.frag.glsl.hpp"
    "src/post.vert.glsl.hpp"
    "src/skeleton.frag.glsl.hpp"
    "src/skeleton.vert.glsl.hpp"
    "src/visualization.frag.glsl.hpp"
    "src/visualization.vert.glsl.hpp"
    "src/synth/songdata.hpp"
    "src/synth/verbatim_chorus.hpp"
    "src/synth/verbatim_common.hpp"
    "src/synth/verbatim_delay.hpp"
    "src/synth/verbatim_distortion.hpp"
    "src/synth/verbatim_echo.hpp"
    "src/synth/verbatim_env_gen.hpp"
    "src/synth/verbatim_filter.hpp"
    "src/synth/verbatim_oscillator.hpp"
    "src/synth/verbatim_parameters.hpp"
    "src/synth/verbatim_poly_handler.hpp"
    "src/synth/verbatim_reverb.hpp"
    "src/synth/verbatim_voice.hpp"
    "src/synth/verbatim_synth.hpp"
    "src/synth/BandLimit.hpp"
    "src/synth/BandLimit.cpp"
    "src/synth/ops_log.hpp"
    ${VGL_HEADERS}
    ${VGL_SOURCES})
if(MSVC)
    target_link_libraries("kerava" "Imm32")
    target_link_libraries("kerava" "Setupapi")
    target_link_libraries("kerava" "version")
    target_link_libraries("kerava" "WinMM")
    target_link_libraries("kerava" "${OPENGL_gl_LIBRARY}")
    target_link_libraries("kerava" debug "${GLEW_LIBRARY_DEBUG}" optimized "${GLEW_LIBRARY}")
    target_link_libraries("kerava" debug "${FFTW_LIBRARY_DEBUG}" optimized "${FFTW_LIBRARY}")
    target_link_libraries("kerava" debug "${FLAC_LIBRARY_DEBUG}" optimized "${FLAC_LIBRARY}")
    target_link_libraries("kerava" debug "${FREETYPE_LIBRARY_DEBUG}" optimized "${FREETYPE_LIBRARY}")
    target_link_libraries("kerava" debug "${OGG_LIBRARY_DEBUG}" optimized "${OGG_LIBRARY}")
    target_link_libraries("kerava" debug "${OPUS_LIBRARY_DEBUG}" optimized "${OPUS_LIBRARY}")
    target_link_libraries("kerava" debug "${PNG_LIBRARY_DEBUG}" optimized "${PNG_LIBRARY}")
    target_link_libraries("kerava" debug "${SDL2_LIBRARY_DEBUG}" optimized "${SDL2_LIBRARY}")
    target_link_libraries("kerava" debug "${ZLIB_LIBRARY_DEBUG}" optimized "${ZLIB_LIBRARY}")
else()
    if(MALI_FOUND)
        target_link_libraries("kerava" "${EGL_LIBRARY}")
        target_link_libraries("kerava" "${OPENGL_gl_LIBRARY}")
    else()
        target_link_libraries("kerava" "${GLEW_LIBRARY}")
    endif()
    target_link_libraries("kerava" "${BOOST_PROGRAM_OPTIONS_LIBRARY}")
    target_link_libraries("kerava" "${BOOST_WAVE_LIBRARY}")
    target_link_libraries("kerava" "${FFTW_LIBRARY}")
    target_link_libraries("kerava" "${FLAC_LIBRARY}")
    target_link_libraries("kerava" "${FREETYPE_LIBRARY}")
    target_link_libraries("kerava" "${OGG_LIBRARY}")
    target_link_libraries("kerava" "${OPUS_LIBRARY}")
    target_link_libraries("kerava" "${PNG_LIBRARY}")
    target_link_libraries("kerava" "${SDL2_LIBRARY}")
endif()
target_link_libraries("kerava" "${THREADS_LIBRARY}")
