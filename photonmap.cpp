#include <cstring>
#include "photonmap.h"

void PhotonMap::store(const Photon &ph) {
    if (size + 1 > max_size) {
        printf("Error: Number of photons exceeds limit.\n");
        return;
    }
    photons[++size] = ph; // operator =
    // update bbox_max and bbox_min
    for (int i = 0; i < 3; ++ i) {
        bbox_max[i] = std::max(bbox_max.getAttr(i), ph.position.getAttr(i));
        bbox_min[i] = std::min(bbox_min.getAttr(i), ph.position.getAttr(i));
    }
}

PhotonMap::PhotonMap(int a_size) : max_size(a_size) {
    size = 0;
    photons = new Photon[max_size + 1];
    bbox_max = Vector3(-INF, -INF, -INF);
    bbox_min = Vector3(INF, INF, INF);
}

void PhotonMap::balance() {
    Photon* porg_arr = new Photon[size + 1];
    memcpy(porg_arr, photons, sizeof(Photon) * (size + 1));

    // median split prog_arr, then heap-copy it into photons array
    balanceSegment(porg_arr, 1, 1, size);
    delete[] porg_arr;
}

// close segment [start, end]
// algorithm from "Realist Image Synthesis Using PM."
void PhotonMap::balanceSegment(Photon *porg, const int index,
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
    porg[median].split_plane = axis;
    photons[index] = porg[median];

    // balance left and right block
    if (median > start) {
        if (start < median - 1) {
            double temp = bbox_max[axis];
            bbox_max[axis] = photons[index].position[axis];
            balanceSegment(porg, 2 * index, start, median - 1);
            bbox_max[axis] = temp;
        } else {
            photons[2 * index] = porg[start];
        }
    }

    if (median < end) {
        if (median + 1 < end) {
            double temp = bbox_min[axis];
            bbox_min[axis] = photons[index].position[axis];
            balanceSegment(porg, 2 * index + 1, median + 1, end);
            bbox_min[axis] = temp;
        } else {
            photons[2 * index + 1] = porg[end];
        }
    }

}

void PhotonMap::medianSplit(Photon *porg, const int start, const int end,
                            const int median, const int axis) {
    int left = start, right = end;
    while (right > left) {
        double v = porg[right].position[axis];
        int i = left - 1;
        int j = right;
        while (true) {
            while (porg[++i].position[axis] < v);
            while (porg[--j].position[axis] > v && j > left);
            if (i >= j) break;
            std::swap(porg[i], porg[j]);
        }
        std::swap(porg[i], porg[right]);
        if (i >= median) right = i - 1;
        if (i <= median) left = i + 1;
    }
}

void PhotonMap::locatePhotons(NearestPhotons *np, int index) {
    Photon* p = photons + index;
    double sigma;
    if (index * 2 <= size) {
        sigma = np->position[p->split_plane] - p->position[p->split_plane];
        if (sigma > 0) {
            locatePhotons(np, 2 * index + 1);
            if (sigma * sigma < np->getMaxDist2())
                locatePhotons(np, 2 * index);
        } else {
            locatePhotons(np, 2 * index);
            if (sigma * sigma < np->getMaxDist2())
                locatePhotons(np, 2 * index + 1);
        }
    }
    // if heepified (means full), pdist2 tunes the distance.
    double pdist2 = (np->position - p->position).getDist2();
    if (pdist2 < np->getMaxDist2())
        np->insertPhoton(p, pdist2);

}

Vector3 PhotonMap::getIrradiance(Vector3 pos, Vector3 normal,
                                 double max_dist) {
    NearestPhotons np(nearest_photon_size);
    np.setConstrain(max_dist * max_dist); // restrict max dist for np.
    np.position = pos;
    locatePhotons(&np, 1);
    if (np.size <= 8) // too few photons which are simple noise.
        return Vector3();
    Vector3 irradiance;
    for (int i = 0; i < np.size; ++ i) {
        if (np.data[i]->direction * normal < 0.0f) // Lambertian Surface's BRDF
            irradiance += np.data[i]->power;       // This is precomputed
    }
    return (60.0f / np.getMaxDist2()) * irradiance;
}

double PhotonMap::getInitialRadius2(Vector3 pos, double max_dist) {
    NearestPhotons np(nearest_photon_size);
    np.setConstrain(max_dist * max_dist); // restrict max dist for np.
    np.position = pos;
    locatePhotons(&np, 1);
    return np.getMaxDist2();
}


NearestPhotons::NearestPhotons(int msize) : max_size(msize) {
    size = 0;
    is_heap = false;
    data = new Photon*[max_size];
    dist2 = new double[max_size];
}

NearestPhotons::~NearestPhotons() {
    delete[] data;
    delete[] dist2;
}

void NearestPhotons::insertPhoton(Photon *ph, double dist) {
    if (is_heap)
        heapInsert(ph, dist);
    else {
        dist2[size] = dist;
        data[size++] = ph;
        if (size == max_size) {
            buildHeap();
            is_heap = true;
        }
    }
}

void NearestPhotons::buildHeap() {
    for (int i = (max_size / 2) - 1; i >= 0; -- i)
        percolateDown(i);
}

void NearestPhotons::heapInsert(Photon *ph, double dist) {
    data[0] = ph;
    dist2[0] = dist;
    percolateDown(0);
}

double NearestPhotons::getMaxDist2() {
    if (is_heap) return dist2[0];
    return max_dist2;
}

void NearestPhotons::percolateDown(int index) {
    int insert_pos = index;
    double dist = dist2[index];
    Photon* ori_ph = data[index];
    while ((insert_pos << 1) + 1 < max_size) {
        int cc = (insert_pos << 1) + 1;
        if (cc + 1 < max_size && dist2[cc] < dist2[cc + 1]) ++ cc;
        if (dist >= dist2[cc]) break;
        data[insert_pos] = data[cc];
        dist2[insert_pos] = dist2[cc];
        insert_pos = cc;
    }
    data[insert_pos] = ori_ph;
    dist2[insert_pos] = dist;
}



