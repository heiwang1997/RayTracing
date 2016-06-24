#include <cmath>
#include "scene.h"
#include "default.h"
#include "light.h"

Scene::Scene() {
    bg_color = DEFAULT_SCENE_BACKGROUND_COLOR;
    light_sample = DEFAULT_SCENE_LIGHT_SAMPLE;
    diffuse_reflection_sample = DEFAULT_SCENE_DIFFUSE_REFLECTION_SAMPLE;
    soft_shadow = DEFAULT_SCENE_SOFT_SHADOW;
    depth_of_field_sample = DEFAULT_SCENE_DOF_QUALITY;
    env_tex = 0;
}

Scene::~Scene() {
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
            it != m_primitives.end(); ++ it) {
        delete (*it);
    }
    m_primitives.clear();
    for (std::vector<Light*>::iterator it = m_lights.begin();
         it != m_lights.end(); ++ it) {
        delete (*it);
    }
    m_lights.clear();
    if (env_tex) delete env_tex;
}

PrimitiveCollision Scene::getNearestPrimitiveCollision(const Ray &m_ray, double max_dist) {
    PrimitiveCollision result;
    Ray t_ray = m_ray.getNormal();
    for (std::vector<Primitive*>::iterator it = m_primitives.begin();
         it != m_primitives.end(); ++ it) {
        // updateCollision must be passed with a normalized ray.
        PrimitiveCollision pcol = (*it)->updateCollision(t_ray, max_dist);
        if (pcol.collide() && (!result.collide() || result.collision.distance > pcol.collision.distance)) {
            result = pcol;
        }
    }
    return result;
}

LightCollision Scene::getNearestLightCollision(const Ray &m_ray, double max_dist) {
    LightCollision result;
    Ray t_ray = m_ray.getNormal();
    for (std::vector<Light*>::iterator it = m_lights.begin();
         it != m_lights.end(); ++ it) {
        // updateCollision must be passed with a normalized ray.
        Collision lcol = (*it)->updateCollision(t_ray, max_dist);
        if (lcol.collide() && (!result.collide() || result.collision.distance > lcol.distance)) {
            result.light = (*it);
            result.collision = lcol;
        }
    }
    return result;
}

void Scene::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "ENVIRONMENTMAP") {
            env_tex = new Texture();
            env_tex->loadAttr(fp);
            continue;
        }
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
            std::vector<Object*> obj_prims;
            obj_prims = Object::loadObjAttr(fp);
            for (std::vector<Object*>::iterator it = obj_prims.begin();
                    it != obj_prims.end(); ++ it) {
                m_primitives.push_back(*it);
            }
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
        if (attr == "SOFTSHADOW") {
            soft_shadow = (bool) getAttrInt(fp);
            continue;
        }
        if (attr == "DEPTHOFFIELDSAMPLE") {
            depth_of_field_sample = getAttrInt(fp);
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
        if ((*it)->updateCollision(t_ray, t_dist).collide())
            return true;
    }
    return false;
}


int Scene::getLightSample() const {
    return light_sample;
}

int Scene::getDiffuseReflectionSample() const {
    return diffuse_reflection_sample;
}

bool Scene::getSoftShadow() const {
    return soft_shadow;
}

int Scene::getDOFSample() const {
    return depth_of_field_sample;
}

Color Scene::getEnvironmentTexture(const Vector3 &dir) const {
    if (env_tex == 0) return getBackgroundColor();
    Vector3 VP = dir.getNormal();
    double phi = acos(VP.getAttr(2));
    double v = phi * (1.0f / PI);
    double theta = acos(VP.getAttr(0) / sin(phi)) * (0.5f / PI);
    double u;
    if (VP.getAttr(1) >= 0)
        u = (1.0f - theta);
    else
        u = theta;
    return env_tex->getColor(u, v);
}
