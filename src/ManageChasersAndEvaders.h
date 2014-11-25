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
void splitFlock();

 public:
  Chaser* chasers;
  vector<Evader*> flocks;
  //vector<Chaser*> predators;

  ManageChasersAndEvaders(GLuint* shader);
  void update(GLfloat time);  
  void draw(mat4 cameraMatrix);
};

#endif

