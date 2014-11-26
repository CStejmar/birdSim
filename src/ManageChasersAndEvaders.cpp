
#include "ManageChasersAndEvaders.h"

ManageChasersAndEvaders::ManageChasersAndEvaders(GLuint* phongShader, char * modelPathEvader, char *imagePathEvader, char * modelPathChaser, char *imagePathChaser)
{
  shader = phongShader;

  //modelPath = mdlPath;
  //imagePath = imgPath;

  glUseProgram(*shader);

  //________________Evader________________
  glActiveTexture(GL_TEXTURE5);
  LoadTGATextureSimple(imagePathEvader, &evaderTexture);
  glUniform1i(glGetUniformLocation(*shader, "tex"), 5); // Texture unit 5

  evaderModel = LoadModelPlus(modelPathEvader);

  //________________Chaser________________
  glActiveTexture(GL_TEXTURE6);
  LoadTGATextureSimple(imagePathChaser, &chaserTexture);
  glUniform1i(glGetUniformLocation(*shader, "tex"), 6); // Texture unit 5

  chaserModel = LoadModelPlus(modelPathChaser);

  Evader* evaders = new Evader(shader, evaderModel, evaderTexture, vec3(25,25,25), 150);
  Evader* evaders2 = new Evader(shader, evaderModel, evaderTexture, vec3(25,25,25), 20);
  //Evader* evaders = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", vec3(25,25,25), 150);
  //Evader* evaders2 = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", vec3(25,25,25), 20);
  flocks.push_back(evaders);
  flocks.push_back(evaders2);
  chasers = new Chaser(shader, "../objects/eagle.obj", "../textures/eagleBrown.tga", vec3(300,100,300), 3);
}

/*ManageChasersAndEvaders::~ManageChasersAndEvaders()
{

}*/

void vectorAppend(vector<Boid> *v1, vector<Boid> *v2)
{ 
  v1->reserve(v1->size() + v2->size());
  std::move(v2->begin(), v2->end(), std::inserter(*v1, v1->end()));
  v2->clear();
}

// TODO: Should merge the smaller vector into the larger.
bool ManageChasersAndEvaders::mergeFlocks()
{
  int numberOfFlocks = flocks.size();
  float mergeThreshold = 35.0;
  
  for(int i = 0; i < numberOfFlocks; i++)
    {
      for(int j = 0; j < numberOfFlocks; j++)
	{
	  if(i != j && Norm(flocks.at(j)->leader.position - flocks.at(i)->leader.position) < mergeThreshold)
	    {
	      vectorAppend(&flocks.at(i)->evaderVector,&flocks.at(j)->evaderVector);
	      cout << "Append!" << endl;
	      flocks.erase(flocks.begin()+j);
		//delete flocks.at(j);
	      cout << "FLOCKS = " << flocks.size() << endl;
	      return true;
	    }
	  
	}
    }
  return false;
}

void ManageChasersAndEvaders::splitFlock(Evader *flock)
{
  // int numberOfFlocks = flocks.size();
  
  // for(int i = 0; i < numberOfFlocks; i++)
  //   {
  
  int splitDistance = 100;
  int minD = 1000; // Initial high value
  int minIndex = -1;
  int maxD = 0;
  int maxIndex = -1;

  int numberOfEvaders = flock->evaderVector.size();
  for(int i = 0; i < numberOfEvaders; i++)
    {
      Boid boidI = flock->evaderVector.at(i);

      if(Norm(flock->leader.position - boidI.position) > maxD)
	{
	  maxD = Norm(flock->leader.position - boidI.position);
	  maxIndex = i;
	}

      if(Norm(flock->leader.position - boidI.position) < minD)
	{
	  minD = Norm(flock->leader.position - boidI.position);
	  minIndex = i;
	}

      //Change maxD to distance from current maxD to minD.
      maxD = maxD - minD;
    }

  if(maxD > splitDistance && maxIndex != -1 && minIndex != -1)
    {
      vec3 clusterPos1 = flock->evaderVector.at(minIndex).position;
      vec3 clusterPos2 = flock->evaderVector.at(maxIndex).position;
      Evader* evader = new Evader(shader, evaderModel, evaderTexture, clusterPos2, 0);
      //Evader *evader = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", clusterPos2, 0);
      
      for(int i = 0; i < flock->evaderVector.size(); i++)
	{
	  vec3 boidIPos = flock->evaderVector.at(i).position;

	  if(Norm(clusterPos2 - boidIPos) < Norm(clusterPos1 - boidIPos))
	    {
	      evader->evaderVector.reserve(evader->evaderVector.size() + 1);
	      std::move(flock->evaderVector.begin() + i,flock->evaderVector.begin() + (i + 1), std::inserter(evader->evaderVector, evader->evaderVector.end()));
	      flock->evaderVector.erase(flock->evaderVector.begin()+i);
	    }
	}

      flocks.push_back(evader);
    }
}

void ManageChasersAndEvaders::update(GLfloat time)
{
  int N = flocks.size();
  cout << "Number of flocks: " << N << endl;
  
  for(int i = 0; i < N; i++)
    {
      flocks.at(i)->update(time,chasers->chaserVector);
      chasers->update(time,flocks.at(i)->evaderVector);
      splitFlock(flocks.at(i));
      cout << "Size of flock i: " << flocks.at(i)->evaderVector.size() << endl;
    }
  
  if(N > 1)
    {
      mergeFlocks();
    }
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
