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
    Scene();
    ~Scene();
};

