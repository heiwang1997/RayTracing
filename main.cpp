#include <iostream>
#include "raytracer.h"
#include "imgwriter.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( int argc, const char* argv[] ) {
    ImgWriter *realimg = new RealtimeImgWriter(400, 300);
    RayTracer engine;
    engine.setImgWriter(realimg);
    engine.run();
    waitKey();
    delete realimg;
    return 0;
}