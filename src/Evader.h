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
  void flocking(vector<Boid> chaserVector);
  void cohesion(Boid *boidI, int index);
  void avoidance(Boid* boidI, int index);
  void alignment(Boid* boidI, int index);
  void avoidChaser(Boid* boidI, vector<Boid> chaserVector);
  void followLeader(Boid* boidI);
  void boundPositionBoid(Boid *boid);
  void checkMaxSpeed(Boid *boid);
  void updateLeader();

  vec3 follow, avoidChaserVector;
  
  //char *modelPath;
  //char *imagePath;
  GLuint* shader;
  GLuint texture;
  Model* model;
  
  //vec3 maxSpeed;
  float maxDistance, minDistance, maxSpeed, awarenessRadius;
  float cohesionWeight, avoidanceWeight, alignmentWeight, followWeight, avoidChaserWeight;

  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float zMin;
  float zMax;

 public:
  Boid leader;
  std::vector<Boid> evaderVector; // Change to pointers?

  Evader(GLuint *phongShader, Model *evaderModel, GLuint evaderTexture, vec3 pos, int numOfBoids);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time, vector<Boid> chaserVector);
  void makeFlockOf(int inhabitants, vec3 position);
  
};

#endif

