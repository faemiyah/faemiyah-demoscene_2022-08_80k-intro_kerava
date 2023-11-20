#ifndef INTRO_STATE
#define INTRO_STATE

#if defined(USE_LD)
/// Position saved from direction data.
static vgl::vec3 g_direction_pos;
/// Direction saved from direction data.
static vgl::vec3 g_direction_fw;
/// Up saved from direction data.
static vgl::vec3 g_direction_up;
/// Saved FOV data.
static float g_direction_fov = 0.0f;
/// Visual debug mode toggle to prevent setting it every frame.
static int g_last_visual_debug = -1;
#endif

/// Generate stipple settings.
///
/// \return Stipple quad settings.
#if !defined(USE_LD)
constexpr
#endif
vgl::vec4 get_stipple_settings()
{
    float fwidth = static_cast<float>(g_screen_w);
    float fheight = static_cast<float>(g_screen_h);
    float iwidth = 1.0f / fwidth;
    float iheight = 1.0f / fheight;
    // 1:1 pixels when below 640p. On greater resolutions potentially multiply.
    int multiply = vgl::max(vgl::iround(fwidth / 640.0f), 1);
    float xmul = fwidth / static_cast<float>(static_cast<int>(IntroData::STIPPLE_SIZE) * multiply);
    float ymul = fheight / static_cast<float>(static_cast<int>(IntroData::STIPPLE_SIZE) * multiply);
    return vgl::vec4(iwidth, iheight, xmul, ymul);
}

/// Crash object settings.
///
/// Used in the final train crash w. greets.
struct CrashObject
{
    /// Trajectory data.
    ///
    /// Elements:
    /// 0-2: Position at start.
    /// 3-5: Position at end.
    /// 6-8: Rotation at start.
    /// 9-11: Rotation at end.
    /// 12: Train index, 1 = locomotive, 2 = carriage. 0 for none.
    int16_t m_data[13];
};

/// Individual crash objects.
const CrashObject g_crash_objects[] =
{
    { { -25, 7, 355, -25, 40, 361, 0, 0, 0, 18, 0, 12, 1, } },
    { { -25, 7, 157, -25, 40, 186, 0, 0, 0, -18, 0, 6, 2, } },
    { { -25, 7, -41, -25, 44, -19, 0, 0, 0, 22, 0, -8, 2, } },
    { { -25, 7, -239, -25, 44, -196, 0, 0, 0, -20, 0, -6, 2, } },
    { { -25, 7, -437, -25, 39, -410, 0, 0, 0, 16, 0, 9, 2, } },
    { { -25, 7, -635, -25, 39, -606, 0, 0, 0, -16, 0, -2, 2, } },
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
};

/// Real-world text position struct.
///
/// Used in greets, credits and some shitposting.
struct TextPosition
{
    /// Trajectory data.
    ///
    /// Elements:
    /// 0-2: Position at start.
    /// 3-5: Position at end.
    /// 6-8: Rotation at start.
    /// 9-11: Rotation at end.
    /// 12: Start time.
    /// 13: End time.
    /// 14: Font size.
    int16_t m_data[15];

    /// Text.
    const char* m_text;
};

/// Individual text positions.
TextPosition g_text_positions[] =
{
    { { 10, 41, 410, 10, 47, 410, 0, 64, 10, 0, 64, 12, 5270, 5340, 222, }, "Adapt" },
    { { 10, 38, 380, 10, 44, 380, 0, 64, 11, 0, 64, 13, 5275, 5345, 222, }, "Blobtrox" },
    { { 10, 52, 370, 10, 59, 370, 0, 64, 11, 0, 64, 13, 5280, 5350, 222, }, "Ctrl-Alt-Test" },
    { { 10, 71, 275, 10, 79, 275, 0, 64, -13, 0, 64, -14, 5285, 5355, 222, }, "Epoch" },
    { { 10, 55, 270, 10, 63, 270, 0, 64, -13, 0, 64, -14, 5290, 5360, 222, }, "Fit" },
    { { 10, 54, 250, 10, 62, 250, 0, 64, -12, 0, 64, -13, 5295, 5365, 222, }, "G-INcorporated" },
    { { 10, 53, 205, 10, 60, 205, 0, 64, -12, 0, 64, -13, 5300, 5370, 222, }, "Hedelmae" },
    { { 10, 28, 50, 10, 31, 50, 0, 64, 14, 0, 64, 16, 5380, 5450, 222, }, "Ivory Labs" },
    { { 10, 44, 45, 10, 47, 45, 0, 64, 14, 0, 64, 16, 5385, 5455, 222, }, "Lonely Coders" },
    { { 10, 49, -5, 10, 54, -5, 0, 64, 15, 0, 64, 17, 5390, 5460, 222, }, "Luminaire" },
    { { 10, 69, -30, 10, 76, -30, 0, 64, 15, 0, 64, 17, 5395, 5465, 222, }, "Paraguay" },
    { { 10, 92, -110, 10, 100, -110, 0, 64, -15, 0, 64, -17, 5400, 5470, 233, }, "Peisik" },
    { { 10, 80, -110, 10, 87, -110, 0, 64, -15, 0, 64, -17, 5405, 5475, 233, }, "Royal Elite Ninjas" },
    { { 10, 78, -150, 10, 85, -150, 0, 64, -15, 0, 64, -17, 5410, 5480, 233, }, "The Old Dude" },
    { { 10, 48, -200, 10, 53, -200, 0, 64, -15, 0, 64, -17, 5415, 5485, 233, }, "TDA" },
    { { 10, 44, -320, 10, 47, -320, 0, 64, 16, 0, 64, 17, 5490, 5560, 222, }, "Tekotuotanto" },
    { { 10, 45, -360, 10, 48, -360, 0, 64, 16, 0, 64, 17, 5495, 5565, 222, }, "Traction" },
    { { 10, 87, -510, 10, 94, -510, 0, 64, -14, 0, 64, -15, 5500, 5570, 255, }, "...and you" },
    { { -31, 17, -138, -31, 17, -146, -11, 0, 0, -8, 0, 0, 5620, 5700, 77, }, "collateralburger" },
    { { -19, 11, -138, -19, 11, -146, -11, 0, 0, -8, 0, 0, 5650, 5730, 77, }, "Trilkk" },
    { { -15, 5, -138, -15, 5, -146, -11, 0, 0, -8, 0, 0, 5670, 5750, 77, }, "Warma" },
    { { -23, 17, -132, -23, 17, -140, -12, 0, 0, -9, 0, 0, 5780, 5860, 77, }, "Odroid XU4Q" },
    { { -21, 11, -132, -21, 11, -140, -12, 0, 0, -9, 0, 0, 5800, 5880, 77, }, "Linux-arm32l" },
    { { -14, 5, -132, -14, 5, -140, -12, 0, 0, -9, 0, 0, 5820, 5900, 77, }, "75k intro" },
    { { -14, 13, -148, -13, 13, -154, -12, 0, 0, -9, 0, 0, 5930, 6020, 88, }, "Faemiyah" },
    { { -11, 7, -148, -10, 7, -154, -12, 0, 0, -9, 0, 0, 5950, 6040, 77, }, "2022-08" },
    { { -5, 5, -140, -5, 5, -145, -14, 0, 0, -13, 0, 0, 6080, 6150, 55, }, "1524mm" },
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, nullptr },
};

/// Screen-space text position struct.
///
/// Used in the idiotic blurbs in the intro.
struct TextOverlay
{
    /// Text position data.
    ///
    /// Elements:
    /// - X, Y (x100)
    /// - random seed (-1 for frame index)
    /// - random X, random Y
    /// - Font size (x100)
    /// - Timestamp, duration
    int16_t m_data[8];

    /// Sign text.
    const char* m_text;
};

