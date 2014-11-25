#ifndef _CHASER_
#define _CHASER_

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

class Chaser
{
 private:
  void searchPrey(vector<Boid> evaderVector);
  
  void avoidance(Boid* boidI, int index);
  void attack(Boid* boidI, vector<Boid> evaders);
  void boundPositionBoid(Boid *boid);
  void checkMaxSpeed(Boid *boid);
  void setRandomSpeed(Boid *boidI);

  vec3 attackVector;
  
  char *modelPath;
  char *imagePath;
  GLuint* shader;
  GLuint texture;
  Model* model;
  
  float awarenessRadius, minDistance, maxSpeed;
  float avoidanceWeight, attackWeight;

  float lowInterval;
  float highInterval;

  // Bounding box
  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float zMin;
  float zMax;

 public:
  std::vector<Boid> chaserVector; // Change to pointers?

  Chaser(GLuint *phongShader, char * modelPath, char *imagePath, vec3 pos, int numOfBoids);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time, vector<Boid> evaderVector);
  void makeIndividuals(int inhabitants, vec3 position);
  
};

#endif

