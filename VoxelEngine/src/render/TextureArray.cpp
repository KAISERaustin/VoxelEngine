#include "render/TextureArray.h"

#include <array>
#include <algorithm>
#include <cstdint>

static uint32_t xorshift32(uint32_t& s)
{
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return s;
}

static void make_grass_16x16(std::array<uint8_t, 16 * 16 * 4>& rgba)
{
    uint32_t seed = 0xA341316Cu;

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            const int i = (y * 16 + x) * 4;

            const uint32_t r = xorshift32(seed);
            const int noise = static_cast<int>(r & 31u) - 15;

            int rr = 40 + noise / 2;
            int gg = 150 + noise;
            int bb = 40 + noise / 3;

            rr = std::clamp(rr, 0, 255);
            gg = std::clamp(gg, 0, 255);
            bb = std::clamp(bb, 0, 255);

            rgba[i + 0] = static_cast<uint8_t>(rr);
            rgba[i + 1] = static_cast<uint8_t>(gg);
            rgba[i + 2] = static_cast<uint8_t>(bb);
            rgba[i + 3] = 255;
        }
    }
}

static void make_stone_16x16(std::array<uint8_t, 16 * 16 * 4>& rgba)
{
    uint32_t seed = 0xC8013EA4u;

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            const int i = (y * 16 + x) * 4;

            const uint32_t r = xorshift32(seed);
            const int noise = static_cast<int>(r & 63u) - 31;

            int g = 120 + noise;
            g = std::clamp(g, 0, 255);

            rgba[i + 0] = static_cast<uint8_t>(g);
            rgba[i + 1] = static_cast<uint8_t>(g);
            rgba[i + 2] = static_cast<uint8_t>(g);
            rgba[i + 3] = 255;
        }
    }
}

TextureArray::~TextureArray()
{
    if (m_tex) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }
}

bool TextureArray::create_grass_stone_16()
{
    if (m_tex) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_tex);
    glTextureStorage3D(m_tex, 1, GL_RGBA8, 16, 16, 2);

    std::array<uint8_t, 16 * 16 * 4> grass{};
    std::array<uint8_t, 16 * 16 * 4> stone{};
    make_grass_16x16(grass);
    make_stone_16x16(stone);

    glTextureSubImage3D(m_tex, 0, 0, 0, 0, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, grass.data());
    glTextureSubImage3D(m_tex, 0, 0, 0, 1, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, stone.data());

    glTextureParameteri(m_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}

void TextureArray::bind_unit(GLuint unit) const
{
    glBindTextureUnit(unit, m_tex);
}
