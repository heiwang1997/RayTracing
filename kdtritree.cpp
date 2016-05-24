#include "kdtritree.h"
//#define DIVISION_USE_SAH

// Constant for surface-area-heuristic Algorithm.
static const double SAH_TRAVERSE_COST = 0.3f;
static const double SAH_INTERCEPT_COST = 1.0f;
static const int SAH_SAMPLE_NUM = 8;

double max3(double a, double b, double c) {
    return std::max(std::max(a, b), c);
}

double min3(double a, double b, double c) {
    return std::min(std::min(a, b), c);
}

KDTriNode::KDTriNode() {
    data = 0;
    left = 0;
    right = 0;
    size = 0;
    type = KDTriNode::X;
}

KDTriNode::~KDTriNode() {
    if (size != 0 && data) delete[] data;
    delete left;
    delete right;
}

void KDTriNode::printInfo() const {
    printf("=== Node: %p ===\n", this);
    printf("lc = %p, rc = %p\n", this->left, this->right);
    printf("split_plane = %lf, axis = %d\n", this->plane, this->type);
    printf("BoundingBox MinX=%lf, MinY=%lf\n", aabb.min.getAttr(0), aabb.min.getAttr(1));
    printf("BoundingBox MaxX=%lf, MaxY=%lf\n", aabb.max.getAttr(0), aabb.max.getAttr(1));
    if (left) left->printInfo();
    if (right) right->printInfo();
}

KDTriTree::KDTriTree() : root(0) {

}

KDTriTree::~KDTriTree() {
    if (root)
        for (int i = 0; i < root->size; ++ i)
            delete root->data[i];
    delete root;
}

// ATTENTION: This SAH is not standardized.
// The best coordinates is an approximation.
void KDTriTree::divideNode(KDTriNode *node, int depth) {
#ifdef DIVISION_USE_SAH
    if (node == 0 || node->size <= 64 || depth == 0) return;
    // Judge best axis to split.
    int split_axis;
    double sx = node->aabb.getSize(0), sy = node->aabb.getSize(1),
            sz = node->aabb.getSize(2);
    if (sx > sz && sx > sy) split_axis = 0;
    else if (sy > sx && sy > sz) split_axis = 1;
    else split_axis = 2;

    // Judge best plane to split.
    double best_cost = INF;
    double best_plane = 0;
    double all_area_coef = 1.0f / node->aabb.getArea();
    int best_left_size = 0, best_right_size = 0;
    TriBoundingBox left_box = node->aabb;
    TriBoundingBox right_box = node->aabb;

    // Make a list of splitting planes.
    int split_count = node->size * SAH_SAMPLE_NUM / 1024 + 1;
    double split_step = node->aabb.getSize(split_axis) / split_count;
    double current_plane = 0.0f;
    while (split_count --) {
        int left_size = 0, right_size = 0;
        left_box.max[split_axis] = right_box.min[split_axis] = current_plane;
        for (int i = 0; i < node->size; ++ i) {
            if (node->data[i]->getMinPos(split_axis) < current_plane + DOZ)
                ++ left_size;
            if (node->data[i]->getMaxPos(split_axis) > current_plane - DOZ)
                ++ right_size;
        }
        double current_cost = SAH_TRAVERSE_COST + SAH_INTERCEPT_COST * all_area_coef *
                    (left_size * left_box.getArea() + right_size * right_box.getArea());
        if (current_cost < best_cost) {
            best_cost = current_cost;
            best_plane = current_plane;
            best_left_size = left_size;
            best_right_size = right_size;
        }
        current_plane += split_step;
    }

    // the cost for not splitting
    double lazy_cost = SAH_INTERCEPT_COST * node->size;
    if (lazy_cost < best_cost) return;

    // Start Splitting.
    node->plane = best_plane; node->type = (KDTriNode::SplitType) split_axis;
    left_box.max[split_axis] = right_box.min[split_axis] = best_plane;

    if (best_left_size > 0) {
        node->left = new KDTriNode;
        node->left->size = best_left_size;
        node->left->data = new Triangle *[best_left_size];
        node->left->aabb = left_box;
    }

    if (best_right_size > 0) {
        node->right = new KDTriNode;
        node->right->size = best_right_size;
        node->right->data = new Triangle *[best_right_size];
        node->right->aabb = right_box;
    }

    int lc = 0, rc = 0;
    for (int i = 0; i < node->size; ++ i) {
        if (node->data[i]->getMinPos(split_axis) < best_plane + DOZ)
            node->left->data[lc ++] = node->data[i];
        if (node->data[i]->getMaxPos(split_axis) > best_plane - DOZ)
            node->right->data[rc ++] = node->data[i];
    }
    printf("Left Size = %d, Right Size = %d, Depth = %d, Best Plane = %lf, Axis = %d\n",
           best_left_size, best_right_size, depth, best_plane, split_axis);
    divideNode(node->left, depth - 1);
    divideNode(node->right, depth - 1);

#else
    // TODO: split axis can judge.
    if (node == 0 || node->size <= 64 || depth == 0) return;
    // Judge best axis to split.
    int split_axis = (8 - depth) % 3;

    // Judge best plane to split.
    double best_plane = node->aabb.min[split_axis] + node->aabb.getSize(split_axis) / 2;
    TriBoundingBox left_box = node->aabb;
    TriBoundingBox right_box = node->aabb;
    node->plane = best_plane; node->type = (KDTriNode::SplitType) split_axis;
    left_box.max[split_axis] = right_box.min[split_axis] = best_plane;

    int left_size = 0, right_size = 0;
    for (int i = 0; i < node->size; ++ i) {
        if (node->data[i]->getMinPos(split_axis) < best_plane + DOZ)
            ++ left_size;
        if (node->data[i]->getMaxPos(split_axis) > best_plane - DOZ)
            ++ right_size;
    }

    if (left_size > 0) {
        node->left = new KDTriNode;
        node->left->size = left_size;
        node->left->data = new Triangle *[left_size];
        node->left->aabb = left_box;
    }

    if (right_size > 0) {
        node->right = new KDTriNode;
        node->right->size = right_size;
        node->right->data = new Triangle *[right_size];
        node->right->aabb = right_box;
    }

    int lc = 0, rc = 0;
    for (int i = 0; i < node->size; ++ i) {
        if (node->data[i]->getMinPos(split_axis) < best_plane + DOZ)
            node->left->data[lc ++] = node->data[i];
        if (node->data[i]->getMaxPos(split_axis) > best_plane - DOZ)
            node->right->data[rc ++] = node->data[i];
    }

    //printf("Left Size = %d, Right Size = %d\n", left_size, right_size);
    divideNode(node->left, depth - 1);
    divideNode(node->right, depth - 1);
#endif
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
    divideNode(root, 8);
}

