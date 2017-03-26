//
//  main.cpp
//  RubiksMiniCube
//
//  Created by Kutlay Hanli on 26/03/2017.
//  Copyright © 2017 Kutlay Hanli. All rights reserved.
//

#include "Angel.h"

using namespace std;

typedef vec4  color4;
typedef vec4  point4;

float maxAllowedDept = 1.0;

// Declare Vertex Array Object Array and Buffer Array
GLuint vao[2];
GLuint buffer[2];

// Declare uniform cariable to be passed to Fragment Shader
GLuint currentColor;

// Initialize Center of Gravity of the object
vec3 centerOfG = vec3(0.0, 0.99 , 0.0);

// Enumerate object types
enum object {SPHERE = 0, CUBE = 1};

// Set initial object to SPHERE
int currentObject = SPHERE;

// Square root of 3 to scale cube correctly
float sqrt3 = 1.73205080757;

// Set inital scale to 0.5
float scale = 0.5;

// Initilize Acceleration
float acceleration = -0.001;

// Initilize Velocity
float velocity = 0.0;

// Initilize Angular Velocity
float angularVelocity = 0.5;

// Initilize Dampening
float dampening = 0.99;

// RGBA olors
// enumerate colors and create a color4 array as colors array
enum color {BLACK,RED,YELLOW,GREEN,BLUE,MAGENTA,WHITE,CYAN};
color4 colors[8] = {
	color4( 0.0, 0.0, 0.0, 1.0 ),  // black
	color4( 1.0, 0.0, 0.0, 1.0 ),  // red
	color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),  // green
	color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),  // white
	color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Yaxis;
