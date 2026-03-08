// Práctica 3: Modelado Geométrico y Cámara Sintética - PYRAMINX RUBIK 
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

// GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

// Clases de tu proyecto
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;


static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

void CrearTrianguloPlano()
{
	unsigned int indices[] = { 0, 1, 2 };
	GLfloat vertices[] = {
		 0.0f,  0.57735f, 0.0f, // punta
		-0.5f, -0.28867f, 0.0f, // izquierda
		 0.5f, -0.28867f, 0.0f  // derecha
	};
	Mesh* plano = new Mesh();
	plano->CreateMesh(vertices, indices, 9, 3);
	meshList.push_back(plano);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CrearTrianguloPlano(); // indice 0
	CreateShaders();

	// Cámara
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;   // Con esto modificamos la cámara
	GLuint uniformColor = 0;  // Con esto modificamos el color del objeto dibujado

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//PYRAMINX 
	glm::vec3 colores[] = {
		glm::vec3(0.8f, 0.1f, 0.1f), // Cara 0, rojo
		glm::vec3(0.1f, 0.8f, 0.1f), // Cara 1, verde
		glm::vec3(0.1f, 0.2f, 0.9f), // Cara 2, azul
		glm::vec3(0.9f, 0.9f, 0.1f)  // Cara 3, amarillo
	};

	//definimos  4 vértices 
	float R = 3.0f;
	glm::vec3 v0(0.0f, 0.61237f * R, 0.0f);
	glm::vec3 v1(0.0f, -0.20412f * R, 0.57735f * R);
	glm::vec3 v2(0.5f * R, -0.20412f * R, -0.28867f * R);
	glm::vec3 v3(-0.5f * R, -0.20412f * R, -0.28867f * R);

	//agrupamos los vértices para formar las 4 caras 
	glm::vec3 caras[4][3] = {
		{ v1, v3, v2 },               
		{ v0, v1, v2 },                 
		{ v0, v2, v3 },            
		{ v0, v3, v1 }           
	};

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();

		// Tu cámara controlada con WASD y el mouse
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Fondo gris oscuro para notar las divisiones negras
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//matriz de posción y rotación base del piraminx, a partir de la cual se calculan las transformaciones de cada cara
		glm::mat4 baseTransform(1.0f);

		
		baseTransform = glm::translate(baseTransform, glm::vec3(0.0f, -0.5f, -5.0f));

		
		baseTransform = glm::rotate(baseTransform, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		baseTransform = glm::rotate(baseTransform, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		baseTransform = glm::rotate(baseTransform, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		
		for (int c = 0; c < 4; c++)
		{
			
			glm::vec3 A = caras[c][0];
			glm::vec3 B = caras[c][1];
			glm::vec3 C = caras[c][2];

			//ejes X, Y, Z locales de esta cara mediante producto cruz
			glm::vec3 X_loc = glm::normalize(C - B);
			glm::vec3 Y_loc = glm::normalize(A - (B + C) * 0.5f);
			glm::vec3 Z_loc = glm::normalize(glm::cross(X_loc, Y_loc));
			glm::vec3 CentroCara = (A + B + C) / 3.0f;

			
			glm::mat4 rot(1.0f);
			rot[0] = glm::vec4(X_loc, 0.0f);
			rot[1] = glm::vec4(Y_loc, 0.0f);
			rot[2] = glm::vec4(Z_loc, 0.0f);

			
			glm::mat4 modelCara = baseTransform * glm::translate(glm::mat4(1.0f), CentroCara) * rot;

		
			//fondo negro 
			glm::mat4 mBlack = glm::translate(modelCara, glm::vec3(0.0f, 0.0f, -0.01f));
			mBlack = glm::scale(mBlack, glm::vec3(3.05f));

			glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mBlack));
			meshList[0]->RenderMeshGeometry();

			//Insertar las 9 estampas de color encima
			glUniform3fv(uniformColor, 1, glm::value_ptr(colores[c]));
			float gap = 0.94f; //al 94% para ver las líneas negras de separación

			for (int fila = 0; fila < 3; fila++) {

				// triángulos normales 
				int numUp = 3 - fila;
				for (int i = 0; i < numUp; i++) {
					glm::mat4 m = modelCara;
					float x = (i - numUp * 0.5f + 0.5f) * 1.0f;
					float y = (fila - 1.0f) * 0.866f + 0.2886f;

					m = glm::translate(m, glm::vec3(x, y, 0.0f));
					m = glm::scale(m, glm::vec3(gap));

					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
					meshList[0]->RenderMeshGeometry();
				}

				// triángulos invertidos 
				if (fila < 2) {
					int numDown = 2 - fila;
					for (int i = 0; i < numDown; i++) {
						glm::mat4 m = modelCara;
						float x = (i - numDown * 0.5f + 0.5f) * 1.0f;
						float y = (fila - 1.0f) * 0.866f + 0.5773f;

						m = glm::translate(m, glm::vec3(x, y, 0.0f));
						m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
						m = glm::scale(m, glm::vec3(gap));

						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
						meshList[0]->RenderMeshGeometry();
					}
				}
			}
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}