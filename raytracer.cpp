#include "raytracer.h"
#include "camera.h"
#include "scene.h"
#include "light.h"
#include "primitive.h"
#include "imgwriter.h"
#include <cmath>

void RayTracer::run() {
    if (!img) return;
    for (int cy = 0; cy < camera->getImgH(); ++cy) {
        for (int cx = 0; cx < camera->getImgW(); ++cx) {
            Ray current_ray = camera->emit((double) cx, (double) cy);
            img->cache(cx, cy, traceRay(current_ray, 20));
        }
    }
}

void RayTracer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}

// Blinn-Phong Model implemented to improve specularity.
Color RayTracer::getBasicPhongColor(Primitive *prim, const Ray& c_ray,
                                    const Vector3& N, const Vector3& V) {
    Color result_color;
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        Light* light = scene->getLightById(light_id);
        Vector3 L = (light->getPos() - prim->collision.pos).getNormal();
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
        result_color += prim->material.getColor() * 0.08f;

    }
    return result_color;
}

RayTracer::RayTracer() : img(0) {
    camera = new Camera;
    scene = new Scene;
}

Color RayTracer::traceRay(const Ray &current_ray, int depth) {
    if (depth == 0) return scene->getBackgroundColor();
    Primitive* collide_primitive = scene->getNearestPrimitive(current_ray);
    if (collide_primitive != 0) {
        Color result;
        Vector3 N = collide_primitive->collision.normal.getNormal();
        Vector3 L = current_ray.direction.getNormal();
        Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();
        result += getBasicPhongColor(collide_primitive, current_ray, N, -L);
        if (collide_primitive->material.getReflection() > DOZ)
            result += collide_primitive->material.getReflection() *
                    traceRay(Ray(collide_primitive->collision.pos, R), depth - 1);
        return result;
    }
    return scene->getBackgroundColor();
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


