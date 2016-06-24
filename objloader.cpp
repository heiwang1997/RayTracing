#include <list>
#include <iostream>
#include "objloader.h"
#include "kdtritree.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

double mat_max3(const float* f) {
    return std::max(std::max(f[0], f[1]), f[2]);
}

ObjLoader::ObjLoader() {
    material_overridden = false;
}

ObjLoader::~ObjLoader() {

}

void ObjLoader::loadObjFile(const std::string &file_name, const Vector3& rot,
                            const Vector3& pos, const double scale) {
    std::string inputfile = file_name;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<Material> norm_materials;
    int pm_code = rand();
    unsigned int f_spl = file_name.find_last_of('/');
    std::string folder_name = file_name.substr(0, f_spl + 1);

    std::string err;
    bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), folder_name.c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    for (unsigned int i = 0; !material_overridden && i < materials.size(); i++) {
        Material n_mat;
        Vector3 n_color = Vector3(materials[i].diffuse[0],
                                  materials[i].diffuse[1],
                                  materials[i].diffuse[2]);
        double max_rgb = mat_max3(materials[i].diffuse);
        n_mat.origin_color = Color(n_color / max_rgb);
        n_mat.diffuse = max_rgb;
//        n_mat.reflection = mat_max3(materials[i].specular);

/*        Vector3 n_absor = Vector3(materials[i].transmittance[0],
                                  materials[i].transmittance[1],
                                  materials[i].transmittance[2]);
        n_mat.refraction = 1 - mat_max3(materials[i].transmittance);
        n_mat.absorbance = Color(n_absor / n_mat.refraction);
        n_mat.rindex = materials[i].ior;*/
        if (!materials[i].diffuse_texname.empty()) {
            std::string dtex = (folder_name + materials[i].diffuse_texname);
            printf("Loading Texture: %s\n", dtex.c_str());
            n_mat.texture = new Texture();
            n_mat.texture->loadFile(dtex.c_str());
            if (!materials[i].bump_texname.empty()) {
                std::string dptex = (folder_name + materials[i].bump_texname);
                printf(" -- with bump: %s\n", dptex.c_str());
                n_mat.bump = new Texture();
                n_mat.bump->loadFile(dptex.c_str());
            }
        }
        norm_materials.push_back(n_mat);
    }


    for (unsigned int i = 0; i < shapes.size(); i++) {
        Object* new_obj = new Object;
        std::vector<Vector3> vecVertices;
        std::vector<Vector3> vecTexcoords;
        std::vector<Triangle*> vecTriangles;
        std::vector<std::list<std::pair<int, int> > > vertex_face;

        printf("shape[%d].name = %s\n", i, shapes[i].name.c_str());
        printf("shape[%d].vertices: %d\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
       
        bool have_tex = shapes[i].mesh.texcoords.size() > 0;
        for (unsigned int v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            Vector3 vP(shapes[i].mesh.positions[3*v+0],
                       shapes[i].mesh.positions[3*v+1],
                       shapes[i].mesh.positions[3*v+2]);
            vP = vP.rotate(Vector3(1, 0, 0), rot.getAttr(0)).
                    rotate(Vector3(0, 1, 0), rot.getAttr(1)).
                    rotate(Vector3(0, 0, 1), rot.getAttr(2)) * scale + pos;
            vecVertices.push_back(vP);
            if (have_tex)
                vecTexcoords.push_back(Vector3(shapes[i].mesh.texcoords[2 * v + 0],
                                               shapes[i].mesh.texcoords[2 * v + 1],
                                               0));
            vertex_face.push_back(std::list<std::pair<int, int> >());
        }

        printf("Size of shape[%d].indices: %d\n", i, shapes[i].mesh.indices.size());
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (unsigned int f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            Triangle* new_tri = new Triangle;
            vecTriangles.push_back(new_tri);
            new_tri->index = vecTriangles.size();
            for (int j = 0; j < 3; ++ j) {
                int fj = shapes[i].mesh.indices[3 * f + j];
                new_tri->vertex[j] = vecVertices[fj];
                vertex_face[fj].push_back(std::make_pair(new_tri->index - 1, j));
            }
            new_tri->updateAccelerator();
            new_tri->parent = new_obj;
            new_tri->pm_hcode = pm_code;
            if (material_overridden)
                new_tri->material = overriden_material;
            else if (shapes[i].mesh.material_ids[f] != -1)
                new_tri->material = norm_materials[shapes[i].mesh.material_ids[f]];
        }
        printf("Merging normals, %d Faces and %d Vertices.\n", vecTriangles.size(), vecVertices.size());
        // Merge Phong Soften. Weight by Area.
        for (unsigned int k = 0; k < vecVertices.size(); ++ k) {
            Vector3 soft_normal(0, 0, 0);
            double total_area = 0;
            for (std::list<std::pair<int, int> >::iterator it = vertex_face[k].begin();
                 it != vertex_face[k].end(); ++ it) {
                total_area += vecTriangles[(*it).first]->area;
                soft_normal += vecTriangles[(*it).first]->area * vecTriangles[(*it).first]->normal;
            }
            soft_normal = (soft_normal * (1.0f / total_area)).getNormal();
            for (std::list<std::pair<int, int> >::iterator it = vertex_face[k].begin();
                 it != vertex_face[k].end(); ++ it) {
                vecTriangles[(*it).first]->vertex_normal[(*it).second] = soft_normal;
                if (have_tex)
                    vecTriangles[(*it).first]->uv_pos[(*it).second] = vecTexcoords[k];
            }
        }
        // calc T Matrix.
        if (have_tex)
            for (auto &&triangle : vecTriangles) {
                triangle->updateTexMatrix();
            }

        new_obj->triangle_tree->buildTree(vecTriangles);
        loaded_objects.push_back(new_obj);
    }
}

void ObjLoader::setMaterialOverride(const Material& mat)
{
    material_overridden = true;
    overriden_material = mat;
}

