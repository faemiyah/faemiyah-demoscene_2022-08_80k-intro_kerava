#ifndef INTRO_DATA_HPP
#define INTRO_DATA_HPP

#include "audio_samples.hpp"
#include "intro_world.hpp"

#if defined(USE_LD)
#include "FLAC/stream_decoder.h"
#endif

#if !defined(DISABLE_SYNTH) || !DISABLE_SYNTH
#include "synth/verbatim_synth.hpp"
#endif

#if defined(USE_LD)

/// Update a CSG header file with data.
///
/// \param filename Filename to update.
/// \param data Data to update with.
void csg_update(std::string_view filename, const vgl::vector<int16_t>& data)
{
    // Do not update unless in developer mode.
    if(!g_flag_developer)
    {
        return;
    }

    // Opening the CSG file is allowed to fail.
    vgl::CsgFile csg_file(filename);
    if(csg_file)
    {
        unsigned written = csg_file.update(data.data(), data.size());
        if(written > 0)
        {
            std::cout << "CSG data " << csg_file.getFilename() << " written: " << human_readable_memory(written) << std::endl;
        }
    }
}

#else

/// Read CSG data from a sized array and create a vector.
///
/// \param data Source array.
/// \param count Source array.
/// \return Constructed vector.
vgl::vector<int16_t> csg_read(const int16_t* data, unsigned count)
{
    vgl::vector<int16_t> ret;

    while(count)
    {
        ret.push_back(*data);
        ++data;
        --count;
    }

    return ret;
}

/// Helper macro for reading CSG data.
#define CSG_READ_HPP(op) csg_read(op, op ## _size);

#endif

/// Callback for landmarks
///
/// \param queue Queue to push rendered meshes to.
/// \param program Program to render the mesh with.
/// \param mesh Mesh to be rendered.
/// \param transform Transformation to render with.
void landmark_callback(vgl::RenderQueue& queue, const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::mat4& transform);

/// Sign position data struct.
struct SignPosition
{
public:
    /// Position data.
    ///
    /// Elements:
    /// - X, Y, Z (x10)
    /// - Sign index.
    /// - Text offset X, Y (x10)
    /// - Font size (x10)
    ///
    /// Text Z is 0.1 offset from sign Z. Construct sign meshes accordingly.
    int16_t m_data[7];

    /// Sign text.
    const char* m_text;
};

/// Individual sign positions.
const SignPosition g_sign_positions[] =
{
    { { 130, 32, -1810, 0, -25, 9, 7, }, "Ali-Kerava 9" },
    { { -136, 62, -2794, 1, -23, 9, 7, }, "Yli-Kerava 7" },
    { { -130, 53, -5200, 3, -19, 1, 13, }, "Xoff\n  III" },
    { { 156, 52, -7540, 2, -56, 13, 13, }, "Mega-Kerava 1" },
    { { 0, 0, 0, 0, 0, 0, 0, }, nullptr },
};

/// Sign easing data.
///
/// Calculated per-frame to construct the camera path.
struct SignEasing
{
public:
    /// Z position this frame.
    float m_z_pos;

    /// Easign target to look at.
    vgl::vec3 m_easing_target;

    /// Easing strength to look at.
    float m_easing_strength;

public:
    /// Constructor.
    ///
    /// \param zpos Z Position.
    /// \param target Easing target,
    /// \param strength Easing strength.
    explicit SignEasing(float zpos, const vgl::vec3& target, float strength) :
        m_z_pos(zpos),
        m_easing_target(target),
        m_easing_strength(strength)
    {
    }
};

/// Persistent intro data storage.
class IntroData
{
private:
#if defined(USE_LD)
    /// FLAC write status.
    struct FlacWriteStatus
    {
    public:
        /// Constructor.
        /// \param dst Target pointer.
        /// \param pos Position.
        explicit FlacWriteStatus(void* dst) noexcept :
            m_ptr(dst)
        {
        }

    public:
        /// Write a floating-point sample.
        ///
        /// \param op Sample.
        void write(float op)
        {
            float* fptr = static_cast<float*>(m_ptr);
            *fptr = op;
            ++fptr;
            m_ptr = fptr;
        }

    public:
        /// Internal write pointer.
        void* m_ptr;
    };
#endif

public:
    /// Audio sample count.
    static const unsigned AUDIO_SAMPLE_COUNT = sizeof(g_sample_sizes) / sizeof(g_sample_sizes[0]);

    /// Stipple texture size.
    static const unsigned STIPPLE_SIZE = 4;

    /// Building count.
    static const int BUILDING_COUNT = 9;

    /// Last Z position to insert entities when looping.
    static const int TERRAIN_LAST_ZPOS = -16000;
    /// Terrain block count.
    static const int TERRAIN_BLOCK_COUNT = 25;
    /// Terrain block width.
    static constexpr float TERRAIN_BLOCK_WIDTH = 512.0f;
    /// Terrain block length.
    static constexpr float TERRAIN_BLOCK_LENGTH = 52.0f;
    /// Terrain tile jitter.
    static constexpr float TERRAIN_TILE_JITTER = 0.17f;
    /// Horizontal slope end.
    static constexpr float TERRAIN_SLOPE_START = 9.0f;
    /// Horizontal slope end.
    static constexpr float TERRAIN_SLOPE_END = 11.0f;
    /// Middle bump end.
    static constexpr float TERRAIN_BUMP_START = 7.5f;
    /// Middle bump end.
    static constexpr float TERRAIN_BUMP_END = 8.0f;
    /// Middle bump height.
    static constexpr float TERRAIN_BUMP_HEIGHT = 0.5f;
    /// Middle noise level.
    static constexpr float TERRAIN_BUMP_NOISE = 0.25f;
    /// Pylon offset (from some location).
    static constexpr float RAILS_OFFSET = 4.3f;
    /// Pylon X offset.
    static constexpr float PYLON_X = TERRAIN_SLOPE_START - 1.1f;
    /// Rail X offset.
    static constexpr float RAILS_X = PYLON_X - RAILS_OFFSET;
    /// Rail Y offset.
    static constexpr float RAILS_Y = 0.6f;
    /// Number of signs.
    /// 0: Traditional roadside sign, 2 posts below.
    /// 1: Hanging sign, 2 posts up.
    /// 2: Mega-kerava sign.
    /// 3: Xoff III can.
    static constexpr unsigned SIGN_COUNT = 4;
    /// Number of terrain blocks visible at a time.
    static constexpr unsigned TERRAIN_BLOCKS_VISIBLE = 7;

    /// Number of visualization elements for the visualization stripe.
    /// The number here is synchronized to display one frame of audio, 1/50 of 44100Hz.
    static const int16_t VISUALIZATION_ELEMENTS = 44100 / 50;
    /// Width of one element in audio visualization block.
    static const int16_t VISUALIZATION_ELEMENT_WIDTH = 1;
    /// Width of whole visualization.
    static const int16_t VISUALIZATION_WIDTH = static_cast<int16_t>(VISUALIZATION_ELEMENTS * VISUALIZATION_ELEMENT_WIDTH);
    /// Multiplier for visualization offset for thread function.
    static constexpr float VISUALIZATION_MULTIPLIER_WAVE = 6.0f;
    /// Height of visualization block for the thread function (both directions).
    static constexpr float VISUALIZATION_HEIGHT_WAVE = 0.4f;
    /// Width of visualization block (transform scale).
    static constexpr float VISUALIZATION_WIDTH_WAVE = 24.0f;
    /// Multiplier for visualization offset for thread function.
    static constexpr float VISUALIZATION_MULTIPLIER_FFT = 5.0f;
    /// Width of visualization block (transform scale).
    static constexpr float VISUALIZATION_WIDTH_FFT = 36.0f;

    /// Sign follow path.
    vgl::vector<SignEasing> m_sign_easing;

    /// Camera jitter noise.
    vgl::vector<vgl::vec3> m_camera_jitter;

private:
    /// Audio samples.
    vgl::vector<float> m_samples[AUDIO_SAMPLE_COUNT];

    /// Framebuffer.
    vgl::FrameBufferUptr m_fbo;

    /// Stipple texture.
    vgl::Texture2DUptr m_texture_stipple;

    /// Program.
    vgl::GlslProgram m_program_font;
    /// Program.
    vgl::GlslProgram m_program_font_overlay;
    /// Program.
    vgl::GlslProgram m_program_offscreen;
    /// Program.
    vgl::GlslProgram m_program_skeleton;
    /// Program.
    vgl::GlslProgram m_program_visualization;
    /// Program.
    vgl::GlslProgram m_program_post;

    /// Font.
    vgl::FontUptr m_font;

    /// Mesh.
    vgl::MeshUptr m_mesh_quad;
    /// Mesh.
    vgl::array<vgl::MeshUptr, WORLD_GLYPH_SLICE_COUNT> m_mesh_glyph;
    /// Glyph mesh slices.
    GlyphMeshArray m_glyph;
    /// Mesh.
    vgl::MeshUptr m_mesh_fence;
    /// Mesh.
    vgl::MeshUptr m_mesh_pylon;
    /// Mesh.
    vgl::MeshUptr m_mesh_pylon_extra_base;
    /// Mesh.
    vgl::MeshUptr m_mesh_arc;
    /// Mesh.
    vgl::MeshUptr m_mesh_tendons;
    /// Mesh.
    vgl::MeshUptr m_mesh_lamppost;
    /// Mesh.
    vgl::MeshUptr m_mesh_bridge;
    /// Mesh.
    vgl::MeshUptr m_mesh_rails;

    /// Visualization mesh array.
    /// 0: Waveform.
    /// 1: FFT.
    vgl::array<vgl::MeshUptr, 2> m_mesh_visualization;
    /// FFT input and output data.
    vgl::array<vgl::vector<double>, 2> m_data_fft;
    /// Levels data.
    /// Calculated from FFT data.
    vgl::vector<float> m_data_levels;

#if defined(ENABLE_CHARTS) && ENABLE_CHARTS
    /// Chart mesh array.
    /// 0: Malmi.
    /// 1: Tapanila.
    /// 2: Puistola, Tikkurila.
    /// 3: Hiekkaharju.
    /// 4: Rekola.
    /// 5: Korso.
    vgl::array<vgl::MeshUptr, 6> m_mesh_chart;
#endif

    /// Mesh.
    vgl::MeshUptr m_mesh_sm5_interior;
    /// Train chair array.
    /// 0: Left-handed.
    /// 1: No handholds.
    /// 2: Right-handed.
    vgl::array<vgl::MeshUptr, 3> m_mesh_sm5_chair;
    /// Train mesh array.
    /// 0: Veturi.
    /// 1: Vaunu.
    vgl::array<vgl::MeshUptr, 2> m_mesh_train;

    /// Mesh.
    vgl::MeshUptr m_mesh_katos;
    /// Mesh.
    vgl::MeshUptr m_mesh_tower;
    /// Mesh.
    vgl::MeshUptr m_mesh_ramp;
    /// Mesh.
    vgl::MeshUptr m_mesh_kerava_station;
    /// Mesh.
    vgl::MeshUptr m_mesh_station_building;

    /// Sign mesh array.
    /// 0: Standing sign.
    /// 1: Hanging sign.
    /// 2: Big sign.
    /// 3: Xoff can sign.
    vgl::array<vgl::MeshUptr, SIGN_COUNT> m_mesh_sign;

    /// Generic Mega-Kerava building array.
    vgl::array<vgl::MeshUptr, BUILDING_COUNT> m_mesh_building;
    /// Specific Mega-Kerava building.
    vgl::MeshUptr m_mesh_kerava_state_building;
    /// Specific Mega-Kerava building.
    vgl::MeshUptr m_mesh_burj_kerava;
    /// Specific Mega-Kerava building.
    vgl::MeshUptr m_mesh_john_kerava_center;
    /// Specific Mega-Kerava building.
    vgl::MeshUptr m_mesh_keravanas_towers;

    /// Terrain mesh array.
    vgl::array<vgl::MeshUptr, TERRAIN_BLOCK_COUNT> m_mesh_terrain;
    /// Terrain mesh.
    vgl::MeshUptr m_mesh_terrain_kerava;
    /// Terrain mesh.
    vgl::MeshUptr m_mesh_terrain_sirkus_hevo_set;

    /// Skinned mesh.
    vgl::MeshUptr m_mesh_ukko;
    /// Skinning animation.
    vgl::array<vgl::AnimationUptr, 7> m_animation_ukko;
    /// Static mesh.
    vgl::MeshUptr m_mesh_sirkus_hevo_nen;

    /// Worlds.
    /// 0: Train carriage.
    /// 1: First-person track.
    /// 2: Signs on first-person track.
    /// 3: Kerava station.
    /// 4: Sirkus hevo set.
    vgl::array<IntroWorld, 5> m_world;

#if defined(USE_LD)
    /// Meshes for preview.
    vgl::vector<vgl::Mesh*> m_preview_meshes;
#endif

public:
    /// Default constructor.
    explicit IntroData() :
        m_data_levels(VISUALIZATION_ELEMENTS)
    {
    }

private:
    /// Initialize samples.
    void initializeSamples()
    {
        vgl::vector<float> sample_data = vgl::opus_read_raw_memory(g_sample_data, sizeof(g_sample_data), 1, 312);

        unsigned read_pos = 0;
        for(unsigned ii = 0; (ii < AUDIO_SAMPLE_COUNT); ++ii)
        {
            vgl::vector<float>& sample = m_samples[ii];
            unsigned sample_length = g_sample_sizes[ii];
            sample.resize(sample_length);
            vgl::detail::internal_memcpy(sample.data(), sample_data.data() + read_pos,
                    static_cast<unsigned>(sample_length * sizeof(float)));
            read_pos += sample_length;
        }
    }

    /// Initialize data for audio levels, per frame.
    ///
    /// Calculated from FFT data.
    void initializeLevelData()
    {
        const unsigned ELEMENT_DIVISION = IntroData::VISUALIZATION_ELEMENTS / 3;
        const float ELEMENT_DIVISION_MUL = 1.0f / static_cast<float>(ELEMENT_DIVISION);

        vgl::vector<float> level_data;

        for(int ii = 0; (ii < INTRO_LENGTH); ++ii)
        {
            evaluateFFT(ii);
            const double* fft_data = getDataFFT(1);

            // Clear level data array.
            float levels[3] =
            {
                0.0f,
                0.0f,
                0.0f,
            };

            for(unsigned jj = 0; (jj < IntroData::VISUALIZATION_ELEMENTS); ++jj)
            {
                float value = static_cast<float>(fft_data[jj]);

                // Write to levels.
                unsigned level_idx = jj / ELEMENT_DIVISION;
                levels[level_idx] += vgl::abs(value);
            }

            level_data.push_back(levels[0] * ELEMENT_DIVISION_MUL);
            level_data.push_back(levels[1] * ELEMENT_DIVISION_MUL);
            level_data.push_back(levels[2] * ELEMENT_DIVISION_MUL);
        }

        // Do moving average over the levels.
        const int AVERAGE_AREA = 2;
        for(int ii = 0; (ii < INTRO_LENGTH); ++ii)
        {
            for(int jj = 0; (jj < 3); ++jj)
            {
                float sum = 0.0f;

                for(int kk = -AVERAGE_AREA; (kk <= AVERAGE_AREA); ++kk)
                {
                    int base_idx = ((ii + kk) * 3) + jj;

                    if((base_idx >= 0) && (base_idx < static_cast<int>(level_data.size())))
                    {
                        sum += level_data[base_idx];
                    }
                }

                m_data_levels.push_back(sum / static_cast<float>((AVERAGE_AREA * 2) + 1));
            }
        }
    }

#if defined(SAMPLE_TEST) && SAMPLE_TEST
    /// Test functionality to copy samples to the beginning of audio buffer.
    void initializeAudioSampleTest()
    {
        unsigned outpos = 0;
        for(unsigned jj = 0; (jj < AUDIO_SAMPLE_COUNT); ++jj)
        {
            vgl::vector<float>& sample = m_samples[jj];
            for(unsigned ii = 0; (ii < sample.size()); ++ii)
            {
                reinterpret_cast<float*>(g_audio_buffer)[outpos + 0] = sample[ii];
                reinterpret_cast<float*>(g_audio_buffer)[outpos + 1] = sample[ii];
                outpos += 2;
            }
        }
    }
#endif

    /// Initialize audio (generate).
    void initializeAudioGenerate()
    {
        initializeSamples();

#if defined(DISABLE_SYNTH) && DISABLE_SYNTH
        // Fallback to debug audio.
        for(unsigned ii = 0;
                ((INTRO_LENGTH_AUDIO / sizeof(float) / AUDIO_CHANNELS) > ii);
                ++ii)
        {
            // Example by "bst", taken from "Music from very short programs - the 3rd iteration" by viznut.
            unsigned input_value = (ii * 8000) / 44100;
            uint8_t audio_value = static_cast<uint8_t>(
                    static_cast<int>(input_value / 70000000 * input_value * input_value + input_value) % 127 |
                    input_value >> 4 | input_value >> 5 | (input_value % 127 + (input_value >> 17)) | input_value);
            float output_value = static_cast<float>(audio_value) * (2.0f / 255.0f) - 1.0f;
            void* audio_buffer = g_audio_buffer;
            float* audio_output = reinterpret_cast<float*>(audio_buffer);
            audio_output[ii * AUDIO_CHANNELS + 0] = output_value;
            audio_output[ii * AUDIO_CHANNELS + 1] = output_value;
        }
#else
        float progress = 0.0f;
        vgl::detail::internal_memset(g_audio_buffer, 0, INTRO_LENGTH_AUDIO);
        void* void_audio_buffer = static_cast<void*>(g_audio_buffer);
        generate_audio(static_cast<float*>(void_audio_buffer), INTRO_LENGTH_AUDIO, m_samples, progress);
#endif

#if defined(SAMPLE_TEST) && SAMPLE_TEST
        initializeAudioSampleTest();
#endif
        initializeLevelData();
    }

#if defined(USE_LD)
    /// Initialize audio (load).
    void initializeAudioLoad()
    {
        std::string_view flac_filename("kerava.flac");
        boost::filesystem::path fname = vgl::find_file(flac_filename);
        if(fname.empty())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("could not locate '" + std::string(flac_filename) + "'"));
        }

        // Initialize decoder.
        FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
        if(!decoder)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("could not create FLAC decoder"));
        }
        FLAC__stream_decoder_set_md5_checking(decoder, true);

        // Initialize write target.
        FlacWriteStatus write_status(g_audio_buffer);

        // Attempt to decode.
        FLAC__StreamDecoderInitStatus init_status = FLAC__stream_decoder_init_file(decoder, fname.string().c_str(),
                flac_write_callback, flac_metadata_callback, flac_error_callback, &write_status);
        if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            std::cout << "decode fail\n";
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("FLAC__stream_decoder_init_file: ") +
                        FLAC__StreamDecoderInitStatusString[init_status]));
        }

        if(!FLAC__stream_decoder_process_until_end_of_stream(decoder))
        {
            FLAC__StreamDecoderState decoder_state = FLAC__stream_decoder_get_state(decoder);
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("FLAC__stream_decoder_process_until_end_of_stream: ") +
                        FLAC__StreamDecoderStateString[decoder_state]));
        }

        // Decode successful.
        FLAC__stream_decoder_delete(decoder);

#if defined(SAMPLE_TEST) && SAMPLE_TEST
        initializeSamples();
        initializeAudioSampleTest();
#endif
        initializeLevelData();
    }
#endif

    /// Create GL objects that can be created immediately.
    void initializeGraphicsImmediate()
    {
        // Create framebuffer.
        {
            const vgl::FrameBuffer& screen = vgl::FrameBuffer::get_default();
            vgl::TextureFormatColor colorFormat(4, 1, nullptr);
            vgl::TextureFormatDepth depthFormat(4);
            m_fbo = vgl::FrameBuffer::create(screen.getWidth(), screen.getHeight(), colorFormat, depthFormat);
        }

        // Create programs.
        {
            m_program_font = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_font),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_font));
            m_program_font.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_font_attribute_position);
            m_program_font.addAttribute(vgl::GeometryChannel::NORMAL, g_shader_vertex_font_attribute_normal);
            m_program_font.addAttribute(vgl::GeometryChannel::TEXCOORD, g_shader_vertex_font_attribute_texcoord);
            m_program_font.addUniform(vgl::UniformSemantic::CAMERA_POSITION,
                    g_shader_vertex_font_uniform_camera_position);
            m_program_font.addUniform(vgl::UniformSemantic::MODELVIEW_MATRIX,
                    g_shader_vertex_font_uniform_modelview_transform);
            m_program_font.addUniform(vgl::UniformSemantic::NORMAL_MATRIX,
                    g_shader_vertex_font_uniform_normal_transform);
            m_program_font.addUniform(vgl::UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX,
                    g_shader_vertex_font_uniform_combined_transform);
            m_program_font.addUniform(vgl::UniformSemantic::PROJECTION_RANGE,
                    g_shader_fragment_font_uniform_projection_range);
            m_program_font.addUniform(vgl::UniformSemantic::GLYPH_TEXTURE,
                    g_shader_fragment_font_uniform_glyph);
            m_program_font.addUniform(vgl::UniformSemantic::GLYPH_POSITION,
                    g_shader_vertex_font_uniform_glyph_position);
            m_program_font.addUniform(vgl::UniformSemantic::GLYPH_QUAD,
                    g_shader_vertex_font_uniform_glyph_quad);
#if defined(USE_LD)
            m_program_font.addUniform("debug_mode");
#endif

            m_program_font_overlay = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_font_overlay),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_font_overlay));
            m_program_font_overlay.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_font_overlay_attribute_position);
            m_program_font_overlay.addAttribute(vgl::GeometryChannel::TEXCOORD, g_shader_vertex_font_overlay_attribute_texcoord);
            m_program_font_overlay.addUniform(vgl::UniformSemantic::MODELVIEW_MATRIX,
                    g_shader_vertex_font_overlay_uniform_modelview_transform);
            m_program_font_overlay.addUniform(vgl::UniformSemantic::GLYPH_TEXTURE,
                    g_shader_fragment_font_overlay_uniform_glyph);
            m_program_font_overlay.addUniform(vgl::UniformSemantic::GLYPH_POSITION,
                    g_shader_vertex_font_overlay_uniform_glyph_position);
            m_program_font_overlay.addUniform(vgl::UniformSemantic::GLYPH_QUAD,
                    g_shader_vertex_font_overlay_uniform_glyph_quad);
            m_program_font_overlay.addUniform(g_shader_vertex_font_overlay_uniform_scoords);
            m_program_font_overlay.addUniform(g_shader_fragment_font_overlay_uniform_stipple);

            m_program_offscreen = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_offscreen),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_offscreen));
            m_program_offscreen.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_offscreen_attribute_position);
            m_program_offscreen.addAttribute(vgl::GeometryChannel::NORMAL, g_shader_vertex_offscreen_attribute_normal);
            m_program_offscreen.addUniform(vgl::UniformSemantic::CAMERA_POSITION,
                    g_shader_vertex_offscreen_uniform_camera_position);
            m_program_offscreen.addUniform(vgl::UniformSemantic::MODELVIEW_MATRIX,
                    g_shader_vertex_offscreen_uniform_modelview_transform);
            m_program_offscreen.addUniform(vgl::UniformSemantic::NORMAL_MATRIX,
                    g_shader_vertex_offscreen_uniform_normal_transform);
            m_program_offscreen.addUniform(vgl::UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX,
                    g_shader_vertex_offscreen_uniform_combined_transform);
            m_program_offscreen.addUniform(vgl::UniformSemantic::PROJECTION_RANGE,
                    g_shader_fragment_offscreen_uniform_projection_range);
#if defined(USE_LD)
            m_program_offscreen.addUniform("debug_mode");
#endif

            m_program_skeleton = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_skeleton),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_skeleton));
            m_program_skeleton.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_skeleton_attribute_position);
            m_program_skeleton.addAttribute(vgl::GeometryChannel::NORMAL, g_shader_vertex_skeleton_attribute_normal);
            m_program_skeleton.addAttribute(vgl::GeometryChannel::BONE_WEIGHT, g_shader_vertex_skeleton_attribute_bone_weight);
            m_program_skeleton.addAttribute(vgl::GeometryChannel::BONE_REF, g_shader_vertex_skeleton_attribute_bone_ref);
            m_program_skeleton.addUniform(vgl::UniformSemantic::CAMERA_POSITION,
                    g_shader_vertex_skeleton_uniform_camera_position);
            m_program_skeleton.addUniform(vgl::UniformSemantic::MODELVIEW_MATRIX,
                    g_shader_vertex_skeleton_uniform_modelview_transform);
            m_program_skeleton.addUniform(vgl::UniformSemantic::NORMAL_MATRIX,
                    g_shader_vertex_skeleton_uniform_normal_transform);
            m_program_skeleton.addUniform(vgl::UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX,
                    g_shader_vertex_skeleton_uniform_combined_transform);
            m_program_skeleton.addUniform(vgl::UniformSemantic::SKELETON,
                    g_shader_vertex_skeleton_uniform_skeleton);
            m_program_skeleton.addUniform(vgl::UniformSemantic::PROJECTION_RANGE,
                    g_shader_fragment_skeleton_uniform_projection_range);
#if defined(USE_LD)
            m_program_skeleton.addUniform("debug_mode");
#endif

            m_program_visualization = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_visualization),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_visualization));
            m_program_visualization.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_visualization_attribute_position);
            m_program_visualization.addUniform(vgl::UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX,
                    g_shader_vertex_visualization_uniform_combined_transform);
            m_program_visualization.addUniform(g_shader_fragment_visualization_uniform_color);
#if defined(USE_LD)
            m_program_visualization.addUniform("debug_mode");
#endif

            m_program_post = vgl::GlslProgram(
                    vgl::GlslShader(GL_VERTEX_SHADER, g_shader_header, g_shader_vertex_post),
                    vgl::GlslShader(GL_FRAGMENT_SHADER, g_shader_header, g_shader_fragment_post));
            m_program_post.addAttribute(vgl::GeometryChannel::POSITION, g_shader_vertex_post_attribute_position);
            m_program_post.addAttribute(vgl::GeometryChannel::TEXCOORD, g_shader_vertex_post_attribute_texcoord);
            m_program_post.addUniform(g_shader_vertex_post_uniform_scoords);
            m_program_post.addUniform(g_shader_fragment_post_uniform_color);
#if !defined(VGL_DISABLE_DEPTH_TEXTURE)
            m_program_post.addUniform(g_shader_fragment_post_uniform_depth);
#endif
            m_program_post.addUniform(g_shader_fragment_post_uniform_stipple);
#if defined(USE_LD)
            m_program_post.addUniform("debug_mode");
#endif
        }

        // Create the stipple texture.
        {
            vgl::Image2DGray stipple(STIPPLE_SIZE, STIPPLE_SIZE);
            for(unsigned ii = 0; (ii < stipple.getWidth()); ++ii)
            {
                for(unsigned jj = 0; (jj < stipple.getHeight()); ++jj)
                {
                    float value = 1.0f;

                    if((ii == 0) && (jj == 0))
                    {
                        value = 0.2f;
                    }
                    else if((ii == 2) && (jj == 2))
                    {
                        value = 0.4f;
                    }
                    else if(((ii % 2) == 0) && ((jj % 2) == 0))
                    {
                        value = 0.6f;
                    }
                    else if(((ii % 2) == 1) && ((jj % 2) == 1))
                    {
                        value = 0.8f;
                    }

                    stipple.setPixel(ii, jj, value);
                }
            }
            m_texture_stipple = vgl::Texture2D::create(stipple, 1, vgl::FilteringMode::NEAREST);
        }

        // Load font.
        {
            static const char *g_font_options[] =
            {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", // (Hardkernel Ubuntu)
#if defined(USE_LD)
                "/usr/local/share/fonts/dejavu/DejaVuSans-Bold.ttf", // (FreeBSD)
                "DejaVuSans-Bold.ttf", // (local copy found by lookup)
#endif
                NULL
            };
            m_font = vgl::Font::create(64, g_font_options);
            // Create ASCII7 characters.
            for(unsigned ii = static_cast<unsigned>(' '); (static_cast<unsigned>('z') >= ii); ++ii)
            {
                m_font->createCharacter(ii);
            }
        }
    }

    /// Initialize the GPU-related data.
    void initializeGraphics()
    {
        // Screen quad.
        {
            static const int16_t data[] =
            {
                to_int16(vgl::CsgCommand::VERTEX),
                -100, -100, 0,
                to_int16(vgl::CsgCommand::VERTEX),
                100, -100, 0,
                to_int16(vgl::CsgCommand::VERTEX),
                100, 100, 0,
                to_int16(vgl::CsgCommand::VERTEX),
                -100, 100, 0,
                to_int16(vgl::CsgCommand::QUAD_TC),
                0, 0, 0,
                1, 100, 0,
                2, 100, 100,
                3, 0, 100,
                to_int16(vgl::CsgCommand::NONE), 0
            };
            vgl::LogicalMesh lmesh(data);
            m_mesh_quad = lmesh.compile();
#if defined(USE_LD)
            addPreviewMesh("quad", *m_mesh_quad);
#endif
        }

        // Font quad.
        {
            auto generate_glyph_mesh = [](int16_t y1, int16_t y2, int16_t s1, int16_t s2)
            {
                vgl::vector<int16_t> data;

                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(0);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(100);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(100);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(0);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::QUAD_TC));
                data.push_back(0);
                data.push_back(0);
                data.push_back(s1);
                data.push_back(1);
                data.push_back(100);
                data.push_back(s1);
                data.push_back(2);
                data.push_back(100);
                data.push_back(s2);
                data.push_back(3);
                data.push_back(0);
                data.push_back(s2);
                data.push_back(to_int16(vgl::CsgCommand::NONE));

                vgl::LogicalMesh lmesh(data.data());
                return lmesh.compile();
            };
            m_mesh_glyph[0] = generate_glyph_mesh(0, 10, 0, 10);
            m_mesh_glyph[1] = generate_glyph_mesh(10, 20, 10, 20);
            m_mesh_glyph[2] = generate_glyph_mesh(20, 30, 20, 30);
            m_mesh_glyph[3] = generate_glyph_mesh(30, 50, 30, 50);
            m_mesh_glyph[4] = generate_glyph_mesh(50, 70, 50, 70);
            m_mesh_glyph[5] = generate_glyph_mesh(70, 80, 70, 80);
            m_mesh_glyph[6] = generate_glyph_mesh(80, 90, 80, 90);
            m_mesh_glyph[7] = generate_glyph_mesh(90, 100, 90, 100);
#if defined(USE_LD)
            addPreviewMesh("glyph0", *m_mesh_glyph[0]);
            addPreviewMesh("glyph1", *m_mesh_glyph[1]);
            addPreviewMesh("glyph2", *m_mesh_glyph[2]);
            addPreviewMesh("glyph3", *m_mesh_glyph[3]);
            addPreviewMesh("glyph4", *m_mesh_glyph[4]);
            addPreviewMesh("glyph5", *m_mesh_glyph[5]);
            addPreviewMesh("glyph6", *m_mesh_glyph[6]);
            addPreviewMesh("glyph7", *m_mesh_glyph[7]);
#endif
            // Copy mesh pointers for easy access.
            for(unsigned ii = 0; (ii < WORLD_GLYPH_SLICE_COUNT); ++ii)
            {
                m_glyph[ii] = m_mesh_glyph[ii].get();
            }
        }

        // Audio visualization stripe.
        {
            const int16_t STRIPE_XSTART = static_cast<int16_t>(-(VISUALIZATION_ELEMENTS / 2) * VISUALIZATION_ELEMENT_WIDTH);
            const int16_t STRIPE_XEND = static_cast<int16_t>((VISUALIZATION_ELEMENTS / 2) * VISUALIZATION_ELEMENT_WIDTH);

            vgl::vector<int16_t> data;

            // Create simple stripes into the data.
            // The Y values will be overwritten before the subdata is flushed to the GPU every frame.
            int16_t jj = 0;
            for(int16_t ii = STRIPE_XSTART; (ii < STRIPE_XEND); ii = static_cast<int16_t>(ii + VISUALIZATION_ELEMENT_WIDTH))
            {
                int16_t x1 = ii;
                int16_t x2 = static_cast<int16_t>(ii + VISUALIZATION_ELEMENT_WIDTH);
                int16_t y1 = -100;
                int16_t y2 = 100;

                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(x1);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(x2);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(x2);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                data.push_back(x1);
                data.push_back(y2);
                data.push_back(0);

                data.push_back(to_int16(vgl::CsgCommand::QUAD));
                data.push_back(static_cast<int16_t>(jj + 0));
                data.push_back(static_cast<int16_t>(jj + 1));
                data.push_back(static_cast<int16_t>(jj + 2));
                data.push_back(static_cast<int16_t>(jj + 3));
                jj = static_cast<int16_t>(jj + 4);
            }

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            vgl::LogicalMesh lmesh(data.data());
            m_mesh_visualization[0] = lmesh.compile(false);
            m_mesh_visualization[1] = lmesh.compile(false);
#if defined(USE_LD)
            addPreviewMesh("visualization0", *m_mesh_visualization[0]);
            addPreviewMesh("visualization1", *m_mesh_visualization[1]);
#endif
        }

