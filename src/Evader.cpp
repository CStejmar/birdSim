
#include "Evader.h"

Evader::Evader(GLuint *phongShader, char * mdlPath, char *imgPath, vec3 pos, int numOfBoids)
{
  shader = phongShader;

  modelPath = mdlPath;
  imagePath = imgPath;

  glUseProgram(*shader);

  glActiveTexture(GL_TEXTURE5);
  LoadTGATextureSimple(imagePath, &texture);
  glUniform1i(glGetUniformLocation(*shader, "tex"), 5); // Texture unit 5

  model = LoadModelPlus(modelPath);

  maxDistance = 20.0;
  minDistance = 5.0;

  //maxSpeed = vec3(1,1,1);
  maxSpeed = Norm(vec3(2,2,2));

  cohesionWeight = 0.002; //0.01
  avoidanceWeight = 0.04; //0.02, 0.2
  alignmentWeight = 0.001; //0.0001;
  followWeight = 0.002; //0.004;

  // Bounding the positions inside this cube. Should maybe center around camera position instead.
  xMin = 0.0;
  xMax = 512.0; //512.0;
  yMin = 20.0;
  yMax = 300.0;
  zMin = 0.0;
  zMax = 512.0;
  
  makeFlockOf(numOfBoids, pos);
  leader.position = pos + vec3(50,0,50); //Makes the leader start ahead of the flock (for following and rotation).
  //leader.speed = vec3(1,0,1);
}

/*Evader::~Evader()
{

}*/

void Evader::draw(mat4 cameraMatrix)
{
  int numberOfEvaders = evaderVector.size();
  for(int i = 0; i < numberOfEvaders; i++)
    {
      evaderVector.at(i).draw(cameraMatrix, shader, model, &texture);
    }

  leader.draw(cameraMatrix, shader, model, &texture);
}

// void Evader::animate(GLfloat time)
// {
//   float xSpeed = cos(time);
//   float ySpeed = 2*sin(time);
//   float zSpeed = sin(time);

//   //speed = vec3(xSpeed,ySpeed,zSpeed);

//   //Create bone class to perform skinning. Insert bone to model here and animate.

//   position += speed;
// }

void Evader::updateLeader()
{
  float lowInterval = -1.0;
  float highInterval = 1.0;

  std::random_device rd;
  std::mt19937 generator(rd());
  //std::uniform_real_distribution<> dis(-1, 1);
  //std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(lowInterval, highInterval); //(-1.0, 1.0)

  float xOffset = distribution(generator);
  float yOffset = distribution(generator);
  float zOffset = distribution(generator);

  // cout << "xOffset = " << xOffset << " zOffset = " << zOffset << endl;
  // cout << "lowInterval = " << lowInterval << endl;
  // cout << "highInterval = " << highInterval << endl;

  vec3 offsetVec = vec3(xOffset/2.0, yOffset/4.0, zOffset/2.0); // for testing: vec3(0,0,0);
  leader.speed += offsetVec;

  if(Norm(leader.speed) > maxSpeed)
    leader.speed /= 2.0;

  leader.position += leader.speed;

  boundPositionBoid(&leader);
}

void Evader::checkMaxSpeed(Boid *boid)
{
  vec3 mSpeed = vec3(1,1,1);
if(Norm(boid->speed) > Norm(mSpeed))
    {
      if(boid->speed.x < 0)
	boid->speed.x = -1*mSpeed.x;
      if(boid->speed.x > 0)
	boid->speed.x = mSpeed.x;

      if(boid->speed.y < 0)
	boid->speed.y = -1*mSpeed.y;
      if(boid->speed.y > 0)
	boid->speed.y = mSpeed.y;

      if(boid->speed.z < 0)
	boid->speed.z = -1*mSpeed.z;
      if(boid->speed.z > 0)
	boid->speed.z = mSpeed.z;	  
    }
}

void Evader::update(GLfloat time)
{
  updateLeader();
  cout << "Position for leader: (" << leader.position.x << "," << leader.position.y << "," << leader.position.z << ")" << endl;
  flocking();
}

void Evader::boundPositionBoid(Boid *boid)
{
  if(boid->position.x < xMin)
      boid->speed.x += 0.2;
  else if(boid->position.x > xMax)
      boid->speed.x -= 0.2;

  else if(boid->position.y < yMin)
      boid->speed.y += 0.2;
  else if(boid->position.y > yMax)
      boid->speed.y -= 0.2;

  else if(boid->position.z < zMin)
      boid->speed.z += 0.2;
  else if(boid->position.z > zMax)
      boid->speed.z -= 0.2;
}

