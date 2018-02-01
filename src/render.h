#ifndef RENDER_H
#define RENDER_H

#include "definitions.h"
#include "GL/glew.h"
#include <map>

#define MAX_FBO_ATTACHMENTS 5

struct game_context;

struct image
{
    void *Buffer;
    int32 Width;
    int32 Height;
    int32 Channels;
};

struct glyph
{
    // NOTE - Schema of behavior
    //   X,Y 0---------o   x
    //       |         |   |
    //       |         |   |
    //       |         |   | CH
    //       |         |   |
    //       0---------o   v
    //       x---------> CW
    //       x-----------> AdvX
    int X, Y;
    real32 TexX0, TexY0, TexX1, TexY1; // Absolute texcoords in Font Bitmap where the char is
    int CW, CH;
    real32 AdvX;
};

// NOTE - Ascii-only
// TODO - UTF
struct font
{
    int Width;
    int Height;
    int LineGap;
    int Ascent;
    int NumGlyphs;
    int Char0, CharN;
    real32 MaxGlyphWidth;
    real32 GlyphHeight;
    uint32 AtlasTextureID;
    uint8 *Buffer;
    glyph *Glyphs;
};

struct display_text
{
    uint32 VAO;
    uint32 VBO[2]; // 0 : positions+texcoords batched, 1 : indices
    uint32 IndexCount;
    uint32 Texture;
    vec4f  Color;
};

struct mesh
{
    uint32 VAO;
    uint32 VBO[5]; // 0: indices, 1-5 : position, normal, texcoords, tangent, bitangent
    uint32 IndexCount;
    uint32 IndexType;
    mat4f  ModelMatrix;
};

struct material
{
    uint32 AlbedoTexture;
    uint32 RoughnessMetallicTexture;
    uint32 NormalTexture;
    uint32 EmissiveTexture;

    vec3f AlbedoMult;
    vec3f EmissiveMult;
    float RoughnessMult;
    float MetallicMult;
};

struct model
{
    std::vector<mesh> Mesh;
    std::vector<int> MaterialIdx;
    std::vector<material> Material;
};

struct frame_buffer
{
    vec2i Size;
    uint32 NumAttachments;
    uint32 FBO;
    uint32 DepthBufferID;
    uint32 BufferIDs[MAX_FBO_ATTACHMENTS];
};

enum render_resource_type
{
    RESOURCE_IMAGE,
    RESOURCE_TEXTURE,
    RESOURCE_FONT,
    RESOURCE_COUNT
};

struct resource_store
{
    std::vector<std::string> Keys;
    std::vector<void*> Values;
};

struct render_resources
{
    resource_helper *RH;
    uint32 *DefaultDiffuseTexture;
    uint32 *DefaultNormalTexture;
    uint32 *DefaultEmissiveTexture;

    resource_store Images;
    resource_store Textures;
    resource_store Fonts;
};

void CheckGLError(char const *Mark = "");
void CheckFramebufferError(char const *Mark = "");

void *ResourceCheckExist(render_resources *RenderResources, render_resource_type Type, path const Filename);
void ResourceStore(render_resources *RenderResources, render_resource_type Type, path const Filename, void *Resource);
void ResourceFree(render_resources *RenderResources);
image *ResourceLoadImage(render_resources *RenderResources, path const Filename, bool IsFloat, bool FlipY = true,
                         int32 ForceNumChannel = 0);
font *ResourceLoadFont(render_resources *RenderResources, path const Filename, uint32 PixelHeight, int Char0 = 32, int CharN = 127);
uint32 *ResourceLoad2DTexture(render_resources *RenderResources, path const Filename, bool IsFloat, bool FloatHalfPrecision,
                              uint32 AnisotropicLevel, int MagFilter = GL_LINEAR, int MinFilter = GL_LINEAR_MIPMAP_LINEAR, 
                              int WrapS = GL_CLAMP_TO_EDGE, int WrapT = GL_CLAMP_TO_EDGE, int32 ForceNumChannel = 0);