#if defined(ENABLE_CHARTS) && ENABLE_CHARTS
        // Station charts.
        {
            const int CHART_START = 0;
            const int CHART_END = 1000;
            const int CHART_WIDTH = 30;
            const int CHART_INCREMENT = 20;
            const int CHART_STATION_START = 440;
            const int CHART_STATION_END = 560;
            const int CHART_STATION_WIDTH = 80;
            const int CHART_STATION_INCREMENT = 1;

            // Generator function for the 2D rail chart with station sphere.
            auto generate_chart = [](const int16_t* input_data, unsigned input_count) -> vgl::MeshUptr
            {
                // Generate spline first.
                // Spline points are in XY plane only.
                vgl::Spline spline;
                for(unsigned it = 0; (it < input_count); it += 2)
                {
                    int16_t px = input_data[it + 0];
                    int16_t py = input_data[it + 1];
                    int16_t tdiff = 0;
                    if(it < (input_count - 2))
                    {
                        tdiff = static_cast<int16_t>(input_data[it + 3] - py);
#if defined(USE_LD)
                        if(tdiff <= 0)
                        {
                            BOOST_THROW_EXCEPTION(std::runtime_error("chart spline not monotonically increasing"));
                        }
#endif
                    }
                    spline.addPoint(px, py, static_cast<int16_t>(0), tdiff);
                }

                vgl::vector<int16_t> data;
                vgl::optional<vgl::vec3> prev;
                int16_t vidx = 0;

                for(int it = CHART_START; (it <= CHART_END);)
                {
                    float width = static_cast<float>(CHART_WIDTH);
                    int next_it = it + CHART_INCREMENT;
                    if((it >= CHART_STATION_START) && (it < CHART_STATION_END))
                    {
                        float ratio = static_cast<float>(it - CHART_STATION_START) /
                            static_cast<float>(CHART_STATION_END - CHART_STATION_START);
                        ratio = vgl::abs(ratio * 2.0f - 1.0f);
                        ratio = 1.0f - ratio * ratio * ratio;
                        /*width = vgl::max(vgl::sin(ratio * static_cast<float>(M_PI)) * static_cast<float>(CHART_STATION_WIDTH),
                                width);*/
                        width = vgl::max(ratio * static_cast<float>(CHART_STATION_WIDTH), width);
                        next_it = it + CHART_STATION_INCREMENT;
                    }

                    vgl::vec3 curr = spline.resolvePosition(it);
                    if(prev)
                    {
                        vgl::vec3 fw = vgl::normalize(curr - *prev);
                        vgl::vec3 rt = vgl::normalize(cross(fw, vgl::vec3(0.0f, 0.0f, 1.0f))) * width;
                        vgl::vec3 lt = curr - rt;
                        rt = curr + rt;

                        data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                        data.push_back(static_cast<int16_t>(lt[0]));
                        data.push_back(static_cast<int16_t>(lt[1]));
                        data.push_back(static_cast<int16_t>(lt[2]));
                        data.push_back(to_int16(vgl::CsgCommand::VERTEX));
                        data.push_back(static_cast<int16_t>(rt[0]));
                        data.push_back(static_cast<int16_t>(rt[1]));
                        data.push_back(static_cast<int16_t>(rt[2]));

                        if(vidx > 0)
                        {
                            data.push_back(to_int16(vgl::CsgCommand::QUAD));
                            data.push_back(static_cast<int16_t>(vidx - 2));
                            data.push_back(static_cast<int16_t>(vidx - 1));
                            data.push_back(static_cast<int16_t>(vidx + 1));
                            data.push_back(static_cast<int16_t>(vidx + 0));
                        }
                        vidx = static_cast<int16_t>(vidx + 2);
                    }

                    prev = curr;
                    it = next_it;
                }

                data.push_back(to_int16(vgl::CsgCommand::NONE));
                vgl::LogicalMesh lmesh(data.data());
                return lmesh.compile(false);
            };

            // Malmi.
            {
                static const int16_t CHART_DATA[] =
                {
                    90, 0,
                    520, 440,
                    630, 550,
                    970, 1000,
                };
                m_mesh_chart[0] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart0", *m_mesh_chart[0]);
#endif
            }

            // Tapanila.
            {
                static const int16_t CHART_DATA[] =
                {
                    190, 0,
                    420, 300,
                    530, 450,
                    600, 570,
                    650, 660,
                    660, 710,
                    720, 1000,
                };
                m_mesh_chart[1] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart1", *m_mesh_chart[1]);
#endif
            }

            // Puistola, Tikkurila.
            {
                static const int16_t CHART_DATA[] =
                {
                    450, 0,
                    550, 450,
                    570, 550,
                    670, 1000,
                };
                m_mesh_chart[2] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart2", *m_mesh_chart[2]);
#endif
            }

            // Hiekkaharju.
            {
                static const int16_t CHART_DATA[] =
                {
                    445, 0,
                    545, 450,
                    570, 550,
                    680, 1000,
                };
                m_mesh_chart[3] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart3", *m_mesh_chart[3]);
#endif
            }

            // Rekola.
            {
                static const int16_t CHART_DATA[] =
                {
                    340, 0,
                    490, 330,
                    580, 570,
                    615, 670,
                    695, 1000,
                };
                m_mesh_chart[4] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart4", *m_mesh_chart[4]);
#endif
            }

            // Korso.
            {
                static const int16_t CHART_DATA[] =
                {
                    430, 0,
                    540, 430,
                    575, 570,
                    660, 880,
                    700, 1000,
                };
                m_mesh_chart[5] = generate_chart(CHART_DATA, static_cast<unsigned>(sizeof(CHART_DATA) / sizeof(int16_t)));
#if defined(USE_LD)
                addPreviewMesh("chart5", *m_mesh_chart[5]);
#endif
            }
        }
#endif

        // Fence.
        {
            const int16_t FENCE_LENGTH = 500;
            const int16_t FENCE_HEIGHT_2 = 210;

#if defined(USE_LD)
            const int16_t FENCE_HEIGHT_1 = 230;

            vgl::vector<int16_t> data;

            // Back post.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(FENCE_HEIGHT_1);
            data.push_back(0);
            data.push_back(1);
            data.push_back(7);
            data.push_back(7);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            csg_update("csg_fence.hpp", data);
#else
#include "csg_fence.hpp"
            auto data = CSG_READ_HPP(g_csg_fence_hpp);
#endif

            // Up posts.
            const unsigned VERT_COUNT = 4;
            for(unsigned ii = 0; (ii < VERT_COUNT); ++ii)
            {
                int16_t pos = static_cast<int16_t>((ii + 1) * (FENCE_LENGTH / static_cast<int>(VERT_COUNT + 1)));
                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(0);
                data.push_back(0);
                data.push_back(pos);
                data.push_back(0);
                data.push_back(FENCE_HEIGHT_2);
                data.push_back(pos);
                data.push_back(1);
                data.push_back(5);
                data.push_back(4);
                data.push_back(vgl::CSG_NO_FRONT);
            }

            // Horiz posts.
            const unsigned HORIZ_COUNT = 5;
            for(unsigned ii = 0; (ii < HORIZ_COUNT); ++ii)
            {
                int16_t pos = static_cast<int16_t>((ii + 1) * (FENCE_HEIGHT_2 / static_cast<int>(HORIZ_COUNT + 1)));
                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(0);
                data.push_back(pos);
                data.push_back(0);
                data.push_back(0);
                data.push_back(pos);
                data.push_back(FENCE_LENGTH);
                data.push_back(1);
                data.push_back(3);
                data.push_back(3);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            }

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_fence = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("fence", *m_mesh_fence);
#endif
        }

        // Zigzag.
        const int16_t PYLON_WIDTH_ZIGZAG = 11;
        // Generates zigzag pattern grider inlet.
        auto generate_zigzag = [PYLON_WIDTH_ZIGZAG](vgl::vector<int16_t>& data, const vgl::vec3& p1,
                const vgl::vec3& p2, const vgl::vec3 other, int16_t separation, unsigned count)
        {
            vgl::vec3 diff = p2 - p1;
            float len = length(diff);
            vgl::vec3 dir = diff * (1.0f / len);
            vgl::vec3 rt = normalize(cross(other, dir));

            float segment_len = len / static_cast<float>(count);
            for(unsigned ii = 0; (ii < count); ++ii)
            {
                vgl::vec3 curr = p1 + dir * segment_len * static_cast<float>(ii);
                vgl::vec3 c1 = curr - (separation * rt);
                vgl::vec3 c2 = curr + (separation * rt) + (dir * segment_len * 0.5f);
                vgl::vec3 c3 = curr - (separation * rt) + (dir * segment_len);
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(c1.x()));
                data.push_back(static_cast<int16_t>(c1.y()));
                data.push_back(static_cast<int16_t>(c1.z()));
                data.push_back(static_cast<int16_t>(c2.x()));
                data.push_back(static_cast<int16_t>(c2.y()));
                data.push_back(static_cast<int16_t>(c2.z()));
                data.push_back(0);
                data.push_back(static_cast<int16_t>(other.x()));
                data.push_back(static_cast<int16_t>(other.y()));
                data.push_back(static_cast<int16_t>(other.z()));
                data.push_back(PYLON_WIDTH_ZIGZAG);
                data.push_back(PYLON_WIDTH_ZIGZAG);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(c2.x()));
                data.push_back(static_cast<int16_t>(c2.y()));
                data.push_back(static_cast<int16_t>(c2.z()));
                data.push_back(static_cast<int16_t>(c3.x()));
                data.push_back(static_cast<int16_t>(c3.y()));
                data.push_back(static_cast<int16_t>(c3.z()));
                data.push_back(0);
                data.push_back(static_cast<int16_t>(other.x()));
                data.push_back(static_cast<int16_t>(other.y()));
                data.push_back(static_cast<int16_t>(other.z()));
                data.push_back(PYLON_WIDTH_ZIGZAG);
                data.push_back(PYLON_WIDTH_ZIGZAG);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            }
        };

        // Pylon (left-handed) and closed power line arc.
        //
        // Modelled after the finnish pylon specification from Liikennevirasto, 2013.
        // See: https://julkaisut.vayla.fi/pdf3/lo_2013-21_rato_5_web.pdf page 33
        // Some dimensions have been exaggerated.
        //
        // Technically the arc is only used in a train yard, but realism is going to be reduced to increase variety.
        {
            const int16_t PYLON_HEIGHT_BASE = 20;
            const int16_t PYLON_WIDTH_GRIDER = 18;
            const int16_t PYLON_HEIGHT_GRIDER = 800;
            const int16_t PYLON_HEIGHT_LINE = 600;
            const int16_t PYLON_HEIGHT_LINE_END = 750;
            const int16_t PYLON_GRIDER_SEPARATION = 35;
            const int16_t PYLON_HORIZONTAL_OFFSET = 430;
            const int16_t TENDON_OFFSET = 8;
            const int16_t TENDON_RADIUS = 4;
            const int16_t ZIGZAG_COUNT = 6;

#if defined(USE_LD)
            const int16_t PYLON_WIDTH_BASE = 98;
            const int16_t PYLON_HEIGHT_LINE_START = 550;
            const int16_t PYLON_HORIZONTAL_EXCESS = 520;
            const int16_t PYLON_HORIZONTAL_DIRECTOR_START = 130;
            const int16_t PYLON_HORIZONTAL_DIRECTOR_END = 550;
            const int16_t PYLON_SUPPORT_RADIUS = 9;
            const int16_t PYLON_SUPPORT_RADIUS_SMALL = 7;
            const int16_t TENDON_HEIGHT_RETURN_1 = 760;
            const int16_t TENDON_HEIGHT_RETURN_2 = 690;
            const int16_t TENDON_RETURN_OFFSET = 28;

            vgl::vector<int16_t> data;

            // Base.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(-PYLON_HEIGHT_BASE * 3); // Extend down, might have to "float".
            data.push_back(0);
            data.push_back(0);
            data.push_back(PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(1);
            data.push_back(PYLON_WIDTH_BASE);
            data.push_back(PYLON_WIDTH_BASE);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_pylon_base.hpp", data);
#else
#include "csg_pylon_base.hpp"
            auto data = CSG_READ_HPP(g_csg_pylon_base_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_pylon_extra_base = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("pylon_extra_base", *m_mesh_pylon_extra_base);
#endif

#if defined(USE_LD)
            // Continue work on pylon.
            data.resize(11);

            // Up posts (on the base).
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(-PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(-PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_GRIDER);
            data.push_back(0);
            data.push_back(1);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_GRIDER);
            data.push_back(0);
            data.push_back(1);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            // Zigzag end.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(-PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_GRIDER - PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_GRIDER - PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(2);
            data.push_back(PYLON_WIDTH_ZIGZAG);
            data.push_back(PYLON_WIDTH_ZIGZAG);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Support struts.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_LINE_START);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_OFFSET + 16);
            data.push_back(PYLON_HEIGHT_LINE_END + 10);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_HORIZONTAL_DIRECTOR_START);
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_DIRECTOR_END);
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_GRIDER_SEPARATION);
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_EXCESS);
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS_SMALL);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Tendon holders.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-PYLON_GRIDER_SEPARATION);
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(0);
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET + TENDON_RADIUS / 4);
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-PYLON_GRIDER_SEPARATION);
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(0);
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET + TENDON_RADIUS / 4);
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            csg_update("csg_pylon.hpp", data);
#else
#include "csg_pylon.hpp"
            data = CSG_READ_HPP(g_csg_pylon_hpp);
#endif

            // Zigzag.
            generate_zigzag(data,
                    vgl::vec3(0.0f, static_cast<float>(PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(0.0f, static_cast<float>(PYLON_HEIGHT_GRIDER - PYLON_HEIGHT_BASE - PYLON_WIDTH_ZIGZAG), 0.0f),
                    vgl::vec3(0.0f, 0.0f, 1.0f),
                    PYLON_GRIDER_SEPARATION,
                    ZIGZAG_COUNT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_pylon = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("pylon", *m_mesh_pylon);
#endif

            const int16_t ARC_PILLAR_OFFSET = static_cast<int16_t>(PYLON_X * 100.0f);
            const int16_t ARC_HEIGHT_GRIDER = 800;
            const int16_t ARC_GRIDER_SEPARATION = 40;

#if defined(USE_LD)
            const int16_t ARC_BASE_WIDTH = 112;
            const int16_t ARC_VERTICAL_STRUT = static_cast<int16_t>(static_cast<int>(PYLON_X * 100.0f) - PYLON_HORIZONTAL_OFFSET);
            const int16_t ARC_HORIZONTAL_DIRECTOR_START = static_cast<int16_t>(ARC_VERTICAL_STRUT + 292);
            const int16_t ARC_STRUT_EXCESS = 30;

            data.clear();

            // Bases.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(ARC_PILLAR_OFFSET);
            data.push_back(-PYLON_HEIGHT_BASE * 3);
            data.push_back(0);
            data.push_back(ARC_PILLAR_OFFSET);
            data.push_back(PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(1);
            data.push_back(ARC_BASE_WIDTH);
            data.push_back(ARC_BASE_WIDTH);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET));
            data.push_back(-PYLON_HEIGHT_BASE * 3);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET));
            data.push_back(PYLON_HEIGHT_BASE);
            data.push_back(0);
            data.push_back(1);
            data.push_back(ARC_BASE_WIDTH);
            data.push_back(ARC_BASE_WIDTH);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            // Horizontal griders.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);

            // Corners (right).
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(2);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + ARC_GRIDER_SEPARATION * 2 + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + ARC_GRIDER_SEPARATION * 2 + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Corners (left).
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(2);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(PYLON_WIDTH_GRIDER);
            data.push_back(vgl::CSG_FLAT);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + ARC_GRIDER_SEPARATION * 2 + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER - PYLON_WIDTH_GRIDER / 4));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_HEIGHT_GRIDER + ARC_GRIDER_SEPARATION * 2 + PYLON_WIDTH_GRIDER * 3 / 4));
            data.push_back(static_cast<int16_t>(ARC_GRIDER_SEPARATION));
            data.push_back(3);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(PYLON_WIDTH_GRIDER - 5);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Right support.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(ARC_HORIZONTAL_DIRECTOR_START);
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(ARC_VERTICAL_STRUT - ARC_STRUT_EXCESS * 2));
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS - 1);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(ARC_VERTICAL_STRUT - ARC_STRUT_EXCESS));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(PYLON_HEIGHT_LINE_START);
            data.push_back(0);
            data.push_back(ARC_VERTICAL_STRUT);
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(ARC_VERTICAL_STRUT);
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(ARC_VERTICAL_STRUT);
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS_SMALL);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(ARC_HEIGHT_GRIDER);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(PYLON_HEIGHT_LINE_START - 35));
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            // Left support.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_HORIZONTAL_DIRECTOR_START));
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_VERTICAL_STRUT + ARC_STRUT_EXCESS * 2));
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS - 1);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_VERTICAL_STRUT + ARC_STRUT_EXCESS));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(PYLON_HEIGHT_LINE_START);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_VERTICAL_STRUT));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_VERTICAL_STRUT));
            data.push_back(PYLON_HEIGHT_LINE_END);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_VERTICAL_STRUT));
            data.push_back(PYLON_HEIGHT_LINE);
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS_SMALL);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(ARC_HEIGHT_GRIDER);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION));
            data.push_back(static_cast<int16_t>(PYLON_HEIGHT_LINE_START - 35));
            data.push_back(0);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            // Tendon holders.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(-ARC_GRIDER_SEPARATION);
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + PYLON_GRIDER_SEPARATION + TENDON_RETURN_OFFSET - TENDON_RADIUS / 4));
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(-ARC_GRIDER_SEPARATION);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER / 2));
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(ARC_GRIDER_SEPARATION);
            data.push_back(static_cast<int16_t>(ARC_PILLAR_OFFSET + PYLON_GRIDER_SEPARATION + TENDON_RETURN_OFFSET - TENDON_RADIUS / 4));
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(ARC_GRIDER_SEPARATION);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(-ARC_GRIDER_SEPARATION);
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET + TENDON_RADIUS / 4));
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(-ARC_GRIDER_SEPARATION);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER / 2));
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(ARC_GRIDER_SEPARATION);
            data.push_back(static_cast<int16_t>(-ARC_PILLAR_OFFSET - PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET + TENDON_RADIUS / 4));
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(ARC_GRIDER_SEPARATION);
            data.push_back(3);
            data.push_back(5);
            data.push_back(PYLON_SUPPORT_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            csg_update("csg_arc.hpp", data);
#else
#include "csg_arc.hpp"
            data = CSG_READ_HPP(g_csg_arc_hpp);
#endif

            // Up posts.
            auto generate_pillars = [PYLON_HEIGHT_BASE, PYLON_WIDTH_GRIDER, ARC_HEIGHT_GRIDER, ARC_GRIDER_SEPARATION, &data]
                (int16_t xpos)
                {
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(xpos - ARC_GRIDER_SEPARATION));
                    data.push_back(PYLON_HEIGHT_BASE);
                    data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
                    data.push_back(static_cast<int16_t>(xpos - ARC_GRIDER_SEPARATION));
                    data.push_back(ARC_HEIGHT_GRIDER);
                    data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
                    data.push_back(3);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(xpos + ARC_GRIDER_SEPARATION));
                    data.push_back(PYLON_HEIGHT_BASE);
                    data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
                    data.push_back(static_cast<int16_t>(xpos + ARC_GRIDER_SEPARATION));
                    data.push_back(ARC_HEIGHT_GRIDER);
                    data.push_back(static_cast<int16_t>(-ARC_GRIDER_SEPARATION));
                    data.push_back(3);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(xpos + ARC_GRIDER_SEPARATION));
                    data.push_back(PYLON_HEIGHT_BASE);
                    data.push_back(ARC_GRIDER_SEPARATION);
                    data.push_back(static_cast<int16_t>(xpos + ARC_GRIDER_SEPARATION));
                    data.push_back(ARC_HEIGHT_GRIDER);
                    data.push_back(ARC_GRIDER_SEPARATION);
                    data.push_back(3);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(xpos - ARC_GRIDER_SEPARATION));
                    data.push_back(PYLON_HEIGHT_BASE);
                    data.push_back(ARC_GRIDER_SEPARATION);
                    data.push_back(static_cast<int16_t>(xpos - ARC_GRIDER_SEPARATION));
                    data.push_back(ARC_HEIGHT_GRIDER);
                    data.push_back(ARC_GRIDER_SEPARATION);
                    data.push_back(3);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(PYLON_WIDTH_GRIDER);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                };
            generate_pillars(ARC_PILLAR_OFFSET);
            generate_pillars(static_cast<int16_t>(-ARC_PILLAR_OFFSET));

            // Right.
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET), static_cast<float>(PYLON_HEIGHT_BASE),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET), static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, 1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET), static_cast<float>(PYLON_HEIGHT_BASE),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET), static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, -1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION),
                        static_cast<float>(PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(-1.0f, 0.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION),
                        static_cast<float>(PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(1.0f, 0.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            // Left.
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET), static_cast<float>(PYLON_HEIGHT_BASE),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET), static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, 1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET), static_cast<float>(PYLON_HEIGHT_BASE),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET), static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, -1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION),
                        static_cast<float>(PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(-1.0f, 0.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION),
                        static_cast<float>(PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_HEIGHT_GRIDER - PYLON_HEIGHT_BASE), 0.0f),
                    vgl::vec3(1.0f, 0.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT - 1);

            // Horizontal zigzags
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION),
                        static_cast<float>(ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, -1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT + 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION),
                        static_cast<float>(-ARC_GRIDER_SEPARATION)),
                    vgl::vec3(0.0f, 0.0f, 1.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT + 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2), 0.0f),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2 + ARC_GRIDER_SEPARATION * 2), 0.0f),
                    vgl::vec3(0.0f, 1.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT + 1);
            generate_zigzag(data,
                    vgl::vec3(static_cast<float>(ARC_PILLAR_OFFSET - ARC_GRIDER_SEPARATION - PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2), 0.0f),
                    vgl::vec3(static_cast<float>(-ARC_PILLAR_OFFSET + ARC_GRIDER_SEPARATION + PYLON_WIDTH_GRIDER),
                        static_cast<float>(ARC_HEIGHT_GRIDER + PYLON_WIDTH_GRIDER / 2), 0.0f),
                    vgl::vec3(0.0f, -1.0f, 0.0f),
                    ARC_GRIDER_SEPARATION,
                    ZIGZAG_COUNT + 1);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_arc = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("arc", *m_mesh_arc);
#endif

            // Tendons.
            const int16_t TENDON_LENGTH = 5500;
            const int16_t TENDON_SUPPORT_COUNT = 6;

