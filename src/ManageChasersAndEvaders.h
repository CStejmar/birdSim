#ifndef _MANAGECHASERSANDEVADERS_
#define _MANAGECHASERSANDEVADERS_

//OpenGL for Linux and Mac OS X included in these headers
#include "../common/VectorUtils3.h"
#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>    // std::move (ranges)
#include <utility>      // std::move (objects)

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "Evader.h"
#include "Chaser.h"

class ManageChasersAndEvaders
{
 private:
  //~ManageChasersAndEvaders();
  bool mergeFlocks();
  void splitFlock(Evader *flock);
  int nearestFlock(Boid chaser);
  void loadEvaderModels();
  void animate(GLfloat time);
  void animateAndDraw(GLfloat time, mat4 cameraMatrix);
  
  GLuint* shader;
  Model *evaderModel;
  Model *chaserModel;
  vector<Model*> evaderModels;
  GLuint evaderTexture;
  GLuint chaserTexture;

  GLfloat prevTime;
  uint modelIndex;

  //FILE *fp;

 public:
  Chaser* chasers;
  vector<Evader*> flocks;
  //vector<Chaser*> predators;

  ManageChasersAndEvaders(GLuint* phongShader, char *modelPathEvader, char *imagePathEvader, char *modelPathChaser, char *imagePathChaser);
  void update(GLfloat time);  
  void draw(GLfloat time, mat4 cameraMatrix);
};

#endif

