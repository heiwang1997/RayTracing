#pragma once

class Camera;

class RayTracer {
    Camera* camera;
public:
    void run();
    RayTracer() { }
    ~RayTracer() { }
};

