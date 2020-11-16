#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define M_PI 3.141592654f

unsigned int g_windowWidth = 600;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW2-Rasterization";

GLFWwindow* g_window;


int x_ = 0;
int y_ = 0;

int x1_ = 0;
int x2_ = 0;

int y1_ = 0;
int y2_ = 0;

int circleX = 0;
int circleY = 0;

int radiusX = 0;
int radiusY = 0;

int flag = 1;
int circleFlag = 1;

int mode = 0;

const int g_image_width = g_windowWidth;
const int g_image_height = g_windowHeight;



std::vector<float> g_image;

void putPixel(int x, int y)
{

	// clamp
	if (x >= g_image_width || x < 0 || y >= g_image_height || y < 0) return;

	// write
	g_image[y* g_image_width + x] = 1.0f;
}

//-------------------------------------------------------------------------------

void drawLine(int x1, int y1, int x2, int y2)
{
	// TODO: part of Homework Task 1
	// This function should draw a line from pixel (x1, y1) to pixel (x2, y2)

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	//flip to help remove error cases
	bool check = dx < dy;
	if (check)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	//set dx and dy values
	dx = x2 - x1;
	dy = abs(y2 - y1);


	//start at the base level
	int level = 0;

	//slope error
	int error = dx / 2;


	//loop through and place pixels
	while (x1 < x2)
	{

		if (check)
		{
			putPixel(y1, x1++);

		}
		else
		{
			putPixel(x1++, y1);
		}

		error -= dy;


		//need this for angled lines to adjust them
		if (error < 0)
		{
			//slope calc (neg or positive slope)
			if (y1 < y2)
			{
				level = 1;
			}
			else
			{
				level = -1;
			}
			//increase slope level based on level value
			y1 += level;
			error += dx;
		}
	}

}


void drawCircle(int x0, int y0, int R)
{
	// TODO: part of Homework Task 2
	// This function should draw a circle,
	// where pixel (x0, y0) is the center of the circle and R is the radius
	int xc = 0;
	int yc = R;
	int d = 1 - R;

	putPixel(xc + x0, yc + y0);
	putPixel(xc + x0, -yc + y0);
	putPixel(yc + x0, xc + y0);
	putPixel(yc + x0, -xc + y0);
	//neg
	putPixel(-xc + x0, yc + y0);
	putPixel(-xc + x0, -yc + y0);
	putPixel(-yc + x0, xc + y0);
	putPixel(-yc + x0, -xc + y0);

	//loop through and put pixels
	while (xc <= yc)
	{

		if (d < 0)
		{
			d += 2 * xc + 3;
		}

		else
		{
			d += 2 * (xc - yc) + 5;
			yc--;
		}

		xc++;

		putPixel(xc + x0, yc + y0);
		putPixel(xc + x0, -yc + y0);
		putPixel(yc + x0, xc + y0);
		putPixel(yc + x0, -xc + y0);

		putPixel(-xc + x0, yc + y0);
		putPixel(-xc + x0, -yc + y0);
		putPixel(-yc + x0, xc + y0);
		putPixel(-yc + x0, -xc + y0);

	}

}


void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}

	//line mode
	else if (p_key == GLFW_KEY_L && p_action == GLFW_PRESS)
	{
		std::cout << "Line mode" << std::endl;
		mode = 1;
	}

	//pixel mode
	else if (p_key == GLFW_KEY_N && p_action == GLFW_PRESS)
	{
		std::cout << "Pixel mode" << std::endl;
		mode = 0;
	}

	//circle mode
	else if (p_key == GLFW_KEY_C && p_action == GLFW_PRESS)
	{
		std::cout << "Circle mode" << std::endl;
		mode = 2;
	}
}

