#pragma once
#include "basic.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImgWriter
{
public:
    virtual void cache(int x, int y, const Color& col) = 0;
    ImgWriter() {}
    ~ImgWriter() {}
};

class RealtimeImgWriter : public ImgWriter
{
    int w, h;
    cv::Mat canvas;
public:
    void cache(int x, int y, const Color& col);
    RealtimeImgWriter(int w, int h);
    ~RealtimeImgWriter() {}
};

