#pragma once

#include "imgwriter.h"

class Camera;
class Scene;
class ImgWriter;

class RayTracer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
public:
    void run();
    void setImgWriter(ImgWriter*);
    RayTracer() { }
    ~RayTracer() { }
};

