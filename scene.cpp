#include "scene.h"
#include "primitive.h"
#include "light.h"

Scene::Scene() {
}

Scene::~Scene() {
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
            it != m_primitives.end(); ++ it) {
        delete (*it);
    }
    m_primitives.clear();
}

Primitive *Scene::getNearestPrimitive(const Ray &m_ray) {
    Primitive* result = 0;
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
         it != m_primitives.end(); ++ it) {
        bool this_collide = (*it)->updateCollision(m_ray);
        if (this_collide && (result == 0 || (*it)->collision.distance < result->collision.distance))
            result = (*it);
    }
    return result;
}

void Scene::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "LIGHT") {
            Light* lt = new Light(Vector3());
            lt->loadAttr(fp);
            m_lights.push_back(lt);
            continue;
        }
        if (attr == "SPHERE") {
            Sphere* sp = new Sphere("DummySphere", Vector3(), 1);
            sp->loadAttr(fp);
            m_primitives.push_back(sp);
            continue;
        }
        if (attr == "PLANE") {
            Plane* pl = new Plane("DummyPlane", Vector3(), 1);
            pl->loadAttr(fp);
            m_primitives.push_back(pl);
            continue;
        }
    }
}


