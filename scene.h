#pragma once

#include <vector>

class Primitive;
class Light;
class Ray;

class Scene
{
    std::vector<Primitive*> m_primitives;
    std::vector<Light*> m_lights;
public:
    Primitive* getNearestPrimitive(const Ray&);
    int getLightsNumber() const { return m_lights.size(); }
    Light* getLightById(int idx) const { return m_lights[idx]; }
    Scene();
    ~Scene();
};

