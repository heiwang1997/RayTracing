#include "raytracer.h"
#include "camera.h"
#include "scene.h"
#include "photontracer.h"
#include <cmath>
#include <thread>
#include <mutex>

#include <iostream>

using namespace std;

static const int MAX_RAYTRACE_DEPTH = 20;
static const int DIFFUSE_REFLECTION_DEPTH = 1;
static const double IMPORTANCE_SAMPLING_INDEX = 0.25f;
static const double RAYTRACE_MAX_DISTANCE = INF;
static const int SHADOW_HASH = 199738;
static const int MAX_THREAD = 4;

RayTracer::RayTracer() : img(0) {
    camera = new Camera;
    scene = new Scene;
    tracer = 0;
    indirect_illumination = false;
}

void RayTracer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}

void RayTracer::setImgLoader(ImgLoader *p_iml) {
    Texture::imgLoader = p_iml;
}

void RayTracer::loadSceneFromFile(const std::string &filename) {
    generateCommentlessFile(filename);
    FILE* fp;
    fopen_s(&fp, "temp_RayTracer.txt", "r");
    while (true) {
        std::string attr_name = getAttrName(fp);
        if (attr_name == "EOF") break;
        if (attr_name == "CAMERA") camera->loadAttr(fp);
        else if (attr_name == "SCENE") scene->loadAttr(fp);
    }
    fclose(fp);
}

RayTracer::~RayTracer() {
    if (scene) delete scene;
    if (camera) delete camera;
}


Camera *RayTracer::getCamera() const {
    return camera;
}

void RayTracer::run() {
    if (!img) return;

    if (indirect_illumination) {
        tracer = new PhotonTracer;
        tracer->setScene(scene);
        tracer->setMaxPhotons(50000);
        tracer->run();
        photonMap = tracer->getPhotonMap();
        photonMap->balance();
        photonMap->setNearestPhotonSize(100);
    }


    int H = camera->getImgH();
    int W = camera->getImgW();
    int** primitive_table = new int*[H];
    for (int i = 0; i < H; ++ i)
        primitive_table[i] = new int[W];
    for (int cy = 0; cy < H; ++cy) {
        for (int cx = 0; cx < W; ++cx) {
            dsx = cx; dsy = cy;
            Ray current_ray = camera->emit((double) cx, (double) cy);
            primitive_table[cy][cx] = 0;

            int dof_sample = scene->getDOFSample();
            if (dof_sample == 0) {
                img->cache(cx, cy, traceRay(current_ray, MAX_RAYTRACE_DEPTH,
                                            primitive_table[cy][cx], scene->getLightSample(), 0));
            } else {
                Color dof_final;
                double dof_weight = 1.0f / dof_sample;
                int dummy_dof;
                for (int i = 0; i < dof_sample; ++ i) {
                    dof_final += traceRay(camera->emitDOF(current_ray), MAX_RAYTRACE_DEPTH,
                                          dummy_dof, scene->getLightSample(), 0) * dof_weight;
                }
                img->cache(cx, cy, dof_final);
            }

        }
    }

    int dummy_hash = 0;
    for (int cy = 0; cy < H; ++cy) {
        for (int cx = 0; cx < W; ++cx) {
            if ((cx == 0 || primitive_table[cy][cx] == primitive_table[cy][cx - 1]) &&
                (cy == 0 || primitive_table[cy][cx] == primitive_table[cy - 1][cx]) &&
                (cx == W - 1 || primitive_table[cy][cx] == primitive_table[cy][cx + 1]) &&
                (cy == H - 1 || primitive_table[cy][cx] == primitive_table[cy + 1][cx])) continue;
            Color new_color;
            double q_index = 1.0f / (SUPER_SAMPLING_QUALITY * SUPER_SAMPLING_QUALITY);
            for (int i = 0; i < SUPER_SAMPLING_QUALITY; ++ i)
                for (int j = 0; j < SUPER_SAMPLING_QUALITY; ++ j) {
                    double tx = cx + i / (double)SUPER_SAMPLING_QUALITY,
                           ty = cy + j / (double)SUPER_SAMPLING_QUALITY;
                    Ray current_ray = camera->emit(tx, ty);
                    new_color += traceRay(current_ray, MAX_RAYTRACE_DEPTH,
                                          dummy_hash, scene->getLightSample(), 0) * q_index;
                }
            img->cache(cx, cy, new_color);
        }
    }
    for (int i = 0; i < H; ++ i)
        delete[] primitive_table[i];
    delete[] primitive_table;

    if (tracer) delete tracer;
    tracer = 0;
}

Color RayTracer::getBasicPhongColor(Primitive *prim, const Collision& c_col,
                                    const Vector3& N, const Vector3& V, int shade_sample, int& hash) {
    Color result_color;
    Color primitive_color = prim->getColor(c_col.pos);
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        Light* light = scene->getLightById(light_id);
        double shade = 1.0f;
        if (scene->getLightSample() != 0 && !indirect_illumination) {
            shade = getShadow(light, shade_sample, c_col.pos + V * DOZ);
            if (shade < 1.0f - DOZ && !scene->getSoftShadow())
                hash = (hash + SHADOW_HASH) % HASH_MOD;
        }
        Vector3 L = (light->getPos() - c_col.pos).getNormal();
        if (prim->material.getDiffuse() > DOZ && (N * L) > DOZ) {
            result_color += (N * L * prim->material.getDiffuse()) *
                    light->getColor() * primitive_color;
        }
        if (prim->material.getSpecular() > DOZ) {
            Vector3 H = (L + V).getNormal();
            double spec_dot = N * H;
            if (spec_dot > DOZ) {
                result_color += (pow(spec_dot, prim->material.getShineness()) *
                        prim->material.getSpecular()) *
                        light->getColor();
            }
        }
        result_color += primitive_color * prim->material.getAmbient();
        result_color = result_color * shade;
    }
    if (indirect_illumination) {
        result_color += primitive_color.scale(photonMap->getIrradiance(c_col.pos, c_col.normal, 3.0f))
                        * prim->material.getDiffuse();
    }
    return result_color;
}

