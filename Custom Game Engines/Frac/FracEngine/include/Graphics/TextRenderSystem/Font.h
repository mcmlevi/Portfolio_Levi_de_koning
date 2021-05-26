#pragma once
#include <set>

#include "TextTexture.h"
#include <stb_image/stb_truetype.h>

namespace Frac 
{
	class Font 
	{
	public:
		stbtt_fontinfo& GetFontInfo();
		unsigned char* GetBuffer() const;
		float GetScale() const;

		Font() = default;
		~Font();

		void LoadFromFile(const std::string& filePath); // todo, use FileIO for this

	private:
		void ReadFile(const std::string& filepath); // todo, use FileIO for this
		void SetupFont();
		unsigned char* m_fontBuffer;
		float m_scale = -1.0f;
		stbtt_fontinfo m_fontInfo;
	};

	struct Glyph
	{
		stbtt_aligned_quad Quad;
		float Kerning = 0;
		float Advance = 0;
		float BearingX = 0;
		unsigned Codepoint = 0;
	};

	class FontAtlas : public TextTexture
	{
	public:
		FontAtlas(const std::string& name, uint32_t width, uint32_t height);
		~FontAtlas();

		/// Add a range of glyphs from a font 
		int AddRange(const std::string& fontfile, float size, uint32_t from, uint32_t to);

		void AddSet(const std::string& fontfile, float size, std::set<uint32_t> set);

		/// Create a p
		void Apply();

		/// Get glyphs bounding boxes
		std::vector<Glyph> GetGlyphs(const std::string& text);

	private:
		struct FontEntry
		{
			Font* Font = nullptr;
			float Scale = -1.0f;
		};

		//Texture TODO

		// Define size for all
		uint32_t m_bitmapWidth = 0;
		uint32_t m_bitmapHeight = 0;
		unsigned char* m_bitmap = nullptr;
		stbtt_pack_context m_pc;
		std::vector<stbtt_packedchar> m_packedChars;
		std::unordered_map<unsigned long, FontEntry> m_fonts;
		std::unordered_map<uint32_t, int> m_unicodeToIndex;
		std::unordered_map<uint32_t, FontEntry*> m_unicodeToFont;
	};
} // namespace Frac