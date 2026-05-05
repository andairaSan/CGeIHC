/*
Animación:
- Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
-Compleja: Por medio de funciones y algoritmos.
-Textura Animada
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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

//Variables para la animación del Fuego
bool fuegoEncendido = false;
bool teclaF_Presionada = false;
float temporizadorFuego = 0.0f;
float offsetFuegoU = 0.0f; 
float offsetFuegoV = 0.0f; 

int columnasFuego = 1;
int filasFuego = 1;
float anchoFrameFuego = 1.0f / columnasFuego; 
float altoFrameFuego = 1.0f / filasFuego;     
float velocidadFuego = 0.08f;

//Variables para el Aeolipile
float rotacionAeolipile = 0.0f;        // Guarda el ángulo actual
float velocidadAeolipile = 1.0f;     // Velocidad de giro (grados por segundo)

//Variables para la animación del Humo

float escalaHumo = 0.0f;              // Empieza en 0 (invisible)
float escalaMaxHumo = 3.0f;           // Tamaño máximo del humo
float temporizadorCalentamiento = 0.0f;//indica cuánto tiempo ha pasado desde que se encendió el fuego
float tiempoParaHervir = 20.0f;        // Segundos que tarda en calentarse antes de salir humo 
float velocidadCrecimiento = 2.0f;    // Qué tan rápido crece/decrece el humo

// Para la textura 
float offsetHumoV = 0.0f;
float velocidadFlujoHumo = 3.0f;

//Variables para la cinemática del Lanzamiento 
bool interruptorActivado = false;
float rotacionCuchara = 0.0f;         // Ángulo de reposo
float limiteRotacionCuchara = 75.0f;  // Catapulta
float velocidadCuchara = 300.0f;      // Grados por segundo 

//Variables para la física de la Esfera 
bool esferaLanzada = false;
float posEsferaX = -3.2f;
float posEsferaY = 1.0f;
float posEsferaZ = 8.0f;
float velX = 0.0f;
float velY = 0.0f;
float velZ = 0.0f;
float gravedad = 0.2f;
int rebotesEsfera = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture FuegoTexture;
Texture HumoTexture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;
Model Humo_M;
Model Cuchara_M;
Model EsferaMetalica_M;
Model Interruptor_M;

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



//cálculo del promedio de las normales para sombreado de Phong
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	// Vértices para el fuego 
	float uMax = 1.0f / 4.0f; 
	float vMax = 1.0f / 4.0f; 

	unsigned int fuegoIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat fuegoVertices[] = {
		// x,      y,    z,         u,    v,          nx,    ny,    nz
		-0.5f, 0.0f,  0.5f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f, 
		 0.5f, 0.0f, -0.5f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f, 
		-0.5f, 0.0f, -0.5f,     0.0f, 1.0f,     0.0f, -1.0f, 0.0f, 
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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); 

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); 
	Mesh* objFuego = new Mesh();

	objFuego->CreateMesh(fuegoVertices, fuegoIndices, 32, 6);
	meshList.push_back(objFuego); 

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
	HumoTexture = Texture("Textures/humo.tga");
	HumoTexture.LoadTextureA();

	FuegoTexture = Texture("Textures/fuego.tga");
	FuegoTexture.LoadTextureA(); 


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");

	Humo_M = Model(); 
	Humo_M.LoadModel("Models/humo.obj"); 

	Cuchara_M = Model();
	Cuchara_M.LoadModel("Models/cuchara.obj");

	EsferaMetalica_M = Model();
	EsferaMetalica_M.LoadModel("Models/esfera.obj");

	Interruptor_M = Model();
	Interruptor_M.LoadModel("Models/interruptor.obj");


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
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	//animaciones por pasos
	float temporizadorNumeros = 0.0f;
	float temporizadorTexturas = 0.0f;
	float tiempoEspera = 14.0f; // Cambiará cada 14.0 segundos para que se pueda ver la animación de cada número
	bool usarTextura1 = true;  




	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		glm::vec2 toffsetHumo = glm::vec2(0.0f, offsetHumoV);
		angulovaria += 0.5f * deltaTime;
		//¿Cómo haces para que el carro no se salga del piso
		movCoche -= movOffset * deltaTime;
		rotllanta += rotllantaOffset * deltaTime;



		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// encender/apagar con la tecla F
		if (mainWindow.getsKeys()[GLFW_KEY_F])
		{
			if (!teclaF_Presionada)
			{
				fuegoEncendido = !fuegoEncendido; // Alterna: si estaba apagado lo prende, y viceversa
				teclaF_Presionada = true;         
			}
		}
		else
		{
			teclaF_Presionada = false; 
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		//CINEMÁTICA DEL AEOLIPILE
		// Si el fuego está encendido
		if (fuegoEncendido) {
			rotacionAeolipile += velocidadAeolipile * deltaTime;

			// Para evitar que el número crezca al infinito y cause desbordamiento de memoria
			if (rotacionAeolipile >= 360.0f) {
				rotacionAeolipile -= 360.0f;
			}
		}
		//LÓGICADEL HUMO
		offsetHumoV += velocidadFlujoHumo * (deltaTime * 0.25f);
		if (offsetHumoV > 1.0f) {
			offsetHumoV -= 1.0f; // Evita que el número se desborde
		}

		//Máquina de estados para el tamaño animación Compleja
		if (fuegoEncendido)
		{
			temporizadorCalentamiento += (deltaTime * 0.02f);

			// Usa la variable en lugar de un número fijo
			if (temporizadorCalentamiento > tiempoParaHervir) {
				temporizadorCalentamiento = tiempoParaHervir;
			}
		}
		else
		{
			temporizadorCalentamiento -= (deltaTime * 0.15f);

			if (temporizadorCalentamiento < 0.0f) {
				temporizadorCalentamiento = 0.0f;
			}
		}

		// ALGORITMO MATEMÁTICO: (tiempo_actual / tiempo_total) ^ potencia
		escalaHumo = escalaMaxHumo * pow((temporizadorCalentamiento / tiempoParaHervir), velocidadCrecimiento);

		//LÓGICA DE LANZAMIENTO
		//El humo al máximo activa el interruptor
		if (temporizadorCalentamiento >= tiempoParaHervir) {
			interruptorActivado = true;
		}
		else if (temporizadorCalentamiento <= 0.0f) {
			//se reinicia automáticamente cuando el humo se disipa por completo
			interruptorActivado = false;
		}

		// Mecánica rotacional
		if (interruptorActivado) {
			//La cuchara da el latigazo hacia adelante
			rotacionCuchara += velocidadCuchara * deltaTime;

			//Límite 
			if (rotacionCuchara > limiteRotacionCuchara) {
				rotacionCuchara = limiteRotacionCuchara;
			}
		}
		else {
			//regresa a su lugar más lento 
			rotacionCuchara -= (velocidadCuchara * 0.1f) * deltaTime;

			if (rotacionCuchara < 0.0f) {
				rotacionCuchara = 0.0f;
			}
		}

		//LÓGICA DE LANZAMIENTO DE LA ESFERA
		if (interruptorActivado && rotacionCuchara >= limiteRotacionCuchara && !esferaLanzada)
		{
			esferaLanzada = true;
			velY = 2.5f;   
			velX = -1.0f;   
			velZ = 0.0f; 
		}
		//tiro Parabólico con rebotes
		if (esferaLanzada)
		{
			velY -= gravedad * deltaTime;
			posEsferaX += velX * deltaTime;
			posEsferaY += velY * deltaTime;
			posEsferaZ += velZ * deltaTime;

			if (posEsferaY < 0.5f) {
				posEsferaY = 0.5f;

				if (rebotesEsfera < 3) {
					velY = -velY * 0.75f;
					velX *= 0.8f;
					rebotesEsfera++;
				}
				else {
					velX = 0.0f; velY = 0.0f; velZ = 0.0f;
				}
			}
		}
		else if (!interruptorActivado)
		{
			//reiniciar posición de la esfera cuando no está lanzada y el interruptor no está activado
			esferaLanzada = false;
			rebotesEsfera = 0;
			posEsferaX = -3.2f;
			posEsferaY = 1.0f;
			posEsferaZ = 8.0f;
		}

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
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Pista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Pista_M.RenderModel();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche - 50.0f, 0.5f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();


		//Aquí va la nave con jerarquía de modelos, completar
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Nave_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Ala_M.RenderModel();

		//AEOLIPILE
		
		//RENDERIZAR LA BASE 
		glm::mat4 modelBase = glm::mat4(1.0f);
		modelBase = glm::translate(modelBase, glm::vec3(0.0f, -0.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBase));
		Aeolipile_base_M.RenderModel();

		//RENDERIZAR LA ESFERA (El Padre que gira)
		glm::mat4 modelEsfera = modelBase; // Nace de la base
		modelEsfera = glm::translate(modelEsfera, glm::vec3(0.0f, 5.0f, -0.1f)); // Subimos a la altura de la esfera
		modelEsfera = glm::rotate(modelEsfera, rotacionAeolipile * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // y gira en su propio eje 

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelEsfera));
		Aeolipile_M.RenderModel();

		//RENDERIZAR EL CHORRO DE HUMO
		if (escalaHumo > 0.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE); 
			HumoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			// DEGRADADO el objeto 3D pasa de negro a blanco según crece, revelando la textura
			float intensidadDegradado = escalaHumo / escalaMaxHumo;
			// el humo pasa de ser oscuro/invisible a blanco intenso conforme crece
			glUniform3f(uniformColor, escalaHumo / escalaMaxHumo, escalaHumo / escalaMaxHumo, escalaHumo / escalaMaxHumo);

			//HUMO DEL TUBO DERECHO
			glm::mat4 modelHumo1 = modelEsfera; 
			modelHumo1 = glm::translate(modelHumo1, glm::vec3(0.1f, 0.6f, 0.0f));  
			modelHumo1 = glm::rotate(modelHumo1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelHumo1 = glm::rotate(modelHumo1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelHumo1 = glm::rotate(modelHumo1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			float reductor = 0.5f;
			modelHumo1 = glm::scale(modelHumo1, glm::vec3(escalaHumo * reductor, escalaHumo * reductor, escalaHumo * reductor));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo1));
			Humo_M.RenderModel();

			//HUMO DEL TUBO IZQUIERDO
			glm::mat4 modelHumo2 = modelEsfera;
			modelHumo2 = glm::translate(modelHumo2, glm::vec3(-0.1f, -0.58f, 0.0f));
			modelHumo2 = glm::rotate(modelHumo2, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelHumo2 = glm::rotate(modelHumo2, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			modelHumo2 = glm::rotate(modelHumo2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			modelHumo2 = glm::scale(modelHumo2, glm::vec3(escalaHumo * reductor, escalaHumo * reductor, escalaHumo * reductor));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo2));
			Humo_M.RenderModel();
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);

		}

		//RENDERIZAR EL INTERRUPTOR
		//rompe el vínculo con modelBase y modelEsfera
		glm::mat4 modelInterruptor = glm::mat4(1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		modelInterruptor = glm::translate(modelInterruptor, glm::vec3(-2.5f, 3.7f, 0.399f));
		if (interruptorActivado) {
			modelInterruptor = glm::rotate(modelInterruptor, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelInterruptor));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Interruptor_M.RenderModel();


		//RENDERIZAR LA CUCHARA
		glm::mat4 modelCuchara = glm::mat4(1.0f);
		modelCuchara = glm::translate(modelCuchara, glm::vec3(-7.8f, 0.0f, 8.0f));
		modelCuchara = glm::scale(modelCuchara, glm::vec3(0.5f, 0.5f, 0.5f));
		modelCuchara = glm::rotate(modelCuchara, glm::radians(rotacionCuchara), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuchara));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		Cuchara_M.RenderModel();

		//RENDERIZAR LA ESFERA
		glm::mat4 modelEsferaMetalica = glm::mat4(1.0f);

		if (!esferaLanzada) {
			modelEsferaMetalica = glm::translate(modelEsferaMetalica, glm::vec3(-7.8f, 0.0f, 8.0f));
			modelEsferaMetalica = glm::rotate(modelEsferaMetalica, glm::radians(rotacionCuchara), glm::vec3(0.0f, 0.0f, 1.0f));
			modelEsferaMetalica = glm::translate(modelEsferaMetalica, glm::vec3(4.6f, 1.0f, 0.0f));
			posEsferaX = modelEsferaMetalica[3][0];
			posEsferaY = modelEsferaMetalica[3][1];
			posEsferaZ = modelEsferaMetalica[3][2];
		}
		else {
			modelEsferaMetalica = glm::translate(modelEsferaMetalica, glm::vec3(posEsferaX, posEsferaY, posEsferaZ));
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelEsferaMetalica));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		EsferaMetalica_M.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		for (int i = 1; i < 4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();

		}

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
		}


		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		Usamos deltaTime y un temporizador
		*/
		temporizadorNumeros += deltaTime; //acumulamos el tiempo transcurrido

		if (temporizadorNumeros >= tiempoEspera)
		{
			toffsetnumerocambiau += 0.25f; //damos el "salto" en la textura
			if (toffsetnumerocambiau >= 1.0f) {
				toffsetnumerocambiau = 0.0f; // Reiniciamos si se pasa
			}
			temporizadorNumeros = 0.0f; //reiniciamos el temporizador
		}

		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Lógica del temporizador para cambiar de textura completa
		temporizadorTexturas += deltaTime;
		if (temporizadorTexturas >= tiempoEspera)
		{
			usarTextura1 = !usarTextura1; 
			temporizadorTexturas = 0.0f;  //Reinicia el temporizador
		}

		if (usarTextura1) {
			Numero1Texture.UseTexture();
		}
		else {
			Numero2Texture.UseTexture();
		}

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//RENDERIZADO DEL FUEGO
		if (fuegoEncendido)
		{
			float escalaVibracion = 1.5f + (sin(glfwGetTime() * 10.0f) * 0.1f);

			toffset = glm::vec2(0.0f, 0.0f); // Offset fijo porque solo hay una imagen
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, -0.1f, 1.5f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaVibracion, escalaVibracion, escalaVibracion));

			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);

			FuegoTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			meshList[7]->RenderMesh();
		}
		


		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}