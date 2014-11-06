
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

  maxDistance = 100;
  minDistance = 10;

  cohesionWeight = 0.001;
  avoidanceWeight = 0.5;
  alignmentWeight = 0.0001;
  
  //makeFlockOf(20);
  //evaderVector.assign(10, Boid(position));
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
  flocking();
  int numberOfEvaders = evaderVector.size();

  for(int i = 0; i < numberOfEvaders; i++)
    {
      //if(Norm(evaderVector.at(i).position - position) < 400)
	evaderVector.at(i).position += evaderVector.at(i).speed;
	//else
	//evaderVector.at(i).position -= evaderVector.at(i).speed;
    }
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
	      if(Norm(evaderVector.at(j).position - evaderVector.at(i).position) < maxDistance) // - instead of VectorSub, overloaded operation in C++
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

      //evaderVector.at(i).speed = Normalize(evaderVector.at(i).speed);
      
      //evaderVector.at(i).position += evaderVector.at(i).speed; // Done in update!

      cout << "Speed for current boid: (" << evaderVector.at(i).speed.x << ","
	   << evaderVector.at(i).speed.y << "," << evaderVector.at(i).speed.z << ")" << endl;
    }

  // bool attacked = false;
  // int max;
  // if(attacked)
  //   max = evaderVector.size()-1;
  // else
  //   max = evaderVector.size()-floor((float)evaderVector.size()/2.0);
  // int min = 0;
  // int randNum = rand()%(max-min + 1) + min;

  // Adding noise to one of the boids
  float divide = 3000;
  float xOffset = (float)((random() % 2000) - 1000);
  float yOffset = (float)((random() % 2000) - 1000);
  float zOffset = (float)((random() % 2000) - 1000);

  vec3 offsetVec = vec3(xOffset, yOffset, zOffset);
  int randNum = 0;
  evaderVector.at(randNum).speed += offsetVec/divide;
  
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
  vec3 differenceVec = posJ - posI;
  float distance = Norm(differenceVec);
  
  if(distance > minDistance)
    {
      float constant = 1.0;
      float fx = exp(constant/distance) - 1.0;
      avoidVec = (fx*differenceVec)/distance;
    }

  return avoidVec;
}