/// Individual text overlays.
const TextOverlay g_text_overlays[] =
{
    // Inside the train -overlays.
    { { -176, -38, 3, 120, 60, 108, 402, 8, }, "Malmi" },
    { { -176, -38, 6, 120, 60, 108, 410, 4, }, "Malmi" },
    { { -88, -19, 2, 40, 20, 54, 414, 4, }, "Malmi" },
    { { -308, -38, 6, 120, 60, 108, 538, 8, }, "Tapanila" },
    { { -308, -38, 4, 120, 60, 108, 546, 4, }, "Tapanila" },
    { { -154, -19, 9, 40, 20, 54, 550, 4, }, "Tapanila" },
    { { -308, -38, 5, 120, 60, 108, 600, 8, }, "Puistola" },
    { { -308, -38, 11, 120, 60, 108, 608, 4, }, "Puistola" },
    { { -154, -19, 12, 40, 20, 54, 612, 4, }, "Puistola" },
    { { -357, -38, 14, 120, 60, 108, 688, 8, }, "Tikkurila" },
    { { -357, -38, 15, 120, 60, 108, 696, 4, }, "Tikkurila" },
    { { -176, -19, 18, 40, 20, 54, 700, 4, }, "Tikkurila" },
    { { -440, -38, 17, 120, 60, 108, 708, 8, }, "Hiekkaharju" },
    { { -440, -38, 13, 120, 60, 108, 716, 4, }, "Hiekkaharju" },
    { { -220, -19, 16, 40, 20, 54, 720, 4, }, "Hiekkaharju" },
    { { -220, -38, 21, 120, 60, 108, 782, 8, }, "Rekola" },
    { { -220, -38, 22, 120, 60, 108, 790, 4, }, "Rekola" },
    { { -110, -19, 23, 40, 20, 54, 794, 4, }, "Rekola" },
    { { -176, -38, 24, 120, 60, 108, 852, 4, }, "Korso" },
    { { -176, -38, 27, 120, 60, 108, 856, 4, }, "Korso" },
    { { -88, -19, 0, 0, 0, 54, 860, 20, }, "Korso" },

    // SIRKUS HEVO SET
    { { -90, 22, -1, 2, 2, 34, 2170, 70, }, "SIRKUS" },
    { { -10, -13, -1, 2, 2, 34, 2190, 50, }, "HEVO" },
    { { -60, -49, -1, 2, 2, 34, 2210, 30, }, "SET" },

    // Kerava State Building.
    { { -110, 40, -1, 4, 4, 39, 3050, 15, }, "Kerava" },
    { { -95, 15, -1, 4, 4, 39, 3050, 15, }, "State" },
    { { -130, -10, -1, 4, 4, 39, 3050, 15, }, "Building" },
    { { -100, 38, -1, 3, 3, 31, 3065, 15, }, "Kerava" },
    { { -85, 15, -1, 3, 3, 31, 3065, 15, }, "State" },
    { { -120, -8, -1, 3, 3, 31, 3065, 15, }, "Building" },
    { { -90, 34, -1, 2, 2, 22, 3080, 45, }, "Kerava" },
    { { -80, 15, -1, 2, 2, 22, 3080, 45, }, "State" },
    { { -95, -4, -1, 2, 2, 22, 3080, 45, }, "Building" },

    // John Kerava Center.
    { { -115, 35, -1, 4, 4, 42, 3210, 10, }, "John" },
    { { -130, 3, -1, 4, 4, 42, 3210, 10, }, "Kerava" },
    { { -115, -30, -1, 4, 4, 42, 3210, 10, }, "Center" },
    { { -100, 30, -1, 3, 3, 34, 3220, 15, }, "John" },
    { { -110, 5, -1, 3, 3, 34, 3220, 15, }, "Kerava" },
    { { -105, -20, -1, 3, 3, 34, 3220, 15, }, "Center" },
    { { -85, 28, -1, 2, 2, 22, 3235, 45, }, "John" },
    { { -95, 10, -1, 2, 2, 22, 3235, 45, }, "Kerava" },
    { { -90, -8, -1, 2, 2, 22, 3235, 45, }, "Center" },

    // Keravanas Towers.
    { { -150, 35, -1, 4, 4, 42, 3400, 20, }, "Keravanas" },
    { { -80, -50, -1, 4, 4, 42, 3415, 20, }, "Towers" },
    { { -105, 26, -1, 3, 3, 33, 3440, 20, }, "Keravanas" },
    { { -96, -4, -1, 3, 3, 33, 3440, 20, }, "Towers" },
    { { -95, 25, -1, 2, 2, 22, 3465, 60, }, "Keravanas" },
    { { -90, 2, -1, 2, 2, 22, 3465, 60, }, "Towers" },

    // Burj Kerava.
    { { -90, 25, -1, 4, 4, 44, 4180, 20, }, "Burj" },
    { { -120, -40, -1, 4, 4, 44, 4195, 20, }, "Kerava" },
    { { -90, 28, -1, 3, 3, 33, 4230, 20, }, "Burj" },
    { { -98, -4, -1, 3, 3, 33, 4230, 20, }, "Kerava" },
    { { -85, 25, -1, 2, 2, 22, 4250, 60, }, "Burj" },
    { { -95, 0, -1, 2, 2, 22, 4250, 60, }, "Kerava" },

    { { 0, 0, 0, 0, 0, 0, }, nullptr },
};

/// Train movement struct.
struct TrainPosition
{
    /// Train movement data.
    ///
    /// 0: X Offset (x10)
    /// 1: Y Offset (x10)
    /// 2: Z start position (x10).
    /// 3: Z end position (x10).
    /// 4: Carriage count.
    /// 5: Start time.
    /// 6: Duration.
    int16_t m_data[7];
};

// Individual train movements.
const TrainPosition g_train_movements[] =
{
    // Vastaantuleva juna.
    { { -36, 7, -10400, -8000, 4, 1940, 130 } },

    // Varokaa ohittavaa junaa.
    { { -25, 7, -3000, 3000, 8, 2450, 130 } },

    // First passing train.
    { { -25, 7, 3000, -3000, 8, 2520, 170 } },

    // Two subsequent passing trains.
    { { -242, 7, -3000, 3000, 8, 2680, 170 } },
    { { -170, 7, 3000, -3000, 8, 2730, 170 } },

    // All around.
    { { -242, 7, -3000, 3000, 8, 2820, 170 } },

    // Approaching ukko.
    { { -25, 7, -1300, 320, 8, 5000, 80 } },
    { { -25, 7, 320, 330, 8, 5080, 50 } },

    // nullptr
    { { 0, 0, 0, 0, 0, 0, } },
};

/// Ukko positions and animation data.
struct UkkoPosition
{
    /// Ukko position data.
    ///
    /// Positions and animation offset and speed are multiplied by 10.
    ///
    /// 0-2: Position in 3D space at start.
    /// 3-5: Rotation at start.
    /// 6-8: Position in 3D space at end.
    /// 9-11: Rotation at end.
    /// 12: Animation index.
    /// 13: Animation offset.
    /// 14: Animation speed multiplier.
    /// 15: Start time.
    /// 16: End time.
    int16_t m_data[17];
};

/// Ukko timing constant helper.
const int16_t UKKO_TIME1 = 4310;
/// Ukko timing constant helper.
const int16_t UKKO_DURATION1 = 399;
/// Ukko timing constant helper.
const int16_t UKKO_TIME2 = static_cast<int16_t>(UKKO_TIME1 + UKKO_DURATION1 + 1);
/// Ukko timing constant helper.
const int16_t UKKO_DURATION2 = 190;
/// Ukko timing constant helper.
const int16_t UKKO_TIME3 = static_cast<int16_t>(UKKO_TIME2 + UKKO_DURATION2 + 1);
/// Ukko timing constant helper.
const int16_t UKKO_DURATION3 = 180;
/// Ukko timing constant helper.
const int16_t UKKO_TIME4 = static_cast<int16_t>(UKKO_TIME3 + UKKO_DURATION3 + 1);
/// Ukko timing constant helper.
const int16_t UKKO_DURATION4 = 510;
/// Ukko timing constant helper.
const int16_t UKKO_ZPOS = 460;

/// Individual ukko positions.
const UkkoPosition g_ukko_positions[] =
{
    // Swing.
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 0, 30, 130, UKKO_TIME1, UKKO_DURATION1 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 1, 30, 130, UKKO_TIME1, UKKO_DURATION1 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 2, 30, 130, UKKO_TIME1, UKKO_DURATION1 } },

    // Throw.
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 5, 20, UKKO_ZPOS, -64, -64, 0, 3, 0, 10, UKKO_TIME2, UKKO_DURATION2 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 4, 0, 10, UKKO_TIME2, UKKO_DURATION2 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 5, 0, 10, UKKO_TIME2, UKKO_DURATION2 } },

    // Fly & decay.
    { { -14, 32, UKKO_ZPOS, 128, -20, 30, -38, 32, UKKO_ZPOS, 128, -60, 70, 6, 0, 14, UKKO_TIME3, UKKO_DURATION3 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 4, 10, -9, UKKO_TIME3, UKKO_DURATION3 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 5, 5, -4, UKKO_TIME3, UKKO_DURATION3 } },

    // Last slomo.
    { { -38, 32, UKKO_ZPOS, 128, -60, 70, -39, 32, UKKO_ZPOS, 128, -60, 70, 6, 14, 4, UKKO_TIME4, UKKO_DURATION4 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 4, 1, -1, UKKO_TIME4, UKKO_DURATION4 } },
    { { 11, 20, UKKO_ZPOS, -64, -64, 0, 11, 20, UKKO_ZPOS, -64, -64, 0, 5, 1, -1, UKKO_TIME4, UKKO_DURATION4 } },

    // nullptr
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};

