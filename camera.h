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
    int img_h;
    int img_w;
    double window_h;
    double window_w;
    Ray world_position;

    Vector3 dx;
    Vector3 dy;
public:
    Ray emit(double wx, double wy); // based on window pos
    Camera();
    ~Camera() {}
};

