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
    Color getBasicPhongColor(Primitive*, const Ray&, const Vector3&, const Vector3&);
    Color traceRay(const Ray&, int depth);
public:
    void loadSceneFromFile(const std::string &file_name);
    void run();
    void setImgWriter(ImgWriter*);

    RayTracer();

    ~RayTracer();
};

