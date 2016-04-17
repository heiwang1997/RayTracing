#pragma once
#include "basic.h"

class Light {

private:
    Vector3 pos;
public:
    Vector3 getPos() const;
    Light(const Vector3 &t_pos);
    ~Light() {}
};