Collision KDTriTree::updateCollision(const Ray &c_ray, double max_dist) {
    if (root->aabb.updateCollision(c_ray, max_dist).collide())
        return traverseNode(root, c_ray, max_dist);
    return Collision();
}

Collision KDTriTree::traverseNode(KDTriNode *node, const Ray &c_ray, double max_dist) {
    if (node == 0) return Collision();
    if (node->left == 0 && node->right == 0) {
        Collision result;
        for (int i = 0; i < node->size; ++ i) {
            Collision test = node->data[i]->updateCollision(c_ray, max_dist);
            if (test.collide() && node->aabb.inside(test.pos) &&
                (!result.collide() || (result.collide() && test.distance < result.distance)))
                result = test;
        }
        return result;
    }

    if (node->left && node->left->aabb.inside(c_ray.source)) {
        Collision col = traverseNode(node->left, c_ray, max_dist);
        if (col.collide()) return col;
        return traverseNode(node->right, c_ray, max_dist);
    }
    if (node->right && node->right->aabb.inside(c_ray.source)) {
        Collision col = traverseNode(node->right, c_ray, max_dist);
        if (col.collide()) return col;
        return traverseNode(node->left, c_ray, max_dist);
    }

    Collision lc, rc;
    if (node->left) lc = node->left->aabb.updateCollision(c_ray, max_dist);
    if (node->right) rc = node->right->aabb.updateCollision(c_ray, max_dist);

    if (!lc.collide()) return traverseNode(node->right, c_ray, max_dist);
    if (!rc.collide()) return traverseNode(node->left, c_ray, max_dist);

    if (lc.distance < rc.distance) {
        Collision col = traverseNode(node->left, c_ray, max_dist);
        if (col.collide()) return col;
        return traverseNode(node->right, c_ray, max_dist);
    } else {
        Collision col = traverseNode(node->right, c_ray, max_dist);
        if (col.collide()) return col;
        return traverseNode(node->left, c_ray, max_dist);
    }
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

void TriBoundingBox::dump() const {
    printf("Box (\n");
    min.dump();
    max.dump();
    printf(")\n");
}

Collision TriBoundingBox::updateCollision(const Ray &c_ray, double max_dist) {
    // if inside, dist = -1, so that a test will sure be made.
    Collision result;
    //if (inside(c_ray.source)) {
    //    result.hit_type = Collision::INSIDE;
    //    result.distance = -1; // TODO: Maybe not robust
    //    return result;
    //}
    double near[3];
    double far[3];
    for (int i = 0; i < 3; ++ i) {
        double t1 = (min[i] - DOZ - c_ray.source.getAttr(i)) / c_ray.direction.getAttr(i);
        double t2 = (max[i] + DOZ - c_ray.source.getAttr(i)) / c_ray.direction.getAttr(i);
        near[i] = std::min(t1, t2); far[i] = std::max(t1, t2);
    }
    double tmin = max3(near[0], near[1], near[2]),
           tmax = min3(far[0], far[1], far[2]);
    if (tmin > tmax || tmin > max_dist) return Collision();
    result.distance = tmin;
    result.hit_type = Collision::OUTSIDE;
    return result;

}

bool TriBoundingBox::inside(const Vector3 &v) const {
    return (v.getAttr(0) < max.getAttr(0) + DOZ && v.getAttr(0) > min.getAttr(0) - DOZ) &&
           (v.getAttr(1) < max.getAttr(1) + DOZ && v.getAttr(1) > min.getAttr(1) - DOZ) &&
           (v.getAttr(2) < max.getAttr(2) + DOZ && v.getAttr(2) > min.getAttr(2) - DOZ);
}

