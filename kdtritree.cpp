#include "kdtritree.h"
#include <algorithm>

#include <iostream>
using namespace std;

// Constant for surface-area-heuristic Algorithm.
static const double SAH_TRAVERSE_COST = 0.3f;
static const double SAH_INTERCEPT_COST = 1.0f;
static const int SAH_SAMPLE_NUM = 8;

KDTriNode::KDTriNode() {
    data = 0;
    left = 0;
    right = 0;
    size = 0;
    type = KDTriNode::X;
}

KDTriNode::~KDTriNode() {

}


KDTriTree::KDTriTree() : root(0) {

}

KDTriTree::~KDTriTree() {

}

void KDTriTree::sortNode(KDTriNode *node) {
    std::sort(node->data, node->data + node->size, triCmp);
}

// ATTENTION: This SAH is not standardized.
// The best coordinates is an approximation.
void KDTriTree::divideNode(KDTriNode *node, int depth) {
    // TODO: stop condition.
    //printf("LEVEL=%d\n", depth);
    if (node->size <= 4 || depth == 0) return;
    //printf("CSIZE=%d\n", node->size);
    // Judge best axis to split.
    int split_axis;
    double sx = node->aabb.getSize(0), sy = node->aabb.getSize(1),
           sz = node->aabb.getSize(2);
    if (sx > sz && sx > sy) split_axis = TriComparer::X;
    else if (sy > sx && sy > sz) split_axis = TriComparer::Y;
    else split_axis = TriComparer::Z;
    triCmp.cmpCoord = (TriComparer::CmpCoord) split_axis;

    // Judge best plane to split.
    double best_cost = INF;
    double best_plane = 0;
    double all_area_coef = 1.0f / node->aabb.getArea();
    TriBoundingBox left_box = node->aabb;
    TriBoundingBox right_box = node->aabb;
    int left_size = 0, right_size = 0;
    // Consider plane is the min coord of all triangles.
    triCmp.cmpBase = TriComparer::MIN;
    sortNode(node);
    int j = 0; // j is the number of tris which cannot be in the right. (mono increase)
    for (int i = 0; i < node->size; ++ i) { // i is the size of left node
        double current_plane = node->data[i]->getMinPos(split_axis);
        while (i + 1 < node->size &&
                std::abs(node->data[i + 1]->getMinPos(split_axis) - current_plane) < DOZ)
            ++ i;
        double current_cost = 0;
        left_box.max[split_axis] = current_plane;
        right_box.min[split_axis] = current_plane;
        while (j < node->size && node->data[j]->getMaxPos(split_axis) <= current_plane + DOZ) ++ j;
        // ASSERT: (node->size - j) is the size of right node.
        //printf("CurrentSituation:\n");
        //node->aabb.dump();
        //left_box.dump(); right_box.dump();
        current_cost = SAH_TRAVERSE_COST + SAH_INTERCEPT_COST * all_area_coef *
                       ((i + 1) * left_box.getArea() + (node->size - j) * right_box.getArea());
        //printf("cc=%lf,i=%d,j=%d\n", current_cost, i, j);
        if (current_cost < best_cost) {
            best_cost = current_cost;
            best_plane = current_plane;
            left_size = i + 1; right_size = node->size - j;
        }
    }
    printf("BC is cal by: leftSize=%d, rightSize=%d, bestPlane=%lf\n", left_size, right_size, best_plane);
    // OR the bst plane is the max coord of all triangles.
    triCmp.cmpBase = TriComparer::MAX;
    sortNode(node);
    j = 0;
    for (int i = 0; i < node->size; ++ i) { // here i is the left node (complete).
        double current_plane = node->data[i]->getMaxPos(split_axis);
        while (i + 1 < node->size &&
               std::abs(node->data[i + 1]->getMaxPos(split_axis) - current_plane) < DOZ)
            ++ i;
        double current_cost = 0;
        left_box.max[split_axis] = current_plane;
        right_box.min[split_axis] = current_plane;
        while (j < node->size && node->data[j]->getMinPos(split_axis) <= current_plane + DOZ) ++ j;
        current_cost = SAH_TRAVERSE_COST + SAH_INTERCEPT_COST * all_area_coef *
                       (j * left_box.getArea() + (node->size - (i + 1)) * right_box.getArea());
        if (current_cost < best_cost) {
            best_cost = current_cost;
            best_plane = current_plane;
            left_size = j; right_size = node->size - (i + 1);
            //printf("SURPISIE!\n");
        }
    }
    // the cost for not splitting
    double lazy_cost = SAH_INTERCEPT_COST * node->size;
    printf("BC is cal by: leftSize=%d, rightSize=%d, bestPlane=%lf\n", left_size, right_size, best_plane);
    //printf("bestCost=%lf, lazyCost=%lf\n", best_cost, lazy_cost);

    if (lazy_cost < best_cost) return;

    // Begin splitting.
    node->plane = best_plane; node->type = (KDTriNode::SplitType) split_axis;
    printf("BEFORE:ls=%d,rs=%d\n", left_size, right_size);
    int ordis = right_size;
    left_size = 0; right_size = 0;
    for (int i = 0; i < node->size; ++ i) {
        if (node->data[i]->getMinPos(split_axis) <= best_plane - DOZ)
            ++ left_size;
        if (node->data[i]->getMaxPos(split_axis) >= best_plane + DOZ)
            ++ right_size;
    }
    if (right_size != ordis) printf("SHIT!");

    printf("AFTER:ls=%d,rs=%d\n", left_size, right_size);
    left_box.max[split_axis] = right_box.min[split_axis] = best_plane;
    node->left = new KDTriNode; node->left->size = left_size;
    node->left->data = new Triangle*[left_size];
    node->left->aabb = left_box;
    node->right = new KDTriNode; node->right->size = right_size;
    node->right->data = new Triangle*[right_size];
    node->right->aabb = right_box;

    int lc = 0, rc = 0;
    for (int i = 0; i < node->size; ++ i) {
        if (node->data[i]->getMinPos(split_axis) <= best_plane + DOZ)
            node->left->data[lc ++] = node->data[i];
        if (node->data[i]->getMaxPos(split_axis) >= best_plane + DOZ)
            node->right->data[rc ++] = node->data[i];
    }
    divideNode(node->left, depth - 1);
    divideNode(node->right, depth - 1);
}

