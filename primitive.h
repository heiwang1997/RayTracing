#pragma once

#include "basic.h"
#include <string>

struct Collision
{
    double distance;
    Vector3 normal;
    Vector3 pos;
    enum HitType {OUTSIDE = 1, INSIDE = 0} hit_type;
    Collision() {}
    ~Collision() {}
};

class Material
{
    double shineness;
    double diffuse;
    double specular;
    double reflection;
    Color origin_color;

public:

    double getShineness() const;
    double getDiffuse() const;
    double getSpecular() const;
    double getReflection() const;
    Color getColor() const;
    void loadAttr(FILE*);
    Material();
    ~Material() {}
};

class Primitive
{
public:
    Collision collision;
    Material material;
    virtual bool updateCollision(const Ray&) = 0;
    virtual void loadAttr(FILE*) = 0;
    Primitive();
    virtual ~Primitive();
};

class Sphere : public Primitive
{
protected:
    Vector3 pos; // position of the center.
    double radius;
    std::string m_name;
public:
    virtual bool updateCollision(const Ray&);
    void loadAttr(FILE*);
    Sphere(const std::string & t_name, const Vector3& m_pos, double m_radius)
        : pos(m_pos), radius(m_radius), m_name(t_name) {}
    ~Sphere() {}
};

class Plane : public Primitive
{
protected:
    Vector3 norm;
    double d;
    std::string m_name;
public:

    virtual bool updateCollision(const Ray &ray);
    void loadAttr(FILE*);
    Plane(const std::string & t_name, const Vector3 &t_norm, double t_d)
            : norm(t_norm), d(t_d), m_name(t_name) { }
    ~Plane() {}
};

class Triangle : public Primitive
{
    struct AccInfo {
        int k;
        double nu, nv, nd;
        double bnu, bnv;
        double cnu, cnv;
        Vector3 max_pos, min_pos;
    } acc;
    Vector3 vertex[3];
    Vector3 normal;
    void updateAccelerator();
public:
    friend class Object;
    virtual bool updateCollision(const Ray &ray);
    virtual void loadAttr(FILE*) {}
    double getMaxPos(int idx) const { return acc.max_pos.getAttr(idx); }
    double getMinPos(int idx) const { return acc.min_pos.getAttr(idx); }
    Triangle() { }
    ~Triangle() {}
};

class KDTriTree;

class Object : public Primitive
{
    KDTriTree* triangle_tree;
    void loadMeshFile(const std::string&);
public:
    virtual bool updateCollision(const Ray &ray);
    virtual void loadAttr(FILE *fp);

    Object();
    ~Object();
};

