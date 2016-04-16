#pragma once

#include "basic.h"

struct Collision
{
    double distance;
    Vector3 normal;
    Vector3 pos;
    Collision() {}
    ~Collision() {}
};

class Primitive
{
public:
    Collision collision;
    virtual bool updateCollision(const Ray&) = 0;
    Primitive();
    virtual ~Primitive();
};

class Sphere : public Primitive
{
protected:
    Vector3 pos; // position of the center.
    double radius;
public:
    virtual bool updateCollision(const Ray&);
    Sphere(const Vector3& m_pos, double m_radius)
        : pos(m_pos), radius(m_radius) {}
    ~Sphere() {}
};

class Plane : public Primitive
{
protected:
    Vector3 norm;
    double d;
public:

    virtual bool updateCollision(const Ray &ray);
    Plane(const Vector3 &t_norm, double t_d)
            : norm(t_norm), d(t_d) { }
    ~Plane() {}
};