#if defined(USE_LD)
            data.clear();

            // Main lines.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE - TENDON_RADIUS - TENDON_OFFSET);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE - TENDON_RADIUS - TENDON_OFFSET);
            data.push_back(TENDON_LENGTH);
            data.push_back(2);
            data.push_back(3);
            data.push_back(TENDON_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE_END - TENDON_RADIUS - TENDON_OFFSET);
            data.push_back(0);
            data.push_back(PYLON_HORIZONTAL_OFFSET);
            data.push_back(PYLON_HEIGHT_LINE_END - TENDON_RADIUS - TENDON_OFFSET);
            data.push_back(TENDON_LENGTH);
            data.push_back(2);
            data.push_back(3);
            data.push_back(TENDON_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Back lines.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET);
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(0);
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET);
            data.push_back(TENDON_HEIGHT_RETURN_1);
            data.push_back(TENDON_LENGTH);
            data.push_back(2);
            data.push_back(3);
            data.push_back(TENDON_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET);
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(0);
            data.push_back(-PYLON_GRIDER_SEPARATION - TENDON_RETURN_OFFSET);
            data.push_back(TENDON_HEIGHT_RETURN_2);
            data.push_back(TENDON_LENGTH);
            data.push_back(2);
            data.push_back(3);
            data.push_back(TENDON_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            csg_update("csg_tendon.hpp", data);
#else
#include "csg_tendon.hpp"
            data = CSG_READ_HPP(g_csg_tendon_hpp);
#endif

            for(int16_t ii = 0; (ii < TENDON_SUPPORT_COUNT); ++ii)
            {
                int16_t zc = static_cast<int16_t>((TENDON_LENGTH / TENDON_SUPPORT_COUNT) * ii);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(PYLON_HORIZONTAL_OFFSET);
                data.push_back(PYLON_HEIGHT_LINE_END - TENDON_RADIUS - TENDON_OFFSET);
                data.push_back(zc);
                data.push_back(PYLON_HORIZONTAL_OFFSET);
                data.push_back(PYLON_HEIGHT_LINE - TENDON_RADIUS - TENDON_OFFSET);
                data.push_back(zc);
                data.push_back(1);
                data.push_back(3);
                data.push_back(TENDON_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            }

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_tendons = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("tendons", *m_mesh_tendons);
#endif
        }

        // Lamppost.
        const int16_t LAMPPOST_RADIUS = 17;
        // Generates lamppost standing into Y and pointing to some cardinal direction.
        auto generate_lamppost = [LAMPPOST_RADIUS](vgl::vector<int16_t>& data, int16_t px, int16_t py, int16_t pz, int16_t dx,
                int16_t dz)
        {
            const int16_t LAMPPOST_HEIGHT = 900;
            const int16_t LAMPPOST_INSET = 340;
            const int16_t LAMPPOST_END_WIDTH = 52;
            const int16_t LAMPPOST_END_HEIGHT = 36;
            const int16_t LAMPPOST_END_INSET = 14;

            int16_t x1 = px;
            int16_t z1 = pz;
            int16_t x2 = x1;
            int16_t z2 = z1;
            int16_t x3 = static_cast<int16_t>(x1 + dx * (LAMPPOST_INSET * 1 / 8));
            int16_t z3 = static_cast<int16_t>(z1 + dz * (LAMPPOST_INSET * 1 / 8));
            int16_t x4 = static_cast<int16_t>(x1 + dx * (LAMPPOST_INSET * 3 / 8));
            int16_t z4 = static_cast<int16_t>(z1 + dz * (LAMPPOST_INSET * 3 / 8));
            int16_t x5 = static_cast<int16_t>(x1 + dx * (LAMPPOST_INSET * 6 / 8));
            int16_t z5 = static_cast<int16_t>(z1 + dz * (LAMPPOST_INSET * 6 / 8));
            int16_t x6 = static_cast<int16_t>(x1 + dx * LAMPPOST_INSET);
            int16_t z6 = static_cast<int16_t>(z1 + dz * LAMPPOST_INSET);
            int16_t y1 = py;
            int16_t y2 = static_cast<int16_t>(y1 + LAMPPOST_HEIGHT * 11/ 17);
            int16_t y3 = static_cast<int16_t>(y1 + LAMPPOST_HEIGHT * 14 / 17);
            int16_t y4 = static_cast<int16_t>(y1 + LAMPPOST_HEIGHT * 16 / 17);
            int16_t y5 = static_cast<int16_t>(y1 + LAMPPOST_HEIGHT);

            data.push_back(to_int16(vgl::CsgCommand::PIPE));
            data.push_back(5);
            data.push_back(x1);
            data.push_back(y1);
            data.push_back(z1);
            data.push_back(x2);
            data.push_back(y2);
            data.push_back(z2);
            data.push_back(x3);
            data.push_back(y3);
            data.push_back(z3);
            data.push_back(x4);
            data.push_back(y4);
            data.push_back(z4);
            data.push_back(x5);
            data.push_back(y5);
            data.push_back(z5);
            data.push_back(6);
            data.push_back(LAMPPOST_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(x5 - dx * LAMPPOST_END_INSET));
            data.push_back(y5);
            data.push_back(static_cast<int16_t>(z5 - dz * LAMPPOST_END_INSET));
            data.push_back(x6);
            data.push_back(y5);
            data.push_back(z6);
            data.push_back(2);
            data.push_back(LAMPPOST_END_WIDTH);
            data.push_back(LAMPPOST_END_HEIGHT);
            data.push_back(0);
        };

        // Lamppost.
        // Default lamppost points to positive X direction.
        {
            vgl::vector<int16_t> data;

            generate_lamppost(data, 0, 0, 0, 1, 0);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_lamppost = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("lamppost", *m_mesh_lamppost);
#endif
        }

        // Bridge
        // Segment length takes 2 lightposts and 3 pillars.
        {
            const int16_t BRIDGE_SEGMENT_LENGTH = 6500;
            const int16_t BRIDGE_BASE_WIDTH = 800;
            const int16_t BRIDGE_BASE_HEIGHT = 900;
            const int16_t BRIDGE_SIDE_WIDTH = 240;
            const int16_t BRIDGE_SIDE_HEIGHT = 970;
            const int16_t BRIDGE_SIDE_THICKNESS = 90;
            const int16_t BRIDGE_SIDE_FLOOR = static_cast<int16_t>(BRIDGE_SIDE_HEIGHT + BRIDGE_SIDE_THICKNESS);
            const int16_t BRIDGE_PILLAR_OFFSET = 100;
            const int16_t BRIDGE_PILLAR_SEPARATION = 600;
            const int16_t BRIDGE_PILLAR_RADIUS = 50;
            const int16_t FENCE_SEGMENT_LENGTH = 500;
            const int16_t FENCE_HEIGHT = 120;
            const int16_t FENCE_POST_LARGE_WIDTH = 20;
            const int16_t FENCE_POST_SMALL_RADIUS = 5;
            const int16_t FENCE_OFFSET = static_cast<int16_t>(BRIDGE_SIDE_WIDTH + (BRIDGE_BASE_WIDTH / 2) - (FENCE_POST_LARGE_WIDTH / 2));

#if defined(USE_LD)
            const int16_t BRIDGE_BASE_THICKNESS = 140;

            vgl::vector<int16_t> data;

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(BRIDGE_BASE_HEIGHT + (BRIDGE_BASE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(-BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(BRIDGE_BASE_HEIGHT + (BRIDGE_BASE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(2);
            data.push_back(BRIDGE_BASE_WIDTH);
            data.push_back(BRIDGE_BASE_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((BRIDGE_BASE_WIDTH + BRIDGE_SIDE_WIDTH) / 2));
            data.push_back(static_cast<int16_t>(BRIDGE_SIDE_HEIGHT + (BRIDGE_SIDE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(-BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(static_cast<int16_t>((BRIDGE_BASE_WIDTH + BRIDGE_SIDE_WIDTH) / 2));
            data.push_back(static_cast<int16_t>(BRIDGE_SIDE_HEIGHT + (BRIDGE_SIDE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(2);
            data.push_back(BRIDGE_SIDE_WIDTH);
            data.push_back(BRIDGE_SIDE_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(BRIDGE_BASE_WIDTH + BRIDGE_SIDE_WIDTH) / 2));
            data.push_back(static_cast<int16_t>(BRIDGE_SIDE_HEIGHT + (BRIDGE_SIDE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(-BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(static_cast<int16_t>(-(BRIDGE_BASE_WIDTH + BRIDGE_SIDE_WIDTH) / 2));
            data.push_back(static_cast<int16_t>(BRIDGE_SIDE_HEIGHT + (BRIDGE_SIDE_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(BRIDGE_SEGMENT_LENGTH / 2));
            data.push_back(2);
            data.push_back(BRIDGE_SIDE_WIDTH);
            data.push_back(BRIDGE_SIDE_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            csg_update("csg_bridge.hpp", data);
#else
#include "csg_bridge.hpp"
            auto data = CSG_READ_HPP(g_csg_bridge_hpp);
#endif

            // Pillars below bridge.
            for(int ii = 0; (ii < 3); ++ii)
            {
                int16_t zpos = static_cast<int16_t>(-(BRIDGE_SEGMENT_LENGTH / 2) + (BRIDGE_SEGMENT_LENGTH / 3) * ii + BRIDGE_PILLAR_OFFSET);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(BRIDGE_PILLAR_SEPARATION / 2));
                data.push_back(static_cast<int16_t>(-BRIDGE_BASE_HEIGHT / 4));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(BRIDGE_PILLAR_SEPARATION / 2));
                data.push_back(BRIDGE_BASE_HEIGHT);
                data.push_back(zpos);
                data.push_back(1);
                data.push_back(9);
                data.push_back(BRIDGE_PILLAR_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-BRIDGE_PILLAR_SEPARATION / 2));
                data.push_back(static_cast<int16_t>(-BRIDGE_BASE_HEIGHT / 4));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(-BRIDGE_PILLAR_SEPARATION / 2));
                data.push_back(BRIDGE_BASE_HEIGHT);
                data.push_back(zpos);
                data.push_back(1);
                data.push_back(9);
                data.push_back(BRIDGE_PILLAR_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            }

            // Fence.
            for(int16_t zpos = static_cast<int16_t>(-BRIDGE_SEGMENT_LENGTH / 2);
                    (zpos < static_cast<int16_t>(BRIDGE_SEGMENT_LENGTH / 2));
                    zpos = static_cast<int16_t>(zpos + FENCE_SEGMENT_LENGTH))
            {
                for(int16_t xpos = -FENCE_OFFSET; xpos <= FENCE_OFFSET; xpos = static_cast<int16_t>(xpos + (FENCE_OFFSET * 2)))
                {
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(xpos);
                    data.push_back(BRIDGE_SIDE_FLOOR);
                    data.push_back(static_cast<int16_t>(zpos + FENCE_POST_LARGE_WIDTH / 2));
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(BRIDGE_SIDE_FLOOR + FENCE_HEIGHT - FENCE_POST_LARGE_WIDTH));
                    data.push_back(static_cast<int16_t>(zpos + FENCE_POST_LARGE_WIDTH / 2));
                    data.push_back(1);
                    data.push_back(FENCE_POST_LARGE_WIDTH);
                    data.push_back(FENCE_POST_LARGE_WIDTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(BRIDGE_SIDE_FLOOR + FENCE_HEIGHT - (FENCE_POST_LARGE_WIDTH / 2)));
                    data.push_back(zpos);
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(BRIDGE_SIDE_FLOOR + FENCE_HEIGHT - (FENCE_POST_LARGE_WIDTH / 2)));
                    data.push_back(static_cast<int16_t>(zpos + FENCE_SEGMENT_LENGTH));
                    data.push_back(1);
                    data.push_back(FENCE_POST_LARGE_WIDTH);
                    data.push_back(FENCE_POST_LARGE_WIDTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    for(int16_t inc = 0; (inc < FENCE_SEGMENT_LENGTH); inc = static_cast<int16_t>(inc + FENCE_SEGMENT_LENGTH / 5))
                    {
                        int16_t cpos = static_cast<int16_t>(zpos + inc);

                        data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                        data.push_back(xpos);
                        data.push_back(BRIDGE_SIDE_FLOOR);
                        data.push_back(cpos);
                        data.push_back(xpos);
                        data.push_back(static_cast<int16_t>(BRIDGE_SIDE_FLOOR + FENCE_HEIGHT - (FENCE_POST_LARGE_WIDTH / 2)));
                        data.push_back(cpos);
                        data.push_back(1);
                        data.push_back(3);
                        data.push_back(FENCE_POST_SMALL_RADIUS);
                        data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
                    }
                }
            }

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_bridge = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("bridge", *m_mesh_bridge);
#endif
        }

        // Rails.
        {
            const int16_t RAIL_DIFFERENCE = 152;
            const int16_t RAIL_GRIDER_WIDTH_TOP = 8;
            const int16_t RAIL_GRIDER_WIDTH_MID = 5;
            const int16_t RAIL_GRIDER_WIDTH_BOTTOM = 10;
            const int16_t RAIL_GRIDER_HEIGHT = 16;
            const int16_t RAIL_GRIDER_BLOCK_HEIGHT = 4;
            const int16_t RAIL_GRIDER_ELEVATION = 4;
            const int16_t RAIL_SEGMENT_LENGTH = 1000;
            const int16_t RAIL_BLOCK_WIDTH = 180;
            const int16_t RAIL_BLOCK_LENGTH = 35;
            const int16_t RAIL_BLOCK_COUNT = 5;

            vgl::vector<int16_t> data;

            auto generate_grider = [RAIL_GRIDER_BLOCK_HEIGHT, RAIL_GRIDER_WIDTH_BOTTOM, RAIL_GRIDER_WIDTH_MID,
                 RAIL_GRIDER_WIDTH_TOP, &data]
                     (int16_t xpos)
            {
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_BLOCK_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(-RAIL_SEGMENT_LENGTH / 2));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_BLOCK_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(RAIL_SEGMENT_LENGTH / 2));
                data.push_back(2);
                data.push_back(RAIL_GRIDER_WIDTH_BOTTOM);
                data.push_back(RAIL_GRIDER_BLOCK_HEIGHT);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(-RAIL_SEGMENT_LENGTH / 2));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(RAIL_SEGMENT_LENGTH / 2));
                data.push_back(2);
                data.push_back(RAIL_GRIDER_WIDTH_MID);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_HEIGHT - RAIL_GRIDER_BLOCK_HEIGHT * 2));
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP | vgl::CSG_NO_BOTTOM);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_HEIGHT - RAIL_GRIDER_BLOCK_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(-RAIL_SEGMENT_LENGTH / 2));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION + RAIL_GRIDER_HEIGHT - RAIL_GRIDER_BLOCK_HEIGHT / 2));
                data.push_back(static_cast<int16_t>(RAIL_SEGMENT_LENGTH / 2));
                data.push_back(2);
                data.push_back(RAIL_GRIDER_WIDTH_TOP);
                data.push_back(RAIL_GRIDER_BLOCK_HEIGHT);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            };
            generate_grider(static_cast<int16_t>(RAIL_DIFFERENCE / 2));
            generate_grider(static_cast<int16_t>(-RAIL_DIFFERENCE / 2));

            // Rail blocks

            for(int16_t ii = 0; (ii < RAIL_BLOCK_COUNT); ++ii)
            {
                int16_t cpos = static_cast<int16_t>((ii * RAIL_SEGMENT_LENGTH) / RAIL_BLOCK_COUNT - (RAIL_SEGMENT_LENGTH / 2));

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(-RAIL_BLOCK_WIDTH / 2));
                data.push_back(static_cast<int16_t>((RAIL_GRIDER_ELEVATION - RAIL_GRIDER_ELEVATION * 5) / 2));
                data.push_back(static_cast<int16_t>(cpos + RAIL_BLOCK_LENGTH / 2));
                data.push_back(static_cast<int16_t>(RAIL_BLOCK_WIDTH / 2));
                data.push_back(static_cast<int16_t>((RAIL_GRIDER_ELEVATION - RAIL_GRIDER_ELEVATION * 5) / 2));
                data.push_back(static_cast<int16_t>(cpos + RAIL_BLOCK_LENGTH / 2));
                data.push_back(2);
                data.push_back(RAIL_BLOCK_LENGTH);
                data.push_back(static_cast<int16_t>(RAIL_GRIDER_ELEVATION * 6));
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM);
            }

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_rails = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("rails", *m_mesh_rails);
#endif
        }

        // Signs.
        {
#if defined(USE_LD)
            const int16_t SIGN_HEIGHT = 220;
            const int16_t SIGN_WIDTH = 570;
            const int16_t SIGN_DEPTH = 6;
            const int16_t SIGN_LEG_HEIGHT = 150;
            const int16_t SIGN_LEG_RADIUS = 8;
            const int16_t SIGN_LEG_SEPARATION = 260;

            vgl::vector<int16_t> data;

            // Plaque.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT));
            data.push_back(0);
            data.push_back(3);
            data.push_back(SIGN_WIDTH);
            data.push_back(SIGN_DEPTH);
            data.push_back(vgl::CSG_FLAT);

            // Legs.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_HEIGHT * 4));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_HEIGHT * 4));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sign0.hpp", data);
#else
#include "csg_sign0.hpp"
            auto data = CSG_READ_HPP(g_csg_sign0_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sign[0] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sign0", *m_mesh_sign[0]);
#endif

#if defined(USE_LD)
            data.resize(11);

            // Legs.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT + SIGN_LEG_HEIGHT));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT + SIGN_LEG_HEIGHT));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sign1.hpp", data);
#else
#include "csg_sign1.hpp"
            data = CSG_READ_HPP(g_csg_sign1_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sign[1] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sign1", *m_mesh_sign[1]);
#endif
        }

        // Mega-kerava sign.
        {
#if defined(USE_LD)
            const int16_t SIGN_HEIGHT = 340;
            const int16_t SIGN_WIDTH = 1240;
            const int16_t SIGN_DEPTH = 15;
            const int16_t SIGN_LEG_HEIGHT = 180;
            const int16_t SIGN_LEG_RADIUS = 11;
            const int16_t SIGN_LEG_SEPARATION = 520;

            vgl::vector<int16_t> data;

            // Plaque.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT));
            data.push_back(0);
            data.push_back(3);
            data.push_back(SIGN_WIDTH);
            data.push_back(SIGN_DEPTH);
            data.push_back(vgl::CSG_FLAT);

            // Legs.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_HEIGHT * 5 / 2));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(-SIGN_LEG_HEIGHT * 5 / 2));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(SIGN_LEG_SEPARATION / 2));
            data.push_back(static_cast<int16_t>(SIGN_HEIGHT - SIGN_LEG_RADIUS));
            data.push_back(static_cast<int16_t>(-SIGN_DEPTH / 2 - SIGN_LEG_RADIUS));
            data.push_back(3);
            data.push_back(5);
            data.push_back(SIGN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sign2.hpp", data);
#else
#include "csg_sign2.hpp"
            auto data = CSG_READ_HPP(g_csg_sign2_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sign[2] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sign2", *m_mesh_sign[2]);
#endif
        }

        // Xoff 3 can.
        // Roughly half the size of the actual landmark.
        // Relative dimensions have been slightly exaggerated for effect.
        {
#if defined(USE_LD)
            const int16_t CAN_RADIUS1 = 170;
            const int16_t CAN_RADIUS2 = 156;
            const int16_t CAN_RADIUS3 = 166;
            const int16_t CAN_LENGTH1 = 230;
            const int16_t CAN_LENGTH2 = 255;
            const int16_t CAN_LENGTH3 = 280;
            const int16_t CAN_Z_OFFSET = static_cast<int16_t>(-CAN_RADIUS1);
            const int16_t CAN_LEG_OFFSET = 90;
            const int16_t CAN_LEG_X1 = static_cast<int16_t>(-CAN_LEG_OFFSET);
            const int16_t CAN_LEG_X2 = static_cast<int16_t>(CAN_LEG_OFFSET);
            const int16_t CAN_LEG_Z1 = static_cast<int16_t>(CAN_Z_OFFSET - CAN_LEG_OFFSET);
            const int16_t CAN_LEG_Z2 = static_cast<int16_t>(CAN_Z_OFFSET + CAN_LEG_OFFSET);
            const int16_t CAN_LEG_Y2 = static_cast<int16_t>(-CAN_LENGTH3);
            const int16_t CAN_LEG_Y1 = static_cast<int16_t>(CAN_LEG_Y2 - 260);
            const int16_t CAN_LEG_RADIUS = 11;

            vgl::vector<int16_t> data;

            // Can form.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-CAN_LENGTH1));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(CAN_LENGTH1));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(3);
            data.push_back(15);
            data.push_back(CAN_RADIUS1);
            data.push_back(0);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-CAN_LENGTH1));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-CAN_LENGTH2));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(3);
            data.push_back(15);
            data.push_back(CAN_RADIUS2);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-CAN_LENGTH2));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-CAN_LENGTH3));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(3);
            data.push_back(15);
            data.push_back(CAN_RADIUS3);
            data.push_back(0);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(CAN_LENGTH1));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(CAN_LENGTH2));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(3);
            data.push_back(15);
            data.push_back(CAN_RADIUS2);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(CAN_LENGTH2));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(CAN_LENGTH3));
            data.push_back(CAN_Z_OFFSET);
            data.push_back(3);
            data.push_back(15);
            data.push_back(CAN_RADIUS3);
            data.push_back(0);

            // Legs.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(CAN_LEG_X1);
            data.push_back(CAN_LEG_Y1);
            data.push_back(CAN_LEG_Z1);
            data.push_back(CAN_LEG_X1);
            data.push_back(CAN_LEG_Y2);
            data.push_back(CAN_LEG_Z1);
            data.push_back(3);
            data.push_back(7);
            data.push_back(CAN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(CAN_LEG_X2);
            data.push_back(CAN_LEG_Y1);
            data.push_back(CAN_LEG_Z1);
            data.push_back(CAN_LEG_X2);
            data.push_back(CAN_LEG_Y2);
            data.push_back(CAN_LEG_Z1);
            data.push_back(3);
            data.push_back(7);
            data.push_back(CAN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(CAN_LEG_X1);
            data.push_back(CAN_LEG_Y1);
            data.push_back(CAN_LEG_Z2);
            data.push_back(CAN_LEG_X1);
            data.push_back(CAN_LEG_Y2);
            data.push_back(CAN_LEG_Z2);
            data.push_back(3);
            data.push_back(7);
            data.push_back(CAN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(CAN_LEG_X2);
            data.push_back(CAN_LEG_Y1);
            data.push_back(CAN_LEG_Z2);
            data.push_back(CAN_LEG_X2);
            data.push_back(CAN_LEG_Y2);
            data.push_back(CAN_LEG_Z2);
            data.push_back(3);
            data.push_back(7);
            data.push_back(CAN_LEG_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sign3.hpp", data);
#else
#include "csg_sign3.hpp"
            auto data = CSG_READ_HPP(g_csg_sign3_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sign[3] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sign3", *m_mesh_sign[3]);
#endif
        }

        // Tower generation function.
        auto generate_building = [](vgl::vector<int16_t>& data, int16_t width, int16_t segment_height, int16_t segment_count,
                int16_t segment_frame_width, int16_t segment_frame_height, int16_t balcony_count, int16_t balcony_depth)
        {
            VGL_ASSERT(segment_frame_width >= width);

            int16_t segment_mid_height = static_cast<int16_t>(segment_height - segment_frame_height);
            int16_t segment_mid_width = static_cast<int16_t>(segment_frame_width - balcony_depth * 2);

            for(int16_t ii = 0; (ii < segment_count); ii = static_cast<int16_t>(ii + 1))
            {
                int16_t y1 = static_cast<int16_t>(ii * segment_height);
                int16_t y2 = static_cast<int16_t>(y1 + segment_mid_height);
                int16_t y3 = static_cast<int16_t>(y1 + segment_height);

                // Separator frame.
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(0);
                data.push_back(y3);
                data.push_back(0);
                data.push_back(3);
                data.push_back(segment_frame_width);
                data.push_back(segment_frame_width);
                data.push_back(vgl::CSG_FLAT);

                int16_t frame_width = static_cast<int16_t>((segment_frame_width - width) / 2);
                int16_t pstart = static_cast<int16_t>(-width / 2 + frame_width / 2);
                int16_t pend = static_cast<int16_t>(width / 2 - frame_width / 2);
                int16_t w1 = segment_mid_width;
                int16_t w2 = width;

                if(ii == 0)
                {
                    w1 = width;
                }
                bool lr = static_cast<bool>(dnload_rand() % 2);
                if(lr)
                {
                    vgl::swap(w1, w2);
                }

                // Mid box.
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(0);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(1);
                data.push_back(w1);
                data.push_back(w2);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                // Balcony separators.
                if(ii > 0)
                {
                    // Balcony frames.
                    for(int jj = 0; (jj <= balcony_count); ++jj)
                    {
                        int16_t x1 = vgl::mix(pstart, pend, static_cast<float>(jj) / static_cast<float>(balcony_count));
                        int16_t x2 = x1;
                        int16_t x3 = static_cast<int16_t>(segment_frame_width / 2 - frame_width / 2);
                        int16_t x4 = static_cast<int16_t>(-x3);
                        int16_t z1 = static_cast<int16_t>(segment_frame_width / 2 - balcony_depth / 2);
                        int16_t z2 = static_cast<int16_t>(-z1);
                        int16_t z3 = x1;
                        int16_t z4 = x1;
                        int16_t updir1 = -1;
                        int16_t updir2 = 3;

                        if(lr)
                        {
                            vgl::swap(x1, z1);
                            vgl::swap(x2, z2);
                            vgl::swap(x3, z3);
                            vgl::swap(x4, z4);
                            vgl::swap(updir1, updir2);
                        }

                        data.push_back(to_int16(vgl::CsgCommand::BOX));
                        data.push_back(x1);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(x1);
                        data.push_back(y2);
                        data.push_back(z1);
                        data.push_back(updir1);
                        data.push_back(static_cast<int16_t>(balcony_depth + 1));
                        data.push_back(frame_width);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT);

                        data.push_back(to_int16(vgl::CsgCommand::BOX));
                        data.push_back(x2);
                        data.push_back(y1);
                        data.push_back(z2);
                        data.push_back(x2);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(updir1);
                        data.push_back(static_cast<int16_t>(balcony_depth + 1));
                        data.push_back(frame_width);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_RIGHT);
                    }

                    // Other framing.
                    for(int jj = 0; (jj <= 2); ++jj)
                    {
                        int16_t x1 = vgl::mix(pstart, pend, static_cast<float>(jj) / static_cast<float>(2));
                        int16_t x2 = x1;
                        int16_t x3 = static_cast<int16_t>(segment_frame_width / 2 - frame_width / 2);
                        int16_t x4 = static_cast<int16_t>(-x3);
                        int16_t z1 = static_cast<int16_t>(segment_frame_width / 2 - balcony_depth / 2);
                        int16_t z2 = static_cast<int16_t>(-z1);
                        int16_t z3 = x1;
                        int16_t z4 = x1;
                        int16_t updir1 = -1;
                        int16_t updir2 = 3;

                        if(lr)
                        {
                            vgl::swap(x1, z1);
                            vgl::swap(x2, z2);
                            vgl::swap(x3, z3);
                            vgl::swap(x4, z4);
                            vgl::swap(updir1, updir2);
                        }

                        data.push_back(to_int16(vgl::CsgCommand::BOX));
                        data.push_back(x3);
                        data.push_back(y1);
                        data.push_back(z3);
                        data.push_back(x3);
                        data.push_back(y2);
                        data.push_back(z3);
                        data.push_back(updir2);
                        data.push_back(frame_width);
                        data.push_back(frame_width);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT);

                        data.push_back(to_int16(vgl::CsgCommand::BOX));
                        data.push_back(x4);
                        data.push_back(y1);
                        data.push_back(z4);
                        data.push_back(x4);
                        data.push_back(y2);
                        data.push_back(z4);
                        data.push_back(updir2);
                        data.push_back(frame_width);
                        data.push_back(frame_width);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_RIGHT);
                    }
                }
            }
        };

        // Buildings.
        {
            const int16_t KORTTELI_WIDTH = 1440;
            const int16_t BUILDING_SEGMENT_HEIGHT = 440;
            const int16_t BUILDING_FRAME_WIDTH = 1530;
            const int16_t BUILDING_FRAME_HEIGHT = 240;
            const int16_t BUILDING_BALCONY_INSET = 160;

            for(int ii = 0; (ii < BUILDING_COUNT); ++ii)
            {
                vgl::vector<int16_t> data;

                generate_building(data, KORTTELI_WIDTH, BUILDING_SEGMENT_HEIGHT, static_cast<int16_t>(3 + ii),
                        BUILDING_FRAME_WIDTH, BUILDING_FRAME_HEIGHT, 3, BUILDING_BALCONY_INSET);

                // End for particular building.
                data.push_back(to_int16(vgl::CsgCommand::NONE));
                {
                    vgl::LogicalMesh lmesh(data.data());
                    m_mesh_building[ii] = lmesh.compile();
                }
#if defined(USE_LD)
                addPreviewMesh(("building" + std::to_string(ii)).c_str(), *m_mesh_building[ii]);
#endif
            }
        }

        // Kerava State Building.
        {
#if defined(USE_LD)
            const int16_t KBS_MID_HEIGHT1 = 5600;
            const int16_t KBS_MID_HEIGHT2 = 6700;
            const int16_t KBS_MID_WIDTH1 = 1800;
            const int16_t KBS_MID_DEPTH1 = 900;
            const int16_t KBS_MID_WIDTH2 = 1100;
            const int16_t KBS_MID_WIDTH3 = 900;
            const int16_t KBS_MID_WIDTH4 = 700;
            const int16_t KBS_MID_WIDTH5 = 600;
            const int16_t KBS_MID_WIDTH6 = 220;
            const int16_t KBS_MID_WIDTH7 = 100;
            const int16_t KBS_MID_HEIGHT3 = 6900;
            const int16_t KBS_MID_HEIGHT4 = 7000;
            const int16_t KBS_MID_HEIGHT5 = 7300;
            const int16_t KBS_MID_HEIGHT6 = 7400;
            const int16_t KBS_MID_HEIGHT7 = 8400;

            vgl::vector<int16_t> data;

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT1);
            data.push_back(0);
            data.push_back(3);
            data.push_back(KBS_MID_WIDTH1);
            data.push_back(KBS_MID_DEPTH1);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT2);
            data.push_back(0);
            data.push_back(3);
            data.push_back(KBS_MID_WIDTH2);
            data.push_back(KBS_MID_WIDTH2);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(6);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT2);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH2);
            data.push_back(KBS_MID_WIDTH2);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT3);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH3);
            data.push_back(KBS_MID_WIDTH3);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT4);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH4);
            data.push_back(KBS_MID_WIDTH4);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT5);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH5);
            data.push_back(KBS_MID_WIDTH5);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT6);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH6);
            data.push_back(KBS_MID_WIDTH6);
            data.push_back(0);
            data.push_back(KBS_MID_HEIGHT7);
            data.push_back(0);
            data.push_back(KBS_MID_WIDTH7);
            data.push_back(KBS_MID_WIDTH7);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            csg_update("csg_kerava_state_building.hpp", data);
#else
#include "csg_kerava_state_building.hpp"
            auto data = CSG_READ_HPP(g_csg_kerava_state_building_hpp);
#endif

            auto generate_kbs_blocks = [&data](int xmul, int zmul)
            {
                const int16_t KBS_HEIGHT1 = 800;
                const int16_t KBS_HEIGHT2 = 1800;
                const int16_t KBS_HEIGHT3 = 3000;
                const int16_t KBS_HEIGHT4 = 5000;
                const int16_t KBS_HEIGHT5 = 6100;
                const int16_t KBS_WIDTH1 = 1000;
                const int16_t KBS_WIDTH2 = 780;
                const int16_t KBS_WIDTH3 = 720;
                const int16_t KBS_WIDTH4 = 580;
                const int16_t KBS_WIDTH5 = 500;
                const int16_t KBS_GAP1 = 110;
                const int16_t KBS_GAP2 = 270;
                const int16_t KBS_GAP3 = 180;
                const int16_t KBS_GAP4 = 220;

                auto generate_kbs_block = [xmul, zmul, &data](int16_t h1, int16_t h2, int16_t gap, int16_t width)
                {
                    int16_t offset = static_cast<int16_t>(gap + width / 2);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(offset * xmul));
                    data.push_back(h1);
                    data.push_back(static_cast<int16_t>(offset * zmul));
                    data.push_back(static_cast<int16_t>(offset * xmul));
                    data.push_back(h2);
                    data.push_back(static_cast<int16_t>(offset * zmul));
                    data.push_back(3);
                    data.push_back(width);
                    data.push_back(width);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                };

                generate_kbs_block(0, KBS_HEIGHT1, KBS_GAP1, KBS_WIDTH1);
                generate_kbs_block(KBS_HEIGHT1, KBS_HEIGHT2, KBS_GAP2, KBS_WIDTH2);
                generate_kbs_block(KBS_HEIGHT1, KBS_HEIGHT3, KBS_GAP1, KBS_WIDTH3);
                generate_kbs_block(KBS_HEIGHT3, KBS_HEIGHT4, KBS_GAP3, KBS_WIDTH4);
                generate_kbs_block(KBS_HEIGHT4, KBS_HEIGHT5, KBS_GAP4, KBS_WIDTH5);
            };

            generate_kbs_blocks(1, 1);
            generate_kbs_blocks(1, -1);
            generate_kbs_blocks(-1, 1);
            generate_kbs_blocks(-1, -1);

            // End for particular building.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_kerava_state_building = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("kerava_state_building", *m_mesh_kerava_state_building);
#endif
        }

        // Burj Kerava.
        {
            vgl::vector<int16_t> data;

            auto generate_bk_h2 = [&data](const vgl::vec3& pos, const vgl::vec3& dir)
            {
                const int16_t BK_HEIGHT1 = 3100;
                const int16_t BK_RADIUS1 = 290;
                const int16_t BK_LR1_OFFSETR = 250;
                const int16_t BK_LR1_OFFSETF = 940;

                vgl::vec3 ndir = normalize(dir);
                int16_t dx = static_cast<int16_t>(vgl::iround(ndir.x() * 100.0f));
                int16_t dy = static_cast<int16_t>(vgl::iround(ndir.y() * 100.0f));
                int16_t dz = static_cast<int16_t>(vgl::iround(ndir.z() * 100.0f));

                {
                    const int16_t BK_TRAPEZOID_W1 = 510;
                    const int16_t BK_TRAPEZOID_W2 = 310;
                    const int16_t BK_TRAPEZOID_H1 = 2800;
                    const int16_t BK_TRAPEZOID_H2 = 2400;
                    const int16_t BK_TRAPEZOID_H3 = 2200;
                    const int16_t BK_TRAPEZOID_R1 = 1080;
                    const int16_t BK_TRAPEZOID_R2 = 1340;
                    const int16_t BK_TRAPEZOID_R3 = 1440;

                    vgl::vec3 pos1 = pos + ndir * BK_TRAPEZOID_R1 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H1 / 2);
                    vgl::vec3 pos2 = pos + ndir * BK_TRAPEZOID_R2 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H2 / 2);
                    vgl::vec3 pos3 = pos + ndir * BK_TRAPEZOID_R3 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H3 / 2);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(3);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.z())));
                    data.push_back(BK_TRAPEZOID_W1);
                    data.push_back(BK_TRAPEZOID_H1);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.z())));
                    data.push_back(BK_TRAPEZOID_W1);
                    data.push_back(BK_TRAPEZOID_H2);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.z())));
                    data.push_back(BK_TRAPEZOID_W2);
                    data.push_back(BK_TRAPEZOID_H3);
                    data.push_back(0);
                    data.push_back(dx);
                    data.push_back(dy);
                    data.push_back(dz);
                    data.push_back(2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM);
                }

                vgl::vec3 rt = cross(ndir, vgl::vec3(0.0f, 1.0f, 0.0f));
                vgl::vec3 pos4 = pos + ndir * BK_LR1_OFFSETF + rt * BK_LR1_OFFSETR;
                int16_t p4x = static_cast<int16_t>(vgl::iround(pos4.x()));
                int16_t p4z = static_cast<int16_t>(vgl::iround(pos4.z()));
                vgl::vec3 pos5 = pos + ndir * BK_LR1_OFFSETF - rt * BK_LR1_OFFSETR;
                int16_t p5x = static_cast<int16_t>(vgl::iround(pos5.x()));
                int16_t p5z = static_cast<int16_t>(vgl::iround(pos5.z()));

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(p4x);
                data.push_back(0);
                data.push_back(p4z);
                data.push_back(p4x);
                data.push_back(BK_HEIGHT1);
                data.push_back(p4z);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(6);
                data.push_back(BK_RADIUS1);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(p5x);
                data.push_back(0);
                data.push_back(p5z);
                data.push_back(p5x);
                data.push_back(BK_HEIGHT1);
                data.push_back(p5z);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(6);
                data.push_back(BK_RADIUS1);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                {
                    const int16_t BK_TRAPEZOID_W1 = 560;
                    const int16_t BK_TRAPEZOID_W2 = 320;
                    const int16_t BK_TRAPEZOID_H1 = 5000;
                    const int16_t BK_TRAPEZOID_H2 = 4600;
                    const int16_t BK_TRAPEZOID_H3 = 4200;
                    const int16_t BK_TRAPEZOID_R1 = 440;
                    const int16_t BK_TRAPEZOID_R2 = 800;
                    const int16_t BK_TRAPEZOID_R3 = 980;

                    vgl::vec3 pos1 = pos + ndir * BK_TRAPEZOID_R1 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H1 / 2);
                    vgl::vec3 pos2 = pos + ndir * BK_TRAPEZOID_R2 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H2 / 2);
                    vgl::vec3 pos3 = pos + ndir * BK_TRAPEZOID_R3 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H3 / 2);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(3);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos1.z())));
                    data.push_back(BK_TRAPEZOID_W1);
                    data.push_back(BK_TRAPEZOID_H1);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos2.z())));
                    data.push_back(BK_TRAPEZOID_W1);
                    data.push_back(BK_TRAPEZOID_H2);
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(pos3.z())));
                    data.push_back(BK_TRAPEZOID_W2);
                    data.push_back(BK_TRAPEZOID_H3);
                    data.push_back(0);
                    data.push_back(dx);
                    data.push_back(dy);
                    data.push_back(dz);
                    data.push_back(2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM);
                }
            };

            auto generate_bk_h1 = [generate_bk_h2, &data](const vgl::vec3& dir)
            {
                const int16_t BK_HEIGHT1 = 8400;
                const int16_t BK_HEIGHT2 = 6400;
                const int16_t BK_RADIUS1 = 400;
                const int16_t BK_RADIUS2 = 360;
                const int16_t BK_OFFSET1 = 400;
                const int16_t BK_LR1_OFFSETR = 400;
                const int16_t BK_LR1_OFFSETF = 780;
                const int16_t BK_TRAPEZOID_W1 = 580;
                const int16_t BK_TRAPEZOID_W2 = 320;
                const int16_t BK_TRAPEZOID_H1 = 6200;
                const int16_t BK_TRAPEZOID_H2 = 5400;
                const int16_t BK_TRAPEZOID_H3 = 5180;
                const int16_t BK_TRAPEZOID_R1 = 780;
                const int16_t BK_TRAPEZOID_R2 = 1320;
                const int16_t BK_TRAPEZOID_R3 = 1440;

                vgl::vec3 ndir = normalize(dir);
                int16_t dx = static_cast<int16_t>(vgl::iround(ndir.x() * 100.0f));
                int16_t dy = static_cast<int16_t>(vgl::iround(ndir.y() * 100.0f));
                int16_t dz = static_cast<int16_t>(vgl::iround(ndir.z() * 100.0f));

                vgl::vec3 rt = cross(ndir, vgl::vec3(0.0f, 1.0f, 0.0f));

                const int16_t BK2_FW_MUL = 4;
                const int16_t BK2_RT_MUL = 5;
                vgl::vec3 bk2_pos = ndir * BK_TRAPEZOID_R1;

                generate_bk_h2(bk2_pos, ndir * BK2_FW_MUL + rt * BK2_RT_MUL);
                generate_bk_h2(bk2_pos, ndir * BK2_FW_MUL - rt * BK2_RT_MUL);

                vgl::vec3 pos4 = ndir * BK_TRAPEZOID_R1 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H1 / 2);
                vgl::vec3 pos5 = ndir * BK_TRAPEZOID_R2 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H2 / 2);
                vgl::vec3 pos6 = ndir * BK_TRAPEZOID_R3 + vgl::vec3(0.0f, 1.0f, 0.0f) * (BK_TRAPEZOID_H3 / 2);

                data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                data.push_back(3);
                data.push_back(static_cast<int16_t>(vgl::iround(pos4.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos4.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos4.z())));
                data.push_back(BK_TRAPEZOID_W1);
                data.push_back(BK_TRAPEZOID_H1);
                data.push_back(static_cast<int16_t>(vgl::iround(pos5.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos5.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos5.z())));
                data.push_back(BK_TRAPEZOID_W1);
                data.push_back(BK_TRAPEZOID_H2);
                data.push_back(static_cast<int16_t>(vgl::iround(pos6.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos6.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(pos6.z())));
                data.push_back(BK_TRAPEZOID_W2);
                data.push_back(BK_TRAPEZOID_H3);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(2);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM);

                vgl::vec3 pos2 = ndir * static_cast<float>(BK_LR1_OFFSETF) + rt * static_cast<float>(BK_LR1_OFFSETR);
                int16_t p2x = static_cast<int16_t>(vgl::iround(pos2.x()));
                int16_t p2z = static_cast<int16_t>(vgl::iround(pos2.z()));
                vgl::vec3 pos3 = ndir * static_cast<float>(BK_LR1_OFFSETF) - rt * static_cast<float>(BK_LR1_OFFSETR);
                int16_t p3x = static_cast<int16_t>(vgl::iround(pos3.x()));
                int16_t p3z = static_cast<int16_t>(vgl::iround(pos3.z()));

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(p2x);
                data.push_back(0);
                data.push_back(p2z);
                data.push_back(p2x);
                data.push_back(BK_HEIGHT2);
                data.push_back(p2z);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(8);
                data.push_back(BK_RADIUS2);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(p3x);
                data.push_back(0);
                data.push_back(p3z);
                data.push_back(p3x);
                data.push_back(BK_HEIGHT2);
                data.push_back(p3z);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(8);
                data.push_back(BK_RADIUS2);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                vgl::vec3 pos1 = ndir * static_cast<float>(BK_OFFSET1);
                int16_t p1x = static_cast<int16_t>(vgl::iround(pos1.x()));
                int16_t p1z = static_cast<int16_t>(vgl::iround(pos1.z()));

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(p1x);
                data.push_back(0);
                data.push_back(p1z);
                data.push_back(p1x);
                data.push_back(BK_HEIGHT1);
                data.push_back(p1z);
                data.push_back(0);
                data.push_back(dx);
                data.push_back(dy);
                data.push_back(dz);
                data.push_back(6);
                data.push_back(BK_RADIUS1);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
            };

            generate_bk_h1(vgl::vec3(0.0f, 0.0f, 1.0f));
            generate_bk_h1(vgl::vec3(0.866f, 0.0f, -0.5f));
            generate_bk_h1(vgl::vec3(-0.866f, 0.0f, -0.5f));

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_burj_kerava = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("burj_kerava", *m_mesh_burj_kerava);
#endif
        }

        // John Kerava Center.
        {
            const int16_t JKS_HEIGHT1 = 6600;
            const int16_t JKS_TOLPPA_WIDTH1 = 270;
            const int16_t JKS_TOLPPA_WIDTH2 = 260;
            const int16_t JKS_TOLPPA_WIDTH3 = 200;
            const int16_t JKS_TOLPPA3_COUNT = 13;
            const int16_t JKS_WIDTH1 = 2400;
            const int16_t JKS_DEPTH1 = 3600;
            const int16_t JKS_WIDTH2 = 1600;
            const int16_t JKS_DEPTH2 = 2800;

#if defined(USE_LD)
            const int16_t JKS_HEIGHT2 = 6900;

            vgl::vector<int16_t> data;

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(2);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(JKS_WIDTH1);
            data.push_back(JKS_DEPTH1);
            data.push_back(0);
            data.push_back(JKS_HEIGHT1);
            data.push_back(0);
            data.push_back(JKS_WIDTH2);
            data.push_back(JKS_DEPTH2);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(2);
            data.push_back(0);
            data.push_back(JKS_HEIGHT1);
            data.push_back(0);
            data.push_back(JKS_WIDTH2);
            data.push_back(static_cast<int16_t>(JKS_DEPTH2 + JKS_TOLPPA_WIDTH1 * 2));
            data.push_back(0);
            data.push_back(JKS_HEIGHT2);
            data.push_back(0);
            data.push_back(JKS_WIDTH2);
            data.push_back(JKS_DEPTH2);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(2);
            data.push_back(0);
            data.push_back(JKS_HEIGHT1);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(JKS_WIDTH2 + JKS_TOLPPA_WIDTH1 * 2));
            data.push_back(JKS_DEPTH2);
            data.push_back(0);
            data.push_back(JKS_HEIGHT2);
            data.push_back(0);
            data.push_back(JKS_WIDTH2);
            data.push_back(JKS_DEPTH2);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            csg_update("csg_john_kerava_center.hpp", data);
#else
#include "csg_john_kerava_center.hpp"
            auto data = CSG_READ_HPP(g_csg_john_kerava_center_hpp);
