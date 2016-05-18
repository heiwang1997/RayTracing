#include "light.h"
#include "default.h"
#include <cstdlib>

Vector3 Light::getPos(int dx, int dy, int all) const {
    double px = pos.getAttr(0) - size / 2 + (dx + rand() / (double) RAND_MAX) * size / all;
    double py = pos.getAttr(1) - size / 2 + (dy + rand() / (double) RAND_MAX) * size / all;
    return Vector3(px, py, pos.getAttr(2));
}

Light::Light(const Vector3 &t_pos) : pos(t_pos) {
    color = DEFAULT_LIGHT_COLOR;
    size = DEFAULT_LIGHT_SIZE;
    hash_code = rand();
}

Color Light::getColor() const {
    return color;
}

void Light::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        else if (attr == "POSITION") pos.loadAttr(fp);
        else if (attr == "COLOR") color.loadAttr(fp);
        else if (attr == "SIZE") size = getAttrDouble(fp);
    }
}

Vector3 Light::getPos() const {
    return pos;
}

Collision Light::updateCollision(const Ray &c_ray, double max_dist) {
    Collision result;
    result.distance = (pos.z - c_ray.source.z) / c_ray.direction.z;
    if (result.distance > DOZ && result.distance < max_dist) {
        Vector3 collision_point = c_ray.source + result.distance * c_ray.direction;
        if (collision_point.x > pos.x - size / 2 && collision_point.x < pos.x + size / 2 &&
            collision_point.y > pos.y - size / 2 && collision_point.y < pos.y + size / 2)
            return result;
    }
    return Collision();
}

int Light::getHashCode() const {
    return hash_code;
}






