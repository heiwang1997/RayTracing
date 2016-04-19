#include "camera.h"
#include "default.h"

Ray Camera::emit(double sx, double sy) {
    double wx = sx / img_w * window_w;
    double wy = sy / img_h * window_h;
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
    dy = dy.rotate(dir, DEFAULT_CAMERA_ROTATE);
}


int Camera::getImgW() const {
    return img_w;
}

int Camera::getImgH() const {
    return img_h;
}

double Camera::getWindowW() const {
    return window_w;
}

double Camera::getWindowH() const {
    return window_h;
}

void Camera::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
    }
}





