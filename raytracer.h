#pragma once

#include "basic.h"
#include "primitive.h"
#include "light.h"
#include <string>

class Camera;
class Scene;
class ImgWriter;
class Primitive;

class RayTracer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
    Color getBasicPhongColor(Primitive*, const Collision&,
                             const Vector3&, const Vector3&);
    Color getReflectionColor(Primitive*, int,
                             const Vector3&, const Collision&);
    Color getRefractionColor(Primitive*, const Vector3&,
                             const Vector3&, const Collision&,
                             int depth);
    double getShadow(Primitive*, Light*);
    Color traceRay(const Ray&, int depth);
public:
    void loadSceneFromFile(const std::string &file_name);
    void run();
    void setImgWriter(ImgWriter*);
    Camera * getCamera() const;
    RayTracer();

    ~RayTracer();
};

