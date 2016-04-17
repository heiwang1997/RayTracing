#include "raytracer.h"
#include "camera.h"
#include "scene.h"
#include "light.h"
#include "primitive.h"
#include "imgwriter.h"
#include <cmath>
#include <iostream> // DEBUG
using namespace std;

void RayTracer::run() {
    camera = new Camera;
    scene = new Scene;
    for (int cy = 0; cy < camera->getImgH(); ++cy) {
        for (int cx = 0; cx < camera->getImgW(); ++cx) {
            Ray current_ray = camera->emit((double) cx, (double) cy);
            Primitive* collide_primitive = scene->getNearestPrimitive(current_ray);
            if (collide_primitive != 0) {
                Color basicPhong = getBasicPhongColor(collide_primitive, current_ray);
                img->cache(cx, cy, basicPhong);
            }
        }
    }
}

void RayTracer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}

Color RayTracer::getBasicPhongColor(Primitive *prim, const Ray& c_ray) {
    Color result_color(0, 0, 0);
    for (int light_id = 0; light_id < scene->getLightsNumber(); ++ light_id) {
        Light* light = scene->getLightById(light_id);
        Vector3 N = prim->collision.normal.getNormal();
        Vector3 L = (light->getPos() - prim->collision.pos).getNormal();
        if (prim->material.getDiffuse() > DOZ) {
            result_color += (N * L * prim->material.getDiffuse()) *
                    Color(255, 255, 255) * prim->material.getColor();
        }
        if (prim->material.getSpecular() > DOZ) {
            Vector3 R = L - 2.0f * (L * N) * N; R = R.getNormal();
            Vector3 V = (c_ray.direction).getNormal();
            result_color += (pow(V * R, prim->material.getShineness()) * prim->material.getSpecular()) *
                    Color(255, 255, 255);
        }

    }
    return result_color;
}





