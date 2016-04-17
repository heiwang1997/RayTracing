#include "scene.h"
#include "basic.h"
#include "primitive.h"
#include "light.h"

Scene::Scene() {
    Sphere* ball1 = new Sphere(Vector3(0.5, 0.5, 0.5), 0.5);
    Plane* p1 = new Plane(Vector3(0, 0, 1), 0.0f);
    m_primitives.push_back(ball1);
    m_primitives.push_back(new Sphere(Vector3(0.5, 1, 0.5), 0.3));
    m_primitives.push_back(p1);
    Light* light1 = new Light(Vector3(-2, -5, 4));
    m_lights.push_back(light1);
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
