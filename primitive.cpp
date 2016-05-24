#include "primitive.h"
#include "kdtritree.h"
#include "default.h"
#include <cmath>
#include <cstdlib>
#include <list>

ImgLoader* Texture::imgLoader = 0;

Texture::Texture() {
    color_table = 0;
    tex_h = 0;
    tex_w = 0;
    scale_x = DEFAULT_TEXTURE_SCALE_X;
    scale_y = DEFAULT_TEXTURE_SCALE_Y;
    offset_x = DEFAULT_TEXTURE_OFFSET_X;
    offset_y = DEFAULT_TEXTURE_OFFSET_Y;
}

Texture::~Texture() {
    if (color_table) {
        for (int i = 0; i < tex_h; ++i)
            delete[] color_table[i];
        delete[] color_table;
    }
}

// Might be time-consuming...
Color Texture::getColor(double x, double y) const {
    int lux = ((int(floor(x)) % tex_w) + tex_w) % tex_w;
    int luy = ((int(floor(y)) % tex_h) + tex_h) % tex_h;
    int rux = (lux + 1) % tex_w, ruy = luy;
    int ldx = lux, ldy = (luy + 1) % tex_h;
    int rdx = (lux + 1) % tex_w, rdy = (luy + 1) % tex_h;

    double ofx = x - floor(x), ofy = y - floor(y);
    Color lu = color_table[luy][lux],
          ld = color_table[ldy][ldx],
          ru = color_table[ruy][rux],
          rd = color_table[rdy][rdx];

    return lu * ((1 - ofx) * (1 - ofy)) + ld * ((1 - ofx) * ofy) +
           ru * (ofx * (1 - ofy)) + rd * (ofx * ofy);
}

void Texture::loadAttr(FILE *fp) {
    std::string attr;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") break;
        if (attr == "NAME") loadFile(getAttrString(fp).data());
        else if (attr == "OFFSETX") offset_x = getAttrDouble(fp);
        else if (attr == "OFFSETY") offset_y = getAttrDouble(fp);
        else if (attr == "SCALEX") scale_x = getAttrDouble(fp);
        else if (attr == "SCALEY") scale_y = getAttrDouble(fp);
    }
}

void Texture::loadFile(const char *file_name) {
    imgLoader->load(file_name);
    tex_h = imgLoader->getHeight();
    tex_w = imgLoader->getWidth();
    if (tex_h != 0) color_table = new Color*[tex_h];
    for (int i = 0; i < tex_h; ++ i) {
        color_table[i] = new Color[tex_w];
        for (int j = 0; j < tex_w; ++ j)
            color_table[i][j] = imgLoader->getColor(j, i);
    }
}


Primitive::Primitive() {
    hash_code = rand();
}

Primitive::~Primitive() {

}

int Primitive::getHashCode() const {
    return hash_code;
}

Color Primitive::getColor(const Vector3 &pos) const {
    if (material.getTexture() == 0) return material.getColor();
    Vector3 UV = getTextureUV(pos);
    return material.getTexture()->getColor(UV.getAttr(0), UV.getAttr(1));
}

Collision Sphere::updateCollision(const Ray& ray, double max_dist) {

    Vector3 P = ray.source - pos;
    double b = -P * ray.direction;
    double det = b * b - P.getLength() * P.getLength() + radius * radius;

    if ( det < DOZ ) return Collision();
    det = sqrt( det );
    double x1 = b - det  , x2 = b + det;

    if ( x2 < DOZ ) return Collision();

    Collision result;
    if ( x1 > DOZ ) {
        result.distance = x1;
        result.hit_type = Collision::OUTSIDE;
    } else {
        result.distance = x2;
        result.hit_type = Collision::INSIDE;
    }
    if (result.distance > max_dist) return Collision();

    result.pos = ray.source + (result.distance) * ray.direction;
    result.normal = result.pos - pos;
    if ( result.hit_type == Collision::INSIDE )
        result.normal = -result.normal;

    return result;

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


Collision Plane::updateCollision(const Ray& ray, double max_dist) {
    double t = - (d + norm * ray.source) / (norm * ray.direction);
    if (t > DOZ && t < max_dist) {
        Collision result;
        result.hit_type = Collision::OUTSIDE;
        result.normal = norm;
        result.pos = ray.source + t * ray.direction;
        result.distance = t;
        return result;
    } else
        return Collision();
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
    updateUV();
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
    texture = 0;
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
        else if (attr == "TEXTURE") { texture = new Texture(); texture->loadAttr(fp); }
        else if (attr == "ABSORBANCE") absorbance.loadAttr(fp);
    }
}

