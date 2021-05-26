#include "pch.h"
#include "Graphics/TextRenderSystem/Font.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_image/stb_truetype.h>

#include <utf8/utf8.h>

#include "Graphics/OpenGL.h"

namespace Frac
{
	unsigned long long StringHash(const std::string& str)
	{
		unsigned long long hash = 0;
		int c;
		const char* cstr = str.c_str();

		while ((c = *cstr++))
			hash = c + (hash << 6) + (hash << 16) - hash;

		return hash;
	}

	stbtt_fontinfo& Font::GetFontInfo()
	{
		return m_fontInfo;
	}

	unsigned char* Font::GetBuffer() const
	{
		return m_fontBuffer;
	}

	float Font::GetScale() const
	{
		return m_scale;
	}

	Font::~Font()
	{
		free(m_fontBuffer);
	}

	void Font::LoadFromFile(const std::string& filePath)
	{
		ReadFile(filePath);
		SetupFont();
	}

	void Font::ReadFile(const std::string& filepath)
	{
		long size;
		FILE* fontFile = fopen(filepath.c_str(), "rb");
		fseek(fontFile, 0, SEEK_END);
		size = ftell(fontFile);
		fseek(fontFile, 0, SEEK_SET);

		m_fontBuffer = (unsigned char*)malloc(size);
		fread(m_fontBuffer, size, 1, fontFile);
		fclose(fontFile);
	}

	void Font::SetupFont()
	{

		// Get font info
		if (!stbtt_InitFont(&m_fontInfo, m_fontBuffer, 0))
			printf("Failed To init Font\n");

		// Worst case scenario ascent and descent
		int ascent;
		int descent;
		int lineGap;
		stbtt_GetFontVMetrics(&m_fontInfo, &ascent, &descent, &lineGap);
		m_scale = 1.0f / float(ascent - descent);
	}
	
	FontAtlas::FontAtlas(const std::string& name, uint32_t width, uint32_t height) :m_bitmapWidth{ width }, m_bitmapHeight{ height }
	{
		m_bitmap = static_cast<unsigned char*>(malloc(m_bitmapWidth * m_bitmapHeight));
		stbtt_PackBegin(&m_pc, m_bitmap, m_bitmapWidth, m_bitmapHeight, 0, 1, nullptr);
	}

	FontAtlas::~FontAtlas()
	{
		free(m_bitmap);
	}

	int FontAtlas::AddRange(const std::string& fontfile, float size, uint32_t from, uint32_t to)
	{
		const auto fontID = static_cast<unsigned long>(StringHash(fontfile));
		if (m_fonts.find(fontID) == m_fonts.end())
		{
			Font* font = new Font();
			font->LoadFromFile(fontfile);
			FontEntry fe;
			fe.Font = font;
			int ascent;
			int descent;
			int lineGap;
			stbtt_GetFontVMetrics(&font->GetFontInfo(), &ascent, &descent, &lineGap);
			fe.Scale = size / float(ascent - descent);
			m_fonts[fontID] = fe;
		}

		// pack font
		auto& fontEntry = m_fonts[fontID];
		const auto num = m_packedChars.size();
		m_packedChars.resize(num + (to - from));
		stbtt_PackFontRange(&m_pc, fontEntry.Font->GetBuffer(), 0, size, from, int(to - from), &m_packedChars[num]);
		for (size_t i = 0; i < to - from; i++)
		{
			m_unicodeToIndex[static_cast<unsigned>(from + i)] = static_cast<int>(num + i);
			m_unicodeToFont[static_cast<unsigned>(from + i)] = &fontEntry;
		}

		return 0;
	}

	void FontAtlas::AddSet(const std::string& fontfile, float size, std::set<uint32_t> set)
	{
		for (auto it : set)
		{
			AddRange(fontfile, size, it, it + 1);
		}
	}

	void FontAtlas::Apply()
	{
		stbtt_PackEnd(&m_pc);
		LoadTexture(m_bitmap, glm::ivec2{ m_bitmapWidth,m_bitmapHeight }, GL_UNSIGNED_BYTE, GL_RED);
#if defined (PLATFORM_WINDOWS)
		stbi_write_png("atlas.png", m_bitmapWidth, m_bitmapHeight, 1, m_bitmap, m_bitmapWidth);
#endif
	}

	std::vector<Glyph> FontAtlas::GetGlyphs(const std::string& text)
	{
		std::vector<Glyph> glyphs;
		float x = 0;
		float y = 0;

		std::vector<unsigned int> utf32text;
		utf8::utf8to32(text.begin(), text.end(), back_inserter(utf32text));

		for (size_t i = 0; i < utf32text.size(); i++)
		{
			Glyph glyph;
			auto unicode = utf32text[i];
			glyph.Codepoint = unicode;
			assert(m_unicodeToFont.find(unicode) != m_unicodeToFont.end());
			const auto& fontEntry = *m_unicodeToFont[unicode];
			auto info = fontEntry.Font->GetFontInfo();

			const int charIndex = m_unicodeToIndex[unicode];
			stbtt_GetPackedQuad(&m_packedChars[0], m_bitmapWidth, m_bitmapHeight, charIndex, &x, &y, &glyph.Quad, 0);
			const int glyphIndex = stbtt_FindGlyphIndex(&info, unicode);

			int advance = 0, bearing = 0;
			stbtt_GetGlyphHMetrics(&info, glyphIndex, &advance, &bearing);
			glyph.Advance = static_cast<float>(advance);
			glyph.BearingX = static_cast<float>(bearing);
			glyph.Advance *= fontEntry.Scale;
			glyph.BearingX *= fontEntry.Scale;

			// Kerning to next letter
			if (i + 1 < utf32text.size())
			{
				const auto nextCodepoint = utf32text[i + 1];
				const auto next = stbtt_FindGlyphIndex(&info, nextCodepoint);
				const int kern = stbtt_GetGlyphKernAdvance(&info, glyphIndex, next);
				glyph.Kerning = static_cast<float>(kern);
				glyph.Kerning *= fontEntry.Scale;
			}

			glyphs.push_back(glyph);
		}

		return glyphs;
	}
}
