#include <font.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <texture2d.hpp>
#include <glad/glad.h>

namespace _priv
{
    FT_Library _library;
}

/*helper*/
static int helper_max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void init_free_type()
{
    FT_Error error = FT_Init_FreeType(&_priv::_library);
    if (error)
    {
        std::cout << "ERROR::FREETYTPE: Free Type library could not initialized!" << std::endl;
    }
}

void destroy_free_type()
{
    FT_Done_FreeType(_priv::_library);
}

Font::Font()
{
}

Font::~Font()
{
    FT_Done_Face(m_face);
}

// LOOK: https://freetype.org/freetype2/docs/reference/index.html to handle load font via network or smthng...
void Font::load_font(const char *file_name)
{
    FT_Error error = FT_New_Face(_priv::_library, file_name, 0, &m_face);
    if (error == FT_Err_Unknown_File_Format)
    {
        // ... the font file could be opened and read, but it appears
        // ... that its font format is unsupported
        std::cout << "ERROR::FREETYTPE: font format is not supported!" << std::endl;
    }
    else if (error)
    {
        // ... another error code means that the font file could not
        // ... be opened or read, or that it is broken...
        std::cout << "ERROR::FREETYTPE: font file could not be read or it is broken file!" << std::endl;
    }

    // Note that both functions return an error code. Usually, an error occurs with a fixed-size font format (like FNT or PCF) when trying to set the pixel size to a value that is not listed in the face->fixed_sizes array.
    // FT_Set_Pixel_Sizes sibling of FT_Set_Char_Size but it allows you to determine chars by directly with the pixel values.
    error = FT_Set_Char_Size(
          m_face,  /* handle to face object         */
          0,       /* char_width in 1/64 of points  if it is 0 it means same with height*/
          16*64,   /* char_height in 1/64 of points if it is 0 it means same with width*/
          300,     /* horizontal device resolution  *///FIXME: do not hardcode here
          300);    /* vertical device resolution    */

    if (error)
    {
        // ... another error code means that the font file could not
        // ... be opened or read, or that it is broken...
        std::cout << "ERROR::FREETYTPE: char size could not be set (it might be a result of a fixed sized font file)!" << std::endl;
    }
    std::cout << "num glyphs: " << m_face->num_glyphs << std::endl;
    std::cout << "num faces: " << m_face->num_faces << std::endl;
    // width: the width (in pixels) of the bitmap accessed via face->glyph->bitmap.width.
    // height: the height (in pixels) of the bitmap accessed via face->glyph->bitmap.rows.
    // bearingX: the horizontal bearing e.g. the horizontal position (in pixels) of the bitmap relative to the origin accessed via face->glyph->bitmap_left.
    // bearingY: the vertical bearing e.g. the vertical position (in pixels) of the bitmap relative to the baseline accessed via face->glyph->bitmap_top.
    // advance: the horizontal advance e.g. the horizontal distance (in 1/64th pixels) from the origin to the origin of the next glyph. Accessed via face->glyph->advance.x.
    // std::cout << "bitmap_width: " << m_face->available_sizes->width << std::endl;
    // std::cout << "bitmap_height: " << m_face->available_sizes->height << std::endl;
    // std::cout << "bitmap_xppem: " << m_face->available_sizes->x_ppem << std::endl;
    // std::cout << "bitmap_yppem: " << m_face->available_sizes->y_ppem << std::endl;
}

Texture2D* Font::create_font_atlas()
{
    // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_02
    size_t width = 0;
    size_t height = 0;
    Texture2D* atlas_texture = new Texture2D();
    int i = 0;
    for (unsigned char c = BEGIN_CHAR_CODE; c < END_CHAR_CODE; c++, i++)//c = 32; c < 128
    {
        // load character glyph 
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        m_atlas_info.characters[i].char_code = c;
        m_atlas_info.characters[i].width = m_face->glyph->bitmap.width;
        m_atlas_info.characters[i].height = m_face->glyph->bitmap.rows;
        m_atlas_info.characters[i].bearing_x = m_face->glyph->bitmap_left;
        m_atlas_info.characters[i].bearing_y = m_face->glyph->bitmap_top;
        m_atlas_info.characters[i].horizontal_advance = m_face->glyph->advance.x;

        width += m_face->glyph->bitmap.width;
        height = helper_max(height, m_face->glyph->bitmap.rows);
    }
    m_atlas_info.atlas_width = width;
    m_atlas_info.atlas_height = height;

    glActiveTexture(GL_TEXTURE0);
    atlas_texture->create_texture(width, height, NULL, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    atlas_texture->generate_texture();

    std::cout << "atlas_width" << width << std::endl;
    std::cout << "atlas_height" << height << std::endl;
    int _gl_max_tex_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_gl_max_tex_size);
    std::cout << "max tex size: " << _gl_max_tex_size << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // FIXME: be aware of you enabled blend, if there is a problem after just check here!

    //glActiveTexture(GL_TEXTURE0);
    atlas_texture->bind();
    int x = 0;
    for(int i = BEGIN_CHAR_CODE; i < END_CHAR_CODE; i++) {
        if(FT_Load_Char(m_face, i, FT_LOAD_RENDER))
            continue;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
        //glTextureSubImage2D((unsigned int)(*atlas_texture), 0, x, 0, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
        

        x += m_face->glyph->bitmap.width;
    }
    atlas_texture->unbind();


    return atlas_texture;
}
//*/
/*
{
    Texture2D* atlas_texture = new Texture2D();
    // quick and dirty max texture size estimate

	int max_dim = (1 + (m_face->size->metrics.height >> 6)) * ceilf(sqrtf(NUM_GLYPHS));
	int tex_width = 1;
	while(tex_width < max_dim) tex_width <<= 1;
	int tex_height = tex_width;
    
    //

    unsigned char* pixels = (unsigned char*)calloc(tex_width * tex_height, 1);
	int pen_x = 0, pen_y = 0;

	for(int i = 0; i < NUM_GLYPHS; ++i){
		FT_Load_Char(m_face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &m_face->glyph->bitmap;

		if(pen_x + bmp->width >= tex_width){
			pen_x = 0;
			pen_y += ((m_face->size->metrics.height >> 6) + 1);
		}

		for(int row = 0; row < bmp->rows; ++row){
			for(int col = 0; col < bmp->width; ++col){
				int x = pen_x + col;
				int y = pen_y + row;
				pixels[y * tex_width + x] = bmp->buffer[row * bmp->pitch + col];
			}
		}

		// this is stuff you'd need when rendering individual glyphs out of the atlas

		info[i].x0 = pen_x;
		info[i].y0 = pen_y;
		info[i].x1 = pen_x + bmp->width;
		info[i].y1 = pen_y + bmp->rows;

		info[i].x_off   = m_face->glyph->bitmap_left;
		info[i].y_off   = m_face->glyph->bitmap_top;
		info[i].advance = m_face->glyph->advance.x >> 6;

		pen_x += bmp->width + 1;

	}

    atlas_texture->create_texture(tex_width, tex_height, pixels, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    atlas_texture->generate_texture();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return atlas_texture;
}
*/
