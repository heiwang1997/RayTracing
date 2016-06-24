#pragma once

#include "scene.h"
#include "photonmap.h"

class HitPointMap;

class PhotonTracer {
    Scene* scene;
    PhotonMap* photon_map;
    HitPointMap* hit_point_map;
    int max_photons;
    void tracePhoton(const Ray& c_ray, Photon& photon, int depth);
    void diffusePhoton(const PrimitiveCollision& pcol, Photon& photon, int depth);
    void reflectPhoton(const Vector3& N, const Vector3& L, const Ray& c_ray,
                       const PrimitiveCollision& pcol, Photon& photon, int depth);
    void refractPhoton(const Vector3& N, const Vector3& L, const Ray& c_ray,
                       const PrimitiveCollision& pcol, Photon& photon, int depth);
    void runSingleThread(int threadID, int randID);
public:
    void run();
    void setScene(Scene * const s) { scene = s; }
    void setMaxPhotons(const int p) { max_photons = p; }
    PhotonMap * getPhotonMap();
    void setHitPointMap(HitPointMap* hpm) { hit_point_map = hpm; }
    void clearPhotonMap() { if (photon_map) delete photon_map; photon_map = 0; }
    PhotonTracer();
    ~PhotonTracer();
};

