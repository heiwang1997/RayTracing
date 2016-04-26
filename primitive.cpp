#include "primitive.h"
#include "kdtritree.h"
#include "default.h"
#include <cmath>
#include <cstdlib>
#include <vector>

Primitive::Primitive() {
}

Primitive::~Primitive() {

}

bool Sphere::updateCollision(const Ray& ray) {

    Vector3 P = ray.source - pos;
    double b = -P * ray.direction;
    double det = b * b - P.getLength() * P.getLength() + radius * radius;

    if ( det > DOZ ) {
        det = sqrt( det );
        double x1 = b - det  , x2 = b + det;

        if ( x2 < DOZ ) return false;
        if ( x1 > DOZ ) {
            this->collision.distance = x1;
            this->collision.hit_type = Collision::OUTSIDE;
        } else {
            this->collision.distance = x2;
            this->collision.hit_type = Collision::INSIDE;
        }
    } else
        return false;

    this->collision.pos = ray.source + (this->collision.distance) * ray.direction;
    this->collision.normal = this->collision.pos - pos;
    if ( this->collision.hit_type == Collision::INSIDE )
        this->collision.normal = -this->collision.normal;

    return true;

}

void Sphere::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "NAME") m_name = getAttrString(fp);
        else if (attr == "POSITION") pos.loadAttr(fp);
        else if (attr == "RADIUS") radius = getAttrDouble(fp);
        else if (attr == "MATERIAL") material.loadAttr(fp);
    }
}


bool Plane::updateCollision(const Ray& ray) {
    double t = - (d + norm * ray.source) / (norm * ray.direction);
    if (t > DOZ) {
        this->collision.normal = norm;
        this->collision.pos = ray.source + t * ray.direction;
        this->collision.distance = t;
        return true;
    } else
        return false;
}

void Plane::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "NAME") m_name = getAttrString(fp);
        else if (attr == "NORMAL") norm.loadAttr(fp);
        else if (attr == "OFFSET") d = getAttrDouble(fp);
        else if (attr == "MATERIAL") material.loadAttr(fp);
    }
}


Material::Material() {
    shineness = DEFAULT_MATERIAL_SHINENESS;
    diffuse = DEFAULT_MATERIAL_DIFFUSE;
    specular = DEFAULT_MATERIAL_SPECULAR;
    reflection = DEFAULT_MATERIAL_REFLECTION;
    origin_color = DEFAULT_MATERIAL_COLOR;
    refraction = DEFAULT_MATERIAL_REFRACTION;
    rindex = DEFAULT_MATERIAL_RINDEX;
    ambient = DEFAULT_MATERIAL_AMBIENT;
    density = DEFAULT_MATERIAL_DENSITY;
    diffuse_reflection = DEFAULT_MATERIAL_DEFUSE_REFLECTION;
}

double Material::getShineness() const {
    return shineness;
}

double Material::getDensity() const {
    return density;
}

double Material::getAmbient() const {
    return ambient;
}

double Material::getDiffuse() const {
    return diffuse;
}

double Material::getSpecular() const {
    return specular;
}

Color Material::getColor() const {
    return origin_color;
}

void Material::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "COLOR") origin_color.loadAttr(fp);
        else if (attr == "DIFFUSION") diffuse = getAttrDouble(fp);
        else if (attr == "SPECULAR") specular = getAttrDouble(fp);
        else if (attr == "AMBIENT") ambient = getAttrDouble(fp);
        else if (attr == "REFLECTION") reflection = getAttrDouble(fp);
        else if (attr == "REFRACTION") refraction = getAttrDouble(fp);
        else if (attr == "RINDEX") rindex = getAttrDouble(fp);
        else if (attr == "DENSITY") density = getAttrDouble(fp);
        else if (attr == "DIFFUSEREFLECTION") diffuse_reflection = getAttrDouble(fp);
    }
}

double Material::getReflection() const {
    return reflection;
}

bool Triangle::updateCollision(const Ray &ray) {
    const static int mods[5] = {0, 1, 2, 0, 1};
    int& k = acc.k;
    int ku = mods[k + 1];
    int kv = mods[k + 2];
    Vector3 O = ray.source, D = ray.direction, A = vertex[0];
    const double lnd = 1.0f / (D[k] + acc.nu * D[ku] + acc.nv * D[kv]);
    const double t = (acc.nd - O[k] - acc.nu * O[ku] - acc.nv * O[kv]) * lnd;
    if (t <= 0) return false;
    double hu = O[ku] + t * D[ku] - A[ku];
    double hv = O[kv] + t * D[kv] - A[kv];
    double beta = hv * acc.bnu + hu * acc.bnv;
    if (beta < 0) return false;
    double gamma = hu * acc.cnu + hv * acc.cnv;
    if (gamma < 0) return false;
    if ((gamma + beta) > 1) return false;
    // Hit the triangle:
    collision.distance = t;
    collision.normal = normal;
    collision.pos = O + t * D; // Smooth Shading's position should be modified.
    collision.hit_type = (D * normal > 0)?
                         Collision::INSIDE : Collision::OUTSIDE;
    return true;
}

