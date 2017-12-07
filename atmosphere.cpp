#include "atmosphere.h"
#include "context.h"
#include "utils.h"
#include "render.h"

namespace Atmosphere
{
    // Values from "CIE (1931) 2-deg color matching functions", see
    // "http://web.archive.org/web/20081228084047/
    //    http://www.cvrl.org/database/data/cmfs/ciexyz31.txt".
    static const real32 CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[380] = {
        360, 0.000129900000f, 0.000003917000f, 0.000606100000f,
        365, 0.000232100000f, 0.000006965000f, 0.001086000000f,
        370, 0.000414900000f, 0.000012390000f, 0.001946000000f,
        375, 0.000741600000f, 0.000022020000f, 0.003486000000f,
        380, 0.001368000000f, 0.000039000000f, 0.006450001000f,
        385, 0.002236000000f, 0.000064000000f, 0.010549990000f,
        390, 0.004243000000f, 0.000120000000f, 0.020050010000f,
        395, 0.007650000000f, 0.000217000000f, 0.036210000000f,
        400, 0.014310000000f, 0.000396000000f, 0.067850010000f,
        405, 0.023190000000f, 0.000640000000f, 0.110200000000f,
        410, 0.043510000000f, 0.001210000000f, 0.207400000000f,
        415, 0.077630000000f, 0.002180000000f, 0.371300000000f,
        420, 0.134380000000f, 0.004000000000f, 0.645600000000f,
        425, 0.214770000000f, 0.007300000000f, 1.039050100000f,
        430, 0.283900000000f, 0.011600000000f, 1.385600000000f,
        435, 0.328500000000f, 0.016840000000f, 1.622960000000f,
        440, 0.348280000000f, 0.023000000000f, 1.747060000000f,
        445, 0.348060000000f, 0.029800000000f, 1.782600000000f,
        450, 0.336200000000f, 0.038000000000f, 1.772110000000f,
        455, 0.318700000000f, 0.048000000000f, 1.744100000000f,
        460, 0.290800000000f, 0.060000000000f, 1.669200000000f,
        465, 0.251100000000f, 0.073900000000f, 1.528100000000f,
        470, 0.195360000000f, 0.090980000000f, 1.287640000000f,
        475, 0.142100000000f, 0.112600000000f, 1.041900000000f,
        480, 0.095640000000f, 0.139020000000f, 0.812950100000f,
        485, 0.057950010000f, 0.169300000000f, 0.616200000000f,
        490, 0.032010000000f, 0.208020000000f, 0.465180000000f,
        495, 0.014700000000f, 0.258600000000f, 0.353300000000f,
        500, 0.004900000000f, 0.323000000000f, 0.272000000000f,
        505, 0.002400000000f, 0.407300000000f, 0.212300000000f,
        510, 0.009300000000f, 0.503000000000f, 0.158200000000f,
        515, 0.029100000000f, 0.608200000000f, 0.111700000000f,
        520, 0.063270000000f, 0.710000000000f, 0.078249990000f,
        525, 0.109600000000f, 0.793200000000f, 0.057250010000f,
        530, 0.165500000000f, 0.862000000000f, 0.042160000000f,
        535, 0.225749900000f, 0.914850100000f, 0.029840000000f,
        540, 0.290400000000f, 0.954000000000f, 0.020300000000f,
        545, 0.359700000000f, 0.980300000000f, 0.013400000000f,
        550, 0.433449900000f, 0.994950100000f, 0.008749999000f,
        555, 0.512050100000f, 1.000000000000f, 0.005749999000f,
        560, 0.594500000000f, 0.995000000000f, 0.003900000000f,
        565, 0.678400000000f, 0.978600000000f, 0.002749999000f,
        570, 0.762100000000f, 0.952000000000f, 0.002100000000f,
        575, 0.842500000000f, 0.915400000000f, 0.001800000000f,
        580, 0.916300000000f, 0.870000000000f, 0.001650001000f,
        585, 0.978600000000f, 0.816300000000f, 0.001400000000f,
        590, 1.026300000000f, 0.757000000000f, 0.001100000000f,
        595, 1.056700000000f, 0.694900000000f, 0.001000000000f,
        600, 1.062200000000f, 0.631000000000f, 0.000800000000f,
        605, 1.045600000000f, 0.566800000000f, 0.000600000000f,
        610, 1.002600000000f, 0.503000000000f, 0.000340000000f,
        615, 0.938400000000f, 0.441200000000f, 0.000240000000f,
        620, 0.854449900000f, 0.381000000000f, 0.000190000000f,
        625, 0.751400000000f, 0.321000000000f, 0.000100000000f,
        630, 0.642400000000f, 0.265000000000f, 0.000049999990f,
        635, 0.541900000000f, 0.217000000000f, 0.000030000000f,
        640, 0.447900000000f, 0.175000000000f, 0.000020000000f,
        645, 0.360800000000f, 0.138200000000f, 0.000010000000f,
        650, 0.283500000000f, 0.107000000000f, 0.000000000000f,
        655, 0.218700000000f, 0.081600000000f, 0.000000000000f,
        660, 0.164900000000f, 0.061000000000f, 0.000000000000f,
        665, 0.121200000000f, 0.044580000000f, 0.000000000000f,
        670, 0.087400000000f, 0.032000000000f, 0.000000000000f,
        675, 0.063600000000f, 0.023200000000f, 0.000000000000f,
        680, 0.046770000000f, 0.017000000000f, 0.000000000000f,
        685, 0.032900000000f, 0.011920000000f, 0.000000000000f,
        690, 0.022700000000f, 0.008210000000f, 0.000000000000f,
        695, 0.015840000000f, 0.005723000000f, 0.000000000000f,
        700, 0.011359160000f, 0.004102000000f, 0.000000000000f,
        705, 0.008110916000f, 0.002929000000f, 0.000000000000f,
        710, 0.005790346000f, 0.002091000000f, 0.000000000000f,
        715, 0.004109457000f, 0.001484000000f, 0.000000000000f,
        720, 0.002899327000f, 0.001047000000f, 0.000000000000f,
        725, 0.002049190000f, 0.000740000000f, 0.000000000000f,
        730, 0.001439971000f, 0.000520000000f, 0.000000000000f,
        735, 0.000999949300f, 0.000361100000f, 0.000000000000f,
        740, 0.000690078600f, 0.000249200000f, 0.000000000000f,
        745, 0.000476021300f, 0.000171900000f, 0.000000000000f,
        750, 0.000332301100f, 0.000120000000f, 0.000000000000f,
        755, 0.000234826100f, 0.000084800000f, 0.000000000000f,
        760, 0.000166150500f, 0.000060000000f, 0.000000000000f,
        765, 0.000117413000f, 0.000042400000f, 0.000000000000f,
        770, 0.000083075270f, 0.000030000000f, 0.000000000000f,
        775, 0.000058706520f, 0.000021200000f, 0.000000000000f,
        780, 0.000041509940f, 0.000014990000f, 0.000000000000f,
        785, 0.000029353260f, 0.000010600000f, 0.000000000000f,
        790, 0.000020673830f, 0.000007465700f, 0.000000000000f,
        795, 0.000014559770f, 0.000005257800f, 0.000000000000f,
        800, 0.000010253980f, 0.000003702900f, 0.000000000000f,
        805, 0.000007221456f, 0.000002607800f, 0.000000000000f,
        810, 0.000005085868f, 0.000001836600f, 0.000000000000f,
        815, 0.000003581652f, 0.000001293400f, 0.000000000000f,
        820, 0.000002522525f, 0.000000910930f, 0.000000000000f,
        825, 0.000001776509f, 0.000000641530f, 0.000000000000f,
        830, 0.000001251141f, 0.000000451810f, 0.000000000000f,
    };

