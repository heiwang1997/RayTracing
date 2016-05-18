#pragma once

#include "basic.h"
#include "image.h"
#include <string>

struct Collision
{
    double distance;
    Vector3 normal;
    Vector3 pos;
    enum HitType {OUTSIDE = 1, INSIDE = 0, MISS = -1} hit_type;
    Collision() { hit_type = MISS; }
    bool collide() const { return (hit_type != MISS); }
    ~Collision() {}
};

class Texture
{
    Color** color_table;
    int tex_w;
    int tex_h;
    double offset_x, offset_y;
    double scale_x, scale_y;
public:

    double getOffsetX() const;
    double getOffsetY() const;
    double getScaleX() const;
    double getScaleY() const;

    void loadAttr(FILE*);
    void loadFile(const char*);
    static ImgLoader* imgLoader;
    Color getColor(double x, double y) const;
    Texture();
    ~Texture();
};

class Material
{
    double shineness;
    double diffuse;
    double specular;
    double reflection;
    double refraction;
    double diffuse_reflection;
    double ambient;
    double density;
    double rindex;
    Color origin_color;
    Color absorbance;
    Texture* texture;

public:

    double getDiffuseReflection() const;
    double getShineness() const;
    double getDiffuse() const;
    double getSpecular() const;
    double getReflection() const;
    double getRefraction() const;
    double getRindex() const;
    double getAmbient() const;
    double getDensity() const;

    Color getAbsorbance() const;
    Color getColor() const;
    Texture* getTexture() const;
    void loadAttr(FILE*);
    Material();
    ~Material();
};

class Primitive
{
protected:
    int hash_code;
public:
    Material material;
    virtual Color getColor(const Vector3&) const;
    virtual Vector3 getTextureUV(const Vector3&) const = 0;
    virtual Collision updateCollision(const Ray&, double) = 0;
    virtual void loadAttr(FILE*) = 0;
    int getHashCode() const;
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

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

    virtual Collision updateCollision(const Ray&, double);
    void loadAttr(FILE*);
    std::string getName() const { return m_name; }

    Sphere(const std::string & t_name, const Vector3& m_pos, double m_radius);
    ~Sphere() {}
};

class Plane : public Primitive
{
protected:
    Vector3 norm;
    double d;
    std::string m_name;
    Vector3 m_UAxis;
    Vector3 m_VAxis;
    void updateUV();
public:

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

    virtual Collision updateCollision(const Ray &ray, double);
    void loadAttr(FILE*);
    std::string getName() const { return m_name; }

    Plane(const std::string & t_name, const Vector3 &t_norm, double t_d);
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
    int index;

    friend class Object;
    virtual Collision updateCollision(const Ray &ray, double);
    virtual void loadAttr(FILE*) {}

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

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
    double scale;
    Vector3 rotation;
    Vector3 pos;
public:
    virtual Collision updateCollision(const Ray &ray, double);
    virtual void loadAttr(FILE *fp);

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

    Object();
    ~Object();
};

