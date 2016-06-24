# RayTracing and Progressive Photon Mapping
A simple shader using RayTracing and Progressive Photon Mapping

Some pictures the Raytracer generated are as the following:

![image](https://github.com/heiwang1997/RayTracing/raw/master/generated/PPM-EX1.bmp)
![image](https://github.com/heiwang1997/RayTracing/raw/master/generated/PPM-EX2.bmp)

Note: This is only a project for my Computer Graphics courses.
The robustness is NOT tested and I may not take time polishing
this set of code further. However, the project is open source and
any contributions are welcome.

## Acknowledgements
The image/texture load & write module the project is currently using includes:
- OpenCV
- Stb::image & Stb::image_writer

The .obj loader is from:
- Tiny Obj Loader

## Usage
Create a scene file and get started. 

After you are done with the scene file, compile the program using CMake
(make sure you have a compiler supporting C++ 11 because multithread is used.)
and type in the scene name into the command line.

A standard scene file looks like this:

    # Use '#' to comment.
    # The format for the scene file looks just like JSON file.
    
    # define camera parameters
    Camera: { 
    	# the width of image generated
    	width: 1280 
    	# the height of image generated
    	height: 720 
    	# the position of the camera
    	position: 5 -5 5 
    	# the position the camera looks at
    	lookAt: 5 5 5 
    	# distance between the viewpoint the the viewplane
    	angleDist: 3 
    
    	# PPM-specified configurations
    	# Use PPM renderer to enable the following features
    	# the size factor of hit point map
    	HitPointMapCoef: 6
    	# numbers of photons emiited per round 
    	Photons: 6000000   
    	# initial sampling radius 
    	PMRadius: 0.12     
    	# number of rounds
    	PPMIterations: 5000 
    }
    
    Scene: { # define scene parameters
    
    	# RayTracing-specified configurations
    	# '0' to disable soft shadow and '1' will enable it
    	SoftShadow: 1 
    	# number of light samples, it defines the quality of soft shadow
    	LightSample: 1 
    	# diffuse Reflections samples
    	DiffuseReflectionSample: 12 
    	# quality of depth of field
    	DepthOfFieldSample: 0 
    	
    	# General Configurations of the Scene.
    
    	# Environment Map.
    	EnvironmentMap: {
    		# name of file
    		name: "env.jpg"
    		# scaling
    		scaleX: 100
    		scaleY: 100
    		# offsets
    		offsetX: 0
    		offsetY: 0
    	}
    
    	# Light (Square Light)
    	Light: {
    		position: 5 5 12
    		# size of light
    		size: 4.0
    		# 255 (8-bit) notation
    		color: 255 255 255
    	}
    
    	# Plane (Infinite)
    	Plane: {
    		# normal Vector of the plane
    		# the plane is defined by:
    		# ax + by + cz + d = 0
    		# while normal represents (a, b, c) and offset = d
    		normal: 0 0 1
    		offset: 0
    
    		#material settings
    		material: {
    			color: 255 128 255
    
    			# The following using a 0-1 scale.
    			# Be sure the sum of following does not exceed 1
    			reflection: 0.1
    			refraction: 0.9
    			rindex: 1.7 
    			ambient: 0
    			specular: 0
    			diffusion: 0
    			ambient: 0
    			# the offset of DiffuseReflection
    			diffuseReflection: 0.1
    			# define the texture placed on the object.
    			texture: {
    				# Same as Environment Map.
    				name: "ground_tex.jpg"
    				offsetX: 0
    				offsetY: 0
    				scaleX: 1
    				scaleY: 1
    			}
    		}
    	}
    
    	# Sphere
    	Sphere: {
    		# position of the ball center
    		position: 6.5 -1 1.5
    		radius: 1
    		# define the direction of the texture
    		NorthPole: 1 1 0
    		EastPole: 0 0 1
    		material: {
    			color: 255 255 255
    			refraction: 0.9
    			rindex: 1.7
    			ambient: 0
    			diffusion: 0
    			specular: 0
    			reflection: 0.1
    		}
    	}
    
    	# Object
    	Object: {
    		file: "XXX.obj"
    		scale: 3
    		position: 5 5 0
    		# rotation (rx, ry, rz)
    		rotation: 0 0 45
    		# you can also use "Material: {}" to overwrite the origin material in .mtl file
    	}
    
    }
