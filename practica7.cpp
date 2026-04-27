/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;//ladrillos
Texture dirtTexture;//suelo
Texture Image_2Texture;//textura de la nave
Texture pisoTexture;
Texture AgaveTexture;
Texture aguaTexture;


Model Kitt_M; //Carro
Model Llanta_M;
//Model Blackhawk_M;//Helicoptero
Model Nave_M;
Model Cofre_M;
Model LlantaFI_M;
Model LlantaFD_M;
Model LlantaAD_M;
Model LlantaAI_M;
Model Carro_M;
Model Lampara_M;
Model PezCuerpo_M;
Model PezFoco_M;
Model PezAntena_M; 


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {

		0, 2, 1,	// Cara 1 
		3, 5, 4,	// Cara 2 
		6, 8, 7,	// Cara 3 
		9, 11, 10,	// Cara 4 
		12, 14, 13,	// Cara 5 
		15, 17, 16,	// Cara 6 
		18, 20, 19,	// Cara 7 
		21, 23, 22	// Cara 8 

	};	GLfloat cubo_vertices[] = {
			// x      y      z         S      T         NX    NY    NZ
			// Cara 1: Arriba-Frente-Derecha
			0.0f,  0.5f,  0.0f,    0.5f,  1.0f,     0.0f, 0.0f, 0.0f, // 0 (Arriba)
			0.0f,  0.0f,  0.5f,    0.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 1 (Frente)
			0.5f,  0.0f,  0.0f,    1.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 2 (Derecha)

			// Cara 2: Arriba-Derecha-Atras
			0.0f,  0.5f,  0.0f,    0.5f,  1.0f,     0.0f, 0.0f, 0.0f, // 3 (Arriba)
			0.5f,  0.0f,  0.0f,    0.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 4 (Derecha)
			0.0f,  0.0f, -0.5f,    1.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 5 (Atrás)

			// Cara 3: Arriba-Atras-Izquierda
			0.0f,  0.5f,  0.0f,    0.5f,  1.0f,     0.0f, 0.0f, 0.0f, // 6 (Arriba)
			0.0f,  0.0f, -0.5f,    0.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 7 (Atrás)
			-0.5f, 0.0f,  0.0f,    1.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 8 (Izquierda)

			// Cara 4: Arriba-Izquierda-Frente
			0.0f,  0.5f,  0.0f,    0.5f,  1.0f,     0.0f, 0.0f, 0.0f, // 9 (Arriba)
			-0.5f, 0.0f,  0.0f,    0.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 10 (Izquierda)
			0.0f,  0.0f,  0.5f,    1.0f,  0.0f,     0.0f, 0.0f, 0.0f, // 11 (Frente)

			// Cara 5: Abajo-Frente-Derecha
			0.0f, -0.5f,  0.0f,    0.5f,  0.0f,     0.0f, 0.0f, 0.0f, // 12 (Abajo)
			0.5f,  0.0f,  0.0f,    1.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 13 (Derecha)
			0.0f,  0.0f,  0.5f,    0.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 14 (Frente)

			// Cara 6: Abajo-Derecha-Atras
			0.0f, -0.5f,  0.0f,    0.5f,  0.0f,     0.0f, 0.0f, 0.0f, // 15 (Abajo)
			0.0f,  0.0f, -0.5f,    1.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 16 (Atrás)
			0.5f,  0.0f,  0.0f,    0.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 17 (Derecha)

			// Cara 7: Abajo-Atras-Izquierda
			0.0f, -0.5f,  0.0f,    0.5f,  0.0f,     0.0f, 0.0f, 0.0f, // 18 (Abajo)
			-0.5f, 0.0f,  0.0f,    1.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 19 (Izquierda)
			0.0f,  0.0f, -0.5f,    0.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 20 (Atrás)

			// Cara 8: Abajo-Izquierda-Frente
			0.0f, -0.5f,  0.0f,    0.5f,  0.0f,     0.0f, 0.0f, 0.0f, // 21 (Abajo)
			0.0f,  0.0f,  0.5f,    1.0f,  1.0f,     0.0f, 0.0f, 0.0f, // 22 (Frente)
			-0.5f, 0.0f,  0.0f,    0.0f,  1.0f,     0.0f, 0.0f, 0.0f  // 23 (Izquierda)

	};

	calcAverageNormals(cubo_indices, 24, cubo_vertices, 192, 8, 5);

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}

