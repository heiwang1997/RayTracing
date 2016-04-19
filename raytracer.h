#pragma once

#include "basic.h"
#include <string>

class Camera;
class Scene;
class ImgWriter;
class Primitive;

class RayTracer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
    Color getBasicPhongColor(Primitive*, const Ray&);
    Color traceRay(const Ray&);
public:
    void loadSceneFromFile(const std::string &file_name);
    void run();
    void setImgWriter(ImgWriter*);

    RayTracer();

    ~RayTracer();
};

