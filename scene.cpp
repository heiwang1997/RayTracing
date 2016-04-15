#include "scene.h"
#include "basic.h"
#include "primitive.h"

Scene::Scene() {

}

Scene::~Scene() {
    for (std::vector::iterator it = m_primitives.begin();
            it != m_primitives.end(); ++ it) {
        delete (*it);
    }
    m_primitives.clear();
}

Primitive *Scene::getNearestPrimitive(const Ray &m_ray) {
    Primitive* result = 0;
    for (std::vector::iterator it = m_primitives.begin();
         it != m_primitives.end(); ++ it) {
        (*it)->updateCollision();
    }
    return result;
}
