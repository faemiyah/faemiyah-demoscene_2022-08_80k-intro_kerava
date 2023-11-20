#include "dnload.h"

#if defined(USE_LD)
#include "image_png.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#endif

//######################################
// Define ##############################
//######################################

#if !defined(DISPLAY_MODE)
/// Screen mode.
///
/// Negative values windowed.
/// Positive values fullscreen.
#define DISPLAY_MODE -720
#endif

/// \cond
#if (0 > (DISPLAY_MODE))
#define SCREEN_F 0
#define SCREEN_H (-(DISPLAY_MODE))
#elif (0 < (DISPLAY_MODE))
#define SCREEN_F 1
#define SCREEN_H (DISPLAY_MODE)
#else
#error "invalid display mode (pre)"
#endif
#if ((800 == SCREEN_H) || (1200 == SCREEN_H))
#define SCREEN_W ((SCREEN_H / 10) * 16)
#else
#define SCREEN_W (((SCREEN_H * 16) / 9) - (((SCREEN_H * 16) / 9) % 4))
#endif
/// \endcond

/// Desired framerate.
#define INTRO_FRAMERATE 50

/// Milliseconds per frame.
#define FRAME_MILLISECONDS (1000 / INTRO_FRAMERATE)

/// Intro length (frames).
#define INTRO_LENGTH (INTRO_FRAMERATE * 124)

/// Intro start position (in seconds).
#define INTRO_START (INTRO_FRAMERATE * 0)

/// Size of one sample in bytes.
#define AUDIO_SAMPLE_SIZE 4
/// Audio channels.
#define AUDIO_CHANNELS 2
/// Audio samplerate.
#define AUDIO_SAMPLERATE 44100

/// \cond
#if (4 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_F32SYS
typedef float sample_t;
#elif (2 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_S16SYS
typedef int16_t sample_t;
#elif (1 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_U8
typedef uint8_t sample_t;
#else
#error "invalid audio sample size"
#endif
/// \endcond

/// Audio byterate.
#define AUDIO_BYTERATE (AUDIO_CHANNELS * AUDIO_SAMPLERATE * AUDIO_SAMPLE_SIZE)

/// Intro length (in bytes of audio).
#define INTRO_LENGTH_AUDIO ((INTRO_LENGTH / INTRO_FRAMERATE) * AUDIO_BYTERATE)

/// Intro start (in bytes of audio).
#define INTRO_START_AUDIO ((INTRO_START / INTRO_FRAMERATE) * AUDIO_BYTERATE)

#if !defined(SAMPLE_TEST)
/// Sample test toggle.
#define SAMPLE_TEST 0
#endif

#if !defined(ENABLE_CHARTS)
/// Charts enabled.
#define ENABLE_CHARTS 0
#endif

//######################################
// Pre-vgl definitions #################
//######################################

#if 0
/// Output a integer value (for debugging).
///
/// \param op Value to output.
static void puts_hex(size_t op)
{
    static const char representation[] =
    {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f',
    };
    char output[sizeof(op) * 2 + 1];
    output[sizeof(op) * 2] = 0;

    for(unsigned ii = 0; (ii < sizeof(op) * 2); ++ii)
    {
        int idx = op & static_cast<size_t>(0xF);
        output[sizeof(op) * 2 - ii - 1] = representation[idx];
        op = op >> 4;
    }

    dnload_puts(output);
}

/// Output a integer value (for debugging).
///
/// \param op Value to output.
void puts_hex(int op)
{
    static const char representation[] =
    {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f',
    };
    char output[sizeof(op) * 2 + 1];
    output[sizeof(op) * 2] = 0;

    for(unsigned ii = 0; (ii < sizeof(op) * 2); ++ii)
    {
        int idx = op & static_cast<int>(0xF);
        output[sizeof(op) * 2 - ii - 1] = representation[idx];
        op = op >> 4;
    }

    dnload_puts(output);
}

/// Output a pointer value (for debugging).
///
/// \param op Value to output.
static void puts_ptr(void* op)
{
    size_t value = reinterpret_cast<size_t>(op);
    puts_hex(value);
}
#endif

//######################################
// vgl include #########################
//######################################

// Only include top-level headers.
#include "vgl/vgl_animation_state.hpp"
#include "vgl/vgl_font.hpp"
#include "vgl/vgl_frame_buffer.hpp"
#include "vgl/vgl_image_2d_gray.hpp"
#include "vgl/vgl_logical_mesh.hpp"
#include "vgl/vgl_opus.hpp"
#include "vgl/vgl_render_queue.hpp"

#if defined(ENABLE_CHARTS) && ENABLE_CHARTS
#include "vgl/vgl_spline.hpp"
#endif
#if defined(USE_LD)
#include "vgl/vgl_csg_file.hpp"
#endif

//######################################
// Global data #########################
//######################################

/// Audio buffer for output.
static uint8_t g_audio_buffer[INTRO_LENGTH_AUDIO * 9 / 8];

/// Current audio position.
static int g_audio_position = INTRO_START_AUDIO;

/// Global SDL window storage.
SDL_Window *g_sdl_window;

#if defined(USE_LD)

/// Next audio position for audio playback.
static int g_next_audio_position = -1;

/// Debug position.
/// Also serves as debug orientation toggle.
static vgl::optional<vgl::vec3> g_pos;
/// Debug forward.
static vgl::vec3 g_fw;
/// Debug up.
static vgl::vec3 g_up;
/// Debug FOV.
static float g_fov;

/// Preview mesh for when opening the mesh viewer.
vgl::string g_preview_mesh;

/// Generation seed to use.
vgl::optional<unsigned> g_seed;

/// Record audio toggle.
static bool g_flag_record_audio = false;
/// Record video toggle.
static bool g_flag_record_video = false;

