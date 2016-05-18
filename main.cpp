#include <iostream>
#include "raytracer.h"
#include "camera.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( int argc, const char* argv[] ) {
    RayTracer engine;

    OpenCVImgLoader *cvloader = new OpenCVImgLoader;
    engine.setImgLoader(cvloader);
    engine.loadSceneFromFile("scene1.txt");
    RealtimeImgWriter *realimg = new RealtimeImgWriter(engine.getCamera()->getImgW(),
                                                       engine.getCamera()->getImgH());
    engine.setImgWriter(realimg);
    engine.run();
    realimg->show();
    waitKey();

    realimg->saveToFile("./sav.bmp");
    delete realimg;
    delete cvloader;
    return 0;
}