#endif

            auto generate_wall = [&data, JKS_TOLPPA_WIDTH1, JKS_TOLPPA_WIDTH2, JKS_TOLPPA_WIDTH3, JKS_HEIGHT1]
                (int16_t xl1, int16_t xr1, int16_t zl1, int16_t zr1, int16_t xl2, int16_t xr2, int16_t zl2,
                 int16_t zr2, int16_t dir)
                 {
                     data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                     data.push_back(2);
                     data.push_back(xl1);
                     data.push_back(0);
                     data.push_back(zl1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(xl2);
                     data.push_back(JKS_HEIGHT1);
                     data.push_back(zl2);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(2);
                     data.push_back(dir);
                     data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM);

                     data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                     data.push_back(2);
                     data.push_back(xr1);
                     data.push_back(0);
                     data.push_back(zr1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(xr2);
                     data.push_back(JKS_HEIGHT1);
                     data.push_back(zr2);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(JKS_TOLPPA_WIDTH1);
                     data.push_back(2);
                     data.push_back(dir);
                     data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM);

                     data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                     data.push_back(2);
                     data.push_back(xl1);
                     data.push_back(0);
                     data.push_back(zl1);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(xr2);
                     data.push_back(JKS_HEIGHT1);
                     data.push_back(zr2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(2);
                     data.push_back(dir);
                     data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM);

                     data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                     data.push_back(2);
                     data.push_back(xr1);
                     data.push_back(0);
                     data.push_back(zr1);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(xl2);
                     data.push_back(JKS_HEIGHT1);
                     data.push_back(zl2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(JKS_TOLPPA_WIDTH2);
                     data.push_back(2);
                     data.push_back(dir);
                     data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM);

                     for(int ii = 1; (ii < JKS_TOLPPA3_COUNT); ++ii)
                     {
                         float mixer = static_cast<float>(ii) / static_cast<float>(JKS_TOLPPA3_COUNT);
                         int16_t cxl = vgl::mix(xl1, xl2, mixer);
                         int16_t cxr = vgl::mix(xr1, xr2, mixer);
                         int16_t czl = vgl::mix(zl1, zl2, mixer);
                         int16_t czr = vgl::mix(zr1, zr2, mixer);
                         int16_t cy = vgl::mix(static_cast<int16_t>(0), JKS_HEIGHT1, mixer);

                         data.push_back(to_int16(vgl::CsgCommand::BOX));
                         data.push_back(cxl);
                         data.push_back(cy);
                         data.push_back(czl);
                         data.push_back(cxr);
                         data.push_back(cy);
                         data.push_back(czr);
                         data.push_back(dir);
                         data.push_back(JKS_TOLPPA_WIDTH3);
                         data.push_back(JKS_TOLPPA_WIDTH3);
                         data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM);
                     }
                 };
            {
                const int16_t near_x1 = static_cast<int16_t>(JKS_WIDTH1 / 2 - JKS_TOLPPA_WIDTH1 / 2);
                const int16_t near_x2 = static_cast<int16_t>(JKS_WIDTH2 / 2 - JKS_TOLPPA_WIDTH1 / 2);
                const int16_t far_x1 = static_cast<int16_t>(JKS_WIDTH1 / 2 + JKS_TOLPPA_WIDTH1 / 2);
                const int16_t far_x2 = static_cast<int16_t>(JKS_WIDTH2 / 2 + JKS_TOLPPA_WIDTH1 / 2);
                const int16_t near_z1 = static_cast<int16_t>(JKS_DEPTH1 / 2 - JKS_TOLPPA_WIDTH1 / 2);
                const int16_t near_z2 = static_cast<int16_t>(JKS_DEPTH2 / 2 - JKS_TOLPPA_WIDTH1 / 2);
                const int16_t far_z1 = static_cast<int16_t>(JKS_DEPTH1 / 2 + JKS_TOLPPA_WIDTH1 / 2);
                const int16_t far_z2 = static_cast<int16_t>(JKS_DEPTH2 / 2 + JKS_TOLPPA_WIDTH1 / 2);

                generate_wall(static_cast<int16_t>(-near_x1), near_x1, far_z1, far_z1, static_cast<int16_t>(-near_x2), near_x2,
                        far_z2, far_z2, 3);
                generate_wall(static_cast<int16_t>(-near_x1), near_x1, static_cast<int16_t>(-far_z1),
                        static_cast<int16_t>(-far_z1), static_cast<int16_t>(-near_x2), near_x2,
                        static_cast<int16_t>(-far_z2), static_cast<int16_t>(-far_z2), -3);
                generate_wall(far_x1, far_x1, static_cast<int16_t>(-near_z1), near_z1, far_x2, far_x2,
                        static_cast<int16_t>(-near_z2), near_z2, 1);
                generate_wall(static_cast<int16_t>(-far_x1), static_cast<int16_t>(-far_x1),
                        static_cast<int16_t>(-near_z1), near_z1, static_cast<int16_t>(-far_x2),
                        static_cast<int16_t>(-far_x2), static_cast<int16_t>(-near_z2), near_z2, -1);
            }

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_john_kerava_center = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("john_kerava_center", *m_mesh_john_kerava_center);
#endif
        }

        // Keravanas towers.
        {
#if defined(USE_LD)
            const int16_t BRIDGE_X1 = 1200;
            const int16_t BRIDGE_X2 = -1200;
            const int16_t BRIDGE_Y = 3000;
            const int16_t BRIDGE_HT = 300;
            const int16_t BRIDGE_WT = 300;
            const int16_t BRIDGE_SUPPORT_Y1 = 2900;
            const int16_t BRIDGE_SUPPORT_Y2 = 2100;
            const int16_t BRIDGE_SUPPORT_R = 50;

            vgl::vector<int16_t> data;

            // Connecting bridge.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(BRIDGE_X1);
            data.push_back(BRIDGE_Y);
            data.push_back(0);
            data.push_back(BRIDGE_X2);
            data.push_back(BRIDGE_Y);
            data.push_back(0);
            data.push_back(2);
            data.push_back(BRIDGE_HT);
            data.push_back(BRIDGE_WT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(BRIDGE_SUPPORT_Y1);
            data.push_back(0);
            data.push_back(BRIDGE_X1);
            data.push_back(BRIDGE_SUPPORT_Y2);
            data.push_back(0);
            data.push_back(2);
            data.push_back(7);
            data.push_back(BRIDGE_SUPPORT_R);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(BRIDGE_SUPPORT_Y1);
            data.push_back(0);
            data.push_back(BRIDGE_X2);
            data.push_back(BRIDGE_SUPPORT_Y2);
            data.push_back(0);
            data.push_back(2);
            data.push_back(7);
            data.push_back(BRIDGE_SUPPORT_R);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            csg_update("csg_keravanas_towers.hpp", data);
#else
#include "csg_keravanas_towers.hpp"
            auto data = CSG_READ_HPP(g_csg_keravanas_towers_hpp);
#endif

            auto generate_tower = [&data](int16_t px)
            {
                // Generate tower shape.
                // 0: Trapezoid width width.
                // 1: Block height.
                // 2: Tube radius.
                static int16_t KT_BOX_DATA[] =
                {
                    1500, 0, 160,
                    1500, 4500, 160,
                    1490, 500, 140,
                    1460, 500, 120,
                    1400, 500, 100,
                    1150, 500, 80,
                    900, 500, 60,
                    550, 500, 40,
                    100, 500, 20,
                };

                int16_t curry = 0;
                for(unsigned ii = 0; ((ii + 5) < (sizeof(KT_BOX_DATA) / sizeof(int16_t))); ii += 3)
                {
                    int16_t nexty = static_cast<int16_t>(curry + KT_BOX_DATA[ii + 4]);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(2);
                    data.push_back(px);
                    data.push_back(curry);
                    data.push_back(0);
                    data.push_back(KT_BOX_DATA[ii + 0]);
                    data.push_back(KT_BOX_DATA[ii + 0]);
                    data.push_back(px);
                    data.push_back(nexty);
                    data.push_back(0);
                    data.push_back(KT_BOX_DATA[ii + 3]);
                    data.push_back(KT_BOX_DATA[ii + 3]);
                    data.push_back(2);
                    data.push_back(3);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(2);
                    data.push_back(px);
                    data.push_back(curry);
                    data.push_back(0);
                    data.push_back(KT_BOX_DATA[ii + 0]);
                    data.push_back(KT_BOX_DATA[ii + 0]);
                    data.push_back(px);
                    data.push_back(nexty);
                    data.push_back(0);
                    data.push_back(KT_BOX_DATA[ii + 3]);
                    data.push_back(KT_BOX_DATA[ii + 3]);
                    data.push_back(2);
                    data.push_back(0);
                    data.push_back(1);
                    data.push_back(0);
                    data.push_back(1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    for(unsigned jj = 0; (jj < 8); ++jj)
                    {
                        float rad = static_cast<float>(M_PI * 2.0 / 8.0) * static_cast<float>(jj) + static_cast<float>(M_PI / 8.0);
                        float dx = vgl::cos(rad) * 0.6f;
                        float dz = vgl::sin(rad) * 0.6f;
                        int16_t px1 = static_cast<int16_t>(vgl::iround(dx * static_cast<float>(KT_BOX_DATA[ii + 0])) + px);
                        int16_t pz1 = static_cast<int16_t>(vgl::iround(dz * static_cast<float>(KT_BOX_DATA[ii + 0])));
                        int16_t px2 = static_cast<int16_t>(vgl::iround(dx * static_cast<float>(KT_BOX_DATA[ii + 3])) + px);
                        int16_t pz2 = static_cast<int16_t>(vgl::iround(dz * static_cast<float>(KT_BOX_DATA[ii + 3])));

                        data.push_back(to_int16(vgl::CsgCommand::CONE));
                        data.push_back(px1);
                        data.push_back(curry);
                        data.push_back(pz1);
                        data.push_back(px2);
                        data.push_back(nexty);
                        data.push_back(pz2);
                        data.push_back(2);
                        data.push_back(3);
                        data.push_back(8);
                        data.push_back(KT_BOX_DATA[ii + 2]);
                        data.push_back(KT_BOX_DATA[ii + 5]);
                        data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
                    }

                    curry = nexty;
                }

                // Side shape.
                // 0: Width.
                // 1: Y position.
                static int16_t KT_SIDE_SHAPE[]
                {
                    500, 0,
                    500, 3500,
                    460, 100,
                    360, 100,
                    240, 100,
                };

                // Pipe shape.
                // 0: Radius.
                // 1: Y position.
                static int16_t KT_PIPE_SHAPE[]
                {
                    300, 0,
                    300, 3600,
                    250, 110,
                    180, 110,
                    90, 110,
                };

                curry = 0;
                for(unsigned ii = 0; ((ii + 3) < (sizeof(KT_SIDE_SHAPE) / sizeof(int16_t))); ii += 2)
                {
                    const int16_t KT_SIDE_PZ = 1200;
                    const int16_t KT_SIDE_HT = 600;
                    int16_t flags = vgl::CSG_FLAT | vgl::CSG_NO_FRONT | ((ii >= 6) ? 0 : vgl::CSG_NO_BACK);

                    int16_t nexty = static_cast<int16_t>(curry + KT_SIDE_SHAPE[ii + 3]);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(2);
                    data.push_back(px);
                    data.push_back(curry);
                    data.push_back(KT_SIDE_PZ);
                    data.push_back(KT_SIDE_SHAPE[ii + 0]);
                    data.push_back(KT_SIDE_HT);
                    data.push_back(px);
                    data.push_back(nexty);
                    data.push_back(KT_SIDE_PZ);
                    data.push_back(KT_SIDE_SHAPE[ii + 2]);
                    data.push_back(KT_SIDE_HT);
                    data.push_back(2);
                    data.push_back(3);
                    data.push_back(flags | static_cast<int16_t>(vgl::CSG_NO_TOP | vgl::CSG_NO_BOTTOM));

                    curry = nexty;
                }

                curry = 0;
                for(unsigned ii = 0; ((ii + 3) < (sizeof(KT_PIPE_SHAPE) / sizeof(int16_t))); ii += 2)
                {
                    const int16_t KT_PIPE_PZ = 1600;
                    int16_t flags = vgl::CSG_FLAT | vgl::CSG_NO_FRONT | ((ii >= 6) ? 0 : vgl::CSG_NO_BACK);

                    int16_t nexty = static_cast<int16_t>(curry + KT_PIPE_SHAPE[ii + 3]);

                    data.push_back(to_int16(vgl::CsgCommand::CONE));
                    data.push_back(px);
                    data.push_back(curry);
                    data.push_back(KT_PIPE_PZ);
                    data.push_back(px);
                    data.push_back(nexty);
                    data.push_back(KT_PIPE_PZ);
                    data.push_back(2);
                    data.push_back(3);
                    data.push_back(9);
                    data.push_back(KT_PIPE_SHAPE[ii + 0]);
                    data.push_back(KT_PIPE_SHAPE[ii + 2]);
                    data.push_back(flags);

                    curry = nexty;
                }
            };

            generate_tower(-2100);
            generate_tower(2100);

            // End for particular building.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_keravanas_towers = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("keravanas_towers", *m_mesh_keravanas_towers);
#endif
        }

        // Figure shape.
        {
#include "mesh_ukko.hpp"
// These definitions are required because the naming in Warma's mesh is just plain weird.
#define g_vertices_ukko g_vertices_Cube_002
#define g_vertices_ukko_size g_vertices_Cube_002_size
#define g_indices_ukko g_indices_Cube_002
#define g_indices_ukko_size g_indices_Cube_002_size
#define g_weights_ukko g_weights_Cube_002
#define g_bones_ukko_size g_bones_Cube_002_size
            (void)g_bones_Cube_002;
            (void)g_armature_Cube_002;
            (void)g_armature_Cube_002_size;
#define g_animation_ukko_0_swing g_animation_Cube_002_Ukko_Swing_SingleUser
#define g_animation_ukko_0_swing_size g_animation_Cube_002_Ukko_Swing_SingleUser_size
#define g_animation_ukko_0_throw g_animation_Cube_002_Ukko_Throw_SingleUser
#define g_animation_ukko_0_throw_size g_animation_Cube_002_Ukko_Throw_SingleUser_size
#define g_animation_ukko_0_trash g_animation_Cube_002_Ukko_Trash
#define g_animation_ukko_0_trash_size g_animation_Cube_002_Ukko_Trash_size
#define g_animation_ukko_1_swing g_animation_Cube_002_Ukko_001_Swing_SingleUser
#define g_animation_ukko_1_swing_size g_animation_Cube_002_Ukko_001_Swing_SingleUser_size
#define g_animation_ukko_1_throw g_animation_Cube_002_Ukko_001_Throw_SingleUser
#define g_animation_ukko_1_throw_size g_animation_Cube_002_Ukko_001_Throw_SingleUser_size
#define g_animation_ukko_2_swing g_animation_Cube_002_Ukko_002_Swing_SingleUser
#define g_animation_ukko_2_swing_size g_animation_Cube_002_Ukko_002_Swing_SingleUser_size
#define g_animation_ukko_2_throw g_animation_Cube_002_Ukko_002_Throw_SingleUser
#define g_animation_ukko_2_throw_size g_animation_Cube_002_Ukko_002_Throw_SingleUser_size
            const float FIGURE_SCALE = 0.0005f;

            vgl::LogicalMesh lmesh(g_vertices_ukko, g_weights_ukko, g_indices_ukko,
                    g_vertices_ukko_size, g_indices_ukko_size,
                    FIGURE_SCALE);
            m_mesh_ukko = lmesh.compile();
            m_animation_ukko[0] = vgl::Animation::create(g_animation_ukko_0_swing,
                    g_bones_ukko_size, g_animation_ukko_0_swing_size,
                    FIGURE_SCALE);
            m_animation_ukko[1] = vgl::Animation::create(g_animation_ukko_1_swing,
                    g_bones_ukko_size, g_animation_ukko_1_swing_size,
                    FIGURE_SCALE);
            m_animation_ukko[2] = vgl::Animation::create(g_animation_ukko_2_swing,
                    g_bones_ukko_size, g_animation_ukko_2_swing_size,
                    FIGURE_SCALE);
            m_animation_ukko[3] = vgl::Animation::create(g_animation_ukko_0_throw,
                    g_bones_ukko_size, g_animation_ukko_0_throw_size,
                    FIGURE_SCALE);
            m_animation_ukko[4] = vgl::Animation::create(g_animation_ukko_1_throw,
                    g_bones_ukko_size, g_animation_ukko_1_throw_size,
                    FIGURE_SCALE);
            m_animation_ukko[5] = vgl::Animation::create(g_animation_ukko_2_throw,
                    g_bones_ukko_size, g_animation_ukko_2_throw_size,
                    FIGURE_SCALE);
            m_animation_ukko[6] = vgl::Animation::create(g_animation_ukko_0_trash,
                    g_bones_ukko_size, g_animation_ukko_0_trash_size,
                    FIGURE_SCALE);
#if defined(USE_LD)
            addPreviewMesh("ukko", *m_mesh_ukko);
#endif
        }

        // Window genration.
        auto generate_window = [](vgl::vector<int16_t>& data, int16_t x1, int16_t y1, int16_t z1, int16_t x2, int16_t y2,
                int16_t z2, int16_t radius, int16_t roundness = 4)
        {
            int16_t xpos = static_cast<int16_t>((x1 + x2) / 2);
            int16_t zpos = static_cast<int16_t>((z1 + z2) / 2);

            data.push_back(to_int16(vgl::CsgCommand::PIPE));
            data.push_back(6);
            data.push_back(xpos);
            data.push_back(y2);
            data.push_back(zpos);
            data.push_back(x2);
            data.push_back(y2);
            data.push_back(z2);
            data.push_back(x2);
            data.push_back(y1);
            data.push_back(z2);
            data.push_back(x1);
            data.push_back(y1);
            data.push_back(z1);
            data.push_back(x1);
            data.push_back(y2);
            data.push_back(z1);
            data.push_back(xpos);
            data.push_back(y2);
            data.push_back(zpos);
            data.push_back(roundness);
            data.push_back(radius);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
        };

        // Sm5 interior.
        // Based on data from Stadler Flirt wikipedia page and pictures.
        // See: https://de.wikipedia.org/wiki/Stadler_Flirt
        // For the purposes of the inside view, all sizes have been blown tenfold into mm instead of cm.
        {
            const int16_t SM5_WALL_THICKNESS = 12;
            const int16_t SM5_WIDTH = 282;
            const int16_t SM5_HEIGHT = 260;
            const int16_t SM5_CARRIAGE_LENGTH = 1880;
            const int16_t SM5_UPPER_LENGTH = 250;
            const int16_t SM5_FLOOR_LENGTH = static_cast<int16_t>(SM5_CARRIAGE_LENGTH - SM5_UPPER_LENGTH * 2);
            const int16_t SM5_WINDOW_HEIGHT = 122;
            const int16_t SM5_WINDOW_BOTTOM1 = 70;
            const int16_t SM5_WINDOW_BOTTOM2 = 110;
            const int16_t SM5_WINDOW_FRAME = 7;
            const int16_t SM5_WINDOW_FORWARD1 = -460;
            const int16_t SM5_WINDOW_FORWARD2 = static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH - SM5_UPPER_LENGTH * 2) / 2);
            const int16_t SM5_WINDOW_FORWARD3 = -720;
            const int16_t SM5_WINDOW_FORWARD4 = -910;
            const int16_t SM5_WINDOW_BACKWARD1 = -220;
            const int16_t SM5_WINDOW_BACKWARD2 = -30;
            const int16_t SM5_WINDOW_BACKWARD3 = 30;
            const int16_t SM5_WINDOW_BACKWARD4 = 220;
            const int16_t SM5_HATTUHYLLY_EXTENT = 40;
            const int16_t SM5_DOOR_WIDTH = 130;
            const int16_t SM5_DOOR_WINDOW_FRAME = 8;
            const int16_t SM5_DOOR_HEIGHT = 220;
            const int16_t SM5_DOOR_WINDOW_BOTTOM = 60;
            const int16_t SM5_DOOR_WINDOW_TOP = 190;
            const int16_t SM5_DOOR_FRAME_WIDTH = 26;
            const int16_t SM5_DOOR_PIPE_X1 = 48;
            const int16_t SM5_DOOR_PIPE_X2 = 68;
            const int16_t SM5_DOOR_PIPE_YMID = 145;
            const int16_t SM5_PIPE_RADIUS = 5;

#if defined(USE_LD)
            const int16_t SM5_STAIR_BLOCK_HEIGHT = 64;
            const int16_t SM5_STAIR_HEIGHT = 16;
            const int16_t SM5_STAIR_WIDTH = 60;
            const int16_t SM5_STAIR_LENGTH = 17;
            const int16_t SM5_FLOOR_LINE_X = static_cast<int16_t>(SM5_STAIR_WIDTH * 2 / 3);
            const int16_t SM5_FLOOR_LINE_Y = static_cast<int16_t>(-SM5_WINDOW_FRAME * 2 / 3);
            const int16_t SM5_CEILING_LINE_Y = static_cast<int16_t>(SM5_HEIGHT + (SM5_WINDOW_FRAME * 2 / 3));

            vgl::vector<int16_t> data;

            // Floor.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-SM5_WALL_THICKNESS / 2));
            data.push_back(static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 2) - 1));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-SM5_WALL_THICKNESS / 2));
            data.push_back(static_cast<int16_t>((SM5_FLOOR_LENGTH / 2) + 1));
            data.push_back(2);
            data.push_back(static_cast<int16_t>(SM5_WIDTH + SM5_WALL_THICKNESS));
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Ceiling.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT + SM5_WALL_THICKNESS / 2));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT + SM5_WALL_THICKNESS / 2));
            data.push_back(static_cast<int16_t>(SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(static_cast<int16_t>(SM5_WIDTH + SM5_WALL_THICKNESS));
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Back stair sides.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 4) - (SM5_STAIR_WIDTH / 4)));
            data.push_back(static_cast<int16_t>(SM5_STAIR_BLOCK_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 4) - (SM5_STAIR_WIDTH / 4)));
            data.push_back(static_cast<int16_t>(SM5_STAIR_BLOCK_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(static_cast<int16_t>(((SM5_WIDTH - SM5_STAIR_WIDTH) / 2)));
            data.push_back(SM5_STAIR_BLOCK_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 4) + (SM5_STAIR_WIDTH / 4)));
            data.push_back(static_cast<int16_t>(SM5_STAIR_BLOCK_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 4) + (SM5_STAIR_WIDTH / 4)));
            data.push_back(static_cast<int16_t>(SM5_STAIR_BLOCK_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(static_cast<int16_t>(((SM5_WIDTH - SM5_STAIR_WIDTH) / 2)));
            data.push_back(SM5_STAIR_BLOCK_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_RIGHT);

            // Back stairs.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_STAIR_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD2);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_STAIR_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - SM5_STAIR_LENGTH));
            data.push_back(2);
            data.push_back(SM5_STAIR_WIDTH);
            data.push_back(SM5_STAIR_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + SM5_STAIR_HEIGHT));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - SM5_STAIR_LENGTH));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + SM5_STAIR_HEIGHT));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - (SM5_STAIR_LENGTH * 2)));
            data.push_back(2);
            data.push_back(SM5_STAIR_WIDTH);
            data.push_back(SM5_STAIR_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + (SM5_STAIR_HEIGHT * 2)));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - (SM5_STAIR_LENGTH * 2)));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + (SM5_STAIR_HEIGHT * 2)));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - (SM5_STAIR_LENGTH * 3)));
            data.push_back(2);
            data.push_back(SM5_STAIR_WIDTH);
            data.push_back(SM5_STAIR_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + (SM5_STAIR_HEIGHT * 3)));
            data.push_back(static_cast<int16_t>(SM5_WINDOW_FORWARD2 - (SM5_STAIR_LENGTH * 3)));
            data.push_back(0);
            data.push_back(static_cast<int16_t>((SM5_STAIR_HEIGHT / 2) + (SM5_STAIR_HEIGHT * 3)));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(SM5_STAIR_WIDTH);
            data.push_back(SM5_STAIR_HEIGHT);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Back wall.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + 1));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(2);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT + 1));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) + 1));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(2);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT + 1));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP | vgl::CSG_NO_LEFT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(SM5_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT * 8 / 9));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(3);
            data.push_back(static_cast<int16_t>(SM5_FLOOR_LINE_X * 2));
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(SM5_STAIR_BLOCK_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(3);
            data.push_back(static_cast<int16_t>(SM5_FLOOR_LINE_X * 2));
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_STAIR_BLOCK_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(3);
            data.push_back(9);
            data.push_back(SM5_DOOR_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_STAIR_BLOCK_HEIGHT);
            data.push_back(static_cast<int16_t>(-(SM5_CARRIAGE_LENGTH / 2) - (SM5_WALL_THICKNESS / 4)));
            data.push_back(3);
            data.push_back(9);
            data.push_back(SM5_DOOR_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            // Floor lines.
            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_FLOOR_LINE_Y);
            data.push_back(static_cast<int16_t>(SM5_FLOOR_LENGTH / 2));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_FLOOR_LINE_Y);
            data.push_back(static_cast<int16_t>(-SM5_FLOOR_LENGTH / 2));
            data.push_back(2);
            data.push_back(9);
            data.push_back(SM5_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_FLOOR_LINE_Y);
            data.push_back(static_cast<int16_t>(SM5_FLOOR_LENGTH / 2));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_FLOOR_LINE_Y);
            data.push_back(static_cast<int16_t>(-SM5_FLOOR_LENGTH / 2));
            data.push_back(2);
            data.push_back(9);
            data.push_back(SM5_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_CEILING_LINE_Y);
            data.push_back(static_cast<int16_t>(SM5_CARRIAGE_LENGTH / 2));
            data.push_back(SM5_FLOOR_LINE_X);
            data.push_back(SM5_CEILING_LINE_Y);
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(9);
            data.push_back(SM5_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_CEILING_LINE_Y);
            data.push_back(static_cast<int16_t>(SM5_CARRIAGE_LENGTH / 2));
            data.push_back(-SM5_FLOOR_LINE_X);
            data.push_back(SM5_CEILING_LINE_Y);
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(2);
            data.push_back(9);
            data.push_back(SM5_WINDOW_FRAME);
            data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

            // Walls toward first windows.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4) - (SM5_DOOR_WIDTH / 2) - SM5_DOOR_FRAME_WIDTH));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD1);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4) - (SM5_DOOR_WIDTH / 2) - SM5_DOOR_FRAME_WIDTH));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD1);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Walls toward second windows.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD2);
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD3);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD2);
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD3);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Walls towards end.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD4);
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_FORWARD4);
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-SM5_CARRIAGE_LENGTH / 2));
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Walls towards back windows.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4) + (SM5_DOOR_WIDTH / 2) + SM5_DOOR_FRAME_WIDTH));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD1);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4) + (SM5_DOOR_WIDTH / 2) + SM5_DOOR_FRAME_WIDTH));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD1);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            // Walls between back windows.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD3);
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD2);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD3);
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD2);
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD4);
            data.push_back(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>((SM5_FLOOR_LENGTH / 4) - (SM5_DOOR_WIDTH / 2) - SM5_DOOR_FRAME_WIDTH));
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(SM5_WINDOW_BACKWARD4);
            data.push_back(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)));
            data.push_back(static_cast<int16_t>(SM5_HEIGHT / 2));
            data.push_back(static_cast<int16_t>((SM5_FLOOR_LENGTH / 4) - (SM5_DOOR_WIDTH / 2) - SM5_DOOR_FRAME_WIDTH));
            data.push_back(1);
            data.push_back(SM5_HEIGHT);
            data.push_back(SM5_WALL_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

            csg_update("csg_sm5_interior.hpp", data);
#else
#include "csg_sm5_interior.hpp"
            auto data = CSG_READ_HPP(g_csg_sm5_interior_hpp);
#endif

            auto generate_sm5_window = [SM5_HEIGHT, SM5_WALL_THICKNESS, SM5_WINDOW_FRAME, generate_window, &data]
                     (int16_t xpos, int16_t ypos, int16_t z1, int16_t z2, bool left)
            {
                int16_t zmid = static_cast<int16_t>((z1 + z2) / 2);
                int16_t y2 = static_cast<int16_t>(ypos + SM5_WINDOW_HEIGHT);

                generate_window(data, xpos, ypos, z1, xpos, y2, z2, SM5_WINDOW_FRAME, 9);

                // Below and above window.
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(ypos);
                data.push_back(zmid);
                data.push_back(xpos);
                data.push_back(0);
                data.push_back(zmid);
                data.push_back(1);
                data.push_back(static_cast<int16_t>(abs(z2 - z1) + 1));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(y2);
                data.push_back(zmid);
                data.push_back(xpos);
                data.push_back(SM5_HEIGHT);
                data.push_back(zmid);
                data.push_back(1);
                data.push_back(static_cast<int16_t>(abs(z2 - z1) + 1));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

                int16_t hattuhylly_extent = static_cast<int16_t>(left ? SM5_HATTUHYLLY_EXTENT : -SM5_HATTUHYLLY_EXTENT);
                int16_t hattuhylly_x = static_cast<int16_t>(xpos + hattuhylly_extent / 2);

                // Hattuhylly.
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(hattuhylly_x);
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT / 2)));
                data.push_back(z1);
                data.push_back(hattuhylly_x);
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT / 2)));
                data.push_back(z2);
                data.push_back(0);
                data.push_back(left ? -1 : 1);
                data.push_back(3);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(SM5_HATTUHYLLY_EXTENT * 4 / 3));
                data.push_back(SM5_WINDOW_FRAME);
                data.push_back(vgl::CSG_FLAT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT * 3 / 2)));
                data.push_back(static_cast<int16_t>(z1 - SM5_WINDOW_FRAME));
                data.push_back(static_cast<int16_t>(xpos + hattuhylly_extent));
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT * 5 / 8)));
                data.push_back(static_cast<int16_t>(z1 - SM5_WINDOW_FRAME));
                data.push_back(3);
                data.push_back(9);
                data.push_back(static_cast<int16_t>(SM5_WINDOW_FRAME / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT * 3 / 2)));
                data.push_back(static_cast<int16_t>(z2 + SM5_WINDOW_FRAME));
                data.push_back(static_cast<int16_t>(xpos + hattuhylly_extent));
                data.push_back(static_cast<int16_t>(y2 + (SM5_HATTUHYLLY_EXTENT * 5 / 8)));
                data.push_back(static_cast<int16_t>(z2 + SM5_WINDOW_FRAME));
                data.push_back(3);
                data.push_back(9);
                data.push_back(static_cast<int16_t>(SM5_WINDOW_FRAME / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            };

            // Front windows.
            generate_sm5_window(static_cast<int16_t>(-SM5_WIDTH / 2 - SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_FORWARD1, SM5_WINDOW_FORWARD2, true);

            generate_sm5_window(static_cast<int16_t>(SM5_WIDTH / 2 + SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_FORWARD1, SM5_WINDOW_FORWARD2, false);

            // First windows.
            generate_sm5_window(static_cast<int16_t>(-SM5_WIDTH / 2 - SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM2,
                    SM5_WINDOW_FORWARD3, SM5_WINDOW_FORWARD4, true);

            generate_sm5_window(static_cast<int16_t>(SM5_WIDTH / 2 + SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM2,
                    SM5_WINDOW_FORWARD3, SM5_WINDOW_FORWARD4, false);

            // Second windows.
            generate_sm5_window(static_cast<int16_t>(-SM5_WIDTH / 2 - SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_BACKWARD2, SM5_WINDOW_BACKWARD1, true);

            generate_sm5_window(static_cast<int16_t>(SM5_WIDTH / 2 + SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_BACKWARD2, SM5_WINDOW_BACKWARD1, false);

            // Back windows.
            generate_sm5_window(static_cast<int16_t>(-SM5_WIDTH / 2 - SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_BACKWARD4, SM5_WINDOW_BACKWARD3, true);

            generate_sm5_window(static_cast<int16_t>(SM5_WIDTH / 2 + SM5_WALL_THICKNESS / 2), SM5_WINDOW_BOTTOM1,
                    SM5_WINDOW_BACKWARD4, SM5_WINDOW_BACKWARD3, false);

            auto generate_door = [SM5_WALL_THICKNESS, &data]
                     (int16_t xpos, int16_t zpos)
            {
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(0);
                data.push_back(zpos);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_BOTTOM));
                data.push_back(zpos);
                data.push_back(1);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WIDTH / 2));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_BOTTOM));
                data.push_back(static_cast<int16_t>(zpos - (SM5_DOOR_WIDTH / 4) + (SM5_DOOR_WINDOW_FRAME / 2)));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_TOP));
                data.push_back(static_cast<int16_t>(zpos - (SM5_DOOR_WIDTH / 4) + (SM5_DOOR_WINDOW_FRAME / 2)));
                data.push_back(1);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_FRAME));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_BOTTOM));
                data.push_back(static_cast<int16_t>(zpos + (SM5_DOOR_WIDTH / 4) - (SM5_DOOR_WINDOW_FRAME / 2)));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_TOP));
                data.push_back(static_cast<int16_t>(zpos + (SM5_DOOR_WIDTH / 4) - (SM5_DOOR_WINDOW_FRAME / 2)));
                data.push_back(1);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_FRAME));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WINDOW_TOP));
                data.push_back(zpos);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_HEIGHT));
                data.push_back(zpos);
                data.push_back(1);
                data.push_back(static_cast<int16_t>(SM5_DOOR_WIDTH / 2));
                data.push_back(SM5_WALL_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);
            };

            auto generate_door_palkit = [SM5_HEIGHT, SM5_DOOR_HEIGHT, SM5_DOOR_FRAME_WIDTH, SM5_DOOR_PIPE_YMID,
                 SM5_PIPE_RADIUS, generate_door, &data]
                (int16_t xpos, int16_t zpos, bool left)
            {
                generate_door(xpos, static_cast<int16_t>(zpos - (SM5_DOOR_WIDTH / 4)));
                generate_door(xpos, static_cast<int16_t>(zpos + (SM5_DOOR_WIDTH / 4)));

                int16_t z1 = static_cast<int16_t>(zpos - (SM5_DOOR_WIDTH / 2) - (SM5_DOOR_FRAME_WIDTH / 2));
                int16_t z2 = static_cast<int16_t>(zpos + (SM5_DOOR_WIDTH / 2) + (SM5_DOOR_FRAME_WIDTH / 2));

                int16_t palkki_inlet = static_cast<int16_t>((SM5_DOOR_FRAME_WIDTH / 2) - (SM5_WALL_THICKNESS / 2));
                int16_t palkki_x = static_cast<int16_t>(xpos + (left ? palkki_inlet : -palkki_inlet));

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(palkki_x);
                data.push_back(0);
                data.push_back(z1);
                data.push_back(palkki_x);
                data.push_back(SM5_DOOR_HEIGHT);
                data.push_back(z1);
                data.push_back(1);
                data.push_back(SM5_DOOR_FRAME_WIDTH);
                data.push_back(SM5_DOOR_FRAME_WIDTH);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(palkki_x);
                data.push_back(0);
                data.push_back(z2);
                data.push_back(palkki_x);
                data.push_back(static_cast<int16_t>(SM5_DOOR_HEIGHT));
                data.push_back(z2);
                data.push_back(1);
                data.push_back(SM5_DOOR_FRAME_WIDTH);
                data.push_back(SM5_DOOR_FRAME_WIDTH);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                int16_t pipe_x1 = static_cast<int16_t>(xpos + (left ? SM5_DOOR_PIPE_X1 : -SM5_DOOR_PIPE_X1));
                int16_t pipe_x2 = static_cast<int16_t>(xpos + (left ? SM5_DOOR_PIPE_X2 : -SM5_DOOR_PIPE_X2));

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(3);
                data.push_back(pipe_x1);
                data.push_back(SM5_HEIGHT);
                data.push_back(z1);
                data.push_back(pipe_x2);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z1);
                data.push_back(pipe_x2);
                data.push_back(0);
                data.push_back(z1);
                data.push_back(11);
                data.push_back(SM5_PIPE_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(3);
                data.push_back(pipe_x1);
                data.push_back(SM5_HEIGHT);
                data.push_back(z2);
                data.push_back(pipe_x2);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z2);
                data.push_back(pipe_x2);
                data.push_back(0);
                data.push_back(z2);
                data.push_back(11);
                data.push_back(SM5_PIPE_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(xpos);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z1);
                data.push_back(pipe_x2);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z1);
                data.push_back(2);
                data.push_back(11);
                data.push_back(SM5_PIPE_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(xpos);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z2);
                data.push_back(pipe_x2);
                data.push_back(SM5_DOOR_PIPE_YMID);
                data.push_back(z2);
                data.push_back(2);
                data.push_back(11);
                data.push_back(SM5_PIPE_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                // Wedge.
                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_HEIGHT + (SM5_DOOR_FRAME_WIDTH * 5 / 2)));
                data.push_back(static_cast<int16_t>(zpos - (SM5_DOOR_WIDTH / 2) - SM5_DOOR_FRAME_WIDTH));
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(SM5_DOOR_HEIGHT + (SM5_DOOR_FRAME_WIDTH * 5 / 2)));
                data.push_back(static_cast<int16_t>(zpos + (SM5_DOOR_WIDTH / 2) + SM5_DOOR_FRAME_WIDTH));
                data.push_back(0);
                data.push_back(left ? 3 : -3);
                data.push_back(2);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(SM5_DOOR_FRAME_WIDTH * 10 / 2));
                data.push_back(static_cast<int16_t>(SM5_DOOR_FRAME_WIDTH * 10 / 2));
                data.push_back(vgl::CSG_FLAT);
            };

            generate_door_palkit(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)),
                    static_cast<int16_t>(SM5_FLOOR_LENGTH / 4), false);
            generate_door_palkit(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)),
                    static_cast<int16_t>(SM5_FLOOR_LENGTH / 4), true);
            generate_door_palkit(static_cast<int16_t>((SM5_WIDTH / 2) + (SM5_WALL_THICKNESS / 2)),
                    static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4)), false);
            generate_door_palkit(static_cast<int16_t>(-(SM5_WIDTH / 2) - (SM5_WALL_THICKNESS / 2)),
                    static_cast<int16_t>(-(SM5_FLOOR_LENGTH / 4)), true);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sm5_interior = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sm5_interior", *m_mesh_sm5_interior);