/// Visual debug mode.
static int g_visual_debug = 0;

static const char *usage = ""
"Usage: kerava <options>\n"
"For Assembly 2022 real wild compo.\n"
"Release version does not pertain to any size limitations.\n";

/// Developer mode global toggle.
static bool g_flag_developer = false;

/// Intro time movement direction (default: one frame forward per iteration).
static int g_time_delta = 1;

/// Intro time movement multiplier.
static int g_time_mul = 1;

/// Global screen width.
static int g_screen_w = SCREEN_W;

/// Global screen height.
static int g_screen_h = SCREEN_H;

#else

/// Developer mode disabled.
#define g_flag_developer 0

/// Can only move forward.
#define g_time_delta 1

/// Only one speed.
#define g_time_mul 1

/// Global screen width.
#define g_screen_w SCREEN_W

/// Global screen height.
#define g_screen_h SCREEN_H

#endif

//######################################
// Frame time display ##################
//######################################

#if defined(USE_LD)

/// Frame time container.
class FrameTimeCounter
{
private:
    /// Nanosecond multiplier.
    static const int64_t NSEC_MUL = 1000000000;

private:
    /// Internal structure for calculating frame time.
    ///
    /// All times are in nanoseconds.
    struct FrameTime
    {
    public:
        /// Timestamp for this frame (after swap).
        int64_t m_stamp;

        /// Frame time until end of draw.
        int64_t m_frame_time;

        /// Frame time until end of swap.
        int64_t m_swap_time;

    public:
        /// Constructor.
        ///
        /// \param stamp Timestamp.
        /// \param ftime Frame time.
        /// \param stime swap time.
        FrameTime(int64_t stamp, int64_t ftime, int64_t stime) :
            m_stamp(stamp),
            m_frame_time(ftime),
            m_swap_time(stime)
        {
        }
    };

private:
    /// Queue of frame times.
    vgl::queue<FrameTime> m_queue;

    /// Total frame time.
    int64_t m_total_frame_time = 0;

    /// Total swap time.
    int64_t m_total_swap_time = 0;

#if defined(WIN32)
    /// Performance counter frequency.
    LARGE_INTEGER m_pfreq;
#endif

public:
    /// Default constructor.
    explicit FrameTimeCounter()
    {
#if defined(WIN32)
        if(!QueryPerformanceFrequency(&m_pfreq))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("QueryPerformanceFrequency() failed"));
        }
#endif
    }

private:
    /// Get average frame time.
    ///
    /// \param frame_time Frame time to calculate.
    /// \return Average frame time, as a string.
    std::string getAverageFrameTime(int64_t frame_time) const
    {
        const int64_t NMUL = 1000000;
        const int64_t NDIV = 10000;
        double average_ftime = static_cast<double>(frame_time) / static_cast<double>(m_queue.size());
        int64_t average_time = std::lround(average_ftime);

        return std::to_string(average_time / NMUL) + "." + std::to_string((average_time % NMUL) / NDIV);
    }

public:
    /// Add a frame time.
    ///
    /// \param tstart Timestamp at the beginning of render.
    /// \param tmid Timestamp after draw.
    /// \param tend Timestamp after swap.
    void addFrame(int64_t tstart, int64_t tmid, int64_t tend)
    {
        int64_t ftime = timestamp_diff(tstart, tmid);
        int64_t stime = timestamp_diff(tstart, tend);

        // Add new frame, update total times.
        m_queue.emplace(tend, ftime, stime);
        m_total_frame_time += ftime;
        m_total_swap_time += stime;

        // Drop frames older than 1s, update total times.
        for(;;)
        {
            const FrameTime& first = m_queue.front();

            if(timestamp_diff(first.m_stamp, tend) >= NSEC_MUL)
            {
                m_total_frame_time -= first.m_frame_time;
                m_total_swap_time -= first.m_swap_time;
                m_queue.pop();
                continue;
            }
            break;
        }
    }

    /// Get frame time.
    ///
    /// \return Average frame time, as a string.
    std::string getFrameTime() const
    {
        return getAverageFrameTime(m_total_frame_time);
    }

    /// Get swap time.
    ///
    /// \return Average swap time, as a string.
    std::string getSwapTime() const
    {
        return getAverageFrameTime(m_total_swap_time);
    }

    /// Get framerate.
    ///
    /// \return Framerate over the last second.
    std::string getFramerate() const
    {
        return std::to_string(m_queue.size());
    }

public:
    /// Gets nanosecond timestamp.
    ///
    /// Return timestamp that could be acquired from a POSIX timespec.
    /// Also works on platforms without clock_gettime().
    /// Second count is forced to 32 bits to enable monitoring of wrap.
    ///
    /// \return Timestamp in nanoseconds.
    int64_t get_timespec_timestamp() const
    {
#if defined(WIN32)
        LARGE_INTEGER query;
        if(!QueryPerformanceCounter(&query))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("QueryPerformanceCounter() failed"));
        }
        int32_t tv_sec_32 = static_cast<int32_t>(query.QuadPart / m_pfreq.QuadPart);
        int64_t tv_nsec = (static_cast<int64_t>(query.QuadPart % m_pfreq.QuadPart) * NSEC_MUL) / static_cast<int64_t>(m_pfreq.QuadPart);
        return static_cast<int64_t>(tv_sec_32) * NSEC_MUL + tv_nsec;
#else
        timespec ret;
        if(clock_gettime(CLOCK_MONOTONIC, &ret))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("clock_gettime() failed"));
        }
        int32_t tv_sec_32 = static_cast<int32_t>(ret.tv_sec);
        return static_cast<int64_t>(tv_sec_32) * NSEC_MUL + static_cast<int64_t>(ret.tv_nsec);