double RayTracer::getShadow(Light *light, int sample, const Vector3& cpos) {

    if (scene->getSoftShadow()) {
        double result = 0.0f;
        double delta = 1.0f / (sample * sample);
        for (int i = 0; i < sample; ++ i)
            for (int j = 0; j < sample; ++ j) {
                Vector3 light_pos = light->getPos(i, j, sample);
                Vector3 L = light_pos - cpos;
                double t_dist = L.getLength();
                L = L * (1.0f / t_dist);
                if (!scene->interceptTest(Ray(cpos + L * DOZ, L), t_dist))
                    result += delta;
            }
        return result;
    } else {
        Vector3 L = light->getPos() - cpos;
        bool intercept = scene->interceptTest(Ray(cpos + L * DOZ, L),
                                              L.getLength());
        return intercept ? 0 : 1;
    }
}

Color RayTracer::getReflectionColor(Primitive* prim, int depth,
                                    const Vector3& R, const Collision& col,
                                    int& hash, int shade_sample) {
    if (col.hit_type == Collision::INSIDE) return Color();
    Color ref_col;
    if (prim->material.getDiffuseReflection() > DOZ &&
        scene->getDiffuseReflectionSample() != 0 &&
        depth >= MAX_RAYTRACE_DEPTH - DIFFUSE_REFLECTION_DEPTH) {
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
            ref_col += traceRay(Ray(col.pos + RF * DOZ, RF), depth - 1,
                                dum_hash, int(shade_sample * IMPORTANCE_SAMPLING_INDEX), prim) * r_weight;
        }
    } else
        ref_col = traceRay(Ray(col.pos + R * DOZ, R), depth - 1, hash, shade_sample, prim);

    return prim->material.getReflection() * ref_col *
              prim->getColor(col.pos);
}

Color RayTracer::getRefractionColor(Primitive* prim, const Vector3& N, const Vector3& L,
                                    const Collision& col, int depth, int& hash, int shade_sample) {
    Color result;
    double rindex = prim->material.getRindex();
    if (col.hit_type == Collision::OUTSIDE)
        rindex = 1.0f / rindex;
    double cosI = -N * L, cosT2 = 1 - ( rindex * rindex ) * ( 1 - cosI * cosI );
    Vector3 P;
    if ( cosT2 > DOZ ) {
        P = L * rindex + N * ( rindex * cosI - sqrt(cosT2));
        Color refract_col = traceRay(Ray(col.pos + P * DOZ, P), depth - 1, hash, shade_sample, prim) *
                            prim->material.getRefraction();
        if (col.hit_type == Collision::OUTSIDE)
            result += refract_col;
        else {
            Vector3 absorbance = Vector3(prim->material.getAbsorbance(), false) *
                                    prim->material.getDensity() * (-col.distance);
            Color transparency = Color(exp(absorbance.x / 255) * 255,
                                        exp(absorbance.y / 255) * 255,
                                        exp(absorbance.z / 255) * 255);
            result += refract_col * transparency;
        }
    } else {
        Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();
        return traceRay(Ray(col.pos + R * DOZ, R), depth - 1, hash, shade_sample, prim);
    }
    return result;
}

Color RayTracer::traceRay(const Ray &current_ray, int depth, int& hash, int shade_sample, Primitive*) {
    if (depth == 0)
        return scene->getBackgroundColor();

    PrimitiveCollision pcol = scene->getNearestPrimitiveCollision(current_ray, RAYTRACE_MAX_DISTANCE);

    if (!scene->hasEnvironmentMap()) {
        LightCollision lcol = scene->getNearestLightCollision(current_ray, RAYTRACE_MAX_DISTANCE);
        if (lcol.collide() && ((pcol.collide() &&
                                lcol.collision.distance < pcol.collision.distance) ||
                               !pcol.collide())) {
            hash = (hash + lcol.light->getHashCode()) % HASH_MOD;
            return lcol.light->getColor();
        }
    }

    if (pcol.collide()) {
        Color result;

        Collision p_collision = pcol.collision;
        Primitive* collide_primitive = pcol.primitive;

        Vector3 N = p_collision.normal.getNormal();
        Vector3 L = current_ray.direction.getNormal();
        Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();

        result += getBasicPhongColor(collide_primitive, p_collision, N, -L, shade_sample, hash);
        if (collide_primitive->material.getReflection() > DOZ) {
            result += getReflectionColor(collide_primitive, depth, R, p_collision, hash, shade_sample);
        }
        if (collide_primitive->material.getRefraction() > DOZ) {
            result += getRefractionColor(collide_primitive, N, L, p_collision, depth, hash, shade_sample);
        }

        hash = (hash + collide_primitive->getHashCode()) % HASH_MOD;

        return result;
    }

    return scene->getEnvironmentTexture(current_ray.direction);
}