void Evader::flocking()
{
  int N = evaderVector.size();

  for(int i = 0; i < N; i++)
    {
      //evaderVector.at(i).speed = vec3(0,0,0);
      cohesion(&evaderVector.at(i), i);
      avoidance(&evaderVector.at(i), i);
      alignment(&evaderVector.at(i), i);
      followLeader(&evaderVector.at(i));

      evaderVector.at(i).speed += evaderVector.at(i).cohesionVector*cohesionWeight +
	evaderVector.at(i).avoidanceVector*avoidanceWeight +
      	evaderVector.at(i).alignmentVector*alignmentWeight +
	follow*followWeight;

      if(Norm(evaderVector.at(i).speed) > maxSpeed)
	evaderVector.at(i).speed /= 2.0;

      evaderVector.at(i).setRotation();

      evaderVector.at(i).position += evaderVector.at(i).speed;
      boundPositionBoid(&evaderVector.at(i));
   
      //cout << "Position for boid i: (" << evaderVector.at(i).position.x << "," << evaderVector.at(i).position.y << "," << evaderVector.at(i).position.z << ")" << endl;
    }
  
}

// Should move the for-loop to flocking() and have boidJ, cohesionCount as arguments and as well the if(i != j) outside.
void Evader::cohesion(Boid *boidI, int index)
{
  boidI->cohesionVector = vec3(0,0,0);
  boidI->averagePosition = vec3(0,0,0);
  int N = evaderVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid boidJ = evaderVector.at(j);
      if(index != j)
	{
	  float distance = Norm(boidJ.position - boidI->position);
	  if(distance < maxDistance)
	    {
	      boidI->averagePosition += boidJ.position;
	      count++;
	    }
	}
    }
  if(count > 0)
    boidI->averagePosition /= (float)count;

  float dist = Norm(boidI->averagePosition - boidI->position);
  boidI->cohesionVector = Normalize(boidI->averagePosition - boidI->position)*dist;
}

void Evader::avoidance(Boid* boidI, int index)
{
  boidI->avoidanceVector = vec3(0,0,0);
  int N = evaderVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid boidJ = evaderVector.at(j);
      if(index != j)
	{
	  vec3 distanceVector = boidJ.position - boidI->position;
	  float distance = Norm(distanceVector);
	  if(distance < minDistance)
	    {
	      //float fx = exp(1.0/distance) - 1.0;
	      boidI->avoidanceVector -= distanceVector/distance;
	      count++;
	    }
	}
    }
  if(count > 0)
    {
      boidI->avoidanceVector /= (float)count;
      //boidI->avoidanceVector = Normalize(boidI->avoidanceVector);
    }
}

void Evader::alignment(Boid* boidI, int index)
{
  boidI->alignmentVector = vec3(0,0,0);
  boidI->speedDifference = vec3(0,0,0);
  int N = evaderVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid boidJ = evaderVector.at(j);
      if(index != j)
	{
	  float distance = Norm(boidJ.position - boidI->position);
	  if(distance < maxDistance)
	    {
	      boidI->speedDifference += (boidJ.speed - boidI->speed);
	      count++;
	    }
	}
    }
  if(count > 0)
    {
      boidI->alignmentVector = boidI->speedDifference/(float)count;
      //boidI->alignmentVector = Normalize(boidI->alignmentVector);
    }
}

void Evader::followLeader(Boid* boidI)
{
  
  //cout << "lookAtPoint: (" << lookAtPoint.x << "," << lookAtPoint.y << "," << lookAtPoint.z << ")" << endl;
  boidI->direction = leader.position - boidI->position; //Normalize(lookAtPoint - boidI->position);

  if(Norm(boidI->averagePosition - boidI->position) < maxDistance)
    follow = boidI->direction;
  else
    follow = vec3(0,0,0);
}

void Evader::makeFlockOf(int inhabitants, vec3 position)
{
  for(int i = 0; i < inhabitants; i++)
    {
      vec3 pos = position;
      pos.x += (float)(rand() % 100)/5.0;
      pos.y += (float)(rand() % 50)/5.0;
      pos.z += (float)(rand() % 100)/5.0;
      //Boid *bird = new Boid(pos);
      evaderVector.push_back(Boid(pos));
    }
}
