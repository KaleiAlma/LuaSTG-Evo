#pragma once
#include "Core/Graphics/Device.hpp"
#include "Core/Graphics/Renderer.hpp"

namespace Core::Graphics
{
	struct GlyphInfo
	{
		uint32_t texture_index = 0; // Texture the glyph is on
		RectF    texture_rect;      // UV coordinates of glyph on texture
		Vector2F size;              // Glyph size
		Vector2F position;          // Distance of stroke from upper-left corner of glyph coordinates
		Vector2F advance;           // Advance
	};

	struct TrueTypeFontInfo
	{
		StringView source;
		uint32_t   font_face;        // Font index, multiple fonts in a font file
		Vector2F   font_size;        // Pixel size
		bool       is_force_to_file; // If true, `source` is taken as a path to a file, and does not read it into memory. This option ignored when `is_buffer = true`
		bool       is_buffer;        // If true, `source` is taken as binary data, not the file path.
	};

	struct IGlyphManager : public IObject
	{
		virtual float getLineHeight() = 0;
		virtual float getAscender() = 0;
		virtual float getDescender() = 0;

		virtual uint32_t getTextureCount() = 0;
		virtual ITexture2D* getTexture(uint32_t index) = 0;

		virtual bool cacheGlyph(uint32_t codepoint) = 0;
		virtual bool cacheString(StringView str) = 0;
		virtual bool flush() = 0;

		virtual bool getGlyph(uint32_t codepoint, GlyphInfo* p_ref_info, bool no_render) = 0;

		static bool create(IDevice* p_device, TrueTypeFontInfo* p_arr_info, size_t info_count, IGlyphManager** pp_glyphmgr);
	};

	struct ITextRenderer : public IObject
	{
		virtual void setScale(Vector2F const& scale) = 0;
		virtual Vector2F getScale() = 0;
		virtual void setColor(Color4B const color) = 0;
		virtual Color4B getColor() = 0;
		virtual void setZ(float const z) = 0;
		virtual float getZ() = 0;
		virtual void setGlyphManager(IGlyphManager* p_mgr) = 0;
		virtual IGlyphManager* getGlyphManager() = 0;

		// Get the bounding box of a drawn string (affected by setScale)
		virtual RectF getTextBoundary(StringView str) = 0;
		// Get text advance (affected by setScale)
		virtual Vector2F getTextAdvance(StringView str) = 0;
		// Draw text with y-axis facing up (affected by setScale)
		virtual bool drawText(StringView str, Vector2F const& start, Vector2F* endout) = 0;
		// Draw text in 3D space (affected by setScale)
		virtual bool drawTextInSpace(StringView str,
			Vector3F const& start, Vector3F const& right_vec, Vector3F const& down_vec,
			Vector3F* endout) = 0;

		static bool create(IRenderer* p_renderer, ITextRenderer** pp_textrenderer);
	};
}
