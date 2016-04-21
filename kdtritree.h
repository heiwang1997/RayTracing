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

class TriComparer {
public:
    enum CmpCoord {X = 0, Y, Z} cmpCoord;
    enum CmpBase {MAX = 0, MIN} cmpBase;
    bool operator() (const Triangle* a, const Triangle* b) const;
};

class KDTriTree {
    KDTriNode* root;
    TriComparer triCmp;
public:
    void buildTree(const std::vector<Triangle*>& tri_data);
    void divideNode(KDTriNode*, int);
    void sortNode(KDTriNode*);
    KDTriTree();
    ~KDTriTree();
};




