#ifndef _EVADER_
#define _EVADER_

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

#include "Boid.h"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Evader
{
 private:
  void cohesion(Boid *boidI, int index);
  void avoidance(Boid* boidI, int index);
  void alignment(Boid* boidI, int index);
  void flocking2();
  void boundPosition(Boid *b);

  float maxSpeed;

  vec3 position;
  vec3 lookAtPoint;
  
  char *modelPath;
  char *imagePath;
  GLuint* shader;
  GLuint texture;
  Model* model;
  std::vector<Boid> evaderVector; // Change to pointers?

  float maxDistance, minDistance;
  float cohesionWeight, avoidanceWeight, alignmentWeight;

 public:
  Evader(GLuint *phongShader, char * modelPath, char *imagePath);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time);
  void flocking();
  void makeFlockOf(int inhabitants);
  vec3 calculateAvoidance(vec3 posi, vec3 posj);
  
};

#endif

