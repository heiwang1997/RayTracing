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
       Ray(Vector3(1.0, -3, 0.7), Vector3(0, 0.5, 0));
static double DEFAULT_CAMERA_FOCAL_DISTANCE = 2.7f;
static double DEFAULT_CAMERA_APERTURE = 0.1f;

/**************************************
 *            SCENE DEFAULTS          *
 **************************************/
static Color DEFAULT_SCENE_BACKGROUND_COLOR = Color(0, 0, 0);
static int DEFAULT_SCENE_LIGHT_SAMPLE = 4;
static int DEFAULT_SCENE_DIFFUSE_REFLECTION_SAMPLE = 64;
static bool DEFAULT_SCENE_SOFT_SHADOW = true;
static int DEFAULT_SCENE_DOF_QUALITY = 0;
/**************************************
 *         MATERIAL DEFAULTS          *
 **************************************/
static double DEFAULT_MATERIAL_SHINENESS = 10.0f;
static double DEFAULT_MATERIAL_DIFFUSE = 0.35f;
static double DEFAULT_MATERIAL_SPECULAR = 0.2f;
static double DEFAULT_MATERIAL_AMBIENT = 0.08f;
static double DEFAULT_MATERIAL_REFLECTION = 0.0f;
static double DEFAULT_MATERIAL_DEFUSE_REFLECTION = 0.0f;
static double DEFAULT_MATERIAL_REFRACTION = 0.0f;
static double DEFAULT_MATERIAL_RINDEX = 1.33f;
static double DEFAULT_MATERIAL_DENSITY = 1.3f;
static Color DEFAULT_MATERIAL_COLOR = Color(0, 0, 0);

/**************************************
 *          TEXTURE DEFAULTS          *
 **************************************/
static double DEFAULT_TEXTURE_SCALE_X = 1.0f;
static double DEFAULT_TEXTURE_SCALE_Y = 1.0f;
static double DEFAULT_TEXTURE_OFFSET_X = 0.0f;
static double DEFAULT_TEXTURE_OFFSET_Y = 0.0f;

/**************************************
 *            LIGHT DEFAULTS          *
 **************************************/
static Color DEFAULT_LIGHT_COLOR = Color(255, 255, 255);
static double DEFAULT_LIGHT_SIZE = 0.5f;
/**************************************
 *             MESH DEFAULTS          *
 **************************************/
static Vector3 DEFAULT_MESH_POS = Vector3(0, 0, 0);
static Vector3 DEFAULT_MESH_ROTATION = Vector3(0, 0, 0);
static double DEFAULT_MESH_SCALE = 1.0f;