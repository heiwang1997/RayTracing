#pragma once
#include "basic.h"
#include "primitive.h"

class Light {

private:
    Vector3 pos;
    double size;
    Color color;
    int hash_code;
public:
    Collision updateCollision(const Ray&, double max_dist);
    Vector3 getPos(int, int, int) const;
    Vector3 getPos() const;
    Color getColor() const;
    int getHashCode() const;
    Light(const Vector3 &t_pos);
    void loadAttr(FILE*);
    ~Light() {}
};