#endif
    }

    /// Timestamp difference.
    ///
    /// \param tstart Start time.
    /// \param tend End time.
    constexpr static int64_t timestamp_diff(int64_t tstart, int64_t tend)
    {
        if(tend >= tstart)
        {
            return tend - tstart;
        }
        // The one difference for wrap is baked into not substracting from tneg.
        int64_t tpos = tstart - static_cast<int64_t>(std::numeric_limits<int32_t>::min()) * NSEC_MUL;
        int64_t tneg = static_cast<int64_t>(std::numeric_limits<int32_t>::max()) * NSEC_MUL + NSEC_MUL - tend;
        return tpos + tneg;
    }
};

/// Global frame time counter.
static FrameTimeCounter g_frame_counter;

/// Frame time display enabled.
static bool g_last_frame_display = false;

#endif

//######################################
// Frame number bookkeeping ############
//######################################

/// Generates an audio position based on frame number.
///
/// Used for jumping into a correct frame as well as finding the position for audio visualization.
///
/// \param op Frame index.
/// \return Audio position in bytes.
static int generate_audio_position(int op)
{
    static_assert((AUDIO_BYTERATE % INTRO_FRAMERATE) == 0, "audio byterate is not divisible by intro framerate");
    int next_pos = op * (AUDIO_BYTERATE / INTRO_FRAMERATE);
    int remainder = next_pos % (AUDIO_SAMPLE_SIZE * AUDIO_CHANNELS);
    return next_pos - remainder;
}

#if defined(USE_LD)
/// Guarder container type for frame number.
class FrameNumber
{
private:
    /// Frame number counter.
    int m_frame_idx = 0;

    /// Guard for the frame number counter.
    vgl::Mutex m_mutex;

public:
    /// Constructor.
    explicit FrameNumber() = default;

public:
    /// Accessor.
    ///
    /// \return Frame index.
    int getFrameIdx()
    {
        return m_frame_idx;
    }

    /// Increment the frame number based on current time delta.
    /// \return Pointer to this.
    void* advanceFrame()
    {
        assignFrame(m_frame_idx + (g_time_delta * g_time_mul));
        return this;
    }

    /// Assign the frame, potentially clamping it.
    ///
    /// \param op New frame index.
    void assignFrame(int op)
    {
        vgl::ScopedLock sl(m_mutex);

        // Can never go negative.
        m_frame_idx = vgl::max(op, 0);

        // If developer mode is on also clamp the end so advancing cannot quit.
        if(g_flag_developer)
        {
            m_frame_idx = vgl::min(m_frame_idx, INTRO_LENGTH);
        }
    }

    /// Assign the audio position based on current frame.
    void assignAudioPosition()
    {
        g_next_audio_position = generate_audio_position(m_frame_idx);
    }
};

/// GLobal frame number.
FrameNumber g_frame_number;
#endif

/// Gets the frame number for given pointer.
/// \param op Frame number pointer.
static int get_frame_number(void* op)
{
#if defined(USE_LD)
    FrameNumber* num = static_cast<FrameNumber*>(op);
    return num->getFrameIdx();
#else
    return static_cast<int>(reinterpret_cast<size_t>(op));
#endif
}

/// Advances the frame number for given pointer.
/// \param op Frame number pointer.
static void* advance_frame_number(void* op)
{
#if defined(USE_LD)
    FrameNumber* num = static_cast<FrameNumber*>(op);
    return num->advanceFrame();
#else
    size_t frame_idx = reinterpret_cast<size_t>(op);
    return reinterpret_cast<void*>(frame_idx + g_time_delta);
#endif
}

//######################################
// Utility #############################
//######################################

#if defined(USE_LD)

/// Human-readable fraction.
///
/// \param num Number to get fraction from.
/// \param detail Fraction detail.
static std::string human_readable_fraction(float num, unsigned detail)
{
    if(detail == 1)
    {
        char lim[4];
        sprintf(lim, "%.1f", num);
        return std::string(lim + 1);
    }

    // 2 is maximum.
    char lim[5];
    sprintf(lim, "%.2f", num);
    return std::string(lim + 1);
}

/// Return a human-readable string of a given number.
///
/// \param op Memory size.
/// \return Human-readable string.
static std::string human_readable_memory(unsigned op)
{
    if(op == 1)
    {
        return std::string("1 byte");
    }

    if(op < 1024)
    {
        return std::to_string(op) + " bytes";
    }

    if(op < 1024 * 1024)
    {
        const char* size_name = " kbytes";
        unsigned kb = op / 1024;
        if(kb > 100)
        {
            return std::to_string(kb) + size_name;
        }
        float num = static_cast<float>(op - (kb * 1024)) / 1024.0f;
        if(kb > 10)
        {
            return std::to_string(kb) + human_readable_fraction(num, 1) + size_name;
        }
        return std::to_string(kb) + human_readable_fraction(num, 2) + size_name;
    }

    const char* size_name = " Mbytes";
    unsigned mb = op / 1048576;
    if(mb > 100)
    {
        return std::to_string(mb) + size_name;
    }
    float num = static_cast<float>(op - mb * 1048576) / 1048576.0f;
    if(mb > 10)
    {
        return std::to_string(mb) + human_readable_fraction(num, 1) + size_name;
    }
    return std::to_string(mb) + human_readable_fraction(num, 2) + size_name;
}

