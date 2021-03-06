
#include "Camera.h"

Camera::Camera(vec3 pos, GLfloat vel, GLfloat sens)
{
  vec3 r = vec3(50,0,50);
  position = pos;
  lookAtPoint = VectorAdd(position,r);
  upVector = vec3(0,1,0);

  //cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = 0.5;

  projectionNear = 0.8;
  projectionFar = 2250.0;
  projectionRight = 0.5;
  projectionLeft = -0.5;
  projectionTop = 0.5;
  projectionBottom = -0.5;


  initKeymapManager();

  cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = vel;
  sensitivity = sens;

  projectionMatrix = frustum(projectionLeft, projectionRight, projectionBottom, projectionTop,projectionNear, projectionFar);

  timer = 30;
  followFlock = false;
  flockIndex = -1; // Is set to zero first time we choose to follow a flock.

}

Camera::Camera(float left, float right, float bottom, float top, float near, float far)
{
  vec3 r = SetVector(-0.5,0,-0.5);
  //position = SetVector(0,0,0);
  lookAtPoint = VectorAdd(position,r);
  upVector = SetVector(0,1,0);

  //cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = 0.5;

  projectionLeft = left;
  projectionRight = right;
  projectionBottom = bottom;
  projectionTop = top;
  projectionNear = near;
  projectionFar = far;
}


void Camera::handleKeyPress()
{
  if(keyIsDown('w'))
  {
    vec3 w = Normalize(VectorSub(lookAtPoint,position));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
    position = VectorAdd(position,ScalarMult(w,velocity));
  }
  if(keyIsDown('s'))
  {
    vec3 s = Normalize(VectorSub(position,lookAtPoint));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(s,velocity));
    position = VectorAdd(position,ScalarMult(s,velocity));
  }
  if(keyIsDown('a'))
  {
    vec3 w = VectorSub(lookAtPoint,position);
    vec3 a = Normalize(CrossProduct(upVector,w));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(a,velocity));
    position = VectorAdd(position,ScalarMult(a,velocity));
  }
  if(keyIsDown('d'))
  {
    vec3 w = VectorSub(lookAtPoint,position);
    vec3 d = Normalize(CrossProduct(w,upVector));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(d,velocity));
    position = VectorAdd(position,ScalarMult(d,velocity));
  }
  if(keyIsDown('o') && timer > 30)
  {
    timer = 0;
    followFlock = true;
    flockIndex++; // Follow next flock each time 'o' is pressed.
  }
  if(keyIsDown('O') && followFlock)
    {
      followFlock = false;
      flockIndex--; // Next time 'o' is pressed, we will follow the previous followed flock.
    }

  timer++;

  //cameraMatrix = lookAtv(position,lookAtPoint,upVector); // In update!

}

void Camera::handleMouse(int x, int y)
{
  GLfloat xtemp = (GLfloat)(-x + SCREEN_WIDTH / 2) / (10000 / sensitivity);
  GLfloat ytemp = (GLfloat)(-y + SCREEN_HEIGHT / 2) / (10000 / sensitivity);

  vec3 r = Normalize(VectorSub(lookAtPoint, position)); // Forward Direction
  vec3 d = Normalize(CrossProduct(r, upVector)); // Right direction
  mat4 phiRot = ArbRotate(upVector, xtemp);
  r = MultVec3(phiRot, r);
  mat4 thetaRot = ArbRotate(d, ytemp);
  r = MultVec3(thetaRot, r);

  vec3 temp = CrossProduct(r, upVector);
  //std::cout << "temp.x = " << std::abs(temp.x) << std::endl;
  //std::cout << "temp.y = " << std::abs(temp.y) << std::endl;
  //std::cout << "temp.z = " << std::abs(temp.z) << std::endl;
  if(std::abs(temp.x)  > 0.09 || std::abs(temp.y) > 0.09 || std::abs(temp.z) > 0.09) // Still possible to get 180 degree flip! But much better.
    lookAtPoint = VectorAdd(position, r);

  //upVector = MultVec3(thetaRot,upVector); // for flight simulator
  //cameraMatrix = lookAtv(position,lookAtPoint,upVector); // In update!

#ifdef __APPLE__
  glutWarpPointer(SCREEN_WIDTH/2, (SCREEN_HEIGHT/2)-520); // On mac the pointer is shifted 520 pixels (why?)
  glutHideCursor();
#else
  glutWarpPointer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2); // Ger delay med linux!
#endif

}

void Camera::update()
{
  //vec3 r = Normalize(VectorSub(lookAtPoint, position)); // Forward Direction
  //vec3 temp = CrossProduct(upVector, r);
  // std::cout << "temp.x = " << std::abs(temp.x) << std::endl;
  // std::cout << "temp.y = " << std::abs(temp.y) << std::endl;
  // std::cout << "temp.z = " << std::abs(temp.z) << std::endl;

  //if(std::abs(temp.x)  > 0.09 || std::abs(temp.y) > 0.09 || std::abs(temp.z) > 0.09)
  cameraMatrix = lookAtv(position,lookAtPoint,upVector);
}
