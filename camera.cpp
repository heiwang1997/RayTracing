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
    window_w = DEFAULT_CAMERA_WINDOW_W;
    window_h = DEFAULT_CAMERA_WINDOW_W * img_h / img_w;
    world_position = DEFAULT_CAMERA_WORLD_POSITION;
    focal_distance = DEFAULT_CAMERA_FOCAL_DISTANCE;
    aperture = DEFAULT_CAMERA_APERTURE;
    hitpointmap_coef = DEFAULT_CAMERA_HITPOINTMAP_COEF;
    photons_emitted = DEFAULT_CAMERA_PHOTONS_EMITTED;
    init_radius = DEFAULT_CAMERA_INITIAL_RADIUS;
    ppm_iterations = DEFAULT_CAMERA_PPM_ITERATIONS;
    rotation = DEFAULT_CAMERA_ROTATE;
    angle_dist = 1.0f;
    ppm_alpha = 0.7f;
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
        else if (attr == "ANGLEDIST") angle_dist = getAttrDouble(fp);
        else if (attr == "LOOKAT") lookat_pos.loadAttr(fp);
        else if (attr == "HEIGHT") img_h = getAttrInt(fp);
        else if (attr == "WIDTH") img_w = getAttrInt(fp);
        else if (attr == "FOCALDISTANCE") focal_distance = getAttrDouble(fp);
        else if (attr == "APERTURE") aperture = getAttrDouble(fp);
        else if (attr == "HITPOINTMAPCOEF") hitpointmap_coef = getAttrInt(fp);
        else if (attr == "PHOTONS") photons_emitted = getAttrInt(fp);
        else if (attr == "PMRADIUS") init_radius = getAttrDouble(fp);
        else if (attr == "PPMITERATIONS") ppm_iterations = getAttrInt(fp);
        else if (attr == "ROTATION") rotation = getAttrDouble(fp) * PI / 180.0;
        else if (attr == "WINDOWSIZE") window_h = getAttrInt(fp);
        else if (attr == "PPMALPHA") ppm_alpha = getAttrDouble(fp);
    }
    initCamera();
}


int Camera::getPhotonsEmitted() const {
    return photons_emitted;
}

int Camera::getHitpointmapCoef() const {
    return hitpointmap_coef;
}

double Camera::getInitRadius() const {
    return init_radius;
}

int Camera::getPPMIterations() const {
    return ppm_iterations;
}

double Camera::getPPMAlpha() const {
    return ppm_alpha;
}

void Camera::initCamera() {
    Vector3 dir;
    window_h = window_w * img_h / img_w;
    dir = (lookat_pos - world_position.source).getNormal() * angle_dist;
    world_position.direction = dir;
    dx = dir.getVertical().getNormal();
    dy = dir.cross(dx).getNormal();
    dx = dx.rotate(dir, rotation);
    dy = dy.rotate(dir, rotation);
}

