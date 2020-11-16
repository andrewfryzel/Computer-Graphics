#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

float getVertex(float a, float b);
void initGL();
void colorCycle();

float getCrossProduct(float a, float b, float c, float d);

#define M_PI 3.141592654f

unsigned int g_windowWidth = 800;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW3-3D-Basics";

GLFWwindow* g_window;

// Model data
std::vector<float> g_meshVertices;
std::vector<float> g_meshNormals;
std::vector<unsigned int> g_meshIndices;
GLfloat g_modelViewMatrix[16];

// Default options
bool enablePersp = true;
bool teapotSpin = false;
bool enableDolly = false;
bool showCheckerboard = false;

// Rotation Options
double time = 0.0;
double pause = 0.0;
double start = 0.0;
double end = 0.0;

// Dolly zoom options 
float fov = M_PI / 4.f;
float distance = 4.5f;
float halfWidth = distance * std::tan(fov / 2);
float fovStart = 0.0;
float fovEnd = 0.0;
float fovPause = 0.0;
int i = 0;
bool disable = false;

// Auxiliary math functions
float dotProduct(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void crossProduct(const float* a, const float* b, float* r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

float radianToDegree(float angleInRadian) {
	return angleInRadian * 180.f / M_PI;
}

void normalize(float* a)
{
	const float len = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] /= len;
	a[1] /= len;
	a[2] /= len;
}

void computeNormals()
{
	g_meshNormals.resize(g_meshVertices.size());

	// TASK 1
	// The code below sets all normals to point in the z-axis, so we get a boring constant gray color
	// The following should be replaced with your code for normal computation

	// had to change this to indices, wasnt filling in the middle section of the teapot
	for (int v = 0; v < g_meshIndices.size() / 3; ++v)
	{

		//replace thise
		//g_meshNormals[3 * v + 2] = 1.0;
		int indice1 = g_meshIndices[3 * v];
		int indice2 = g_meshIndices[3 * v + 1];
		int indice3 = g_meshIndices[3 * v + 2];

		//get vertices using indices
		float ux = getVertex(g_meshVertices[3 * indice1], g_meshVertices[3 * indice2]);
		float uy = getVertex(g_meshVertices[3 * indice1 + 1], g_meshVertices[3 * indice2 + 1]);
		float uz = getVertex(g_meshVertices[3 * indice1 + 2], g_meshVertices[3 * indice2 + 2]);

		float vx = getVertex(g_meshVertices[3 * indice1], g_meshVertices[3 * indice3]);
		float vy = getVertex(g_meshVertices[3 * indice1 + 1], g_meshVertices[3 * indice3 + 1]);
		float vz = getVertex(g_meshVertices[3 * indice1 + 2], g_meshVertices[3 * indice3 + 2]);

		// get cross product using vertices
		float crossX = getCrossProduct(uy, vz, uz, vy);
		float crossY = getCrossProduct(uz, vx, ux, vz);
		float crossZ = getCrossProduct(ux, vy, uy, vx);

		// normalize using cross product and length
		// divide by the sqrt of the product of the length of the vectors
		float total = (crossX * crossX) + (crossY * crossY) + (crossZ * crossZ);
		float length = sqrt(total);

		float normalX = crossX / length;
		float normalY = crossY / length;
		float normalZ = crossZ / length;

		//do what I commented out before, add each normalized value

		g_meshNormals[3 * indice1] += normalX;
		g_meshNormals[3 * indice1 + 1] += normalY;
		g_meshNormals[3 * indice1 + 2] += normalZ;

		g_meshNormals[3 * indice2] += normalX;
		g_meshNormals[3 * indice2 + 1] += normalY;
		g_meshNormals[3 * indice2 + 2] += normalZ;

		g_meshNormals[3 * indice3] += normalX;
		g_meshNormals[3 * indice3 + 1] += normalY;
		g_meshNormals[3 * indice3 + 2] += normalZ;

		/*
		g_meshNormals[3 * v] = g_meshNormals[3 * v] / 6;
		g_meshNormals[3 * v + 1] = g_meshNormals[3 * v + 1] / 6;
		g_meshNormals[3 * v + 2] = g_meshNormals[3 * v + 2] / 6;
		// moved this outside, have to go through meshnormals not meshindices, putting in this for loop causing it to take eternity to load
		*/


	}

	for (int w = 0; w < g_meshNormals.size() / 3; ++w) {
		g_meshNormals[3 * w] = g_meshNormals[3 * w] / 6;
		g_meshNormals[3 * w + 1] = g_meshNormals[3 * w + 1] / 6;
		g_meshNormals[3 * w + 2] = g_meshNormals[3 * w + 2] / 6;
	}
}

float getVertex(float a, float b)
{

	return b - a;

}

float getCrossProduct(float a, float b, float c, float d)
{

	return (a * b) - (c * d);

}

void loadObj(std::string p_path)
{
	std::ifstream nfile;
	nfile.open(p_path);
	std::string s;

	while (nfile >> s)
	{
		if (s.compare("v") == 0)
		{
			float x, y, z;
			nfile >> x >> y >> z;
			g_meshVertices.push_back(x);
			g_meshVertices.push_back(y);
			g_meshVertices.push_back(z);
		}
		else if (s.compare("f") == 0)
		{
			std::string sa, sb, sc;
			unsigned int a, b, c;
			nfile >> sa >> sb >> sc;

			a = std::stoi(sa);
			b = std::stoi(sb);
			c = std::stoi(sc);

			g_meshIndices.push_back(a - 1);
			g_meshIndices.push_back(b - 1);
			g_meshIndices.push_back(c - 1);
		}
		else
		{
			std::getline(nfile, s);
		}
	}

	computeNormals();

	std::cout << p_path << " loaded. Vertices: " << g_meshVertices.size() / 3 << " Triangles: " << g_meshIndices.size() / 3 << std::endl;
}

double getTime()
{
	return glfwGetTime();
}

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void togglePerspective() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double timer = getTime();
	// Perspective Projection
	if (enablePersp)
	{
		// Dolly zoom computation
		if (enableDolly) {
			// TASK 3
			// Your code for dolly zoom computation goes here
			// You can use getTime() to change fov over time
			// distance should be recalculated here using the Equation 2 in the description file

			fov = abs(cos(timer - fovPause));
			fovStart = timer;
			distance = halfWidth / tan(fov / 2);
		}
		else
		{
			fovEnd = getTime();
			fovPause = fovEnd - fovStart;
		}

		float fovInDegree = radianToDegree(fov);
		gluPerspective(fovInDegree, (GLfloat)g_windowWidth / (GLfloat)g_windowHeight, 1.0f, 40.f);
	}
	// Othogonal Projection
	else
	{
		// Scale down the object for a better view in orthographic projection
		glScalef(0.5, 0.5, 0.5);

		// TASK 3
		// Your code for orthogonal projection goes here
		// (Hint: you can use glOrtho() function in OpenGL)
		//https://lmb.informatik.uni-freiburg.de/people/reisert/opengl/doc/glOrtho.html#targetText=DESCRIPTION,%2C%200%2C%200).%20%2D
		//glOrtho(-1.f, 1.f, -1.f * (g_windowHeight), 1.f * (g_windowWidth), 1.f, 40.f);
		//glOrtho(-1.f, 1.f, -1.f , 1.f , 1.f, 40.f);
		glOrtho(-1.f, 1.f, -1.f * (float)(g_windowHeight) / (g_windowWidth), 1.f * (float)(g_windowHeight) / (g_windowWidth), 1.f, 40.f);

	}
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
	if (p_key == GLFW_KEY_P && p_action == GLFW_PRESS) {

		// Perspective Projection
		enablePersp = true;
		togglePerspective();
		std::cout << "Perspective activated\n";

	}
	if (p_key == GLFW_KEY_O && p_action == GLFW_PRESS) {

		// Orthographic Projection
		enablePersp = false;
		togglePerspective();
		std::cout << "Orthogonal activated\n";

	}
	if (p_key == GLFW_KEY_S && p_action == GLFW_PRESS) {

		// Toggle Spinning
		if (!teapotSpin) {
			std::cout << "Teapot spinning on\n";
		}
		else {
			std::cout << "Teapot spinning off\n";
		}
		teapotSpin = !teapotSpin;
	}
	if (p_key == GLFW_KEY_D && p_action == GLFW_PRESS) {

		// Toggle dolly zoom
		if (!enableDolly)
		{
			std::cout << "Dolly zoom on\n";
		}
		else {
			std::cout << "Dolly zoom off\n";
		}
		enableDolly = !enableDolly;
	}
	if (p_key == GLFW_KEY_C && p_action == GLFW_PRESS) {



		// Show/hide Checkerboard
		if (!showCheckerboard)
		{

			std::cout << "Show checkerboard\n";

			glDisable(GL_COLOR_MATERIAL);
			disable = true;

		}
		else {
			disable = false;
			std::cout << "Hide checkerboard\n";
		}

		showCheckerboard = !showCheckerboard;
	}

	if (p_key == GLFW_KEY_Y && p_action == GLFW_PRESS) {
		glClearColor(1.f, 1.f, 1.f, 1.0f);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_FLAT);

	}

	if (p_key == GLFW_KEY_U && p_action == GLFW_PRESS) {
		glClearColor(1.f, 1.f, 1.f, 1.0f);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
	}

	if (p_key == GLFW_KEY_Z && p_action == GLFW_PRESS) {
		if (disable == true) {
			;
		}
		else
			colorCycle();
	}

}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void colorCycle()
{
	i++;
	if (i == 1) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.5f, 0.0f, 1.0f);
	}

	if (i == 2) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.9f, 0.91f, 0.98f);
	}

	if (i == 3) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.89f, 0.47f, 0.2f);
	}

	if (i == 4) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.72f, 0.45f, 0.2f);
	}

	if (i == 5) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.22f, 0.69f, 0.87f);
	}

	if (i == 6) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.137255f, 0.419608f, 0.556863f);
	}

	if (i == 7) {
		glEnable(GL_COLOR_MATERIAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(0.752941f, 0.752941f, 0.752941f);
		i = 0;
	}



}




