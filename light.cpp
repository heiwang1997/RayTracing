#include "light.h"

Vector3 Light::getPos() const {
    return pos;
}

Light::Light(const Vector3 &t_pos) : pos(t_pos) { }