void Triangle::updateAccelerator() {
    Vector3 c = vertex[1] - vertex[0];
    Vector3 b = vertex[2] - vertex[0];
    normal = b.cross( c );
    if (fabs( normal.x ) > fabs( normal.y ))
        if (fabs( normal.x ) > fabs( normal.z )) acc.k = 0; else acc.k = 2;
    else
        if (fabs( normal.y ) > fabs( normal.z )) acc.k = 1; else acc.k = 2;
    int u = (acc.k + 1) % 3;
    int v = (acc.k + 2) % 3;
    // precomp
    double krec = 1.0f / normal[acc.k];
    acc.nu = normal[u] * krec;
    acc.nv = normal[v] * krec;
    acc.nd = normal * vertex[0] * krec;
    double reci = 1.0f / (b[u] * c[v] - b[v] * c[u]);
    acc.bnu = b[u] * reci; acc.bnv = -b[v] * reci;
    acc.cnu = c[v] * reci; acc.cnv = -c[u] * reci;
    // finalize normal
    normal = normal.getNormal();
    acc.max_pos = Vector3(-INF, -INF, -INF);
    acc.min_pos = Vector3(INF, INF, INF);
    for (int i = 0; i < 3; ++ i)
        for (int j = 0; j < 3; ++ j) {
            acc.max_pos[i] = std::max(acc.max_pos[i], vertex[j][i]);
            acc.min_pos[i] = std::min(acc.min_pos[i], vertex[j][i]);
        }
}


void Object::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "FILE") loadMeshFile(getAttrString(fp));
    }
}

bool Object::updateCollision(const Ray &ray) {
    Triangle* col_tri = triangle_tree->updateCollision(ray);
    if (col_tri == 0) return false;
    collision = col_tri->collision;
    return true;
}

// implementation simplified.
void Object::loadMeshFile(const std::string &file_name) {
    FILE* fp;
    fopen_s(&fp, file_name.data(), "r");

    printf("Start Loading Mesh File: %s\n", file_name.data());
    char buf[256];
    std::vector<Vector3> vecVertices;
    std::vector<Triangle*> vecTriangles;

    int lineNumber = 0;

    while(fscanf_s(fp, "%s", buf) != EOF) {
        lineNumber ++;
        switch(buf[0]) {
        case '#':
            fgets(buf, sizeof(buf), fp); break;
        case 'v':
            switch(buf[1]) {
            case '\0': {
                Vector3 vP;
                if(fscanf_s(fp, "%lf %lf %lf", &vP.x, &vP.y, &vP.z) == 3)
                    vecVertices.push_back(vP);
                else {
                    printf("Error while parsing vertex at Line %d\n", lineNumber);
                    return;
                }
                break;
            }
            default:
                fgets(buf, sizeof(buf), fp); break;
            }
            break;
        case 'f': {
            switch (buf[1]) {
                case '\0': {
                    int f1, f2, f3;
                    if (fscanf_s(fp, "%d %d %d", &f1, &f2, &f3) == 3) {
                        Triangle* new_tri = new Triangle;
                        new_tri->vertex[0] = vecVertices[f1 - 1] * 0.2;
                        new_tri->vertex[1] = vecVertices[f2 - 1] * 0.2;
                        new_tri->vertex[2] = vecVertices[f3 - 1] * 0.2;
                        new_tri->updateAccelerator();
                        vecTriangles.push_back(new_tri);
                    }
                    else {
                        printf("Error parsing faces at Line %d\n", lineNumber);
                        return;
                    }
                    break;
                }
                default:
                    fgets(buf, sizeof(buf), fp); break;
            }
            break;
        }
        default:
            fgets(buf, sizeof(buf), fp); break;
        }
    }
    fclose(fp);
    triangle_tree->buildTree(vecTriangles);
}

Object::Object() {
    triangle_tree = new KDTriTree;
}

Object::~Object() {
    delete triangle_tree;
}


double Material::getRefraction() const {
    return refraction;
}

double Material::getRindex() const {
    return rindex;
}

double Material::getDiffuseReflection() const {
    return diffuse_reflection;
}