void printHotKeys() {
	std::cout << "\nHot Keys..\n"
		<< "Orthogonal Projection:  O\n"
		<< "Perspective Projection: P\n"
		<< "Toggle Spinning:        S\n"
		<< "Toggle Dolly Zoom:      D\n"
		<< "Show/hide Checkerboard: C\n"
		<< "Change Color:		Z\n"
		<< "Show GL_FLAT:		Y\n"
		<< "Show GL_SMOOTH:		U\n"
		<< "Exit:                   Esc\n\n";
}

void clearModelViewMatrix()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			g_modelViewMatrix[4 * i + j] = 0.0f;
		}
	}
}

void updateModelViewMatrix()
{
	clearModelViewMatrix();

	// TASK 2
	// The following code sets a static modelView matrix
	// This should be replaced with code implementing Task 2
	// You can use getTime() to change rotation over time
	if (teapotSpin) {
		//sin + or - determines clockwise or counter clockwise rotations
		//need to save the angle of where it is when paused, so that it doesnt skip around
		time = getTime() - pause;
		start = time;
		g_modelViewMatrix[0] = cos(time);
		g_modelViewMatrix[2] = -sin(time);
		g_modelViewMatrix[5] = 1.0f;
		g_modelViewMatrix[8] = sin(time);
		g_modelViewMatrix[10] = cos(time);
		g_modelViewMatrix[14] = -distance;
		g_modelViewMatrix[15] = 1.0f;
	}

	else
	{
		//need to preserve the time that the rotation was stopped at
		end = getTime();
		pause = end - time;

		g_modelViewMatrix[0] = cos(start);
		g_modelViewMatrix[2] = -sin(start);
		g_modelViewMatrix[5] = 1.0f;
		g_modelViewMatrix[8] = sin(start);
		g_modelViewMatrix[10] = cos(start);
		g_modelViewMatrix[14] = -distance;
		g_modelViewMatrix[15] = 1.0f;
	}


	// https://open.gl/transformations y-Rotations
}

void setModelViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	updateModelViewMatrix();
	glLoadMatrixf(g_modelViewMatrix);
}

void drawTeapot() {
	glBegin(GL_TRIANGLES);
	for (size_t f = 0; f < g_meshIndices.size(); ++f)
	{
		const float scale = 0.1f;
		const unsigned int idx = g_meshIndices[f];
		const float x = scale * g_meshVertices[3 * idx + 0];
		const float y = scale * g_meshVertices[3 * idx + 1];
		const float z = scale * g_meshVertices[3 * idx + 2];

		const float nx = g_meshNormals[3 * idx + 0];
		const float ny = g_meshNormals[3 * idx + 1];
		const float nz = g_meshNormals[3 * idx + 2];

		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}
	glEnd();
}
void renderTeapot() {
	drawTeapot();
}

void drawCheckerBoard() {


	int checkerCount = g_windowWidth;
	int checkerSize = (g_windowWidth) / checkerCount;

	glBegin(GL_QUADS);
	for (int i = 0; i < checkerCount; ++i) {
		for (int j = 0; j < checkerCount; ++j) {
			if ((i + j) % 2 == 0)
				glColor3f(0.0, 0.1, 1.0);
			else
				glColor3f(1.0, 0.0, 1.0);

			float x = i - checkerCount / 2; // to be between -1 and 1
			float z = j - checkerCount / 2;
			x *= checkerSize;
			z *= checkerSize;
			float y = -1.0f;
			glVertex3f(x, y, z);
			glVertex3f(x, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z);
		}
	}
	glEnd();
}
void renderCheckerBoard() {

	/*
	/* If you want to keep checkerboard still while rotating the
	/* the teapot, you need to change the transformation for the
	/* checkerboard plane
	*/
	glMatrixMode(GL_MODELVIEW);
	clearModelViewMatrix();

	g_modelViewMatrix[0] = 1;
	g_modelViewMatrix[2] = 0;
	g_modelViewMatrix[5] = 1;
	g_modelViewMatrix[8] = 0;
	g_modelViewMatrix[10] = 1;
	g_modelViewMatrix[14] = -distance;
	g_modelViewMatrix[15] = 1.0f;

	glLoadMatrixf(g_modelViewMatrix);

	// Disable shading for the checkerboard
	glDisable(GL_LIGHTING);
	drawCheckerBoard();
	glEnable(GL_LIGHTING);
}

void render()
{
	togglePerspective();
	setModelViewMatrix();
	renderTeapot();
	if (showCheckerboard)
		renderCheckerBoard();
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

int main()
{
	initWindow();
	initGL();
	loadObj("data/teapot.obj");
	printHotKeys();
	renderLoop();
}
