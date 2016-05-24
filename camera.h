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

    double focal_distance;
    double aperture;
public:
    int getImgW() const;
    int getImgH() const;
    double getWindowW() const;
    double getWindowH() const;
    void loadAttr(FILE*);
    Ray emit(double sx, double sy);
    Ray emitDOF(const Ray&);
    Camera();
    ~Camera() {}
};