/// Visualization positions and fade-in data.
struct VisualizationPosition
{
    /// Visualization positions.
    ///
    /// Positions are multiplied by 10.
    ///
    /// 0-2: Position.
    /// 3:   Rotation over Y-axis.
    /// 4:   Mirrored repeat count (l/r).
    /// 5-6: Fadein start and duration.
    /// 7-8: Fadeout start and duration.
    /// 9:   Color value (0-255).
    /// 10:  Visualization type (0: wave, 1: FFT).
    int16_t m_data[11];
};

/// Individual visualization positions.
///
/// For colors converted in post, fitting values:
/// 0:   No color.
/// 80:  Color 1.
/// 160: Color 2.
/// 255: Color 3.
const VisualizationPosition g_visualization_positions[] =
{
    // First passing train.
    { { -24, 70, 0, 64, 1, 2580, 20, 2670, 10, 80, 0} },

    // Subsequent passing trains.
    { { -242, 70, 0, 64, 1, 2730, 10, 2800, 10, 80, 0 } },
    { { -170, 70, 0, 64, 1, 2810, 10, 2870, 10, 80, 0 } },

    // Enable all.
    { { -24, 70, 0, 64, 1, 2870, 20, 3840, 40, 80, 0 } },
    { { -170, 70, 0, 64, 1, 2870, 20, 3840, 40, 80, 0 } },
    { { -242, 70, 0, 64, 1, 2870, 20, 3840, 40, 80, 0 } },

    // FFT on.
    { { -138, 137, -937, 0, 1, 3600, 60, 4150, 60, 160, 1 } },
    { { -138, 137, -1063, 0, 1, 3600, 60, 4150, 60, 160, 1 } },

    // Re-enable waves for throw.
    { { -24, 70, 0, 64, 1, 4090, 20, 4460, 40, 80, 0 } },
    { { -170, 70, 0, 64, 1, 4090, 20, 4460, 40, 80, 0 } },
    { { -242, 70, 0, 64, 1, 4090, 20, 4460, 40, 80, 0 } },

    // Crash all on.
    { { -24, 70, 0, 64, 1, 5140, 10, 5280, 170, 80, 0 } },
    { { -170, 70, 0, 64, 1, 5140, 10, 5280, 170, 80, 0 } },
    { { -242, 70, 0, 64, 1, 5140, 10, 5280, 170, 80, 0 } },
    { { -138, 137, -937, 0, 1, 5140, 10, 5280, 170, 160, 1 } },
    { { -138, 137, -1063, 0, 1, 5140, 10, 5280, 170, 160, 1 } },

    // nullptr
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};

/// Volume indicator level data.
///
/// Arranged as:
/// - Timestamp.
/// - Level multilier (0-255).
///
/// Between timestamps, the value is interpolated in a linear fashion.
const int16_t g_volume_indicator_data[] =
{
    0, 0,
    3800, 0,
    3880, 255,
    4340, 255,
    4420, 0,
    32767, 0,
};

/// Direction and camera path data.
///
///  Arranged as:
/// - Start position (3)
/// - End position (3)
/// - Start forward (3)
/// - End forward (3)
/// - Up (3)
/// - Start FOV.
/// - End FOV.
/// - Scene id.
/// - Scene length.
///
/// If scene length is negative, it's interpreted as positive, but other data is arranged as:
/// - Start angles (3)
/// - End angles (3)
/// - Start lookat (3)
/// - End lookat (3)
/// - Start radius (1)
/// - End radius (1)
/// - Unused (1)
/// - Start FOV.
/// - End FOV.
/// - Scene id.
/// - Scene length.
///
/// FOV values are Y fov where [0, 255] is mapped to [0.0, PI / 2].
///
/// While scene id does not change, the so-called time in-scene keeps updating.
static const int16_t g_direction_data[] =
{
    // Looking from back of train.
    1, 18, -88, 2, 19, -84, -13, -23, -97, -12, -12, -99, 0, 1, 0, 65, 65, 0, 260,

    // Train backside jump cuts.
    4, 18, -80, 5, 18, -70, -39, -3, -92, -55, -1, -83, 0, 1, 0, 65, 65, 0, 160,
    3, 19, -61, 3, 19, -58, -43, -3, -90, -43, -3, -90, 0, 1, 0, 65, 65, 0, 40,
    2, 19, -53, 2, 19, -49, -35, -4, -94, -35, -4, -94, 0, 1, 0, 65, 65, 0, 40,
    2, 18, -39, 2, 18, -34, -23, -1, -97, -23, -1, -97, 0, 1, 0, 65, 65, 0, 50,
    2, 15, -21, 2, 15, -16, -16, -1, -99, -16, -1, -99, 0, 1, 0, 65, 65, 0, 50,
    0, 14, 15, 0, 14, 30, 0, -3, -100, 0, -3, -100, 0, 1, 0, 65, 65, 0, 280,

    // First-person train ride.
    32, 32, 0, 32, 32, 0, 0, 0, -1, 0, 0, -1, 0, 1, 0, 65, 65, 1, 1290,

    // Sirkus hevo set.
    -8, 32, 0, -8, -34, 0, 0, 70, 0, 0, 70, 0, 270, 270, 0, 65, 65, 4, -40,
    -8, -34, 0, -8, 26, 0, 0, 70, 0, 0, 70, 0, 270, 270, 0, 65, 65, 4, -30,

    // Ratapiha look-around.
    -449, 41, 611, -432, 42, 613, 47, 1, -88, 47, 1, -88, 0, 1, 0, 65, 65, 3, 40,
    -360, 42, 648, -343, 42, 654, 40, 0, -92, 40, 0, -92, 0, 1, 0, 65, 65, 3, 40,
    -281, 42, 675, -265, 42, 681, 30, 1, -96, 30, 1, -96, 0, 1, 0, 65, 65, 3, 40,
    -178, 42, 703, -160, 42, 709, 19, 1, -98, 19, 1, -98, 0, 1, 0, 65, 65, 3, 40,
    -36, 47, 655, -21, 47, 660, 10, 2, -98, 10, 2, -98, 0, 1, 0, 65, 65, 3, 60,

    // Varokaa ohittavaa junaa.
    105, 46, 700, 97, 46, 665, -45, 0, -89, -45, 0, -89, 0, 1, 0, 65, 65, 3, 110,

    // Aallot ja junat ja low-level perseily.
    339, 171, 715, 357, 171, 687, -83, -21, -51, -86, -19, -46, 0, 1, 0, 65, 65, 3, 80,
    -587, 178, -845, -577, 178, -855, 77, -21, 60, 66, -19, 72, 0, 1, 0, 65, 65, 3, 320,

    // Kerava State Building.
    -196, 119, -8021, -195, 133, -8022, 54, 7, -84, 54, 7, -84, 0, 1, 0, 65, 65, 2, 60,
    -182, 239, -8042, -182, 246, -8042, 54, 17, -82, 54, 17, -82, 0, 1, 0, 65, 65, 2, 30,
    -890, 506, -8310, -888, 523, -8311, 86, 4, -51, 86, 4, -51, 0, 1, 0, 65, 65, 2, 70,

    // John Kerava Center.
    273, 124, -9331, 272, 145, -9332, -71, 10, -70, -71, 10, -70, 0, 1, 0, 65, 65, 2, 70,
    443, 471, -9076, 436, 509, -9084, -73, -8, -68, -73, -6, -68, 0, 1, 0, 65, 65, 2, 90,

    // Lisää aaltoja.
    -249, 157, 626, -262, 157, 612, 51, -17, -84, 56, -14, -82, 0, 1, 0, 65, 65, 3, 80,

    // Keravanas Towers.
    -31, 8, -10391, -31, 17, -10389, 1, 34, -94, 1, 34, -94, 0, 1, 0, 65, 65, 2, 40,
    -317, 347, -10327, -318, 362, -10327, 36, -9, -93, 36, -9, -93, 0, 1, 0, 65, 65, 2, 20,
    -667, 850, -10151, -679, 907, -10124, 41, -41, -81, 42, -36, -83, 0, 1, 0, 65, 65, 2, 100,

    // FFT fade in.
    -506, 171, 891, -497, 168, 857, 45, -8, -89, 45, -8, -89, 0, 1, 0, 65, 65, 3, 40,
    -496, 158, 722, -490, 156, 694, 45, -8, -89, 45, -8, -89, 0, 1, 0, 65, 65, 3, 40,
    258, 155, 54, 183, 157, 97, -55, -2, -83, -35, 0, -94, 0, 1, 0, 65, 65, 3, 250,

    // Juna volyymi-indikaattorit.
    -111, 40, -431, -111, 40, 200, 5, 12, -99, 2, 0, -99, 0, 98, 21, 108, 55, 3, 70,
    -442, 179, 99, -454, 176, 96, 49, -20, -85, 49, -20, -85, 34, 91, 22, 65, 72, 3, 70,
    21, 94, -439, 16, 90, -433, -66, -12, -74, -66, -12, -74, 0, 1, 0, 84, 79, 3, 70,
    -573, 364, -534, -593, 376, -547, 75, -59, -30, 75, -59, -30, 0, 1, 0, 65, 65, 3, 70,

    // Burj Kerava.
    -621, 80, -11215, -429, 74, -12241, -18, 13, -97, -28, 15, -95, 0, 1, 0, 65, 65, 2, 70,
    -243, 1035, -12721, -245, 1140, -12722, -40, -59, -70, -40, -50, -70, 0, 1, 0, 65, 65, 2, 100,

    // Ukko throwing.
    55, 48, 575, 93, 47, 522, -31, -11, -94, -77, -13, -63, 0, 1, 0, 65, 65, 3, 180,
    -96, 37, 332, -106, 37, 344, 67, -3, 74, 74, -1, 67, 0, 1, 0, 65, 65, 3, 160,
    -36, 61, 542, -51, 61, 531, 49, -30, -82, 55, -32, -77, 0, 1, 0, 65, 65, 3, 250,
    -18, 26, 515, -34, 25, 510, 28, 7, -96, 33, 8, -94, 0, 1, 0, 65, 65, 3, 180,
    -34, 25, 510, -35, 25, 510, 33, 8, -94, 34, 8, -94, 0, 1, 0, 65, 65, 3, 50,

    // Greets & train crash.
    107, 63, 589, 115, 66, 584, -45, -11, -89, -48, -13, -87, 0, 1, 0, 65, 65, 3, 130,
    196, 56, 467, 207, 56, 451, -79, -5, -60, -83, -5, -55, 0, 1, 0, 65, 65, 3, 110,
    211, 127, 127, 211, 130, 127, -75, -26, -61, -74, -26, -62, 0, 1, 0, 65, 65, 3, 110,
    229, 149, -267, 237, 148, -278, -77, -30, -56, -78, -30, -55, 0, 1, 0, 65, 65, 3, 110,

    // Inside the train -outro.
    5, 17, -82, 0, 16, -86, -32, -7, -95, -3, -12, -99, 0, 1, 0, 65, 65, 0, 610,

    // End scene lasts longer than the intro possibly can.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 65, 65, 0, 32767,
};

