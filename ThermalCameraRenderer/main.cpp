
#include <GL/glew.h>
#include <GL/glut.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#include <iostream>
#include <cmath>
#include <thread>

#include "SerialPort.h"

using namespace std;

int w = 1440, h = 900;
bool gl_info = true;
float dt;

float output[64];
char *port = (char*)"\\\\.\\COM3"; //ARDUINO COM IS CURRENTLY HARDCODED

float buffer[64];

float fmin1, fmax1, favg;

bool lock_buffer = false;
int displayMode = 0;

#pragma region UTILITIES
float lerp(float a, float b, float f)
{
	return a * (1.0 - f) + (b * f);
}

float invlerp(float a, float b, float f)
{
	return (f - a) / (b - a);
}

/// <summary>
/// Set color using a custom scale. 
/// 0.0f represents blue
/// 0.5f represents green
/// 1.0f represents red
/// intermediate values are an interpolation of those colors.
/// </summary>
/// <param name="f"></param>
void SetColor(float f)
{
	if (f > 0.5f)
	{
		glColor3d((f * 2.0f) - 1.0f, 2.0f - (f * 2.0f), 0.0f);
	}
	else
	{
		glColor3d(0.0f, (f * 2.0f), 1.0f - (f * 2.0f));
	}
}
#pragma endregion

#pragma region DISPLAY_FUNCTIONS

/// <summary>
/// Mode that displays raw information of each node of the thermal camera.
/// </summary>
void DisplayThermalMode0()
{
	int i, j;
	float ffmin, ffmax, ffavg;
	ffavg = 0;
	ffmax = ffmin = buffer[0];
	for (i = 1; i < 64; i++)
	{
		ffavg += buffer[i];
		if (buffer[i] > ffmax)	ffmax = buffer[i];
		if (buffer[i] < ffmin)	ffmin = buffer[i];
	}

	fmin1 = ffmin;
	fmax1 = ffmax;
	favg = ffavg / 64.0;



	for (j = 0; j < 8; j++)
	{
		for (i = 0; i < 8; i++)
		{

			glBegin(GL_QUADS);

			SetColor(invlerp(fmin1, fmax1, buffer[i + j * 8]));
			glVertex3f(-6 + i, 4 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[i + j * 8]));
			glVertex3f(-5 + i, 4 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[i + j * 8]));
			glVertex3f(-5 + i, 3 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[i + j * 8]));
			glVertex3f(-6 + i, 3 - j, -10);

			glEnd();
			glFlush();
		}
	}
}

/// <summary>
/// Mode that displays information of each node of the themal camera with a basic intepolation between colors.
/// </summary>
void DisplayThermalMode1()
{
	int i, j;
	float ffmin, ffmax, ffavg;
	ffavg = 0;
	ffmax = ffmin = buffer[0];
	for (i = 1; i < 64; i++)
	{
		ffavg += buffer[i];
		if (buffer[i] > ffmax)	ffmax = buffer[i];
		if (buffer[i] < ffmin)	ffmin = buffer[i];
	}

	fmin1 = ffmin;
	fmax1 = ffmax;
	favg = ffavg / 64.0;

	float ox = -5.5f;
	float oy = -0.5f;

	for (j = 0; j < 7; j++)
	{
		for (i = 0; i < 7; i++)
		{

			glBegin(GL_QUADS);

			SetColor(invlerp(fmin1, fmax1, buffer[i + j * 8]));
			glVertex3f(ox + i, oy + 4 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[(i + 1) + j * 8]));
			glVertex3f(ox + 1 + i, oy + 4 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[(i + 1) + (j + 1) * 8]));
			glVertex3f(ox + 1 + i, oy + 3 - j, -10);

			SetColor(invlerp(fmin1, fmax1, buffer[i + (j + 1) * 8]));
			glVertex3f(ox + i, oy + 3 - j, -10);
			glEnd();
			glFlush();
		}
	}
}

void DrawReferenceScale()
{
	glBegin(GL_QUADS);
	SetColor(1);
	glVertex3f(3, 3, -10);
	SetColor(1);
	glVertex3f(4, 3, -10);
	SetColor(0.5f);
	glVertex3f(4, 0, -10);
	SetColor(0.5f);
	glVertex3f(3, 0, -10);
	glEnd();
	glFlush();

	glBegin(GL_QUADS);
	SetColor(0.5f);
	glVertex3f(3, 0, -10);
	SetColor(0.5f);
	glVertex3f(4, 0, -10);
	SetColor(0.0f);
	glVertex3f(4, -3, -10);
	SetColor(0.0f);
	glVertex3f(3, -3, -10);
	glEnd();
	glFlush();

	glColor3f(1.0f, 0.0f, 0.0f);
	unsigned char string[64];
	snprintf((char*)string, sizeof(string), "%02.02f °C", fmax1);
	glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, string);
	glRasterPos3f(4.2, 3, -10);

	int len = strlen((char*)string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);

	glColor3f(0.0f, 1.0f, 0.0f);
	snprintf((char*)string, sizeof(string), "%02.02f °C", favg);
	glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, string);
	glRasterPos3f(4.2, 0, -10);

	len = strlen((char*)string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);

	glColor3f(0.0f, 0.0f, 1.0f);
	snprintf((char*)string, sizeof(string), "%02.02f °C", fmin1);
	glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, string);
	glRasterPos3f(4.2, -3, -10);

	len = strlen((char*)string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
}
#pragma endregion

