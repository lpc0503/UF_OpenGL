#pragma once
#ifndef OPENGLPLAYGROUND_TEXTURE_H
#define OPENGLPLAYGROUND_TEXTURE_H

#include "Core.h"

#include <cstdint>
#include <string>

class Image
{
public:
    Image();
    ~Image();
    Image(const Image& other) = delete;
    Image& operator=(const Image& rhs) = delete;

    Ref<Image> Clone();

    static Ref<Image> Load(const std::string& path);

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }
    int Channel() const { return m_Channels; }
    unsigned char* Data() const { return m_Data; }

protected:
    int m_Width;
    int m_Height;
    int m_Channels; // in file
    unsigned char* m_Data = nullptr;
};

class Texture
{

};


#endif //OPENGLPLAYGROUND_TEXTURE_H
