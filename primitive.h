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
    Collision collision;
    virtual bool updateCollision(const Ray&);
};

