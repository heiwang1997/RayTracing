#pragma once

#include <vector>
#include "basic.h"

class Primitive;
class Light;
class Ray;

class Scene
{
    std::vector<Primitive*> m_primitives;
    std::vector<Light*> m_lights;
    Color bg_color;
    int light_sample;
    int diffuse_reflection_sample;
public:
    Primitive* getNearestPrimitive(const Ray&);
    Light* getNearestLight(const Ray&);
    bool interceptTest(const Ray&, double);
    int getLightsNumber() const { return m_lights.size(); }
    Light* getLightById(int idx) const { return m_lights[idx]; }
    Color getBackgroundColor() const;
    int getLightSample() const;
    int getDiffuseReflectionSample() const;
    void loadAttr(FILE*);
    Scene();
    ~Scene();
};