    // The conversion matrix from XYZ to linear sRGB color spaces.
    // Values from https://en.wikipedia.org/wiki/SRGB.
    static const real32 XYZ_TO_SRGB[9] = {
        +3.2406f, -1.5372f, -0.4986f,
        -0.9689f, +1.8758f, +0.0415f,
        +0.0557f, -0.2040f, +1.0570f
    };
    static const real32 kOzoneCrossSection[48] = {
        1.18e-27f, 2.182e-28f, 2.818e-28f, 6.636e-28f, 1.527e-27f, 2.763e-27f, 5.52e-27f,
        8.451e-27f, 1.582e-26f, 2.316e-26f, 3.669e-26f, 4.924e-26f, 7.752e-26f, 9.016e-26f,
        1.48e-25f, 1.602e-25f, 2.139e-25f, 2.755e-25f, 3.091e-25f, 3.5e-25f, 4.266e-25f,
        4.672e-25f, 4.398e-25f, 4.701e-25f, 5.019e-25f, 4.305e-25f, 3.74e-25f, 3.215e-25f,
        2.662e-25f, 2.238e-25f, 1.852e-25f, 1.473e-25f, 1.209e-25f, 9.423e-26f, 7.455e-26f,
        6.566e-26f, 5.105e-26f, 4.15e-26f, 4.228e-26f, 3.237e-26f, 2.451e-26f, 2.801e-26f,
        2.534e-26f, 1.624e-26f, 1.465e-26f, 2.078e-26f, 1.383e-26f, 7.105e-27f
    };

