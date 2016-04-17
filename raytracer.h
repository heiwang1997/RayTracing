#pragma once

#include "basic.h"

class Camera;
class Scene;
class ImgWriter;
class Primitive;

class RayTracer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
    Color getBasicPhongColor(Primitive*, const Ray&);
public:
    void run();
    void setImgWriter(ImgWriter*);
    RayTracer() { }
    ~RayTracer() { }
};

