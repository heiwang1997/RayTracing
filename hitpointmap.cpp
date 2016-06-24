#include "hitpointmap.h"
#include "primitive.h"
#include "image.h"
#include "camera.h"
#include <cmath>
#include <cstring>
#include <algorithm>

HitPointMap::HitPointMap(int a_size) : max_size(a_size) {
    size = 0;
    alpha = 0.7f;
    hit_points = new HitPoint[max_size + 1];
    bbox_max = Vector3(-INF, -INF, -INF);
    bbox_min = Vector3(INF, INF, INF);
}

HitPointMap::~HitPointMap() {
    delete[] hit_points;
    //if (working_id) delete[] working_id;
}

void HitPointMap::store(const HitPoint &rv) {
    if (size + 1 > max_size) {
        printf("Hit Point Map Error: No more hit points could be added.\n");
        return;
    }
    hit_points_mtx.lock();
    hit_points[++size] = rv;
    // update bbox_max and bbox_min
    for (int i = 0; i < 3; ++ i) {
        bbox_max[i] = std::max(bbox_max.getAttr(i), rv.pos.getAttr(i));
        bbox_min[i] = std::min(bbox_min.getAttr(i), rv.pos.getAttr(i));
    }
    hit_points_mtx.unlock();
}

void HitPointMap::renderImage(ImgWriter *img, Camera *camera,
                              int iteration, const char* file_name) {
    int H = camera->getImgH();
    int W = camera->getImgW();
    Vector3** cmap = new Vector3*[H];
    for (int i = 0; i < H; ++ i)
        cmap[i] = new Vector3[W];

    double min_radius = INF;
    double factor = 0;
    Vector3 tgt_arf;
    for (int i = 1; i <= size; ++ i) {
        const HitPoint& hp = hit_points[i];
        Vector3 irradiance = (hp.is_light) ? Vector3(1, 1, 1) :
                             (80.0f / (hp.radius2 * iteration)) * hp.arf;
        if (!hp.is_light) {
            if (sqrt(hp.radius2) < min_radius) {
                min_radius = sqrt(hp.radius2);
                if (sqrt(hp.radius2) < DOZ) {
                    printf("Yes.\n");
                    printf("SX = %d, SY = %d\n", hp.sx, hp.sy);
                }
                factor = (80.0f / (hp.radius2 * iteration));
                tgt_arf = irradiance;
            }
        }
        cmap[hp.sy][hp.sx] += irradiance.scale(hp.weight);
    }
    printf("Min Radius = %lf\n", min_radius);
    printf("Factor = %lf\n", factor);
    tgt_arf.dump();
    for (int i = 0; i < H; ++ i)
        for (int j = 0; j < W; ++ j)
            img->cache(j, i, Color(cmap[i][j]));
    for (int i = 0; i < H; ++ i)
        delete[] cmap[i];
    delete[] cmap;
    img->saveToFile(file_name);
}

void HitPointMap::update() {
    for (int i = 1; i <= size; ++ i)
        hit_points[i].update(alpha);
    updateMaxDist();
}

void HitPointMap::balance() {
    HitPoint** pa1 = new HitPoint*[size + 1];
    HitPoint** pa2 = new HitPoint*[size + 1];

    for (int i = 1; i <= size; ++ i)
        pa2[i] = &hit_points[i];

    // median split prog_arr, then heap-copy it into photons array
    balanceSegment(pa1, pa2, 1, 1, size);
    delete[] pa2;

    int d, j = 1, foo = 1;
    HitPoint temp_hp = hit_points[j];
    for (int i = 1; i <= size; ++ i) {
        d = int(pa1[j] - hit_points);
        pa1[j] = 0;
        if (d != foo)
            hit_points[j] = hit_points[d];
        else {
            hit_points[j] = temp_hp;
            if (i < size) {
                for (; foo <= size; ++ foo)
                    if (pa1[foo] != 0) break;
                temp_hp = hit_points[foo];
                j = foo;
            }
            continue;
        }
        j = d;
    }
    delete[] pa1;
    updateMaxDist();
}

