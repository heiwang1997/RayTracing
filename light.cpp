#include "light.h"
#include "default.h"
#include <cstdlib>

Vector3 Light::getPos(int dx, int dy, int all) const {
    double px = pos.getAttr(0) - size / 2 + (dx + random01()) * size / all;
    double py = pos.getAttr(1) - size / 2 + (dy + random01()) * size / all;
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
    result.hit_type = Collision::OUTSIDE;
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

Ray Light::getPhotonRay() const {
    // Emit direction of a DIFFUSE SQUARE LIGHT
    // first randomly select a direction
    double px = pos.getAttr(0) - size / 2 + random01() * size;
    double py = pos.getAttr(1) - size / 2 + random01() * size;
    Vector3 epos = Vector3(px, py, pos.getAttr(2));
    return Ray(epos, Vector3(0, 0, -1).getDiffuse());
}








