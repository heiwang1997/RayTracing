#pragma once
/*   CAMERA SCHEME:
 *   screen |\./| window (virtual world.)
 *          |/ \|
 *            ^
 *            |
 *          sensitization point
 * */
#include "basic.h"

class Camera
{
private:
    int img_h;
    int img_w;
    double window_h;
    double window_w;
    Ray world_position;

    Vector3 dx;
    Vector3 dy;

    Vector3 lookat_pos;
    double angle_dist;

    double focal_distance;
    double aperture;
    int photons_emitted;
    int hitpointmap_coef;
    double init_radius;
    int ppm_iterations;
    double ppm_alpha;
    double rotation;

    void initCamera();
public:
    int getImgW() const;
    int getImgH() const;
    double getWindowW() const;
    double getWindowH() const;
    int getPhotonsEmitted() const;
    int getHitpointmapCoef() const;
    double getInitRadius() const;
    int getPPMIterations() const;
    double getPPMAlpha() const;
    void loadAttr(FILE*);
    Ray emit(double sx, double sy);
    Ray emitDOF(const Ray&);
    Camera();
    ~Camera() {}
};