#endif
        }

        // Sm5 chair.
        {
#if defined(USE_LD)
            const int16_t SM5_CHAIR_SITZ_HEIGHT = 50;
            const int16_t SM5_CHAIR_SITZ_THICKNESS = 10;
            const int16_t SM5_CHAIR_SITZ_SIZE = 47;
            const int16_t SM5_CHAIR_SITZ_BASE_HEIGHT = static_cast<int16_t>(SM5_CHAIR_SITZ_HEIGHT - SM5_CHAIR_SITZ_THICKNESS);
            const int16_t SM5_CHAIR_SITZ_BASE_SIZE = 20;
            const int16_t SM5_CHAIR_BACK_HEIGHT1 = static_cast<int16_t>(SM5_CHAIR_SITZ_HEIGHT + 50);
            const int16_t SM5_CHAIR_BACK_HEIGHT2 = static_cast<int16_t>(SM5_CHAIR_SITZ_HEIGHT + 70);
            const int16_t SM5_CHAIR_BACK_INCLINATION = 10;
            const int16_t SM5_CHAIR_HANDHOLD = 75;
            const int16_t SM5_CHAIR_HANDHOLD_SIZE1 = 6;
            const int16_t SM5_CHAIR_HANDHOLD_SIZE2 = 4;

            vgl::vector<int16_t> data;

            // Base.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
            data.push_back(SM5_CHAIR_SITZ_BASE_HEIGHT);
            data.push_back(0);
            data.push_back(3);
            data.push_back(SM5_CHAIR_SITZ_BASE_SIZE);
            data.push_back(SM5_CHAIR_SITZ_BASE_SIZE);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            // Seat.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(SM5_CHAIR_SITZ_BASE_HEIGHT);
            data.push_back(0);
            data.push_back(0);
            data.push_back(SM5_CHAIR_SITZ_HEIGHT);
            data.push_back(0);
            data.push_back(3);
            data.push_back(SM5_CHAIR_SITZ_SIZE);
            data.push_back(SM5_CHAIR_SITZ_SIZE);
            data.push_back(vgl::CSG_NO_TOP);

            // Back.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(SM5_CHAIR_SITZ_BASE_HEIGHT);
            data.push_back(static_cast<int16_t>(SM5_CHAIR_SITZ_SIZE / 2));
            data.push_back(0);
            data.push_back(SM5_CHAIR_BACK_HEIGHT1);
            data.push_back(static_cast<int16_t>((SM5_CHAIR_SITZ_SIZE / 2) + SM5_CHAIR_BACK_INCLINATION));
            data.push_back(3);
            data.push_back(SM5_CHAIR_SITZ_SIZE - 1);
            data.push_back(SM5_CHAIR_SITZ_THICKNESS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(SM5_CHAIR_BACK_HEIGHT1 - 10);
            data.push_back(static_cast<int16_t>((SM5_CHAIR_SITZ_SIZE / 2) + SM5_CHAIR_BACK_INCLINATION));
            data.push_back(0);
            data.push_back(SM5_CHAIR_BACK_HEIGHT2);
            data.push_back(static_cast<int16_t>((SM5_CHAIR_SITZ_SIZE / 2) + SM5_CHAIR_BACK_INCLINATION));
            data.push_back(3);
            data.push_back(SM5_CHAIR_SITZ_SIZE);
            data.push_back(SM5_CHAIR_SITZ_THICKNESS);
            data.push_back(vgl::CSG_NO_FRONT);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sm5_chair.hpp", data);
#else
#include "csg_sm5_chair.hpp"
            auto data = CSG_READ_HPP(g_csg_sm5_chair_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sm5_chair[1] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sm5_chair", *m_mesh_sm5_chair[1]);
#endif

#if defined(USE_LD)
            // Add left handedness.
            data.pop_back();

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-SM5_CHAIR_SITZ_SIZE / 2));
            data.push_back(SM5_CHAIR_HANDHOLD);
            data.push_back(static_cast<int16_t>((SM5_CHAIR_SITZ_SIZE / 2) + SM5_CHAIR_BACK_INCLINATION / 2));
            data.push_back(static_cast<int16_t>(-SM5_CHAIR_SITZ_SIZE / 2));
            data.push_back(SM5_CHAIR_HANDHOLD);
            data.push_back(static_cast<int16_t>((SM5_CHAIR_SITZ_SIZE / 2) - SM5_CHAIR_BACK_INCLINATION * 3));
            data.push_back(2);
            data.push_back(SM5_CHAIR_HANDHOLD_SIZE1);
            data.push_back(SM5_CHAIR_HANDHOLD_SIZE2);
            data.push_back(0);

            // End.
            data.push_back(to_int16(vgl::CsgCommand::NONE));

            csg_update("csg_sm5_chair_l.hpp", data);
#else
#include "csg_sm5_chair_l.hpp"
            data = CSG_READ_HPP(g_csg_sm5_chair_l_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sm5_chair[0] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sm5_chair_l", *m_mesh_sm5_chair[0]);
#endif

#if defined(USE_LD)
            // Mirror handhold.
            data[data.size() - 8] = static_cast<int16_t>(SM5_CHAIR_SITZ_SIZE / 2);
            data[data.size() - 11] = static_cast<int16_t>(SM5_CHAIR_SITZ_SIZE / 2);

            csg_update("csg_sm5_chair_r.hpp", data);
#else
#include "csg_sm5_chair_r.hpp"
            data = CSG_READ_HPP(g_csg_sm5_chair_r_hpp);
#endif
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_sm5_chair[2] = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("sm5_chair_r", *m_mesh_sm5_chair[2]);
#endif
        }

        // Train.
        // Loosely based on Siemens Vectron, but with sm5 mixed in. Or something like that.
        // https://assets.new.siemens.com/siemens/assets/api/uuid:187443e1-48ca-449e-9542-31b8c449f454/mo-vectron-technikbroschuere-de.pdf
        {
            const int16_t TRAIN_LENGTH = 1900;
            const int16_t TRAIN_WIDTH = 302;
            const int16_t TRAIN_HEIGHT = 426;
#if defined(USE_LD)
            const int16_t TRAIN_BOTTOM_HEIGHT = 110;
#endif

            // Train wheels.
            auto generate_wheels = [](vgl::vector<int16_t>& data, int16_t zpos)
            {
                const int16_t RAIL_DIFFERENCE = 152;
                const int16_t WHEEL_THICKNESS = 20;
                const int16_t WHEEL_RADIUS = 74;
                const int16_t AXIS_RADIUS = 24;
                const int16_t WHEEL_BOX_WIDTH = 64;
                const int16_t WHEEL_BOX_THICKNESS = 38;

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(RAIL_DIFFERENCE / 2 + WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(RAIL_DIFFERENCE / 2 - WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(2);
                data.push_back(18);
                data.push_back(WHEEL_RADIUS);
                data.push_back(0);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-RAIL_DIFFERENCE / 2 - WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(-RAIL_DIFFERENCE / 2 + WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(2);
                data.push_back(18);
                data.push_back(WHEEL_RADIUS);
                data.push_back(0);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-RAIL_DIFFERENCE / 2 + WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(RAIL_DIFFERENCE / 2 - WHEEL_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS));
                data.push_back(zpos);
                data.push_back(2);
                data.push_back(9);
                data.push_back(AXIS_RADIUS);
                data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(-RAIL_DIFFERENCE / 2 - WHEEL_THICKNESS / 2 - WHEEL_BOX_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS - WHEEL_BOX_WIDTH / 2));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(-RAIL_DIFFERENCE / 2 - WHEEL_THICKNESS / 2 - WHEEL_BOX_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS * 2));
                data.push_back(zpos);
                data.push_back(3);
                data.push_back(WHEEL_BOX_THICKNESS);
                data.push_back(WHEEL_BOX_WIDTH);
                data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_RIGHT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(RAIL_DIFFERENCE / 2 + WHEEL_THICKNESS / 2 + WHEEL_BOX_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS - WHEEL_BOX_WIDTH / 2));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(RAIL_DIFFERENCE / 2 + WHEEL_THICKNESS / 2 + WHEEL_BOX_THICKNESS / 2));
                data.push_back(static_cast<int16_t>(WHEEL_RADIUS * 2));
                data.push_back(zpos);
                data.push_back(3);
                data.push_back(WHEEL_BOX_THICKNESS);
                data.push_back(WHEEL_BOX_WIDTH);
                data.push_back(vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT);
            };

            // Doors.
            auto generate_door = [](vgl::vector<int16_t>& data, int16_t ypos, int16_t zpos, int16_t width, int16_t height)
            {
                const int16_t DOOR_INSET2 = 12;
                const int16_t DOOR_INSET1 = 1;
                const int16_t DOOR_FRAME_RADIUS = 10;

                int16_t z1 = static_cast<int16_t>(zpos + width / 2);
                int16_t z2 = static_cast<int16_t>(zpos - width / 2);

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(4);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos + height / 2 + DOOR_INSET2 * 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos - height / 2 - DOOR_INSET2 * 2));
                data.push_back(z1);
                data.push_back(9);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(4);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos + height / 2 + DOOR_INSET2 * 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos - height / 2 - DOOR_INSET2 * 2));
                data.push_back(z2);
                data.push_back(9);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(4);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos + height / 2 + DOOR_INSET2 * 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos - height / 2 - DOOR_INSET2 * 2));
                data.push_back(z1);
                data.push_back(9);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(4);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos + height / 2 + DOOR_INSET2 * 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z2);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET2));
                data.push_back(static_cast<int16_t>(ypos - height / 2 - DOOR_INSET2 * 2));
                data.push_back(z2);
                data.push_back(9);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos - height / 2));
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z1);
                data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - DOOR_INSET1));
                data.push_back(static_cast<int16_t>(ypos + height / 2));
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(DOOR_FRAME_RADIUS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            };

            // Junan yhdistys osa.
            auto generate_yhdistys = [](vgl::vector<int16_t>& data, int16_t zpos, bool dir)
            {
                const int16_t Y_OFFSET = 104;
                const int16_t SEPARATION = 80;
                const int16_t LENGTH1 = 48;
                const int16_t LENGTH2 = 60;
                const int16_t BLOCK_WIDTH = 50;
                const int16_t PALKKI_RADIUS = 14;

                int16_t z0 = static_cast<int16_t>(zpos + (dir ? LENGTH1 / 3 : -LENGTH1 / 3));
                int16_t z1 = zpos;
                int16_t z2 = static_cast<int16_t>(zpos + (dir ? -LENGTH1 : LENGTH1));
                int16_t z3 = static_cast<int16_t>(zpos + (dir ? -LENGTH2 : LENGTH2));

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(Y_OFFSET);
                data.push_back(z1);
                data.push_back(0);
                data.push_back(Y_OFFSET);
                data.push_back(z3);
                data.push_back(2);
                data.push_back(BLOCK_WIDTH);
                data.push_back(BLOCK_WIDTH);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(SEPARATION);
                data.push_back(Y_OFFSET);
                data.push_back(z0);
                data.push_back(0);
                data.push_back(Y_OFFSET);
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(PALKKI_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(-SEPARATION));
                data.push_back(Y_OFFSET);
                data.push_back(z0);
                data.push_back(0);
                data.push_back(Y_OFFSET);
                data.push_back(z2);
                data.push_back(2);
                data.push_back(7);
                data.push_back(PALKKI_RADIUS);
                data.push_back(vgl::CSG_NO_FRONT);
            };

            {
                const int16_t TRAIN_FRONT_TRAPEZOID_LENGTH = 100;
                const int16_t TRAIN_BACK_TRAPEZOID_LENGTH = 80;
                const int16_t TRAIN_ROOF_TRAPEZOID1_HEIGHT = 70;
                const int16_t TRAIN_ROOF_HEIGHT = static_cast<int16_t>(TRAIN_HEIGHT - TRAIN_ROOF_TRAPEZOID1_HEIGHT);
                const int16_t TRAIN_FRONT = static_cast<int16_t>(TRAIN_LENGTH / 2 - TRAIN_FRONT_TRAPEZOID_LENGTH);
                const int16_t TRAIN_BACK = static_cast<int16_t>(-TRAIN_LENGTH / 2 + TRAIN_BACK_TRAPEZOID_LENGTH);

#if defined(USE_LD)
                const int16_t TRAIN_BOTTOM_TRAPEZOID_HEIGHT = 50;

                vgl::vector<int16_t> data;

                {
                    int16_t WIDTH_DEC_1 = 20;
                    int16_t WIDTH_DEC_2 = 30;

                    const int16_t ypos = vgl::mix(TRAIN_ROOF_HEIGHT, TRAIN_BOTTOM_HEIGHT, 0.5f);
                    const int16_t ht = static_cast<int16_t>(TRAIN_ROOF_HEIGHT - TRAIN_BOTTOM_HEIGHT);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(ypos);
                    data.push_back(TRAIN_FRONT + 1);
                    data.push_back(0);
                    data.push_back(ypos);
                    data.push_back(TRAIN_BACK - 1);
                    data.push_back(2);
                    data.push_back(TRAIN_WIDTH);
                    data.push_back(ht + 1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP);

                    // Front and back.
                    {
                        int16_t HEIGHT_DEC_1 = 40;
                        int16_t HEIGHT_DEC_2 = 60;

                        int16_t y1 = static_cast<int16_t>(ypos - TRAIN_BOTTOM_TRAPEZOID_HEIGHT / 2);
                        int16_t z1 = TRAIN_FRONT;
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = static_cast<int16_t>(ht + TRAIN_BOTTOM_TRAPEZOID_HEIGHT);
                        int16_t y2 = static_cast<int16_t>(y1 - HEIGHT_DEC_1 / 2);
                        int16_t z2 = static_cast<int16_t>(z1 + TRAIN_FRONT_TRAPEZOID_LENGTH / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC_1);
                        int16_t h2 = static_cast<int16_t>(h1 - HEIGHT_DEC_1);
                        int16_t y3 = static_cast<int16_t>(y2 - HEIGHT_DEC_2 / 2);
                        int16_t z3 = static_cast<int16_t>(z2 + TRAIN_FRONT_TRAPEZOID_LENGTH / 2);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC_2);
                        int16_t h3 = static_cast<int16_t>(h2 - HEIGHT_DEC_2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(3);
                        data.push_back(2);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }

                    {
                        int16_t HEIGHT_DEC_1 = 50;
                        int16_t HEIGHT_DEC_2 = 70;

                        int16_t y1 = static_cast<int16_t>(ypos - TRAIN_BOTTOM_TRAPEZOID_HEIGHT / 2);
                        int16_t z1 = TRAIN_BACK;
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = static_cast<int16_t>(ht + TRAIN_BOTTOM_TRAPEZOID_HEIGHT);
                        int16_t y2 = static_cast<int16_t>(y1 - HEIGHT_DEC_1 / 2);
                        int16_t z2 = static_cast<int16_t>(z1 - TRAIN_BACK_TRAPEZOID_LENGTH / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC_1);
                        int16_t h2 = static_cast<int16_t>(h1 - HEIGHT_DEC_1);
                        int16_t y3 = static_cast<int16_t>(y2 - HEIGHT_DEC_2 / 2);
                        int16_t z3 = static_cast<int16_t>(z2 - TRAIN_BACK_TRAPEZOID_LENGTH / 2);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC_2);
                        int16_t h3 = static_cast<int16_t>(h2 - HEIGHT_DEC_2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(-3);
                        data.push_back(2);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }
                }

                // Roof.
                {
                    const int16_t WIDTH_DEC1 = 10;
                    const int16_t WIDTH_DEC2 = 30;

                    const int16_t FRONT_LENGTH = 380;
                    const int16_t BACK_LENGTH = 280;

                    const int16_t PIPE_RADIUS = 11;
                    const int16_t PIPE_INSET = 4;
                    const int16_t PIPE_OFFSET = 110;

                    int16_t midh = static_cast<int16_t>(TRAIN_FRONT - TRAIN_BACK - FRONT_LENGTH - BACK_LENGTH);
                    int16_t ymid = vgl::mix(static_cast<int16_t>(TRAIN_FRONT - FRONT_LENGTH), static_cast<int16_t>(TRAIN_BACK + BACK_LENGTH), 0.5f);
                    int16_t pipey = static_cast<int16_t>(TRAIN_ROOF_HEIGHT + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2 + PIPE_RADIUS - PIPE_INSET);

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(PIPE_OFFSET);
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - FRONT_LENGTH * 2 / 3));
                    data.push_back(PIPE_OFFSET);
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + BACK_LENGTH * 2 / 3));
                    data.push_back(2);
                    data.push_back(7);
                    data.push_back(PIPE_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(static_cast<int16_t>(-PIPE_OFFSET));
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - FRONT_LENGTH * 2 / 3));
                    data.push_back(static_cast<int16_t>(-PIPE_OFFSET));
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + BACK_LENGTH * 2 / 3));
                    data.push_back(2);
                    data.push_back(7);
                    data.push_back(PIPE_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(2);
                    data.push_back(0);
                    data.push_back(TRAIN_ROOF_HEIGHT);
                    data.push_back(ymid);
                    data.push_back(TRAIN_WIDTH);
                    data.push_back(midh);
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_ROOF_HEIGHT + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2));
                    data.push_back(ymid);
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH - WIDTH_DEC1));
                    data.push_back(midh);
                    data.push_back(2);
                    data.push_back(3);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

                    // Front.
                    {
                        const int16_t FRONT_DEC1 = 60;
                        const int16_t FRONT_DEC2 = 90;

                        int16_t y1 = TRAIN_ROOF_HEIGHT;
                        int16_t z1 = static_cast<int16_t>(TRAIN_FRONT - FRONT_LENGTH / 2);
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = FRONT_LENGTH;
                        int16_t y2 = static_cast<int16_t>(y1 + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2);
                        int16_t z2 = static_cast<int16_t>(z1 - FRONT_DEC1 / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC1);
                        int16_t h2 = static_cast<int16_t>(h1 - FRONT_DEC1);
                        int16_t y3 = static_cast<int16_t>(y2 + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2);
                        int16_t z3 = static_cast<int16_t>(z2 - FRONT_DEC2 / 3);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC2);
                        int16_t h3 = static_cast<int16_t>(h2 - FRONT_DEC2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }

                    // Back.
                    {
                        const int16_t BACK_DEC1 = 50;
                        const int16_t BACK_DEC2 = 80;

                        int16_t y1 = TRAIN_ROOF_HEIGHT;
                        int16_t z1 = static_cast<int16_t>(TRAIN_BACK + BACK_LENGTH / 2);
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = BACK_LENGTH;
                        int16_t y2 = static_cast<int16_t>(y1 + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2);
                        int16_t z2 = static_cast<int16_t>(z1 + BACK_DEC1 / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC1);
                        int16_t h2 = static_cast<int16_t>(h1 - BACK_DEC1);
                        int16_t y3 = static_cast<int16_t>(y2 + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2);
                        int16_t z3 = static_cast<int16_t>(z2 + BACK_DEC2 / 3);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC2);
                        int16_t h3 = static_cast<int16_t>(h2 - BACK_DEC2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }
                }

                // Floor.
                {
                    const int16_t BOTTOM_LENGTH = 160;
                    const int16_t BOTTOM_DEC = 40;
                    int16_t y1 = TRAIN_BOTTOM_HEIGHT;
                    int16_t y2 = static_cast<int16_t>(y1 - TRAIN_BOTTOM_TRAPEZOID_HEIGHT);
                    int16_t h2 = static_cast<int16_t>(BOTTOM_LENGTH - BOTTOM_DEC);

                    {
                        int16_t z1 = static_cast<int16_t>(TRAIN_FRONT - BOTTOM_LENGTH / 2);
                        int16_t z2 = static_cast<int16_t>(z1 + BOTTOM_DEC / 2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(2);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(BOTTOM_LENGTH);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(h2);
                        data.push_back(-2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_TOP);
                    }

                    {
                        int16_t z1 = static_cast<int16_t>(TRAIN_BACK + BOTTOM_LENGTH / 2);
                        int16_t z2 = static_cast<int16_t>(z1 - BOTTOM_DEC / 2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(2);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(BOTTOM_LENGTH);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(h2);
                        data.push_back(-2);
                        data.push_back(-3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_TOP);
                    }
                }

                // Undercarriage(s).
                {
                    int16_t UNDERCARRIAGE_WIDTH = 104;
                    int16_t UNDERCARRIAGE_HEIGHT = 40;
                    int16_t UNDERBLOCK_HEIGHT1 = 52;
                    int16_t UNDERBLOCK_HEIGHT2 = 64;
                    int16_t UNDERBLOCK_INSET = 12;
                    int16_t UNDERBLOCK_INSET2 = 42;

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERCARRIAGE_HEIGHT / 2));
                    data.push_back(TRAIN_FRONT);
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERCARRIAGE_HEIGHT / 2));
                    data.push_back(TRAIN_BACK);
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERCARRIAGE_HEIGHT);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 370));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 370));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 370));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 370));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 780));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 780));
                    data.push_back(2);
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH - UNDERBLOCK_INSET2 * 2));
                    data.push_back(UNDERBLOCK_HEIGHT2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT1 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 700));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT1 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 700));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT1 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 700));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT1 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 700));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);
                }

                // Stoppers.
                {
                    const int16_t STOPPER_LENGTH1 = 40;
                    const int16_t STOPPER_LENGTH2 = 60;
                    const int16_t STOPPER_WIDTH1 = 40;
                    const int16_t STOPPER_WIDTH2 = 60;
                    const int16_t STOPPER_OFFSETX = 90;
                    const int16_t STOPPER_OFFSETY = 120;

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(-STOPPER_OFFSETX));
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2));
                    data.push_back(static_cast<int16_t>(-STOPPER_OFFSETX));
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH1));
                    data.push_back(2);
                    data.push_back(STOPPER_WIDTH1);
                    data.push_back(STOPPER_WIDTH1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(-STOPPER_OFFSETX));
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH1));
                    data.push_back(static_cast<int16_t>(-STOPPER_OFFSETX));
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH2));
                    data.push_back(2);
                    data.push_back(STOPPER_WIDTH2);
                    data.push_back(STOPPER_WIDTH2);
                    data.push_back(vgl::CSG_FLAT);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(STOPPER_OFFSETX);
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2));
                    data.push_back(STOPPER_OFFSETX);
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH1));
                    data.push_back(2);
                    data.push_back(STOPPER_WIDTH1);
                    data.push_back(STOPPER_WIDTH1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(STOPPER_OFFSETX);
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH1));
                    data.push_back(STOPPER_OFFSETX);
                    data.push_back(STOPPER_OFFSETY);
                    data.push_back(static_cast<int16_t>(TRAIN_LENGTH / 2 + STOPPER_LENGTH2));
                    data.push_back(2);
                    data.push_back(STOPPER_WIDTH2);
                    data.push_back(STOPPER_WIDTH2);
                    data.push_back(vgl::CSG_FLAT);
                }

                csg_update("csg_train0.hpp", data);
#else
#include "csg_train0.hpp"
                auto data = CSG_READ_HPP(g_csg_train0_hpp);
#endif

                generate_wheels(data, static_cast<int16_t>(TRAIN_FRONT - 220));
                generate_wheels(data, static_cast<int16_t>(TRAIN_FRONT - 520));
                generate_wheels(data, static_cast<int16_t>(TRAIN_BACK + 220));
                generate_wheels(data, static_cast<int16_t>(TRAIN_BACK + 520));

                generate_door(data, 260, static_cast<int16_t>(TRAIN_FRONT - 200), 90, 200);
                generate_door(data, 260, static_cast<int16_t>(TRAIN_BACK + 200), 90, 200);
                generate_door(data, 300, 30, 1020, 40);
                generate_door(data, 170, -10, 1020, 40);

                // Johtimet.
                auto generate_johdin = [&data](int16_t zpos, bool dir)
                {
                    const int16_t JOHDIN_INSET = 6;
                    const int16_t JOHDIN_WIDTH1 = 40;
                    const int16_t JOHDIN_WIDTH2 = 60;
                    const int16_t JOHDIN_WIDTH3 = 220;
                    const int16_t JOHDIN_WIDTH4 = 280;
                    const int16_t JOHDIN_HEIGHT1 = 92;
                    const int16_t JOHDIN_HEIGHT2 = 164;
                    const int16_t JOHDIN_BACK = 90;
                    const int16_t JOHDIN_FRONT = 80;
                    const int16_t JOHDIN_RADIUS = 7;
                    const int16_t BLOCK_HEIGHT = 26;
                    const int16_t BLOCK_DEPTH = 50;
                    const int16_t BLOCK_WIDTH = 70;

                    int16_t ybase = static_cast<int16_t>(TRAIN_ROOF_HEIGHT + TRAIN_ROOF_TRAPEZOID1_HEIGHT / 2);

                    int16_t y1 = static_cast<int16_t>(ybase - JOHDIN_INSET);
                    int16_t y2 = static_cast<int16_t>(ybase + JOHDIN_HEIGHT1);
                    int16_t y3 = static_cast<int16_t>(ybase + JOHDIN_HEIGHT2);
                    int16_t x1 = static_cast<int16_t>(JOHDIN_WIDTH1 / 2);
                    int16_t x2 = static_cast<int16_t>(JOHDIN_WIDTH2 / 2);
                    int16_t x3 = static_cast<int16_t>(JOHDIN_WIDTH3 / 2);
                    int16_t x4 = static_cast<int16_t>(JOHDIN_WIDTH4 / 2);
                    int16_t z1 = static_cast<int16_t>(zpos + (dir ? JOHDIN_INSET : -JOHDIN_INSET) * 3);
                    int16_t z2 = static_cast<int16_t>(zpos + (dir ? -JOHDIN_BACK : JOHDIN_BACK));
                    int16_t z3 = static_cast<int16_t>(zpos + (dir ? JOHDIN_FRONT : -JOHDIN_FRONT));

                    data.push_back(to_int16(vgl::CsgCommand::PIPE));
                    data.push_back(3);
                    data.push_back(static_cast<int16_t>(-x1));
                    data.push_back(y1);
                    data.push_back(z1);
                    data.push_back(static_cast<int16_t>(-x2));
                    data.push_back(y2);
                    data.push_back(z2);
                    data.push_back(static_cast<int16_t>(-x3));
                    data.push_back(y3);
                    data.push_back(z3);
                    data.push_back(7);
                    data.push_back(JOHDIN_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::PIPE));
                    data.push_back(3);
                    data.push_back(x1);
                    data.push_back(y1);
                    data.push_back(z1);
                    data.push_back(x2);
                    data.push_back(y2);
                    data.push_back(z2);
                    data.push_back(x3);
                    data.push_back(y3);
                    data.push_back(z3);
                    data.push_back(7);
                    data.push_back(JOHDIN_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(x4);
                    data.push_back(y3);
                    data.push_back(z3);
                    data.push_back(static_cast<int16_t>(-x4));
                    data.push_back(y3);
                    data.push_back(z3);
                    data.push_back(2);
                    data.push_back(7);
                    data.push_back(static_cast<int16_t>(JOHDIN_RADIUS + 1));
                    data.push_back(0);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(ybase);
                    data.push_back(zpos);
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(ybase + BLOCK_HEIGHT));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(BLOCK_WIDTH);
                    data.push_back(BLOCK_DEPTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                };

                generate_johdin(360, false);
                generate_johdin(-470, true);

                generate_yhdistys(data, static_cast<int16_t>(TRAIN_BACK - TRAIN_BACK_TRAPEZOID_LENGTH), true);

                // Veturi end.
                data.push_back(to_int16(vgl::CsgCommand::NONE));
                {
                    vgl::LogicalMesh lmesh(data.data());
                    m_mesh_train[0] = lmesh.compile();
                }
#if defined(USE_LD)
                addPreviewMesh("train0", *m_mesh_train[0]);
#endif
            }

            // Carriage is based on Fiat SM3 (Pendolino) carriage.
            // https://fi.wikipedia.org/wiki/Sm3
            {
                const int16_t TRAIN_TRAPEZOID_LENGTH = 60;
                const int16_t TRAIN_FRONT = static_cast<int16_t>(TRAIN_LENGTH / 2 - TRAIN_TRAPEZOID_LENGTH);
                const int16_t TRAIN_BACK = static_cast<int16_t>(-TRAIN_LENGTH / 2 + TRAIN_TRAPEZOID_LENGTH);

#if defined(USE_LD)
                const int16_t TRAIN_BOTTOM_TRAPEZOID_HEIGHT = 70;
                const int16_t TRAIN_ROOF_TRAPEZOID_HEIGHT = 80;
                const int16_t TRAIN_ROOF_HEIGHT = static_cast<int16_t>(TRAIN_HEIGHT - TRAIN_ROOF_TRAPEZOID_HEIGHT);

                vgl::vector<int16_t> data;

                {
                    int16_t WIDTH_DEC_1 = 40;
                    int16_t WIDTH_DEC_2 = 70;

                    const int16_t ypos = vgl::mix(TRAIN_ROOF_HEIGHT, TRAIN_BOTTOM_HEIGHT, 0.5f);
                    const int16_t ht = static_cast<int16_t>(TRAIN_ROOF_HEIGHT - TRAIN_BOTTOM_HEIGHT);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(ypos);
                    data.push_back(TRAIN_FRONT + 1);
                    data.push_back(0);
                    data.push_back(ypos);
                    data.push_back(TRAIN_BACK - 1);
                    data.push_back(2);
                    data.push_back(TRAIN_WIDTH);
                    data.push_back(ht + 1);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP);

                    // Front and back.
                    {
                        int16_t HEIGHT_DEC_1 = 30;
                        int16_t HEIGHT_DEC_2 = 40;

                        int16_t y1 = static_cast<int16_t>(ypos - TRAIN_BOTTOM_TRAPEZOID_HEIGHT / 2);
                        int16_t z1 = TRAIN_FRONT;
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = static_cast<int16_t>(ht + TRAIN_BOTTOM_TRAPEZOID_HEIGHT);
                        int16_t y2 = static_cast<int16_t>(y1 - HEIGHT_DEC_1 / 2);
                        int16_t z2 = static_cast<int16_t>(z1 + TRAIN_TRAPEZOID_LENGTH / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC_1);
                        int16_t h2 = static_cast<int16_t>(h1 - HEIGHT_DEC_1);
                        int16_t y3 = static_cast<int16_t>(y2 - HEIGHT_DEC_2 * 2 / 5);
                        int16_t z3 = static_cast<int16_t>(z2 + TRAIN_TRAPEZOID_LENGTH / 2);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC_2);
                        int16_t h3 = static_cast<int16_t>(h2 - HEIGHT_DEC_2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(3);
                        data.push_back(2);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                        z1 = TRAIN_BACK;
                        z2 = static_cast<int16_t>(z1 - TRAIN_TRAPEZOID_LENGTH / 2);
                        z3 = static_cast<int16_t>(z2 - TRAIN_TRAPEZOID_LENGTH / 2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(-3);
                        data.push_back(2);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }
                }

                // Roof.
                {
                    const int16_t WIDTH_DEC1 = 30;
                    const int16_t WIDTH_DEC2 = 50;
                    const int16_t ELEVATION_LENGTH = 480;
                    const int16_t PIPE_RADIUS = 13;
                    const int16_t PIPE_INSET = 4;
                    const int16_t PIPE_OFFSET = 94;
                    const int16_t ILMASTOINTI_OFFSET = 240;
                    const int16_t ILMASTOINTI_HEIGHT = 32;
                    const int16_t ILMASTOINTI_LENGTH = 120;
                    const int16_t ILMASTOINTI_WIDTH = 94;

                    int16_t midh = static_cast<int16_t>(TRAIN_FRONT - TRAIN_BACK - ELEVATION_LENGTH * 2);
                    int16_t ymid = vgl::mix(static_cast<int16_t>(TRAIN_FRONT - ELEVATION_LENGTH), static_cast<int16_t>(TRAIN_BACK + ELEVATION_LENGTH), 0.5f);
                    int16_t pipey = static_cast<int16_t>(TRAIN_ROOF_HEIGHT + TRAIN_ROOF_TRAPEZOID_HEIGHT / 2 + PIPE_RADIUS - PIPE_INSET);
                    int16_t endh = static_cast<int16_t>(TRAIN_ROOF_HEIGHT + TRAIN_ROOF_TRAPEZOID_HEIGHT / 2);

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(PIPE_OFFSET);
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - ELEVATION_LENGTH * 2 / 3));
                    data.push_back(PIPE_OFFSET);
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + ELEVATION_LENGTH * 2 / 3));
                    data.push_back(2);
                    data.push_back(7);
                    data.push_back(PIPE_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(static_cast<int16_t>(-PIPE_OFFSET));
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - ELEVATION_LENGTH * 2 / 3));
                    data.push_back(static_cast<int16_t>(-PIPE_OFFSET));
                    data.push_back(pipey);
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + ELEVATION_LENGTH * 2 / 3));
                    data.push_back(2);
                    data.push_back(7);
                    data.push_back(PIPE_RADIUS);
                    data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                    data.push_back(2);
                    data.push_back(0);
                    data.push_back(TRAIN_ROOF_HEIGHT);
                    data.push_back(ymid);
                    data.push_back(TRAIN_WIDTH);
                    data.push_back(midh);
                    data.push_back(0);
                    data.push_back(endh);
                    data.push_back(ymid);
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH - WIDTH_DEC1));
                    data.push_back(midh);
                    data.push_back(2);
                    data.push_back(3);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(endh);
                    data.push_back(static_cast<int16_t>(ILMASTOINTI_OFFSET));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(endh + ILMASTOINTI_HEIGHT));
                    data.push_back(static_cast<int16_t>(ILMASTOINTI_OFFSET));
                    data.push_back(3);
                    data.push_back(ILMASTOINTI_WIDTH);
                    data.push_back(ILMASTOINTI_LENGTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(endh);
                    data.push_back(static_cast<int16_t>(-ILMASTOINTI_OFFSET));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(endh + ILMASTOINTI_HEIGHT));
                    data.push_back(static_cast<int16_t>(-ILMASTOINTI_OFFSET));
                    data.push_back(3);
                    data.push_back(ILMASTOINTI_WIDTH);
                    data.push_back(ILMASTOINTI_LENGTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                    // Front and back.
                    {
                        const int16_t FRONT_DEC1 = 50;
                        const int16_t FRONT_DEC2 = 70;

                        int16_t y1 = TRAIN_ROOF_HEIGHT;
                        int16_t z1 = static_cast<int16_t>(TRAIN_FRONT - ELEVATION_LENGTH / 2);
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t h1 = ELEVATION_LENGTH;
                        int16_t y2 = static_cast<int16_t>(y1 + TRAIN_ROOF_TRAPEZOID_HEIGHT / 2);
                        int16_t z2 = static_cast<int16_t>(z1 - FRONT_DEC1 / 2);
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC1);
                        int16_t h2 = static_cast<int16_t>(h1 - FRONT_DEC1);
                        int16_t y3 = static_cast<int16_t>(y2 + TRAIN_ROOF_TRAPEZOID_HEIGHT / 2);
                        int16_t z3 = static_cast<int16_t>(z2 - FRONT_DEC2 / 3);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC2);
                        int16_t h3 = static_cast<int16_t>(h2 - FRONT_DEC2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                        z1 = static_cast<int16_t>(TRAIN_BACK + ELEVATION_LENGTH / 2);
                        z2 = static_cast<int16_t>(z1 + FRONT_DEC1 / 2);
                        z3 = static_cast<int16_t>(z2 + FRONT_DEC1 / 3);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(z2);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z3);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }
                }

                // Floor.
                {
                    const int16_t BOTTOM_LENGTH = 130;
                    const int16_t MIDDLE_LENGTH = 620;
                    const int16_t BOTTOM_DEC = 50;
                    const int16_t WIDTH_DEC1 = 30;
                    const int16_t WIDTH_DEC2 = 40;
                    int16_t y1 = TRAIN_BOTTOM_HEIGHT;
                    int16_t y2 = static_cast<int16_t>(y1 - TRAIN_BOTTOM_TRAPEZOID_HEIGHT / 2);
                    int16_t y3 = static_cast<int16_t>(y2 - TRAIN_BOTTOM_TRAPEZOID_HEIGHT / 2);

                    {
                        int16_t z1 = static_cast<int16_t>(TRAIN_FRONT - BOTTOM_LENGTH / 2);
                        int16_t z2 = static_cast<int16_t>(z1 + BOTTOM_DEC / 2);
                        int16_t h2 = static_cast<int16_t>(BOTTOM_LENGTH - BOTTOM_DEC);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(2);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(BOTTOM_LENGTH);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z2);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(h2);
                        data.push_back(-2);
                        data.push_back(3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_TOP);

                        z1 = static_cast<int16_t>(TRAIN_BACK + BOTTOM_LENGTH / 2);
                        z2 = static_cast<int16_t>(z1 - BOTTOM_DEC / 2);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(2);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(z1);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(BOTTOM_LENGTH);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(z2);
                        data.push_back(TRAIN_WIDTH);
                        data.push_back(h2);
                        data.push_back(-2);
                        data.push_back(-3);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_TOP);
                    }

                    {
                        int16_t h1 = MIDDLE_LENGTH;
                        int16_t h2 = static_cast<int16_t>(h1 - BOTTOM_DEC);;
                        int16_t h3 = static_cast<int16_t>(h2 - BOTTOM_DEC);;
                        int16_t w1 = TRAIN_WIDTH;
                        int16_t w2 = static_cast<int16_t>(w1 - WIDTH_DEC1);
                        int16_t w3 = static_cast<int16_t>(w2 - WIDTH_DEC2);

                        int16_t zmid = vgl::mix(static_cast<int16_t>(TRAIN_FRONT - BOTTOM_LENGTH), static_cast<int16_t>(TRAIN_BACK + BOTTOM_LENGTH), 0.5f);

                        data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                        data.push_back(3);
                        data.push_back(0);
                        data.push_back(y1);
                        data.push_back(zmid);
                        data.push_back(w1);
                        data.push_back(h1);
                        data.push_back(0);
                        data.push_back(y2);
                        data.push_back(zmid);
                        data.push_back(w2);
                        data.push_back(h2);
                        data.push_back(0);
                        data.push_back(y3);
                        data.push_back(zmid);
                        data.push_back(w3);
                        data.push_back(h3);
                        data.push_back(-2);
                        data.push_back(-3);
                        data.push_back(vgl::CSG_FLAT);
                    }
                }

                // Undercarriage(s).
                {
                    int16_t UNDERCARRIAGE_WIDTH = 104;
                    int16_t UNDERCARRIAGE_HEIGHT = 40;
                    int16_t UNDERBLOCK_HEIGHT2 = 64;
                    int16_t UNDERBLOCK_INSET2 = 42;

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERCARRIAGE_HEIGHT / 2));
                    data.push_back(TRAIN_FRONT);
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERCARRIAGE_HEIGHT / 2));
                    data.push_back(TRAIN_BACK);
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERCARRIAGE_HEIGHT);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 350));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_FRONT - 350));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(TRAIN_WIDTH / 2 - UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 350));
                    data.push_back(static_cast<int16_t>(-TRAIN_WIDTH / 2 + UNDERBLOCK_INSET2));
                    data.push_back(static_cast<int16_t>(TRAIN_BOTTOM_HEIGHT - UNDERBLOCK_HEIGHT2 / 2));
                    data.push_back(static_cast<int16_t>(TRAIN_BACK + 350));
                    data.push_back(2);
                    data.push_back(UNDERCARRIAGE_WIDTH);
                    data.push_back(UNDERBLOCK_HEIGHT2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_TOP);
                }

                csg_update("csg_train1.hpp", data);
