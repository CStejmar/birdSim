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
#include <random>

using namespace std;

class Evader
{
 private:
  void flocking();
  void cohesion(Boid *boidI, int index);
  void avoidance(Boid* boidI, int index);
  void alignment(Boid* boidI, int index);
  void followLeader(Boid* boidI);
  void boundPositionBoid(Boid *boid);
  void checkMaxSpeed(Boid *boid);
  void updateLeader();

  vec3 follow;

  Boid leader;
  
  char *modelPath;
  char *imagePath;
  GLuint* shader;
  GLuint texture;
  Model* model;
  std::vector<Boid> evaderVector; // Change to pointers?
  
  //vec3 maxSpeed;
  float maxDistance, minDistance, maxSpeed;
  float cohesionWeight, avoidanceWeight, alignmentWeight, followWeight;

  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float zMin;
  float zMax;

 public:
  Evader(GLuint *phongShader, char * modelPath, char *imagePath, vec3 pos, int numOfBoids);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time);
  void makeFlockOf(int inhabitants, vec3 position);
  
};

#endif

