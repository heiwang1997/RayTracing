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
public:
    Primitive* getNearestPrimitive(const Ray&);
    int getLightsNumber() const { return m_lights.size(); }
    Light* getLightById(int idx) const { return m_lights[idx]; }
    Color getBackgroundColor() const;
    void loadAttr(FILE*);
    Scene();
    ~Scene();
};

