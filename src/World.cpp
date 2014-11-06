#include "World.h"

World::World(){
  init();
}

void World::init(){

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  phongObjShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  t = 0;
  // Init objects
  patchGenerator = new MockupPatchGenerator("../textures/fft-terrain.tga");
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  evaders = new Evader(&phongObjShader, "../objects/crowMedium.obj", "../textures/crow.tga");

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);
  
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);

  // Boid
  int s = (int)(sizeof(evaders));
  cout << "Size of class evader: " << s << endl;

  evaders->makeFlockOf(100);

  s = (int)(sizeof(Evader));
  cout << "Size of class evader: " << s << endl;

  glUseProgram(phongObjShader);
  
  glUniform3fv(glGetUniformLocation(phongObjShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongObjShader, "specularExponent"), 1, &specularExponent);
  
  glUniformMatrix4fv(glGetUniformLocation(phongObjShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
  for(int y = 0; y < 3; y++){
    for(int x = 0; x < 3; x++){
      printf("Adding patch @ %i, %i\n", x, y);
      clock_t time;
      time = clock();
      generatePatch(x, y, 255);
      time = clock() - time;
      printf ("Time to generate terrain: %f seconds)\n",((float)time)/CLOCKS_PER_SEC);
    }
  }
}


void World::generatePatch(int patchX, int patchY, float patchSize){

  TextureData heightMap = patchGenerator->generatePatch(patchSize);

  TerrainPatch* terrainPatch = new TerrainPatch(&heightMap, patchX*patchSize , patchY*patchSize, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch

  terrainVector.push_back(terrainPatch);
}


void World::drawTerrainVector(TerrainPatch* t){
  t->draw(camera->cameraMatrix);
}

void World::draw(){
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 3000;
  camera->update();
  evaders->update(t);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);
  
  //for_each(terrainVector.begin(), terrainVector.end(), drawTerrainVector);
  //printf("size of vector = %i\n", terrainVector.size());
  for(int i = 0; i < terrainVector.size(); i++){
    terrainVector.at(i)->draw(camera->cameraMatrix);
  }

  //bird->animate(t);
  evaders->draw(camera->cameraMatrix);
}


World::~World(){
  delete camera;
  delete skybox;
  delete evaders;
  terrainVector.clear();
  delete patchGenerator;
}