/// Direction and camera path override data.
///
///  Arranged as:
/// - Start position (3)
/// - End position (3)
/// - Start forward (3)
/// - End forward (3)
/// - Up (3)
/// - Start FOV.
/// - End FOV.
/// - Scene id.
/// - Scene start.
/// - Scene length.
///
/// FOV values are Y fov where [0, 255] is mapped to [0.0, PI / 2].
///
/// Does not update time in-scene. Replaces the direction at given time points.
static const int16_t g_direction_override_data[] =
{
    70, 71, 17, 56, 82, 12, -83, 52, -22, -83, 52, -22, 45, 89, 4, 65, 65, 4, 2690, 20,
    110, 80, 84, 105, 76, 80, -45, 22, -86, -43, 32, -84, 21, 98, 7, 65, 65, 4, 2770, 20,
    -133, 19, 57, -132, 25, 63, 63, 57, -52, 62, 52, -59, -75, 52, -40, 65, 65, 4, 3770, 10,
    -132, 25, 63, -131, 30, 69, 62, 52, -59, 60, 47, -65, -75, 52, -40, 65, 65, 4, 3790, 10,
    -136, 70, 77, -137, 71, 76, 74, -22, -64, 74, -22, -64, -59, -66, -47, 65, 65, 4, 3810, 5,
    -138, 72, 75, -139, 73, 74, 74, -22, -64, 74, -22, -64, -59, -66, -47, 65, 65, 4, 3820, 5,
    -8, 10, 56, -8, 10, 56, 65, 72, -25, 65, 72, -25, -75, 64, -16, 63, 61, 4, 3825, 2,
    -8, 10, 56, -8, 10, 56, 65, 72, -25, 65, 72, -25, -75, 64, -16, 58, 56, 4, 3830, 2,
    -8, 10, 56, -8, 10, 56, 65, 72, -25, 65, 72, -25, -75, 64, -16, 53, 50, 4, 3835, 2,
    -8, 10, 56, -8, 10, 56, 65, 72, -25, 65, 72, -25, -75, 64, -16, 47, 44, 4, 3840, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//-132, 25, 63;; fw: 62, 52, -59;; up: -75, 52, -40;; fov: 65
//-133, 19, 57 ;; fw: 63, 57, -52 ;; up: -75, 52, -40 ;; fov: 65
//-131, 30, 69;; fw: 60, 47, -65;; up: -77, 51, -38;; fov: 65

/// Landmark vemputus data.
///
/// Arranged into pairs of timestamps, each is the start and duration of vemputus.
static const int16_t g_landmark_data[] =
{
    3050, 80,
    3210, 80,
    3450, 80,
    4230, 80,
    0, 0,
};

/// Static landmark vemputus state.
static float g_landmark_vemputus = 1.0f;

/// Renderable intro state.
///
/// Static data here includes the direction.
///
/// Generated concurrently.
class IntroState
{
private:
    /// Render queue at this state.
    vgl::RenderQueue m_queue;

public:
    /// Constructor.
    constexpr explicit IntroState() noexcept = default;

public:
    /// Draw the intro.
    ///
    /// Must be executed on main thread.
    void draw() const
    {
        m_queue.draw();

#if defined(USE_LD)
        vgl::error_check("draw()");
#endif
    }

    /// Push crash objects into the render queue.
    ///
    /// \param ticks Current timestamp.
    void pushCrash(int ticks)
    {
        // All of the entries here have the same timescale.
        const int CRASH_START = 5131;
        const int CRASH_DURATION = 460;
        const int CRASH_END = CRASH_START + CRASH_DURATION;

        if((ticks < CRASH_START) || (ticks > CRASH_END))
        {
            return;
        }
        float ratio = static_cast<float>(ticks - CRASH_START) / static_cast<float>(CRASH_END - CRASH_START);

        // Adjust ratio for slomo.
        const float RATIO_FAST_TIME = 0.04f;
        const float RATIO_FAST_PHASE = 0.3f;
        if(ratio <= RATIO_FAST_TIME)
        {
            ratio = ratio * (RATIO_FAST_PHASE / RATIO_FAST_TIME);
        }
        else
        {
            ratio = (ratio - RATIO_FAST_TIME) / (1.0f - RATIO_FAST_TIME) * (1.0f - RATIO_FAST_PHASE) + RATIO_FAST_PHASE;
        }

        for(const CrashObject* ii = g_crash_objects; true; ++ii)
        {
            const float POS_MUL = 0.1f;
            const float ROT_MUL = static_cast<float>(M_PI) / 128.0f;
            vgl::vec3 pos1 = vgl::vec3(static_cast<float>(ii->m_data[0]), static_cast<float>(ii->m_data[1]), static_cast<float>(ii->m_data[2])) * POS_MUL;
            vgl::vec3 pos2 = vgl::vec3(static_cast<float>(ii->m_data[3]), static_cast<float>(ii->m_data[4]), static_cast<float>(ii->m_data[5])) * POS_MUL;
            vgl::vec3 rot1 = vgl::vec3(static_cast<float>(ii->m_data[6]), static_cast<float>(ii->m_data[7]), static_cast<float>(ii->m_data[8])) * ROT_MUL;
            vgl::vec3 rot2 = vgl::vec3(static_cast<float>(ii->m_data[9]), static_cast<float>(ii->m_data[10]), static_cast<float>(ii->m_data[11])) * ROT_MUL;

            vgl::vec3 pos = vgl::mix(pos1, pos2, ratio);
            vgl::vec3 rot = vgl::mix(rot1, rot2, ratio);
            vgl::mat4 trns = vgl::mat4::rotation_euler(rot, pos);

            // Needs to have meaning
            int16_t trainIndex = ii->m_data[12];
            if(trainIndex <= 0)
            {
                break;
            }

            //std::cout << pos << " ;; " << rot << std::endl;
            m_queue.push(g_data.getProgramOffscreen());
            m_queue.push(g_data.getMeshTrain(static_cast<unsigned>(trainIndex - 1)), trns);
        }
    }

    /// Push landmark vemputus.
    ///
    /// \param ticks Current timestamp.
    void pushLandmark(int ticks)
    {
        const int RAMP_UP_TIME = 2;
        const int RAMP_DOWN_TIME = 18;
        const int RAMP_TIME = RAMP_UP_TIME + RAMP_DOWN_TIME;
        const float VEMPUTUS_HIGH = 1.06f;
        const float VEMPUTUS_LOW = 0.97f;

        for(const int16_t* ii = g_landmark_data;;)
        {
            int16_t stime = ii[0];
            int16_t etime = static_cast<int16_t>(stime + ii[1]);
            if(stime == 0)
            {
                break;
            }
            if((stime <= ticks) && (ticks < etime))
            {
                int ramp_state = (ticks - stime) % RAMP_TIME;
                if (ramp_state < RAMP_UP_TIME)
                {
                    float phase = static_cast<float>(ramp_state) / static_cast<float>(RAMP_UP_TIME);
                    g_landmark_vemputus = (VEMPUTUS_HIGH - VEMPUTUS_LOW) * phase + VEMPUTUS_LOW;
                    return;
                }
                float phase = static_cast<float>(ramp_state - RAMP_UP_TIME) / static_cast<float>(RAMP_DOWN_TIME);
                g_landmark_vemputus = (VEMPUTUS_LOW - VEMPUTUS_HIGH) * phase + VEMPUTUS_HIGH;
                return;
            }

            ii += 2;
        }

        // Fallback.
        g_landmark_vemputus = 1.0f;
    }

    /// Push real world text into the render queue.
    ///
    /// \param ticks Current timestamp.
    void pushText(int ticks)
    {
        for(const TextPosition* ii = g_text_positions; (ii->m_text); ++ii)
        {
            if((ticks < ii->m_data[12]) || (ticks > ii->m_data[13]))
            {
                continue;
            }
            float ratio = static_cast<float>(ticks - ii->m_data[12]) / static_cast<float>(ii->m_data[13] - ii->m_data[12]);

            // Blink checks.
            const float BLINK_EASE = 0.2f;
            vgl::optional<float> blink;
            if(ratio < BLINK_EASE)
            {
                blink = ratio / BLINK_EASE;
            }
            else if((1.0f - BLINK_EASE) < ratio)
            {
                blink = (1.0f - ratio) / BLINK_EASE;
            }

            // Create copy of glyph mesh array.
            // Blink out parts at random if necessary.
            GlyphMeshArray glyph = g_data.getGlyph();
            if(blink)
            {
                dnload_srand(static_cast<unsigned>(ticks));
                for(auto& vv : glyph)
                {
                    if(vgl::frand() > *blink)
                    {
                        vv = nullptr;
                    }
                }
            }

            // Not blinked out - render.
            const float POS_MUL = 0.1f;
            const float ROT_MUL = static_cast<float>(M_PI) / 128.0f;
            vgl::vec3 pos1 = vgl::vec3(static_cast<float>(ii->m_data[0]), static_cast<float>(ii->m_data[1]), static_cast<float>(ii->m_data[2])) * POS_MUL;
            vgl::vec3 pos2 = vgl::vec3(static_cast<float>(ii->m_data[3]), static_cast<float>(ii->m_data[4]), static_cast<float>(ii->m_data[5])) * POS_MUL;
            vgl::vec3 rot1 = vgl::vec3(static_cast<float>(ii->m_data[6]), static_cast<float>(ii->m_data[7]), static_cast<float>(ii->m_data[8])) * ROT_MUL;
            vgl::vec3 rot2 = vgl::vec3(static_cast<float>(ii->m_data[9]), static_cast<float>(ii->m_data[10]), static_cast<float>(ii->m_data[11])) * ROT_MUL;

            vgl::vec3 pos = vgl::mix(pos1, pos2, ratio);
            vgl::vec3 rot = vgl::mix(rot1, rot2, ratio);
            vgl::mat4 trns = vgl::mat4::rotation_euler(rot, pos);

            const vgl::Font& font = g_data.getFont();
            float fs = static_cast<float>(ii->m_data[14]) * (1.0f / 255.0f);
            m_queue.push(g_data.getProgramFont());
            draw_text(m_queue, vgl::vec3(0.0f, 0.0f, 0.0f), trns, font, glyph, vgl::vec2(fs, fs), ii->m_text);
            //std::cout << ii->m_text << std::endl;
        }
    }

    /// Push train positions into the render queue.
    ///
    /// \param ticks Current timestamp.
    void pushTrains(int ticks)
    {
        auto pushTrainSegments = [this](vgl::vec3 tpos, float advance, float rot_y, int16_t count)
        {
            // Veturi.
            {
                vgl::mat4 trns = vgl::mat4::rotation_euler(0.0f, rot_y, 0.0f, tpos);
                m_queue.push(g_data.getProgramOffscreen());
                m_queue.push(g_data.getMeshTrain(0), trns);
                //std::cout << tpos << " ; " << tratio << std::endl;
            }
            // Vaunut.
            for(int16_t ii = 1; (ii < count); ++ii)
            {
                tpos[2] += advance;
                vgl::mat4 trns = vgl::mat4::rotation_euler(0.0f, rot_y, 0.0f, tpos);
                m_queue.push(g_data.getMeshTrain(1), trns);
                //std::cout << tpos << std::endl;
            }
        };

        // Push regular train positions into rendering.
        for(const TrainPosition* ii = g_train_movements; true; ++ii)
        {
            int tstart = ii->m_data[5];
            int tend = tstart + ii->m_data[6];
            if(tstart <= 0)
            {
                break;
            }
            VGL_ASSERT(tend > tstart);
            if((ticks >= tstart) && (ticks <= tend))
            {
                float tstartf = static_cast<float>(tstart);
                float tendf = static_cast<float>(tend);
                float tratio = (static_cast<float>(ticks) - tstartf) / (tendf - tstartf);
                float xpos = static_cast<float>(ii->m_data[0]) * 0.1f;
                float ypos = static_cast<float>(ii->m_data[1]) * 0.1f;
                float zstart = static_cast<float>(ii->m_data[2]) * 0.1f;
                float zend = static_cast<float>(ii->m_data[3]) * 0.1f;
                float zpos = vgl::mix(zstart, zend, tratio);

                // Determine advancement direction and rotation
                float advance = -19.8f;
                float rot_y = 0.0f;
                if(zend < zstart)
                {
                    advance = -advance;
                    rot_y = static_cast<float>(M_PI);
                }

                // Veturi.
                vgl::vec3 tpos(xpos, ypos, zpos);
                pushTrainSegments(tpos, advance, rot_y, ii->m_data[4]);
            }
        }

        // Push trains as volume levels into rendering.
        for(const int16_t* vol = g_volume_indicator_data; true; vol += 2)
        {
            int16_t pticks = vol[0];
            int16_t nticks = vol[2];
            VGL_ASSERT(nticks > pticks);
            if((pticks <= ticks) && (ticks <= nticks))
            {
                float ratio = static_cast<float>(ticks - pticks) / static_cast<float>(nticks - pticks);
                float strength = vgl::mix(static_cast<float>(vol[1]), static_cast<float>(vol[3]), ratio) * (1.0f / 255.0f);
                //std::cout << ratio << " ;; " << strength << " ;; " << static_cast<float>(vol[1]) << " ;; " << static_cast<float>(vol[3]) << std::endl;
                if(strength > 0.0f)
                {
                    const float XPOS[] =
                    {
                        -2.5f, -17.0f, -24.2f
                    };
                    const float YPOS = 0.7f;
                    const float ZFAR = -256.0f;
                    const float ZSTART = -94.0f;
                    const float ZEND = -24.0f;
                    const float ADVANCE = -19.8f;
                    const float ROT_Y = 0.0f;

                    for(unsigned ii = 0; (ii < 3); ++ii)
                    {
                        // Current level value may be one frame late depending on concurrency. Don't care.
                        float cstr = strength * g_data.getDataLevels(ticks, ii);
                        vgl::vec3 tpos(XPOS[ii], YPOS, vgl::mix(ZFAR, ZSTART, strength) + vgl::mix(0.0f, ZEND - ZSTART, cstr));
                        pushTrainSegments(tpos, ADVANCE, ROT_Y, 7);
                    }
                }
                //std::cout << "pushTrains: " << ticks << std::endl;
                break;
            }
        }
    }

    /// Push ukko animations into rendering.
    ///
    /// \param ticks Current timestamp.
    void pushUkko(int ticks)
    {
        for(const UkkoPosition* ii = g_ukko_positions; true; ++ii)
        {
            int tstart = ii->m_data[15];
            int duration = ii->m_data[16];
            // Abort if duration is 0.
            // Otherwise multiple ukkos can exist at the same time.
            if(duration == 0)
            {
                break;
            }
            int tend = tstart + duration;

            if((ticks >= tstart) && (ticks <= tend))
            {
                // Resolve position.
                const float ROT_MUL = static_cast<float>(M_PI / 128.0);
                float anim_ratio = static_cast<float>(ticks - tstart) / static_cast<float>(duration);
                vgl::vec3 pstart = vgl::vec3(static_cast<float>(ii->m_data[0]), static_cast<float>(ii->m_data[1]), static_cast<float>(ii->m_data[2])) * 0.1f;
                vgl::vec3 rstart = vgl::vec3(static_cast<float>(ii->m_data[3]), static_cast<float>(ii->m_data[4]), static_cast<float>(ii->m_data[5])) * ROT_MUL;
                vgl::vec3 pend = vgl::vec3(static_cast<float>(ii->m_data[6]), static_cast<float>(ii->m_data[7]), static_cast<float>(ii->m_data[8])) * 0.1f;
                vgl::vec3 rend = vgl::vec3(static_cast<float>(ii->m_data[9]), static_cast<float>(ii->m_data[10]), static_cast<float>(ii->m_data[11])) * ROT_MUL;
                vgl::vec3 anim_rot = vgl::mix(rstart, rend, anim_ratio);
                vgl::vec3 anim_pos = vgl::mix(pstart, pend, anim_ratio);
                vgl::mat4 trns = vgl::mat4::rotation_euler(anim_rot, anim_pos);

                // Resolve animation state.
                const vgl::Animation& anim = g_data.getAnimationUkko(static_cast<unsigned>(ii->m_data[12]));
                float anim_time = (static_cast<float>(ii->m_data[13]) + (static_cast<float>(ii->m_data[14]) * anim_ratio)) * 0.1f;
                vgl::AnimationState state(anim, anim_time);

                // Push ukko into the rendering pipeline.
                m_queue.push(g_data.getProgramSkeleton());
                m_queue.push(vgl::UniformSemantic::SKELETON, state.getBoneData(), state.getBoneCount());
                m_queue.push(g_data.getMeshUkko(), trns);
            }
        }
    }

    // Push visualization positions into rendering.
    ///
    /// \param ticks Current timestamp.
    void pushVisualizations(int ticks)
    {
        for(const VisualizationPosition* ii = g_visualization_positions; true; ++ii)
        {
            int t1 = ii->m_data[5];
            int t1d = ii->m_data[6];
            int t2 = ii->m_data[7];
            int t2d = ii->m_data[8];
            if(t2 == 0)
            {
                break;
            }
            float imul = vgl::linear_step(static_cast<float>(t1), static_cast<float>(t1 + t1d), static_cast<float>(ticks));
            float omul = vgl::linear_step(static_cast<float>(-t2 - t2d), static_cast<float>(-t2), static_cast<float>(-ticks));
            float mul = imul * omul;
            if(mul > 0.0f)
            {
                const float DSCALE = 0.005f;
                unsigned vis_index = static_cast<unsigned>(ii->m_data[10]);
                float vis_xscale = vis_index ? IntroData::VISUALIZATION_WIDTH_FFT : IntroData::VISUALIZATION_WIDTH_WAVE;
                vgl::vec3 vpos = vgl::vec3(static_cast<float>(ii->m_data[0]), static_cast<float>(ii->m_data[1]),
                        static_cast<float>(ii->m_data[2])) * 0.1f;
                float rot = static_cast<float>(M_PI * 2.0 / 256.0) * static_cast<float>(ii->m_data[3]);
                float dx = vgl::cos(rot) * (vis_xscale * static_cast<float>(IntroData::VISUALIZATION_WIDTH) * DSCALE);
                float dz = vgl::sin(rot) * (vis_xscale * static_cast<float>(IntroData::VISUALIZATION_WIDTH) * DSCALE);

                vgl::mat4 strns = vgl::mat4::scale(vis_xscale, mul, 1.0f);
                for(int jj = -ii->m_data[4]; (jj <= ii->m_data[4]); ++jj)
                {
                    vgl::vec3 offset = vpos + (vgl::vec3(dx, 0.0f, dz) * static_cast<float>(jj));
                    vgl::mat4 trns = vgl::mat4::rotation_euler(0.0f, rot, 0.0f) * strns;
                    trns[12] = offset[0];
                    trns[13] = offset[1];
                    trns[14] = offset[2];
                    float color = static_cast<float>(ii->m_data[9]) * (1.0f / 255.0f);

                    // Push visualization bar into the renderin pipeline.
                    m_queue.push(g_data.getProgramVisualization());
                    m_queue.pushBlend(vgl::ADDITIVE);
                    m_queue.pushDepth(GL_LESS, false);
                    m_queue.push(g_shader_fragment_visualization_uniform_color, color);
                    m_queue.push(g_data.getMeshVisualization(vis_index), trns);

                    // Invert the x multiplier so it matches for next element.
                    strns[0] *= -1.0f;
                }
            }
        }
    }

    /// Initialize the state.
    ///
    /// \param ticks Current timestamp.
    void initialize(int ticks)
    {
        // Constants used for rendering.
        const vgl::vec3 WORLD0_EXTENTS(48.0f, -160.0f, 48.0f);
        const vgl::vec3 WORLD12_EXTENTS(1.0f, -256.0f, 48.0f);
        const vgl::vec3 WORLD3_EXTENTS(192.0f, -192.0f, 48.0f);
        const vgl::vec3 WORLD4_EXTENTS(48.0f, -48.0f, 48.0f);

        // Current state calculated from ticks.
        float ratio = 0.0f;
        int scene_ticks = 0;

        // Direction variables captured into the direction lambda.
        int world_index = -1;
        vgl::vec3 pos;
        vgl::vec3 fw;
        vgl::vec3 up;
        float fov = 0.8f;

        auto storeDirection = [&ratio, &world_index, &pos, &fw, &up, &fov](const int16_t* direction, bool mode)
        {
            if (mode)
            {
                vgl::vec3 angles1 = vgl::vec3(static_cast<float>(direction[0]),
                    static_cast<float>(direction[1]),
                    static_cast<float>(direction[2])) * static_cast<float>(M_PI * 2.0 / 256.0);
                vgl::vec3 angles2 = vgl::vec3(static_cast<float>(direction[3]),
                    static_cast<float>(direction[4]),
                    static_cast<float>(direction[5])) * static_cast<float>(M_PI * 2.0 / 256.0);
                vgl::vec3 angles = mix(angles1, angles2, ratio);

                vgl::vec3 lookat1 = vgl::vec3(static_cast<float>(direction[6]),
                    static_cast<float>(direction[7]),
                    static_cast<float>(direction[8])) * 0.1f;
                vgl::vec3 lookat2 = vgl::vec3(static_cast<float>(direction[9]),
                    static_cast<float>(direction[10]),
                    static_cast<float>(direction[11])) * 0.1f;
                vgl::vec3 lookat = mix(lookat1, lookat2, ratio);

                float radius1 = static_cast<float>(direction[12]) * 0.1f;
                float radius2 = static_cast<float>(direction[13]) * 0.1f;
                vgl::vec3 offset(0.0f, 0.0f, vgl::mix(radius1, radius2, ratio));

                vgl::mat3 trns = vgl::mat3::rotation_euler(angles[0], angles[1], angles[2]);
                pos = lookat + (trns * offset);
                fw = lookat - pos;
                up = trns * vgl::vec3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                // Position.
                vgl::vec3 pos1 = vgl::vec3(static_cast<float>(direction[0]),
                    static_cast<float>(direction[1]),
                    static_cast<float>(direction[2])) * 0.1f;
                vgl::vec3 pos2 = vgl::vec3(static_cast<float>(direction[3]),
                    static_cast<float>(direction[4]),
                    static_cast<float>(direction[5])) * 0.1f;
                pos = mix(pos1, pos2, ratio);

                // Direction.
                vgl::vec3 dir1 = vgl::vec3(static_cast<float>(direction[6]),
                    static_cast<float>(direction[7]),
                    static_cast<float>(direction[8]));
                vgl::vec3 dir2 = vgl::vec3(static_cast<float>(direction[9]),
                    static_cast<float>(direction[10]),
                    static_cast<float>(direction[11]));
                fw = mix(dir1, dir2, ratio);

                // Up is not mixed.
                up = vgl::normalize(vgl::vec3(static_cast<float>(direction[12]),
                    static_cast<float>(direction[13]),
                    static_cast<float>(direction[14])));
            }

            // FOV is same for both modes.
            fov = vgl::mix(static_cast<float>(direction[15]), static_cast<float>(direction[16]), ratio) *
                static_cast<float>(M_PI / 255.0);

            // Store world index.
            world_index = direction[17];
        };

        // Calculate position from direction data.
        {
            int curr_ticks = ticks;

            // Loop until current ticks run out.
            for(const int16_t* direction = g_direction_data;;)
            {
                bool mode = false;
                int timestamp = static_cast<int>(direction[18]);
                if(timestamp < 0)
                {
                    timestamp = -timestamp;
                    mode = true;
                }

                // Reset scene ticks if we changed scene.
                if(world_index != direction[17])
                {
                    scene_ticks = 0;
                }

                if(timestamp >= curr_ticks)
                {
                    ratio = static_cast<float>(curr_ticks) / static_cast<float>(timestamp);
                    storeDirection(direction, mode);

                    // Increment ticks within this scene.
                    scene_ticks += curr_ticks;
                    break;
                }

                // Advance to next direction element.
                scene_ticks += timestamp;
                curr_ticks -= timestamp;
                direction += 19;
            }
        }

        // Calculate direction override.
        {
            // Loop until current ticks run out.
            for (const int16_t* direction = g_direction_override_data;;)
            {
                int tstart = static_cast<int>(direction[18]);
                if (!tstart)
                {
                    break;
                }

                int duration = static_cast<int>(direction[19]);
                int tend = tstart + duration;
                if ((tstart <= ticks) && (ticks < tend))
                {
                    ratio = static_cast<float>(ticks - tstart) / static_cast<float>(duration);
                    storeDirection(direction, false);
                    break;
                }
                direction += 20;
            }
        }

#if defined(USE_LD)
        g_direction_pos = pos;
        g_direction_fw = fw;
        g_direction_up = up;
        g_direction_fov = fov;

        if(g_flag_developer && g_pos)
        {
            if(g_preview_mesh.empty())
            {
                pos += *g_pos;
            }
            else
            {
                pos = *g_pos;
            }
            fw = g_fw;
            up = g_up;
            fov = g_fov;
        }
        else
        {
            g_fov = fov;
        }
#endif

        // Clear render queue before beginning.
        m_queue.clear();

        // Forward phase.
        const vgl::FrameBuffer& fbo = g_data.getFbo();
        m_queue.push(fbo);
        m_queue.pushBlend(vgl::DISABLED);
        m_queue.pushCull(GL_BACK);
        m_queue.pushDepth(GL_LESS, true);
        m_queue.pushClear(vgl::uvec4(255u, 255u, 255u, 0u), 1.0f);

#if defined(USE_LD)
        // Set debug mode on or off for all programs.
        if(g_visual_debug != g_last_visual_debug)
        {
            m_queue.push(g_data.getProgramFont());
            m_queue.push("debug_mode", g_visual_debug);
            m_queue.push(g_data.getProgramOffscreen());
            m_queue.push("debug_mode", g_visual_debug);
            m_queue.push(g_data.getProgramPost());
            m_queue.push("debug_mode", g_visual_debug);
            m_queue.push(g_data.getProgramSkeleton());
            m_queue.push("debug_mode", g_visual_debug);
            g_last_visual_debug = g_visual_debug;
        }
#endif

        // Default projection matrix.
        const vgl::FrameBuffer& screen = vgl::FrameBuffer::get_default();

#if defined(USE_LD)
        const vgl::Mesh* preview_mesh = g_data.getMeshByName(g_preview_mesh);
        if(preview_mesh)
        {
            vgl::vec2 rng(0.2f, 256.0f);
            vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
            vgl::mat4 cam = vgl::mat4::lookat(pos, pos + fw, up);
            m_queue.push(prj, rng, cam);

            if(g_preview_mesh == vgl::string_view("ukko"))
            {
                m_queue.push(g_data.getProgramSkeleton());
                const vgl::Animation& anim = g_data.getAnimationUkko(2);
#if 1
                float anim_time = static_cast<float>(ticks) * 0.01f;
                vgl::AnimationState state(anim, anim_time);
                std::cout << anim_time << std::endl;
#else
                vgl::AnimationState state(anim);
#endif
                m_queue.push(vgl::UniformSemantic::SKELETON, state.getBoneData(), state.getBoneCount());
                m_queue.push(*preview_mesh, vgl::mat4::translation(0.0f, 0.0f, 0.0f));
                m_queue.push(g_data.getProgramOffscreen());
            }
            else
            {
                m_queue.push(g_data.getProgramOffscreen());
                m_queue.push(*preview_mesh, vgl::mat4::translation(0.0f, 0.0f, 0.0f));
            }

            // Do not render a world.
            world_index = -1;
        }
        else if(g_preview_mesh.starts_with("world"))
        {
            world_index = static_cast<int>(g_preview_mesh[5]) - static_cast<int>('0');
        }
#endif
        // Render scenes.
        switch(world_index)
        {
            // Scene 0 - inside the train.
        case 0:
            {
                // Inside.
                vgl::vec2 rng(0.05f, 64.0f);
                vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
                {
                    vgl::mat4 cam = vgl::mat4::lookat(pos, pos + fw, up);
                    m_queue.push(prj, rng, cam);
                }

                // Push text before the camera settings are changed for the world coordinates.
                pushText(ticks);

                g_data.getWorld(0).render(m_queue, pos, WORLD0_EXTENTS);
                // Outside.
                {
                    vgl::vec3 cpos = vgl::vec3(IntroData::RAILS_X, 1.5f, -800.0f + static_cast<float>(scene_ticks) * 0.5f) + pos;
                    vgl::mat4 cam = vgl::mat4::lookat(cpos, cpos + fw, up);
                    m_queue.push(prj, rng, cam);

                    g_data.getWorld(1).render(m_queue, cpos, WORLD12_EXTENTS);
                }
            }
            break;

            // Scene 1 - tracks in front of the train.
        case 1:
            {
                const float JITTER_INC_START = 0.86f;
                const float JITTER_INC_MUL = 28.0f;

                const SignEasing& easing = g_data.getSignEasing(static_cast<unsigned>(scene_ticks));
                pos[2] += easing.m_z_pos;
                vgl::vec3 direction_target = pos + fw;
                vgl::vec3 jitter = g_data.getCameraJitter(static_cast<unsigned>(ticks));
                if(ratio > JITTER_INC_START)
                {
                    float jitter_inc = (ratio - JITTER_INC_START) / (1.0f - JITTER_INC_START);
                    jitter *= 1.0f + (jitter_inc * jitter_inc * JITTER_INC_MUL);
                }
                vgl::vec3 tgt = mix(direction_target + jitter, easing.m_easing_target, easing.m_easing_strength);

                vgl::vec2 rng(0.2f, 256.0f);
                vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
                vgl::mat4 cam = vgl::mat4::lookat(pos, tgt, up);
                m_queue.push(prj, rng, cam);

                // Dynamic objects likely to be in front of the world.
                pushTrains(ticks);

                g_data.getWorld(1).render(m_queue, pos, WORLD12_EXTENTS);
                g_data.getWorld(2).render(m_queue, pos, WORLD12_EXTENTS);
            }
            break;

            // Scene 2 - tracks but free camera.
        case 2:
            {
                vgl::vec2 rng(0.2f, 256.0f);
                vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
                vgl::mat4 cam = vgl::mat4::lookat(pos, pos + fw, up);
                m_queue.push(prj, rng, cam);

                // Change landmark state before rendering world.
                pushLandmark(ticks);

                g_data.getWorld(1).render(m_queue, pos, WORLD12_EXTENTS);
            }
            break;

            // Scene 3 - Kerava station yard.
        case 3:
            {
                vgl::vec2 rng(0.2f, 200.0f);
                vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
                vgl::mat4 cam = vgl::mat4::lookat(pos, pos + fw, up);
                m_queue.push(prj, rng, cam);

                // Dynamic objects likely to be in front of the world.
                pushCrash(ticks);
                pushText(ticks);
                pushTrains(ticks);
                pushUkko(ticks);

                g_data.getWorld(3).render(m_queue, pos, WORLD3_EXTENTS);

                pushVisualizations(ticks);
            }
            break;

            // Scene 4: sirkus hevo set.
        case 4:
#if !defined(USE_LD)
        default:
#endif
            {
                vgl::vec2 rng(0.05f, 64.0f);
                vgl::mat4 prj = vgl::mat4::projection(fov, screen.getWidth(), screen.getHeight(), rng.x(), rng.y());
                vgl::mat4 cam = vgl::mat4::lookat(pos, pos + fw, up);
                m_queue.push(prj, rng, cam);

                g_data.getWorld(4).render(m_queue, pos, WORLD4_EXTENTS);
            }
            break;

#if defined(USE_LD)
        default:
            break;
#endif
        }

        // Post phase.
        m_queue.push(screen);
        m_queue.pushBlend(vgl::DISABLED);
        m_queue.pushDepth(GL_FALSE, true);

        {
            m_queue.push(g_data.getProgramPost());

            m_queue.push(g_shader_fragment_post_uniform_color, *(fbo.getTextureColor()));
#if !defined(VGL_DISABLE_DEPTH_TEXTURE)
            m_queue.push(g_shader_fragment_post_uniform_depth, *(fbo.getTextureDepth()));
#endif

            m_queue.push(g_shader_vertex_post_uniform_scoords, get_stipple_settings());
            m_queue.push(g_shader_fragment_post_uniform_stipple, g_data.getTextureStipple());

            m_queue.push(g_data.getMeshQuad(), vgl::mat4::identity());
        }

        // Render text overlays directly in front of the camera.
        {
            vgl::vec2 fs(1.0f, static_cast<float>(g_screen_w) / static_cast<float>(g_screen_h));

            // Get data we're going to use.
            const vgl::GlslProgram& prog = g_data.getProgramFontOverlay();
            const vgl::Font& font = g_data.getFont();
            const GlyphMeshArray& glyph = g_data.getGlyph();
            m_queue.push(prog);
            m_queue.push(g_shader_vertex_font_overlay_uniform_scoords, get_stipple_settings());
            m_queue.pushPersistent(g_shader_fragment_font_overlay_uniform_stipple, g_data.getTextureStipple());
            m_queue.pushBlend(vgl::PREMULTIPLIED);

            for(const TextOverlay* ii = g_text_overlays; (ii->m_text); ++ii)
            {
                int tstart = ii->m_data[6];
                int tend = tstart + ii->m_data[7];
                if((ticks >= tstart) && (ticks <= tend))
                {
                    vgl::vec2 fpos(static_cast<float>(ii->m_data[0]), static_cast<float>(ii->m_data[1]));
                    // Set random seed and add the random element.
                    if(ii->m_data[2] < 0)
                    {
                        dnload_srand(static_cast<unsigned>(ticks));
                    }
                    else
                    {
                        dnload_srand(static_cast<unsigned>(ii->m_data[2]));
                    }
                    fpos[0] += vgl::frand(-1.0f, 1.0f) * static_cast<float>(ii->m_data[3]);
                    fpos[1] += vgl::frand(-1.0f, 1.0f) * static_cast<float>(ii->m_data[4]);
                    // Draw text.
                    vgl::vec2 cpos = fpos * fs * 0.01f;
                    vgl::vec2 cfs = fs * (static_cast<float>(ii->m_data[5]) * 0.01f);
                    draw_text(m_queue, vgl::vec3(cpos[0], cpos[1], 0.0f), vgl::mat4::identity(), font, glyph, cfs, ii->m_text);
                    //std::cout << ii->m_text << std::endl;
                }
            }
        }

#if defined(USE_LD)
        if(g_last_frame_display)
        {
            const vgl::Font& font = g_data.getFont();
            const GlyphMeshArray& glyph = g_data.getGlyph();
            vgl::vec2 fs = vgl::vec2(1.0f, static_cast<float>(g_screen_w) / static_cast<float>(g_screen_h)) * 0.05f;
            {
                std::string ftime = g_frame_counter.getFrameTime();
                draw_text(m_queue, vgl::vec3(-0.98f, 0.89f, 0.0f), vgl::mat4::identity(), font, glyph, fs, ftime.c_str());
            }
            {
                std::string ftime = g_frame_counter.getSwapTime();
                draw_text(m_queue, vgl::vec3(-0.98f, 0.80f, 0.0f), vgl::mat4::identity(), font, glyph, fs, ftime.c_str());
            }
            {
                std::string ftime = g_frame_counter.getFramerate();
                draw_text(m_queue, vgl::vec3(-0.98f, 0.71f, 0.0f), vgl::mat4::identity(), font, glyph, fs, ftime.c_str());
            }
        }
#endif
    }
};

/// Double-buffered intro states.
static IntroState g_intro_states[2];
/// Current intro state.
static IntroState* g_intro_state_current = nullptr;
/// Next intro state.
static IntroState* g_intro_state_next = nullptr;

/// Modify the visualization data from audio.
///
/// \param op Pointer to current ticks.
/// \return nullptr
static void* intro_state_generate_mesh_wave(void* op)
{
    int frame_number = *static_cast<int*>(op);
    const void* audio_buffer = reinterpret_cast<void*>(g_audio_buffer + generate_audio_position(frame_number));
    const float* input = reinterpret_cast<const float*>(audio_buffer);
    vgl::Mesh& msh = g_data.getMeshVisualization(0);
    uint8_t* mesh_data = reinterpret_cast<uint8_t*>(msh.getDataRaw());

    // Format of mesh data:
    // 3 floats of vertex data.
    // 3 int16_t's of normal data
    // => 18 bytes.
    // 4 vertices per segment.
    // => 72
    for(unsigned ii = 0; (ii < IntroData::VISUALIZATION_ELEMENTS); ++ii)
    {
        // Left channel only.
        float mid_y = input[ii * 2] * IntroData::VISUALIZATION_MULTIPLIER_WAVE;
        float lo_y = mid_y - IntroData::VISUALIZATION_HEIGHT_WAVE;
        float hi_y = mid_y + IntroData::VISUALIZATION_HEIGHT_WAVE;
        unsigned offset = ii * 72;
        VGL_ASSERT(offset < msh.getData().getDataSize());
        // Use memcpy and uint8_t buffers to avoid unaligned write.
        uint8_t* y1 = mesh_data + offset + (18 * 0) + 4;
        uint8_t* y2 = mesh_data + offset + (18 * 1) + 4;
        uint8_t* y3 = mesh_data + offset + (18 * 2) + 4;
        uint8_t* y4 = mesh_data + offset + (18 * 3) + 4;
        vgl::detail::internal_memcpy(y1, &lo_y, sizeof(float));
        vgl::detail::internal_memcpy(y2, &lo_y, sizeof(float));
        vgl::detail::internal_memcpy(y3, &hi_y, sizeof(float));
        vgl::detail::internal_memcpy(y4, &hi_y, sizeof(float));
    }

    return nullptr;
}

/// Modify the visualization data from audio.
///
/// \param op Pointer to current ticks.
/// \return nullptr
static void* intro_state_generate_mesh_fft(void* op)
{
    int frame_number = *static_cast<int*>(op);
    g_data.evaluateFFT(frame_number);
    const double* fft_data = g_data.getDataFFT(1);

    // Get mesh data.
    vgl::Mesh& msh = g_data.getMeshVisualization(1);
    uint8_t* mesh_data = reinterpret_cast<uint8_t*>(msh.getDataRaw());

    // Format of mesh data:
    // 3 floats of vertex data.
    // 3 int16_t's of normal data
    // => 18 bytes.
    // 4 vertices per segment.
    // => 72
    for(unsigned ii = 0; (ii < IntroData::VISUALIZATION_ELEMENTS); ++ii)
    {
        float value = static_cast<float>(fft_data[ii]);

        // Write to mesh.
        float lo_y = 0.0f;
        float hi_y = value * IntroData::VISUALIZATION_MULTIPLIER_FFT;
        unsigned offset = ii * 72;
        VGL_ASSERT(offset < msh.getData().getDataSize());
        // Use memcpy and uint8_t buffers to avoid unaligned write.
        uint8_t* y1 = mesh_data + offset + (18 * 0) + 4;
        uint8_t* y2 = mesh_data + offset + (18 * 1) + 4;
        uint8_t* y3 = mesh_data + offset + (18 * 2) + 4;
        uint8_t* y4 = mesh_data + offset + (18 * 3) + 4;
        vgl::detail::internal_memcpy(y1, &lo_y, sizeof(float));
        vgl::detail::internal_memcpy(y2, &lo_y, sizeof(float));
        vgl::detail::internal_memcpy(y3, &hi_y, sizeof(float));
        vgl::detail::internal_memcpy(y4, &hi_y, sizeof(float));
    }

    return nullptr;
}

/// Initializes the next intro state.
///
/// \param op Pointer to current ticks.
/// \return nullptr
static void* intro_state_generate_next(void* op)
{
    int frame_number = *static_cast<int*>(op);

    if(!g_intro_state_next)
    {
        g_intro_state_next = g_intro_states;
    }
    g_intro_state_next->initialize(frame_number);

    return nullptr;
}

// landmark_callback() definition.
void landmark_callback(vgl::RenderQueue& queue, const vgl::GlslProgram& program, const vgl::Mesh& mesh, const vgl::mat4& transform)
{
    vgl::mat4 mtrns = transform;

    mtrns[4] *= g_landmark_vemputus;
    mtrns[5] *= g_landmark_vemputus;
    mtrns[6] *= g_landmark_vemputus;

    queue.push(program);
    queue.push(mesh, mtrns);
}

#endif