    // Match a wavelength to a CIE color tabulated value
    static real32 CieColorMatchingFunctionTableValue(real32 Wavelength, int Col)
    {
        if(Wavelength <= LAMBDA_MIN || Wavelength >= LAMBDA_MAX)
            return 0.f;

        real32 u = (Wavelength - LAMBDA_MIN) / 5.f;
        int Row = (int)(std::floor(u));
        Assert(Row >= 0 && Row+1 < 95);
        u -= Row;
        return CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[4 * Row + Col] * (1.0 - u) +
               CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[4 * (Row + 1) + Col] * u;
    }

    static real32 Interpolate(real32 const *Wavelengths, real32 const *WavelengthFunctions, int N, real32 Wavelength)
    {
        if(Wavelength < Wavelengths[0])
            return Wavelengths[0];

        for(int i = 0; i < N; ++i)
        {
            if(Wavelength < Wavelengths[i+1])
            {
                real32 u = (Wavelength - Wavelengths[i]) / (Wavelengths[i+1] - Wavelengths[i]);
                return WavelengthFunctions[i] * (1.f - u) + WavelengthFunctions[i+1] * u;
            }
        }
        return WavelengthFunctions[N-1];
    }

    /// Defined as "ExpTerm * exp(ExpScale * H) + LinearTerm * H + ConstantTerm"
    /// Clamped in [0,1]
    struct density_profile_layer
    {
        real32 Width;
        real32 ExpTerm;
        real32 ExpScale;
        real32 LinearTerm;
        real32 ConstantTerm;
    };

    /// Atmosphere is made of several layers from bottom to top
    /// The height of the topmost layer is inconsequential as it extends always to the top of the atmosphere
    struct density_profile
    {
        density_profile_layer Layers[2];
    };
    
    struct atmosphere_parameters
    {
        real32 TopRadius;           // distance between planet's center and top of atmosphere
        real32 BottomRadius;        // distance between planet's center and bottom of atmosphere
        vec3f  RayleighScattering;  // scattering coefficient of air molecules at max density (function of wavelength)
        density_profile Rayleigh;   // density profile of air molecules
        vec3f  MieScattering;       // scattering coefficient of aerosols at max density (function of wavelength)
        vec3f  MieExtinction;       // extinction coefficient of aerosols at max density (function of wavelength)
        density_profile Mie;        // density profile of aerosols
        //vec3f  AbsorptionScattering;// scattering coefficient of air molecules that absorb light at max density (function of wavelength)
        vec3f  AbsorptionExtinction;// extinction coefficient of air molecules that absorb light at max density (function of wavelength);
        density_profile Absorption; // density profile of air molecules that absorb light (e.g. ozone)
        vec3f  GroundAlbedo;        // average albedo of the ground
        vec3f  SolarIrradiance;     // defined at the top of the atmosphere
        real32 SunAngularRadius;    // < 0.1 radians
        real32 MiePhaseG;           // asymmetry coefficient for the Mie phase function
        real32 MinMuS;              // Cosine of the maximum sun zenith (102deg for earth, so that MinMuS = -0.208)

    };

    atmosphere_parameters AtmosphereParameters;
    mesh ScreenQuad = {};
    uint32 AtmosphereProgram = 0;
    uint32 TransmittanceTexture = 0;
    uint32 IrradianceTexture = 0;
    uint32 ScatteringTexture = 0;

    static const vec2i  kTransmittanceTextureSize = vec2i(256, 64);
    static const vec2i  kIrradianceTextureSize = vec2i(64, 16);
    static const vec4i  kScatteringTextureRMuMuSNuSize = vec4i(64, 128, 32, 8);
    static const vec3i  kScatteringTextureSize = vec3i(kScatteringTextureRMuMuSNuSize.w * kScatteringTextureRMuMuSNuSize.z, kScatteringTextureRMuMuSNuSize.y, kScatteringTextureRMuMuSNuSize.x);