double Material::getReflection() const {
    return reflection;
}

Collision Triangle::updateCollision(const Ray &ray, double max_dist) {
    const static int mods[5] = {0, 1, 2, 0, 1};
    int& k = acc.k;
    int ku = mods[k + 1];
    int kv = mods[k + 2];
    Vector3 O = ray.source, D = ray.direction, A = vertex[0];
    const double lnd = 1.0f / (D[k] + acc.nu * D[ku] + acc.nv * D[kv]);
    const double t = (acc.nd - O[k] - acc.nu * O[ku] - acc.nv * O[kv]) * lnd;
    if (!(t > 0 && t < max_dist)) return Collision();
    double hu = O[ku] + t * D[ku] - A[ku];
    double hv = O[kv] + t * D[kv] - A[kv];
    double beta = hv * acc.bnu + hu * acc.bnv;
    if (beta < 0) return Collision();
    double gamma = hu * acc.cnu + hv * acc.cnv;
    if (gamma < 0) return Collision();
    if ((gamma + beta) > 1) return Collision();
    // Hit the triangle:
    Collision result;
    result.distance = t;
//    result.normal = normal;
    result.normal = (vertex_normal[k] * (1 - gamma - beta) +
                    vertex_normal[ku] * beta +
                    vertex_normal[kv] * gamma).getNormal();

    result.pos = O + t * D; // Smooth Shading's position should be modified.
    if (D * result.normal > 0) {
        result.hit_type = Collision::INSIDE;
        result.normal = - normal;
    } else {
        result.hit_type = Collision::OUTSIDE;
    }
    return result;
}

void Triangle::updateAccelerator() {
    Vector3 c = vertex[1] - vertex[0];
    Vector3 b = vertex[2] - vertex[0];
    normal = c.cross( b );
    area = 0.5 * normal.getLength();
//    if (fabs( normal.x ) > fabs( normal.y ))
//        if (fabs( normal.x ) > fabs( normal.z )) acc.k = 0; else acc.k = 2;
//    else
//        if (fabs( normal.y ) > fabs( normal.z )) acc.k = 1; else acc.k = 2;
    acc.k = 0;
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
    std::string mesh_file_name;
    while (true) {
        attr = getAttrName(fp);
        if (attr == "END") {
            loadMeshFile(mesh_file_name);
            break;
        }
        if (attr == "FILE") mesh_file_name = getAttrString(fp);
        if (attr == "SCALE") scale = getAttrDouble(fp);
        if (attr == "ROTATION") rotation.loadAttr(fp);
        if (attr == "POSITION") pos.loadAttr(fp);
        if (attr == "MATERIAL") material.loadAttr(fp);
    }
}

Collision Object::updateCollision(const Ray &ray, double max_dist) {
    return triangle_tree->updateCollision(ray, max_dist);
}

