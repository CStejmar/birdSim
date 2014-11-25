
#include "ManageChasersAndEvaders.h"

ManageChasersAndEvaders::ManageChasersAndEvaders(GLuint* shader)
{
  Evader* evaders = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", vec3(25,25,25), 150);
  flocks.push_back(evaders);
  chasers = new Chaser(shader, "../objects/eagle.obj", "../textures/eagleBrown.tga", vec3(300,100,300), 3);
}

/*ManageChasersAndEvaders::~ManageChasersAndEvaders()
{

}*/

void vectorAppend(vector<Boid> v1, vector<Boid> v2)
{ 
  v1.reserve(v1.size() + v2.size());
  std::move(v2.begin(), v2.end(), std::inserter(v1, v1.end()));
  v2.clear();
}

void ManageChasersAndEvaders::mergeFlocks()
{
  int numberOfFlocks = flocks.size();
  if(numberOfFlocks > 1)
    {
      for(int i = 0; i < numberOfFlocks; i++)
	{
	  for(int j = 0; j < numberOfFlocks; j++)
	    {
	      if(i != j && Norm(flocks.at(j)->leader.position - flocks.at(i)->leader.position) < 50)
		vectorAppend(flocks.at(i)->evaderVector,flocks.at(j)->evaderVector);
	    }
	}
    }
}

void ManageChasersAndEvaders::splitFlock()
{
  // int N = flocks.size();
  // for(int i = 0; i < N; i++)
  //   {
  //     flocks.at(i)->draw(cameraMatrix);
  //   }
  // chasers->draw(cameraMatrix);
}

void ManageChasersAndEvaders::update(GLfloat time)
{
  int N = flocks.size();
  for(int i = 0; i < N; i++)
    {
      flocks.at(i)->update(time,chasers->chaserVector);
      chasers->update(time,flocks.at(i)->evaderVector);
    }
  //mergeFlocks();
}

void ManageChasersAndEvaders::draw(mat4 cameraMatrix)
{
  int N = flocks.size();
  for(int i = 0; i < N; i++)
    {
      flocks.at(i)->draw(cameraMatrix);
    }
  chasers->draw(cameraMatrix);
}
