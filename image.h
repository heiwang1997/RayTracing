#pragma once
#include "basic.h"

class ImgWriter
{
public:
    virtual void cache(int x, int y, const Color& col) = 0;
    virtual bool saveToFile(const char*) = 0;
    ImgWriter() {}
    virtual ~ImgWriter() {}
};

class ImgLoader
{
protected:
    int width;
    int height;
public:
    virtual void load(const char*) = 0;
    virtual Color getColor(int x, int y) const = 0;
    virtual int getWidth() { return width; }
    virtual int getHeight() { return height; }
    ImgLoader() {}
    virtual ~ImgLoader() {}
};
