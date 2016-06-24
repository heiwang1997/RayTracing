// -------------------------------------------
// Main
#define IMAGE_USING_OPENCV
// -------------------------------------------
#include <iostream>
#include "raytracer.h"
#include "photonvisualizer.h"
#include "camera.h"

#ifndef IMAGE_USING_OPENCV

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;

typedef unsigned char uchar;

// ------------------Writer---------------------
class StbImgWriter : public ImgWriter
{
    int w, h;
    uchar* mat;
public:
    void cache(int x, int y, const Color& col);
    bool saveToFile(const char *string);
    StbImgWriter(int w, int h);
    ~StbImgWriter();
};

StbImgWriter::StbImgWriter(int pw, int ph) {
    w = pw; h = ph;
    mat = new uchar[w * h * 3];
}

StbImgWriter::~StbImgWriter() {
    delete[] mat;
}


void StbImgWriter::cache(int x, int y, const Color &col) {
    if (x >= 0 && y >= 0 && x < w && y < h) {
        mat[3 * (y * w + x) + 0] = (uchar)col.r;
        mat[3 * (y * w + x) + 1] = (uchar)col.g;
        mat[3 * (y * w + x) + 2] = (uchar)col.b;
    }
}

bool StbImgWriter::saveToFile(const char *fname) {
    stbi_write_bmp(fname, w, h, 3, mat);
    return true;
}


// ------------------Loader---------------------
class StbImgLoader : public ImgLoader
{
    uchar *data;
public:
    void load(const char *filename);
    Color getColor(int x, int y) const;
    virtual ~StbImgLoader();
    StbImgLoader() { data = 0; }
};

void StbImgLoader::load(const char *filename) {
    int comp;
    data = stbi_load(filename, &width, &height, &comp, 3);
    cout << filename << ' ' << width << ' ' << height << endl;
}

Color StbImgLoader::getColor(int x, int y) const {
    if (x >= 0 && y >= 0 && x < width && y < height && data != 0) {
        return Color((double)data[3 * (y * width + x) + 0],
                     (double)data[3 * (y * width + x) + 1],
                     (double)data[3 * (y * width + x) + 2]);
    }
    return Color();
}

StbImgLoader::~StbImgLoader() {
    stbi_image_free(data);
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

        StbImgLoader *cvloader = new StbImgLoader;
        engine.setImgLoader(cvloader);
        engine.loadSceneFromFile(buf + ".scn");
        engine.setProjectName(buf);
        StbImgWriter *realimg = new StbImgWriter(engine.getCamera()->getImgW(),
            engine.getCamera()->getImgH());
        engine.setImgWriter(realimg);
        //engine.setIndirectIllumination(true);
        engine.run();

        realimg->saveToFile("./sav.bmp");
        delete realimg;
        delete cvloader;
        return 0;
    }

    RayTracer engine;

    StbImgLoader *cvloader = new StbImgLoader;
    engine.setImgLoader(cvloader);
    engine.loadSceneFromFile(buf + ".scn");
    StbImgWriter *realimg = new StbImgWriter(engine.getCamera()->getImgW(),
                                                       engine.getCamera()->getImgH());
    engine.setImgWriter(realimg);
    //engine.setIndirectIllumination(true);
    engine.run();

    realimg->saveToFile("./sav.bmp");
    delete realimg;
    delete cvloader;
    return 0;
}

#else

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

#endif
