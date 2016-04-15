#pragma once

#include <vector>

class Primitive;
class Ray;

class Scene
{
    std::vector<Primitive*> m_primitives;
public:
    int getPrimitiveNumber() const { return m_primitives.size(); }
    Primitive* getPrimitive(int idx) const { return m_primitives[idx]; }
    Primitive* getNearestPrimitive(const Ray&);
    Scene();
    ~Scene();
};

