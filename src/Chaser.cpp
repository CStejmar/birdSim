
#include "Chaser.h"

Chaser::Chaser(GLuint *phongShader, char * mdlPath, char *imgPath, vec3 pos, int numOfBoids)
{
  shader = phongShader;

  modelPath = mdlPath;
  imagePath = imgPath;

  glUseProgram(*shader);

  glActiveTexture(GL_TEXTURE5);
  LoadTGATextureSimple(imagePath, &texture);
  glUniform1i(glGetUniformLocation(*shader, "tex"), 6); // Texture unit 6

  model = LoadModelPlus(modelPath);

  awarenessRadius = 80.0;
  minDistance = 40.0;

  maxSpeed = Norm(vec3(1.7,1.7,1.7));

  avoidanceWeight = 0.02;//0.04; //0.02, 0.2
  attackWeight = 0.02; //0.004;

  float lowInterval = 0.0;
  float highInterval = 0.5;

  // Bounding the positions inside this cube. Should maybe center around camera position instead.
  xMin = 0.0;
  xMax = 512.0;
  yMin = 20.0;
  yMax = 300.0;
  zMin = 0.0;
  zMax = 512.0;
  
  makeIndividuals(numOfBoids, pos);
}

/*Chaser::~Chaser()
{

}*/

void Chaser::draw(mat4 cameraMatrix)
{
  int numberOfChasers = chaserVector.size();
  for(int i = 0; i < numberOfChasers; i++)
    {
      chaserVector.at(i).draw(cameraMatrix, shader, model, &texture);
    }
}

// void Chaser::animate(GLfloat time)
// {
//   float xSpeed = cos(time);
//   float ySpeed = 2*sin(time);
//   float zSpeed = sin(time);

//   //speed = vec3(xSpeed,ySpeed,zSpeed);

//   //Create bone class to perform skinning. Insert bone to model here and animate.

//   position += speed;
// }

void Chaser::setRandomSpeed(Boid *boidI)
{
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

  vec3 offsetVec = vec3(xOffset, yOffset/2.0, zOffset); // for testing: vec3(0,0,0);
  boidI->speed += offsetVec/2.0;

  //if(Norm(boidI->speed) > maxSpeed)
  //boidI->speed /= 2.0;

  //boidI->position += boidI->speed;
}

void Chaser::checkMaxSpeed(Boid *boid)
{
  vec3 mSpeed = vec3(1,1,1);
if(Norm(boid->speed) > Norm(mSpeed))
    {
      boid->speed = (boid->speed/Norm(boid->speed))*Norm(mSpeed);

      // if(boid->speed.x < 0)
      // 	boid->speed.x = -1*mSpeed.x;
      // if(boid->speed.x > 0)
      // 	boid->speed.x = mSpeed.x;

      // if(boid->speed.y < 0)
      // 	boid->speed.y = -1*mSpeed.y;
      // if(boid->speed.y > 0)
      // 	boid->speed.y = mSpeed.y;

      // if(boid->speed.z < 0)
      // 	boid->speed.z = -1*mSpeed.z;
      // if(boid->speed.z > 0)
      // 	boid->speed.z = mSpeed.z;
      
    }
}

void Chaser::update(GLfloat time, vector<Boid> evaderVector)
{
  /*cout << "Position for chaser: ("
       << chaserVector.at(0).position.x << ","
       << chaserVector.at(0).position.y << ","
       << chaserVector.at(0).position.z << ")" << endl;*/

  searchPrey(evaderVector);
}

void Chaser::boundPositionBoid(Boid *boid)
{
  if(boid->position.x < xMin)
    {
      boid->speed.x += 0.2;
      lowInterval = 0.0;
      highInterval = 0.5;
    }
  else if(boid->position.x > xMax)
    {
      boid->speed.x -= 0.2;
      lowInterval = -0.5;
      highInterval = 0.0;
    }

  else if(boid->position.y < yMin)
      boid->speed.y += 0.2;
  else if(boid->position.y > yMax)
      boid->speed.y -= 0.2;

  else if(boid->position.z < zMin)
    {
      boid->speed.z += 0.2;
      lowInterval = 0.0;
      highInterval = 0.5;
    }
  else if(boid->position.z > zMax)
    {
      boid->speed.z -= 0.2;
      lowInterval = -0.5;
      highInterval = 0.0;
    }
}

void Chaser::searchPrey(vector<Boid> evaderVector)
{
  int N = chaserVector.size();

  for(int i = 0; i < N; i++)
    {
      if(N > 1)
	avoidance(&chaserVector.at(i), i);
      
      attack(&chaserVector.at(i),evaderVector);
      setRandomSpeed(&chaserVector.at(i));

      chaserVector.at(i).speed += chaserVector.at(i).avoidanceVector*avoidanceWeight +
	chaserVector.at(i).direction*attackWeight;
      
      chaserVector.at(i).direction = chaserVector.at(i).speed; // Normalize(

      if(Norm(chaserVector.at(i).speed) > maxSpeed)
	chaserVector.at(i).speed /= 2.0;

      chaserVector.at(i).setRotation();

      chaserVector.at(i).position += chaserVector.at(i).speed;
      boundPositionBoid(&chaserVector.at(i));
   
      //cout << "Position for boid i: (" << chaserVector.at(i).position.x << "," << chaserVector.at(i).position.y << "," << chaserVector.at(i).position.z << ")" << endl;
    }
  
}

// Should move the for-loop to flocking() and have boidJ, cohesionCount as arguments and as well the if(i != j) outside.
void Chaser::avoidance(Boid* boidI, int index)
{
  boidI->avoidanceVector = vec3(0,0,0);
  int N = chaserVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid boidJ = chaserVector.at(j);
      if(index != j)
	{
	  vec3 distanceVector = boidJ.position - boidI->position;
	  float distance = Norm(distanceVector);
	  if(distance < minDistance)
	    {
	      //float fx = exp(1.0/distance) - 1.0;
	      if(distance != 0.0)
		boidI->avoidanceVector -= CrossProduct(distanceVector/distance,boidI->up);
	      else
		boidI->avoidanceVector -= CrossProduct(distanceVector,boidI->up);
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

void Chaser::attack(Boid* boidI, vector<Boid> evaders)
{
  boidI->direction = vec3(0,0,0);
  int N = evaders.size();
  uint count = 0;

  for(int i = 0; i < N; i++)
    {
      if(Norm(evaders.at(i).position - boidI->position) < awarenessRadius)
	{
	  boidI->direction += evaders.at(i).position - boidI->position;
	  count++;
	}
    }
  if(count > 0)
    boidI->direction /= (float)count;
  //boidI->direction = Normalize(boidI->direction);
}

void Chaser::makeIndividuals(int inhabitants, vec3 position)
{
  for(int i = 0; i < inhabitants; i++)
    {
      vec3 pos = position;
      pos.x += (float)(rand() % 100)/5.0;
      pos.y += (float)(rand() % 50)/5.0;
      pos.z += (float)(rand() % 100)/5.0;
      //Boid *bird = new Boid(pos);
      chaserVector.push_back(Boid(pos));
    }
}
