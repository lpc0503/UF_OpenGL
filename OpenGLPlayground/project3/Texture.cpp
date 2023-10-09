#include "Texture.h"

#include <stb_image.h>

Image::Image()
{
}

Image::~Image()
{
    if(m_Data)
    {
        stbi_image_free(m_Data);
    }
}

Ref<Image> Image::Load(const std::string &path)
{
    Ref<Image> image = MakeRef<Image>();

    unsigned char *data = stbi_load(path.c_str(), &image->m_Width, &image->m_Height, &image->m_Channels, 0);
    if(data)
    {
        image->m_Data = data;
        return image;
    }
    else
    {
        return nullptr;
    }
}

Ref<Image> Image::Clone()
{
    Ref<Image> image = MakeRef<Image>();
    image->m_Width = m_Width;
    image->m_Height = m_Height;
    image->m_Channels = m_Channels;

    image->m_Data = new unsigned char[m_Width * m_Height];
    memcpy(image->m_Data, m_Data, m_Width * m_Height);

    return image;
}
