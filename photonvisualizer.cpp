#include "photonvisualizer.h"
#include "photontracer.h"
#include "camera.h"
#include "light.h"
#include <thread>
#include <iostream>
#include <cmath>

using namespace std;

static const int MAX_PMV_DEPTH = 8;
static const int PMV_DIFFUSE_REFLECTION_DEPTH = 1;

PhotonVisualizer::PhotonVisualizer() : img(0) {
    camera = new Camera;
    scene = new Scene;
    tracer = new PhotonTracer;
    projectName = "New Photon Trace Project";
}

void PhotonVisualizer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}

void PhotonVisualizer::setImgLoader(ImgLoader *p_iml) {
    Texture::imgLoader = p_iml;
}

void PhotonVisualizer::loadSceneFromFile(const std::string &filename) {
    generateCommentlessFile(filename);
    FILE* fp = fopen("temp_RayTracer.txt", "r");
    while (true) {
        std::string attr_name = getAttrName(fp);
        if (attr_name == "EOF") break;
        if (attr_name == "CAMERA") camera->loadAttr(fp);
        else if (attr_name == "SCENE") scene->loadAttr(fp);
    }
    fclose(fp);
}

void PhotonVisualizer::setProjectName(const std::string &pro_name) {
    projectName = pro_name;
}

PhotonVisualizer::~PhotonVisualizer() {
    if (scene) delete scene;
    if (camera) delete camera;
    if (tracer) delete tracer;
}

Camera *PhotonVisualizer::getCamera() const {
    return camera;
}

void PhotonVisualizer::run() {
    if (!img) return;

    // PPM-PASS 1: Ray Trace
    printf("===============================\n");
    printf("     PHOTON VISUALIZER v1.0    \n");
    printf("===============================\n");
    printf(" PROJECT: %s\n", projectName.c_str());

    printf("+ PPM-PASS1: Ray Tracing...\n");
    clock_t start_time;
    start_time = clock();

    int H = camera->getImgH();
    int W = camera->getImgW();
    int** primitive_table = new int*[H];
    for (int i = 0; i < H; ++ i) {
        primitive_table[i] = new int[W];
        for (int j = 0; j < W; ++ j) {
            primitive_table[i][j] = 0;
        }
    }
    int hm_size = H * W * camera->getHitpointmapCoef();
    hitPointMap = new HitPointMap(hm_size);
    tracer->setScene(scene);
    tracer->setMaxPhotons(camera->getPhotonsEmitted());
    hitPointMap->setAlpha(camera->getPPMAlpha());
    //photonMap = tracer->getPhotonMap();

    std::vector<std::thread> vec_threads;
    for (int i = 0; i < MAX_RAYTRACE_THREAD_NUM; ++ i) {
        vec_threads.push_back(std::thread(&PhotonVisualizer::singleThreadRowSampling,
            this, i, primitive_table));
    }
    for (int i = 0; i < MAX_RAYTRACE_THREAD_NUM; ++ i) {
        vec_threads[i].join();
    }

    std::vector<std::thread> vec_resample_threads;
    for (int i = 0; i < MAX_RAYTRACE_THREAD_NUM; ++ i) {
        vec_resample_threads.push_back(std::thread(&PhotonVisualizer::singleThreadRowResampling,
            this, i, primitive_table));
    }
    for (int i = 0; i < MAX_RAYTRACE_THREAD_NUM; ++ i) {
        vec_resample_threads[i].join();
    }

    for (int i = 0; i < H; ++ i)
        delete[] primitive_table[i];
    delete[] primitive_table;
    printf("+ Hit Point Map Balancing, Size = %d\n", hitPointMap->getSize());
    hitPointMap->balance();
    //tracer->clearPhotonMap();

    printf("+ Pass1 Done: time elapsed = %lf s\n", double(clock() - start_time) / CLOCKS_PER_SEC);

    // PPM-PASS 2: Photon Trace
    printf("+ PPM-PASS2: Photon Tracing...\n");
    tracer->setHitPointMap(hitPointMap);

    for (int i = 0; i < camera->getPPMIterations(); ++ i) {
        printf("+ Round = %d\n", i + 1);
        start_time = clock();
        tracer->run();
        hitPointMap->update();
        printf("+ Round %d done: time elapsed = %lf s\n", i + 1, 
            double(clock() - start_time) / CLOCKS_PER_SEC);
        printf("+ PPM: Rendering Image For Round = %d\n", i + 1);
        hitPointMap->renderImage(img, camera, i + 1, (projectName + ".bmp").c_str());
    }

//    printf("PPM: Rendering Image\n");
//    hitPointMap->renderImage(img, camera);

    delete hitPointMap;
    hitPointMap = 0;
}

