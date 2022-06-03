//FINAL PROJECT - TOMAS VERA

//Imports
#include <iostream>
#include <glad/glad.h>
#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include "particleSys.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong has diffuse
	std::shared_ptr<Program> prog;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	std::shared_ptr<Program> cubeProg;

	// Our shader program for particles
	std::shared_ptr<Program> partProg;

	//the partricle system setup
	particleSys *thePartSystem;
	particleSys *thePartSystem2;
	particleSys *thePartSystem3;
	particleSys *thePartSystem4;
	particleSys *thePartSystem5;
	particleSys *thePartSystem6;
	particleSys *thePartSystem7;
	particleSys *WinPart1;
	particleSys *WinPart2;
	particleSys *WinPart3;
	particleSys *WinPart4;
	particleSys *WinPart5;

	//Mesh Setup
	shared_ptr<Shape> sphere;
	shared_ptr<Shape> theDog;
	shared_ptr<Shape> cube;
	shared_ptr<Shape> moai;
	shared_ptr<Shape> frog;
	shared_ptr<Shape> palm;
	shared_ptr<Shape> palm2;
	shared_ptr<Shape> alpine;
	shared_ptr<Shape> item;
	shared_ptr<Shape> house;
	shared_ptr<Shape> campfire;
	shared_ptr<Shape> dino_head;
	shared_ptr<Shape> dino_left_arm;
	shared_ptr<Shape> dino_left_low_leg;
	shared_ptr<Shape> dino_left_up_leg;
	shared_ptr<Shape> dino_low_torso;
	shared_ptr<Shape> dino_right_arm;
	shared_ptr<Shape> dino_right_low_leg;
	shared_ptr<Shape> dino_right_up_leg;
	shared_ptr<Shape> dino_tail;
	shared_ptr<Shape> dino_up_torso;
	vector<shared_ptr<Shape>> tent;


	//Collision detection array
	int collision[100][100];

	//Skybox setup
	vector<std::string> faces {
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	}; 

	unsigned int cubeMapTexture;

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;

	//Texture setup
	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;	
	shared_ptr<Texture> texture2;
	shared_ptr<Texture> texture3;
	shared_ptr<Texture> texture4;
	shared_ptr<Texture> texture5;

	// OpenGL handle to texture data used in particle
	shared_ptr<Texture> texture;

	//animation data
	float lightTrans = 0;
	int colorMode = 0;
	bool keyToggles[256] = { false };


	//some particle variables
	float t = 0.0f; //reset in init
	float h = 0.01f;

	glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);

	//camera
	double g_phi = 0;
	double g_theta = 0;

	//animation data
	float itemTheta = 0;
	float dinoTheta = 0;
	float dinoMoveTheta = 0;
	float dinoMoveCheck = 0;
	
	float jump_time = 0;

	//camera setup
	vec3 view = vec3(0, 0, 1);
	vec3 strafe = vec3(1, 0, 0);

	vec3 eye;
	vec3 lookat;
	vec3 up;

	vec3 gaze;
	vec3 w;
	vec3 u;

	bool goCamera = false;
	bool moveLeft = false;
	bool moveRight = false;
	bool moveForward = false;
	bool moveBackward = false;
	bool jump_up = false;
	bool jump_down = false;


	//Collection setup
	bool collect_item_1 = false;
	bool collect_item_2 = false;
	bool collect_item_3 = false;
	bool collect_item_4 = false;
	bool collect_item_5 = false;



	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS){
			lightTrans += 0.5;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			lightTrans -= 0.5;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
			goCamera = !goCamera;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS){
			moveLeft = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS){
			moveRight = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS){
			moveForward = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS){
			moveBackward = true;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
			
			jump_up = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE){
			moveLeft = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE){
			moveRight = false;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE){
			moveForward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE){
			moveBackward = false;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}


	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {
   		
   		//fill in for game camera
		g_theta += deltaX*.1;
		if (g_phi < 1.2) {
			g_phi += -deltaY*.05;
		}
		else {
			g_phi = 1.19555;
		}
		if (g_phi > -1.2) {
			g_phi += -deltaY*.05;
		}
		else {
			g_phi = -1.1555;
		}
		
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPointSize(24.0f);

		g_theta = -PI/2.0;

		// Initialize the GLSL program that we will use for local shading
		prog = make_shared<Program>();
		prog->setVerbose(false);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");


		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(false);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("intensity");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatShine");
		texProg->addUniform("lightPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		// Initialize the GLSL program.
		partProg = make_shared<Program>();
		partProg->setVerbose(false);
		partProg->setShaderNames(
			resourceDirectory + "/part_vert.glsl",
			resourceDirectory + "/part_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("V");
		partProg->addUniform("M");
		partProg->addUniform("pColor");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");

		

		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(false);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addUniform("skybox");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");

		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/bottom.jpg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_REPEAT, GL_REPEAT);

  		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/skyBox/back.jpg");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/rock.jpeg");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture3 = make_shared<Texture>();
  		texture3->setFilename(resourceDirectory + "/alpine-rock.jpg");
  		texture3->init();
  		texture3->setUnit(3);
  		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture4 = make_shared<Texture>();
  		texture4->setFilename(resourceDirectory + "/totem.png");
  		texture4->init();
  		texture4->setUnit(4);
  		texture4->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture5 = make_shared<Texture>();
  		texture5->setFilename(resourceDirectory + "/boards.png");
  		texture5->init();
  		texture5->setUnit(5);
  		texture5->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		  

		cubeMapTexture = createSky(resourceDirectory + "/beach/", faces);


		//Create particle systems
		thePartSystem = new particleSys(vec3(-27,0,18));
		thePartSystem->gpuSetup();

		thePartSystem2 = new particleSys(vec3(-27,0,6));
		thePartSystem2->gpuSetup();

		thePartSystem3 = new particleSys(vec3(45,0.3,34));
		thePartSystem3->gpuSetup();

		thePartSystem4 = new particleSys(vec3(-38,0.3,8));
		thePartSystem4->gpuSetup();

		thePartSystem5 = new particleSys(vec3(0,0.3,-4));
		thePartSystem5->gpuSetup();

		thePartSystem6 = new particleSys(vec3(-40,0.3,-20));
		thePartSystem6->gpuSetup();

		thePartSystem7 = new particleSys(vec3(35,0.3,-35));
		thePartSystem7->gpuSetup();

		WinPart1 = new particleSys(vec3(0,0,0));
		WinPart1->gpuSetup();
		WinPart2 = new particleSys(vec3(0,1.5,0));
		WinPart2->gpuSetup();
		WinPart3 = new particleSys(vec3(0,3,0));
		WinPart3->gpuSetup();
		WinPart4 = new particleSys(vec3(0,4.5,0));
		WinPart4->gpuSetup();
		WinPart5 = new particleSys(vec3(0,6,0));
		WinPart5->gpuSetup();

    
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphereWTex.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			sphere = make_shared<Shape>();
			sphere->createShape(TOshapes[0]);
			sphere->measure();
			sphere->init();
		}

		// Initialize bunny mesh.
		vector<tinyobj::shape_t> TOshapesB;
 		vector<tinyobj::material_t> objMaterialsB;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/dog.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
			theDog = make_shared<Shape>();
			theDog->createShape(TOshapesB[0]);
			theDog->measure();
			theDog->init();
		}
		vector<tinyobj::shape_t> TOshapesC;
 		vector<tinyobj::material_t> objMaterialsC;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesC, objMaterialsC, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
			cube = make_shared<Shape>();
			cube->createShape(TOshapesC[0]);
			cube->measure();
			cube->init();
		}

		vector<tinyobj::shape_t> TOshapesD;
 		vector<tinyobj::material_t> objMaterialsD;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesD, objMaterialsD, errStr, (resourceDirectory + "/Moai_statue.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			moai = make_shared<Shape>();
			moai->createShape(TOshapesD[0]);
			moai->measure();
			moai->init();
		}

		vector<tinyobj::shape_t> TOshapesE;
 		vector<tinyobj::material_t> objMaterialsE;

		rc = tinyobj::LoadObj(TOshapesE, objMaterialsE, errStr, (resourceDirectory + "/palm2.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			palm = make_shared<Shape>();
			palm->createShape(TOshapesE[4]);
			palm->measure();
			palm->init();
			palm2 = make_shared<Shape>();
			palm2->createShape(TOshapesE[2]);
			palm2->measure();
			palm2->init();
		}

		vector<tinyobj::shape_t> TOshapesF;
 		vector<tinyobj::material_t> objMaterialsF;

		rc = tinyobj::LoadObj(TOshapesF, objMaterialsF, errStr, (resourceDirectory + "/frog.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			frog = make_shared<Shape>();
			frog->createShape(TOshapesF[6]);
			frog->measure();
			frog->init();
		}

		vector<tinyobj::shape_t> TOshapesG;
 		vector<tinyobj::material_t> objMaterialsG;

		rc = tinyobj::LoadObj(TOshapesG, objMaterialsG, errStr, (resourceDirectory + "/alpine.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			alpine = make_shared<Shape>();
			alpine->createShape(TOshapesG[0]);
			alpine->measure();
			alpine->init();
		}

		vector<tinyobj::shape_t> TOshapesH;
 		vector<tinyobj::material_t> objMaterialsH;

		rc = tinyobj::LoadObj(TOshapesH, objMaterialsH, errStr, (resourceDirectory + "/item.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			item = make_shared<Shape>();
			item->createShape(TOshapesH[0]);
			item->measure();
			item->init();
		}

		vector<tinyobj::shape_t> TOshapesI;
 		vector<tinyobj::material_t> objMaterialsI;

		rc = tinyobj::LoadObj(TOshapesI, objMaterialsI, errStr, (resourceDirectory + "/house.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			house = make_shared<Shape>();
			house->createShape(TOshapesI[0]);
			house->measure();
			house->init();
		}

		vector<tinyobj::shape_t> TOshapesJ;
 		vector<tinyobj::material_t> objMaterialsJ;

		rc = tinyobj::LoadObj(TOshapesJ, objMaterialsJ, errStr, (resourceDirectory + "/campfire.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				campfire = make_shared<Shape>();
				campfire->createShape(TOshapesJ[0]);
				campfire->measure();
				campfire->init();
		}

		vector<tinyobj::shape_t> TOshapesK;
 		vector<tinyobj::material_t> objMaterialsK;

		rc = tinyobj::LoadObj(TOshapesK, objMaterialsK, errStr, (resourceDirectory + "/tent.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			for (int i = 0; i < TOshapesK.size(); i++) {
				tent.push_back(make_shared<Shape>());
				tent[i]->createShape(TOshapesK[i]);
				tent[i]->measure();
				tent[i]->init();
			}
		}

		vector<tinyobj::shape_t> TOshapesL_1;
 		vector<tinyobj::material_t> objMaterialsL_1;

		rc = tinyobj::LoadObj(TOshapesL_1, objMaterialsL_1, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaHead.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_head = make_shared<Shape>();
				dino_head->createShape(TOshapesL_1[0]);
				dino_head->measure();
				dino_head->init();
		}

		vector<tinyobj::shape_t> TOshapesL_2;
 		vector<tinyobj::material_t> objMaterialsL_2;

		rc = tinyobj::LoadObj(TOshapesL_2, objMaterialsL_2, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaLeftArm.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_left_arm = make_shared<Shape>();
				dino_left_arm->createShape(TOshapesL_2[0]);
				dino_left_arm->measure();
				dino_left_arm->init();
		}

		vector<tinyobj::shape_t> TOshapesL_3;
 		vector<tinyobj::material_t> objMaterialsL_3;

		rc = tinyobj::LoadObj(TOshapesL_3, objMaterialsL_3, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaLeftLowerLeg.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_left_low_leg = make_shared<Shape>();
				dino_left_low_leg->createShape(TOshapesL_3[0]);
				dino_left_low_leg->measure();
				dino_left_low_leg->init();
		}

		vector<tinyobj::shape_t> TOshapesL_4;
 		vector<tinyobj::material_t> objMaterialsL_4;

		rc = tinyobj::LoadObj(TOshapesL_4, objMaterialsL_4, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaLeftUpperLeg.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_left_up_leg = make_shared<Shape>();
				dino_left_up_leg->createShape(TOshapesL_4[0]);
				dino_left_up_leg->measure();
				dino_left_up_leg->init();
		}

		vector<tinyobj::shape_t> TOshapesL_5;
 		vector<tinyobj::material_t> objMaterialsL_5;

		rc = tinyobj::LoadObj(TOshapesL_5, objMaterialsL_5, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaLowerTorso.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_low_torso = make_shared<Shape>();
				dino_low_torso->createShape(TOshapesL_5[0]);
				dino_low_torso->measure();
				dino_low_torso->init();
		}

		vector<tinyobj::shape_t> TOshapesL_6;
 		vector<tinyobj::material_t> objMaterialsL_6;

		rc = tinyobj::LoadObj(TOshapesL_6, objMaterialsL_6, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaRightArm.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_right_arm = make_shared<Shape>();
				dino_right_arm->createShape(TOshapesL_6[0]);
				dino_right_arm->measure();
				dino_right_arm->init();
		}

		vector<tinyobj::shape_t> TOshapesL_7;
 		vector<tinyobj::material_t> objMaterialsL_7;

		rc = tinyobj::LoadObj(TOshapesL_7, objMaterialsL_7, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaRightLowerLeg.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_right_low_leg = make_shared<Shape>();
				dino_right_low_leg->createShape(TOshapesL_7[0]);
				dino_right_low_leg->measure();
				dino_right_low_leg->init();
		}

		vector<tinyobj::shape_t> TOshapesL_8;
 		vector<tinyobj::material_t> objMaterialsL_8;

		rc = tinyobj::LoadObj(TOshapesL_8, objMaterialsL_8, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaRightUpperLeg.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_right_up_leg = make_shared<Shape>();
				dino_right_up_leg->createShape(TOshapesL_8[0]);
				dino_right_up_leg->measure();
				dino_right_up_leg->init();
		}

		vector<tinyobj::shape_t> TOshapesL_9;
 		vector<tinyobj::material_t> objMaterialsL_9;

		rc = tinyobj::LoadObj(TOshapesL_9, objMaterialsL_9, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaTail.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_tail = make_shared<Shape>();
				dino_tail->createShape(TOshapesL_9[0]);
				dino_tail->measure();
				dino_tail->init();
		}

		vector<tinyobj::shape_t> TOshapesL_10;
 		vector<tinyobj::material_t> objMaterialsL_10;

		rc = tinyobj::LoadObj(TOshapesL_10, objMaterialsL_10, errStr, (resourceDirectory + "/GodzillaObjs/GodzillaUpperTorso.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
				dino_up_torso = make_shared<Shape>();
				dino_up_torso->createShape(TOshapesL_10[0]);
				dino_up_torso->measure();
				dino_up_torso->init();
		}


		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();

		initCollision();
	}

	// Code to load in the texture
	void initTex(const std::string& resourceDirectory)
	{
		texture = make_shared<Texture>();
		texture->setFilename(resourceDirectory + "/alpha.bmp");
		texture->init();
		texture->setUnit(0);
		texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 100;
		float g_groundY = -0.25;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

	void initCollision() {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				collision[i][j] = 0;
			}
		}
		for (int i = 0; i < 100; i++) {
			collision[i][99] = 1;
			collision[99][i] = 1;
			collision[i][0] = 1;
			collision[0][i] = 1;
		}
		
		for (int i = 81; i < 96; i++) {
			for (int j = 77; j < 87; j++) {
				collision[i][j] = 1;
			}
		}
		for (int i = 0; i < 23; i++) {
			for (int j = 0; j < 18; j++) {
				collision[i][j] = 1;
			}
		}
		for (int i = 23; i < 34; i++) {
			for (int j = 0; j < 15; j++) {
				collision[i][j] = 1;
			}
		}
		for (int i = 34; i < 50; i++) {
			for (int j = 0; j < 13; j++) {
				collision[i][j] = 1;
			}
		}
		for (int i = 93; i < 98; i++) {
			for (int j = 84; j < 98; j++) {
				collision[i][j] = 0;
			}
		}
		collision[40][30] = 1;
		collision[40][31] = 1;
		collision[41][30] = 1;
		collision[41][31] = 1;


		collision[40][22] = 1;
		collision[40][23] = 1;
		collision[41][22] = 1;
		collision[41][23] = 1;


		collision[20][40] = 1;
		collision[20][41] = 1;
		collision[21][40] = 1;
		collision[21][41] = 1;


		collision[55][42] = 1;
		collision[55][43] = 1;
		collision[56][42] = 1;
		collision[56][43] = 1;


		collision[63][35] = 1;
		collision[63][36] = 1;
		collision[62][35] = 1;
		collision[62][36] = 1;

		



	}

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	texture0->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
  		SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for(GLuint i = 0; i < faces.size(); i++) {
			unsigned char *data =
			stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else {
				cout << "failed to load: " << (dir+faces[i]).c_str() << endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	} 

     //helper function to pass material data to the GPU
	void SetMaterial(shared_ptr<Program> curS, int i) {

    	switch (i) {
    		case 0: //purple
    			glUniform3f(curS->getUniform("MatAmb"), 0.096, 0.046, 0.095);
    			glUniform3f(curS->getUniform("MatDif"), 0.96, 0.46, 0.95);
    			glUniform3f(curS->getUniform("MatSpec"), 0.45, 0.23, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 120.0);
    		break;
    		case 1: // pink
    			glUniform3f(curS->getUniform("MatAmb"), 0.063, 0.038, 0.1);
    			glUniform3f(curS->getUniform("MatDif"), 0.63, 0.38, 1.0);
    			glUniform3f(curS->getUniform("MatSpec"), 0.3, 0.2, 0.5);
    			glUniform1f(curS->getUniform("MatShine"), 4.0);
    		break;
    		case 2: 
    			glUniform3f(curS->getUniform("MatAmb"), 0.004, 0.05, 0.09);
    			glUniform3f(curS->getUniform("MatDif"), 0.04, 0.5, 0.9);
    			glUniform3f(curS->getUniform("MatSpec"), 0.02, 0.25, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 27.9);
    		break;
			case 3: //new material
    			glUniform3f(curS->getUniform("MatAmb"), 0.043, 0.2, 0.09);
    			glUniform3f(curS->getUniform("MatDif"), 0.9, 0.5, 0.4);
    			glUniform3f(curS->getUniform("MatSpec"), 0.12, 0.03, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 32);
    		break;
  		}
	}

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}


   	/* code to draw waving hierarchical model */
   	void drawHierModel(shared_ptr<MatrixStack> Model, shared_ptr<Program> prog) {
   		// simplified for releaes code
		Model->pushMatrix();
			Model->loadIdentity();
			Model->translate(vec3(0, 0, -6));
			Model->scale(vec3(2.3));
			setModel(prog, Model);
			sphere->draw(prog);
		Model->popMatrix();
   	}

	void movement() {
		float sensitivity = 0.2;
		
		if(moveLeft) {
			vec3 res = strafe - (sensitivity*u);
			
			if (collision[static_cast<int>(res[0])+50][static_cast<int>(res[2])+50] != 1) {
				strafe = res;
			}

		}
		if(moveRight) {
			vec3 res = strafe + (sensitivity*u);
			if (collision[static_cast<int>(res[0])+50][static_cast<int>(res[2])+50] != 1) {
				strafe = res;
			}
		}
		if(moveForward) {
			vec3 res = strafe - (sensitivity*vec3(w[0], 0, w[2]));
			if (collision[static_cast<int>(res[0])+50][static_cast<int>(res[2])+50] != 1) {
				strafe = res;
			}
		}
		if(moveBackward) {
			vec3 res = strafe + (sensitivity*vec3(w[0], 0, w[2]));
			if (collision[static_cast<int>(res[0])+50][static_cast<int>(res[2])+50] != 1) {
				strafe = res;
			}
		}
	}

	void getItem() {
		if(static_cast<int>(strafe[0]) == 45 && static_cast<int>(strafe[2]) == 34) {
			collect_item_1 = true;
		}
		if(static_cast<int>(strafe[0]) == -38 && static_cast<int>(strafe[2]) == 8) {
			collect_item_2 = true;
		}
		if(static_cast<int>(strafe[0]) == 0 && static_cast<int>(strafe[2]) == -4) {
			collect_item_3 = true;
		}
		if(static_cast<int>(strafe[0]) == -40 && static_cast<int>(strafe[2]) == -20) {
			collect_item_4 = true;
		}
		if(static_cast<int>(strafe[0]) == 35 && static_cast<int>(strafe[2]) == -35) {
			collect_item_5 = true;
		}
	}



	void render(float frametime) {
		

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		//update the camera position
		

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		eye = vec3(0, 1, 0);
		lookat = vec3(PI*cos(g_phi)*cos(g_theta),PI*sin(g_phi),PI*cos(g_phi)*cos((PI/2.0)-g_theta));
		up = vec3(0, 1, 0);

		

		if(jump_up) {
			jump_time += 0.01;
			strafe = strafe + vec3(0,(-4.9*jump_time*jump_time + 2*jump_time),0);
			if (strafe[1] == 0 || strafe[1] < 0) {
				strafe[1] = 0;
				jump_up = false;
				jump_time = 0;
			}
		}
		
		

		glm::mat4 Cam = glm::lookAt(eye + strafe, lookat + strafe, up);
		gaze = glm::normalize(lookat - eye);
		w = -(gaze/glm::length(gaze));
		u = glm::cross(up, w);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
		glUniform3f(prog->getUniform("lightPos"), -2.0+lightTrans, 2.0, 2.0);

		// draw the array of bunnies
		Model->pushMatrix();

			for (int i = 1; i < 3; i++) {
			  Model->pushMatrix();
				Model->translate(vec3(-13*i, -2, -9*i));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(-5*i, -2, -14*i));
				Model->rotate( 4, vec3(0, 1, 0));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(10*i, -2, -17*i));
				Model->rotate( 1, vec3(0, 0, 1));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(10*i, -2, -17*i));
				
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  

			}


			for (int i = -1; i > -3; i--) {
			  Model->pushMatrix();
				Model->translate(vec3(-13*i, -2, -9*i));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(-5*i, -2, -14*i));
				Model->rotate( 4, vec3(0, 1, 0));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(10*i, -2, -17*i));
				Model->rotate( 1, vec3(0, 0, 1));
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  Model->pushMatrix();
				Model->translate(vec3(10*i, -2, -17*i));
				
				Model->scale(vec3(0.85, 0.85, 0.85));
				SetMaterial(prog, (colorMode+3)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				palm->draw(prog);
				palm2->draw(prog);
			  Model->popMatrix();

			  

			}
			Model->pushMatrix();
				
				
				Model->translate(vec3(9, -1, -4));
				Model->scale(vec3(0.2, 0.2, 0.2));
				SetMaterial(prog, (colorMode+2)%4);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				frog->draw(prog);
			Model->popMatrix();
		Model->popMatrix();

			
			//Heirarchical modeling dino Mesh

			SetMaterial(prog, 3);

			Model->pushMatrix();


				Model->scale(vec3(0.6, 0.6, 0.6));
				
				Model->translate(vec3(40, -0.5, -30));

				Model->translate(vec3(dinoMoveTheta, 0, dinoMoveTheta));
				Model->rotate(0.8, vec3(0, 1, 0));

				

				if (dinoMoveCheck > 0) {
					Model->rotate(3.14159, vec3(0, 1, 0));
				}

				
			

				//Dino Head
				Model->pushMatrix();

				
				Model->translate(vec3(1, 8.1, -1.3));
				Model->rotate( -0.2 * dinoTheta, vec3(0, 1, 0));
				Model->translate(vec3(0, 0, -0.2));

				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				dino_head->draw(prog);

				Model->popMatrix();

				//Dino Torso
				Model->pushMatrix();

					//Dino Left Arm
					Model->pushMatrix();
						Model->rotate( -0.02 * dinoTheta, vec3(0, 0, 1));
						Model->rotate( -0.03 * dinoTheta, vec3(1, 0, 0));
						Model->translate(vec3(-1.3, 4.4, -.8));
						Model->rotate( -0.1 * dinoTheta, vec3(1, 0, 0));
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						dino_left_arm->draw(prog);

					Model->popMatrix();

					//Dino Right Arm
					Model->pushMatrix();

						Model->rotate( 0.02 * dinoTheta, vec3(0, 0, 1));
						Model->rotate( 0.02 * dinoTheta, vec3(1, 0, 0));
						Model->translate(vec3(3.3, 4.4, -.8));
						Model->rotate( 0.1 * dinoTheta, vec3(1, 0, 0));
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						dino_right_arm->draw(prog);

					Model->popMatrix();

					//Dino tail
					Model->pushMatrix();

					
						Model->rotate( 0.3 * dinoTheta, vec3(0, 1, 0));
						Model->translate(vec3(1, 2.8, 4.8));
						

						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						dino_tail->draw(prog);

					Model->popMatrix();

					//Dino Left Upper Leg
					Model->pushMatrix();
						Model->rotate( -0.2 * dinoTheta, vec3(0, 0, 1));
						Model->translate(vec3(-.2, -0.1, -0.08));

						//Dino Left Lower Leg
						Model->pushMatrix();
							Model->translate(vec3(0, 1, 0));
							Model->rotate( -0.5 * dinoTheta, vec3(1, 0, 0));
							Model->translate(vec3(0, -1.1, 0));
							glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
							dino_left_low_leg->draw(prog);

						Model->popMatrix();

						Model->translate(vec3(0, 2, 0.5));
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						dino_left_up_leg->draw(prog);

					Model->popMatrix();

					//Dino Right Upper Leg
					Model->pushMatrix();
						Model->rotate( 0.2 * dinoTheta, vec3(0, 0, 1));
						Model->translate(vec3(2.1, -0.1, -0.08));

						//Dino Right Lower Leg
						Model->pushMatrix();
							Model->translate(vec3(0, 1, 0));
							Model->rotate( 0.5 * dinoTheta, vec3(1, 0, 0));
							Model->translate(vec3(0, -1.1, 0));
							glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
							dino_right_low_leg->draw(prog);

						Model->popMatrix();

						Model->translate(vec3(0, 2, 0.5));
					
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						dino_right_up_leg->draw(prog);

					Model->popMatrix();

					Model->translate(vec3(1, 2.5, 1.1));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					dino_low_torso->draw(prog);

					Model->translate(vec3(0, 2.9, 0));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					dino_up_torso->draw(prog);

				Model->popMatrix();

				
				
			Model->popMatrix();


		prog->unbind();

		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
  		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
		glUniform3f(texProg->getUniform("lightPos"), 2.0+lightTrans, 2.0, 2.9);
		glUniform1f(texProg->getUniform("MatShine"), 27.9);
		glUniform1i(texProg->getUniform("flip"), 1);
		glUniform1i(texProg->getUniform("intensity"), 1);
		texture2->bind(texProg->getUniform("Texture0"));

		Model->pushMatrix();
			Model->translate(vec3(13, 0, -15));
			Model->scale(vec3(0.85, 0.85, 0.85));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			moai->draw(texProg);
			
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(6, 0, -8));
			Model->rotate( -1, vec3(0, 1, 0));
			Model->rotate( 0.3, vec3(0, 0, 1));
			Model->rotate( -0.3, vec3(1, 0, 0));
			Model->scale(vec3(0.85, 0.85, 0.85));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			moai->draw(texProg);
			
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(-30, 0, -10));
			Model->rotate( .9, vec3(0, 1, 0));
			Model->rotate( 0.3, vec3(0, 0, 1));
			Model->rotate( -0.3, vec3(1, 0, 0));
			Model->scale(vec3(0.85, 0.85, 0.85));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			moai->draw(texProg);
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(-10, 0, -20));
			Model->rotate( -.5, vec3(0, 1, 0));
			Model->rotate( 0.4, vec3(0, 0, 1));
			Model->rotate( -0.1, vec3(1, 0, 0));
			Model->scale(vec3(0.85, 0.85, 0.85));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			moai->draw(texProg);
		Model->popMatrix();

		texture3->bind(texProg->getUniform("Texture0"));

		Model->pushMatrix();
			Model->translate(vec3(-30, -3, -35));
			Model->rotate( -.5, vec3(0, 1, 0));
			Model->scale(vec3(0.5, 0.5, 0.5));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			alpine->draw(texProg);
		Model->popMatrix();

		texture4->bind(texProg->getUniform("Texture0"));

		Model->pushMatrix();
			Model->translate(vec3(0,0.3+(0.5*itemTheta),-4));
			Model->scale(vec3(0.006, 0.006, 0.006));

			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(!collect_item_3) {	
				item->draw(texProg);
			}
			
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(-38,0.3+(0.5*itemTheta),8));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(!collect_item_2) {	
				item->draw(texProg);
			}
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(-40,0.3+(0.5*itemTheta),-20));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(!collect_item_4) {	
				item->draw(texProg);
			}
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(45,0.3+(0.5*itemTheta),34));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(!collect_item_1) {	
				item->draw(texProg);
			}
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(35,0.3+(0.5*itemTheta),-35));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(!collect_item_5) {	
				item->draw(texProg);
			}
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(0,0,0));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(collect_item_1) {	
				item->draw(texProg);
			}
		Model->popMatrix();
		Model->pushMatrix();
			Model->translate(vec3(0,1.5,0));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(collect_item_2) {	
				item->draw(texProg);
			}
		Model->popMatrix();
		Model->pushMatrix();
			Model->translate(vec3(0,3,0));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(collect_item_3) {	
				item->draw(texProg);
			}
		Model->popMatrix();
		Model->pushMatrix();
			Model->translate(vec3(0,4.5,0));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(collect_item_4) {	
				item->draw(texProg);
			}
		Model->popMatrix();
		Model->pushMatrix();
			Model->translate(vec3(0,6,0));
			Model->scale(vec3(0.006, 0.006, 0.006));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			if(collect_item_5) {	
				item->draw(texProg);
			}
		Model->popMatrix();


		texture5->bind(texProg->getUniform("Texture0"));

		Model->pushMatrix();
			
			Model->translate(vec3(35,-1,28));
			
			Model->scale(vec3(0.3, 0.3, 0.3));
			Model->rotate( 2.3, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			house->draw(texProg);
		Model->popMatrix();


		Model->pushMatrix();
			
			Model->translate(vec3(-35,-1,18));
			
			Model->scale(vec3(0.05, 0.05, 0.05));
			Model->rotate( 1.5, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			for (int i = 0; i < tent.size(); i++) {
				tent[i]->draw(texProg);
			}
			
		Model->popMatrix();

		Model->pushMatrix();
			
			Model->translate(vec3(-38,-1,8));
			
			Model->scale(vec3(0.05, 0.05, 0.05));
			Model->rotate( 1.5, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			for (int i = 0; i < tent.size(); i++) {
				tent[i]->draw(texProg);
			}
			
		Model->popMatrix();

		Model->pushMatrix();
			
			Model->translate(vec3(-35,-1,-2));
			
			Model->scale(vec3(0.05, 0.05, 0.05));
			Model->rotate( 1.2, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			for (int i = 0; i < tent.size(); i++) {
				tent[i]->draw(texProg);
			}
			
		Model->popMatrix();

		Model->pushMatrix();
			
			Model->translate(vec3(-27,-1,6));
			
			Model->scale(vec3(0.03, 0.03, 0.03));
			Model->rotate( 1.2, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			campfire->draw(texProg);
			
			
		Model->popMatrix();

		Model->pushMatrix();
			
			Model->translate(vec3(-27,-1,18));
			
			Model->scale(vec3(0.03, 0.03, 0.03));
			Model->rotate( 1.2, vec3(0, 1, 0));
			
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			campfire->draw(texProg);
			
		Model->popMatrix();

		
			

		//draw big background sphere
		texture1->bind(texProg->getUniform("Texture0"));

		glUniform1i(texProg->getUniform("flip"), 1);
		glUniform1i(texProg->getUniform("intensity"),2);
		drawGround(texProg);

		texProg->unbind();

		// Particle Shader Program
		partProg->bind();
		Model->pushMatrix();
			texture->bind(partProg->getUniform("alphaTexture"));
			CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
			CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("V"), 1, GL_FALSE, value_ptr(Cam)));
			CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));

			CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.1, 0.1));

			thePartSystem->drawMe(partProg);
			thePartSystem->update();

			thePartSystem2->drawMe(partProg);
			thePartSystem2->update();

			CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.1, 0.9, 0.1));

			if(!collect_item_1) {
				thePartSystem3->drawMe(partProg);
				thePartSystem3->update();
			}
			
			if(!collect_item_2) {
				thePartSystem4->drawMe(partProg);
				thePartSystem4->update();
			}

			if(!collect_item_3) {
				thePartSystem5->drawMe(partProg);
				thePartSystem5->update();
			}

			if(!collect_item_4) {
				thePartSystem6->drawMe(partProg);
				thePartSystem6->update();
			}

			if(!collect_item_5) {
				thePartSystem7->drawMe(partProg);
				thePartSystem7->update();
			}

			if(collect_item_1 && collect_item_2 && collect_item_3 && collect_item_4 && collect_item_5) {
				CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.1, 0.1));
				WinPart1->drawMe(partProg);
				WinPart1->update();
				CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.1, 0.1, 0.9));
				WinPart2->drawMe(partProg);
				WinPart2->update();
				CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.1, 0.9, 0.1));
				WinPart3->drawMe(partProg);
				WinPart3->update();
				CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.1, 0.1, 0.9));
				WinPart4->drawMe(partProg);
				WinPart4->update();
				CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.1, 0.1));
				WinPart5->drawMe(partProg);
				WinPart5->update();

			}


		Model->popMatrix();
		partProg->unbind();


		//to draw the sky box bind the right shader
		cubeProg->bind();
		//set the projection matrix - can use the same one
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//set the depth function to always draw the box!
		glDepthFunc(GL_LEQUAL);
		//set up view matrix to include your view transforms
		//(your code likely will be different depending
		glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE,value_ptr(Cam) );
		//set and send model transforms - likely want a bigger cube
		glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE,value_ptr(Model->topMatrix()));
		//bind the cube map texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		//draw the actual cube
		Model->pushMatrix();
			Model->loadIdentity();
			Model->scale(vec3(100.0));
			setModel(cubeProg, Model);
			cube->draw(cubeProg);
		Model->popMatrix();
		
		//set the depth test back to normal!
		glDepthFunc(GL_LESS);
		//unbind the shader for the skybox
		cubeProg->unbind(); 

		// Pop matrix stacks.
		Projection->popMatrix();

		itemTheta = sin(glfwGetTime()*.8);

		dinoTheta = sin(glfwGetTime()*.8);

		dinoMoveTheta = 10*sin(glfwGetTime()*0.2);
		dinoMoveCheck = 10*cos(glfwGetTime()*0.2);

		movement();
		getItem();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(1024, 768);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initTex(resourceDir);
	application->initGeom(resourceDir);

	auto lastTime = chrono::high_resolution_clock::now();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
