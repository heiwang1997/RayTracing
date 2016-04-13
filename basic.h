#pragma once

class Vector3
{
public:
    double x, y, z;
    Vector3 operator+ (const Vector3&) const;
    Vector3 operator- (const Vector3&) const;
    double operator* (const Vector3&) const;
    Vector3 operator* (double) const;
    friend Vector3 operator* (double, Vector3);
    // operator = and copy construction uses default.
    Vector3 getNormal() const;
    double getLength() const;
    void dump() const;
    Vector3();
    Vector3(double nx, double ny, double nz);
    ~Vector3() {}
};

class Ray
{
public:
    Vector3 source;
    Vector3 direction;
    void dump();
    Ray();
    Ray(const Vector3& src, const Vector3& dir);
    ~Ray() {}
};

class Color
{
public:
    double r, g, b;
    // operator = and copy construction uses default.
    void dump() const;
    Color();
    Color(double nr, double ng, double nb);
    ~Color() {}
};

