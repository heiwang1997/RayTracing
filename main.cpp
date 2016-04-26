#include <iostream>
#include "raytracer.h"
#include "image.h"
#include "camera.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( int argc, const char* argv[] ) {
    RayTracer engine;
    engine.loadSceneFromFile("scene1.txt");
    RealtimeImgWriter *realimg = new RealtimeImgWriter(engine.getCamera()->getImgW(),
                                                       engine.getCamera()->getImgH());
    engine.setImgWriter(realimg);
    engine.run();
    realimg->show();
    waitKey();
    delete realimg;
    return 0;
}