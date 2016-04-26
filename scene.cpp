#include "scene.h"
#include "default.h"
#include "primitive.h"
#include "light.h"

Scene::Scene() {
    bg_color = DEFAULT_SCENE_BACKGROUND_COLOR;
    light_sample = DEFAULT_SCENE_LIGHT_SAMPLE;
    diffuse_reflection_sample = DEFAULT_SCENE_DIFFUSE_REFLECTION_SAMPLE;
}

Scene::~Scene() {
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
            it != m_primitives.end(); ++ it) {
        delete (*it);
    }
    m_primitives.clear();
}

Primitive *Scene::getNearestPrimitive(const Ray &m_ray) {
    Primitive* result = 0; Ray t_ray = m_ray.getNormal();
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
         it != m_primitives.end(); ++ it) {
        // updateCollision must be passed with a normalized ray.
        bool this_collide = (*it)->updateCollision(t_ray);
        if (this_collide && (result == 0 || (*it)->collision.distance < result->collision.distance))
            result = (*it);
    }
    return result;
}

Light *Scene::getNearestLight(const Ray &m_ray) {
    Light* result = 0; Ray t_ray = m_ray.getNormal();
    for (std::vector<Light*>::iterator it = m_lights.begin();
         it != m_lights.end(); ++ it) {
        // updateCollision must be passed with a normalized ray.
        bool this_collide = (*it)->updateCollision(t_ray);
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
        if (attr == "OBJECT") {
            Object* obj = new Object();
            obj->loadAttr(fp);
            printf("LOadFinish!\n");
            m_primitives.push_back(obj);
            continue;
        }
        if (attr == "LIGHTSAMPLE") {
            light_sample = getAttrInt(fp);
            continue;
        }
        if (attr == "DIFFUSEREFLECTIONSAMPLE") {
            diffuse_reflection_sample = getAttrInt(fp);
            continue;
        }
    }
}

Color Scene::getBackgroundColor() const {
    return bg_color;
}

bool Scene::interceptTest(const Ray &m_ray, double t_dist) {
    Ray t_ray = m_ray.getNormal();
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
         it != m_primitives.end(); ++ it) {
        // updateCollision must be passed with a normalized ray.
        bool this_collide = (*it)->updateCollision(t_ray);
        if (this_collide && (*it)->collision.distance < t_dist) return true;
    }
    return false;
}


int Scene::getLightSample() const {
    return light_sample;
}

int Scene::getDiffuseReflectionSample() const {
    return diffuse_reflection_sample;
}




