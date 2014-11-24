
#include "Evader.h"

Evader::Evader(GLuint *phongShader, char * mdlPath, char *imgPath, vec3 pos)
{
  shader = phongShader;
  glUseProgram(*shader);

  modelPath = mdlPath;
  imagePath = imgPath;

  position = pos;

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

  lookAtPoint = vec3(0,0,0); //position + vec3(100,0,100);

  xMin = 0.0;
  xMax = 512.0;
  yMin = 10.0;
  yMax = 250.0;
  zMin = 0.0;
  zMax = 512.0;

  iLow = 0.0;
  iHigh = 1.0;
  
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
  // Adding noise to imaginary leader position
  float xOffset;// = (float)((random() % 2000) - 1000);
  float yOffset;// = (float)((random() % 2000) - 1000);
  float zOffset;// = (float)((random() % 2000) - 1000);

  std::random_device rd;
  std::mt19937 generator(rd());
  //std::uniform_real_distribution<> dis(-1, 1);

  //std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(iLow, iHigh); //(-1.0, 1.0)
  xOffset = distribution(generator);
  yOffset = distribution(generator);
  zOffset = distribution(generator);

  cout << "xOffset = " << xOffset << " zOffset = " << zOffset << endl;
  cout << "iLow = " << iLow << endl;
  cout << "iHigh = " << iHigh << endl;

  vec3 offsetVec = vec3(xOffset*2, yOffset/2.0, zOffset*2);///divide;
  position += offsetVec; // Leader
  lookAtPoint = position + offsetVec*500; // + vec3(100,0,100); //Makes direction wrong

  flocking();
}

void Evader::boundPositionBoid(Boid *b)
{

  if(b->position.x < xMin)
    {
      b->speed.x += 0.2;
      iLow = 0.0;
      iHigh = 1.0;
    }
  else if(b->position.x > xMax)
    {
      b->speed.x -= 0.2;
      iLow = -1.0;
      iHigh = 0.0;
    }
  else if(b->position.y < yMin)
    {
      b->speed.y += 0.2;
    }
  else if(b->position.y > yMax)
    {
      b->speed.y -= 0.2;
    }
  else if(b->position.z < zMin)
    {
      b->speed.z += 0.2;
      iLow = 0.0;
      iHigh = 1.0;
    }
  else if(b->position.z > zMax)
    {
      b->speed.z -= 0.2;
      iLow = -1.0;
      iHigh = 0.0;
    }
}

void Evader::boundPositionFlock()
{

  if(position.x < xMin)
    {
      position.x = xMin;//+= 0.1;
      //iLow = 0.0;
      //iHigh = 1.0;
    }
  else if(position.x > xMax)
    {
      position.x = xMax;//-= 0.1;
      //iLow = -1.0;
      //iHigh = 0.0;
    }
  else if(position.y < yMin)
    {
      position.y = yMin;//+= 0.1;
    }
  else if(position.y > yMax)
    {
      position.y = yMax;//-= 0.1;
    }
  else if(position.z < zMin)
    {
      position.z = zMin;//+= 0.1;
      //iLow = 0.0;
      //iHigh = 1.0;
    }
  else if(position.z > zMax)
    {
      position.z = zMax;//-= 0.1;
      //iLow = -1.0;
      //iHigh = 0.0;
    }
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
	follow*0.0004; //0.004;

      if(Norm(evaderVector.at(i).speed) > maxSpeed)
	evaderVector.at(i).speed /= 2.0;

      //evaderVector.at(i).setDirection();
      evaderVector.at(i).setRotation(lookAtPoint);

      evaderVector.at(i).position += evaderVector.at(i).speed;
      boundPositionBoid(&evaderVector.at(i));
      boundPositionFlock();
      if(evaderVector.at(i).position.x < lookAtPoint.x && evaderVector.at(i).position.z < lookAtPoint.z)
	cout << "Look at point larger than boid position" << endl;
      else
	cout << "Look at point SMALLER than boid position" << endl;
      //cout << "Position for boid i: (" << evaderVector.at(i).position.x << "," << evaderVector.at(i).position.y << "," << evaderVector.at(i).position.z << ")" << endl;
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
  boidI->direction = lookAtPoint - boidI->position; //Normalize(lookAtPoint - boidI->position);

  if(Norm(boidI->averagePosition - boidI->position) < maxDistance)
    follow = boidI->direction;
  else
    follow = vec3(0,0,0);
}

// int max = evaderVector.size()-1;
// int min = 0;

// //int randNum = rand()%(max-min + 1) + min;

// // Adding noise to one of the boids
// float divide = 100;
// float xOffset = (float)((random() % 2000) - 1000);
// float yOffset = (float)((random() % 2000) - 1000);
// float zOffset = (float)((random() % 2000) - 1000);

// //cout << "xOffset = " << xOffset << endl;

// vec3 offsetVec = vec3(xOffset, yOffset, zOffset);
// int randNum = 0;
// evaderVector.at(randNum).speed += offsetVec/divide; // Leader

// //lookAtPoint += offsetVec/divide;
// //position += offsetVec/divide;

void Evader::makeFlockOf(int inhabitants)
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
