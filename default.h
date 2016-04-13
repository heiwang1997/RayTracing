#pragma once
#include "basic.h"

/**************************************
 *         CAMERA DEFAULTS            *
 **************************************/
static int DEFAULT_CAMERA_IMG_W = 400;
static int DEFAULT_CAMERA_IMG_H = 300;
static double DEFAULT_CAMERA_WINDOW_W = 1.2f;
static double DEFAULT_CAMERA_WINDOW_H = 0.8f;
static double DEFAULT_CAMERA_ROTATE = 0.0f;
static Ray DEFAULT_CAMERA_WORLD_POSITION = \
       Ray(Vector3(0, 0, 0), Vector3(0, 1, 0));