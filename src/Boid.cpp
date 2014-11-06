
#include "Boid.h"

Boid::Boid(vec3 pos)
{
  position = vec3(pos.x, pos.y, pos.z);
  averagePosition = vec3(0.0, 0.0, 0.0);
  
  speed = vec3(0.5, 0.1, 0.6);
  speedDifference = vec3(0.0, 0.0, 0.0);
  
  avoidanceVector = vec3(0.0, 0.0, 0.0);

  lookAtPoint = vec3(60.0,60.0,60.0);
  direction = VectorSub(position,lookAtPoint); //?
}

/*Boid::~Boid()
{

}*/

void Boid::draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture)
{
  mat4 modelView = T(position.x, position.y, position.z);
  glUseProgram(*shader);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glBindTexture(GL_TEXTURE_2D, *texture);	
  DrawModel(model, *shader, "inPosition", "inNormal", "inTexCoord");
}

void Boid::animate(GLfloat time)
{
  float xSpeed = cos(time);
  float ySpeed = 2*sin(time);
  float zSpeed = sin(time);

  //speed = vec3(xSpeed,ySpeed,zSpeed);

  //position += speed;
}