void KDTriTree::buildTree(const std::vector<Triangle *> &tri_data) {
    root = new KDTriNode;
    root->size = tri_data.size();
    root->data = new Triangle*[root->size];
    int i = 0;
    for (std::vector<Triangle*>::const_iterator it = tri_data.begin();
            it != tri_data.end(); ++ it) {
        root->data[i ++] = *it;
        root->aabb.updateBox(*it);
    }
    divideNode(root, 20);
}

void TriBoundingBox::updateBox(Triangle *tri) {
    for (int i = 0; i < 3; ++ i) {
        max[i] = std::max(max[i], tri->getMaxPos(i));
        min[i] = std::min(min[i], tri->getMinPos(i));
    }
}

double TriBoundingBox::getSize(int axis) const {
    return max.getAttr(axis) - min.getAttr(axis);
}

double TriBoundingBox::getArea() const {
    double d[3]; for (int i = 0; i < 3; ++ i) d[i] = getSize(i);
    return 2 * (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]);
}


bool TriComparer::operator()(const Triangle *a, const Triangle *b) const {
    if (cmpBase == MAX) return a->getMaxPos((int)cmpCoord) < b->getMaxPos((int)cmpCoord);
    else return a->getMinPos((int)cmpCoord) < b->getMinPos((int)cmpCoord);
}

void TriBoundingBox::dump() const {
    printf("Box (\n");
    min.dump();
    max.dump();
    printf(")\n");
}

