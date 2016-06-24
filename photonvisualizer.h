#pragma once

#include "basic.h"
#include "primitive.h"
#include "hitpointmap.h"
#include <string>

class Camera;
class Scene;
class ImgWriter;
class PhotonTracer;
class PhotonMap;

class PhotonVisualizer {
    Camera* camera;
    Scene* scene;
    ImgWriter* img;
    PhotonTracer* tracer;
    HitPointMap* hitPointMap;
    std::string projectName;
    void traceDiffuse(Primitive *, const Collision &, const Vector3&,
                      const Vector3&, HitPoint);
    void traceReflect(Primitive *, int,
                       const Vector3 &, const Collision &,
                       int &hash, HitPoint);
    void traceRefract(Primitive *, const Vector3 &,
                       const Vector3 &, const Collision &,
                       int depth, int &hash, HitPoint);
    void traceRay(const Ray&, int depth, int& hash, HitPoint);
    void singleThreadRowSampling(int base, int** primitive_table);
    void singleThreadRowResampling(int base, int** primitive_table);
public:
    void loadSceneFromFile(const std::string &file_name);
    void setProjectName(const std::string &pro_name);
    void run();
    void setImgWriter(ImgWriter*);
    void setImgLoader(ImgLoader*);
    Camera * getCamera() const;
    PhotonVisualizer();

    ~PhotonVisualizer();
};

