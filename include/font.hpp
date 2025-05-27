#ifndef FONT_HPP
#define FONT_HPP

#include <defs.hh>

typedef unsigned char byte;
class Texture2D;

extern "C" {

struct FT_LibraryRec_;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_  *FT_Library;
typedef struct FT_FaceRec_*  FT_Face;

};

#define BEGIN_CHAR_CODE 0
#define END_CHAR_CODE 128
#define CHARACTERS_COUNT (END_CHAR_CODE - BEGIN_CHAR_CODE)

struct Characters
{
    byte char_code;
    int width;
    int height;
    int bearing_x;
    int bearing_y;
    long horizontal_advance;
};

struct AtlasInfo
{
    float atlas_width;
    float atlas_height;
    Characters characters[CHARACTERS_COUNT];
};


void init_free_type();
void destroy_free_type();


class Font
{
public:
    Font();
    ~Font();

    void load_font(const char* file_name);
    Texture2D* create_font_atlas();
    AtlasInfo m_atlas_info;

private:
    FT_Face m_face;

};

#endif//FONT_HPP