#ifndef _MANAGECHASERSANDEVADERS_
#define _MANAGECHASERSANDEVADERS_

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

#include <cmath>
#include <iostream>
#include <algorithm>    // std::move (ranges)
#include <utility>      // std::move (objects)

#include "Evader.h"
#include "Chaser.h"

class ManageChasersAndEvaders
{
 private:
  //~ManageChasersAndEvaders();
  bool mergeFlocks();
  void splitFlock(Evader *flock);
  GLuint* shader;
  Model *evaderModel;
  Model *chaserModel;
  GLuint evaderTexture;
  GLuint chaserTexture;

 public:
  Chaser* chasers;
  vector<Evader*> flocks;
  //vector<Chaser*> predators;

  ManageChasersAndEvaders(GLuint* phongShader, char * modelPathEvader, char *imagePathEvader, char * modelPathChaser, char *imagePathChaser);
  void update(GLfloat time);  
  void draw(mat4 cameraMatrix);
};

#endif

