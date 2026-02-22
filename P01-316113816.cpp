#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <ctime>   // para los colores aleatoriosS
#include <cstdlib>


//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,0.0f,0.0f,1.0f);	 			\n\
}";



//void CrearTriangulo()
//{
//	GLfloat vertices[] = {
//		-1.0f, -1.0f,0.0f,
//		1.0f,-1.0f, 0.0f,
//		0.0f,1.0f,0.0f
//
//	};
//	glGenVertexArrays(1, &VAO); //generar 1 VAO
//	glBindVertexArray(VAO);//asignar VAO
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
//		glEnableVertexAttribArray(0);
//		//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindVertexArray(0);
//
//}

//##############################CREACIÓN DEL TRAPECIO Y ROMBO##########################################################
//void CrearTriangulo()
//{
//	GLfloat vertices[] = {
//
//		// Rombo
//		 -0.6f,  0.5f, 0.0f,   
//		 -0.8f,  0.0f, 0.0f,   
//		 -0.6f, -0.5f, 0.0f,   
//
//		 -0.6f,  0.5f, 0.0f,   
//		 -0.4f,  0.0f, 0.0f,   
//		 -0.6f, -0.5f, 0.0f,   
//
//
//		 // Trapecio
//
//		  0.0f, -0.5f, 0.0f,    
//		  0.2f,  0.2f, 0.0f,    
//		  0.4f, -0.5f, 0.0f,    
//
//		   0.2f,  0.2f, 0.0f,   
//		   0.6f,  0.2f, 0.0f,   
//		   0.4f, -0.5f, 0.0f,   
//
//			0.4f, -0.5f, 0.0f,  
//			0.6f,  0.2f, 0.0f,  
//			0.8f, -0.5f, 0.0f   
//	};
//
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
//FINALIZA CREACIÓN DE TRAPECIO Y ROMBO#################################################################################################

////////////////CREACIÓN DE LAS PRIMERA TRES LETRAS DE MI NOMBRE QUE SON: A O Y S; PRÁCTICA 1  ////////////////////////////////////////


void CrearTriangulo() {
	GLfloat vertices[] = {
		// A Para que quede del lado izquierdo va de -0.9 a -0.4
		-0.9f, -0.7f, 0.0f,  -0.75f, -0.7f, 0.0f,  -0.75f,  0.7f, 0.0f, //Manita izquierda
		-0.9f, -0.7f, 0.0f,  -0.75f,  0.7f, 0.0f,  -0.9f,   0.7f, 0.0f,
		-0.55f, -0.7f, 0.0f, -0.4f,  -0.7f, 0.0f,  -0.4f,   0.7f, 0.0f, //Manitaa Derecha
		-0.55f, -0.7f, 0.0f, -0.4f,   0.7f, 0.0f,  -0.55f,  0.7f, 0.0f,
		-0.75f,  0.7f, 0.0f, -0.55f,  0.7f, 0.0f,  -0.55f,  0.5f, 0.0f, //techo
		-0.75f,  0.7f, 0.0f, -0.55f,  0.5f, 0.0f,  -0.75f,  0.5f, 0.0f,
		-0.75f,  0.1f, 0.0f, -0.55f,  0.1f, 0.0f,  -0.55f, -0.1f, 0.0f, //Barra de en medio
		-0.75f,  0.1f, 0.0f, -0.55f, -0.1f, 0.0f,  -0.75f, -0.1f, 0.0f,

		// O para que quede en el centro es de -0.25  a 0.25
		-0.25f, -0.7f, 0.0f, -0.1f,  -0.7f, 0.0f,  -0.1f,   0.7f, 0.0f, //barra izquierda
		-0.25f, -0.7f, 0.0f, -0.1f,   0.7f, 0.0f,  -0.25f,  0.7f, 0.0f,
		 0.1f,  -0.7f, 0.0f,  0.25f, -0.7f, 0.0f,   0.25f,  0.7f, 0.0f, //barra derecha
		 0.1f,  -0.7f, 0.0f,  0.25f,  0.7f, 0.0f,   0.1f,   0.7f, 0.0f,
		-0.1f,   0.7f, 0.0f,  0.1f,   0.7f, 0.0f,   0.1f,   0.5f, 0.0f, //arriba
		-0.1f,   0.7f, 0.0f,  0.1f,   0.5f, 0.0f,  -0.1f,   0.5f, 0.0f,
		-0.1f,  -0.5f, 0.0f,  0.1f,  -0.5f, 0.0f,   0.1f,  -0.7f, 0.0f, //abajo
		-0.1f,  -0.5f, 0.0f,  0.1f,  -0.7f, 0.0f,  -0.1f,  -0.7f, 0.0f,

		// S para que quede en la derecha va de 0.4 a 0.9
		 0.4f,   0.7f, 0.0f,  0.9f,   0.7f, 0.0f,   0.9f,   0.5f, 0.0f, //arriba
		 0.4f,   0.7f, 0.0f,  0.9f,   0.5f, 0.0f,   0.4f,   0.5f, 0.0f,
		 0.4f,   0.5f, 0.0f,  0.55f,  0.5f, 0.0f,   0.55f,  0.1f, 0.0f, //vertical izquiera (cabeza)
		 0.4f,   0.5f, 0.0f,  0.55f,  0.1f, 0.0f,   0.4f,   0.1f, 0.0f,
		 0.4f,   0.1f, 0.0f,  0.9f,   0.1f, 0.0f,   0.9f,  -0.1f, 0.0f, //En medio
		 0.4f,   0.1f, 0.0f,  0.9f,  -0.1f, 0.0f,   0.4f,  -0.1f, 0.0f,
		 0.75f, -0.1f, 0.0f,  0.9f,  -0.1f, 0.0f,   0.9f,  -0.5f, 0.0f, //pancita 
		 0.75f, -0.1f, 0.0f,  0.9f,  -0.5f, 0.0f,   0.75f, -0.5f, 0.0f,
		 0.4f,  -0.5f, 0.0f,  0.9f,  -0.5f, 0.0f,   0.9f,  -0.7f, 0.0f, //piso
		 0.4f,  -0.5f, 0.0f,  0.9f,  -0.7f, 0.0f,   0.4f,  -0.7f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}


void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader,1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n",shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader= glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	 //verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}
int main()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

 //Llamada a las funciones creadas antes del main
	CrearTriangulo();
	CompileShaders();


	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		
		//Recibir eventos del usuario
		glfwPollEvents();

		////Limpiar la ventana
		//glClearColor(0.0f,0.0f,0.0f,1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		// 
		//MODIFICACIÓN TAREA EJERCICIO DE CLASE 1
		//Limpiar la ventana con cambio cíclico entre rojo, verde y azul
		


		float tiempo = glfwGetTime();
		static int ultimoSegundo = -1; // 'static' para que el valor persista entre ciclos
		int segundoActual = (int)tiempo;

		if (segundoActual != ultimoSegundo) {
			int colorAzar = rand() % 3; // de forma aleatoria se asigna un número entre 0 y 2 para elegir el color

			if (colorAzar == 0)      glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
			else if (colorAzar == 1) glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // green
			else                     glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // blueee

			ultimoSegundo = segundoActual;
		}

		glClear(GL_COLOR_BUFFER_BIT);



		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,78);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}