/// Creates a human-readable string from orientation data.
///
/// \param pos Position.
/// \param fw Forward.
/// \param up Up.
/// \return String representation.
static std::string orientation_string(const vgl::vec3& pos, const vgl::vec3& fw, const vgl::vec3& up, float fov)
{
    vgl::vec3 npos = pos * 10.0f;
    int iposx = vgl::iround(npos.x());
    int iposy = vgl::iround(npos.y());
    int iposz = vgl::iround(npos.z());
    vgl::vec3 nfw = normalize(fw) * 100.0f;
    int ifwx = vgl::iround(nfw.x());
    int ifwy = vgl::iround(nfw.y());
    int ifwz = vgl::iround(nfw.z());
    vgl::vec3 nup = normalize(up) * 100.0f;
    int iupx = vgl::iround(nup.x());
    int iupy = vgl::iround(nup.y());
    int iupz = vgl::iround(nup.z());
    int ifov = vgl::iround(fov * static_cast<float>(255.0 / M_PI));
    std::ostringstream sstr;
    sstr << "pos: " << iposx << ", " << iposy << ", " << iposz << " ;; fw: " << ifwx << ", " <<
        ifwy << ", " << ifwz << " ;; up: " << iupx << ", " << iupy << ", " << iupz << " ;; fov: " << ifov;
    return sstr.str();
}

/// Parse resolution from string input.
///
/// \param op Resolution string.
/// \return Tuple of width and height.
static std::pair<unsigned, unsigned> parse_resolution(const std::string &op)
{
    size_t cx = op.find("x");

    if(std::string::npos == cx)
    {
        cx = op.rfind("p");

        if((std::string::npos == cx) || (0 >= cx))
        {
            std::ostringstream sstr;
            sstr << "invalid resolution string '" << op << '\'';
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }

        std::string sh = op.substr(0, cx);

        unsigned rh = boost::lexical_cast<unsigned>(sh);
        unsigned rw = (rh * 16) / 9;
        unsigned rem4 = rw % 4;

        return std::make_pair(rw - rem4, rh);
    }

    std::string sw = op.substr(0, cx);
    std::string sh = op.substr(cx + 1);

    return std::make_pair(boost::lexical_cast<int>(sw), boost::lexical_cast<int>(sh));
}

/// Generates script metainformation string that can be output to a stream.
///
/// \return Script metainformation string.
std::string generate_script_info()
{
    std::ostringstream sstr;
    if(g_pos)
    {
        sstr << "--camera=" << g_pos->x() << "," << g_pos->y() << "," << g_pos->z() <<
            "," << g_fw[0] << "," << g_fw[1] << "," << g_fw[2] <<
            "," << g_up[0] << "," << g_up[1] << "," << g_up[2] << std::endl;
    }
    if(g_seed)
    {
        sstr << "--seed=" << *g_seed << std::endl;
    }
    sstr << "--ticks=" << g_frame_number.getFrameIdx() << std::endl;
    return sstr.str();
}

/// Audio writing callback.
///
/// \param data Raw audio data.
/// \param size Audio data size (in samples).
static void write_audio(std::string_view basename, void *data, size_t size)
{
    std::string filename = std::string(basename) + ".raw";
    FILE *fd = fopen(filename.c_str(), "wb");
    if(!fd)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("could not open '" + filename + "' for writing"));
    }
    fwrite(data, size, 1, fd);
    fclose(fd);
}

/// Image writing callback.
///
/// \param basename Base name of the image.
/// \param screen_w Screen width.
/// \param screen_h Screen height.
/// \param idx Frame index to write.
static void write_frame(std::string_view basename, unsigned screen_w, unsigned screen_h, unsigned idx)
{
    std::unique_ptr<uint8_t[]> image(new uint8_t[screen_w * screen_h * 3]);
    std::ostringstream sstr;

    glReadPixels(0, 0, static_cast<GLsizei>(screen_w), static_cast<GLsizei>(screen_h), GL_RGB, GL_UNSIGNED_BYTE,
            image.get());

    sstr << basename << "_" << std::setfill('0') << std::setw(4) << idx << ".png";

    gfx::image_png_save(sstr.str(), screen_w, screen_h, 24, image.get());
    return;
}

#endif

/// Get current ticks.
///
/// \return Current ticks as int.
static int get_current_ticks()
{
    return static_cast<int>(dnload_SDL_GetTicks());
}

/// Swap buffers.
///
/// Uses global data.
static void swap_buffers()
{
    dnload_SDL_GL_SwapWindow(g_sdl_window);
}

/// Tear down initialized systems.
///
/// Uses global data.
static void teardown()
{
    dnload_SDL_Quit();
}

//######################################
// Intro content #######################
//######################################

#include "header.glsl.hpp" // g_shader_header

#include "font.vert.glsl.hpp" // g_shader_vertex_font
#include "font.frag.glsl.hpp" // g_shader_fragment_font

#include "font_overlay.vert.glsl.hpp" // g_shader_vertex_font_overlay
#include "font_overlay.frag.glsl.hpp" // g_shader_fragment_font_overlay

#include "offscreen.vert.glsl.hpp" // g_shader_vertex_offscreen
#include "offscreen.frag.glsl.hpp" // g_shader_fragment_offscreen

#include "skeleton.vert.glsl.hpp" // g_shader_vertex_skeleton
#include "skeleton.frag.glsl.hpp" // g_shader_fragment_skeleton

#include "visualization.vert.glsl.hpp" // g_shader_vertex_visualization
#include "visualization.frag.glsl.hpp" // g_shader_fragment_visualization

#include "post.vert.glsl.hpp" // g_shader_vertex_post
#include "post.frag.glsl.hpp" // g_shader_fragment_post

#include "intro_data.hpp"
#include "intro_state.hpp"

/// \cond
static void* intro_state_move(void*);
/// \endcond

/// Update mesh data to GPU.
static void intro_state_update_mesh_data()
{
    for(unsigned ii = 0; (ii < 2); ++ii)
    {
        vgl::Mesh& msh = g_data.getMeshVisualization(ii);
        VGL_ASSERT(msh.getGeometryHandle());
        msh.update();
    }
}

