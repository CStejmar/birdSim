#ifndef _CHASER_
#define _CHASER_

#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"

#include "Boid.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

class Chaser
{
 private:
  void searchPrey(int chaserIndex, vector<Boid> evaderVector);
  
  void avoidance(Boid* boidI, int index);
  void attack(Boid* boidI, vector<Boid> evaders);
  void boundPositionBoid(Boid *boid);
  void checkMaxSpeed(Boid *boid);
  void setRandomSpeed(Boid *boidI);
  bool insideView(Boid chaser, Boid evader);
  void updateBoundingPositions(vec3 cameraPosition);

  vec3 attackVector, nearest;
  
  GLuint* shader;
  Model* model;
  GLuint texture;
  
  float awarenessRadius, minDistance, maxSpeed;
  float avoidanceWeight, attackWeight, nearestWeight;

  float lowInterval, highInterval;

  // Bounding box
  float xMin, xMax;
  float yMin, yMax;
  float zMin, zMax;

 public:
  std::vector<Boid> chaserVector; // Change to pointers?

  Chaser(GLuint *phongShader, Model *chaserModel, GLuint chaserTexture, vec3 pos, int numOfBoids, vec3 cameraPosition);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time, int chaserIndex, vector<Boid> evaderVector, vec3 cameraPosition);
  void makeIndividuals(int inhabitants, vec3 position);
  
};

#endif

