#pragma once

#include "basic.h"
#include "photonmap.h"
#include <mutex>

class ImgWriter;
class Camera;
class PhotonMap;
class Primitive;

struct HitPoint {
    Vector3 pos;
    Vector3 weight;
    Vector3 normal;
    double radius2;
    // the maximum radius of all its successors (plus himself.)
    // i.e. the maximum radius all the half can reach.
    double suc_radius2;
    int apc; // accumulated photon count
    Vector3 arf; // accumulated reflected flux

    int inserted_num;
    int sx;
    int sy;
    short split_plane;
    Primitive* primitive;
    bool is_light;

    HitPoint();
    HitPoint(int x, int y, double init_w);
    ~HitPoint() {}
    void addPhoton(const Photon*);
    void update(double);
    void dump() const;
};

class HitPointMap {
    const int max_size;
    int size;
    std::mutex hit_points_mtx;
    HitPoint* hit_points;
    double alpha;
    Vector3 bbox_min;
    Vector3 bbox_max;
    void balanceSegment(HitPoint**, HitPoint**, const int index, const int start, const int end);
    void medianSplit(HitPoint**, const int start, const int end, const int median, const int axis);
    void locatePhoton(const Photon *, int);
    void updateMaxDist();
public:
    void store(const HitPoint&);
    void balance();
    void insertPhoton(const Photon&);
    void renderImage(ImgWriter*, Camera*, int, const char*);
    void setAlpha(double na) { alpha = na; }
    void update();
    int getSize() const { return size; }
    HitPointMap(int a_size);
    ~HitPointMap();
};