#else
#include "csg_train1.hpp"
                auto data = CSG_READ_HPP(g_csg_train1_hpp);
#endif

                generate_wheels(data, static_cast<int16_t>(TRAIN_FRONT - 200));
                generate_wheels(data, static_cast<int16_t>(TRAIN_FRONT - 500));
                generate_wheels(data, static_cast<int16_t>(TRAIN_BACK + 200));
                generate_wheels(data, static_cast<int16_t>(TRAIN_BACK + 500));

                generate_door(data, 244, static_cast<int16_t>(TRAIN_FRONT - 164), 90, 190);
                generate_door(data, 244, static_cast<int16_t>(TRAIN_BACK + 164), 90, 190);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_FRONT - 370), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_BACK + 370), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_FRONT - 520), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_BACK + 520), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_FRONT - 670), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_BACK + 670), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_FRONT - 820), 110, 70);
                generate_door(data, 270, static_cast<int16_t>(TRAIN_BACK + 820), 110, 70);

                generate_yhdistys(data, static_cast<int16_t>(TRAIN_FRONT + TRAIN_TRAPEZOID_LENGTH), false);
                generate_yhdistys(data, static_cast<int16_t>(-TRAIN_FRONT - TRAIN_TRAPEZOID_LENGTH), true);

                // Vaunu end.
                data.push_back(to_int16(vgl::CsgCommand::NONE));
                {
                    vgl::LogicalMesh lmesh(data.data());
                    m_mesh_train[1] = lmesh.compile();
                }
#if defined(USE_LD)
                addPreviewMesh("train1", *m_mesh_train[1]);
#endif
            }
        }

        // Kerava station katos.
        {
            static const int16_t KATOS_LENGTH = 2400;
            static const int16_t KATOS_SIDE_WIDTH = 180;
            static const int16_t KATOS_HEIGHT = 360;
            static const int16_t KATOS_THICKNESS = 24;
            static const int16_t KATOS_TOP_THICKNESS = 60;
            static const int16_t KATOS_PILLAR_INSET = 100;
            static const int16_t KATOS_PILLAR_BASE_WIDTH = 60;
            static const int16_t KATOS_PILLAR_BASE_HEIGHT = 30;
            static const int16_t KATOS_PILLAR_WIDTH = 36;
            static const int16_t KATOS_FRAME_WIDTH = 40;

            vgl::vector<int16_t> data;

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT + KATOS_TOP_THICKNESS / 2));
            data.push_back(static_cast<int16_t>(-KATOS_LENGTH / 2));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT + KATOS_TOP_THICKNESS / 2));
            data.push_back(static_cast<int16_t>(KATOS_LENGTH / 2));
            data.push_back(2);
            data.push_back(KATOS_TOP_THICKNESS);
            data.push_back(KATOS_TOP_THICKNESS);
            data.push_back(vgl::CSG_FLAT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(KATOS_SIDE_WIDTH * 7 / 8));
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 3 / 4));
            data.push_back(static_cast<int16_t>(-KATOS_LENGTH / 2));
            data.push_back(static_cast<int16_t>(KATOS_SIDE_WIDTH * 7 / 8));
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 3 / 4));
            data.push_back(static_cast<int16_t>(KATOS_LENGTH / 2));
            data.push_back(0);
            data.push_back(3);
            data.push_back(8);
            data.push_back(0);
            data.push_back(KATOS_SIDE_WIDTH * 11 / 7);
            data.push_back(KATOS_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_RIGHT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(-KATOS_SIDE_WIDTH * 7 / 8));
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 3 / 4));
            data.push_back(static_cast<int16_t>(-KATOS_LENGTH / 2));
            data.push_back(static_cast<int16_t>(-KATOS_SIDE_WIDTH * 7 / 8));
            data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 3 / 4));
            data.push_back(static_cast<int16_t>(KATOS_LENGTH / 2));
            data.push_back(0);
            data.push_back(-3);
            data.push_back(8);
            data.push_back(0);
            data.push_back(KATOS_SIDE_WIDTH * 11 / 7);
            data.push_back(KATOS_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_LEFT);

            {
                int16_t zstart = static_cast<int16_t>(-KATOS_LENGTH / 2 + KATOS_PILLAR_INSET / 2);
                int16_t zend = static_cast<int16_t>(KATOS_LENGTH / 2 - KATOS_PILLAR_INSET / 2);
                for(int16_t zpos = zstart; (zpos <= zend); zpos = static_cast<int16_t>(zpos + (zend - zstart) / 8))
                {
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(KATOS_HEIGHT + KATOS_TOP_THICKNESS * 6 / 9));
                    data.push_back(zpos);
                    data.push_back(static_cast<int16_t>(KATOS_SIDE_WIDTH * 11 / 7));
                    data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 11 / 4));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(KATOS_FRAME_WIDTH);
                    data.push_back(KATOS_THICKNESS);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(KATOS_HEIGHT + KATOS_TOP_THICKNESS * 6 / 9));
                    data.push_back(zpos);
                    data.push_back(static_cast<int16_t>(-KATOS_SIDE_WIDTH * 11 / 7));
                    data.push_back(static_cast<int16_t>(KATOS_HEIGHT - KATOS_THICKNESS * 11 / 4));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(KATOS_FRAME_WIDTH);
                    data.push_back(KATOS_THICKNESS);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                }
            }

            {
                int16_t zstart = static_cast<int16_t>(-(KATOS_LENGTH / 2) + KATOS_PILLAR_INSET);
                int16_t zend = static_cast<int16_t>((KATOS_LENGTH / 2) - KATOS_PILLAR_INSET);
                for(int16_t zpos = zstart; (zpos <= zend); zpos = static_cast<int16_t>(zpos + (zend - zstart) / 2))
                {
                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(static_cast<int16_t>(-KATOS_PILLAR_BASE_HEIGHT));
                    data.push_back(zpos);
                    data.push_back(0);
                    data.push_back(KATOS_PILLAR_BASE_HEIGHT);
                    data.push_back(zpos);
                    data.push_back(1);
                    data.push_back(KATOS_PILLAR_BASE_WIDTH);
                    data.push_back(KATOS_PILLAR_BASE_WIDTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK);

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(0);
                    data.push_back(KATOS_PILLAR_BASE_HEIGHT);
                    data.push_back(zpos);
                    data.push_back(0);
                    data.push_back(KATOS_HEIGHT);
                    data.push_back(zpos);
                    data.push_back(1);
                    data.push_back(KATOS_PILLAR_WIDTH);
                    data.push_back(KATOS_PILLAR_WIDTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK | vgl::CSG_NO_FRONT);
                }
            }

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_katos = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("katos", *m_mesh_katos);
#endif
        }

        // Tower generation function.
        auto generate_tower = [](vgl::vector<int16_t>& data, int16_t width, int16_t segment_height, int16_t segment_count,
                int16_t segment_frame_width, int16_t segment_frame_height, int16_t frame_inset)
        {
            VGL_ASSERT(segment_frame_width >= width);

            for(int16_t ii = 0; (ii < segment_count); ii = static_cast<int16_t>(ii + 1))
            {
                int16_t y1 = static_cast<int16_t>(ii * segment_height);
                int16_t y2 = static_cast<int16_t>(y1 + segment_height - segment_frame_height);
                int16_t y3 = static_cast<int16_t>(y1 + segment_height);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(y1);
                data.push_back(0);
                data.push_back(0);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(3);
                data.push_back(width);
                data.push_back(width);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(y2);
                data.push_back(0);
                data.push_back(0);
                data.push_back(y3);
                data.push_back(0);
                data.push_back(3);
                data.push_back(segment_frame_width);
                data.push_back(segment_frame_width);
                data.push_back(vgl::CSG_FLAT);
            }

            if(frame_inset > 0)
            {
                int16_t frame_width = static_cast<int16_t>(segment_frame_width - width + frame_inset);
                int16_t offsetp = static_cast<int16_t>((segment_frame_width / 2) - (frame_width / 2));
                int16_t offsetn = static_cast<int16_t>(-offsetp);
                ++frame_width;

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(offsetp);
                data.push_back(0);
                data.push_back(offsetp);
                data.push_back(offsetp);
                data.push_back(static_cast<int16_t>(segment_height * segment_count + frame_width));
                data.push_back(offsetp);
                data.push_back(3);
                data.push_back(frame_width);
                data.push_back(frame_width);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(offsetn);
                data.push_back(0);
                data.push_back(offsetp);
                data.push_back(offsetn);
                data.push_back(static_cast<int16_t>(segment_height * segment_count + frame_width));
                data.push_back(offsetp);
                data.push_back(3);
                data.push_back(frame_width);
                data.push_back(frame_width);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(offsetp);
                data.push_back(0);
                data.push_back(offsetn);
                data.push_back(offsetp);
                data.push_back(static_cast<int16_t>(segment_height * segment_count + frame_width));
                data.push_back(offsetn);
                data.push_back(3);
                data.push_back(frame_width);
                data.push_back(frame_width);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(offsetn);
                data.push_back(0);
                data.push_back(offsetn);
                data.push_back(offsetn);
                data.push_back(static_cast<int16_t>(segment_height * segment_count + frame_width));
                data.push_back(offsetn);
                data.push_back(3);
                data.push_back(frame_width);
                data.push_back(frame_width);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
            }
        };

        // Kerava station tower.
        {
            static const int16_t TOWER_WIDTH = 340;
            static const int16_t TOWER_SEGMENT_HEIGHT = 280;
            static const int16_t TOWER_SEGMENT_FRAME_HEIGHT = 40;
            static const int16_t TOWER_SEGMENT_FRAME_WIDTH = 400;
            static const int16_t TOWER_FRAME_INSET = 10;

            vgl::vector<int16_t> data;

            generate_tower(data, TOWER_WIDTH, TOWER_SEGMENT_HEIGHT, 5, TOWER_SEGMENT_FRAME_WIDTH, TOWER_SEGMENT_FRAME_HEIGHT,
                    TOWER_FRAME_INSET);

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_tower = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("tower", *m_mesh_tower);
#endif
        }

        // Kerava station ramp.
        {
            const int16_t RAMP_HEIGHT = 1056;
            const int16_t RAMP_WIDTH = 260;
            const int16_t RAMP_LENGTH = 2800;
            const int16_t RAMP_STOP_LENGTH = 600;
            const int16_t RAMP_SIDE_THICKNESS = 68;

            const int16_t RAMP_Z1 = 0;
            const int16_t RAMP_Z2 = static_cast<int16_t>(RAMP_LENGTH / 2 - RAMP_STOP_LENGTH / 2);
            const int16_t RAMP_Z3 = static_cast<int16_t>(RAMP_LENGTH / 2 + RAMP_STOP_LENGTH / 2);
            const int16_t RAMP_Z4 = static_cast<int16_t>(RAMP_LENGTH);
            const int16_t RAMP_Y1 = RAMP_HEIGHT;
            const int16_t RAMP_Y2 = static_cast<int16_t>(RAMP_HEIGHT / 2);
            const int16_t RAMP_Y3 = 0;
            const int16_t RAMP_XL = static_cast<int16_t>(-RAMP_WIDTH + RAMP_SIDE_THICKNESS / 2);
            const int16_t RAMP_XR = static_cast<int16_t>(RAMP_WIDTH - RAMP_SIDE_THICKNESS / 2);

#if defined(USE_LD)
            const int16_t RAMP_STOP_EXTENT = 17;
            const int16_t RAMP_STOP_MID_EXTENT = 10;
            const int16_t RAMP_SIDE_THICKNESS_EXTENT = 8;
            const int16_t RAMP_MID_THICKNESS = 36;
            const int16_t RAMP_MID_THICKNESS_EXTENT = 5;
            const int16_t RAMP_MIDW = static_cast<int16_t>(RAMP_WIDTH * 2 - RAMP_SIDE_THICKNESS * 2);

            vgl::vector<int16_t> data;

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y1);
            data.push_back(RAMP_Z1);
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z2);
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(static_cast<int16_t>(RAMP_SIDE_THICKNESS + RAMP_SIDE_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y1);
            data.push_back(RAMP_Z1);
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z2);
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(static_cast<int16_t>(RAMP_SIDE_THICKNESS + RAMP_SIDE_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(RAMP_Y1);
            data.push_back(RAMP_Z1);
            data.push_back(0);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z2);
            data.push_back(2);
            data.push_back(RAMP_MIDW);
            data.push_back(static_cast<int16_t>(RAMP_MID_THICKNESS + RAMP_MID_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z2 - RAMP_STOP_EXTENT));
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z3 + RAMP_STOP_EXTENT));
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z2 - RAMP_STOP_EXTENT));
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z3 + RAMP_STOP_EXTENT));
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z2 - RAMP_STOP_MID_EXTENT));
            data.push_back(0);
            data.push_back(RAMP_Y2);
            data.push_back(static_cast<int16_t>(RAMP_Z3 + RAMP_STOP_MID_EXTENT));
            data.push_back(2);
            data.push_back(RAMP_MIDW);
            data.push_back(RAMP_MID_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z3);
            data.push_back(RAMP_XL);
            data.push_back(RAMP_Y3);
            data.push_back(RAMP_Z4);
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(static_cast<int16_t>(RAMP_SIDE_THICKNESS + RAMP_SIDE_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z3);
            data.push_back(RAMP_XR);
            data.push_back(RAMP_Y3);
            data.push_back(RAMP_Z4);
            data.push_back(2);
            data.push_back(RAMP_SIDE_THICKNESS);
            data.push_back(static_cast<int16_t>(RAMP_SIDE_THICKNESS + RAMP_SIDE_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(RAMP_Y2);
            data.push_back(RAMP_Z3);
            data.push_back(0);
            data.push_back(RAMP_Y3);
            data.push_back(RAMP_Z4);
            data.push_back(2);
            data.push_back(RAMP_MIDW);
            data.push_back(static_cast<int16_t>(RAMP_MID_THICKNESS + RAMP_MID_THICKNESS_EXTENT));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            csg_update("csg_ramp.hpp", data);
#else
#include "csg_ramp.hpp"
            auto data = CSG_READ_HPP(g_csg_ramp_hpp);
#endif

            auto generate_kaide = [RAMP_Z1, RAMP_Z2, RAMP_Z3, RAMP_Z4, &data]
                (int16_t xpos)
            {
                static const int16_t RAMP_KAIDE_HEIGHT = 156;
                static const int16_t RAMP_KAIDE_START = 20;
                static const int16_t RAMP_KAIDE_THICKNESS = 10;
                static const int16_t RAMP_KAIDE_THICKNESS2 = 8;

                data.push_back(to_int16(vgl::CsgCommand::PIPE));
                data.push_back(4);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAMP_Y1 + RAMP_KAIDE_HEIGHT));
                data.push_back(RAMP_Z1);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAMP_Y2 + RAMP_KAIDE_HEIGHT));
                data.push_back(RAMP_Z2);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAMP_Y2 + RAMP_KAIDE_HEIGHT));
                data.push_back(RAMP_Z3);
                data.push_back(xpos);
                data.push_back(static_cast<int16_t>(RAMP_Y3 + RAMP_KAIDE_HEIGHT));
                data.push_back(RAMP_Z4);
                data.push_back(5);
                data.push_back(RAMP_KAIDE_THICKNESS);
                data.push_back(0);

                for(int ii = 0; ii <= 9; ++ii)
                {
                    float ratio = static_cast<float>(ii) / 9.0f * 0.96f + 0.02f;
                    int16_t ypos = static_cast<int16_t>(vgl::mix(RAMP_Y1, RAMP_Y2, ratio));
                    int16_t zpos = static_cast<int16_t>(vgl::mix(RAMP_Z1, RAMP_Z2, ratio));

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(ypos + RAMP_KAIDE_START));
                    data.push_back(zpos);
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(ypos + RAMP_KAIDE_HEIGHT));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(3);
                    data.push_back(RAMP_KAIDE_THICKNESS2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                    ypos = static_cast<int16_t>(vgl::mix(RAMP_Y2, RAMP_Y3, ratio));
                    zpos = static_cast<int16_t>(vgl::mix(RAMP_Z3, RAMP_Z4, ratio));

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(ypos + RAMP_KAIDE_START));
                    data.push_back(zpos);
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(ypos + RAMP_KAIDE_HEIGHT));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(3);
                    data.push_back(RAMP_KAIDE_THICKNESS2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
                }

                for(int ii = 1; ii < 5; ++ii)
                {
                    float ratio = static_cast<float>(ii) / 5.0f;
                    int16_t zpos = static_cast<int16_t>(vgl::mix(RAMP_Z2, RAMP_Z3, ratio));

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(RAMP_Y2 + RAMP_KAIDE_START));
                    data.push_back(zpos);
                    data.push_back(xpos);
                    data.push_back(static_cast<int16_t>(RAMP_Y2 + RAMP_KAIDE_HEIGHT));
                    data.push_back(zpos);
                    data.push_back(3);
                    data.push_back(3);
                    data.push_back(RAMP_KAIDE_THICKNESS2);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
                }
            };

            generate_kaide(RAMP_XL);
            generate_kaide(RAMP_XR);

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_ramp = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("ramp", *m_mesh_ramp);
#endif
        }

        // Window generation command with w/h/d as opposed to corner 2 coordinates.
        auto generate_window_whd = [generate_window](vgl::vector<int16_t>& data, int16_t px, int16_t py, int16_t pz,
                int16_t width, int16_t height, int16_t depth, int16_t radius)
        {
            int16_t x1 = static_cast<int16_t>(px - width / 2);
            int16_t x2 = static_cast<int16_t>(x1 + width);
            int16_t y1 = static_cast<int16_t>(py - height / 2);
            int16_t y2 = static_cast<int16_t>(y1 + height);
            int16_t z1 = static_cast<int16_t>(pz - depth / 2);
            int16_t z2 = static_cast<int16_t>(z1 + depth);

            generate_window(data, x1, y1, z1, x2, y2, z2, radius);
        };

        // Kerava station building.
        // Based on reference images (see ref/).
        // Some shapes have been modified to decrease complexity.
        {
            const int16_t STATION_LENGTH = 5000;
            const int16_t STATION_MID_OFFSET = 1500;
            const int16_t STATION_WIDTH = 1000;
            const int16_t STATION_HEIGHT2 = 640;
            const int16_t STATION_HEIGHT3 = 820;
            const int16_t STATION_ROOF_THICKNESS = 20;
            const int16_t STATION_WINDOW_W = 132;
            const int16_t STATION_WINDOW_Y1 = 260;
            const int16_t STATION_WINDOW_H1 = 330;
            const int16_t STATION_WINDOW_R = 16;

            const int16_t PAATY_OUTSET = 130;

            const int16_t MAIN_DOOR_LENGTH = 1220;

#if defined(USE_LD)
            const int16_t STATION_HEIGHT1 = 580;
            const int16_t STATION_HEIGHT4 = 860;

            const int16_t PAATY_WIDTH1 = 320;
            const int16_t PAATY_WIDTH2 = 560;
            const int16_t PAATY_HEIGHT1 = static_cast<int16_t>(STATION_HEIGHT4 - 40);
            const int16_t PAATY_HEIGHT2 = static_cast<int16_t>(STATION_HEIGHT4 + 40);
            const int16_t PAATY_HEIGHT3 = static_cast<int16_t>(PAATY_HEIGHT2 + 380);
            const int16_t PAATY_HEIGHT4 = static_cast<int16_t>(PAATY_HEIGHT3 + 20);
            const int16_t PAATY_HEIGHT5 = static_cast<int16_t>(PAATY_HEIGHT4 + 70);
            const int16_t PAATY_HEIGHT6 = static_cast<int16_t>(PAATY_HEIGHT5 + 160);
            const int16_t PAATY_HEIGHT7 = static_cast<int16_t>(PAATY_HEIGHT6 + 360);
            const int16_t PAATY_RADIUS1 = 240;
            const int16_t PAATY_RADIUS2 = 80;
            const int16_t PAATY_RADIUS3 = 10;
            const int16_t PAATY_ZPOS = static_cast<int16_t>(STATION_LENGTH / 2 + PAATY_OUTSET - PAATY_WIDTH2 / 2);

            const int16_t MAIN_DOOR_WIDTH = 580;
            const int16_t MAIN_DOOR_HEIGHT1 = static_cast<int16_t>(STATION_HEIGHT1 - 80);
            const int16_t MAIN_DOOR_HEIGHT2 = static_cast<int16_t>(MAIN_DOOR_HEIGHT1 + 200);

            const int16_t ILMANVAIHTO_Y1 = 770;
            const int16_t ILMANVAIHTO_Y2 = 1000;
            const int16_t ILMANVAIHTO_Y3 = 1100;
            const int16_t ILMANVAIHTO_OFFSETX = 90;
            const int16_t ILMANVAIHTO_WIDTH = 100;
            const int16_t ILMANVAIHTO_WIDTH2 = 20;
            const int16_t ILMANVAIHTO_LENGTH = 200;
            const int16_t ILMANVAIHTO_OFFSET1 = 1300;
            const int16_t ILMANVAIHTO_OFFSET2 = -1300;

            const int16_t ANTENNA_OFFSETX = -80;
            const int16_t ANTENNA_OFFSET = -1100;
            const int16_t ANTENNA_HEIGHT1 = 1250;
            const int16_t ANTENNA_HEIGHT2 = 1450;
            const int16_t ANTENNA_RADIUS = 12;
            const int16_t ANTENNA_INSET1 = 170;
            const int16_t ANTENNA_INSET2 = 70;

            vgl::vector<int16_t> data;

            // Middle shape.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(STATION_HEIGHT1 / 2));
            data.push_back(static_cast<int16_t>(STATION_LENGTH / 2));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(STATION_HEIGHT1 / 2));
            data.push_back(static_cast<int16_t>(-STATION_LENGTH / 2));
            data.push_back(2);
            data.push_back(STATION_WIDTH);
            data.push_back(STATION_HEIGHT1);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(2);
            data.push_back(0);
            data.push_back(STATION_HEIGHT1);
            data.push_back(0);
            data.push_back(STATION_LENGTH);
            data.push_back(STATION_WIDTH);
            data.push_back(0);
            data.push_back(STATION_HEIGHT4);
            data.push_back(0);
            data.push_back(STATION_LENGTH);
            data.push_back(1);
            data.push_back(2);
            data.push_back(1);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

            // Sloped roofs.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(STATION_HEIGHT4);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-STATION_WIDTH * 7 / 12));
            data.push_back(static_cast<int16_t>(STATION_HEIGHT1 * 9 / 10));
            data.push_back(0);
            data.push_back(1);
            data.push_back(static_cast<int16_t>(STATION_LENGTH * 28 / 27));
            data.push_back(STATION_ROOF_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(STATION_HEIGHT4);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(STATION_WIDTH * 7 / 12));
            data.push_back(static_cast<int16_t>(STATION_HEIGHT1 * 9 / 10));
            data.push_back(0);
            data.push_back(1);
            data.push_back(static_cast<int16_t>(STATION_LENGTH * 28 / 27));
            data.push_back(STATION_ROOF_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(STATION_HEIGHT4);
            data.push_back(static_cast<int16_t>(STATION_LENGTH * 28 / 27 / 2));
            data.push_back(0);
            data.push_back(STATION_HEIGHT4);
            data.push_back(static_cast<int16_t>(-STATION_LENGTH * 28 / 27 / 2));
            data.push_back(2);
            data.push_back(7);
            data.push_back(static_cast<int16_t>(STATION_ROOF_THICKNESS * 3 / 5));
            data.push_back(vgl::CSG_FLAT);

            // Pääty ja torni.
            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(3);
            data.push_back(0);
            data.push_back(0);
            data.push_back(PAATY_ZPOS);
            data.push_back(PAATY_WIDTH1);
            data.push_back(PAATY_WIDTH2);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT1);
            data.push_back(PAATY_ZPOS);
            data.push_back(PAATY_WIDTH1);
            data.push_back(PAATY_WIDTH2);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT2);
            data.push_back(PAATY_ZPOS);
            data.push_back(PAATY_WIDTH1);
            data.push_back(PAATY_WIDTH1);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(STATION_HEIGHT2);
            data.push_back(PAATY_ZPOS);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT3);
            data.push_back(PAATY_ZPOS);
            data.push_back(3);
            data.push_back(13);
            data.push_back(static_cast<int16_t>(PAATY_WIDTH1 / 2));
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CONE));
            data.push_back(0);
            data.push_back(PAATY_HEIGHT3);
            data.push_back(PAATY_ZPOS);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT4);
            data.push_back(PAATY_ZPOS);
            data.push_back(2);
            data.push_back(3);
            data.push_back(8);
            data.push_back(static_cast<int16_t>(PAATY_RADIUS1));
            data.push_back(static_cast<int16_t>(PAATY_RADIUS1));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CONE));
            data.push_back(0);
            data.push_back(PAATY_HEIGHT4);
            data.push_back(PAATY_ZPOS);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT5);
            data.push_back(PAATY_ZPOS);
            data.push_back(2);
            data.push_back(3);
            data.push_back(8);
            data.push_back(static_cast<int16_t>(PAATY_RADIUS1));
            data.push_back(static_cast<int16_t>(PAATY_RADIUS2));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CONE));
            data.push_back(0);
            data.push_back(PAATY_HEIGHT5);
            data.push_back(PAATY_ZPOS);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT6);
            data.push_back(PAATY_ZPOS);
            data.push_back(2);
            data.push_back(3);
            data.push_back(8);
            data.push_back(static_cast<int16_t>(PAATY_RADIUS2));
            data.push_back(static_cast<int16_t>(PAATY_RADIUS3));
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(0);
            data.push_back(PAATY_HEIGHT6);
            data.push_back(PAATY_ZPOS);
            data.push_back(0);
            data.push_back(PAATY_HEIGHT7);
            data.push_back(PAATY_ZPOS);
            data.push_back(3);
            data.push_back(8);
            data.push_back(static_cast<int16_t>(PAATY_RADIUS3));
            data.push_back(vgl::CSG_NO_FRONT);

            // Main door (smaller mid shape).
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(static_cast<int16_t>(MAIN_DOOR_LENGTH / 2));
            data.push_back(static_cast<int16_t>(MAIN_DOOR_HEIGHT1 / 2));
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-MAIN_DOOR_LENGTH / 2));
            data.push_back(static_cast<int16_t>(MAIN_DOOR_HEIGHT1 / 2));
            data.push_back(0);
            data.push_back(2);
            data.push_back(MAIN_DOOR_WIDTH);
            data.push_back(MAIN_DOOR_HEIGHT1);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

            data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
            data.push_back(2);
            data.push_back(0);
            data.push_back(MAIN_DOOR_HEIGHT1);
            data.push_back(0);
            data.push_back(MAIN_DOOR_LENGTH);
            data.push_back(MAIN_DOOR_WIDTH);
            data.push_back(0);
            data.push_back(MAIN_DOOR_HEIGHT2);
            data.push_back(0);
            data.push_back(MAIN_DOOR_LENGTH);
            data.push_back(1);
            data.push_back(2);
            data.push_back(3);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(MAIN_DOOR_HEIGHT2);
            data.push_back(0);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(MAIN_DOOR_HEIGHT1 * 15 / 16));
            data.push_back(static_cast<int16_t>(-MAIN_DOOR_WIDTH * 6 / 10));
            data.push_back(3);
            data.push_back(static_cast<int16_t>(MAIN_DOOR_LENGTH * 12 / 11));
            data.push_back(STATION_ROOF_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(0);
            data.push_back(MAIN_DOOR_HEIGHT2);
            data.push_back(0);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(MAIN_DOOR_HEIGHT1 * 15 / 16));
            data.push_back(static_cast<int16_t>(MAIN_DOOR_WIDTH * 6 / 10));
            data.push_back(3);
            data.push_back(static_cast<int16_t>(MAIN_DOOR_LENGTH * 12 / 11));
            data.push_back(STATION_ROOF_THICKNESS);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(static_cast<int16_t>(MAIN_DOOR_LENGTH * 12 / 11 / 2));
            data.push_back(MAIN_DOOR_HEIGHT2);
            data.push_back(0);
            data.push_back(static_cast<int16_t>(-MAIN_DOOR_LENGTH * 12 / 11 / 2));
            data.push_back(MAIN_DOOR_HEIGHT2);
            data.push_back(0);
            data.push_back(2);
            data.push_back(7);
            data.push_back(static_cast<int16_t>(STATION_ROOF_THICKNESS * 3 / 5));
            data.push_back(vgl::CSG_FLAT);

            // Ilmanvaihto ja antenni.
            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y1);
            data.push_back(ILMANVAIHTO_OFFSET1);
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y3);
            data.push_back(ILMANVAIHTO_OFFSET1);
            data.push_back(3);
            data.push_back(ILMANVAIHTO_WIDTH);
            data.push_back(ILMANVAIHTO_LENGTH);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y3);
            data.push_back(ILMANVAIHTO_OFFSET1);
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_Y3 + ILMANVAIHTO_WIDTH2));
            data.push_back(ILMANVAIHTO_OFFSET1);
            data.push_back(3);
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_WIDTH + ILMANVAIHTO_WIDTH2));
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_LENGTH + ILMANVAIHTO_WIDTH2));
            data.push_back(vgl::CSG_FLAT);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y1);
            data.push_back(ILMANVAIHTO_OFFSET2);
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y2);
            data.push_back(ILMANVAIHTO_OFFSET2);
            data.push_back(3);
            data.push_back(ILMANVAIHTO_WIDTH);
            data.push_back(ILMANVAIHTO_LENGTH);
            data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::BOX));
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(ILMANVAIHTO_Y2);
            data.push_back(ILMANVAIHTO_OFFSET2);
            data.push_back(ILMANVAIHTO_OFFSETX);
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_Y2 + ILMANVAIHTO_WIDTH2));
            data.push_back(ILMANVAIHTO_OFFSET2);
            data.push_back(3);
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_WIDTH + ILMANVAIHTO_WIDTH2));
            data.push_back(static_cast<int16_t>(ILMANVAIHTO_LENGTH + ILMANVAIHTO_WIDTH2));
            data.push_back(vgl::CSG_FLAT);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ILMANVAIHTO_Y1);
            data.push_back(ANTENNA_OFFSET);
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ANTENNA_HEIGHT2);
            data.push_back(ANTENNA_OFFSET);
            data.push_back(3);
            data.push_back(5);
            data.push_back(ANTENNA_RADIUS);
            data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ANTENNA_HEIGHT1);
            data.push_back(static_cast<int16_t>(ANTENNA_OFFSET + ANTENNA_INSET1));
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ANTENNA_HEIGHT1);
            data.push_back(static_cast<int16_t>(ANTENNA_OFFSET - ANTENNA_INSET1));
            data.push_back(2);
            data.push_back(5);
            data.push_back(ANTENNA_RADIUS);
            data.push_back(0);

            data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ANTENNA_HEIGHT2);
            data.push_back(static_cast<int16_t>(ANTENNA_OFFSET + ANTENNA_INSET1));
            data.push_back(ANTENNA_OFFSETX);
            data.push_back(ANTENNA_HEIGHT2);
            data.push_back(static_cast<int16_t>(ANTENNA_OFFSET - ANTENNA_INSET2));
            data.push_back(2);
            data.push_back(5);
            data.push_back(ANTENNA_RADIUS);
            data.push_back(0);

            csg_update("csg_kerava_station.hpp", data);
