#include "camera.h"
#include "default.h"

Ray Camera::emit(double sx, double sy) {
    return Ray();
}

Camera::Camera() {
    img_h = DEFAULT_CAMERA_IMG_H;
    img_w = DEFAULT_CAMERA_IMG_W;
    window_h = DEFAULT_CAMERA_WINDOW_H;
    window_w = DEFAULT_CAMERA_WINDOW_W;
    world_position = DEFAULT_CAMERA_WORLD_POSITION;
}




