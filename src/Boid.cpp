
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

  rotationMatrix = IdentityMatrix();
}

void Boid::setDirection()
{

  //vec3 p = vec3(averagePosition.x*50, averagePosition.y, averagePosition.z*50);
  direction = position - averagePosition;

}

void Boid::setRotation()
{
  float degrees = 30;
  // if(direction.x > 0 && direction.y > 0 && direction.z > 0)
  //   degrees += 10.0;
  // else
  //   degrees -= 10.0;
  
  // mat4 xRot = Rx(degrees);
  // mat4 yRot = Ry(degrees);
  rotationMatrix = ArbRotate(direction, degrees); //Mult(yRot,xRot);

}

/*Boid::~Boid()
{

}*/

void Boid::draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture)
{
  mat4 translationMatrix = T(position.x, position.y, position.z);
  mat4 transformationMatrix = Mult(translationMatrix, rotationMatrix);
  glUseProgram(*shader);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, transformationMatrix.m);
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