#else
#include "csg_kerava_station.hpp"
            auto data = CSG_READ_HPP(g_csg_kerava_station_hpp);
#endif

            // End windows.
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    2300, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    2300, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -2300, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -2300, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);

            // Mid windows.
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    780, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    480, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -780, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(-STATION_WIDTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -480, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    780, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    480, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -780, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(STATION_WIDTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    -480, 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);

            // Pääty windows.
            generate_window_whd(data, 0, STATION_WINDOW_Y1, static_cast<int16_t>(STATION_LENGTH / 2 + PAATY_OUTSET + STATION_WINDOW_R / 2),
                    STATION_WINDOW_W, STATION_WINDOW_H1, 0, STATION_WINDOW_R);
            generate_window_whd(data, 180, STATION_WINDOW_Y1, static_cast<int16_t>(-STATION_LENGTH / 2 - STATION_WINDOW_R / 2),
                    STATION_WINDOW_W, STATION_WINDOW_H1, 0, STATION_WINDOW_R);
            generate_window_whd(data, -180, STATION_WINDOW_Y1, static_cast<int16_t>(-STATION_LENGTH / 2 - STATION_WINDOW_R / 2),
                    STATION_WINDOW_W, STATION_WINDOW_H1, 0, STATION_WINDOW_R);

            // Main door windows.
            generate_window_whd(data, static_cast<int16_t>(-MAIN_DOOR_LENGTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                    0, 0, STATION_WINDOW_H1, static_cast<int16_t>(STATION_WINDOW_W * 2), STATION_WINDOW_R);
            generate_window_whd(data, static_cast<int16_t>(MAIN_DOOR_LENGTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                    0, 0, STATION_WINDOW_H1, static_cast<int16_t>(STATION_WINDOW_W * 2), STATION_WINDOW_R);

            // Mid shapes.
            auto generate_mid_shape = [STATION_WIDTH, STATION_HEIGHT2, STATION_HEIGHT3, STATION_ROOF_THICKNESS,
                 generate_window_whd, &data]
                (int16_t zpos)
            {
                const int16_t MID_LENGTH = 1220;

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(MID_LENGTH / 2));
                data.push_back(static_cast<int16_t>(STATION_HEIGHT2 / 2));
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(-MID_LENGTH / 2));
                data.push_back(static_cast<int16_t>(STATION_HEIGHT2 / 2));
                data.push_back(zpos);
                data.push_back(2);
                data.push_back(STATION_WIDTH);
                data.push_back(STATION_HEIGHT2);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

                data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                data.push_back(2);
                data.push_back(0);
                data.push_back(STATION_HEIGHT2);
                data.push_back(zpos);
                data.push_back(MID_LENGTH);
                data.push_back(STATION_WIDTH);
                data.push_back(0);
                data.push_back(STATION_HEIGHT3);
                data.push_back(zpos);
                data.push_back(MID_LENGTH);
                data.push_back(1);
                data.push_back(2);
                data.push_back(3);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_TOP);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(STATION_HEIGHT3);
                data.push_back(zpos);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(STATION_HEIGHT2 * 15 / 16));
                data.push_back(static_cast<int16_t>(zpos - STATION_WIDTH * 6 / 10));
                data.push_back(3);
                data.push_back(static_cast<int16_t>(MID_LENGTH * 12 / 11));
                data.push_back(STATION_ROOF_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(STATION_HEIGHT3);
                data.push_back(zpos);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(STATION_HEIGHT2 * 15 / 16));
                data.push_back(static_cast<int16_t>(zpos + STATION_WIDTH * 6 / 10));
                data.push_back(3);
                data.push_back(static_cast<int16_t>(MID_LENGTH * 12 / 11));
                data.push_back(STATION_ROOF_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(static_cast<int16_t>(MID_LENGTH * 12 / 11 / 2));
                data.push_back(STATION_HEIGHT3);
                data.push_back(zpos);
                data.push_back(static_cast<int16_t>(-MID_LENGTH * 12 / 11 / 2));
                data.push_back(STATION_HEIGHT3);
                data.push_back(zpos);
                data.push_back(2);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(STATION_ROOF_THICKNESS * 3 / 5));
                data.push_back(vgl::CSG_FLAT);

                generate_window_whd(data, static_cast<int16_t>(-MID_LENGTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos + 310), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(-MID_LENGTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos + 0), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(-MID_LENGTH / 2 - STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos - 310), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(MID_LENGTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos + 310), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(MID_LENGTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos + 0), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(MID_LENGTH / 2 + STATION_WINDOW_R), STATION_WINDOW_Y1,
                        static_cast<int16_t>(zpos - 310), 0, STATION_WINDOW_H1, STATION_WINDOW_W, STATION_WINDOW_R);
            };

            generate_mid_shape(STATION_MID_OFFSET);
            generate_mid_shape(static_cast<int16_t>(-STATION_MID_OFFSET));

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_kerava_station = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("kerava_station", *m_mesh_kerava_station);
#endif
        }

        // Function for generating pier shape.
        auto generate_pier = [](vgl::vector<int16_t>& data, const vgl::vec3& pos, const vgl::vec3& dir, int width,
                int height, int depth, int division)
        {
            static const int16_t PIER_GAP_WIDTH = 100;
            static const int16_t PIER_GAP_DEPTH = 12;
            static const int16_t PIER_GAP_LENGTH = 80;

            VGL_ASSERT(vgl::almost_equal(1.0f, length(dir)));

            const vgl::vec3 up(0.0f, 1.0f, 0.0f);
            vgl::vec3 rt = cross(dir, up);

            // Top plates (done first to reduce overdraw).
            {
                float len = static_cast<float>(width - ((division - 1) * PIER_GAP_LENGTH)) / static_cast<float>(division);
                vgl::vec3 curr = pos + dir * static_cast<float>(PIER_GAP_WIDTH / 2) +
                    up * static_cast<float>(height - PIER_GAP_DEPTH / 2) +
                    rt * static_cast<float>(width / 2);

                for(int ii = 0; (ii < division); ++ii)
                {
                    vgl::vec3 next = curr - rt * len;

                    data.push_back(to_int16(vgl::CsgCommand::BOX));
                    data.push_back(static_cast<int16_t>(vgl::iround(curr.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(curr.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(curr.z())));
                    data.push_back(static_cast<int16_t>(vgl::iround(next.x())));
                    data.push_back(static_cast<int16_t>(vgl::iround(next.y())));
                    data.push_back(static_cast<int16_t>(vgl::iround(next.z())));
                    data.push_back(2);
                    data.push_back(PIER_GAP_WIDTH);
                    data.push_back(PIER_GAP_DEPTH);
                    data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_RIGHT);

                    curr -= rt * (len + static_cast<float>(PIER_GAP_LENGTH));
                }
            }

            // Base form.
            {
                vgl::vec3 mid1 = pos + dir * static_cast<float>((depth - PIER_GAP_WIDTH) / 2 + PIER_GAP_WIDTH) +
                    up * static_cast<float>(height - PIER_GAP_DEPTH / 2) +
                    rt * static_cast<float>(width / 2);
                vgl::vec3 mid2 = mid1 - rt * static_cast<float>(width);
                vgl::vec3 mid3 = mid1 - up * static_cast<float>((height - PIER_GAP_DEPTH) / 2 + PIER_GAP_DEPTH / 2);
                vgl::vec3 mid4 = mid3 - rt * static_cast<float>(width);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.z())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.z())));
                data.push_back(2);
                data.push_back(static_cast<int16_t>(depth - PIER_GAP_WIDTH + 1));
                data.push_back(PIER_GAP_DEPTH);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(vgl::iround(mid3.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid3.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid3.z())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid4.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid4.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid4.z())));
                data.push_back(2);
                data.push_back(static_cast<int16_t>(depth - PIER_GAP_WIDTH));
                data.push_back(static_cast<int16_t>(height - PIER_GAP_DEPTH));
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_LEFT);
            }

            // Front.
            {
                vgl::vec3 mid1 = pos + dir * static_cast<float>(PIER_GAP_WIDTH / 2) +
                    up * static_cast<float>((height - PIER_GAP_DEPTH) / 2) +
                    rt * static_cast<float>(width / 2);
                vgl::vec3 mid2 = mid1 - rt * static_cast<float>(width);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid1.z())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.x())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.y())));
                data.push_back(static_cast<int16_t>(vgl::iround(mid2.z())));
                data.push_back(2);
                data.push_back(PIER_GAP_WIDTH);
                data.push_back(static_cast<int16_t>(height - PIER_GAP_DEPTH));
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_BOTTOM | vgl::CSG_NO_RIGHT);
            }
        };

        // Construct a station building
        {
            const int16_t BUILDING_L = 800;
            const int16_t BUILDING_INSET_D = 260;

#if defined(USE_LD)
            const int16_t BUILDING_W = 600;
            const int16_t BUILDING_H1 = 300;
            const int16_t BUILDING_H2 = 320;
            const int16_t BUILDING_H3 = 340;
            const int16_t BUILDING_INSET_L = 380;
            const int16_t ROOF_THICKNESS = 17;

            vgl::vector<int16_t> data;

            // Base form and roof.
            {
                int16_t endl = static_cast<int16_t>((BUILDING_L - BUILDING_INSET_L) / 2);
                int16_t zmid1 = static_cast<int16_t>((BUILDING_L / 2) - (endl / 2));
                int16_t zmid2 = static_cast<int16_t>(-zmid1);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(0);
                data.push_back(zmid1);
                data.push_back(0);
                data.push_back(BUILDING_H1);
                data.push_back(zmid1);
                data.push_back(3);
                data.push_back(BUILDING_W);
                data.push_back(endl);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                data.push_back(2);
                data.push_back(0);
                data.push_back(BUILDING_H1);
                data.push_back(zmid1);
                data.push_back(BUILDING_W);
                data.push_back(endl);
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(zmid1);
                data.push_back(1);
                data.push_back(endl);
                data.push_back(2);
                data.push_back(3);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(0);
                data.push_back(zmid2);
                data.push_back(0);
                data.push_back(BUILDING_H1);
                data.push_back(zmid2);
                data.push_back(3);
                data.push_back(BUILDING_W);
                data.push_back(endl);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::TRAPEZOID));
                data.push_back(2);
                data.push_back(0);
                data.push_back(BUILDING_H1);
                data.push_back(zmid2);
                data.push_back(BUILDING_W);
                data.push_back(endl);
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(zmid2);
                data.push_back(1);
                data.push_back(endl);
                data.push_back(2);
                data.push_back(3);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_LEFT | vgl::CSG_NO_RIGHT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(0);
                data.push_back(0);
                data.push_back(0);
                data.push_back(BUILDING_H2);
                data.push_back(0);
                data.push_back(3);
                data.push_back(BUILDING_INSET_D);
                data.push_back(BUILDING_INSET_L);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK | vgl::CSG_NO_TOP | vgl::CSG_NO_BOTTOM);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(BUILDING_W * 16 / 23));
                data.push_back(static_cast<int16_t>(BUILDING_H1 * 37 / 40));
                data.push_back(0);
                data.push_back(2);
                data.push_back(static_cast<int16_t>(BUILDING_L * 23 / 21));
                data.push_back(ROOF_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::BOX));
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(-BUILDING_W * 21 / 25));
                data.push_back(static_cast<int16_t>(BUILDING_H1 * 17 / 18));
                data.push_back(0);
                data.push_back(2);
                data.push_back(static_cast<int16_t>(BUILDING_L * 23 / 21));
                data.push_back(ROOF_THICKNESS);
                data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(static_cast<int16_t>(-BUILDING_L * 23 / 21 / 2));
                data.push_back(0);
                data.push_back(BUILDING_H3);
                data.push_back(static_cast<int16_t>(BUILDING_L * 23 / 21 / 2));
                data.push_back(2);
                data.push_back(5);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS * 2 / 3));
                data.push_back(vgl::CSG_FLAT);
            }

            // Tolpat.
            {
                const int16_t TOLPPA_OFFSET_X1 = -470;
                const int16_t TOLPPA_OFFSET_X2 = 370;
                const int16_t TOLPPA_OFFSET_Z = 410;
                const int16_t TOLPPA_HEIGHT = 280;

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(0);
                data.push_back(TOLPPA_OFFSET_Z);
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(TOLPPA_HEIGHT);
                data.push_back(TOLPPA_OFFSET_Z);
                data.push_back(1);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(-TOLPPA_OFFSET_Z));
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(TOLPPA_HEIGHT);
                data.push_back(static_cast<int16_t>(-TOLPPA_OFFSET_Z));
                data.push_back(1);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(TOLPPA_OFFSET_X2);
                data.push_back(0);
                data.push_back(TOLPPA_OFFSET_Z);
                data.push_back(TOLPPA_OFFSET_X2);
                data.push_back(TOLPPA_HEIGHT);
                data.push_back(TOLPPA_OFFSET_Z);
                data.push_back(1);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(TOLPPA_OFFSET_X2);
                data.push_back(0);
                data.push_back(static_cast<int16_t>(-TOLPPA_OFFSET_Z));
                data.push_back(TOLPPA_OFFSET_X2);
                data.push_back(TOLPPA_HEIGHT);
                data.push_back(static_cast<int16_t>(-TOLPPA_OFFSET_Z));
                data.push_back(1);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(0);
                data.push_back(0);
                data.push_back(TOLPPA_OFFSET_X1);
                data.push_back(TOLPPA_HEIGHT);
                data.push_back(0);
                data.push_back(1);
                data.push_back(7);
                data.push_back(static_cast<int16_t>(ROOF_THICKNESS / 2));
                data.push_back(vgl::CSG_NO_FRONT | vgl::CSG_NO_BACK);
            }

            csg_update("csg_station_building.hpp", data);
#else
#include "csg_station_building.hpp"
            auto data = CSG_READ_HPP(g_csg_station_building_hpp);
#endif

            // Ikkunat.
            {
                const int16_t WINDOW_H = 170;
                const int16_t WINDOW_W = 90;
                const int16_t WINDOW_Y = 160;
                const int16_t WINDOW_R = 14;

                generate_window_whd(data, -120, WINDOW_Y, static_cast<int16_t>(BUILDING_L / 2 + WINDOW_R / 2),
                        WINDOW_W, WINDOW_H, 0, WINDOW_R);
                generate_window_whd(data, 120, WINDOW_Y, static_cast<int16_t>(BUILDING_L / 2 + WINDOW_R / 2),
                        WINDOW_W, WINDOW_H, 0, WINDOW_R);
                generate_window_whd(data, -120, WINDOW_Y, static_cast<int16_t>(-BUILDING_L / 2 - WINDOW_R / 2),
                        WINDOW_W, WINDOW_H, 0, WINDOW_R);
                generate_window_whd(data, 120, WINDOW_Y, static_cast<int16_t>(-BUILDING_L / 2 - WINDOW_R / 2),
                        WINDOW_W, WINDOW_H, 0, WINDOW_R);

                generate_window_whd(data, static_cast<int16_t>(-BUILDING_INSET_D / 2 - WINDOW_R / 2), WINDOW_Y, 0,
                        0, WINDOW_H, static_cast<int16_t>(WINDOW_W * 2), WINDOW_R);
                generate_window_whd(data, static_cast<int16_t>(BUILDING_INSET_D / 2 + WINDOW_R / 2), WINDOW_Y, 0,
                        0, WINDOW_H, static_cast<int16_t>(WINDOW_W * 2), WINDOW_R);
            }

            // Pier aseman alle.
            const int16_t PIER_OFFSET_X = -850;
            const int16_t PIER_OFFSET_Z = -430;
            const int16_t PIER_EXTENT = 3500;
            const int16_t PIER_WIDTH = 5960;
            const int16_t PIER_HEIGHT = 440;

            generate_pier(data, vgl::vec3(static_cast<float>(PIER_OFFSET_X), static_cast<float>(-PIER_HEIGHT),
                        static_cast<float>(PIER_OFFSET_Z)),
                    vgl::vec3(1.0f, 0.0f, 0.0f), PIER_WIDTH, PIER_HEIGHT, PIER_EXTENT, 16);

            data.push_back(to_int16(vgl::CsgCommand::NONE));
            {
                vgl::LogicalMesh lmesh(data.data());
                m_mesh_station_building = lmesh.compile();
            }
#if defined(USE_LD)
            addPreviewMesh("station_building", *m_mesh_station_building);
#endif
        }

        // Sirkus hevo nen.
        {
#include "mesh_sirkus_hevo_nen.hpp"
            const float SIRKUS_HEVO_NEN_SCALE = 0.0005f;

            vgl::LogicalMesh lmesh(g_vertices_g_sirkus_hevo_nen, g_indices_g_sirkus_hevo_nen,
                    g_vertices_g_sirkus_hevo_nen_size, g_indices_g_sirkus_hevo_nen_size,
                    SIRKUS_HEVO_NEN_SCALE);
            m_mesh_sirkus_hevo_nen = lmesh.compile();
#if defined(USE_LD)
            addPreviewMesh("sirkus_hevo_nen", *m_mesh_sirkus_hevo_nen);
#endif
        }

        // Construct world 0 (first-person train carriage).
#if defined(USE_LD)
        if(g_preview_mesh.empty() || g_preview_mesh.starts_with("world"))
#endif
        {
            IntroWorld& world = m_world[0];
            vgl::GlslProgram& prog = getProgramOffscreen();

            world.addEntity(prog, getMeshSm5Interior(), 0, 0, 0, 0, 0, 0);

            // Elevated chairs.
            world.addEntity(prog, getMeshSm5Chair(1), -12, 6, -73, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(2), -7, 6, -73, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(0), 7, 6, -73, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 6, -73, 0, 0, 0);

            world.addEntity(prog, getMeshSm5Chair(1), -12, 6, -89, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(0), -7, 6, -89, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(2), 7, 6, -89, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 6, -89, 0, 128, 0);

            // Chairs behind first doors.
            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, -49, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(2), -7, 0, -49, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(0), 7, 0, -49, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, -49, 0, 0, 0);

            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, -65, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(0), -7, 0, -65, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(2), 7, 0, -65, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, -65, 0, 128, 0);

            // Chairs before first doors.
            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, -5, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(2), -7, 0, -5, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(0), 7, 0, -5, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, -5, 0, 0, 0);

            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, -20, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(0), -7, 0, -20, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(2), 7, 0, -20, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, -20, 0, 128, 0);

            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, 20, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(2), -7, 0, 20, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(0), 7, 0, 20, 0, 0, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, 20, 0, 0, 0);

            world.addEntity(prog, getMeshSm5Chair(1), -12, 0, 5, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(0), -7, 0, 5, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(2), 7, 0, 5, 0, 128, 0);
            world.addEntity(prog, getMeshSm5Chair(1), 12, 0, 5, 0, 128, 0);

            world.sort();
        }

        // Bridges.
        auto lay_bridge = [this](IntroWorld& world, const vgl::GlslProgram& prog, const vgl::vec3& origin, const vgl::vec3& dir,
                int neg, int pos)
        {
            const float BRIDGE_LENGTH = 65.0f;
            const float BRIDGE_LAMPPOST_OFFSET_X = 6.03f;
            const float BRIDGE_LAMPPOST_OFFSET_Y = 10.6f;
            const float BRIDGE_LAMPPOST_OFFSET_Z1 = -30.5f;
            const float BRIDGE_LAMPPOST_OFFSET_Z2 = -14.25f;
            const float BRIDGE_LAMPPOST_OFFSET_Z3 = 2.0f;
            const float BRIDGE_LAMPPOST_OFFSET_Z4 = 18.25f;
            const vgl::vec3 fw = normalize(dir);
            const vgl::vec3 up(0.0f, 1.0f, 0.0f);
            const vgl::vec3 rt = cross(fw, up);
            for(int ii = neg; (ii <= pos); ++ii)
            {
                vgl::vec3 advance = fw * (BRIDGE_LENGTH * static_cast<float>(ii));
                vgl::vec3 cpos = origin + advance;
                vgl::mat4 trns = vgl::mat4::lookat(cpos, cpos + dir, vgl::vec3(0.0f, 1.0f, 0.0f));

                world.addEntity(prog, getMeshBridge(), trns);

                // Lightposts.
                {
                    vgl::vec3 pos1 = origin + advance - rt * BRIDGE_LAMPPOST_OFFSET_X + up * BRIDGE_LAMPPOST_OFFSET_Y +
                        fw * BRIDGE_LAMPPOST_OFFSET_Z1;
                    vgl::vec3 pos2 = origin + advance + rt * BRIDGE_LAMPPOST_OFFSET_X + up * BRIDGE_LAMPPOST_OFFSET_Y +
                        fw * BRIDGE_LAMPPOST_OFFSET_Z2;
                    vgl::vec3 pos3 = origin + advance - rt * BRIDGE_LAMPPOST_OFFSET_X + up * BRIDGE_LAMPPOST_OFFSET_Y +
                        fw * BRIDGE_LAMPPOST_OFFSET_Z3;
                    vgl::vec3 pos4 = origin + advance + rt * BRIDGE_LAMPPOST_OFFSET_X + up * BRIDGE_LAMPPOST_OFFSET_Y +
                        fw * BRIDGE_LAMPPOST_OFFSET_Z4;
                    vgl::mat4 trns1 = vgl::mat4::rotation_euler(0.0f, static_cast<float>(M_PI * 1.5f), 0.0f, pos1);
                    vgl::mat4 trns2 = vgl::mat4::rotation_euler(0.0f, static_cast<float>(M_PI * 0.5f), 0.0f, pos2);
                    vgl::mat4 trns3 = vgl::mat4::rotation_euler(0.0f, static_cast<float>(M_PI * 1.5f), 0.0f, pos3);
                    vgl::mat4 trns4 = vgl::mat4::rotation_euler(0.0f, static_cast<float>(M_PI * 0.5f), 0.0f, pos4);

                    world.addEntity(prog, getMeshLamppost(), trns1);
                    world.addEntity(prog, getMeshLamppost(), trns2);
                    world.addEntity(prog, getMeshLamppost(), trns3);
                    world.addEntity(prog, getMeshLamppost(), trns4);
                }
            }
        };

        // Construct world 1 (first-person track) and world 2 (signs, same space as first-person track).
#if defined(USE_LD)
        if(g_preview_mesh.empty() || g_preview_mesh.starts_with("terrain") || g_preview_mesh.starts_with("world"))
#endif
        {
            const int HORIZ_FIDELITY = 160;
            const int HORIZ_FIDELITY_X2 = 50;
            const int HORIZ_ADJUSTED_FIDELITY = HORIZ_FIDELITY - HORIZ_FIDELITY_X2;
            const int VERT_BLOCK_FIDELITY = HORIZ_FIDELITY / 10;
            const int VERT_FIDELITY = VERT_BLOCK_FIDELITY * TERRAIN_BLOCK_COUNT;
            const float UNIT_WIDTH_X = TERRAIN_BLOCK_WIDTH / static_cast<float>(HORIZ_FIDELITY);
            const float UNIT_WIDTH_Z = TERRAIN_BLOCK_LENGTH / VERT_BLOCK_FIDELITY;
            const float INV_HORIZ = 1.0f / static_cast<float>(HORIZ_FIDELITY);
            const float INV_VERT = 1.0f / static_cast<float>(VERT_FIDELITY);

            vgl::Image2DGray img(HORIZ_FIDELITY, VERT_FIDELITY);

            // Set the terrain seed.
            {
                unsigned terrain_seed = 1096;
#if defined(USE_LD)
                if(g_seed)
                {
                    terrain_seed = *g_seed;
                }
#endif
                dnload_srand(terrain_seed);
            }

            img.noise(-45.0f, 47.0f);
            img.filterLowpass(4);
            img.filterLowpass(2);

            for(int jj = 0; (jj < VERT_FIDELITY); ++jj)
            {
                unsigned imgy = static_cast<unsigned>(jj);

                for(int ii = 0; (ii <= HORIZ_FIDELITY); ++ii)
                {
                    unsigned imgx = static_cast<unsigned>((ii >= HORIZ_FIDELITY) ? 0 : ii);

                    auto track_shape = [](float px, float py) -> float
                    {
                        float bump_noise = frand(1.0f - TERRAIN_BUMP_NOISE, 1.0f + TERRAIN_BUMP_NOISE) * TERRAIN_BUMP_HEIGHT;
                        return py * vgl::smooth_step(TERRAIN_SLOPE_START, TERRAIN_SLOPE_END, abs(px)) +
                            vgl::smooth_step(-TERRAIN_BUMP_END, -TERRAIN_BUMP_START, -abs(px)) * bump_noise;
                    };

                    float xx = (static_cast<float>(ii) - static_cast<float>(HORIZ_FIDELITY / 2)) * UNIT_WIDTH_X;
                    float yy = img.getPixel(imgx, imgy);
                    img.setPixel(imgx, imgy, track_shape(xx, yy));
                }
            }

            for(int kk = 0; (kk < TERRAIN_BLOCK_COUNT); ++kk)
            {
                vgl::LogicalMesh lmesh;
                int bb = kk * VERT_BLOCK_FIDELITY;

                for(int jj = 0; (jj <= VERT_BLOCK_FIDELITY); ++jj)
                {
                    // Create vertices, more polygons at center.
                    for(int ii = 0; (ii <= HORIZ_FIDELITY); ++ii)
                    {
                        float x1 = static_cast<float>(ii);
                        float z1 = static_cast<float>(jj);
                        // Seam must match, so no jitter if there.
                        if((ii != 0) && (jj != 0) && (ii < HORIZ_FIDELITY) && (jj < VERT_BLOCK_FIDELITY))
                        {
                            x1 += frand(-TERRAIN_TILE_JITTER, TERRAIN_TILE_JITTER);
                            z1 += frand(-TERRAIN_TILE_JITTER, TERRAIN_TILE_JITTER);
                        }
                        float y1 = img.sampleLinear(x1 * INV_HORIZ, (z1 + static_cast<float>(bb)) * INV_VERT);
                        x1 = (x1 - static_cast<float>(HORIZ_FIDELITY / 2)) * UNIT_WIDTH_X;
                        z1 *= UNIT_WIDTH_Z;

                        // Intentionally becomes an orphaned vertex at the last row/column.
                        float x5 = static_cast<float>(ii) + 0.5f + frand(-TERRAIN_TILE_JITTER, TERRAIN_TILE_JITTER);
                        float z5 = static_cast<float>(jj) + 0.5f + frand(-TERRAIN_TILE_JITTER, TERRAIN_TILE_JITTER);
                        float y5 = img.sampleLinear(x5 * INV_HORIZ, (z5 + static_cast<float>(bb)) * INV_VERT);
                        x5 = (x5 - static_cast<float>(HORIZ_FIDELITY / 2)) * UNIT_WIDTH_X;
                        z5 *= UNIT_WIDTH_Z;

                        lmesh.addVertex(x1, y1, z1);
                        lmesh.addVertex(x5, y5, z5);

                        // Advance twice if we're in half fidelity.
                        if((ii < HORIZ_FIDELITY_X2) || (ii >= (HORIZ_FIDELITY - HORIZ_FIDELITY_X2)))
                        {
                            ++ii;
                        }
                    }

                    // Create faces.
                    if(jj < VERT_BLOCK_FIDELITY)
                    {
                        for(int ii = 0; (ii < HORIZ_ADJUSTED_FIDELITY); ++ii)
                        {
                            unsigned v1 = static_cast<unsigned>((ii + jj * (HORIZ_ADJUSTED_FIDELITY + 1)) * 2);
                            unsigned v2 = static_cast<unsigned>((ii + (jj + 1) * (HORIZ_ADJUSTED_FIDELITY + 1)) * 2);
                            unsigned v3 = static_cast<unsigned>((ii + 1 + (jj + 1) * (HORIZ_ADJUSTED_FIDELITY + 1)) * 2);
                            unsigned v4 = static_cast<unsigned>((ii + 1 + jj * (HORIZ_ADJUSTED_FIDELITY + 1)) * 2);
                            unsigned v5 = static_cast<unsigned>((ii + jj * (HORIZ_ADJUSTED_FIDELITY + 1)) * 2 + 1);

                            lmesh.addFace(v1, v2, v5, true);
                            lmesh.addFace(v2, v3, v5, true);
                            lmesh.addFace(v3, v4, v5, true);
                            lmesh.addFace(v4, v1, v5, true);
                        }
                    }
                }

                m_mesh_terrain[kk] = lmesh.compile(false);
#if defined(USE_LD)
                addPreviewMesh(("terrain" + std::to_string(kk)).c_str(), *m_mesh_terrain[kk]);
#endif
            }

            // World layout.
            {
                IntroWorld& world = m_world[1];
                const vgl::GlslProgram& prog = getProgramOffscreen();

                // Helper function to place entities.
                auto place_entity = [&world, &img](const vgl::GlslProgram& program, vgl::Mesh& mesh, int px, int py,
                        int pz, int ry, bool allow_conflict = true) -> bool
                {
                    float fx = static_cast<float>(px) * 0.1f;
                    float fy = static_cast<float>(py) * 0.1f;
                    float fz = static_cast<float>(pz) * 0.1f;
                    float rel_x = 0.5f + (fx / TERRAIN_BLOCK_WIDTH);
                    float rel_y = fz / (TERRAIN_BLOCK_LENGTH * static_cast<float>(TERRAIN_BLOCK_COUNT));
                    float sy = img.sampleLinear(rel_x, rel_y);
                    vgl::vec3 pos(fx, fy + sy, fz);
                    float fry = static_cast<float>(ry) * static_cast<float>(M_PI * 2.0f / 256.0);
                    if(allow_conflict)
                    {
                        world.addEntity(program, mesh, pos, 0.0f, fry, 0.0f);
                    }
                    return world.tryAddEntity(program, mesh, pos, 0.0f, fry, 0.0f);
                };

                // Fences.
                {
                    const float FENCE_XF = vgl::mix(TERRAIN_SLOPE_START, TERRAIN_SLOPE_END, 0.1f);
                    const int16_t FENCE_X = static_cast<int16_t>(vgl::iround(FENCE_XF * 10.0f));
                    const int16_t FENCE_LENGTH = 50;
                    const int16_t FENCE_LEFT_GAP_START = -4020;
                    const int16_t FENCE_LEFT_GAP_END = -4620;
                    const int16_t FENCE_RIGHT_GAP_START = -1950;
                    const int16_t FENCE_RIGHT_GAP_END = -2590;

                    for(int zpos = 0; (zpos > TERRAIN_LAST_ZPOS); zpos -= FENCE_LENGTH)
                    {
                        if((zpos >= FENCE_RIGHT_GAP_START) || (zpos <= FENCE_RIGHT_GAP_END))
                        {
                            place_entity(prog, getMeshFence(), FENCE_X, 0, zpos, 0);
                        }
                        if((zpos >= FENCE_LEFT_GAP_START) || (zpos <= FENCE_LEFT_GAP_END))
                        {
                            place_entity(prog, getMeshFence(), -FENCE_X, 0, zpos, 0);
                        }
                    }
                }

                // Smaller station 1 and associated models.
                {
                    const int16_t STATION_CENTER = -2210;
                    world.addEntity(prog, getMeshFence(), 70, 14, STATION_CENTER + 245, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 120, 14, STATION_CENTER + 245, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 170, 14, STATION_CENTER + 245, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 220, 14, STATION_CENTER + 245, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 270, 14, STATION_CENTER + 245, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 70, 14, STATION_CENTER - 337, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 120, 14, STATION_CENTER - 337, 0, 64, 0);
                    world.addEntity(prog, getMeshFence(), 170, 14, STATION_CENTER - 337, 0, 64, 0);
                    world.addEntity(prog, getMeshKatos(), 110, 19, STATION_CENTER - 181, 0, 0, 0);
                    world.addEntity(prog, getMeshPylonExtraBase(), 79, 21, STATION_CENTER + 109);
                    world.addEntity(prog, getMeshLamppost(), 240, 19, STATION_CENTER - 6, 0, 128, 0);
                    world.addEntity(prog, getMeshStationBuilding(), 148, 19, STATION_CENTER, 0, 0, 0);
                }
                // Smaller station 2 and associated models.
                {
                    const int16_t STATION_CENTER = -4344;
                    world.addEntity(prog, getMeshFence(), -70, 14, STATION_CENTER + 335, 0, 192, 0);
                    world.addEntity(prog, getMeshFence(), -120, 14, STATION_CENTER + 335, 0, 192, 0);
                    world.addEntity(prog, getMeshFence(), -70, 14, STATION_CENTER + 335, 0, 192, 0);
                    world.addEntity(prog, getMeshFence(), -70, 14, STATION_CENTER - 247, 0, 192, 0);
                    world.addEntity(prog, getMeshFence(), -120, 14, STATION_CENTER - 247, 0, 192, 0);
                    world.addEntity(prog, getMeshFence(), -170, 14, STATION_CENTER - 247, 0, 192, 0);
                    world.addEntity(prog, getMeshKatos(), -140, 19, STATION_CENTER + 194, 0, 0, 0);
                    world.addEntity(prog, getMeshPylonExtraBase(), -79, 21, STATION_CENTER + 294);
                    world.addEntity(prog, getMeshLamppost(), -240, 19, STATION_CENTER - 136, 0, 0, 0);
                    world.addEntity(prog, getMeshLamppost(), -240, 19, STATION_CENTER + 94, 0, 0, 0);
                    world.addEntity(prog, getMeshStationBuilding(), -148, 19, STATION_CENTER, 0, 128, 0);
                }

                // Pylons and rails.
                {
                    const int PYLON_IX = static_cast<int>(PYLON_X * 10.0f);
                    const int RAILS_IX = static_cast<int>(RAILS_X * 10.0f);
                    const int RAILS_IY = static_cast<int>(RAILS_Y * 10.0f);
                    const int PYLON_LENGTH = 550;
                    const int RAILS_LENGTH = 100;

                    for(int zpos = 0; (zpos > TERRAIN_LAST_ZPOS); zpos -= PYLON_LENGTH)
                    {
                        // Arc or double pylon.
                        if(!(dnload_rand() % 6))
                        {
                            world.addEntity(prog, getMeshArc(), 0, RAILS_IY, zpos, 0, 0, 0);
                        }
                        else
                        {
                            world.addEntity(prog, getMeshPylon(), -PYLON_IX, RAILS_IY, zpos, 0, 0, 0);
                            world.addEntity(prog, getMeshPylon(), PYLON_IX, RAILS_IY, zpos, 0, 128, 0);
                        }

                        // Tendons.
                        world.addEntity(prog, getMeshTendons(), -PYLON_IX, RAILS_IY, zpos, 0, 0, 0);
                        world.addEntity(prog, getMeshTendons(), PYLON_IX, RAILS_IY, zpos, 0, 128, 0);
                    }

                    for(int zpos = 0; (zpos > TERRAIN_LAST_ZPOS); zpos -= RAILS_LENGTH)
                    {
                        world.addEntity(prog, getMeshRails(), RAILS_IX, RAILS_IY, zpos);
                        world.addEntity(prog, getMeshRails(), -RAILS_IX, RAILS_IY, zpos);
                    }
                }

                lay_bridge(world, prog, vgl::vec3(0.0f, 0.0f, -285.0f), vgl::vec3(1.0f, 0.0f, 0.0f), -4, 4);

                // Buildings (Mega-Kerava 1).
                {
                    const float CITY_CENTER_ZF = -960.0f;
                    const int16_t CITY_CENTER_Z = -9600;
                    const int16_t CITY_AREA_X = 940;
                    const int16_t CITY_AREA_Z = 1800;

                    dnload_srand(9);

                    // Lay bridges at the center of city.
                    // Actually not that many blocks left or right... we wouldn't see them.
                    lay_bridge(world, prog, vgl::vec3(0.0f, 0.0f, CITY_CENTER_ZF + 110.0f), vgl::vec3(1.0f, 0.0f, 0.0f), 0, 1);
                    lay_bridge(world, prog, vgl::vec3(0.0f, 0.0f, CITY_CENTER_ZF + 20.0f), vgl::vec3(1.0f, 0.0f, 0.0f), 0, 0);

                    // Lay known buildings.
                    world.addEntity(prog, getMeshKeravaStateBuilding(), 250, 2, CITY_CENTER_Z + 880, landmark_callback);
                    world.addEntity(prog, getMeshJohnKeravaCenter(), -340, -16, CITY_CENTER_Z - 360, landmark_callback);
                    world.addEntity(prog, getMeshKeravanasTowers(), 0, -22, CITY_CENTER_Z - 1530, landmark_callback);
                    world.addEntity(prog, getMeshBurjKerava(), -510, -22, CITY_CENTER_Z - 4100, landmark_callback);

                    // Blocks sight of inconvenient bridge.
                    place_entity(prog, getMeshBuilding(2), 880, -22, CITY_CENTER_Z + 1320, 0, true);

                    for(unsigned ii = 0; (ii < 78); ++ii)
                    {
                        // Loop until placement successful.
                        for(;;)
                        {
                            int px = dnload_rand() % (CITY_AREA_X * 2 + 1) - CITY_AREA_X;
                            int pz = dnload_rand() % (CITY_AREA_Z * 2 + 1) - CITY_AREA_Z;

                            float rx = static_cast<float>(px) / static_cast<float>(CITY_AREA_X);
                            float rz = static_cast<float>(pz) / static_cast<float>(CITY_AREA_Z);

                            float rel_len = length(vgl::vec2(static_cast<float>(rx), static_cast<float>(rz)));
                            float idx_dec = frand(1.22f);
                            idx_dec *= idx_dec * idx_dec * idx_dec * 4.0f;
                            int idx = BUILDING_COUNT - vgl::iround(rel_len * static_cast<float>(BUILDING_COUNT) + idx_dec);

                            if(idx >= 0)
                            {
                                int iz = pz + CITY_CENTER_Z;
                                int ry = (dnload_rand() % 2) ? 64 : 0;

                                if(place_entity(prog, getMeshBuilding(idx), px, -22, iz, ry, false))
                                {
                                    break;
                                }
                            }
                        }
                    }
                }

                // Terrain added as last, so it won't conflict with buildings.
                // The terrain will loop after 12800 units.
                {
                    for(int ii = 0; true; --ii)
                    {
                        vgl::Mesh& terrain_mesh = getMeshTerrain(ii);
                        int zpos = ii * static_cast<int>(TERRAIN_BLOCK_LENGTH * 10);
                        world.addEntity(prog, terrain_mesh, 0, 0, zpos);

                        if(zpos <= TERRAIN_LAST_ZPOS)
                        {
                            break;
                        }
                    }
                }

                world.sort();
            }

            // Signs.
            {
                IntroWorld& world = m_world[2];
                vgl::GlslProgram& prog = getProgramOffscreen();
                vgl::GlslProgram& fontProg = getProgramFont();
                vgl::Font& font = getFont();

                // Add all signs from data array.
                {
                    for(const SignPosition* sign = g_sign_positions; sign->m_text; ++sign)
                    {
                        // Add sign.
                        int sx = sign->m_data[0];
                        int sy = sign->m_data[1];
                        int sz = sign->m_data[2];
                        vgl::Mesh& smesh = getMeshSign(static_cast<unsigned>(sign->m_data[3]));
                        world.addEntity(prog, smesh, sx, sy, sz);

                        // Add text.
                        vgl::mat4 trns = world_position(sx + sign->m_data[4], sy + sign->m_data[5], sz + 1);
                        world.addEntity(fontProg, font, getGlyph(), vgl::vec2(static_cast<float>(sign->m_data[6] * 0.1f)),
                                vgl::vec3(0.0f, 0.0f, 0.0f), sign->m_text, trns);
                    }
                }

                // Calculate sign easing.
                {
                    const unsigned SIGN_FRAMES = 3000;
                    const float TRAIN_SPEED = -0.92f;
                    const float MIN_TIME_ADVANCE = 0.03f;
                    const float SIGN_TOLERANCE_MAX = 46.0f;
                    const float SIGN_TOLERANCE_MIN = 8.0f;
                    const float SIGN_TOLERANCE_FORGET = -22.0f;
                    const float EASING_POWER = 3.0f;
                    const float EASING_DECAY = 0.8f;

                    float zpos = 0.0f;
                    float easing = 0.0f;

                    for(unsigned ii = 0; (ii < SIGN_FRAMES); ++ii)
                    {
                        // Need to initialize spos to prevent inf, even if easing will eliminate it.
                        vgl::vec3 spos(0.0f, 0.0f, 0.0f);
                        bool decay = true;

                        for(const SignPosition* sign = g_sign_positions; sign->m_text; ++sign)
                        {
                            vgl::vec3 cpos = vgl::vec3(static_cast<float>(sign->m_data[0]),
                                    static_cast<float>(sign->m_data[1] + sign->m_data[5] / 2),
                                    static_cast<float>(sign->m_data[2])) * 0.1f;
                            float diff = zpos - cpos.z();

                            if(diff < SIGN_TOLERANCE_MAX)
                            {
                                if(diff > SIGN_TOLERANCE_MIN)
                                {
                                    float dist = (diff - SIGN_TOLERANCE_MIN) / (SIGN_TOLERANCE_MAX - SIGN_TOLERANCE_MIN);
                                    easing = vgl::pow(1.0f - dist, EASING_POWER);
                                    spos = cpos;
                                    decay = false;
                                    break;
                                }
                                // If forget distance has not been reached, keep the target anyway.
                                if(diff > SIGN_TOLERANCE_FORGET)
                                {
                                    spos = cpos;
                                    break;
                                }
                            }
                        }

                        // If we're not approaching a sign, decay.
                        if(decay)
                        {
                            easing = easing * EASING_DECAY;
                        }

                        zpos += TRAIN_SPEED * vgl::max(1.0f - easing, MIN_TIME_ADVANCE);

                        m_sign_easing.emplace_back(zpos, spos, easing);
                    }
                }

                world.sort();
            }

            // Camera jitter (calculated for every intro frame - why not?
            {
                const float CAMERA_JITTER_INC = 0.0008f;
                const float CAMERA_JITTER_MUL = 0.94f;

                vgl::vec3 jitter(0.0f, 0.0f, 0.0f);

                for(unsigned ii = 0; (ii < INTRO_LENGTH); ++ii)
                {
                    float jx = frand(-CAMERA_JITTER_INC, CAMERA_JITTER_INC);
                    float jy = frand(-CAMERA_JITTER_INC, CAMERA_JITTER_INC);
                    float jz = frand(-CAMERA_JITTER_INC, CAMERA_JITTER_INC);

                    jitter = jitter * CAMERA_JITTER_MUL + vgl::vec3(jx, jy, jz);
                    m_camera_jitter.push_back(jitter);
                }
            }
        }

        // World 3 (Kerava station).