GLfloat  Theta[NumAxes] = { 0, 0, 0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

/* color menu function
 * changes color on user request
 * 8 predefined colors and a random color
 */
void changeColor(int color)
{
	if (color == -1)
	{
		glUniform4f(currentColor ,(float)rand()/(float)(RAND_MAX),
					(float)rand()/(float)(RAND_MAX),
					(float)rand()/(float)(RAND_MAX),
					(float)rand()/(float)(RAND_MAX));
	}
	else
	{
		glUniform4f(currentColor ,colors[color].x, colors[color].y, colors[color].z, colors[color].w);
	}
	if (color == WHITE)
	{
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
	}
	else
	{
		glClearColor( 1.0, 1.0, 1.0, 1.0 );
	}
}

//----------------------------------------------------------------------------

/* object menu function
 * Changes object on user request
 */
void changeObject(int obj)
{
	currentObject = obj;
}

//----------------------------------------------------------------------------

/* polygone mode menu function
 * Enables or disables wireframe mode on user request
 */
void changePolygonMode(int choice)
{
	if (choice == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (choice == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//----------------------------------------------------------------------------

/* scale menu function
 * Increase or decrease scale on user request
 * Scale value is limited between 0.95 and 0.1
 * If the scaled up object won't fit in the scene it prints a waring and
 doesn't scale up
 */
void changeScale(int choice)
{
	if (choice == 0 && scale <=0.95)
	{
		centerOfG.y/=1.25;
		scale*=1.25;
	}
	if (choice == 1 && scale >= 0.1)
	{
		centerOfG.y/=0.8;
		scale*=0.8;
	}
	if (centerOfG.y <= -1/scale + 1 || centerOfG.y >= 1/scale - 1)
	{
		centerOfG.y/=0.8;
		scale*=0.8;
		cout << "Warning: Can't scale up at this time." << endl;
	}
}

//----------------------------------------------------------------------------

/* acceleration menu function
 * Increase or decrease acceleration on user request
 */
void changeAcceleration(int choice)
{
	if (choice == 0)
	{
		acceleration*=1.25;
	}
	else if (choice == 1)
	{
		acceleration*=0.8;
	}
}

//----------------------------------------------------------------------------

/* dampening menu function
 * Enables or disables dampening on user request
 */
void enableDampening(int choice)
{
	if (choice == 0)
	{
		dampening = 0.99;
	}
	else if (choice == 1)
	{
		dampening = 1;
	}
}

//----------------------------------------------------------------------------

// struct SPHERE APPROXIMATION

//----------------------------------------------------------------------------

struct sphereApproximation
{
	// vertex array pointer
	point4 *vertices;
	// initilize bipyramid points
	point4 initialBiPyramidVertices[6] = {
		point4( 0, 1, 0,1),	// top
		point4( 1, 0, 0,1),	// plane1
		point4( 0, 0, 1,1),	// plane2
		point4(-1, 0, 0,1),	// plane3
		point4( 0, 0,-1,1),	// plane4
		point4( 0,-1, 0,1)	// bottom
	};
	// number of vertices
	int numberOfVertices = 0;
	
	// creates one half of the bipyramid object using inital vertices
	void oneHalf(int peak,int &index)
	{
		vertices[index] = initialBiPyramidVertices[peak];	index++;
		vertices[index] = initialBiPyramidVertices[1];		index++;
		vertices[index] = initialBiPyramidVertices[2];		index++;
		vertices[index] = initialBiPyramidVertices[peak];	index++;
		vertices[index] = initialBiPyramidVertices[4];		index++;
		vertices[index] = initialBiPyramidVertices[1];		index++;
		vertices[index] = initialBiPyramidVertices[peak];	index++;
		vertices[index] = initialBiPyramidVertices[2];		index++;
		vertices[index] = initialBiPyramidVertices[3];		index++;
		vertices[index] = initialBiPyramidVertices[peak];	index++;
		vertices[index] = initialBiPyramidVertices[3];		index++;
		vertices[index] = initialBiPyramidVertices[4];		index++;
	}
	
	// create all bipyramid points using oneHalf func.
	void biPyramid()
	{
		int index = 0;
		oneHalf(0,index);
		oneHalf(5,index);
	}
	
	/*
	 * Iteration function which takes the lower level sphereApproximation and
	 the reference to number of times the user would like to iterate.
	 * Each iteration creates 4 new equilateral triangles from the lower level
	 triangles and corrects the distance of the new points from the origin
	 such that the points will lay on the unit sphere. using genTriangles func.
	 * param lowerLevel:
	 Type: sphereApproximation
		It is the previous sphereAprroximation that the next one will base on.
	 * param times:
		Type: int reference
		It is the number of iterations remaining from the user request.
	 */
	void iterate(sphereApproximation lowerLevel, int &times)
	{
		int index = 0;
		if (numberOfVertices == 0)
		{
			numberOfVertices = 36;
			vertices = new point4[numberOfVertices];
			biPyramid();
		}
		else
		{
			numberOfVertices = lowerLevel.numberOfVertices * 4;
			vertices = new point4[numberOfVertices];
			for (int i=0; i < lowerLevel.numberOfVertices / 3; i++)
			{
				genTriangles(vertices, index, lowerLevel.vertices[i*3+0],
							 lowerLevel.vertices[i*3+1],
							 lowerLevel.vertices[i*3+2]);
			}
		}
		if (times > 0)
		{
			times--;
			this->iterate(*this,times);
		}
	}
	
	/*
	 * Creates 4 new equilateral triangles from the lower level
	 triangle and corrects the distance of the new points from the origin
	 such that the points will lay on the unit sphere using interpolate func.
	 * param arr:
		Type: point4 pointer
		The array where the points will be inserted
	 * param int reference:
		Type: int reference
		The index where the points will be inserted
	 * param a:
		Type: point4
		A point of the lower triangle
	 * param b:
		Type: point4
		A point of the lower triangle
	 * param c:
		Type: point4
		A point of the lower triangle
	 */
	void genTriangles(point4* arr,int &currentIndex,point4 a,point4 b,point4 c)
	{
		point4 d = interpolate(a, b);
		point4 e = interpolate(a, c);
		point4 f = interpolate(b, c);
		arr[currentIndex] = a; currentIndex++;
		arr[currentIndex] = d; currentIndex++;
		arr[currentIndex] = e; currentIndex++;
		arr[currentIndex] = b; currentIndex++;
		arr[currentIndex] = d; currentIndex++;
		arr[currentIndex] = f; currentIndex++;
		arr[currentIndex] = c; currentIndex++;
		arr[currentIndex] = e; currentIndex++;
		arr[currentIndex] = f; currentIndex++;
		arr[currentIndex] = d; currentIndex++;
		arr[currentIndex] = e; currentIndex++;
		arr[currentIndex] = f; currentIndex++;
	}
	
	/*
	 * Interpolates 2 points and creates a new point. Then adjust the distance
	 of the point from the origin such that it will lay on the unit sphere.
	 * param p1:
		Type: point4
		first point
	 * param p2:
		Type: point4
		second point
	 * return point4
		returns the middle point.
	 */
	point4 interpolate(point4 p1, point4 p2)
	{
		point4 result;
		float r;
		result.x = (p1.x + p2.x) / 2;
		result.y = (p1.y + p2.y) / 2;
		result.z = (p1.z + p2.z) / 2;
		result.w = 1;
		
		r = sqrtf(result.x*result.x + result.y*result.y + result.z*result.z);
		
		result.x/=r;
		result.y/=r;
		result.z/=r;
		
		return result;
	}
};

// declare sphere struct
struct sphereApproximation sphere;

//----------------------------------------------------------------------------

// struct CUBE
struct cube
{
	int numberOfVertices = 36;
	point4 vertices[36];
	// Vertices of a cube centered at origin which fits in a unit sphere, sides aligned with axes
	point4 initialVertices[8] = {
		RotateX(45) * RotateZ(45) * point4( -1, -1,  1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4( -1,  1,  1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4(  1,  1,  1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4(  1, -1,  1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4( -1, -1, -1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4( -1,  1, -1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4(  1,  1, -1, sqrt3 ),
		RotateX(45) * RotateZ(45) * point4(  1, -1, -1, sqrt3 )
	};
	
	// quad generates two triangles for each face and assigns colors
	//    to the vertices
	
	void quad(int &index, int a, int b, int c, int d )
	{
		vertices[index] = initialVertices[a]; index++;
		vertices[index] = initialVertices[b]; index++;
		vertices[index] = initialVertices[c]; index++;
		vertices[index] = initialVertices[a]; index++;
		vertices[index] = initialVertices[c]; index++;
		vertices[index] = initialVertices[d]; index++;
	}
	
	// generate 12 triangles: 36 vertices and 36 colors
	
	void generateCube()
	{
		int index = 0;
		quad(index, 1, 0, 3, 2 );
		quad(index, 2, 3, 7, 6 );
		quad(index, 3, 0, 4, 7 );
		quad(index, 6, 5, 1, 2 );
		quad(index, 4, 5, 6, 7 );
		quad(index, 5, 4, 0, 1 );
	}
};

// declare cube struct
struct cube cube;

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// generate cube
	cube.generateCube();
	
	// generate sphere
	int detailLevel = 4;
	sphere.iterate(sphere, detailLevel);
	
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	
	// Create a vertex array object
	glGenVertexArrays( 2, vao );
	// Bind vao for SPHERE
	glBindVertexArray( vao[SPHERE] );
	// Initialize a buffer object
	glGenBuffers( 1, &buffer[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(point4)*sphere.numberOfVertices, sphere.vertices, GL_STATIC_DRAW );
	GLuint vPositionSphere = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPositionSphere );
	glVertexAttribPointer( vPositionSphere, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	// Bind vao for CUBE
	glBindVertexArray( vao[CUBE] );
	// Initialize a buffer object
	glGenBuffers( 1, &buffer[CUBE] );
	glBindBuffer( GL_ARRAY_BUFFER, buffer[CUBE] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(point4)*cube.numberOfVertices, cube.vertices, GL_STATIC_DRAW );
	GLuint vPositionCube = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPositionCube );
	glVertexAttribPointer( vPositionCube, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	currentColor = glGetUniformLocation(program, "currentColor");
	glUniform4f(currentColor ,(float)rand()/(float)(RAND_MAX),
				(float)rand()/(float)(RAND_MAX),
				(float)rand()/(float)(RAND_MAX),
				(float)rand()/(float)(RAND_MAX));
	
	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );
	
	// Set current program object
	glUseProgram( program );
	
	// Set projection matrix
	mat4  projection;
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	// Ortho(): user-defined function in mat.h
	//projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
	glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
	
	// Enable hiddden surface removal
	glEnable( GL_DEPTH_TEST );
	
	// Set state variable "clear color" to clear buffer with.
	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	
	cout << "Initially dampening is enabled, polygone mode is fill," << endl;
	cout << "object is sphere, scale is 0.5 and color is random." << endl;
	cout << "Press 'h' for help" << endl;
}

//----------------------------------------------------------------------------

void
display( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//  Generate tha model-view matrix
	const vec3 displacement( 0.0, 0.0, 0.0 );
	mat4  model_view = ( Scale(scale, scale, scale) * Translate( centerOfG ) *
						RotateX( Theta[Xaxis] ) *
						RotateY( Theta[Yaxis] ) *
						RotateZ( Theta[Zaxis] ) );
	
	// Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
	glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
	
	// Changes Vertex Array Object depending on users selection
	glBindVertexArrayAPPLE(vao[currentObject]);
	if (currentObject == SPHERE)
	{
		glDrawArrays( GL_TRIANGLES, 0, sphere.numberOfVertices);
	}
	else if (currentObject == CUBE)
	{
		glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
	}
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
idle( void )
{
	Theta[Axis] += angularVelocity;
	
	if ( Theta[Axis] > 360.0 )
	{
		Theta[Axis] -= 360.0;
	}
	
	// Bounces the object from the bottom of the screen
	if (centerOfG.y <= (-1/scale) * maxAllowedDept + 1)
	{
		velocity = -velocity;
	}
	
	// Bounces the object from the top of the screen
	if (centerOfG.y >= (1/scale) * maxAllowedDept - 1)
	{
		centerOfG.y -= velocity;
		velocity = -velocity*dampening*dampening*dampening;
	}
	
	// Decreases velocity when the object is rising.
	if (velocity > 0)
	{
		velocity*=dampening;
	}
	
	// Changes the center of gravity of the object depending on velocity
	centerOfG.y += velocity;
	
	// Changes velocity depending on acceleration
	velocity += acceleration;
	
	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key,int x, int y )
{
	// quits on 'q' command
	if(key == 'Q' | key == 'q')
		exit(0);
	
	// initializes the scene
	if(key == 'I' | key == 'i')
	{
		centerOfG.y=0.99;
		velocity=0.0;
	}
	
	// prints command menu
	if(key == 'H' | key == 'h')
	{
		cout << "You can change scale, color, object, polygone mode," << endl;
		cout << "dampening and acceleration from right click menu." << endl;
		cout << "Keyboard commands:" << endl;
		cout << "i\t\t\treturn to initial position" << endl;
		cout << "Arrow up\t\tincrease velocity" << endl;
		cout << "Arrow down\t\tdecrease velocity" << endl;
		cout << "Arrow right\tincrease angular velocity" << endl;
		cout << "Arrow left\t\tdecrease angular velocity" << endl;
		cout << "q\t\t\tquit" << endl;
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void specialCallBack(int key, int x, int y)
{
	// increase angular velocity
	if (key == GLUT_KEY_RIGHT)
		angularVelocity+=0.5;
	
	// decrease angular velocity
	if (key == GLUT_KEY_LEFT && angularVelocity > 0.0)
		angularVelocity-=0.5;
	
	// increase velocity
	if (key == GLUT_KEY_UP && velocity <= 1)
		velocity*=1.25;
	
	// decrease velocity
	if (key == GLUT_KEY_DOWN)
		velocity*=0.8;
}

//----------------------------------------------------------------------------

void reshape( int w, int h )
{
	glViewport( 0, 0, w, h );
	
	mat4  projection;
	if (w <= h)
		projection = Ortho(-1.0, 1.0, -1.0 * (GLfloat) h / (GLfloat) w,
						   1.0 * (GLfloat) h / (GLfloat) w, -1.0, 1.0);
	else  projection = Ortho(-1.0* (GLfloat) w / (GLfloat) h, 1.0 *
							 (GLfloat) w / (GLfloat) h, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
	if (float(h)/float(w) > 1)
	{
		maxAllowedDept = float(h)/float(w);
	}
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 512, 512 );
	glutCreateWindow( "Jumping Object" );
	
	init();
	
	glutDisplayFunc( display ); // set display callback function
	glutIdleFunc( idle );
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialCallBack);
	glutReshapeFunc(reshape);
	
	// menus
	int colorMenu = glutCreateMenu(changeColor); // {BLACK,RED,YELLOW,GREEN,BLUE,MAGENTA,WHITE,CYAN};
	glutAddMenuEntry("black", BLACK);
	glutAddMenuEntry("red", RED);
	glutAddMenuEntry("yellow", YELLOW);
	glutAddMenuEntry("green", GREEN);
	glutAddMenuEntry("blue", BLUE);
	glutAddMenuEntry("magenta", MAGENTA);
	glutAddMenuEntry("white", WHITE);
	glutAddMenuEntry("cyan", CYAN);
	glutAddMenuEntry("random", -1);
	int objectMenu = glutCreateMenu(changeObject);
	glutAddMenuEntry("sphere", 0);
	glutAddMenuEntry("cube", 1);
	int modeMenu = glutCreateMenu(changePolygonMode);
	glutAddMenuEntry("wireframe", 0);
	glutAddMenuEntry("fill", 1);
	int scaleMenu = glutCreateMenu(changeScale);
	glutAddMenuEntry("up", 0);
	glutAddMenuEntry("down", 1);
	int dampeningMenu = glutCreateMenu(enableDampening);
	glutAddMenuEntry("enable", 0);
	glutAddMenuEntry("disable", 1);
	int accelerationMenu = glutCreateMenu(changeAcceleration);
	glutAddMenuEntry("inrease", 0);
	glutAddMenuEntry("decrease", 1);
	
	glutCreateMenu(0);
	glutAddSubMenu("scale", scaleMenu);
	glutAddSubMenu("colors", colorMenu);
	glutAddSubMenu("object", objectMenu);
	glutAddSubMenu("mode", modeMenu);
	glutAddSubMenu("dampening", dampeningMenu);
	glutAddSubMenu("acceleration", accelerationMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	glutMainLoop();
	return 0;
}
