#include "primitive.h"
#include <cmath>

Primitive::Primitive(void) {
}


Primitive::~Primitive(void) {
}

bool Sphere::updateCollision(const Ray& ray)
    ray.direction = ray.direction.getNormal();
    Vector3 L = pos - ray.source;
    double l = L.getLength();
    if (l > radius) {
        double tp = L * ray.direction.getNormal();
        if (tp < DOZ) return false;
        double d2 = l * l - tp * tp;
        double tpp = radius * radius - d2;
        if (tpp < DOZ) return false;
        this->collision.distance = tp - tpp;
        this->collision.pos = ray.source + (this->collision.distance) * ray.direction;
        this->collision.normal = this->collision.pos - pos;
    } else
        return false;

}