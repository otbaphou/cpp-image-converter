#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader 
{
    array<char, 2> signature; // = {'B', 'M'}
    //Total file size including data and headers
    unsigned int size;
    //Reserved space
    int reserved_space;
    //Stride from the file beginning
    unsigned int stride;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader 
{
    unsigned int header_size;
    int width;
    int height;
    uint16_t plane_amount; // = 1;
    uint16_t bits_per_pixel; // = 24;
    unsigned int compression_type; // = 0;
    unsigned int data_bytes;
    int horizontal_resolution; // = 11811;
    int vertical_resolution; // = 11811;
    int used_colors; // = 0;
    int colors; // = 0x1000000;
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image)
{
    ofstream out(file, ios::binary);
    
    //##########Headers##########
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
    
    //########ConstValues########
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int bmp_stride = GetBMPStride(w);
    const size_t header_sum = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    //########ConstValues########
    
    //#####BitmapFileHeader#####
    file_header.signature[0] = 'B';
    file_header.signature[1] = 'M';
    file_header.size = header_sum + bmp_stride * h;
    file_header.reserved_space = 0;
    file_header.stride = header_sum;
    //#####BitmapFileHeader#####
    
    //#####BitmapInfoHeader#####
    info_header.header_size = sizeof(BitmapInfoHeader);
    info_header.width = w;
    info_header.height = h;
    info_header.plane_amount = 1;
    info_header.bits_per_pixel = 24;
    info_header.compression_type = 0;
    info_header.data_bytes = bmp_stride * h;
    info_header.horizontal_resolution = 11811;
    info_header.vertical_resolution = 11811;
    info_header.used_colors = 0;
    info_header.colors = 0x1000000;
    //#####BitmapInfoHeader#####
    
    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    std::vector<char> buff(bmp_stride);

    for (int y = h-1; y >= 0; y--) 
    {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) 
        {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), bmp_stride);
    }

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file)
{    
    ifstream ifs(file, ios::binary);
    
    //##########Headers##########
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
        
    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));
    
    //########ConstValues########
    const int w = info_header.width;
    const int h = info_header.height;
    const int bmp_stride = GetBMPStride(w);
    //########ConstValues########

    Image result(w, h, Color::Black());
    std::vector<char> buff(bmp_stride);

    for (int y = h-1; y >= 0; y--) 
    {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), bmp_stride);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib