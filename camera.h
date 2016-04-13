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
public:
    Ray emit(double sx, double sy);
    Camera();
    ~Camera() {}
};

