#include "raytracer.h"
#include "camera.h"

void RayTracer::run() {
    camera = new Camera;
    for (int cy = 0; cy < camera->getImgH(); ++cy) {
        for (int cx = 0; cx < camera->getImgW(); ++cx) {
            Ray current_ray = camera->emit((double) cx, (double) cy);

        }
    }
}

