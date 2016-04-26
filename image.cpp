#include "image.h"
#include <iostream>
using namespace std;

void RealtimeImgWriter::cache(int x, int y, const Color &col) {
    canvas.at<cv::Vec3b>(y, x) = cv::Vec3b((uchar)col.r, (uchar)col.g, (uchar)col.b);
    if (x == 0) show();
}

RealtimeImgWriter::RealtimeImgWriter(int w, int h) 
    : canvas(h, w, CV_8UC3, cv::Scalar(0, 0, 0)) {
    cv::namedWindow(wnd_name, CV_WINDOW_AUTOSIZE);
}

void RealtimeImgWriter::show() const {
    cv::imshow(wnd_name, canvas);
    cv::waitKey(1);
}
