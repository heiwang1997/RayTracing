#pragma once

#include <vector>
#include "basic.h"
#include "primitive.h"

class Primitive;
class Light;
class Ray;
struct PrimitiveCollision;
class Texture;

struct LightCollision {
    Collision collision;
    Light* light;
    bool collide() const { return collision.collide(); }
    LightCollision() { light = 0; }
};

class Scene
{
    std::vector<Primitive*> m_primitives;
    std::vector<Light*> m_lights;
    Color bg_color;
    bool soft_shadow;
    int light_sample;
    int diffuse_reflection_sample;
    int depth_of_field_sample;
    Texture* env_tex;
public:
    PrimitiveCollision getNearestPrimitiveCollision(const Ray&, double);
    LightCollision getNearestLightCollision(const Ray&, double);
    bool interceptTest(const Ray&, double);
    int getLightsNumber() const { return m_lights.size(); }
    Light* getLightById(int idx) const { return m_lights[idx]; }
    Color getBackgroundColor() const;
    Color getEnvironmentTexture(const Vector3 &dir) const;
    bool hasEnvironmentMap() const { return (env_tex != 0); }
    int getLightSample() const;
    int getDiffuseReflectionSample() const;
    int getDOFSample() const;
    bool getSoftShadow() const;
    void loadAttr(FILE*);
    Scene();
    ~Scene();
};