void PhotonVisualizer::traceDiffuse(Primitive *prim, const Collision &c_col,
                                    const Vector3& N, const Vector3&,
                                    HitPoint hp) {
    Color primitive_color = prim->getColor(c_col.pos);
    // Store in HitPointMap.
    hp.pos = c_col.pos;
    hp.normal = N;
    hp.weight *= Vector3(primitive_color, true) * prim->material.getDiffuse();
    hp.primitive = prim;
//    hp.dir = L;
    hp.radius2 = camera->getInitRadius() * camera->getInitRadius();
    hitPointMap->store(hp);
    return;
}

void PhotonVisualizer::traceReflect(Primitive *prim, int depth, const Vector3 &R,
                                     const Collision &col, int &hash, HitPoint hp) {
    if (col.hit_type == Collision::INSIDE) return;
    hp.weight = hp.weight * prim->material.getReflection();
    hp.weight *= Vector3(prim->getColor(col.pos), true);
    if (prim->material.getDiffuseReflection() > DOZ &&
        scene->getDiffuseReflectionSample() != 0 &&
        depth >= MAX_PMV_DEPTH - PMV_DIFFUSE_REFLECTION_DEPTH) {
        Vector3 RN1 = Vector3(R.z, R.y, -R.x);
        Vector3 RN2 = R.cross(RN1);
        double dref_scale = prim->material.getDiffuseReflection();
        double r_weight = 1.0f / scene->getDiffuseReflectionSample();
        for (int i = 0; i < scene->getDiffuseReflectionSample(); ++ i) {
            double offx, offy;
            do {
                offx = random01() * 2 - 1;
                offy = random01() * 2 - 1;
            } while (offx * offx + offy * offy > dref_scale * dref_scale);
            Vector3 RF = R + offx * RN1 + offy * RN2;
            int dum_hash = 0;
            hp.weight = hp.weight * r_weight;
            traceRay(Ray(col.pos + RF * DOZ, RF), depth - 1, dum_hash, hp);
        }
    } else
        traceRay(Ray(col.pos + R * DOZ, R), depth - 1, hash, hp);

}

void PhotonVisualizer::traceRefract(Primitive *prim, const Vector3 &N,
                                     const Vector3 &L, const Collision &col, int depth,
                                     int &hash, HitPoint hp) {
    double rindex = prim->material.getRindex();
    if (col.hit_type == Collision::OUTSIDE)
        rindex = 1.0f / rindex;
    double cosI = -N * L, cosT2 = 1 - ( rindex * rindex ) * ( 1 - cosI * cosI );
    Vector3 P;
    if ( cosT2 > DOZ ) {
        P = L * rindex + N * ( rindex * cosI - sqrt(cosT2));
        hp.weight = hp.weight * prim->material.getRefraction();

        if (col.hit_type == Collision::INSIDE) {
            Vector3 absorbance = Vector3(prim->material.getAbsorbance(), false) *
                                 prim->material.getDensity() * (-col.distance);
            Vector3 transparency = Vector3(exp(absorbance.x), exp(absorbance.y), exp(absorbance.z));
            hp.weight *= transparency;
        }
        traceRay(Ray(col.pos + P * DOZ, P), depth - 1, hash, hp);
    }
}