// implementation simplified.
void Object::loadMeshFile(const std::string &file_name) {
    FILE* fp;
    fopen_s(&fp, file_name.data(), "r");

    printf("Start Loading Mesh File: %s\n", file_name.data());
    char buf[256];
    std::vector<Vector3> vecVertices;
    std::vector<Triangle*> vecTriangles;
    std::vector<std::list<std::pair<int, int> > > vertex_face;

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
                if(fscanf_s(fp, "%lf %lf %lf", &vP.x, &vP.y, &vP.z) == 3) {
                    vP = vP.rotate(Vector3(1, 0, 0), rotation.getAttr(0)).
                            rotate(Vector3(0, 1, 0), rotation.getAttr(1)).
                            rotate(Vector3(0, 0, 1), rotation.getAttr(2)) * scale + pos;
                    vecVertices.push_back(vP);
                    vertex_face.push_back(std::list<std::pair<int, int> >());
                }
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
                    int f[3];
                    if (fscanf_s(fp, "%d %d %d", f, f + 1, f + 2) == 3) {
                        Triangle* new_tri = new Triangle;
                        vecTriangles.push_back(new_tri);
                        new_tri->index = vecTriangles.size();
                        for (int i = 0; i < 3; ++ i) {
                            new_tri->vertex[i] = vecVertices[f[i] - 1];
                            vertex_face[f[i] - 1].push_back(std::make_pair(new_tri->index - 1, i));
                        }
                        new_tri->updateAccelerator();
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

    printf("Merging normals, %d Faces and %d Vertices.\n", vecTriangles.size(), vecVertices.size());
    // Merge Phong Soften. Weight by Area.
    for (unsigned int i = 0; i < vecVertices.size(); ++ i) {
        Vector3 soft_normal(0, 0, 0);
        double total_area = 0;
        for (std::list<std::pair<int, int> >::iterator it = vertex_face[i].begin();
             it != vertex_face[i].end(); ++ it) {
            total_area += vecTriangles[(*it).first]->area;
            soft_normal += vecTriangles[(*it).first]->area * vecTriangles[(*it).first]->normal;
        }
        soft_normal = (soft_normal * (1.0f / total_area)).getNormal();
        for (std::list<std::pair<int, int> >::iterator it = vertex_face[i].begin();
             it != vertex_face[i].end(); ++ it) {
            vecTriangles[(*it).first]->vertex_normal[(*it).second] = soft_normal;
        }
    }
    triangle_tree->buildTree(vecTriangles);
}

Object::Object() {
    triangle_tree = new KDTriTree;
    pos = DEFAULT_MESH_POS;
    rotation = DEFAULT_MESH_ROTATION;
    scale = DEFAULT_MESH_SCALE;
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

Material::~Material() {
    if (texture) delete texture;
}

Texture *Material::getTexture() const {
    return texture;
}

Color Material::getAbsorbance() const {
    return absorbance;
}


Vector3 Plane::getTextureUV(const Vector3 &pos) const {
    double u = pos * m_UAxis * material.getTexture()->getScaleX();
    double v = pos * m_VAxis * material.getTexture()->getScaleY();
    return Vector3(u + material.getTexture()->getOffsetX(),
                   v + material.getTexture()->getOffsetY(), 0);
}

Vector3 Sphere::getTextureUV(const Vector3 &m_pos) const {
    static const Vector3 VN = Vector3(0, 0, 1);
    static const Vector3 VE = Vector3(1, 0, 0);
    static const Vector3 VC = VN.cross(VE);
    Vector3 VP = (m_pos - pos) * radius;
    double phi = acos(- VP * VN);
    double v = phi * material.getTexture()->getScaleY() * (1.0f / PI);
    double theta = (acos(VP * VE / sin(phi))) * (0.5f / PI);
    double u;
    if (VC * VP >= 0)
        u = (1.0f - theta) * material.getTexture()->getScaleX();
    else
        u = theta * material.getTexture()->getScaleX();
    return Vector3(u + material.getTexture()->getOffsetX(),
                   v + material.getTexture()->getOffsetY(), 0);
}

Vector3 Triangle::getTextureUV(const Vector3 &pos) const {
    return Vector3();
}

Vector3 Object::getTextureUV(const Vector3 &pos) const {
    return Vector3();
}

Sphere::Sphere(const std::string & t_name, const Vector3& m_pos, double m_radius)
        : pos(m_pos), radius(m_radius), m_name(t_name) {
}

Plane::Plane(const std::string & t_name, const Vector3 &t_norm, double t_d)
        : norm(t_norm), d(t_d), m_name(t_name) {
    updateUV();
}

void Plane::updateUV() {
    m_UAxis = Vector3(norm.y, norm.z, -norm.x).getNormal();
    m_VAxis = m_UAxis.cross(norm).getNormal();
}

double Texture::getOffsetX() const {
    return offset_x;
}

double Texture::getOffsetY() const {
    return offset_y;
}

double Texture::getScaleX() const {
    return scale_x;
}

double Texture::getScaleY() const {
    return scale_y;
}