void BindTexture2D(uint32 TextureID, uint32 TextureUnit);
void BindTexture3D(uint32 TextureID, uint32 TextureUnit);
uint32 Make2DTexture(void *ImageBuffer, uint32 Width, uint32 Height, uint32 Channels, bool IsFloat,
        bool FloatHalfPrecision, real32 AnisotropicLevel, int MagFilter, int MinFilter, int WrapS, int WrapT);
uint32 Make3DTexture(uint32 Width, uint32 Height, uint32 Depth, uint32 Channels, bool IsFloat, bool FloatHalfPrecision,
        int MagFilter, int MinFilter, int WrapS, int WrapT, int WrapR);
uint32 MakeCubemap(render_resources *RenderResources, path *Paths, bool IsFloat, bool FloatHalfPrecision, uint32 Width, uint32 Height, bool MakeMipmap);
void ComputeIrradianceCubemap(render_resources *RenderResources, char const *HDREnvmapFilename,
        uint32 *HDRCubemapEnvmap, uint32 *HDRGlossyEnvmap, uint32 *HDRIrradianceEnvmap);
uint32 PrecomputeGGXLUT(render_resources *RenderResources, uint32 Width);

mesh MakeUnitCube(bool MakeAdditionalAttribs = true);
mesh Make2DQuad(vec2i Start, vec2i End);
mesh Make3DPlane(game_memory *Memory, vec2i Dimension, uint32 Subdivisions, uint32 TextureRepeatCount, bool Dynamic = false);
mesh MakeUnitSphere(bool MakeAdditionalAttribs = true, real32 TexScale = 1.f);

uint32 BuildShader(game_memory *Memory, char *VSPath, char *FSPath, char *GSPath = NULL);

void SendVec2(uint32 Loc, vec2f value);
void SendVec3(uint32 Loc, vec3f value);
void SendVec4(uint32 Loc, vec4f value);
void SendMat4(uint32 Loc, mat4f value);
void SendInt(uint32 Loc, int value);
void SendFloat(uint32 Loc, real32 value);

frame_buffer MakeFramebuffer(uint32 NumAttachments, vec2i Size, bool AddDepthBuffer = true);
void DestroyFramebuffer(frame_buffer *FB);
void FramebufferAttachBuffer(frame_buffer *FB, uint32 Attachment, uint32 Channels, bool IsFloat, bool FloatHalfPrecision, bool Mipmap);
void FramebufferAttachBuffer(frame_buffer *FB, uint32 Attachment, uint32 TextureID);
void FramebufferSetAttachmentCount(frame_buffer *FB, int Count);

uint32 MakeVertexArrayObject();
uint32 AddIBO(uint32 Usage, uint32 Size, void const *Data);
uint32 AddEmptyVBO(uint32 Size, uint32 Usage);
void FillVBO(uint32 Attrib, uint32 AttribStride, uint32 Type, size_t ByteOffset, uint32 Size, void const *Data);
void UpdateVBO(uint32 VBO, size_t ByteOffset, uint32 Size, void *Data);
void DestroyMesh(mesh *Mesh);
void RenderMesh(mesh *Mesh);

real32 GetDisplayTextWidth(char const *Text, font *Font, real32 Scale);
void FillDisplayTextInterleaved(char const *Text, uint32 TextLength, font *Font, vec3i Pos, int MaxPixelWidth,
                                real32 *VertData, uint16 *Indices, real32 Scale = 1.0f);
void FillDisplayTextInterleavedUTF8(char const *Text , uint32 TextLength, font *Font, vec3i Pos, int MaxPixelWidth,
                                    real32 *VertData, uint16 *IdxData, real32 Scale = 1.0f);

bool ResourceLoadGLTFModel(render_resources *RenderResources, model *Model, path const Filename, game_context *Context);
#endif