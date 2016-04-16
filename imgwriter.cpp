#include "imgwriter.h"

void RealtimeImgWriter::cache(int x, int y, const Color &col) {
    canvas.at<cv::Vec3b>(y, x) = cv::Vec3b((uchar)col.r, (uchar)col.g, (uchar)col.b);
    cv::imshow("Render Result", canvas);
}

RealtimeImgWriter::RealtimeImgWriter(int w, int h) 
    : canvas(h, w, CV_8UC3, cv::Scalar(0, 0, 0)) {
    cv::namedWindow("Render Result", CV_WINDOW_AUTOSIZE);
    cv::imshow("Render Result", canvas);
}