void PhotonVisualizer::traceRay(const Ray &current_ray, int depth,
                                 int &hash, HitPoint hp) {
    if (depth == 0)
        return;

    PrimitiveCollision pcol = scene->getNearestPrimitiveCollision(current_ray, INF);

    if (!scene->hasEnvironmentMap()) {
        LightCollision lcol = scene->getNearestLightCollision(current_ray, INF);
        if (lcol.collide() && ((pcol.collide() &&
                                lcol.collision.distance < pcol.collision.distance) ||
                               !pcol.collide())) {
            hash = (hash + lcol.light->getHashCode()) % HASH_MOD;
            hp.pos = lcol.collision.pos;
            hp.normal = Vector3(0, 0, -1);
            hp.weight *= Vector3(lcol.light->getColor(), true);
            hp.is_light = true;
            hitPointMap->store(hp);
            return;
        }
    }

    if (pcol.collide()) {
        Collision p_collision = pcol.collision;
        Primitive* collide_primitive = pcol.primitive;

        Vector3 N = p_collision.normal.getNormal();
        Vector3 L = current_ray.direction.getNormal();
        Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();

        if (collide_primitive->material.getDiffuse() + collide_primitive->material.getSpecular() > DOZ)
            traceDiffuse(collide_primitive, p_collision, N, L, hp);
        if (collide_primitive->material.getReflection() > DOZ)
            traceReflect(collide_primitive, depth, R, p_collision, hash, hp);
        if (collide_primitive->material.getRefraction() > DOZ)
            traceRefract(collide_primitive, N, L, p_collision, depth, hash, hp);

        hash = (hash + collide_primitive->getHashCode()) % HASH_MOD;
        return;
    }

    if (scene->hasEnvironmentMap()) {
        hp.pos = Vector3(0, 0, 0);
        hp.weight *= Vector3(scene->getEnvironmentTexture(current_ray.direction), true);
        hp.is_light = true;
        hitPointMap->store(hp);
    }

}

void PhotonVisualizer::singleThreadRowSampling(int base, int** primitive_table) {
    if (base == 0)
        printf("    - Pass 1: Sampling\n");
    for (int cy = base; cy < camera->getImgH(); cy += MAX_RAYTRACE_THREAD_NUM) {
        for (int cx = 0; cx < camera->getImgW(); ++cx) {
            Ray current_ray = camera->emit((double) cx, (double) cy);
            HitPoint hit_point(cx, cy, 1);
            traceRay(current_ray, MAX_PMV_DEPTH, primitive_table[cy][cx], hit_point);
        }
    }
}

void PhotonVisualizer::singleThreadRowResampling(int base, int** primitive_table) {
    if (base == 0)
        printf("    - Pass 2: Antialiasing\n");

    int dummy_hash = 0;
    int W = camera->getImgW();
    int H = camera->getImgH();
    for (int cy = base; cy < H; cy += MAX_RAYTRACE_THREAD_NUM) {
        for (int cx = 0; cx < W; ++cx) {
            if ((cx == 0 || primitive_table[cy][cx] == primitive_table[cy][cx - 1]) &&
                (cy == 0 || primitive_table[cy][cx] == primitive_table[cy - 1][cx]) &&
                (cx == W - 1 || primitive_table[cy][cx] == primitive_table[cy][cx + 1]) &&
                (cy == H - 1 || primitive_table[cy][cx] == primitive_table[cy + 1][cx])) continue;
            double q_index = 1.0f / (SUPER_SAMPLING_QUALITY * SUPER_SAMPLING_QUALITY);
            HitPoint rhp(cx, cy, -1);
            traceRay(camera->emit((double) cx, (double) cy), MAX_PMV_DEPTH,
                dummy_hash, rhp);
            for (int i = 0; i < SUPER_SAMPLING_QUALITY; ++ i)
                for (int j = 0; j < SUPER_SAMPLING_QUALITY; ++ j) {
                    double tx = cx + i / (double)SUPER_SAMPLING_QUALITY,
                        ty = cy + j / (double)SUPER_SAMPLING_QUALITY;
                    Ray current_ray = camera->emit(tx, ty);
                    HitPoint hit_point(cx, cy, q_index);
                    traceRay(current_ray, MAX_PMV_DEPTH, dummy_hash, hit_point);
                }
        }
    }
}

