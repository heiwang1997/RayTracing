#pragma once

#include <string>
#include <vector>
#include "basic.h"
#include "primitive.h"

class Object;

double mat_max3(const float*);

class ObjLoader {
    std::vector<Object*> loaded_objects;
    Material overriden_material;
    bool material_overridden;
public:
    std::vector<Object*> getLoadedObjects() { return loaded_objects; }
    void loadObjFile(const std::string &, const Vector3& rot,
                     const Vector3& pos, const double scale);
    void setMaterialOverride(const Material&);
    ObjLoader();

    ~ObjLoader();
};
