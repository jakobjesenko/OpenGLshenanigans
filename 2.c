#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Utils.h"
#define WINDOW_TITLE_PREFIX "Chapter 2"

#define RED {1.0f, 0.0f, 0.0f, 1.0f}
#define GREEN {0.0f, 1.0f, 0.0f, 1.0f}
#define BLUE {0.0f, 0.0f, 1.0f, 1.0f}
#define YELLOW {1.0f, 1.0f, 0.0f, 1.0f}
#define MAGENTA {1.0f, 0.0f, 1.0f, 1.0f}
#define CYAN {0.0f, 1.0f, 1.0f, 1.0f}
#define WHITE {1.0f, 1.0f, 1.0f, 1.0f}
#define BLACK {0.0f, 0.0f, 0.0f, 1.0f}

int CurrentWidth = 800;
int CurrentHeight = 600;
int WindowHandle = 0;
unsigned int FrameCount = 0;

float CubeRotation = 0;
clock_t LastTime = 0;
float CubeSpeed = 1.0f;

GLuint
	ProjectionMatrixUniformLocation,
	ViewMatrixUniformLocation,
	ModelMatrixUniformLocation,
	BufferIds[3] = { 0 },
	ShaderIds[3] = { 0 };

Matrix
	ProjectionMatrix,
	ViewMatrix,
	ModelMatrix;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);

void CreateCube(void);
void DestroyCube(void);
void DrawCube(void);
void mouse(int, int, int, int);

int main(int argc, char* argv[]){
	Initialize(argc, argv);

	glutMainLoop();
	
	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[]){
	GLenum GlewInitResult;

	InitWindow(argc, argv);

	GlewInitResult = glewInit();

	if (GlewInitResult != GLEW_OK){
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);


	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	ExitOnGLError("ERROR: Could not set OpenGL culling options");

	ModelMatrix = IDENTITY_MATRIX;
	ProjectionMatrix = IDENTITY_MATRIX;
	ViewMatrix = IDENTITY_MATRIX;
	TranslateMatrix(&ViewMatrix, 0, 0, -2);

	CreateCube();
}

void InitWindow(int argc, char* argv[]){
	glutInit(&argc, argv);
	
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (WindowHandle < 1){
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0);
	glutCloseFunc(DestroyCube);
	glutMouseFunc(mouse);
}

void ResizeFunction(int Width, int Height){
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);

	ProjectionMatrix =
	CreateProjectionMatrix(
		60,
		(float) CurrentWidth / CurrentHeight,
		1.0f,
		100.0f
	); 

	glUseProgram(ShaderIds[0]);
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.m);
	glUseProgram(0);
}

void RenderFunction(void){
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawCube();

	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleFunction(void){
	glutPostRedisplay();
}

void TimerFunction(int Value){
	if (0 != Value) {
		char* TempString = (char*)
		malloc(512 + strlen(WINDOW_TITLE_PREFIX));
		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
		);
		glutSetWindowTitle(TempString);
		free(TempString);
	}
	FrameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}

void CreateCube(void){
	const Vertex VERTICES[8] = {
		{{ -.5f, -.5f,  .5f, 1 }, BLUE},
		{{ -.5f,  .5f,  .5f, 1 }, WHITE},
		{{  .5f,  .5f,  .5f, 1 }, GREEN},
		{{  .5f, -.5f,  .5f, 1 }, CYAN},
		{{ -.5f, -.5f, -.5f, 1 }, MAGENTA},
		{{ -.5f,  .5f, -.5f, 1 }, RED},
		{{  .5f,  .5f, -.5f, 1 }, YELLOW},
		{{  .5f, -.5f, -.5f, 1 }, WHITE}
	};
	const GLuint INDICES[36] = {
		0,2,1,  0,3,2,
		4,3,0,  4,7,3,
		4,1,5,  4,0,1,
		3,6,2,  3,7,6,
		1,6,5,  1,2,6,
		7,5,6,  7,4,5
	};
	ShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");

	ShaderIds[1] = LoadShader("SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
	ShaderIds[2] = LoadShader("SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
	glAttachShader(ShaderIds[0], ShaderIds[1]);
	glAttachShader(ShaderIds[0], ShaderIds[2]);

	glLinkProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
	ExitOnGLError("ERROR: Could not get the shader uniform locations");

	glGenBuffers(2, &BufferIds[1]);
	ExitOnGLError("ERROR: Could not generate the buffer objects");

	glGenVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glBindVertexArray(0);
}

void DestroyCube(void){
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void DrawCube(void){
	float CubeAngle;
	clock_t Now = clock();
	if (LastTime == 0){
		LastTime = Now;
	}
	CubeRotation += CubeSpeed * 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
	CubeAngle = DegreesToRadians(CubeRotation);
	LastTime = Now;

	ModelMatrix = IDENTITY_MATRIX;
	RotateAboutY(&ModelMatrix, CubeAngle);
	RotateAboutX(&ModelMatrix, CubeAngle);

	glUseProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not use the shader program");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
	ExitOnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
	ExitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);
}

void mouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		CubeSpeed = 0.0f;
	} else {
		CubeSpeed = 1.0f;
	}
}