#pragma once
#include "basic.h"

class Light {

private:
    Vector3 pos;
    Color color;
public:
    Vector3 getPos() const;
    Color getColor() const;
    Light(const Vector3 &t_pos);
    void loadAttr(FILE*);
    ~Light() {}
};


