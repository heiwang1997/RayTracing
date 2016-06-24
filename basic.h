#pragma once

#include <cstdio>
#include <string>
extern const double DOZ;
extern const double PI;
extern const double INF;
extern int dsx;
extern int dsy;
extern const int HASH_MOD;
extern const int SUPER_SAMPLING_QUALITY;
extern const int MAX_RAYTRACE_THREAD_NUM;
extern const int MAX_THREAD_NUM;

std::string getAttrName(FILE*);
std::string getAttrString(FILE*);
double getAttrDouble(FILE*);
int getAttrInt(FILE*);
double random01();
void generateCommentlessFile(std::string);

class Color;

class Vector3
{
public:
    double x, y, z;
    Vector3 operator+ (const Vector3&) const;
    Vector3 operator- (const Vector3&) const;
    Vector3 operator/ (double) const;
    Vector3 operator- () const;
    double operator* (const Vector3&) const;
    Vector3 operator* (double) const;
    double& operator[] (int);
    Vector3 operator += (const Vector3&);
    Vector3 operator *= (const Vector3&);
    double getAttr(int) const;
    Vector3 getDiffuse() const;
    friend Vector3 operator* (double, Vector3);
    // operator = and copy construction uses default.
    Vector3 getNormal() const;
    Vector3 powerize() const;
    Vector3 cross(const Vector3&) const;
    Vector3 rotate(const Vector3& axis, double angle) const;
    Vector3 scale(const Vector3&) const;
    double getLength() const;
    double getDist2() const;
    void dump() const;
    void loadAttr(FILE*);
    Vector3 getVertical() const;
    Vector3();
    Vector3(const Color&, bool);
    Vector3(double nx, double ny, double nz);
    ~Vector3() {}
};

class Ray
{
public:
    Vector3 source;
    Vector3 direction;
    void dump() const;
    Ray getNormal() const;
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
    Color scale(const Vector3&) const;
    double getPower() const;
    friend Color operator* (double, Color);
    Color();
    Color(double nr, double ng, double nb);
    Color(const Vector3&);
    void loadAttr(FILE*);
    ~Color() {}
};


