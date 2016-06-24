#include <cmath>
#include <thread>
#include "photontracer.h"
#include "light.h"
#include "hitpointmap.h"

static const int MAX_PHOTONTRACE_DEPTH = 4;
static const double MAX_PHOTONTRACE_DIST = INF;

PhotonTracer::PhotonTracer() {
    scene = 0;
    photon_map = 0;
    hit_point_map = 0;
}

PhotonTracer::~PhotonTracer() {
    if (photon_map) delete photon_map;
}

void PhotonTracer::run() {
    std::vector<std::thread> vec_threads;
    for (int i = 0; i < MAX_THREAD_NUM; ++ i) {
        vec_threads.push_back(std::thread(&PhotonTracer::runSingleThread, this, i, rand()));
    }
    for (int i = 0; i < MAX_THREAD_NUM; ++ i) {
        vec_threads[i].join();
    }
}

void PhotonTracer::tracePhoton(const Ray& c_ray, Photon& photon, int depth) {
/* How to:
 * based on current ray. find collision.
 * if find: calc Russian Roulette.
 * Determine Diffuse, Reflection or Refraction.
 */
    if (depth == 0) return;

    PrimitiveCollision pcol = scene->getNearestPrimitiveCollision(c_ray, MAX_PHOTONTRACE_DIST);

    if (pcol.collide()) {
        Material* collide_material = &pcol.primitive->material;
        // TODO: may add depth > 1 to eliminate direct illumination
        if (collide_material->getDiffuse() > DOZ /*&& depth < MAX_PHOTONTRACE_DEPTH*/) {
            photon.direction = c_ray.direction.getNormal();
            photon.position = pcol.collision.pos;
            photon.primitive = pcol.primitive;
            if (photon_map) photon_map->store(photon);
            if (hit_point_map) hit_point_map->insertPhoton(photon);
        }
        double pointer = random01();
        Vector3 N = pcol.collision.normal.getNormal();
        Vector3 L = c_ray.direction.getNormal();
        if ((pointer -= (collide_material->getDiffuse() + collide_material->getSpecular())) < DOZ)
            diffusePhoton(pcol, photon, depth);
        else if ((pointer -= collide_material->getReflection()) < DOZ)
            reflectPhoton(N, L, c_ray, pcol, photon, depth);
        else if ((pointer -= collide_material->getRefraction()) < DOZ)
            refractPhoton(N, L, c_ray, pcol, photon, depth);
        // else -> photon absorbed, stop tracing
    }

}

void PhotonTracer::diffusePhoton(const PrimitiveCollision &pcol, Photon &photon, int depth) {
    photon.power *= Vector3(pcol.primitive->getColor(pcol.collision.pos), true).powerize();
    tracePhoton(Ray(pcol.collision.pos, pcol.collision.normal.getDiffuse()), photon, depth - 1);
}

void PhotonTracer::reflectPhoton(const Vector3& N, const Vector3& L,
                                 const Ray& c_ray, const PrimitiveCollision &pcol,
                                 Photon &photon, int depth) {
    photon.power *= Vector3(pcol.primitive->getColor(pcol.collision.pos), true).powerize();
    Vector3 R = L - 2.0f * (L * N) * N; //R = R.getNormal();
    tracePhoton(Ray(pcol.collision.pos, R), photon, depth - 1);
}

void PhotonTracer::refractPhoton(const Vector3& N, const Vector3& L,
                                 const Ray& c_ray, const PrimitiveCollision &pcol,
                                 Photon &photon, int depth) {
    Material* pm = &pcol.primitive->material;
    double rindex = pm->getRindex();
    if (pcol.collision.hit_type == Collision::OUTSIDE)
        rindex = 1.0f / rindex;
    double cosI = -N * L, cosT2 = 1 - ( rindex * rindex ) * ( 1 - cosI * cosI );
    Vector3 P;
    if ( cosT2 > DOZ ) {
        P = L * rindex + N * ( rindex * cosI - sqrt(cosT2));
        if (pcol.collision.hit_type == Collision::INSIDE) {
            Vector3 absorbance = Vector3(pm->getAbsorbance(), true) *
                                 pm->getDensity() * (-pcol.collision.distance);
            Vector3 transparency = Vector3(exp(absorbance.x), exp(absorbance.y), exp(absorbance.z));
            photon.power *= transparency.powerize();
        }
        tracePhoton(Ray(pcol.collision.pos, P), photon, depth - 1);
    }
}

void PhotonTracer::runSingleThread(int threadID, int randID) {
    srand(randID);
    double total_power = 0;
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        total_power += scene->getLightById(light_id)->getColor().getPower();
    }
    double photon_power = total_power / max_photons;
    /* photons are emitted by weight of their power
     * (actually their will be only one light)
     * then trace Photon
     */
    int emitted_photons = 0;
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        Light* this_light = scene->getLightById(light_id);
        double light_power = this_light->getColor().getPower() / MAX_THREAD_NUM;
        while (light_power > 0) {
            // photons are deleted in photon map.
            Photon photon;
            photon.power = Vector3(this_light->getColor(), true) / max_photons;
            tracePhoton(this_light->getPhotonRay(), photon, MAX_PHOTONTRACE_DEPTH);
            light_power -= photon_power;

            ++ emitted_photons;
            if ((emitted_photons & 131071) == 0 && (threadID == 0))
                printf("    - Emitted Photons = %d\n", emitted_photons * MAX_THREAD_NUM);
        }
    }
}

PhotonMap *PhotonTracer::getPhotonMap() {
    photon_map = new PhotonMap(max_photons * 4);
    photon_map->setNearestPhotonSize(200);
    run();
    photon_map->balance();
    return photon_map;
}