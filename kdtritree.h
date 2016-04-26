#pragma once
#include <vector>
#include "basic.h"
#include "primitive.h"

class Triangle; // from primitive.h

class TriBoundingBox {
public:
    Vector3 min;
    Vector3 max;
    double collision_distance;
    void updateBox(Triangle*);
    void dump() const;
    double getSize(int) const;
    double getArea() const;
    bool updateCollision(const Ray&);
    bool inside(const Vector3&) const;
    TriBoundingBox() : min(INF, INF, INF), max(-INF, -INF, -INF) {}
    ~TriBoundingBox() {}
};

class KDTriNode {
    friend class KDTriTree;
    Triangle** data;
    int size;
    TriBoundingBox aabb;
    enum SplitType {U, X = 0, Y, Z} type;
    double plane;
    KDTriNode* left;
    KDTriNode* right;
public:
    KDTriNode();
    ~KDTriNode();
};

class KDTriTree {
    KDTriNode* root;
    Triangle* traverseNode(KDTriNode*, const Ray&);
public:
    void buildTree(const std::vector<Triangle*>& tri_data);
    void divideNode(KDTriNode*, int);
    Triangle* updateCollision(const Ray&);
    KDTriTree();
    ~KDTriTree();
};




