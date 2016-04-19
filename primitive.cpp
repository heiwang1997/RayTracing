#include "primitive.h"
#include "default.h"
#include <cmath>

Primitive::Primitive(const std::string &t_name) {
    m_name = t_name;
}

Primitive::~Primitive() {

}

bool Sphere::updateCollision(const Ray& ray) {
    Vector3 L = pos - ray.source;
    double l = L.getLength();
    if (l > radius) { // Camera is outside the ball
        double tp = L * ray.direction.getNormal();
        if (tp < DOZ) return false;
        double d2 = l * l - tp * tp;
        double tpp = radius * radius - d2;
        if (tpp < DOZ) return false; tpp = sqrt(tpp);
        this->collision.distance = tp - tpp;
        this->collision.pos = ray.source + (this->collision.distance) * ray.direction.getNormal();
        this->collision.normal = this->collision.pos - pos;
        return true;
    } else  // TODO: handle this situation
        return false;

}

void Sphere::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "NAME") m_name = getAttrString(fp);
        else if (attr == "POSITION") pos.loadAttr(fp);
        else if (attr == "RADIUS") radius = getAttrDouble(fp);
        else if (attr == "MATERIAL") material.loadAttr(fp);
    }
}


bool Plane::updateCollision(const Ray& ray) {
    double t = - (d + norm * ray.source) / (norm * ray.direction.getNormal());
    if (t > DOZ) {
        this->collision.normal = norm;
        this->collision.pos = ray.source + t * ray.direction.getNormal();
        this->collision.distance = t;
        return true;
    } else
        return false;
}

void Plane::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "NAME") m_name = getAttrString(fp);
        else if (attr == "NORMAL") norm.loadAttr(fp);
        else if (attr == "OFFSET") d = getAttrDouble(fp);
        else if (attr == "MATERIAL") material.loadAttr(fp);
    }
}


Material::Material() {
    shineness = DEFAULT_MATERIAL_SHINENESS;
    diffuse = DEFAULT_MATERIAL_DIFFUSE;
    specular = DEFAULT_MATERIAL_SPECULAR;
    origin_color = DEFAULT_MATERIAL_COLOR;
}

double Material::getShineness() const {
    return shineness;
}

void Material::setShineness(double shineness) {
    Material::shineness = shineness;
}

double Material::getDiffuse() const {
    return diffuse;
}

void Material::setDiffuse(double diffuse) {
    Material::diffuse = diffuse;
}

double Material::getSpecular() const {
    return specular;
}

void Material::setSpecular(double specular) {
    Material::specular = specular;
}

Color Material::getColor() const {
    return origin_color;
}

void Material::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "COLOR") origin_color.loadAttr(fp);
        else if (attr == "DIFFUSION") diffuse = getAttrDouble(fp);
        else if (attr == "SPECULAR") specular = getAttrDouble(fp);
        else if (attr == "REFLECTION") continue;
    }
}


std::string Primitive::getName() const {
    return m_name;
}