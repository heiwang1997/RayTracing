#include <iostream>
#include "raytracer.h"
#include "photonvisualizer.h"
#include "camera.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

static const char* wnd_name = "Render Result";
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

class DumImgWriter : public ImgWriter
{
    int w, h;
    cv::Mat canvas;
public:
    void cache(int x, int y, const Color& col);
    bool saveToFile(const char *string);
    void show() const;
    DumImgWriter(int w, int h);
    ~DumImgWriter() {}
};

class OpenCVImgLoader : public ImgLoader
{
    cv::Mat image;
public:
    void load(const char *filename);
    Color getColor(int x, int y) const;
};

void RealtimeImgWriter::cache(int x, int y, const Color &col) {
    canvas.at<cv::Vec3b>(y, x) = cv::Vec3b((uchar)col.b, (uchar)col.g, (uchar)col.r);
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
    return Color(vec[2], vec[1], vec[0]);
}

bool RealtimeImgWriter::saveToFile(const char *fname) {
    return cv::imwrite(fname, canvas);
}

bool DumImgWriter::saveToFile(const char *fname) {
    return cv::imwrite(fname, canvas);
}

void DumImgWriter::cache(int x, int y, const Color &col) {
    canvas.at<cv::Vec3b>(y, x) = cv::Vec3b((uchar)col.b, (uchar)col.g, (uchar)col.r);
    if (x == 0) {
        cout << "Dum Img Writer: Currently on Row:" << y << endl;
    }
}

DumImgWriter::DumImgWriter(int w, int h) 
    : canvas(h, w, CV_8UC3, cv::Scalar(0, 0, 0)) {
}

void DumImgWriter::show() const {
    cv::namedWindow(wnd_name, CV_WINDOW_AUTOSIZE);
    cv::imshow(wnd_name, canvas);
    cv::waitKey(1);
}

int main( int argc, const char* argv[] ) {
    cout << "Enter Scene Name:" << endl;
    std::string buf;
    cin >> buf;

    char method[1024];
    cout << "Enter Render Method" << endl;
    cin >> method;

    if (method[0] == 'P') {
        //RayTracer engine;
        PhotonVisualizer engine;

        OpenCVImgLoader *cvloader = new OpenCVImgLoader;
        engine.setImgLoader(cvloader);
        engine.loadSceneFromFile(buf + ".scn");
        engine.setProjectName(buf);
        RealtimeImgWriter *realimg = new RealtimeImgWriter(engine.getCamera()->getImgW(),
            engine.getCamera()->getImgH());
        engine.setImgWriter(realimg);
        //engine.setIndirectIllumination(true);
        engine.run();
        realimg->show();
        waitKey();

        realimg->saveToFile("./sav.bmp");
        delete realimg;
        delete cvloader;
        return 0;
    }

    RayTracer engine;
    //PhotonVisualizer engine;

    OpenCVImgLoader *cvloader = new OpenCVImgLoader;
    engine.setImgLoader(cvloader);
    engine.loadSceneFromFile(buf + ".scn");
    RealtimeImgWriter *realimg = new RealtimeImgWriter(engine.getCamera()->getImgW(),
                                                       engine.getCamera()->getImgH());
    engine.setImgWriter(realimg);
    //engine.setIndirectIllumination(true);
    engine.run();
    realimg->show();
    waitKey();

    realimg->saveToFile("./sav.bmp");
    delete realimg;
    delete cvloader;
    return 0;
}