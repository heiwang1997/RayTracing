#include "light.h"
#include "default.h"

Vector3 Light::getPos() const {
    return pos;
}

Light::Light(const Vector3 &t_pos) : pos(t_pos) {
    color = DEFAULT_LIGHT_COLOR;
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
    }
}