void HitPointMap::balanceSegment(HitPoint** pbal, HitPoint** porg, const int index,
                                 const int start, const int end) {
    // compute new median
    int median = 1;
    while ((median * 4) <= (end - start + 1))
        median <<= 1;
    if ((3 * median) <= (end - start + 1)) {
        median <<= 1;
        median += start - 1;
    } else
        median = end - median + 1;

    // find axis to split along.
    double delx = bbox_max[0] - bbox_min[0],
           dely = bbox_max[1] - bbox_min[1],
           delz = bbox_max[2] - bbox_min[2];
    short axis = 2;
    if (delx > dely && delx > delz)
        axis = 0;
    else if (dely > delz)
        axis = 1;

    // median split around median with axis.
    // This is one logN pass of Partition Algorithm
    medianSplit(porg, start, end, median, axis);
    pbal[index] = porg[median];
    pbal[index]->split_plane = axis;

    // balance left and right block
    if (median > start) {
        if (start < median - 1) {
            double temp = bbox_max[axis];
            bbox_max[axis] = pbal[index]->pos[axis];
            balanceSegment(pbal, porg, 2 * index, start, median - 1);
            bbox_max[axis] = temp;
        } else {
            pbal[2 * index] = porg[start];
        }
    }

    if (median < end) {
        if (median + 1 < end) {
            double temp = bbox_min[axis];
            bbox_min[axis] = pbal[index]->pos[axis];
            balanceSegment(pbal, porg, 2 * index + 1, median + 1, end);
            bbox_min[axis] = temp;
        } else {
            pbal[2 * index + 1] = porg[end];
        }
    }
}

void HitPointMap::medianSplit(HitPoint **porg, const int start,
                              const int end, const int median, const int axis) {
    int left = start, right = end;
    while (right > left) {
        double v = porg[right]->pos[axis];
        int i = left - 1;
        int j = right;
        while (true) {
            while (porg[++i]->pos[axis] < v);
            while (porg[--j]->pos[axis] > v && j > left);
            if (i >= j) break;
            std::swap(porg[i], porg[j]);
        }
        std::swap(porg[i], porg[right]);
        if (i >= median) right = i - 1;
        if (i <= median) left = i + 1;
    }
}

// Use the pos of the photon to find if a hit point
// contains this photon, if so, store it, else abandon it.
void HitPointMap::locatePhoton(const Photon * ph, int index) {
    HitPoint* h = hit_points + index;
    double sigma;
    if ((index << 1) <= size) {
        sigma = ph->position.getAttr(h->split_plane) - h->pos[h->split_plane];
        if (sigma > 0) {
            locatePhoton(ph, (index << 1) + 1);
            if (sigma * sigma < hit_points[index << 1].suc_radius2)
                locatePhoton(ph, (index << 1));
        } else {
            locatePhoton(ph, (index << 1));
            if ((index << 1) + 1 <= size && /* pre judge */
                    sigma * sigma < hit_points[(index << 1) + 1].suc_radius2)
                locatePhoton(ph, (index << 1) + 1);
        }
    }

    double pdist2 = (h->pos - ph->position).getDist2();
    if (pdist2 < h->radius2)
        h->addPhoton(ph);
}

void HitPointMap::insertPhoton(const Photon &ph) {
    locatePhoton(&ph, 1);
}

void HitPointMap::updateMaxDist() {
    // DP: from bottom to top.
    static double lr2, rr2, sr2;
    for (int i = size; i >= 1; -- i) {
        if (i > size / 2) {
            hit_points[i].suc_radius2 = hit_points[i].radius2;
            continue;
        }
        lr2 = hit_points[(i << 1)].suc_radius2;
        rr2 = ((i << 1) + 1 <= size) ? hit_points[(i << 1) + 1].suc_radius2 : -INF;
        sr2 = hit_points[i].radius2;
        hit_points[i].suc_radius2 = std::max(std::max(lr2, rr2), sr2);
    }
}

void HitPoint::addPhoton(const Photon *ph) {
    //static double k = 1.1f;
    //static double base = 1 - 2 / (3 * k);
    // update flux
    if (ph->direction * normal < 0.0f && !this->is_light) {
        //double hdist = (ph->position - pos).getLength();
        //double wpc = 1 - (hdist / (k * sqrt(radius2)));
        if (ph->primitive->getPMComparer() == this->primitive->getPMComparer()) {
            arf += ph->power;
            ++ inserted_num;
        }
    }
}

HitPoint::HitPoint(int x, int y, double init_w) {
    weight = Vector3(init_w, init_w, init_w);
    apc = 0;
    sx = x; sy = y;
    inserted_num = 0;
    primitive = 0;
    radius2 = 0;
    is_light = false;
}

HitPoint::HitPoint() {
    weight = Vector3(1, 1, 1);
    apc = 0;
    inserted_num = 0;
    primitive = 0;
    radius2 = 0;
    is_light = false;
}

void HitPoint::update(double alpha) {
    if (inserted_num == 0) return;
    double s_factor = (apc + alpha * inserted_num) / (apc + inserted_num);
    radius2 = radius2 * s_factor;
    arf = arf * s_factor;
    apc += alpha * inserted_num;
    inserted_num = 0;
}

void HitPoint::dump() const {
    printf("== HitPoint : x = %d, y = %d\n", sx, sy);
    printf("radius2 = %lf\n", radius2);
    printf("radius2 = %lf\n", radius2);
    printf("arf = ");
    arf.dump();
}
