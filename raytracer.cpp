#include "raytracer.h"
#include "camera.h"
#include "scene.h"
#include "image.h"
#include <cmath>

#include <iostream>
#include <cstdlib>

using namespace std;

RayTracer::RayTracer() : img(0) {
    camera = new Camera;
    scene = new Scene;
}

void RayTracer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}

void RayTracer::loadSceneFromFile(const std::string &filename) {
    FILE* fp;
    fopen_s(&fp, filename.data(), "r");
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
    for (int cy = 0; cy < camera->getImgH(); ++cy) {
        for (int cx = 0; cx < camera->getImgW(); ++cx) {
            dsx = cx; dsy = cy;
            Ray current_ray = camera->emit((double) cx, (double) cy);
            img->cache(cx, cy, traceRay(current_ray, 20));
        }
    }
}

Color RayTracer::getBasicPhongColor(Primitive *prim, const Collision& c_col,
                                    const Vector3& N, const Vector3& V) {
    Color result_color;
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        Light* light = scene->getLightById(light_id);
        double shade = 1.0f;
        if (scene->getLightSample() != 0) shade = getShadow(prim, light);
        Vector3 L = (light->getPos() - c_col.pos).getNormal();
        if (prim->material.getDiffuse() > DOZ && (N * L) > DOZ) {
            result_color += (N * L * prim->material.getDiffuse()) *
                    light->getColor() * prim->material.getColor();
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
        result_color += prim->material.getColor() * prim->material.getAmbient();
        result_color = result_color * shade;
    }
    return result_color;
}

double RayTracer::getShadow(Primitive *prim, Light *light) {
    int sample = scene->getLightSample();
    double result = 0.0f;
    double delta = 1.0f / (sample * sample);
    for (int i = 0; i < sample; ++ i)
        for (int j = 0; j < sample; ++ j) {
            Vector3 light_pos = light->getPos(i, j, sample);
            Vector3 L = light_pos - prim->collision.pos;
            double t_dist = L.getLength();
            L = L * (1.0f / t_dist);
            if (!scene->interceptTest(Ray(prim->collision.pos + L * DOZ, L), t_dist))
                result += delta;
        }
    return result;
}

Color RayTracer::getReflectionColor(Primitive* prim, int depth,
                                    const Vector3& R, const Collision& col) {
    Color ref_col;
    if (prim->material.getDiffuseReflection() > DOZ &&
        scene->getDiffuseReflectionSample() != 0 &&
        depth > 18) {
        Vector3 RN1 = Vector3(R.z, R.y, -R.x);
        Vector3 RN2 = R.cross(RN1);
        double dref_scale = prim->material.getDiffuseReflection();
        double r_weight = 1.0f / scene->getDiffuseReflectionSample();
        for (int i = 0; i < scene->getDiffuseReflectionSample(); ++ i) {
            double offx, offy;
            do {
                offx = rand() / (double) RAND_MAX;
                offy = rand() / (double) RAND_MAX;
            } while (offx * offx + offy * offy > dref_scale * dref_scale);
            Vector3 RF = R + offx * RN1 + offy * RN2;
            ref_col += traceRay(Ray(col.pos + RF * DOZ, RF), depth - 1) * r_weight;
        }
    } else
        ref_col = traceRay(Ray(col.pos + R * DOZ, R), depth - 1);

    return prim->material.getReflection() * ref_col *
              prim->material.getColor();
}

Color RayTracer::getRefractionColor(Primitive* prim, const Vector3& N, const Vector3& L,
                                    const Collision& col, int depth) {
    Color result;
    double rindex = prim->material.getRindex();
    if (col.hit_type == Collision::OUTSIDE)
        rindex = 1.0f / rindex;
    double cosI = -N * L, cosT2 = 1 - ( rindex * rindex ) * ( 1 - cosI * cosI );
    if ( cosT2 > DOZ ) {
        Vector3 P = L * rindex + N * ( rindex * cosI - sqrt(cosT2));
        Color refract_col = traceRay(Ray(col.pos + P * DOZ, P), depth - 1) *
                            prim->material.getRefraction();
        if (col.hit_type == Collision::OUTSIDE)
            result += refract_col;
        else {
            Vector3 absorbance = Vector3(prim->material.getColor()) *
                                 prim->material.getDensity() * (-col.distance);
            Color transparency = Color(exp(absorbance.x / 255) * 255,
                                       exp(absorbance.y / 255) * 255,
                                       exp(absorbance.z / 255) * 255);
            result += refract_col * transparency;
        }
    }
    return result;
}

Color RayTracer::traceRay(const Ray &current_ray, int depth) {
    if (depth == 0)
        return scene->getBackgroundColor();

    Primitive* collide_primitive = scene->getNearestPrimitive(current_ray);
    Light* collide_light = scene->getNearestLight(current_ray);
    if (collide_light && ((collide_primitive &&
                           collide_light->collision.distance < collide_primitive->collision.distance) ||
                          !collide_primitive))
        return collide_light->getColor();

    if (collide_primitive != 0) {
        Color result;

        Collision p_collision = collide_primitive->collision;
        Vector3 N = p_collision.normal.getNormal();
        Vector3 L = current_ray.direction.getNormal();
        Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();

        result += getBasicPhongColor(collide_primitive, p_collision, N, -L);
        if (collide_primitive->material.getReflection() > DOZ) {
            result += getReflectionColor(collide_primitive, depth, R, p_collision);
        }
        if (collide_primitive->material.getRefraction() > DOZ) {
            result += getRefractionColor(collide_primitive, N, L, p_collision, depth);
        }

        return result;
    }

    return scene->getBackgroundColor();
}
