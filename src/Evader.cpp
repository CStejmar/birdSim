
#include "Evader.h"

Evader::Evader(GLuint *phongShader, char * mdlPath, char *imgPath)
{
  shader = phongShader;
  glUseProgram(*shader);

  modelPath = mdlPath;
  imagePath = imgPath;

  position = vec3(50,50,50); // Should be a parameter in constructor instead

  //texture = (GLuint*)malloc(sizeof(GLuint)); // if texture is a pointer we need this!
  //memset(texture, 0, sizeof(GLuint));

  glActiveTexture(GL_TEXTURE5);
  LoadTGATextureSimple(imagePath, &texture);
  glUniform1i(glGetUniformLocation(*shader, "tex"), 5); // Texture unit 5

  model = LoadModelPlus(modelPath);

  maxDistance = 100.0;
  minDistance = 15.0;

  maxSpeed = Norm(vec3(2,2,2));

  cohesionWeight = 0.002; //0.01
  avoidanceWeight = 0.02; //0.2
  alignmentWeight = 0.001; //0.0001;

  lookAtPoint = position + vec3(100,0,100);
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

void Evader::update(GLfloat time)
{
  flocking2();
}

void Evader::boundPosition(Boid *b)
{
  float xMin = 0.0;
  float xMax = 512.0;
  float yMin = 10.0;
  float yMax = 250.0;
  float zMin = 0.0;
  float zMax = 512.0;

  if(b->position.x < xMin)
    b->speed.x += 0.3;
  else if(b->position.x > xMax)
    b->speed.x -= 0.2;

  else if(b->position.y < yMin)
    b->speed.y += 0.2;
  else if(b->position.y > yMax)
    b->speed.y -= 0.2;

  else if(b->position.z < zMin)
    b->speed.z += 0.2;
  else if(b->position.z > zMax)
    b->speed.z -= 0.2;
}

void Evader::flocking2()
{
  int N = evaderVector.size();

  for(int i = 0; i < N; i++)
    {
      //evaderVector.at(i).speed = vec3(0,0,0);
      cohesion(&evaderVector.at(i), i);
      avoidance(&evaderVector.at(i), i);
      alignment(&evaderVector.at(i), i);

      evaderVector.at(i).speed += evaderVector.at(i).cohesionVector*cohesionWeight +
	evaderVector.at(i).avoidanceVector*avoidanceWeight +
	evaderVector.at(i).alignmentVector*alignmentWeight;

      if(Norm(evaderVector.at(i).speed) > maxSpeed)
	evaderVector.at(i).speed /= 2.0;

      evaderVector.at(i).position += evaderVector.at(i).speed;
      boundPosition(&evaderVector.at(i));

      cout << "Position for boid i: (" << evaderVector.at(i).position.x << "," << evaderVector.at(i).position.y << "," << evaderVector.at(i).position.z << ")" << endl;
    }
}

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
    boidI->avoidanceVector /= (float)count;
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
    boidI->alignmentVector = boidI->speedDifference/(float)count;
}


void Evader::flocking()
{
  int numberOfEvaders = evaderVector.size();

  // Double loop for accumulating contributions from other boids
  for(int i = 0; i < numberOfEvaders; i++)
    {
      int count = 0;
      evaderVector.at(i).speedDifference = vec3(0.0, 0.0, 0.0);
      evaderVector.at(i).averagePosition = vec3(0.0, 0.0, 0.0);
      evaderVector.at(i).avoidanceVector = vec3(0.0, 0.0, 0.0);
      
      for(int j = 0; j < numberOfEvaders; j++)
	{
	  if(i != j)
	    {
	      if(Norm(evaderVector.at(j).position - evaderVector.at(i).position) < maxDistance)
		{
		  // Alignment:
		  evaderVector.at(i).speedDifference += (evaderVector.at(j).speed - evaderVector.at(i).speed);
		  // Cohesion:
		  evaderVector.at(i).averagePosition += evaderVector.at(j).position;
		  // Avoidance:
		  evaderVector.at(i).avoidanceVector += calculateAvoidance(evaderVector.at(j).position, evaderVector.at(i).position);
		  count++;	  
		}
	    }
	}

      if(count > 0)
	{
	  // Divisions
	  evaderVector.at(i).speedDifference /= (float)count;
	  evaderVector.at(i).averagePosition /= (float)count;
	  evaderVector.at(i).avoidanceVector /= (float)count;
	}
    }

  // Loop for adding the resulting contributions
  for(int i = 0; i < numberOfEvaders; i++)
    {
      vec3 averageDifference = evaderVector.at(i).averagePosition - evaderVector.at(i).position;

      averageDifference = Normalize(averageDifference);
      //evaderVector.at(i).speedDifference = Normalize(evaderVector.at(i).speedDifference);
      //evaderVector.at(i).avoidanceVector = Normalize(evaderVector.at(i).avoidanceVector);
      
      evaderVector.at(i).speed +=
	evaderVector.at(i).speedDifference*alignmentWeight 
	+ averageDifference*cohesionWeight 
	+ evaderVector.at(i).avoidanceVector*avoidanceWeight;
    }

  // bool attacked = false;
  // int max;
  // if(attacked)
  //   max = evaderVector.size()-1;
  // else
  //   max = evaderVector.size()-floor((float)evaderVector.size()/2.0);
  // int min = 0;
  // int randNum = rand()%(max-min + 1) + min;

  int max = evaderVector.size()-1;
  int min = 0;

  //int randNum = rand()%(max-min + 1) + min;

  // Adding noise to one of the boids
  float divide = 100;
  float xOffset = (float)((random() % 2000) - 1000);
  float yOffset = (float)((random() % 2000) - 1000);
  float zOffset = (float)((random() % 2000) - 1000);

  //cout << "xOffset = " << xOffset << endl;

  vec3 offsetVec = vec3(xOffset, yOffset, zOffset);
  int randNum = 0;
  evaderVector.at(randNum).speed += offsetVec/divide; // Leader

  //lookAtPoint += offsetVec/divide;

  //position += offsetVec/divide;
  
}

void Evader::makeFlockOf(int inhabitants)
{
  for(int i = 0; i < inhabitants; i++)
    {
      vec3 pos = vec3(25, 25, 25);
      pos.x += (float)(rand() % 100)/5.0;
      pos.y += (float)(rand() % 50)/5.0;
      pos.z += (float)(rand() % 100)/5.0;
      //Boid *bird = new Boid(pos);
      evaderVector.push_back(Boid(pos));
    }
}

vec3 Evader::calculateAvoidance(vec3 posJ, vec3 posI)
{
  vec3 avoidVec = vec3(0.0, 0.0, 0.0);
  vec3 differenceVec = posI - posJ; // posJ - posI ?
  float distance = Norm(differenceVec);
  
  if(distance < minDistance) // > ?
    {
      float constant = 1.0;
      float fx = exp(constant/distance) - 1.0;
      avoidVec = (fx*differenceVec)/distance;
    }

  return avoidVec;
}