#pragma region CALLBACKS

void Display_cb() {

	glClearColor(0.0, 0.0, 0.0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// rellena con color de fondo

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (displayMode)
	{
	case 1:
		DisplayThermalMode1();
		break;

	case 0:
	default:
		DisplayThermalMode0();
		break;
	}

	DrawReferenceScale();

	glutSwapBuffers(); // lo manda al monitor
	// chequea errores
	int errornum = glGetError();
	while (errornum != GL_NO_ERROR) {
		if (gl_info) {
			if (errornum == GL_INVALID_ENUM)
				cout << "GL_INVALID_ENUM" << endl;
			else if (errornum == GL_INVALID_VALUE)
				cout << "GL_INVALID_VALUE" << endl;
			else if (errornum == GL_INVALID_OPERATION)
				cout << "GL_INVALID_OPERATION" << endl;
			else if (errornum == GL_STACK_OVERFLOW)
				cout << "GL_STACK_OVERFLOW" << endl;
			else if (errornum == GL_STACK_UNDERFLOW)
				cout << "GL_STACK_UNDERFLOW" << endl;
			else if (errornum == GL_OUT_OF_MEMORY)
				cout << "GL_OUT_OF_MEMORY" << endl;
		}
		errornum = glGetError();
	}
}


void Reshape_cb(int width, int height) {
	if (!width || !height) return;

	w = width; h = height;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);  glLoadIdentity();

	gluPerspective(45.0,                  //The camera angle
		(double)w / (double)h, //The width-to-height ratio
		0.1,                //The near z clipping coordinate
		200.0);              //The far z clipping coordinate

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}


void Special_cb(int key, int xm = 0, int ym = 0)
{
	if (key == GLUT_KEY_F4 && glutGetModifiers() == GLUT_ACTIVE_ALT) // alt+f4 => exit
		exit(EXIT_SUCCESS);
}

void CheckKeysPressed()
{

	if (GetKeyState(VK_F1) & 0x80)
	{
		displayMode = 0;
	}

	if (GetKeyState(VK_F2) & 0x80)
	{
		displayMode = 1;
	}
}


void funcIdle()
{
	static unsigned int lt = 0;
	dt = glutGet(GLUT_ELAPSED_TIME) - lt;
	lt = glutGet(GLUT_ELAPSED_TIME);

	CheckKeysPressed();

	if (!lock_buffer)
	{
		lock_buffer = true;

		for (int i = 0; i < 64; i++)
			buffer[i] = output[i];

		lock_buffer = false;
	}


	Display_cb();
}

void windowKey(unsigned char key, int x, int y)
{
	glutPostRedisplay();
}
#pragma endregion

void initialize() {

	// GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(w, h); 
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Thermal Camera Render");

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glEnable(GL_DEPTH_TEST);
	
	//Callbacks
	glutDisplayFunc(Display_cb);
	glutReshapeFunc(Reshape_cb);
	glutSpecialFunc(Special_cb);
	glutIdleFunc(funcIdle);	//Main callback
	glutKeyboardFunc(windowKey);
                
	glLoadIdentity();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void readArduino(SerialPort* s)
{
	const bool DEBUG_LOGS = false;

	char buf_aux[MAX_DATA_LENGTH];
	float oout[64];
	int iaux = 0;
	bool err_flag = false;
	while (true)
	{
		Sleep(250);
		if (!lock_buffer)
		{
			lock_buffer = true;
			err_flag = false;
			s->readSerialPort(buf_aux, MAX_DATA_LENGTH);

			int j = 0;
			for (int i = 0; i < strlen(buf_aux) && !err_flag; i+=3)
			{
				if (j < 64) //reads only the first 64 char of the buffer
				{
					oout[j] = 0;
					for (int k = 0; k < 3; k++)
					{
						iaux = ((int)buf_aux[i + k]) - '0'; //converts char number to int
						//checks if the current char contains a number from 0 to 9. If not then the buffer has wrong data 
						//if the buffer has wrong data then it gets discarded
						if (iaux > 9 || iaux < 0) 
						{
							err_flag = true;
							break;
						}

						if (k == 0)
							oout[j] += iaux * 100;
						else if (k == 1)
							oout[j] += iaux * 10;
						else
							oout[j] += iaux;

					}

					if (DEBUG_LOGS)
					{
						std::cout << ((int)buf_aux[i]) - '0' << ((int)buf_aux[i + 1]) - '0' << ((int)buf_aux[i + 2]) - '0' << " | ";
						if ((j + 1) % 8 == 0)
						{
							std::cout << "\n";
						}
					}
					
				}
					
				j += 1;
				
			}

			if (!err_flag)
			{
				//updates buffer
				for (int i = 0; i < 64; i++)
					output[i] = oout[i]/10.0;

			}
			if (DEBUG_LOGS)
				std::cout << "\n\n";

			lock_buffer = false;
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	initialize();
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	SerialPort arduino(port);
	if (arduino.isConnected())
		std::cout << "Arduino connected\n";
	else
	{
		std::cout << "ERROR: Arduino not found\n";
		return 1;
	}
	std::thread th1(readArduino, &arduino);

	glutMainLoop();
	return 0;
}
