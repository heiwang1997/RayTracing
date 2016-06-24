#pragma once

#include "basic.h"
#include "image.h"
#include <string>
#include <vector>

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

class Primitive;

struct PrimitiveCollision {
    Collision collision;
    Primitive* primitive;
    bool collide() const { return collision.collide(); }
    PrimitiveCollision() { primitive = 0; }
    PrimitiveCollision(const Collision& col, Primitive* prim) {
        collision = col;
        primitive = prim;
    }
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
    int getTexW() const { return tex_w; }
    int getTexH() const { return tex_h; }

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
    Texture* bump;
    friend class ObjLoader;
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
    Texture* getBump() const;
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
    virtual PrimitiveCollision updateCollision(const Ray&, double) = 0;
    virtual void loadAttr(FILE*) = 0;
    virtual int getHashCode() const;
    virtual int getPMComparer() const { return getHashCode(); }
    Primitive();
    virtual ~Primitive();
};

class Sphere : public Primitive
{
protected:
    Vector3 pos; // position of the center.
    Vector3 VN;
    Vector3 VE;
    Vector3 VC;
    double radius;
    std::string m_name;
public:

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

    virtual PrimitiveCollision updateCollision(const Ray&, double);
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

    virtual PrimitiveCollision updateCollision(const Ray &ray, double);
    void loadAttr(FILE*);
    std::string getName() const { return m_name; }

    Plane(const std::string & t_name, const Vector3 &t_norm, double t_d);
    ~Plane() {}
};

class Triangle : public Primitive
{

    friend class Object;
    friend class ObjLoader;
    struct AccInfo {
        int k;
        double nu, nv, nd;
        double bnu, bnv;
        double cnu, cnv;
        Vector3 max_pos, min_pos;
    } acc;
    Vector3 vertex[3];
    Vector3 vertex_normal[3];
    Vector3 uv_pos[3];
    Vector3 tX, tY;
    double area;
    Vector3 normal;
    int pm_hcode;
    Object* parent;
    void updateAccelerator();
    void updateTexMatrix();
    Vector3 getBump(double beta, double gamma, const Vector3& normal);
public:
    int index;
    virtual PrimitiveCollision updateCollision(const Ray &ray, double);
    virtual void loadAttr(FILE*) {}
    virtual int getHashCode() const;
    virtual Vector3 getTextureUV(const Vector3 &vector3) const;
    int getPMComparer() { return pm_hcode; }
    double getMaxPos(int idx) const { return acc.max_pos.getAttr(idx); }
    double getMinPos(int idx) const { return acc.min_pos.getAttr(idx); }
    Triangle() { parent = 0; pm_hcode = 0; }
    ~Triangle() {}
};

class KDTriTree;

class Object : public Primitive
{
    friend class ObjLoader;
    KDTriTree* triangle_tree;
public:
    virtual PrimitiveCollision updateCollision(const Ray &ray, double);
    virtual void loadAttr(FILE *fp);
    static std::vector<Object*> loadObjAttr(FILE *fp);

    virtual Vector3 getTextureUV(const Vector3 &vector3) const;

    Object();
    ~Object();
};

