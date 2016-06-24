#pragma once

#include "basic.h"
#include "primitive.h"
#include "light.h"
#include "image.h"
#include <string>

class Camera;
class Scene;
class ImgWriter;
class Primitive;
class PhotonTracer;
class PhotonMap;

class RayTracer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
    bool indirect_illumination;
    PhotonTracer* tracer;
    PhotonMap* photonMap;
    Color getBasicPhongColor(Primitive*, const Collision&,
                             const Vector3&, const Vector3&,
                             int shade_sample, int& hash);
    Color getReflectionColor(Primitive*, int,
                             const Vector3&, const Collision&, int& hash,
                             int shade_sample);
    Color getRefractionColor(Primitive*, const Vector3&,
                             const Vector3&, const Collision&,
                             int depth, int& hash, int shade_sample);
    double getShadow(Light*, int, const Vector3&);
    Color traceRay(const Ray&, int depth, int& hash, int shade_sample, Primitive* last_prim);
public:
    void loadSceneFromFile(const std::string &file_name);
    void run();
    void setImgWriter(ImgWriter*);
    void setImgLoader(ImgLoader*);
    void setIndirectIllumination(bool on) { indirect_illumination = on; }
    Camera* getCamera() const;
    RayTracer();

    ~RayTracer();
};

