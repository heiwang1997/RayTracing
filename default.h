#pragma once
#include "basic.h"

/**************************************
 *         CAMERA DEFAULTS            *
 **************************************/
static int DEFAULT_CAMERA_IMG_W = 800;
static int DEFAULT_CAMERA_IMG_H = 550;
static double DEFAULT_CAMERA_WINDOW_W = 1.2f;
static double DEFAULT_CAMERA_WINDOW_H = 0.8f;
static double DEFAULT_CAMERA_ROTATE = 0.0f;
static Ray DEFAULT_CAMERA_WORLD_POSITION = \
       Ray(Vector3(-1, -1, 2), Vector3(1, 1, -1));

/**************************************
 *         MATERIAL DEFAULTS          *
 **************************************/
static double DEFAULT_MATERIAL_SHINENESS = 1.0f;
static double DEFAULT_MATERIAL_DIFFUSE = 0.45f;
static double DEFAULT_MATERIAL_SPECULAR = 0.2f;