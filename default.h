#pragma once
#include "basic.h"

/**************************************
 *         CAMERA DEFAULTS            *
 **************************************/
static int DEFAULT_CAMERA_IMG_W = 800;
static int DEFAULT_CAMERA_IMG_H = 570;
static double DEFAULT_CAMERA_WINDOW_W = 1.2f;
static double DEFAULT_CAMERA_WINDOW_H = 0.8f;
static double DEFAULT_CAMERA_ROTATE = 0.0f;
static Ray DEFAULT_CAMERA_WORLD_POSITION = \
       Ray(Vector3(10, -13, 6), Vector3(-2, 2.4, -1));

/**************************************
 *            SCENE DEFAULTS          *
 **************************************/
static Color DEFAULT_SCENE_BACKGROUND_COLOR = Color(0, 0, 0);

/**************************************
 *         MATERIAL DEFAULTS          *
 **************************************/
static double DEFAULT_MATERIAL_SHINENESS = 10.0f;
static double DEFAULT_MATERIAL_DIFFUSE = 0.35f;
static double DEFAULT_MATERIAL_SPECULAR = 0.2f;
static double DEFAULT_MATERIAL_REFLECTION = 0.5f;
static Color DEFAULT_MATERIAL_COLOR = Color(128, 128, 128);
/**************************************
 *            LIGHT DEFAULTS          *
 **************************************/
static Color DEFAULT_LIGHT_COLOR = Color(255, 255, 255);