#include "camera.h"
#include "default.h"

Ray Camera::emit(double sx, double sy) {
    double wx = sx / img_w * window_w;
    double wy = sy / img_h * window_h;
    return Ray(world_position.source,
               world_position.direction + dx * (wx - window_w / 2) + dy * (wy - window_h / 2));
}

Ray Camera::emitDOF(const Ray& ori_ray) {
    Vector3 focal_point = ori_ray.source + ori_ray.direction * focal_distance; // focal distance
    Vector3 image_point = ori_ray.source + ori_ray.direction;
    double offx, offy;
    do {
        offx = random01() * 2 - 1;
        offy = random01() * 2 - 1;
    } while (offx * offx + offy * offy > 1);
    Vector3 dof_emit_point = image_point + dx * offx * aperture + dy * offy * aperture;
    return Ray(dof_emit_point, focal_point - dof_emit_point);
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
    focal_distance = DEFAULT_CAMERA_FOCAL_DISTANCE;
    aperture = DEFAULT_CAMERA_APERTURE;
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
        if (attr == "POSITION") world_position.source.loadAttr(fp);
        else if (attr == "DIRECTION") world_position.direction.loadAttr(fp);
        else if (attr == "HEIGHT") img_h = getAttrInt(fp);
        else if (attr == "WIDTH") img_w = getAttrInt(fp);
        else if (attr == "FOCALDISTANCE") focal_distance = getAttrDouble(fp);
        else if (attr == "APERTURE") aperture = getAttrDouble(fp);
    }
}








