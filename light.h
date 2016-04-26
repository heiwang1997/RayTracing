#pragma once
#include "basic.h"

class LightCollision {
public:
    double distance;
    LightCollision() {}
    ~LightCollision() {}
};

class Light {

private:
    Vector3 pos;
    double size;
    Color color;
public:
    LightCollision collision;
    bool updateCollision(const Ray&);
    Vector3 getPos(int, int, int) const;
    Vector3 getPos() const;
    Color getColor() const;
    Light(const Vector3 &t_pos);
    void loadAttr(FILE*);
    ~Light() {}
};