//!GLFW mouse callback
void glfwMouseButtonCallback(GLFWwindow* p_window, int p_button, int p_action, int p_mods)
{
	double xpos, ypos;
	glfwGetCursorPos(p_window, &xpos, &ypos);
	ypos = g_windowHeight - ypos;
	if (p_button == GLFW_MOUSE_BUTTON_LEFT && p_action == GLFW_PRESS)
	{
		std::cout << "You clicked pixel: " << xpos << ", " << ypos << std::endl;

		putPixel(xpos, ypos);

		//line controls
		if (mode == 1)
		{
			//	int x1, x2, y1, y2;
			if (flag == 2)
			{
				x1_ = xpos;
				y1_ = ypos;

				x2_ = 0;
				y2_ = 0;
				flag = 1;
			}

			else if (x1_ == 0 && y1_ == 0)
			{
				x1_ = xpos;
				y1_ = ypos;
			}

			else
			{
				x2_ = xpos;
				y2_ = ypos;
				drawLine(x1_, y1_, x2_, y2_);
				flag = 2;
			}
		}

		//Circle controls
		else if (mode == 2)
		{

			if (circleFlag == 2)
			{
				circleX = xpos;
				circleY = ypos;

				radiusX = 0;
				radiusY = 0;
				circleFlag = 1;
			}

			else if (circleX == 0 && circleY == 0)
			{
				circleX = xpos;
				circleY = ypos;
			}

			else
			{
				radiusX = xpos;
				radiusY = ypos;

				int first = (circleX - radiusX);
				int second = (circleY - radiusY);

				int radius = sqrt((first * first) + (second * second));


				drawCircle(circleX, circleY, radius);
				circleFlag = 2;
			}
		}
		// TODO: part of Homework Task 3
		// This part should draw appropriate figure according to the current mode

	}
}

//-------------------------------------------------------------------------------

struct color
{
	unsigned char r, g, b;
};

int ReadLine(FILE *fp, int size, char *buffer)
{
	int i;
	for (i = 0; i < size; i++) {
		buffer[i] = fgetc(fp);
		if (feof(fp) || buffer[i] == '\n' || buffer[i] == '\r') {
			buffer[i] = '\0';
			return i + 1;
		}
	}
	return i;
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
	glfwSetMouseButtonCallback(g_window, glfwMouseButtonCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(g_image_width, g_image_height, GL_LUMINANCE, GL_FLOAT, &g_image[0]);
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

void initImage()
{
	g_image.resize(g_image_width * g_image_height);
}

bool writeImage()
{
	std::vector<color> tmpData;
	tmpData.resize(g_image_width * g_image_height);

	for (int i = 0; i < g_image_height; i++)
	{
		for (int j = 0; j < g_image_width; j++)
		{
			// make sure the value will not be larger than 1 or smaller than 0, which might cause problem when converting to unsigned char
			float tmp = g_image[i* g_image_width + j];
			if (tmp < 0.0f)	tmp = 0.0f;
			if (tmp > 1.0f)	tmp = 1.0f;

			tmpData[(g_image_height - i - 1)* g_image_width + j].r = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1)* g_image_width + j].g = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1)* g_image_width + j].b = unsigned char(tmp * 255.0);
		}
	}

	FILE *fp = fopen("data/out.ppm", "wb");
	if (!fp) return false;

	fprintf(fp, "P6\r");
	fprintf(fp, "%d %d\r", g_image_width, g_image_height);
	fprintf(fp, "255\r");
	fwrite(tmpData.data(), sizeof(color), g_image_width * g_image_height, fp);
	fclose(fp);

	return true;
}

void drawImage()
{
	drawLine(150, 10, 450, 10);
	drawLine(150, 310, 450, 310);
	drawLine(150, 10, 150, 310);
	drawLine(450, 10, 450, 310);
	drawLine(150, 310, 300, 410);
	drawLine(300, 410, 450, 310);

	drawCircle(500, 500, 50);
}

int main()
{
	initImage();
	drawImage();
	writeImage();
	writeImage();

	// render loop
	initWindow();
	initGL();
	renderLoop();

	return 0;
}