    static const real32 kLengthUnitInMeters = 1000.f;
    static const real32 kRayleighScaleHeight = 8000.f;
    static const real32 kRayleigh = 1.24062e-6f;
    static const real32 kMieScaleHeight = 1200.f;
    static const real32 kMieAngstromAlpha = 0.f;
    static const real32 kMieAngstromBeta = 5.328e-3f;
    static const real32 kMieSingleScatteringAlbedo = 0.9f;
    static const real32 kDobsonUnit = 2.687e20f; // From wiki, in molecules.m^-2
    static const real32 kMaxOzoneNumberDensity = 300.f * kDobsonUnit / 15000.f; // Max nb density of ozone molecules in m^-3, 300 DU integrated over the ozone density profile (15km)
    static const real32 kConstantSolarIrradiance = 1.5f;
    static const real32 kGroundAlbedo = 0.000012f;
    static const real32 kSunAngularRadius = 0.00935f / 2.f;
    static const real32 kSunSolidAngle = M_PI * kSunAngularRadius * kSunAngularRadius;
    static const real32 kMiePhaseG = 0.99;
    static const real32 kMaxSunZenithAngle = DEG2RAD * 180.f;

    static vec3f ScatteringSpectrumToSRGB(real32 const *Wavelengths, real32 const *WavelengthFunctions, int N, real32 Scale)
    {
        real32 R = Interpolate(Wavelengths, WavelengthFunctions, N, LAMBDA_R) * Scale;
        real32 G = Interpolate(Wavelengths, WavelengthFunctions, N, LAMBDA_G) * Scale;
        real32 B = Interpolate(Wavelengths, WavelengthFunctions, N, LAMBDA_B) * Scale;
        return vec3f(R, G, B);
    }

