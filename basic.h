#pragma once

extern const double DOZ;
extern const double PI;

class Vector3
{
public:
    double x, y, z;
    Vector3 operator+ (const Vector3&) const;
    Vector3 operator- (const Vector3&) const;
    Vector3 operator- () const;
    double operator* (const Vector3&) const;
    Vector3 operator* (double) const;
    friend Vector3 operator* (double, Vector3);
    // operator = and copy construction uses default.
    Vector3 getNormal() const;
    Vector3 cross(const Vector3&) const;
    Vector3 rotate(const Vector3& axis, double angle) const;
    double getLength() const;
    void dump() const;
    Vector3 getVertical() const;
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
    Color confine();
public:
    double r, g, b;
    // operator = and copy construction uses default.
    void dump() const;
    Color operator + (const Color&) const;
    Color operator += (const Color&);
    Color operator * (double) const;
    Color operator * (const Color&) const;
    friend Color operator* (double, Color);
    Color();
    Color(double nr, double ng, double nb);
    ~Color() {}
};


