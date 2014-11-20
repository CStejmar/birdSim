#ifndef _BOID_
#define _BOID_

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "../common/mac/MicroGlut.h"
#elif _WIN32
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <GL/freeglut.h>
	#include <GL/gl.h>
#elif __linux
	#include <GL/gl.h>
	#include "../common/linux/MicroGlut.h"
#endif

#include "../common/VectorUtils3.h"
#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"

#include <cmath>
#include <iostream>

class Boid
{
 private:
  
  
  //~Boid();

 public:

  vec3 position, averagePosition;
  vec3 speed, speedDifference;

  vec3 avoidanceVector;
  vec3 cohesionVector;
  vec3 alignmentVector;

  vec3 lookAtPoint;
  vec3 direction;

  mat4 rotationMatrix;

  Boid(vec3 pos);
  void draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture);
  void animate(GLfloat time);
  void setDirection();
  void setRotation();
  
};

#endif

