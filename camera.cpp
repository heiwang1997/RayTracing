#include "camera.h"
#include "default.h"

Ray Camera::emit(double wx, double wy) {
    return Ray(world_position.source,
               world_position.direction + dx * (wx - window_w / 2) + dy * (wy - window_h / 2));
}

Camera::Camera() {
    img_h = DEFAULT_CAMERA_IMG_H;
    img_w = DEFAULT_CAMERA_IMG_W;
    window_h = DEFAULT_CAMERA_WINDOW_H;
    window_w = DEFAULT_CAMERA_WINDOW_W;
    world_position = DEFAULT_CAMERA_WORLD_POSITION;
    Vector3 dir = world_position.direction;
    dx = dir.getVertical().getNormal();
    dy = dir.cross(dx).getNormal();
    dx = dx.rotate(dir, DEFAULT_CAMERA_ROTATE);
}