void CrearPeceraYAgua()
{
	// 1. CRISTAL PECERA,normales hacia adentro
	unsigned int pecera_indices[] = {
		0, 2, 1,  0, 3, 2, // Frente
		4, 6, 5,  4, 7, 6, // Derecha
		8, 10, 9, 8, 11, 10, // Atrás
		12, 14, 13, 12, 15, 14, // Izquierda
		16, 18, 17, 16, 19, 18, // Abajo
		20, 22, 21, 20, 23, 22  // Arriba
	};

	GLfloat pecera_vertices[] = {
		// Frente
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		// Derecha
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		 // Atrás
		  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  // Izquierda
		  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  // Abajo
		  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  // Arriba
		  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f
	};

	calcAverageNormals(pecera_indices, 36, pecera_vertices, 192, 8, 5);
	Mesh* pecera = new Mesh();
	pecera->CreateMesh(pecera_vertices, pecera_indices, 192, 36);
	meshList.push_back(pecera);

	//PLANO DEL AGUA SUPERIOR
	unsigned int agua_indices[] = { 0, 1, 2,  0, 2, 3 };
	GLfloat agua_vertices[] = {
		// X      Y      Z       S     T      NX    NY    NZ
		-0.5f,  0.0f,  0.5f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.0f,  0.5f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.0f, -0.5f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.0f, -0.5f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f
	};
	calcAverageNormals(agua_indices, 6, agua_vertices, 32, 8, 5);
	Mesh* agua = new Mesh();
	agua->CreateMesh(agua_vertices, agua_indices, 32, 6);
	meshList.push_back(agua); // Ahora será meshList[6]
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearPeceraYAgua();


	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	Image_2Texture = Texture("Textures/Image_2.tga");
	Image_2Texture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	aguaTexture = Texture("Textures/agua.tga");
	aguaTexture.LoadTextureA();





	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");

	Cofre_M = Model();
	Cofre_M.LoadModel("Models/cofre.obj");
	LlantaFI_M = Model();
	LlantaFI_M.LoadModel("Models/llSuperiorIzq.obj");
	LlantaFD_M = Model();
	LlantaFD_M.LoadModel("Models/llSuperiorDer.obj");
	LlantaAD_M = Model();
	LlantaAD_M.LoadModel("Models/llAtrasDer.obj");
	LlantaAI_M = Model();
	LlantaAI_M.LoadModel("Models/llAtrasIzq.obj");

	Carro_M = Model();
	Carro_M.LoadModel("Models/CarroCompleto.obj");

	Lampara_M = Model();
	Lampara_M.LoadModel("Models/lampara.obj");

	PezCuerpo_M = Model();
	PezCuerpo_M.LoadModel("Models/cuerpo_pez.obj");

	PezAntena_M = Model(); 
	PezAntena_M.LoadModel("Models/antena_pez.obj");

	PezFoco_M = Model(); 
	PezFoco_M.LoadModel("Models/foco_pez.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;


	// Luz puntual de la Lámpara (Blanca)
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.2f, 1.0f,
		5.0f, 2.0f, -5.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Luz Foco del Pez 
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f, // Color Azul
		0.8f, 1.0f,                               
		0.0f, 0.0f, 0.0f,                         
		0.3f, 0.2f, 0.1f);                        
	pointLightCount++;


	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		8.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//Faro Frontal

	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.05f, 0.0f,
		15.0f);
	spotLightCount++;


	// Luz del helicóptero 
	// Faro delantero de la Nave (Apunta hacia X negativa)
	spotLights[3] = SpotLight(
		1.0f, 1.0f, 0.0f,   // Color amarillo
		1.0f, 4.0f,         // intensidad ambiente y difusa
		0.0f, 5.0f, 6.0f,   // Posición
		-1.0f, -1.0f, 0.0f, // Dirección
		1.0f, 0.02f, 0.005f,// Atenuación 
		30.0f);             // Ángulo de apertura
	spotLightCount++;
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	GLfloat heliX = 0.0f; // Posición inicial del helicóptero 
	// Luz 4: Faro trasero de la Nave (Apunta hacia X positiva)
	spotLights[4] = SpotLight(
		1.0f, 1.0f, 0.0f,
		1.0f, 4.0f,
		0.0f, 5.0f, 6.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.005f,
		30.0f);
	spotLightCount++;

	//SPOTLIGHT DEL PEZ 
	spotLights[5] = SpotLight(
		0.0f, 1.0f, 0.0f,   // Color Verde 
		1.0f, 3.0f,         
		0.0f, 0.0f, 0.0f,   
		0.0f, -1.0f, 0.0f,  
		1.0f, 0.02f, 0.005f,
		20.0f);             
	spotLightCount++;


	//PREVIO
	GLfloat constAtt = 1.0f;
	GLfloat linAtt = 0.09f;
	GLfloat expAtt = 0.032f;
	// Variables para controlar la dirección del Spotlight
	GLfloat dirPezX = 0.0f;
	GLfloat dirPezY = -1.0f;
	GLfloat dirPezZ = 0.0f;
	//Colores del faro
	glm::vec3 coloresFaro[] = {
		glm::vec3(1.0f, 0.0f, 0.0f), // 0: Rojo
		glm::vec3(1.0f, 1.0f, 0.0f), // 1: Amarillo
		glm::vec3(0.0f, 1.0f, 0.0f), // 2: Verde
		glm::vec3(0.0f, 1.0f, 1.0f), // 3: Cian
		glm::vec3(0.0f, 0.0f, 1.0f), // 4: Azul
		glm::vec3(1.0f, 0.0f, 1.0f)  // 5: Magenta
	};
	int indiceColorFaro = 0;
	bool teclaCPresionada = false;

	//encendido y apagado de lámpara
	bool lamparaEncendida = true;
	bool teclaBPresionada = false;

	// Variables para la Nave
	bool naveAvanza = false;
	bool naveRetrocede = false;

	// Pez
	bool pezLuzEncendida = true;
	bool teclaMPresionada = false;

	// Variable para el movimiento del pez
	GLfloat movPezV = 0.0f;

	// Arreglos temporales para el filtrado de luces 
	PointLight lucesActivas[3];
	unsigned int conteoActivas = 0;

	SpotLight spotLucesActivas[10];
	unsigned int conteoSpotActivas = 0;



	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Encendido y Apagado de la lámpara tecla B 
		if (mainWindow.getsKeys()[GLFW_KEY_B]) {
			if (!teclaBPresionada) {
				lamparaEncendida = !lamparaEncendida;
				teclaBPresionada = true;
			}
		}
		else {
			teclaBPresionada = false;
		}

		// Encendido y Apagado de la luz del PEZ (Tecla M) 
		if (mainWindow.getsKeys()[GLFW_KEY_M]) {
			if (!teclaMPresionada) {
				pezLuzEncendida = !pezLuzEncendida;
				teclaMPresionada = true;
			}
		}
		else {
			teclaMPresionada = false;
		}


		// Movimiento del helicóptero hacia adelante T
		// Atrás G
		// LÓGICA DE MOVIMIENTO DE LA NAVE
		// Solo asignamos valor
		naveAvanza = false;
		naveRetrocede = false;

		// Movimiento hacia adelante (T)
		if (mainWindow.getsKeys()[GLFW_KEY_T]) {
			heliX -= 0.5f * deltaTime;
			naveAvanza = true; // Encendemos faro delantero
		}
		// Movimiento hacia atrás (G) 
		else if (mainWindow.getsKeys()[GLFW_KEY_G]) {
			heliX += 0.5f * deltaTime;
			naveRetrocede = true; // Encendemos faro trasero
		}
		// Movimiento pez en V (Vertical)
		// Avanzar  (Tecla V)
		if (mainWindow.getsKeys()[GLFW_KEY_V]) {
			movPezV += 0.05f * deltaTime;
		}
		// Retroceder (Tecla J)
		else if (mainWindow.getsKeys()[GLFW_KEY_J]) {
			movPezV -= 0.05f * deltaTime;
		}

		// Topes lógicos para que no se salga de la pecera
		if (movPezV > 1.0f) movPezV = 1.0f;
		if (movPezV < -1.0f) movPezV = -1.0f;


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();


		//información en el shader de intensidad y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//Atenuación

		// 1. Factor Constante (con) con I y K
		if (mainWindow.getsKeys()[GLFW_KEY_I]) { constAtt += 0.1f; }
		if (mainWindow.getsKeys()[GLFW_KEY_K]) { constAtt -= 0.1f; }
		if (constAtt < 0.1f) constAtt = 0.1f; // Evitar que sea 0 para no apagar la luz

		// 2. Factor Lineal (lin) con O y L
		if (mainWindow.getsKeys()[GLFW_KEY_O]) { linAtt += 0.01f; }
		if (mainWindow.getsKeys()[GLFW_KEY_L]) { linAtt -= 0.01f; }
		if (linAtt < 0.0f) linAtt = 0.0f;
		//3. Factor Exponencial (exp) con P
		if (mainWindow.getsKeys()[GLFW_KEY_P]) { expAtt += 0.001f; }
		if (mainWindow.getsKeys()[GLFW_KEY_SEMICOLON]) { expAtt -= 0.001f; }
		if (expAtt < 0.0f) expAtt = 0.0f;

		// Cambio de colores de faro tecla C
		if (mainWindow.getsKeys()[GLFW_KEY_C]) {
			if (!teclaCPresionada) {
				indiceColorFaro++;
				if (indiceColorFaro > 5) {
					indiceColorFaro = 0;
				}
				teclaCPresionada = true;
			}
		}
		else {
			teclaCPresionada = false;
		}

		spotLights[2].SetColor(coloresFaro[indiceColorFaro]);
		//SPOTLIGHT DEL PEZ 
		GLfloat velDir = 1.0f * deltaTime; // Velocidad con la que gira la luz

		// Teclas 1 y 2 para el Eje X 
		if (mainWindow.getsKeys()[GLFW_KEY_1]) { dirPezX += velDir; }
		if (mainWindow.getsKeys()[GLFW_KEY_2]) { dirPezX -= velDir; }

		// Teclas 3 y 4 para el Eje Y 
		if (mainWindow.getsKeys()[GLFW_KEY_3]) { dirPezY += velDir; }
		if (mainWindow.getsKeys()[GLFW_KEY_4]) { dirPezY -= velDir; }

		// Teclas 5 y 6 para el Eje Z 
		if (mainWindow.getsKeys()[GLFW_KEY_5]) { dirPezZ += velDir; }
		if (mainWindow.getsKeys()[GLFW_KEY_6]) { dirPezZ -= velDir; }

		//Luz

		//Posición de la luz
		glm::vec3 posCoche = glm::vec3(0.0f + mainWindow.getmuevex(), 0.44f, 3.29f);
		glm::vec3 offsetFaro = glm::vec3(3.0f, 0.0f, 0.0f);
		spotLights[2].SetFlash(posCoche + offsetFaro, glm::vec3(-1.0f, 0.0f, 0.0f));


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Renderizado del Helicóptero
		// Renderizado de la Nave
		model = glm::mat4(1.0);
		glm::vec3 posNaveBase = glm::vec3(heliX, 5.0f, 6.0f);
		model = glm::translate(model, posNaveBase);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		//JERARQUÍA LUCES DE NAVE 
		// Faro delantero inclinado 
		glm::vec3 posFaroDelantero = posNaveBase + glm::vec3(-1.5f, 0.0f, 0.0f);
		glm::vec3 dirFaroDelantero = glm::normalize(glm::vec3(-1.0f, -1.2f, 0.0f));
		spotLights[3].SetFlash(posFaroDelantero, dirFaroDelantero);

		// Faro trasero inclinado 
		glm::vec3 posFaroTrasero = posNaveBase + glm::vec3(1.5f, 0.0f, 0.0f);
		glm::vec3 dirFaroTrasero = glm::normalize(glm::vec3(1.0f, -1.2f, 0.0f));
		spotLights[4].SetFlash(posFaroTrasero, dirFaroTrasero);


		// Renderizado de la Lámpara
		model = glm::mat4(1.0);
		// Posición base de la lámpara
		glm::vec3 posLampara = glm::vec3(5.0f, 0.0f, -5.0f);
		model = glm::translate(model, posLampara);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_M.RenderModel();


		// Jerarquía luz Lámpara
		pointLights[1].SetPos(posLampara + glm::vec3(0.0f, 2.0f, 0.0f));
		pointLights[1].SetAttenuation(constAtt, linAtt, expAtt);

		// Renderizado del dado de 8 caras
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 4.5f, 3.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glm::vec3 colorOctaedro = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorOctaedro));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glBindTexture(GL_TEXTURE_2D, 0);

		//RENDERIZADO DEL CUERPO DEL PEZ 
		model = glm::mat4(1.0f);

		GLfloat pezX = -12.0f + movPezV;
		GLfloat pezY = -0.2f + abs(movPezV);

		model = glm::translate(model, glm::vec3(pezX, pezY, 4.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// Color base para el cuerpo 
		glm::vec3 colorCuerpo = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorCuerpo));

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PezCuerpo_M.RenderModel();

		//RENDERIZADO DE LA ANTENA 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(pezX + 0.07f, pezY + 0.32f, 3.89f));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PezAntena_M.RenderModel();

		//RENDERIZADO DEL FOCO
		model = glm::mat4(1.0f);
		glm::vec3 posFoco = glm::vec3(pezX, pezY + 0.9f, 4.2f);

		model = glm::translate(model, posFoco);
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//color azul 
		glm::vec3 colorDelFoco = glm::vec3(0.0f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorDelFoco));

		PezFoco_M.RenderModel();

		//LIGAR LUZ PUNTUAL AL FOCO 
		pointLights[2].SetPos(glm::vec3(pezX, pezY + 0.9f, 4.4f));
		glm::vec3 direccionPez = glm::vec3(dirPezX, dirPezY, dirPezZ);

		//vector de dirección 
		if (glm::length(direccionPez) > 0.001f) {
			direccionPez = glm::normalize(direccionPez); 
		}
		else {
			direccionPez = glm::vec3(0.0f, -1.0f, 0.0f); 
		}
		spotLights[5].SetFlash(glm::vec3(pezX, pezY + 0.9f, 4.4f), direccionPez);



		//RENDERIZADO DE LA PECERA Y EL AGUA 
		glEnable(GL_BLEND);

		// Cristal (Normales hacia adentro)
		glBlendColor(0.0f, 0.0f, 0.0f, 0.20f);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-12.0f, 0.5f, 4.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// Un color más claro para que parezca cristal iluminado
		glm::vec3 colorCristal = glm::vec3(0.4f, 0.8f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorCristal));
		aguaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Tapa de Agua
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-12.0f, 1.76f, 4.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.3f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		aguaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		glDisable(GL_BLEND);

	   //LUCES

		// 1. PointLights (Lámpara)
		conteoActivas = 0; // Reiniciamos el contador cada frame
		lucesActivas[conteoActivas] = pointLights[0];
		conteoActivas++;
		if (lamparaEncendida) {
			lucesActivas[conteoActivas] = pointLights[1];
			conteoActivas++;
		}

		// 2. SpotLights Nave
		conteoSpotActivas = 0; 
		spotLucesActivas[conteoSpotActivas] = spotLights[0]; conteoSpotActivas++;
		spotLucesActivas[conteoSpotActivas] = spotLights[1]; conteoSpotActivas++;
		spotLucesActivas[conteoSpotActivas] = spotLights[2]; conteoSpotActivas++;

		// Si se mueve
		if (naveAvanza) {
			spotLucesActivas[conteoSpotActivas] = spotLights[3];
			conteoSpotActivas++;
		}
		if (naveRetrocede) {
			spotLucesActivas[conteoSpotActivas] = spotLights[4];
			conteoSpotActivas++;
		}
		//Foco del Pez 
		if (pezLuzEncendida) {
			lucesActivas[conteoActivas] = pointLights[2];
			conteoActivas++;
		}

		// SPOTLIGHT VERDE DEL PEZ
		spotLucesActivas[conteoSpotActivas] = spotLights[5];
		conteoSpotActivas++;

		//ENVÍO AL SHADER
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(lucesActivas, conteoActivas);

		shaderList[0].SetSpotLights(spotLucesActivas, conteoSpotActivas);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}