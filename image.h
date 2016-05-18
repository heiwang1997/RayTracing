#pragma once
#include "basic.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

static const char* wnd_name = "Render Result";

class ImgWriter
{
public:
    virtual void cache(int x, int y, const Color& col) = 0;
    virtual bool saveToFile(const char*) = 0;
    ImgWriter() {}
    virtual ~ImgWriter() {}
};

class RealtimeImgWriter : public ImgWriter
{
    int w, h;
    cv::Mat canvas;
    int threshold;
public:
    void cache(int x, int y, const Color& col);
    bool saveToFile(const char *string);
    void show() const;
    RealtimeImgWriter(int w, int h);
    ~RealtimeImgWriter() {}
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

class OpenCVImgLoader : public ImgLoader
{
    cv::Mat image;
public:
    void load(const char *filename);
    Color getColor(int x, int y) const;
};