    static void SendShaderUniforms(uint32 Program)
    {
        glUseProgram(Program);
        // TODO - Write the constants directly in the header (construct it from here), instead of this disgusting mess
        SendFloat(glGetUniformLocation(Program, "Atmosphere.BottomRadius"), AtmosphereParameters.BottomRadius / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.TopRadius"), AtmosphereParameters.TopRadius / kLengthUnitInMeters);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.RayleighScattering"), AtmosphereParameters.RayleighScattering);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[0].Width"), AtmosphereParameters.Rayleigh.Layers[0].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[0].ExpTerm"), AtmosphereParameters.Rayleigh.Layers[0].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[0].ExpScale"), AtmosphereParameters.Rayleigh.Layers[0].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[0].LinearTerm"), AtmosphereParameters.Rayleigh.Layers[0].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[0].ConstantTerm"), AtmosphereParameters.Rayleigh.Layers[0].ConstantTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[1].Width"), AtmosphereParameters.Rayleigh.Layers[1].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[1].ExpTerm"), AtmosphereParameters.Rayleigh.Layers[1].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[1].ExpScale"), AtmosphereParameters.Rayleigh.Layers[1].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[1].LinearTerm"), AtmosphereParameters.Rayleigh.Layers[1].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.RayleighDensity.Layers[1].ConstantTerm"), AtmosphereParameters.Rayleigh.Layers[1].ConstantTerm);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.MieExtinction"), AtmosphereParameters.MieExtinction);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.MieScattering"), AtmosphereParameters.MieScattering);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[0].Width"), AtmosphereParameters.Mie.Layers[0].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[0].ExpTerm"), AtmosphereParameters.Mie.Layers[0].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[0].ExpScale"), AtmosphereParameters.Mie.Layers[0].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[0].LinearTerm"), AtmosphereParameters.Mie.Layers[0].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[0].ConstantTerm"), AtmosphereParameters.Mie.Layers[0].ConstantTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[1].Width"), AtmosphereParameters.Mie.Layers[1].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[1].ExpTerm"), AtmosphereParameters.Mie.Layers[1].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[1].ExpScale"), AtmosphereParameters.Mie.Layers[1].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[1].LinearTerm"), AtmosphereParameters.Mie.Layers[1].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MieDensity.Layers[1].ConstantTerm"), AtmosphereParameters.Mie.Layers[1].ConstantTerm);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.AbsorptionExtinction"), AtmosphereParameters.AbsorptionExtinction);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[0].Width"), AtmosphereParameters.Absorption.Layers[0].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[0].ExpTerm"), AtmosphereParameters.Absorption.Layers[0].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[0].ExpScale"), AtmosphereParameters.Absorption.Layers[0].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[0].LinearTerm"), AtmosphereParameters.Absorption.Layers[0].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[0].ConstantTerm"), AtmosphereParameters.Absorption.Layers[0].ConstantTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[1].Width"), AtmosphereParameters.Absorption.Layers[1].Width / kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[1].ExpTerm"), AtmosphereParameters.Absorption.Layers[1].ExpTerm);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[1].ExpScale"), AtmosphereParameters.Absorption.Layers[1].ExpScale * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[1].LinearTerm"), AtmosphereParameters.Absorption.Layers[1].LinearTerm * kLengthUnitInMeters);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.AbsorptionDensity.Layers[1].ConstantTerm"), AtmosphereParameters.Absorption.Layers[1].ConstantTerm);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.GroundAlbedo"), AtmosphereParameters.GroundAlbedo);
        SendVec3(glGetUniformLocation(Program, "Atmosphere.SolarIrradiance"), AtmosphereParameters.SolarIrradiance);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.SunAngularRadius"), AtmosphereParameters.SunAngularRadius);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MiePhaseG"), AtmosphereParameters.MiePhaseG);
        SendFloat(glGetUniformLocation(Program, "Atmosphere.MinMuS"), AtmosphereParameters.MinMuS);
        CheckGLError("Atmosphere Uniform Shader");
    }

    void Init(game_memory *Memory, game_context *Context, game_state *State, game_system *System)
    {
        resource_helper *RH = &Memory->ResourceHelper;
        ScreenQuad = Make2DQuad(vec2i(-1,1), vec2i(1, -1));

        AtmosphereParameters.TopRadius = 6420000.f;
        AtmosphereParameters.BottomRadius = 6360000.f;

        density_profile_layer DefaultLayer = { 0.f, 0.f, 0.f, 0.f, 0.f };
        density_profile_layer RayleighLayer = { 0.f, 1.f, -1.f / kRayleighScaleHeight, 0.f, 0.f };
        density_profile_layer MieLayer = { 0.f, 1.f, -1.f / kMieScaleHeight, 0.f, 0.f };
        density_profile_layer Ozone0Layer = { 25000.f, 0.f, 0.f, 1.f / 15000.f, -2.f / 3.f };
        density_profile_layer Ozone1Layer = { 0.f, 0.f, 0.f, -1.f / 15000.f, 8.f / 3.f };

        // Compute absorption and scattering SRGB colors from wavelength
        int const nWavelengths = (LAMBDA_MAX-LAMBDA_MIN) / 10;
        real32 Wavelengths[nWavelengths];
        real32 SolarIrradianceWavelengths[nWavelengths];
        real32 GroundAlbedoWavelengths[nWavelengths];
        real32 RayleighScatteringWavelengths[nWavelengths];
        real32 MieScatteringWavelengths[nWavelengths];
        real32 MieExtinctionWavelengths[nWavelengths];
        real32 AbsorptionExtinctionWavelengths[nWavelengths];
        for(int l = LAMBDA_MIN; l <= LAMBDA_MAX; l += 10)
        {
            int Idx = (l-LAMBDA_MIN)/10;
            real32 Lambda = (real32)l / kLengthUnitInMeters; // micrometers
            real32 Mie = kMieAngstromBeta / kMieScaleHeight * std::pow(Lambda, -kMieAngstromAlpha);
            Wavelengths[Idx] = l;
            RayleighScatteringWavelengths[Idx] = kRayleigh * std::pow(Lambda, -4);
            MieScatteringWavelengths[Idx] = Mie * kMieSingleScatteringAlbedo;
            MieExtinctionWavelengths[Idx] = Mie;
            AbsorptionExtinctionWavelengths[Idx] = kMaxOzoneNumberDensity * kOzoneCrossSection[Idx];
            SolarIrradianceWavelengths[Idx] = kConstantSolarIrradiance;
            GroundAlbedoWavelengths[Idx] = kGroundAlbedo;
        }

        AtmosphereParameters.RayleighScattering = ScatteringSpectrumToSRGB(Wavelengths, RayleighScatteringWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.Rayleigh.Layers[0] = DefaultLayer;
        AtmosphereParameters.Rayleigh.Layers[1] = RayleighLayer;
        AtmosphereParameters.MieExtinction = ScatteringSpectrumToSRGB(Wavelengths, MieExtinctionWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.MieScattering = ScatteringSpectrumToSRGB(Wavelengths, MieScatteringWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.Mie.Layers[0] = DefaultLayer;
        AtmosphereParameters.Mie.Layers[1] = MieLayer;
        AtmosphereParameters.AbsorptionExtinction = ScatteringSpectrumToSRGB(Wavelengths, AbsorptionExtinctionWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.Absorption.Layers[0] = Ozone0Layer;
        AtmosphereParameters.Absorption.Layers[1] = Ozone1Layer;
        AtmosphereParameters.GroundAlbedo = ScatteringSpectrumToSRGB(Wavelengths, GroundAlbedoWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.SolarIrradiance = ScatteringSpectrumToSRGB(Wavelengths, SolarIrradianceWavelengths, nWavelengths, kLengthUnitInMeters);
        AtmosphereParameters.SunAngularRadius = kSunAngularRadius;
        AtmosphereParameters.MiePhaseG = kMiePhaseG;
        AtmosphereParameters.MinMuS = cosf(kMaxSunZenithAngle);
    }

    void Update()
    {
    }

    void Render(game_state *State, game_context *Context)
    {
        mat4f ModelFromView = State->Camera.ViewMatrix.Inverse();

        real32 AspectRatio = Context->WindowWidth / (real32)Context->WindowHeight;
        real32 FovRadians = HFOVtoVFOV(AspectRatio, Context->FOV) * DEG2RAD;
        real32 TanFovY = tanf(FovRadians * 0.5f);
        mat4f ViewFromClip
        (
            TanFovY * AspectRatio, 0.f, 0.f, 0.f,
            0.f, TanFovY, 0.f, 0.f,
            0.f, 0.f, 0.f, -1.f,
            0.f, 0.f, 1.f, 1.f
        );

        glDepthFunc(GL_LEQUAL);

        vec3f Camera;
        real32 l = 1.0f / (kLengthUnitInMeters);
        Camera.x = ModelFromView.M[3].x * l;
        Camera.y = ModelFromView.M[3].y * l;
        Camera.z = ModelFromView.M[3].z * l;
    


        glUseProgram(AtmosphereProgram);
        SendMat4(glGetUniformLocation(AtmosphereProgram, "InverseModelMatrix"), ModelFromView);
        SendMat4(glGetUniformLocation(AtmosphereProgram, "InverseProjMatrix"), ViewFromClip);
        SendVec3(glGetUniformLocation(AtmosphereProgram, "CameraPosition"), Camera);
        SendVec3(glGetUniformLocation(AtmosphereProgram, "SunDirection"), State->SunDirection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TransmittanceTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, IrradianceTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, ScatteringTexture);
        glBindVertexArray(ScreenQuad.VAO);
        RenderMesh(&ScreenQuad);
        glUseProgram(0);


        glDepthFunc(GL_LESS);
    }

    void ReloadShaders(game_memory *Memory, game_context *Context)
    {
        resource_helper *RH = &Memory->ResourceHelper;

        path VSPath, FSPath;

        // Precompute atmosphere textures
        frame_buffer RenderBuffer = {};
        MakeRelativePath(RH, VSPath, "data/shaders/atmosphere_precompute_vert.glsl");
        MakeRelativePath(RH, FSPath, "data/shaders/atmosphere_precompute_frag.glsl");
        uint32 AtmospherePrecomputeProgram = BuildShader(Memory, VSPath, FSPath);
        glUseProgram(AtmospherePrecomputeProgram);
        CheckGLError("Atmosphere Precompute Shader");

        SendShaderUniforms(AtmospherePrecomputeProgram);
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "Tex0"), 0);
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "Tex1"), 1);
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "Tex2"), 2);
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "Tex3"), 3);
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "Tex4"), 4);

        // Transmittance texture
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "ProgramUnit"), 0);
        DestroyFramebuffer(&RenderBuffer);
        RenderBuffer = MakeFramebuffer(1, kTransmittanceTextureSize);
        glDeleteTextures(1, &TransmittanceTexture);
        TransmittanceTexture = Make2DTexture(NULL, kTransmittanceTextureSize.x, kTransmittanceTextureSize.y, 4, true, false, 1, 
                                             GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        //FramebufferAttachBuffer(&RenderBuffer, 0, 4, true, false, false); // RGBA32F
        glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer.FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TransmittanceTexture, 0);
        glViewport(0, 0, kTransmittanceTextureSize.x, kTransmittanceTextureSize.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(ScreenQuad.VAO);
        RenderMesh(&ScreenQuad);

        // Ground Irradiance texture
        SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "ProgramUnit"), 1);
        DestroyFramebuffer(&RenderBuffer);
        RenderBuffer = MakeFramebuffer(2, kIrradianceTextureSize);
        glDeleteTextures(1, &IrradianceTexture);
        IrradianceTexture = Make2DTexture(NULL, kIrradianceTextureSize.x, kIrradianceTextureSize.y, 4, true, false, 1, 
                                             GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        uint32 DeltaIrradianceTexture = Make2DTexture(NULL, kIrradianceTextureSize.x, kIrradianceTextureSize.y, 4, true, false, 1, 
                                             GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer.FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DeltaIrradianceTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, IrradianceTexture, 0);
        glViewport(0, 0, kIrradianceTextureSize.x, kIrradianceTextureSize.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TransmittanceTexture);
        glBindVertexArray(ScreenQuad.VAO);
        RenderMesh(&ScreenQuad);

        // Single Scattering
        path GSPath;
        MakeRelativePath(RH, GSPath, "data/shaders/atmosphere_precompute_geom.glsl");
        uint32 ScatteringProgram = BuildShader(Memory, VSPath, FSPath, GSPath);
        glUseProgram(ScatteringProgram);
        SendShaderUniforms(ScatteringProgram);
        SendInt(glGetUniformLocation(ScatteringProgram, "ProgramUnit"), 2);
        SendInt(glGetUniformLocation(ScatteringProgram, "Tex0"), 0);
        SendInt(glGetUniformLocation(ScatteringProgram, "Tex1"), 1);
        SendInt(glGetUniformLocation(ScatteringProgram, "Tex2"), 2);
        SendInt(glGetUniformLocation(ScatteringProgram, "Tex3"), 3);
        SendInt(glGetUniformLocation(ScatteringProgram, "Tex4"), 4);
        CheckGLError("Scattering Precompute Shader");

        DestroyFramebuffer(&RenderBuffer);
        RenderBuffer = MakeFramebuffer(3, vec2i(kScatteringTextureSize.x, kScatteringTextureSize.y), false);
        glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer.FBO);
        glDeleteTextures(1, &ScatteringTexture);
        ScatteringTexture = Make3DTexture(kScatteringTextureSize.x, kScatteringTextureSize.y, kScatteringTextureSize.z, 4, true, false, 
                                          GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        uint32 DeltaRayleighTexture = Make3DTexture(kScatteringTextureSize.x, kScatteringTextureSize.y, kScatteringTextureSize.z, 4, true, false, 
                                          GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        uint32 DeltaMieTexture = Make3DTexture(kScatteringTextureSize.x, kScatteringTextureSize.y, kScatteringTextureSize.z, 4, true, false, 
                                          GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        FramebufferAttachBuffer(&RenderBuffer, 0, DeltaRayleighTexture);
        FramebufferAttachBuffer(&RenderBuffer, 1, DeltaMieTexture);
        FramebufferAttachBuffer(&RenderBuffer, 2, ScatteringTexture);
        D(if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LogError("Incomplete Scattering Framebuffer");
        })
        glViewport(0, 0, kScatteringTextureSize.x, kScatteringTextureSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TransmittanceTexture);
        glBindVertexArray(ScreenQuad.VAO);
        uint32 LayerLoc = glGetUniformLocation(ScatteringProgram, "ScatteringLayer");
        for(int layer = 0; layer < kScatteringTextureSize.z; ++layer)
        {
            SendInt(LayerLoc, layer);
            RenderMesh(&ScreenQuad);
        }

        CheckGLError("Scattering Precomputation");

        // Multiple Scattering
        uint32 DeltaScatteringDensityTexture = Make3DTexture(kScatteringTextureSize.x, kScatteringTextureSize.y, kScatteringTextureSize.z, 4, true, false,
                                                             GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

        glDisablei(GL_BLEND, 0);
        glDisablei(GL_BLEND, 1);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        uint32 NumScatteringBounces = 4;
        for(uint32 bounce = 2; bounce <= NumScatteringBounces; ++bounce)
        {
            // 1. Compute Scattering density into DeltaScatteringDensityTexture
            glUseProgram(ScatteringProgram);
            SendInt(glGetUniformLocation(ScatteringProgram, "ProgramUnit"), 3);
            FramebufferSetAttachmentCount(&RenderBuffer, 1);
            FramebufferAttachBuffer(&RenderBuffer, 0, DeltaScatteringDensityTexture);
            FramebufferAttachBuffer(&RenderBuffer, 1, 0);
            FramebufferAttachBuffer(&RenderBuffer, 2, 0);
            FramebufferAttachBuffer(&RenderBuffer, 3, 0);
            CheckFramebufferError("Scattering Density Framebuffer");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TransmittanceTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, DeltaRayleighTexture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, DeltaMieTexture);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_3D, DeltaRayleighTexture);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, DeltaIrradianceTexture);
            SendInt(glGetUniformLocation(ScatteringProgram, "ScatteringBounce"), bounce);
            for(int32 layer = 0; layer < kScatteringTextureSize.z; ++layer)
            {
                SendInt(LayerLoc, layer);
                RenderMesh(&ScreenQuad);
            }

            // 2. Compute Indirect irradiance into DeltaIrradianceTexture, accumulate it into IrradianceTexture
            glUseProgram(AtmospherePrecomputeProgram);
            SendInt(glGetUniformLocation(ScatteringProgram, "ProgramUnit"), 4);
            FramebufferSetAttachmentCount(&RenderBuffer, 2);
            FramebufferAttachBuffer(&RenderBuffer, 0, DeltaIrradianceTexture);
            FramebufferAttachBuffer(&RenderBuffer, 1, IrradianceTexture);
            CheckFramebufferError("Indirect Irradiance Framebuffer");
            BindTexture3D(DeltaRayleighTexture, 1);
            BindTexture3D(DeltaMieTexture, 2);
            BindTexture3D(DeltaRayleighTexture, 3);
            SendInt(glGetUniformLocation(AtmospherePrecomputeProgram, "ScatteringBounce"), bounce - 1);
            // Accumulate Irradiance in Attachment 1
            glEnablei(GL_BLEND, 1);
            RenderMesh(&ScreenQuad);
            glDisablei(GL_BLEND, 1);

            // 3. Compute Multiple scattering into RayleighTexture
            glUseProgram(ScatteringProgram);
            SendInt(glGetUniformLocation(ScatteringProgram, "ProgramUnit"), 5);
            FramebufferSetAttachmentCount(&RenderBuffer, 2);
            FramebufferAttachBuffer(&RenderBuffer, 0, DeltaRayleighTexture);
            FramebufferAttachBuffer(&RenderBuffer, 1, ScatteringTexture);
            CheckFramebufferError("MS Framebuffer");
            BindTexture2D(TransmittanceTexture, 0);
            BindTexture3D(DeltaScatteringDensityTexture, 1);
            glEnablei(GL_BLEND, 1);
            for(int32 layer = 0; layer < kScatteringTextureSize.z; ++layer)
            {
                SendInt(LayerLoc, layer);
                RenderMesh(&ScreenQuad);
            }
            glDisablei(GL_BLEND, 1);
        }


        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        glViewport(0, 0, Context->WindowWidth, Context->WindowHeight);

        glDeleteTextures(1, &DeltaIrradianceTexture);
        glDeleteTextures(1, &DeltaRayleighTexture);
        glDeleteTextures(1, &DeltaMieTexture);
        glDeleteTextures(1, &DeltaScatteringDensityTexture);
        glDeleteProgram(ScatteringProgram);
        glDeleteProgram(AtmospherePrecomputeProgram);
        DestroyFramebuffer(&RenderBuffer);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnablei(GL_BLEND, 0);
        glEnablei(GL_BLEND, 1);


        // Rendering shader
        MakeRelativePath(RH, VSPath, "data/shaders/atmosphere_vert.glsl");
        MakeRelativePath(RH, FSPath, "data/shaders/atmosphere_frag.glsl");
        AtmosphereProgram = BuildShader(Memory, VSPath, FSPath);
        glUseProgram(AtmosphereProgram);
        CheckGLError("Atmosphere Shader");

        // Init constants
        SendShaderUniforms(AtmosphereProgram);
        SendInt(glGetUniformLocation(AtmosphereProgram, "TransmittanceTexture"), 0);
        SendInt(glGetUniformLocation(AtmosphereProgram, "IrradianceTexture"), 1);
        SendInt(glGetUniformLocation(AtmosphereProgram, "ScatteringTexture"), 2);

        glUseProgram(0);
    }

    vec3f ConvertSpectrumToSRGB(real32 *Wavelengths, real32 *Spectrum, int N)
    {
        vec3f xyz(0), SRGB(0);

        int const dLambda = 1;
        for(int lambda = LAMBDA_MIN; lambda < LAMBDA_MAX; lambda += dLambda)
        {
            real32 Val = Interpolate(Wavelengths, Spectrum, N, lambda);
            xyz.x += CieColorMatchingFunctionTableValue(lambda, 1) * Val;
            xyz.y += CieColorMatchingFunctionTableValue(lambda, 2) * Val;
            xyz.z += CieColorMatchingFunctionTableValue(lambda, 3) * Val;
        }

        SRGB.x = MAX_LUMINOUS_EFFICACY * (XYZ_TO_SRGB[0] * xyz.x + XYZ_TO_SRGB[1] * xyz.y + XYZ_TO_SRGB[2] * xyz.z) * dLambda;
        SRGB.y = MAX_LUMINOUS_EFFICACY * (XYZ_TO_SRGB[3] * xyz.x + XYZ_TO_SRGB[4] * xyz.y + XYZ_TO_SRGB[5] * xyz.z) * dLambda;
        SRGB.z = MAX_LUMINOUS_EFFICACY * (XYZ_TO_SRGB[6] * xyz.x + XYZ_TO_SRGB[7] * xyz.y + XYZ_TO_SRGB[8] * xyz.z) * dLambda;

        return SRGB;
    }
}
