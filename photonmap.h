#pragma once

#include "basic.h"

class Primitive;

struct Photon {
    Vector3 direction;
    Vector3 power;
    Vector3 position;
    short split_plane;
    Primitive* primitive;
};

// max heap contains nearest photons
class NearestPhotons {
    const int max_size;
    int size;
    double max_dist2;
    Photon** data;
    double* dist2;
    Vector3 position;

    bool is_heap;
    void buildHeap();
    void heapInsert(Photon*, double);
    void percolateDown(int);

    friend class PhotonMap;
public:
    void insertPhoton(Photon*, double);
    double getMaxDist2();
    void setConstrain(double cons) { max_dist2 = cons; }
    NearestPhotons(int msize);
    ~NearestPhotons();
};

// kd-tree structured Array.
// each photon contains its split plane.
class PhotonMap {
    Photon* photons;
    Vector3 bbox_min;
    Vector3 bbox_max;
    const int max_size; // size of the array (max photons sotred = max_size - 1)
    int size; // number of photons stored.
    int nearest_photon_size;
    void balanceSegment(Photon*, const int index, const int start, const int end);
    void medianSplit(Photon*, const int start, const int end, const int median, const int axis);
    void locatePhotons(NearestPhotons*, int);
public:
    void store(const Photon&);
    void balance();
    void setNearestPhotonSize(int m_s) { nearest_photon_size = m_s; }
    Vector3 getIrradiance(Vector3 pos, Vector3 normal, double max_dist);
    double getInitialRadius2(Vector3 pos, double max_dist);
    int getSize() const { return size; }
    PhotonMap(int a_size);
    ~PhotonMap() {}
};

