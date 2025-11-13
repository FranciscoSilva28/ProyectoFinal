/*---------------------------------------------------------*/
/* ----------------   PROYECTO FINAL  --------------------------*/
/*-----------------    2026-1   ---------------------------*/
/*------------- Alumnos: Arellanes Conde Esteban----------*/
/*-------------			Cervantes Valencia Mar ??a Fernanda----*/
/*-------------			Silva Castro Francisco Javier*/
/*-------------			Rufino López María Elena---------------*/
/*------------- No. Cuenta                  ---------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(40.0f, 10.0f, 3.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
//Lineas Agregadas para Texturas Elena
unsigned int
t_obra1, t_obra2, t_obra3, t_obra4, t_obra5, t_obra6, t_obra7, t_obra8, 
t_obra9, t_obra10, t_obra11, t_obra12, t_obra13, t_obra14, t_obra15, t_obra16,
t_obra17, t_obra18, t_obra19, t_obra20, t_obra21, t_obra22, t_obra23, t_obra24, 
t_obra25, t_obra26, t_obra27, t_obra28, t_obra29, t_obra30, t_obra31, t_obra32, 
t_obra33, t_obra34, t_obra35, t_obra36, t_ladrillos;


//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// Variables para el movimiento en zig-zag
float movEscultura2_x = 50.0f;  // Posición inicial en X
float movEscultura2_z = 50.0f;   // Posición inicial en Z
int recorrido = 1;               // Controla el patrón de movimiento


bool animacion = false; // Controla si la animación está activa o no

// Variables para el modelo obra_Elena
float rotOsoY = 0.0f;
int stateOso = 0;
bool animOso = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f;
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotRodIzqInc = 0.0f,
giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	if (isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}


//texturas de cuadros Elena
void LoadTextures()
{
	t_obra1 = generateTextures("Texturas/obra1.jpg", 0, true); t_obra2 = generateTextures("Texturas/obra2.jpg", 0, true);
	t_obra3 = generateTextures("Texturas/obra3.jpg", 0, true); t_obra4 = generateTextures("Texturas/obra4.jpg", 0, true);
	t_obra5 = generateTextures("Texturas/obra5.jpg", 0, true); t_obra6 = generateTextures("Texturas/obra6.jpg", 0, true);
	t_obra7 = generateTextures("Texturas/obra7.jpg", 0, true); t_obra8 = generateTextures("Texturas/obra8.jpg", 0, true);
	t_obra9 = generateTextures("Texturas/obra9.jpg", 0, true); t_obra10 = generateTextures("Texturas/obra10.jpg", 0, true);
	t_obra11 = generateTextures("Texturas/obra11.jpg", 0, true); t_obra12 = generateTextures("Texturas/obra12.jpg", 0, true);
	t_obra13 = generateTextures("Texturas/obra13.jpg", 0, true); t_obra14 = generateTextures("Texturas/obra14.jpg", 0, true);
	t_obra15 = generateTextures("Texturas/obra15.jpg", 0, true); 
	//bicicleta
	t_obra16 = generateTextures("Texturas/obra_bici1.jpg", 0, true); t_obra17 = generateTextures("Texturas/obra_bici2.jpg", 0, true);
	t_obra18 = generateTextures("Texturas/obra_bici3.jpg", 0, true); t_obra19 = generateTextures("Texturas/obra_bici4.jpg", 0, true); 
	t_obra20 = generateTextures("Texturas/obra_bici5.jpg", 0, true); 

	
	//cefetera
	t_obra21 = generateTextures("Texturas/obra_cafet1.jpg", 0, true); t_obra22 = generateTextures("Texturas/obra_cafet2.jpg", 0, true); 
	t_obra23 = generateTextures("Texturas/obra_cafet3.jpg", 0, true); t_obra24 = generateTextures("Texturas/obra_cafet4.jpg", 0, true); 
	t_obra25 = generateTextures("Texturas/obra_cafet5.jpg", 0, true); t_obra26 = generateTextures("Texturas/obra_cafet6.jpg", 0, true); 
	t_obra27 = generateTextures("Texturas/obra_cafet7.jpg", 0, true); t_obra28 = generateTextures("Texturas/obra_cafet8.jpg", 0, true); 
	t_obra29 = generateTextures("Texturas/obra_cafet9.jpg", 0, true); t_obra30 = generateTextures("Texturas/obra_cafet10.jpg", 0, true); 
	//pajaro y gatos
	t_obra31 = generateTextures("Texturas/obra_gato1.jpg", 0, true); t_obra32 = generateTextures("Texturas/obra_gato2.jpg", 0, true); 
	t_obra33 = generateTextures("Texturas/obra_gato3.jpg", 0, true); t_obra34 = generateTextures("Texturas/obra_gato4.jpg", 0, true); 
	t_obra35 = generateTextures("Texturas/obra_gato5.jpg", 0, true); t_obra36 = generateTextures("Texturas/obra_gato6.jpg", 0, true);

	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);  

}


void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}
	//----- Animacion oso
	if (animOso) //Elena
	{
		rotOsoY += 1.5f;          // rota sobre eje X
		if (rotOsoY >= 360.0f)    // cuando completa la vuelta
			rotOsoY = 0.0f;
	}
	else if (stateOso == 0)
	{
		rotOsoY = rotOsoY;
	}
	//recorrido en zig-zag

// Escultura2 Elena
	if (animacion)
	{
		if (recorrido == 1)
		{
			movEscultura2_x += 1.0f;
			movEscultura2_z += 1.5f;
			if (movEscultura2_x >= 60.0f) {
				recorrido = 2;
			}
		}
		if (recorrido == 2)
		{
			movEscultura2_x -= 1.0f;
			movEscultura2_z -= 1.5f;
			if (movEscultura2_x <= 60.0f) {
				recorrido = 3;
			}
		}
		if (recorrido == 3)
		{
			movEscultura2_x += 3.0f;
			movEscultura2_z += 4.5f;
			if (movEscultura2_x >= 120.0f) {
				recorrido = 4;
			}
		}
		if (recorrido == 4)
		{
			movEscultura2_x -= 3.0f;
			movEscultura2_z -= 4.5f;
			if (movEscultura2_x <= 120.0f) {
				recorrido = 1;
			}
		}
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Definición de la estructura para los cuadros Elena
struct Cuadro {
	glm::vec3 posicion;
	glm::vec3 escala;
	float rotacionY;
	unsigned int textura;
};


int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2026-1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	// Definición de los cuadros a mostrar en la escena Elena
	Cuadro cuadros[] = {
		{glm::vec3(0.0f, 10.0f,  0.0f), glm::vec3(6.5f),45.0f, t_obra1 }, {glm::vec3(5.0f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra2 },
		{glm::vec3(-5.0f, 10.0f, 0.0f), glm::vec3(4.5f),-30.0f,t_obra3 }, {glm::vec3(15.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra4 },
		{glm::vec3(-15.f, 10.0f, 0.0f), glm::vec3(8.5f), 0.0f, t_obra5},  {glm::vec3(25.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra6},
		{glm::vec3(-25.f, 10.0f, 0.0f), glm::vec3(1.5f),0.0f,  t_obra7},  {glm::vec3(35.f, 10.0f, 0.0f),  glm::vec3(4.5f), 0.0f, t_obra8},
		{glm::vec3(-35.f, 10.0f, 0.0f), glm::vec3(3.5f),0.0f,  t_obra9},  {glm::vec3(45.f, 10.0f, 0.0f),  glm::vec3(4.5f), 0.0f, t_obra10},
		{glm::vec3(-45.f, 10.0f, 0.0f), glm::vec3(2.5f),0.0f,  t_obra11}, {glm::vec3(55.f, 10.0f, 0.0f),  glm::vec3(4.5f), 0.0f, t_obra12},
		{glm::vec3(-55.f, 10.0f, 0.0f), glm::vec3(9.5f),0.0f,  t_obra13}, {glm::vec3(65.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra14},
		{glm::vec3(-65.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra15}, {glm::vec3(75.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra16},
		{glm::vec3(-75.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra17}, {glm::vec3(85.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra18},
		{glm::vec3(-85.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra19}, {glm::vec3(95.f, 10.0f, -20.0f), glm::vec3(4.5f), 0.0f, t_obra20},
		{glm::vec3(-95.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra21}, {glm::vec3(105.f, 10.0f, 10.0f), glm::vec3(4.5f), 0.0f, t_obra22},
		{glm::vec3(-105.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra23}, {glm::vec3(115.f, 10.0f, 10.0f), glm::vec3(4.5f), 0.0f, t_obra24},
		{glm::vec3(-115.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra25}, {glm::vec3(125.f, 10.0f, 10.0f), glm::vec3(4.5f), 0.0f, t_obra26},
		{glm::vec3(-125.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra27}, {glm::vec3(135.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra28},
		{glm::vec3(-135.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra29}, {glm::vec3(145.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra30},
		{glm::vec3(-145.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra31}, {glm::vec3(155.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra32},
		{glm::vec3(-155.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra33}, {glm::vec3(165.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra34},
		{glm::vec3(-165.f, 10.0f, 0.0f), glm::vec3(4.5f),0.0f,  t_obra35}, {glm::vec3(175.f, 10.0f, 0.0f), glm::vec3(4.5f), 0.0f, t_obra36},

	};


	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	stbi_set_flip_vertically_on_load(false); //IMPORTANTE PARA NO VOLTEAR LAS TEXTURAS DEL SKYBOX FONDO Elena
	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model escultura3_Elena("resources/objects/escultura3/oso.obj");
	Model escultura1_Elena("resources/objects/escultura1/pajaro.obj");
	Model base_Elena("resources/objects/base/base.obj");
	ModelAnim escultura2_Elena("resources/objects/escultura2/escultura2.dae");
	escultura2_Elena.initShaders(animShader.ID);

	//Model cubo("resources/objects/cubo/cube02.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// Cuadros con texturas Elena
		myShader.use();
		glActiveTexture(GL_TEXTURE0);
		myShader.setInt("texture1", 0);

		glBindVertexArray(VAO[0]);

		for (auto& c : cuadros) {
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, c.posicion);
			modelOp = glm::rotate(modelOp, glm::radians(c.rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, c.escala);

			myShader.setMat4("model", modelOp);
			myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, c.textura);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}



		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		//Escultura2 Elena
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movEscultura2_x, 0.0f, movEscultura2_z));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f)); // Escalado del modelo
		animShader.setMat4("model", modelOp);
		escultura2_Elena.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar código aquí
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



		glBindVertexArray(0);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		//piso.Draw(staticShader);


		//-------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------

		//Dibujar esculturas Elena
		glm::mat4 tmpEsc;
		glBindTexture(GL_TEXTURE_2D, 0);
		staticShader.use();
		staticShader.setVec3("material.diffuse", glm::vec3(0.55f, 0.55f, 0.55f));
		staticShader.setVec3("material.specular", glm::vec3(0.1f, 0.1f, 0.1f));   // poco brillo
		staticShader.setFloat("material.shininess", 32.0f);
		modelOp = glm::mat4(1.0f);
		modelOp = glm::translate(modelOp, glm::vec3(-30.0f, 0.0f, 50.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		tmpEsc = modelOp;
		staticShader.setMat4("model", tmpEsc);
		base_Elena.Draw(staticShader);

		modelOp = glm::translate(tmpEsc, glm::vec3(1.5f, 32.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.5f));
		staticShader.setMat4("model", modelOp);
		escultura1_Elena.Draw(staticShader);



		//Escultura 3 OSO Elena
		modelOp = glm::mat4(1.0f);
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 10.0f, 40.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(20.0f, 3.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(rotOsoY), glm::vec3(0.0f, .0f, 1.0f)); // ROTACIÓN EN Y
		staticShader.setMat4("model", modelOp);
		escultura3_Elena.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------





		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);






		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		stateOso = 0; //Elena
		animOso = false;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		animOso = true; //Elena
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Elena RATON 
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}