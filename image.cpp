#include "image.h"
#include <iostream>
using namespace std;

void RealtimeImgWriter::cache(int x, int y, const Color &col) {
    canvas.at<cv::Vec3b>(y, x) = cv::Vec3b((uchar)col.r, (uchar)col.g, (uchar)col.b);
    if (x == 0) {
        if (threshold == 10) {
            show();
            threshold = 0;
        }
        else
            threshold += 1;
    }
}

RealtimeImgWriter::RealtimeImgWriter(int w, int h) 
    : canvas(h, w, CV_8UC3, cv::Scalar(0, 0, 0)) {
    threshold = 0;
    cv::namedWindow(wnd_name, CV_WINDOW_AUTOSIZE);
}

void RealtimeImgWriter::show() const {
    cv::imshow(wnd_name, canvas);
    cv::waitKey(1);
}

void OpenCVImgLoader::load(const char *filename) {
    image = cv::imread(filename);
    if (image.empty()) {
        printf("Error Loading Image: %s", filename);
        return;
    }
    width = image.cols; height = image.rows;
}

Color OpenCVImgLoader::getColor(int x, int y) const {
    cv::Vec3b vec = image.at<cv::Vec3b>(y, x);
    return Color(vec[0], vec[1], vec[2]);
}

bool RealtimeImgWriter::saveToFile(const char *fname) {
    return cv::imwrite(fname, canvas);
}