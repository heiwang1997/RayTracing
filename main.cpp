#include <iostream>
#include "raytracer.h"
#include "imgwriter.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( int argc, const char* argv[] ) {
    RealtimeImgWriter *realimg = new RealtimeImgWriter(800, 550);
    RayTracer engine;
    engine.setImgWriter(realimg);
    engine.run();
    realimg->show();
    waitKey();
    delete realimg;
    return 0;
}