/// Draw intro state.
///
/// \return nullptr
static void* intro_state_draw(void*)
{
    if(g_intro_state_current)
    {
        // Update mesh data before drawing.
        intro_state_update_mesh_data();
        g_intro_state_current->draw();
    }
    return nullptr;
};

/// Generate new intro state.
///
/// \param op Pointer to current ticks.
/// \return nullptr
static void* intro_state_generate(void* op)
{
    int frame_number = get_frame_number(op);

    // Need scope to wait on fences.
    {
        vgl::Fence fence_next = vgl::task_wait(intro_state_generate_next, &frame_number);
        vgl::Fence fence_fft = vgl::task_wait(intro_state_generate_mesh_fft, &frame_number);
        vgl::Fence fence_wave = vgl::task_wait(intro_state_generate_mesh_wave, &frame_number);
    }

    // Dispatch swap task.
    vgl::task_dispatch_main(intro_state_move, op);
    return nullptr;
}

/// Move intro state from next into current slot.
///
/// \param op Pointer to current ticks.
/// \return nullptr
static void* intro_state_move(void* op)
{
    // Current state is the generated state, next state is swapped.
    g_intro_state_current = g_intro_state_next;
    g_intro_state_next = (g_intro_state_next == g_intro_states) ? (g_intro_states + 1) : g_intro_states;

    // Dispatch main thread tasks first.
    // Must be before generate, as it could theoretically dispatch a new move task in front otherwise.
    vgl::task_dispatch_main(intro_state_draw, nullptr);

    // Advance time based on time delta, then generate new frame.
    vgl::task_dispatch(intro_state_generate, advance_frame_number(op));

    return nullptr;
}

//######################################
// Audio playback ######################
//######################################

/// \brief Update audio stream.
///
/// \param userdata Not used.
/// \param stream Target stream.
/// \param len Number of bytes to write.
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
#if defined(USE_LD)
    // Read audio position given in parallel by resuming playback.
    int next_pos = g_next_audio_position;
    if(next_pos != -1)
    {
        g_next_audio_position = -1;
        g_audio_position = next_pos;
    }
#endif
    (void)userdata;

    for(int ii = 0; (ii < len); ++ii)
    {
        stream[ii] = g_audio_buffer[g_audio_position + ii];
    }
    g_audio_position += len;
}

/// SDL audio specification struct.
static SDL_AudioSpec audio_spec =
{
    AUDIO_SAMPLERATE,
    AUDIO_SAMPLE_TYPE_SDL,
    AUDIO_CHANNELS,
    0,
#if defined(USE_LD)
    4096,
#else
    256, // ~172.3Hz, lower values seem to cause underruns
#endif
    0,
    0,
    audio_callback,
    NULL
};

//######################################
// intro / _start ######################
//######################################