#if defined(USE_LD)
        if(g_preview_mesh.empty() || g_preview_mesh.starts_with("terrain") || g_preview_mesh.starts_with("world"))
#endif
        {
            // Kerava station terrain.
            {
                static const int16_t PIER_LENGTH = 20000;
                static const int16_t PIER_GAP_DEPTH = 12;
                static const int16_t PIER_HEIGHT = 200;
                static const int16_t PIER_EXTENT = 20000;
                static const int16_t PIER_FAR = -2790;

                vgl::vector<int16_t> data;

                generate_pier(data, vgl::vec3(0.0f, 0.0f, static_cast<float>(-PIER_HEIGHT)), vgl::vec3(1.0f, 0.0f, 0.0f),
                        static_cast<int>(PIER_LENGTH * 2), PIER_HEIGHT, PIER_EXTENT, 40);

                generate_pier(data, vgl::vec3(static_cast<float>(PIER_FAR), 0.0f, static_cast<float>(-PIER_HEIGHT)),
                        vgl::vec3(-1.0f, 0.0f, 0.0f),
                        static_cast<int>(PIER_LENGTH * 2), PIER_HEIGHT, PIER_EXTENT, 40);

                {
                    const int16_t ARC_BASE_WIDTH = 112;
                    const int16_t PYLON_LENGTH = 5500;

                    for(int16_t zpos = PIER_LENGTH; (zpos > static_cast<int16_t>(-PIER_LENGTH));
                            zpos = static_cast<int16_t>(zpos - PYLON_LENGTH))
                    {
                        int16_t xpos = static_cast<int16_t>(PIER_FAR - (ARC_BASE_WIDTH / 2) - 4);

                        data.push_back(to_int16(vgl::CsgCommand::BOX));
                        data.push_back(xpos);
                        data.push_back(PIER_HEIGHT);
                        data.push_back(zpos);
                        data.push_back(xpos);
                        data.push_back(static_cast<int16_t>(PIER_HEIGHT + PIER_GAP_DEPTH * 2));
                        data.push_back(zpos);
                        data.push_back(3);
                        data.push_back(ARC_BASE_WIDTH);
                        data.push_back(ARC_BASE_WIDTH);
                        data.push_back(vgl::CSG_FLAT | vgl::CSG_NO_FRONT);
                    }
                }

                data.push_back(to_int16(vgl::CsgCommand::NONE));
                {
                    vgl::LogicalMesh lmesh(data.data());

                    // Add the terrain into between piers.
                    {
                        const unsigned HORIZ_FIDELITY = 19;
                        const unsigned VERT_FIDELITY = 180;
                        const float TERRAIN_LT = -28.1f;
                        const float TERRAIN_RT = 0.2f;
                        const float TERRAIN_BK = static_cast<float>(20000) * 0.01f;
                        const float TERRAIN_FW = -TERRAIN_BK;

                        vgl::Image2DGray img(HORIZ_FIDELITY, VERT_FIDELITY);

                        dnload_srand(1);

                        img.noise(-0.5f, 1.1f);

                        img.filterLowpass(2);

                        unsigned index_base = lmesh.getLogicalVertexCount();

                        for(unsigned jj = 0; (jj <= VERT_FIDELITY); ++jj)
                        {
                            unsigned yc = (jj >= VERT_FIDELITY) ? 0 : jj;
                            float fj = static_cast<float>(jj) / static_cast<float>(VERT_FIDELITY);
                            float pz = vgl::mix(TERRAIN_BK, TERRAIN_FW, fj);

                            for(unsigned ii = 0; (ii <= HORIZ_FIDELITY); ++ii)
                            {
                                unsigned xc = (ii >= HORIZ_FIDELITY) ? 0 : ii;
                                float fi = static_cast<float>(ii) / static_cast<float>(HORIZ_FIDELITY);
                                float px = vgl::mix(TERRAIN_LT, TERRAIN_RT, fi);

                                float py = img.getPixel(xc, yc);

                                lmesh.addVertex(px, py, pz);

                                if((ii < HORIZ_FIDELITY) && (jj < VERT_FIDELITY))
                                {
                                    unsigned row1 = index_base + (jj * (HORIZ_FIDELITY + 1));
                                    unsigned row2 = row1 + HORIZ_FIDELITY + 1;
                                    lmesh.addFace(row1 + ii, row1 + ii + 1, row2 + ii + 1, true);
                                    lmesh.addFace(row1 + ii, row2 + ii + 1, row2 + ii, true);
                                }
                            }
                        }
                    }
                    m_mesh_terrain_kerava = lmesh.compile();
                }
#if defined(USE_LD)
                addPreviewMesh("terrain_kerava", *m_mesh_terrain_kerava);
#endif
            }

            // Kerava station world.
            IntroWorld& world = m_world[3];
            vgl::GlslProgram& prog = getProgramOffscreen();

            world.addEntity(prog, getMeshTerrainKerava(), 0, 0, 0);

            {
                const int RAILS_IX = static_cast<int>(RAILS_X * 10.0f);
                const int PYLON_IX = static_cast<int>(PYLON_X * 10.0f);
                const int RAILS1_OFFSET = -25;
                const int PYLON1_OFFSET = RAILS1_OFFSET - static_cast<int>(RAILS_OFFSET * 10.0f);
                const int ARC_OFFSET = -310 - RAILS1_OFFSET + PYLON_IX;
                const int RAILS2_OFFSET = ARC_OFFSET - RAILS_IX;
                const int RAILS3_OFFSET = ARC_OFFSET + RAILS_IX;
                const int FENCE_OFFSET = ((ARC_OFFSET + RAILS_IX) + RAILS1_OFFSET) / 2;
                const int FENCE_LENGTH = 50;

                const int PYLON_LENGTH = 550;
                const int RAILS_LENGTH = 100;

                for(int zpos = 2000; (zpos > -2000); zpos -= RAILS_LENGTH)
                {
                    world.addEntity(prog, getMeshRails(), RAILS1_OFFSET, 5, zpos);
                    world.addEntity(prog, getMeshRails(), RAILS2_OFFSET, 5, zpos);
                    world.addEntity(prog, getMeshRails(), RAILS3_OFFSET, 5, zpos);
                }

                for(int zpos = 2000; (zpos > -2000); zpos -= PYLON_LENGTH)
                {
                    world.addEntity(prog, getMeshPylon(), PYLON1_OFFSET, 5, zpos);
                    world.addEntity(prog, getMeshTendons(), PYLON1_OFFSET, 5, zpos);
                    world.addEntity(prog, getMeshArc(), ARC_OFFSET, 5, zpos);
                    world.addEntity(prog, getMeshTendons(), ARC_OFFSET - PYLON_IX, 5, zpos);
                    world.addEntity(prog, getMeshTendons(), ARC_OFFSET + PYLON_IX, 5, zpos, 0, 128, 0);
                }

                for(int zpos = 2000; (zpos > -2000); zpos -= FENCE_LENGTH)
                {
                    world.addEntity(prog, getMeshFence(), FENCE_OFFSET, 0, zpos);
                }
            }

            world.addEntity(prog, getMeshKatos(), -330, 20, -320);
            world.addEntity(prog, getMeshKatos(), -330, 20, -60);
            world.addEntity(prog, getMeshKatos(), -330, 20, 200);
            world.addEntity(prog, getMeshKatos(), -330, 20, 360);

            world.addEntity(prog, getMeshTower(), -346, 20, -915);
            world.addEntity(prog, getMeshRamp(), -396, 16, -938);
            world.addEntity(prog, getMeshKeravaStation(), 174, 20, 0);

            world.addEntity(prog, getMeshLamppost(), 115, 20, 850, 0, 128, 0);
            world.addEntity(prog, getMeshLamppost(), 115, 20, 600, 0, 128, 0);
            world.addEntity(prog, getMeshLamppost(), 115, 20, 350, 0, 128, 0);
            world.addEntity(prog, getMeshLamppost(), 115, 20, -350, 0, 128, 0);
            world.addEntity(prog, getMeshLamppost(), 115, 20, -600, 0, 128, 0);
            world.addEntity(prog, getMeshLamppost(), 115, 20, -850, 0, 128, 0);

            world.addEntity(prog, getMeshLamppost(), -480, 20, 800, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, 550, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, 300, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, 50, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, -200, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, -450, 0, 0, 0);
            world.addEntity(prog, getMeshLamppost(), -480, 20, -700, 0, 0, 0);

            lay_bridge(world, prog, vgl::vec3(2.0f, 2.0f, -100.0f), vgl::vec3(1.0f, 0.0f, 0.0f), -3, 3);

            world.sort();
        }

        // World 4 (Sirkus hevo set).
#if defined(USE_LD)
        if(g_preview_mesh.empty() || g_preview_mesh.starts_with("terrain") || g_preview_mesh.starts_with("world"))
#endif
        {
            // Sirkus hevo set terrain.
            {
                const int16_t SIRKUS_HEVO_SET_RADIUS1 = 1200;
                const int16_t SIRKUS_HEVO_SET_RADIUS2 = 130;
                const int16_t SIRKUS_HEVO_SET_RADIUS3 = 14;
                const int16_t SIRKUS_HEVO_SET_H1 = 120;
                const int16_t SIRKUS_HEVO_SET_H2 = 280;
                const int16_t SIRKUS_HEVO_SET_H3 = 390;
                const int16_t SIRKUS_HEVO_SET_H4 = 760;
                const int16_t SIRKUS_HEVO_SET_TOFFSET = 210;

                vgl::vector<int16_t> data;

                data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                data.push_back(0);
                data.push_back(0);
                data.push_back(0);
                data.push_back(0);
                data.push_back(SIRKUS_HEVO_SET_H1);
                data.push_back(0);
                data.push_back(3);
                data.push_back(27);
                data.push_back(SIRKUS_HEVO_SET_RADIUS1);
                data.push_back(vgl::CSG_NO_FRONT);

                auto generate_hevos_stand = [&data, SIRKUS_HEVO_SET_H1, SIRKUS_HEVO_SET_H4, SIRKUS_HEVO_SET_RADIUS2,
                     SIRKUS_HEVO_SET_RADIUS3]
                         (int16_t px, int16_t pz, int16_t height, bool lr)
                {
                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(px);
                    data.push_back(SIRKUS_HEVO_SET_H1);
                    data.push_back(pz);
                    data.push_back(px);
                    data.push_back(height);
                    data.push_back(pz);
                    data.push_back(3);
                    data.push_back(11);
                    data.push_back(SIRKUS_HEVO_SET_RADIUS2);
                    data.push_back(vgl::CSG_NO_FRONT);

                    int16_t px2 = static_cast<int16_t>(px + (lr ? SIRKUS_HEVO_SET_TOFFSET : -SIRKUS_HEVO_SET_TOFFSET));

                    data.push_back(to_int16(vgl::CsgCommand::CYLINDER));
                    data.push_back(px2);
                    data.push_back(SIRKUS_HEVO_SET_H1);
                    data.push_back(pz);
                    data.push_back(px2);
                    data.push_back(SIRKUS_HEVO_SET_H4);
                    data.push_back(pz);
                    data.push_back(3);
                    data.push_back(7);
                    data.push_back(SIRKUS_HEVO_SET_RADIUS3);
                    data.push_back(vgl::CSG_NO_FRONT);
                };

                generate_hevos_stand(460, 380, SIRKUS_HEVO_SET_H2, true);
                generate_hevos_stand(320, -480, SIRKUS_HEVO_SET_H2, true);
                generate_hevos_stand(-320, -320, SIRKUS_HEVO_SET_H2, false);
                generate_hevos_stand(-490, 250, SIRKUS_HEVO_SET_H2, false);
                generate_hevos_stand(-90, 0, SIRKUS_HEVO_SET_H3, true);

                data.push_back(to_int16(vgl::CsgCommand::NONE));
                {
                    vgl::LogicalMesh lmesh(data.data());
                    m_mesh_terrain_sirkus_hevo_set = lmesh.compile();
                }
#if defined(USE_LD)
                addPreviewMesh("terrain_sirkus_hevo_set", *m_mesh_terrain_sirkus_hevo_set);
#endif
            }

            // Sirkus hevo set world.
            IntroWorld& world = m_world[4];
            vgl::GlslProgram& prog = getProgramOffscreen();

            world.addEntity(prog, getMeshTerrainSirkusHevoSet(), 0, 0, 0);

            world.addEntity(prog, getMeshSirkusHevoNen(), 44, 68, 38, -64, 64, 0);
            world.addEntity(prog, getMeshSirkusHevoNen(), 26, 68, -48, -64, 64, 0);
            world.addEntity(prog, getMeshSirkusHevoNen(), -31, 68, -32, -64, -64, 0);
            world.addEntity(prog, getMeshSirkusHevoNen(), -47, 68, 25, -64, -64, 0);
            world.addEntity(prog, getMeshSirkusHevoNen(), -11, 79, 0, -64, 64, 0);

            world.sort();
        }
    }

public:
    /// Accessor.
    ///
    /// \return Framebuffer.
    vgl::FrameBuffer& getFbo()
    {
        return *m_fbo;
    }
    /// Accessor.
    ///
    /// \return Texture.
    vgl::Texture2D& getTextureStipple()
    {
        return *m_texture_stipple;
    }

    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramFont()
    {
        return m_program_font;
    }
    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramFontOverlay()
    {
        return m_program_font_overlay;
    }
    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramOffscreen()
    {
        return m_program_offscreen;
    }
    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramSkeleton()
    {
        return m_program_skeleton;
    }
    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramVisualization()
    {
        return m_program_visualization;
    }
    /// Accessor.
    ///
    /// \return Program.
    vgl::GlslProgram& getProgramPost()
    {
        return m_program_post;
    }

    /// Accessor.
    ///
    /// \return Font.
    vgl::Font& getFont()
    {
        return *m_font;
    }

    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshQuad()
    {
        return *m_mesh_quad;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    const GlyphMeshArray& getGlyph()
    {
        return m_glyph;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshFence()
    {
        return *m_mesh_fence;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshPylon()
    {
        return *m_mesh_pylon;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshPylonExtraBase()
    {
        return *m_mesh_pylon_extra_base;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshArc()
    {
        return *m_mesh_arc;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshTendons()
    {
        return *m_mesh_tendons;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshLamppost()
    {
        return *m_mesh_lamppost;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshBridge()
    {
        return *m_mesh_bridge;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshRails()
    {
        return *m_mesh_rails;
    }

    /// Accessor.
    ///
    /// \param idx Visualization mesh index.
    /// \return Mesh.
    vgl::Mesh& getMeshVisualization(unsigned idx)
    {
        return *m_mesh_visualization[idx];
    }
    /// Accessor.
    ///
    /// \param idx FFT data index.
    /// \return Pointer to FFT data.
    double* getDataFFT(unsigned idx)
    {
        vgl::vector<double>& farray = m_data_fft[idx];
        farray.resize(VISUALIZATION_ELEMENTS);
        return farray.data();
    }

#if defined(ENABLE_CHARTS) && ENABLE_CHARTS
    /// Accessor.
    ///
    /// \param idx Chart mesh index.
    /// \return Mesh.
    vgl::Mesh& getMeshChart(unsigned idx)
    {
        return *m_mesh_chart[idx];
    }
#endif

    /// Accessor.
    ///
    /// \param frame_idx Frame index.
    /// \param idx Index of FFT data.
    /// \return Volume level array.
    float getDataLevels(int frame_idx, unsigned idx)
    {
        VGL_ASSERT(frame_idx > 0);
        VGL_ASSERT(frame_idx < INTRO_LENGTH);
        VGL_ASSERT(idx < 3);
        return m_data_levels[static_cast<unsigned>(frame_idx * 3) + idx];
    }

    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshSm5Interior()
    {
        return *m_mesh_sm5_interior;
    }
    /// Accessor.
    ///
    /// \param op Chair index.
    /// \return Mesh.
    vgl::Mesh& getMeshSm5Chair(unsigned op)
    {
        return *m_mesh_sm5_chair[op];
    }
    /// Accessor.
    ///
    /// \param op Train index.
    /// \return Mesh.
    vgl::Mesh& getMeshTrain(unsigned op)
    {
        return *m_mesh_train[op];
    }

    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshKatos()
    {
        return *m_mesh_katos;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshTower()
    {
        return *m_mesh_tower;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshRamp()
    {
        return *m_mesh_ramp;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshStationBuilding()
    {
        return *m_mesh_station_building;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshKeravaStation()
    {
        return *m_mesh_kerava_station;
    }

    /// Accessor.
    ///
    /// \param op Terrain index.
    /// \return Mesh.
    vgl::Mesh& getMeshBuilding(int op)
    {
        return *m_mesh_building[vgl::min(op, BUILDING_COUNT - 1)];
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshKeravaStateBuilding()
    {
        return *m_mesh_kerava_state_building;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshBurjKerava()
    {
        return *m_mesh_burj_kerava;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshJohnKeravaCenter()
    {
        return *m_mesh_john_kerava_center;
    }
    /// Accessor.
    ///
    /// \return Mesh.
    vgl::Mesh& getMeshKeravanasTowers()
    {
        return *m_mesh_keravanas_towers;
    }

    /// Accessor.
    ///
    /// \param op Sign index.
    /// \return Mesh.
    vgl::Mesh& getMeshSign(unsigned op)
    {
        return *m_mesh_sign[op];
    }

    /// Accessor.
    ///
    /// Since the terrain loops, the index wraps around to the beginning.
    ///
    /// \param op Terrain index.
    /// \return Mesh.
    vgl::Mesh& getMeshTerrain(int op)
    {
        int idx = vgl::congr(op, TERRAIN_BLOCK_COUNT);
        return *m_mesh_terrain[idx];
    }
    /// Accessor.
    ///
    /// \param op Terrain index.
    vgl::Mesh& getMeshTerrainKerava()
    {
        return *m_mesh_terrain_kerava;
    }
    /// Accessor.
    ///
    /// \param op Terrain index.
    vgl::Mesh& getMeshTerrainSirkusHevoSet()
    {
        return *m_mesh_terrain_sirkus_hevo_set;
    }

    /// Accessor.
    ///
    /// \return Figure mesh.
    vgl::Mesh& getMeshUkko()
    {
        return *m_mesh_ukko;
    }
    /// Accessor.
    ///
    /// \return Figure animation.
    vgl::Animation& getAnimationUkko(unsigned op)
    {
        return *m_animation_ukko[op];
    }
    /// Accessor.
    ///
    /// \return Figure mesh.
    vgl::Mesh& getMeshSirkusHevoNen()
    {
        return *m_mesh_sirkus_hevo_nen;
    }

    /// Accessor.
    ///
    /// \param op Index to access with.
    /// \return World by index.
    IntroWorld& getWorld(unsigned op)
    {
        return m_world[op];
    }

    /// Accessor.
    ///
    /// \param op Frame index.
    /// \return Sign easing at given index.
    const SignEasing& getSignEasing(unsigned op)
    {
        return m_sign_easing[op];
    }
    /// Accessor.
    ///
    /// \param op Frame index.
    /// \return Camera jitter at given frame.
    const vgl::vec3& getCameraJitter(unsigned op)
    {
        return m_camera_jitter[op];
    }

    /// Evaluate FFT for a single frame.
    ///
    /// The result of the evaluation is stored in the internal FFT buffer of the
    void evaluateFFT(int frame_number)
    {
        const void* audio_buffer = reinterpret_cast<void*>(g_audio_buffer + generate_audio_position(frame_number));
        const float* input = reinterpret_cast<const float*>(audio_buffer);

        // Allocate data for FFTW operations.
        double* fftw_in = getDataFFT(0);
        double* fftw_out = getDataFFT(1);
        for(unsigned ii = 0; (ii < IntroData::VISUALIZATION_ELEMENTS); ++ii)
        {
            // Left channel only.
            fftw_in[ii] = static_cast<double>(input[ii * 2]);
        }

        // Just select a sub-optimal plan and execute.
        fftw_plan r2r_plan = dnload_fftw_plan_r2r_1d(static_cast<int>(IntroData::VISUALIZATION_ELEMENTS), fftw_in, fftw_out,
            FFTW_R2HC, FFTW_ESTIMATE);
        dnload_fftw_execute(r2r_plan);
        dnload_fftw_destroy_plan(r2r_plan);
    }

    /// Initialize all data.
    void initialize()
    {
        // Some GPU data needs to be initialized in the main thread immediately.
        {
            vgl::task_wait_main(taskfunc_graphics_immediate, this);
        }

#if defined(USE_LD)
        // If developer mode is on, load audio instead of generating it.
        if (g_flag_developer)
        {
            vgl::Fence audio_fence = vgl::task_wait(taskfunc_audio_load, this);
            initializeGraphics();
        }
        else
#endif
        {
            vgl::Fence audio_fence = vgl::task_wait(taskfunc_audio_generate, this);
            initializeGraphics();
        }

        // All other tasks complete, signal main thread about being ready.
        vgl::task_dispatch_main(taskfunc_ready, this);
    }

#if defined(USE_LD)
    /// Add a preview mesh.
    ///
    /// \param name Name of the mesh.
    /// \param mesh Mesh.
    void addPreviewMesh(vgl::string_view name, vgl::Mesh& mesh)
    {
        mesh.setName(name);
        m_preview_meshes.push_back(&mesh);
    }

    /// Gets a mesh by name.
    /// \param name Name to search.
    /// \return Mesh found or nullptr.
    const vgl::Mesh* getMeshByName(vgl::string_view name) const
    {
        for(const auto& ii : m_preview_meshes)
        {
            if(ii->getName() == name)
            {
                return ii;
            }
        }
        return nullptr;
    }

    /// Recreate shaders.
    void recreateShaders()
    {
        if(!m_program_font.relink() || !m_program_font_overlay.relink() || !m_program_offscreen.relink() ||
                !m_program_skeleton.relink() || !m_program_visualization.relink() || !m_program_post.relink())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("program recreation failure"));
        }
    }
#endif

public:
    /// Task function for initializing intro data.
    ///
    /// \param op Intro data passed as pointer.
    /// \return nullptr
    static void* taskfunc_initialize(void* op)
    {
        IntroData* data = static_cast<IntroData*>(op);
        data->initialize();
        return nullptr;
    }

    /// Task function for signalling intro data is ready.
    ///
    /// \return Always nullptr.
    static void* taskfunc_ready(void*)
    {
        return nullptr;
    }

private:
    /// Function for generating audio.
    ///
    /// \param op Intro data passed as pointer.
    /// \return nullptr
    static void* taskfunc_audio_generate(void* op)
    {
        IntroData* data = static_cast<IntroData*>(op);
        data->initializeAudioGenerate();
        return nullptr;
    }

#if defined(USE_LD)
    /// Function for generating audio.
    ///
    /// \param op Intro data passed as pointer.
    /// \return nullptr
    static void* taskfunc_audio_load(void* op)
    {
        IntroData* data = static_cast<IntroData*>(op);
        data->initializeAudioLoad();
        return nullptr;
    }

    /// FLAC error callback.
    ///
    /// \param decoder Decoder.
    /// \param status Error status.
    /// \param client_data User-defined data.
    static void flac_error_callback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(std::string("FLAC error callback: ") +
                    FLAC__StreamDecoderErrorStatusString[status]));
        (void)decoder;
        (void)client_data;
    }

    /// FLAC metadata callback.
    ///
    /// \param decoder Decoder.
    /// \param metadata Stream metadata.
    /// \param client_data User-defined data.
    static void flac_metadata_callback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata,
            void* client_data)
    {
        static unsigned channels = metadata->data.stream_info.channels;
        static unsigned sample_rate = metadata->data.stream_info.sample_rate;
        static unsigned bps = metadata->data.stream_info.bits_per_sample;
        if((channels != 2) || (sample_rate != 44100) || (bps != 16))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("incompatible FLAC stream: " + std::to_string(channels) + "ch " +
                        std::to_string(sample_rate) + "Hz " + std::to_string(bps) + "bps"));
        }
        (void)decoder;
        (void)client_data;
    }

    /// FLAC write callback.
    ///
    /// \param decoder Decoder.
    /// \param frame Decoded frame.
    /// \param buffer Audio data.
    /// \param client_data User-defined data.
    /// \return Write status.
    static FLAC__StreamDecoderWriteStatus flac_write_callback(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame,
            const FLAC__int32* const buffer[], void* client_data)
    {
        FlacWriteStatus* write_status = static_cast<FlacWriteStatus*>(client_data);

        for(unsigned ii = 0; (ii < frame->header.blocksize); ++ii)
        {
            int16_t lt = static_cast<int16_t>(buffer[0][ii]);
            int16_t rt = static_cast<int16_t>(buffer[1][ii]);
            float flt = (static_cast<float>(lt) + 32768.0f) / 65535.0f * 2.0f - 1.0f;
            float frt = (static_cast<float>(rt) + 32768.0f) / 65535.0f * 2.0f - 1.0f;

            write_status->write(flt);
            write_status->write(frt);
        }

        (void)decoder;
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }
#endif

    /// Function for compiling shaders.
    ///
    /// \param op Intro data passed as pointer.
    /// \return nullptr
    static void* taskfunc_graphics_immediate(void* op)
    {
        IntroData* data = static_cast<IntroData*>(op);
        data->initializeGraphicsImmediate();
        return nullptr;
    }
};

/// Intro data instance.
static IntroData g_data;

#endif
