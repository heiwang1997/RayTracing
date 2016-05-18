#pragma once
#include <vector>
#include "basic.h"
#include "primitive.h"

class Triangle; // from primitive.h

class TriBoundingBox {
public:
    Vector3 min;
    Vector3 max;
    void updateBox(Triangle*);
    void dump() const;
    double getSize(int) const;
    double getArea() const;
    Collision updateCollision(const Ray&, double);
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
    void printInfo() const;
public:
    KDTriNode();
    ~KDTriNode();
};

class KDTriTree {
    KDTriNode* root;
    Collision traverseNode(KDTriNode*, const Ray&, double max_dist);
public:
    void buildTree(const std::vector<Triangle*>& tri_data);
    void divideNode(KDTriNode*, int);
    Collision updateCollision(const Ray&, double max_dist);
    KDTriTree();
    ~KDTriTree();
};