#if defined(USE_LD)
/// \brief Intro body function.
void intro(bool flag_fullscreen)
#else
void _start()
#define flag_fullscreen SCREEN_F
#endif
{
    dnload();

#if !defined(USE_LD) && 0
    for(void** it = reinterpret_cast<void**>(&g_symbol_table); true; ++it)
    {
        if(reinterpret_cast<size_t>(it) >=
                reinterpret_cast<size_t>(reinterpret_cast<uint8_t*>(&g_symbol_table) + sizeof(g_symbol_table)))
        {
            break;
        }
        puts_ptr(*it);
    }
#endif

    dnload_SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
#if defined(DNLOAD_GLESV2)
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    g_sdl_window = dnload_SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_screen_w,
            g_screen_h, SDL_WINDOW_OPENGL | (flag_fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
    dnload_SDL_GL_CreateContext(g_sdl_window);
    dnload_SDL_ShowCursor(g_flag_developer);

#if defined(USE_LD) && !defined(DNLOAD_GLESV2)
    {
        GLenum err = glewInit();
        if(GLEW_OK != err)
        {
            std::cerr << "glewInit(): " << glewGetErrorString(err) << std::endl;
            teardown();
            exit(1);
        }
    }
#endif

#if defined(USE_LD)
    std::cout << "Vendor:      " + vgl::gl_vendor_string() + "\nVersion:     " + vgl::gl_version_string() <<
        "\nExtensions:  " << vgl::gl_extension_string(79, 13) << std::endl;
#endif

    vgl::tasks_initialize(3);

    vgl::FrameBuffer::initialize_default(static_cast<unsigned>(g_screen_w), static_cast<unsigned>(g_screen_h));
    vgl::task_dispatch(IntroData::taskfunc_initialize, &g_data);

    for(;;)
    {
        vgl::Task task = vgl::task_acquire_main();
        if(task() == IntroData::taskfunc_ready)
        {
            break;
        }
    }

#if defined(USE_LD)
    std::cout << "Vertex data:   " << human_readable_memory(vgl::get_data_size_vertex()) <<  " in " <<
        vgl::get_num_geometry_buffers() << " buffers\nIndex data:    " <<
        human_readable_memory(vgl::get_data_size_index()) << "\nTexture data:  " <<
        human_readable_memory(vgl::get_data_size_texture()) << std::endl;
#endif

#if defined(USE_LD)
    if(g_flag_record_audio || g_flag_record_video)
    {
        if(g_flag_record_audio)
        {
            write_audio("kerava", g_audio_buffer, INTRO_LENGTH_AUDIO);
        }

        if(g_flag_record_video)
        {
            for(int frame_idx = 0; (frame_idx < INTRO_LENGTH); ++frame_idx)
            {
                SDL_Event event;
                if(SDL_PollEvent(&event) && (event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))
                {
                    break;
                }

                IntroState state;
                state.initialize(frame_idx);
                intro_state_generate_mesh_fft(&frame_idx);
                intro_state_generate_mesh_wave(&frame_idx);
                intro_state_update_mesh_data();
                state.draw();
                {
                    const vgl::FrameBuffer& screen = vgl::FrameBuffer::get_default();
                    write_frame("kerava", screen.getWidth(), screen.getHeight(), static_cast<unsigned>(frame_idx));
                }
                swap_buffers();
            }
        }

        // Exit after recording.
        teardown();
        return;
    }
#endif

    // Start draw loop.
#if defined(USE_LD)
    g_time_delta = static_cast<int>(!g_flag_developer);
    vgl::task_dispatch(intro_state_generate, &g_frame_number);
#else
    vgl::task_dispatch(intro_state_generate, reinterpret_cast<void*>(static_cast<size_t>(INTRO_START)));
#endif

    // Open audio device.
    dnload_SDL_OpenAudio(&audio_spec, NULL);
#if defined(USE_LD)
    if(!g_flag_developer)
#endif
    {
        dnload_SDL_PauseAudio(0);
    }
    int prev_ticks = get_current_ticks();

    for(;;)
    {
#if defined(USE_LD)
        static unsigned successful_frames = 0;
        static float move_speed = 2.0f / 50.0f;
        static uint8_t mouse_look = 0;
        static int8_t move_backward = 0;
        static int8_t move_down = 0;
        static int8_t move_forward = 0;
        static int8_t move_left = 0;
        static int8_t move_right = 0;
        static int8_t move_up = 0;
        static int8_t fov_decrease = 0;
        static int8_t fov_increase = 0;
        int mouse_look_x = 0;
        int mouse_look_y = 0;
        bool quit = false;
#endif
        SDL_Event event;

#if defined(USE_LD)
        while(SDL_PollEvent(&event))
        {
            if(SDL_QUIT == event.type)
            {
                quit = true;
            }
            else if(SDL_KEYDOWN == event.type)
            {
                switch(event.key.keysym.sym)
                {
                case SDLK_a:
                    move_left = 1;
                    break;

                case SDLK_d:
                    move_right = 1;
                    break;

                case SDLK_e:
                    move_up = 1;
                    break;

                case SDLK_q:
                    move_down = 1;
                    break;

                case SDLK_s:
                    move_backward = 1;
                    break;

                case SDLK_w:
                    move_forward = 1;
                    break;

                case SDLK_z:
                    fov_decrease = 1;
                    break;

                case SDLK_x:
                    fov_increase = 1;
                    break;

                case SDLK_f:
                    g_last_frame_display = !g_last_frame_display;
                    break;

                case SDLK_p:
                    if(g_time_delta == 0)
                    {
                        g_time_delta = 1;
                        {
                            g_frame_number.assignAudioPosition();
                        }
                        SDL_PauseAudio(0);
                    }
                    else
                    {
                        g_time_delta = 0;
                        SDL_PauseAudio(1);
                    }
                    break;

                case SDLK_LEFT:
                    {
                        int curr_pos = g_frame_number.getFrameIdx();
                        g_frame_number.assignFrame(curr_pos - INTRO_FRAMERATE * 5);
                        g_frame_number.assignAudioPosition();
                    }
                    break;

                case SDLK_DOWN:
                    {
                        int curr_pos = g_frame_number.getFrameIdx();
                        g_frame_number.assignFrame(curr_pos - INTRO_FRAMERATE * 30);
                        g_frame_number.assignAudioPosition();
                    }
                    break;

                case SDLK_RIGHT:
                    {
                        int curr_pos = g_frame_number.getFrameIdx();
                        g_frame_number.assignFrame(curr_pos + INTRO_FRAMERATE * 5);
                        g_frame_number.assignAudioPosition();
                    }
                    break;

                case SDLK_UP:
                    {
                        int curr_pos = g_frame_number.getFrameIdx();
                        g_frame_number.assignFrame(curr_pos + INTRO_FRAMERATE * 30);
                        g_frame_number.assignAudioPosition();
                    }
                    break;

                case SDLK_m:
                    quit = true;
                    std::cout << generate_script_info();
                    break;

                case SDLK_v:
                    g_visual_debug = static_cast<int>(!static_cast<bool>(g_visual_debug));
                    break;

                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    move_speed = 2.0f / 5.0f;
                    g_time_mul = 8;
                    break;

                case SDLK_LALT:
                    g_time_delta = -1;
                    SDL_PauseAudio(1);
                    break;

                case SDLK_MODE:
                case SDLK_RALT:
                    g_time_delta = 1;
                    SDL_PauseAudio(1);
                    break;

                case SDLK_F5:
                    g_data.recreateShaders();
                    break;

                case SDLK_RETURN:
                    if(g_preview_mesh.empty())
                    {
                        g_pos.reset();
                    }
                    break;

                case SDLK_SPACE:
                    if(g_pos)
                    {
                        if(g_preview_mesh.empty())
                        {
                            std::cout << orientation_string(*g_pos + g_direction_pos, g_fw, g_up, g_fov) << std::endl;
                        }
                        else
                        {
                            std::cout << orientation_string(*g_pos, g_fw, g_up, g_fov) << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << orientation_string(g_direction_pos, g_direction_fw, g_direction_up, g_direction_fov) <<
                            std::endl;
                    }
                    std::cout << "ticks: " << g_frame_number.getFrameIdx() << std::endl;
                    break;

                case SDLK_COMMA:
                    if(g_seed)
                    {
                        quit = true;
                        *g_seed = *g_seed - 1;
                        std::cout << generate_script_info();
                    }
                    break;

                case SDLK_PERIOD:
                    if(g_seed)
                    {
                        quit = true;
                        *g_seed = *g_seed + 1;
                        std::cout << generate_script_info();
                    }
                    break;

                case SDLK_ESCAPE:
                    quit = true;
                    break;

                default:
                    break;
                }
            }
            else if(SDL_KEYUP == event.type)
            {
                switch(event.key.keysym.sym)
                {
                case SDLK_a:
                    move_left = 0;
                    break;

                case SDLK_d:
                    move_right = 0;
                    break;

                case SDLK_e:
                    move_up = 0;
                    break;

                case SDLK_q:
                    move_down = 0;
                    break;

                case SDLK_s:
                    move_backward = 0;
                    break;

                case SDLK_w:
                    move_forward = 0;
                    break;

                case SDLK_z:
                    fov_decrease = 0;
                    break;

                case SDLK_x:
                    fov_increase = 0;
                    break;

                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    move_speed = 1.0f / 60.0f;
                    g_time_mul = 1;
                    break;

                case SDLK_MODE:
                case SDLK_LALT:
                case SDLK_RALT:
                    g_time_delta = 0;
                    SDL_PauseAudio(1);
                    break;

                default:
                    break;
                }
            }
            else if(SDL_MOUSEBUTTONDOWN == event.type)
            {
                if(1 == event.button.button)
                {
                    mouse_look = 1;
                }
            }
            else if(SDL_MOUSEBUTTONUP == event.type)
            {
                if(1 == event.button.button)
                {
                    mouse_look = 0;
                }
            }
            else if(SDL_MOUSEMOTION == event.type)
            {
                if(0 != mouse_look)
                {
                    mouse_look_x += event.motion.xrel;
                    mouse_look_y += event.motion.yrel;

                    // Init lookat if empty.
                    if(!g_pos && g_preview_mesh.empty())
                    {
                        g_pos = vgl::vec3(0.0f, 0.0f, 0.0f);
                        g_fw = g_direction_fw;
                        g_up = g_direction_up;
                    }
                }
            }
        }

        if(g_flag_developer && g_pos)
        {
            float uplen = length(g_up);
            float fwlen = length(g_fw);
            float movement_rt = static_cast<float>(move_right - move_left) * move_speed;
            float movement_up = static_cast<float>(move_up - move_down) * move_speed;
            float movement_fw = static_cast<float>(move_forward - move_backward) * move_speed;

            g_up /= uplen;
            g_fw /= fwlen;

            vgl::vec3 rt = cross(g_fw, g_up);

            if(0 != mouse_look_x)
            {
                float angle = static_cast<float>(mouse_look_x) / static_cast<float>(g_screen_h / 4) * 0.25f;
                float ca = cosf(angle);
                float sa = sinf(angle);
                vgl::vec3 new_rt(ca * rt.x() + sa * g_fw.x(),
                        ca * rt.y() + sa * g_fw.y(),
                        ca * rt.z() + sa * g_fw.z());
                vgl::vec3 new_fw(ca * g_fw.x() - sa * rt.x(),
                        ca * g_fw.y() - sa * rt.y(),
                        ca * g_fw.z() - sa * rt.z());
                rt = new_rt;
                g_fw = new_fw;
            }
            if(0 != mouse_look_y)
            {
                float angle = static_cast<float>(mouse_look_y) / static_cast<float>(g_screen_h / 4) * 0.25f;
                float ca = cosf(angle);
                float sa = sinf(angle);
                vgl::vec3 new_fw(ca * g_fw.x() + sa * g_up.x(),
                        ca * g_fw.y() + sa * g_up.y(),
                        ca * g_fw.z() + sa * g_up.z());
                vgl::vec3 new_up(ca * g_up.x() - sa * g_fw.x(),
                        ca * g_up.y() - sa * g_fw.y(),
                        ca * g_up.z() - sa * g_fw.z());
                g_fw = new_fw;
                g_up = new_up;
            }

            *g_pos += (movement_rt * rt) + (movement_up * g_up) + (movement_fw * g_fw);
            g_fov = vgl::min(vgl::max(g_fov + static_cast<float>(fov_increase - fov_decrease) * 0.004f, 0.1f), 2.8f);
        }
#endif

        // If we're at least 3 frames behind, disable rendering for this frame.
        bool frameskip = false;
        {
            int tick_diff = get_current_ticks() - prev_ticks;
            if(tick_diff >= (FRAME_MILLISECONDS * 3))
            {
#if defined(USE_LD)
                std::cout << "frameskip(" << successful_frames << "): " << (tick_diff - (FRAME_MILLISECONDS * 3)) <<
                    std::endl;
                successful_frames = 0;
#endif
                frameskip = true;
            }
#if defined(USE_LD)
            else
            {
                ++successful_frames;
            }
#endif
        }

        // Execute main loop tasks until draw command is reached.
#if defined(USE_LD)
        int64_t tstart = g_frame_counter.get_timespec_timestamp();
#endif
        for(;;)
        {
            vgl::Task task = vgl::task_acquire_main();

            // If not draw, execute and continue.
            if(task.getFunc() != intro_state_draw)
            {
                task();
                continue;
            }

            // Skip draw if frameskip is on.
            if(!frameskip)
            {
                task();
#if defined(USE_LD)
                int64_t tmid = g_frame_counter.get_timespec_timestamp();
#endif
                swap_buffers();
#if defined(USE_LD)
                int64_t tend = g_frame_counter.get_timespec_timestamp();
                g_frame_counter.addFrame(tstart, tmid, tend);
#endif
            }
            break;
        }

        // If we've drawn and tick difference has not reached one frame, limit the framerate.
        for(;;)
        {
            int tick_diff = get_current_ticks() - prev_ticks;
            int remaining = FRAME_MILLISECONDS - tick_diff;
            if(remaining <= 0)
            {
                break;
            }
            // If remaining time is large enough, sleep a bit, otherwise just yield.
            if(remaining > 2)
            {
                dnload_SDL_Delay(1);
            }
            else
            {
                dnload_SDL_Delay(0);
            }
        }
        prev_ticks += FRAME_MILLISECONDS;

#if defined(USE_LD)
        if((g_frame_number.getFrameIdx() > static_cast<int>(INTRO_LENGTH)) || quit)
        {
            break;
        }
#else
        // Minified form - use audio to check for end condition instead of frames.
        dnload_SDL_PollEvent(&event);
        if((g_audio_position > INTRO_LENGTH_AUDIO) || (event.type == SDL_KEYDOWN))
        {
            break;
        }
#endif
    }

#if defined(USE_LD)
    // Wait until next loop so the parallel tasks are done.
    for(;;)
    {
        vgl::Task task = vgl::task_acquire_main();
        if(task.getFunc() == intro_state_move)
        {
            break;
        }
        dnload_SDL_Delay(1);
    }
#endif

    teardown();
#if !defined(USE_LD)
    asm_exit();
#endif
}

//######################################
// Main ################################
//######################################

#if defined(USE_LD)
/// Main function.
///
/// \param argc Argument count.
/// \param argv Arguments.
/// \return Program return code.
int DNLOAD_MAIN(int argc, char **argv)
{
    bool option_fullscreen = false;
    bool option_windowed = false;

    try
    {
        if(argc > 0)
        {
            po::options_description desc("Options");
            desc.add_options()
                ("camera,c", po::value<std::string>(), "Sets default camera location at intro start, 9 floating point values")
                ("developer,d", "Developer mode.")
                ("fullscreen,f", "Start in fullscreen as opposed to windowed mode.")
                ("help,h", "Print help text.")
                ("mesh,m", po::value<std::string>(), "Specify a mesh preview to view. Implies developer mode.")
                ("record-audio", "Do not play intro normally. Record audio as .raw -file.")
                ("record-video", "Do not play intro normally. Record video as .png -files.")
                ("record,R", "Do not play intro normally, instead record audio and video as files.")
                ("resolution,r", po::value<std::string>(), "Resolution to use, specify as 'WIDTHxHEIGHT' or 'HEIGHTp'.")
                ("seed,s", po::value<unsigned>(), "RNG seed, used when iterating generation settings.")
                ("ticks,t", po::value<int>(), "Timestamp to start from in frames.")
                ("window,w", "Start in windowed mode as opposed to fullscreen.");

            po::variables_map vmap;
            po::store(po::command_line_parser(argc, argv).options(desc).run(), vmap);
            po::notify(vmap);

            if(vmap.count("camera"))
            {
                std::vector<std::string> values;
                boost::split(values, vmap["camera"].as<std::string>(), boost::is_any_of(","));
                if(values.size() != 9)
                {
                    BOOST_THROW_EXCEPTION(std::runtime_error("camera settings requires exactly 9 floating point values"));
                }
                g_pos = vgl::vec3(std::stof(values[0], nullptr),
                        std::stof(values[1], nullptr),
                        std::stof(values[2], nullptr));
                g_fw = vgl::vec3(std::stof(values[3], nullptr),
                        std::stof(values[4], nullptr),
                        std::stof(values[5], nullptr));
                g_up = vgl::vec3(std::stof(values[6], nullptr),
                        std::stof(values[7], nullptr),
                        std::stof(values[8], nullptr));
            }
            if(vmap.count("seed"))
            {
                g_seed = vmap["seed"].as<unsigned>();
            }
            if(vmap.count("ticks"))
            {
                g_frame_number.assignFrame(vmap["ticks"].as<int>());
            }
            if(vmap.count("developer"))
            {
                g_flag_developer = true;
            }
            if(vmap.count("fullscreen"))
            {
                option_fullscreen = true;
            }
            if(vmap.count("help"))
            {
                std::cout << usage << desc << std::endl;
                return 0;
            }
            if(vmap.count("mesh"))
            {
                g_preview_mesh = vgl::string(vmap["mesh"].as<std::string>().c_str());
            }
            if(vmap.count("record"))
            {
                g_flag_record_audio = true;
                g_flag_record_video = true;
            }
            if (vmap.count("record-audio"))
            {
                g_flag_record_audio = true;
            }
            if (vmap.count("record-video"))
            {
                g_flag_record_video = true;
            }
            if(vmap.count("resolution"))
            {
                std::pair<unsigned, unsigned> resolution = parse_resolution(vmap["resolution"].as<std::string>());
                g_screen_w = static_cast<int>(resolution.first);
                g_screen_h = static_cast<int>(resolution.second);
            }
            if(vmap.count("window"))
            {
                option_windowed = true;
            }
        }

        // Enable developer mode if model view mode is on.
        if(!g_preview_mesh.empty())
        {
            // Default location if location hasn't been set.
            if(!g_pos)
            {
                g_pos = vgl::vec3(3.0f, 2.0f, 9.0f);
                g_fw = vgl::vec3(3.0f, 0.0f, -9.0f);
                g_up = vgl::vec3(0.0f, 1.0f, 0.0f);
            }
            g_flag_developer = true;
        }

        // Record mode cannot be enabled with developer mode.
        if((g_flag_record_audio || g_flag_record_video) && g_flag_developer)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("both developer mode and recording mode specified"));
            g_flag_developer = false;
        }

        // Fullscreen mode cannot be enabled with windowed mode.
        if(option_fullscreen && option_windowed)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("both windowed mode and fullscreen mode specified"));
        }
        bool fullscreen = option_fullscreen ? true :
            (option_windowed ? false :
             (!g_flag_developer && !(g_flag_record_audio || g_flag_record_video)));

        intro(fullscreen);
    }
    catch(const boost::exception &err)
    {
        std::cerr << boost::diagnostic_information(err);
        return 1;
    }
    return 0;
}
#endif

//######################################
// End #################################
//######################################

