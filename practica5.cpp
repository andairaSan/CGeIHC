/*
Práctica 5: Optimización y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Model Goddard_M;
Model Cofre_M;
Model LlantaFI_M;
Model LlantaFD_M;
Model LlantaAD_M;
Model LlantaAI_M;
Model Carro_M;

// Modelos de Goddard
Model Goddard_Cuerpo;
Model Goddard_Mandibula;
Model Goddard_PataED;
Model Goddard_PataEI;
Model Goddard_PataAD;
Model Goddard_PataAI;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";





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

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	Goddard_M = Model();
	Goddard_M.LoadModel("Models/goddard_base.obj");
    Cofre_M = Model();
	Cofre_M.LoadModel("Models/cofre.fbx");
    LlantaFI_M = Model();
	LlantaFI_M.LoadModel("Models/llSuperiorIzq.fbx");
	LlantaFD_M = Model();
	LlantaFD_M.LoadModel("Models/llSuperiorDer.fbx");
	LlantaAD_M = Model();
	LlantaAD_M.LoadModel("Models/llAtrasDer.fbx");
	LlantaAI_M = Model();
	LlantaAI_M.LoadModel("Models/llAtrasIzq.fbx");
	Carro_M = Model();
	Carro_M.LoadModel("Models/CarroCompleto.fbx");
	
	//Goddoard
	Goddard_Cuerpo = Model();
	Goddard_Cuerpo.LoadModel("Models/cuerpo.obj");
	Goddard_Mandibula = Model();
	Goddard_Mandibula.LoadModel("Models/mandibula.obj");
	Goddard_PataED = Model();
	Goddard_PataED.LoadModel("Models/PataEnfrenteDer.obj");
	Goddard_PataEI = Model();
	Goddard_PataEI.LoadModel("Models/PataEnfrenteIzq.obj");
	Goddard_PataAD = Model();
	Goddard_PataAD.LoadModel("Models/PataAtraDer.obj");
	Goddard_PataAI = Model();
	Goddard_PataAI.LoadModel("Models/PataAtraIzq.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	float movCarro = 0.0f;
	float rotLlantas = 0.0f;
	float rotCofre = 0.0f;

	// Variables de control de Goddard
	float movGoddard = 0.0f;
	float marchaGoddard = 0.0f; // Variable que controla la oscilación de la caminata
	float rotMandibula = 0.0f;
	

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

		
		//###########################################################################################################################################
	
		bool* keys = mainWindow.getsKeys();

		// Movimiento de llantas y carro
		if (keys[GLFW_KEY_UP]) {
			movCarro -= 0.2f * deltaTime;     
			rotLlantas -= 5.0f * deltaTime; 
		}
		if (keys[GLFW_KEY_DOWN]) {
			movCarro += 0.2f * deltaTime;     
			rotLlantas += 5.0f * deltaTime;
		}

		// 3. Rotación del cofre para abrir y cerrar
		if (keys[GLFW_KEY_C]) {
			rotCofre += 60.0f * deltaTime;
			if (rotCofre > 60.0f) rotCofre = 60.0f;
		}
		if (keys[GLFW_KEY_V]) {
			rotCofre -= 60.0f * deltaTime;
			if (rotCofre < 0.0f) rotCofre = 0.0f;
		}
		//##########################################################################################################################################
		//Movimiento Goddard - Teclas I y K para avanzar y retroceder
		if (keys[GLFW_KEY_I]) {
			movGoddard -= 0.2f * deltaTime;
			marchaGoddard += 0.1f * deltaTime; 
		}
		if (keys[GLFW_KEY_K]) {
			movGoddard += 0.2f * deltaTime;
			marchaGoddard -= 0.1f * deltaTime;
		}

		// Abrir y cerrar mandíbula - Teclas O y P para abrir y cerrar la mandíbula 
		if (keys[GLFW_KEY_O]) {
			rotMandibula += 60.0f * deltaTime;
			if (rotMandibula > 45.0f) rotMandibula = 45.0f; // Tope para que no gire 360
		}
		if (keys[GLFW_KEY_P]) {
			rotMandibula -= 60.0f * deltaTime;
			if (rotMandibula < 0.0f) rotMandibula = 0.0f;
		}
		//##########################################################################################################################################
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		//------------*INICIA DIBUJO DE NUESTROS DEMÁS OBJETOS-------------------*
		
		//Siguientes modelos
		/* Práctica:
		1.- unir por medio de jerarquía cada llanta al cuerpo del carro
		2.- agregar el mismo valor de rotación a las llantas para que al presionar puedan rotar hacia adelante y hacia atrás.
		3.-el cofre del coche, acomodarlo jerárquicamente y agregar la rotación para poder abrir y cerrar. 
		4.-Agregar traslación con teclado para que pueda avanzar y retroceder de forma independiente
		*/

		// 1. Carro chasis  -  Padre de la jerarquía
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, movCarro));

		color = glm::vec3(0.0f, 0.0f, 0.0f); // Chasis negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_M.RenderModel();

		// 2. Cofre - Hijo del Chasis
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-0.049f, -0.23f, 2.06f));
		modelaux = glm::rotate(modelaux, rotCofre * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		color = glm::vec3(0.0f, 0.0f, 0.5f); // Cofre azul oscuro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Cofre_M.RenderModel();

		// 3. Llantas - Hijas del Chasis
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		// Llanta Delantera Derecha
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(0.6f, -0.16f, 1.23f));// x y z
		modelaux = glm::rotate(modelaux, rotLlantas * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LlantaFD_M.RenderModel();

		// Llanta Delantera Izquierda
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-0.6f, -0.16f, 1.23f)); //x y z
		modelaux = glm::rotate(modelaux, rotLlantas * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LlantaFI_M.RenderModel();

		// Llanta Trasera Derecha 
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(0.67f, -0.16f, -1.15f));
		modelaux = glm::rotate(modelaux, rotLlantas * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LlantaAD_M.RenderModel();

		// Llanta Trasera Izquierda
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-0.67, -0.16f, -1.15f));
		modelaux = glm::rotate(modelaux, rotLlantas * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LlantaAI_M.RenderModel();

		
		// DIBUJO DE GODDARD 

		// Variables de oscilación para la caminata
		float oscilacionPata1 = sin(marchaGoddard) * 30.0f;
		float oscilacionPata2 = sin(marchaGoddard + 3.14159f) * 30.0f;

		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		
		// CUERPO PRINCIPAL Padre
		glm::mat4 modelGoddard = glm::mat4(1.0f);
		modelGoddard = glm::translate(modelGoddard, glm::vec3(3.0f, 4.5f, movGoddard));
		// Render del padre
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelGoddard));
		Goddard_Cuerpo.RenderModel();


		// MANDÍBULA INFERIOR
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = modelGoddard; // 1. REINICIAMOS AL PADRE
		modelaux = glm::translate(modelaux, glm::vec3(-2.6f, 0.8f, 0.1f)); 
		modelaux = glm::rotate(modelaux, rotMandibula * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Goddard_Mandibula.RenderModel();


		
		// PATA ENFRENTE DERECHA
		color = glm::vec3(0.0f, 0.1f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = modelGoddard; // 1. REINICIAMOS AL PADRE
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, -1.0f, 0.9f)); // X(Der), Y(Alto), Z(Frente)
		modelaux = glm::rotate(modelaux, oscilacionPata1 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Goddard_PataAI.RenderModel();


		
		// PATA ENFRENTE IZQUIERDA
		modelaux = modelGoddard; // 1. REINICIAMOS AL PADRE
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, -0.8f, 0.9f)); // X(Izq), Y(Alto), Z(Frente)
		modelaux = glm::rotate(modelaux, oscilacionPata2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Goddard_PataEI.RenderModel();


		// PATA ATRÁS DERECHA
		modelaux = modelGoddard;
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, -1.0f, -0.9f)); // X(Der), Y(Abajo), Z(Atrás)
		modelaux = glm::rotate(modelaux, oscilacionPata2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Goddard_PataAD.RenderModel();


		// PATA ATRÁS IZQUIERDA 
		modelaux = modelGoddard; 
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, -0.6f, -0.8f)); // X(Izq), Y(Abajo), Z(Atrás)
		modelaux = glm::rotate(modelaux, oscilacionPata1 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Goddard_PataED.RenderModel();
		

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
