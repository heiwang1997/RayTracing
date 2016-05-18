#pragma once

#include <vector>
#include "basic.h"
#include "primitive.h"

class Primitive;
class Light;
class Ray;

struct PrimitiveCollision {
    Collision collision;
    Primitive* primitive;
    bool collide() const { return collision.collide(); }
    PrimitiveCollision() { primitive = 0; }
};

struct LightCollision {
    Collision collision;
    Light* light;
    bool collide() const { return collision.collide(); }
    LightCollision() { light = 0; }
};

class Scene
{
    std::vector<Primitive*> m_primitives;
    std::vector<Light*> m_lights;
    Color bg_color;
    bool soft_shadow;
    int light_sample;
    int diffuse_reflection_sample;
public:
    PrimitiveCollision getNearestPrimitiveCollision(const Ray&, double);
    LightCollision getNearestLightCollision(const Ray&, double);
    bool interceptTest(const Ray&, double);
    int getLightsNumber() const { return m_lights.size(); }
    Light* getLightById(int idx) const { return m_lights[idx]; }
    Color getBackgroundColor() const;
    int getLightSample() const;
    int getDiffuseReflectionSample() const;
    bool getSoftShadow() const;
    void loadAttr(FILE*);
    Scene();
    ~Scene();
};

