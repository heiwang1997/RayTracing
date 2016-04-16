#include "raytracer.h"
#include "camera.h"
#include "scene.h"
#include "primitive.h"
#include "imgwriter.h"
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
                //cout << collide_primitive->collision.distance << endl;
                int this_color = 255 - (collide_primitive->collision.distance - 1.2) / 0.4 * 255;
                img->cache(cx, cy, Color(this_color, this_color, this_color));
            }
        }
    }
}

void RayTracer::setImgWriter(ImgWriter *imgWriter) {
    img = imgWriter;
}



