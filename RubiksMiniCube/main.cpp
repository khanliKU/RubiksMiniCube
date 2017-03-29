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

// Declare Vertex Array Object Array and Buffer Array
GLuint vao[3];
GLuint buffer[3]; // try it
GLuint vPosition[3], vColor[3];

float rotationSpeed = 5;
bool enableAxis;

// Initialize Center of Gravity of the cubes
vec3 centerOfGs[8] = {
	vec3( -0.27, -0.27,  0.27),
	vec3( -0.27,  0.27,  0.27),
	vec3(  0.27,  0.27,  0.27),
	vec3(  0.27, -0.27,  0.27),
	vec3( -0.27, -0.27, -0.27),
	vec3( -0.27,  0.27, -0.27),
	vec3(  0.27,  0.27, -0.27),
	vec3(  0.27, -0.27, -0.27)
};

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
mat4  model_view[10] = {
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[0]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[1]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[2]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[3]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[4]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[5]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[6]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0) * Translate(centerOfGs[7]),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(1.0, 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0)
};

// Declare uniform cariable to be passed to Fragment Shader
GLuint currentCubeColor;
int currentCube;

// RGBA olors
// enumerate colors and create a color4 array as colors array
enum color {ORANGE,RED,YELLOW,GREEN,BLUE,MAGENTA,BLACK,CYAN,NEUTRAL};
color4 color[8] = {
	color4( 1.0, 0.549, 0.0, 1.0 ),  // orange
	color4( 1.0, 0.0, 0.0, 1.0 ),    // red
	color4( 1.0, 1.0, 0.0, 1.0 ),    // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),    // green
	color4( 0.0, 0.0, 1.0, 1.0 ),    // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),    // magenta
	color4( 0.0, 0.0, 0.0, 1.0 ),    // black
	color4( 0.0, 1.0, 1.0, 1.0 )    // cyan
};

// Axis
point4 axisV[6] = {
	point4(0.0, 0.0, 0.0, 1.0),
	point4(1.0, 0.0, 0.0, 1.0),
	point4(0.0, 0.0, 0.0, 1.0),
	point4(0.0, 1.0, 0.0, 1.0),
	point4(0.0, 0.0, 0.0, 1.0),
	point4(0.0, 0.0, 1.0, 1.0)
};

color4 axisC[6] = {
	color[MAGENTA],
	color[MAGENTA],
	color[BLUE],
	color[BLUE],
	color[BLACK],
	color[BLACK]
};

// color castig array
mat4 colorCast[9] = {
	mat4(0.0, 0.0, 0.0, 1.0, // orange
		 0.0, 0.0, 0.0, 0.549,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 1.0, // red
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 1.0, // yellow
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 0.0, // green
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 0.0, // blue
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 1.0, // magenta
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 0.0, // black
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(0.0, 0.0, 0.0, 0.0, // cyan
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 1.0,
		 0.0, 0.0, 0.0, 1.0),
	mat4(1.0, 0.0, 0.0, 0.0, // neutral
		 0.0, 1.0, 0.0, 0.0,
		 0.0, 0.0, 1.0, 0.0,
		 0.0, 0.0, 0.0, 1.0)
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };

//----------------------------------------------------------------------------

struct axis
{
	int numberOfVertices = 6;
	point4 vertices[6] =
	{
		point4( 0, 0, 0, 1), point4( 1, 0, 0, 1),
		point4( 0, 0, 0, 1), point4( 0, 1, 0, 1),
		point4( 0, 0, 0, 1), point4( 0, 0, 1, 1)
	};
	color4 colors[6] =
	{
		color[BLUE],color[BLUE],
		color[YELLOW],color[YELLOW],
		color[RED],color[RED]
	};
};

struct axis axis;

//----------------------------------------------------------------------------

// struct CUBE
struct Cube
{
	int numberOfVertices = 36;
	int numberOfEdges = 24;
	int edgeColor = BLACK;
	point4 vertices[36];
	color4 colors[36];
	// Vertices of a cube centered at origin which fits in a unit sphere, sides aligned with axes
	point4 initialVertices[8] = {
		point4( -0.25, -0.25,  0.25, 1 ),
		point4( -0.25,  0.25,  0.25, 1 ),
		point4(  0.25,  0.25,  0.25, 1 ),
		point4(  0.25, -0.25,  0.25, 1 ),
		point4( -0.25, -0.25, -0.25, 1 ),
		point4( -0.25,  0.25, -0.25, 1 ),
		point4(  0.25,  0.25, -0.25, 1 ),
		point4(  0.25, -0.25, -0.25, 1 )
	};
	
	point4 edges[24] = {
		initialVertices[0],initialVertices[1],
		initialVertices[0],initialVertices[3],
		initialVertices[0],initialVertices[4],
		initialVertices[1],initialVertices[2],
		initialVertices[1],initialVertices[5],
		initialVertices[2],initialVertices[3],
		initialVertices[2],initialVertices[6],
		initialVertices[3],initialVertices[7],
		initialVertices[4],initialVertices[5],
		initialVertices[4],initialVertices[7],
		initialVertices[6],initialVertices[5],
		initialVertices[6],initialVertices[7]
	};
	
	color4 edgeColors[24] = {
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor],
		color[edgeColor],color[edgeColor]
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
		quad(index, 1, 0, 3, 2 ); // quad 0
		quad(index, 2, 3, 7, 6 ); // quad 1
		quad(index, 3, 0, 4, 7 ); // quad 2
		quad(index, 6, 5, 1, 2 ); // quad 3
		quad(index, 4, 5, 6, 7 ); // quad 4
		quad(index, 5, 4, 0, 1 ); // quad 5
		
		index = 0;
		for (; index < 6 ; index++)
			colors[index] = color[RED];
		for (; index < 12 ; index++)
			colors[index] = color[BLUE];
		for (; index < 18 ; index++)
			colors[index] = color[GREEN];
		for (; index < 24 ; index++)
			colors[index] = color[YELLOW];
		for (; index < 30 ; index++)
			colors[index] = color[ORANGE];
		for (; index < 36 ; index++)
			colors[index] = color[CYAN];
	}
};

// declare cube struct
struct Cube cube;

//----------------------------------------------------------------------------

struct Rubik
{
	// hangi kupun hangi yuzu, hangi ana yuze bakiyor
	int cubes[8][6] = {
		{ 0, -1,  2, -1, -1,  5},
		{ 0, -1, -1,  3, -1,  5},
		{ 0,  1, -1,  3 ,-1, -1},
		{ 0,  1,  2, -1, -1, -1},
		{-1, -1,  2, -1,  4,  5},
		{-1, -1, -1,  3,  4,  5},
		{-1,  1, -1,  3,  4, -1},
		{-1,  1,  2, -1,  4, -1}
	};
	
	int rotating = 0;
	bool cubesToRotate[8] = {false,false,false,false,false,false,false,false};
	int rotationAxe;
	int remainingShuffle;
	
	void init()
	{
		model_view[8] = mat4();
		model_view[9] = mat4();
		remainingShuffle = 0;
	}
	
	void restart()
	{
		remainingShuffle = 50;
	}
	
	int getQuad(int c, int f)
	{
		for (int i=0; i<6 ; i++)
		{
			if (cubes[c][i] == f)
				return i;
		}
		return -1;
	}
	
	void rotateCW(int cube, int face)
	{
		int temp;
		int quad = getQuad(cube, face);
		if (quad == 0)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][0] != -1)
				{
					temp = cubes[c][1];
					cubes[c][1] = cubes[c][2];
					cubes[c][2] = cubes[c][5];
					cubes[c][5] = cubes[c][3];
					cubes[c][3] = temp;
					
					rotationAxe = Zaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 1)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][1] != -1)
				{
//					printf("%d %d %d %d %d\n",c,cubes[c][0],cubes[c][2],cubes[c][3],cubes[c][4]);
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][3];
					cubes[c][3] = cubes[c][4];
					cubes[c][4] = cubes[c][2];
					cubes[c][2] = temp;
					
					rotationAxe = Xaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 2)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][2] != -1)
				{
					//					printf("%d %d %d %d %d\n",c,cubes[c][0],cubes[c][2],cubes[c][3],cubes[c][4]);
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][1];
					cubes[c][1] = cubes[c][4];
					cubes[c][4] = cubes[c][5];
					cubes[c][5] = temp;
					
					rotationAxe = Yaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 3)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][3] != -1)
				{
					//					printf("%d %d %d %d %d\n",c,cubes[c][0],cubes[c][2],cubes[c][3],cubes[c][4]);
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][5];
					cubes[c][5] = cubes[c][4];
					cubes[c][4] = cubes[c][1];
					cubes[c][1] = temp;
					
					rotationAxe = Yaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 4)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][4] != -1)
				{
					temp = cubes[c][1];
					cubes[c][1] = cubes[c][3];
					cubes[c][3] = cubes[c][5];
					cubes[c][5] = cubes[c][2];
					cubes[c][2] = temp;
					
					rotationAxe = Zaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 5)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][5] != -1)
				{
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][2];
					cubes[c][2] = cubes[c][4];
					cubes[c][4] = cubes[c][3];
					cubes[c][3] = temp;
					
					rotationAxe = Xaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
	}
	
	void rotateCounterCW(int cube, int face)
	{
		int temp;
		int quad = getQuad(cube, face);
		if (quad == 0)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][0] != -1)
				{
					temp = cubes[c][1];
					cubes[c][1] = cubes[c][3];
					cubes[c][3] = cubes[c][5];
					cubes[c][5] = cubes[c][2];
					cubes[c][2] = temp;
					
					rotationAxe = Zaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 1)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][1] != -1)
				{
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][2];
					cubes[c][2] = cubes[c][4];
					cubes[c][4] = cubes[c][3];
					cubes[c][3] = temp;
					
					rotationAxe = Xaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 2)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][2] != -1)
				{
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][5];
					cubes[c][5] = cubes[c][4];
					cubes[c][4] = cubes[c][1];
					cubes[c][1] = temp;
					
					rotationAxe = Yaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 3)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][3] != -1)
				{
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][1];
					cubes[c][1] = cubes[c][4];
					cubes[c][4] = cubes[c][5];
					cubes[c][5] = temp;
					
					rotationAxe = Yaxis;
					rotating = -360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 4)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][4] != -1)
				{
					temp = cubes[c][1];
					cubes[c][1] = cubes[c][2];
					cubes[c][2] = cubes[c][5];
					cubes[c][5] = cubes[c][3];
					cubes[c][3] = temp;
					
					rotationAxe = Zaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
		else if (quad == 5)
		{
			for (int c = 0; c < 8; c++)
			{
				if (cubes[c][5] != -1)
				{
					//					printf("%d %d %d %d %d\n",c,cubes[c][0],cubes[c][2],cubes[c][3],cubes[c][4]);
					temp = cubes[c][0];
					cubes[c][0] = cubes[c][3];
					cubes[c][3] = cubes[c][4];
					cubes[c][4] = cubes[c][2];
					cubes[c][2] = temp;
					
					rotationAxe = Xaxis;
					rotating = 360;
					
					cubesToRotate[c] = true;
				}
				else
				{
					cubesToRotate[c] = false;
				}
			}
		}
	}
	
	mat4 rotation()
	{
		if (rotating < 0)
		{
			rotating+=rotationSpeed;
			if (rotationAxe == Xaxis)
			{
				return RotateX(-rotationSpeed);
			}
			else if (rotationAxe == Yaxis)
			{
				return RotateY(-rotationSpeed);
			}
			else if (rotationAxe == Zaxis)
			{
				return RotateZ(-rotationSpeed);
			}
		}
		else if (rotating > 0)
		{
			rotating-=rotationSpeed;
			if (rotationAxe == Xaxis)
			{
				return RotateX(rotationSpeed);
			}
			else if (rotationAxe == Yaxis)
			{
				return RotateY(rotationSpeed);
			}
			else if (rotationAxe == Zaxis)
			{
				return RotateZ(rotationSpeed);
			}
		}
		return mat4();
	}
	
	void shuffle()
	{
		remainingShuffle--;
		int cube = rand()%8;
		int face = rand()%6;
		int cw = rand()%2;
		if (cw == 0)
		{
			rotateCounterCW(cube, face);
		}
		else
		{
			rotateCW(cube, face);
		}
	}
};

// Rubik
struct Rubik _2x2;

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// generate cube
	cube.generateCube();
	_2x2.init();
	
	enableAxis = false;
	
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );
	currentCubeColor = glGetUniformLocation( program, "cubeColor" );
	
	// Create a vertex array object
	glGenVertexArrays( 2, vao );
	
	// create a cube
	glBindVertexArray( vao[0] );
	glGenBuffers( 1, &buffer[0]);
	glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube.vertices)+sizeof(cube.colors), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cube.vertices), cube.vertices );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cube.vertices), sizeof(cube.colors), cube.colors );
	
	// set up shader variables
	vPosition[0] = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition[0] );
	glVertexAttribPointer( vPosition[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	vColor[0] = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor[0] );
	glVertexAttribPointer( vColor[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube.vertices)));
	glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, mat4());
	
	// create a edges
	glBindVertexArray( vao[1] );
	glGenBuffers( 1, &buffer[1]);
	glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube.edges) + sizeof(cube.edgeColors), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cube.edges), cube.edges );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cube.edges), sizeof(cube.edgeColors), cube.edgeColors );
	
	vPosition[1] = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition[1] );
	glVertexAttribPointer( vPosition[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	vColor[1] = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor[1] );
	glVertexAttribPointer( vColor[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube.vertices)));
	
	// create a axis
	glBindVertexArray( vao[2] );
	glGenBuffers( 1, &buffer[2]);
	glBindBuffer( GL_ARRAY_BUFFER, buffer[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(axis.vertices) + sizeof(axis.colors), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(axis.vertices), axis.vertices );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(axis.vertices), sizeof(axis.colors), axis.colors );
	
	vPosition[2] = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition[2] );
	glVertexAttribPointer( vPosition[2], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	vColor[2] = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor[2] );
	glVertexAttribPointer( vColor[2], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(axis.vertices)));
	
	glLineWidth(5);

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
	glClearColor( 0.5, 0.5, 0.5, 1.0 );
	
	cout << "Mouse commands:" << endl;
	cout << "To rotate counter clockwise use mouse left button" << endl;
	cout << "To rotate clockwise use mouse right button" << endl;
	cout << "\nKeyboard commands:" << endl;
	cout << "Use the arrow keys to rotate the Rubik's Cube" << endl;
	cout << "s:\tshuffle" << endl;
	cout << "q:\tquit" << endl;
}

//----------------------------------------------------------------------------

void
display( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (_2x2.rotating == 0 && _2x2.remainingShuffle > 0)
	{
		_2x2.shuffle();
	}
	
	//  Generate tha model-view matrix 0
	for (int i = 0; i<8;i++)
	{
		if (_2x2.cubesToRotate[i])
		{
			model_view[i] = model_view[8] *
							_2x2.rotation() *
							model_view[9] *
							model_view[i];
		}
		
		glBindVertexArray( vao[0] );
		glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view[i] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
		glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);

		glBindVertexArray( vao[1] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[BLACK]);
		glDrawArrays( GL_LINES, 0, 24);
		
	}
	
	if (enableAxis)
	{
		glBindVertexArray( vao[2] );
		glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view[8] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
		glDrawArrays( GL_LINES, 0, axis.numberOfVertices);
	}
	
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
idle( void )
{
	/*
	if ( Theta[Axis] > 360.0 )
	{
		Theta[Axis] -= 360.0;
	}
	 */
	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key,int x, int y )
{
	// quits on 'q' command
	if(key == 'Q' | key == 'q')
		exit(0);
	if((key == 'S' | key == 's')
	   && _2x2.remainingShuffle <= 0)
	{
		printf("Shuffling... \n");
		_2x2.restart();
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void specialCallBack(int key, int x, int y)
{
	GLfloat Theta[NumAxes] = { 0, 0, 0 };
	
	// increase angular velocity
	if (key == GLUT_KEY_RIGHT)
		Theta[Yaxis]-=rotationSpeed;
	
	// decrease angular velocity
	if (key == GLUT_KEY_LEFT)
		Theta[Yaxis]+=rotationSpeed;
	
	// increase velocity
	if (key == GLUT_KEY_UP)
		Theta[Xaxis]-=rotationSpeed;
	
	// decrease velocity
	if (key == GLUT_KEY_DOWN)
		Theta[Xaxis]+=rotationSpeed;
	
	
	model_view[8] = (RotateX( Theta[Xaxis] ) *
					 RotateY( Theta[Yaxis] ) *
					 RotateZ( Theta[Zaxis] )) * model_view[8];
	
	for (int i = 0; i<8;i++)
	{
		model_view[i] = model_view[8] *
						model_view[9] *
						model_view[i];
	}
	
	model_view[9] = model_view[9] * (RotateZ( -Theta[Zaxis] ) *
					 RotateY( -Theta[Yaxis] ) *
					 RotateX( -Theta[Xaxis] ));
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y )
{
	int c; //cube
	int f; //face
	unsigned char pixel[4];
	
	if (state == GLUT_DOWN &&
		(button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
		&& _2x2.rotating == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray( vao[0] );
		for (int i = 0; i<8;i++)
		{
			glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view[i] );
			glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[i]);
			glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
		}
		
		// openGL coordinate system starts from bottom left, not top left
		glReadPixels(x,500-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		
		c = -1;
		
		if      (pixel[0] == 255 && pixel[1] == 140 && pixel[2] ==    0) // orange
		{
//			printf("orange\n");
			c = 0;
		}
		else if (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==    0) // red
		{
//			printf("red\n");
			c = 1;
		}
		else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] ==    0) // yellow
		{
//			printf("yellow\n");
			c = 2;
		}
		else if (pixel[0] ==  00 && pixel[1] == 255 && pixel[2] ==    0) // green
		{
//			printf("green\n");
			c = 3;
		}
		else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==  255) // blue
		{
//			printf("blue\n");
			c = 4;
		}
		else if (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==  255) // magenta
		{
//			printf("magenta\n");
			c = 5;
		}
		else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==    0) // black
		{
//			printf("black\n");
			c = 6;
		}
		else if (pixel[0] ==   0 && pixel[1] == 255 && pixel[2] ==  255) // cyan
		{
//			printf("cyan\n");
			c = 7;
		}
		
		if (c > -1)
		{
			// ekranı sıfırla
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view[c] );
			glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
			glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
			glReadPixels(x,500-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
//			printf("R: %d G: %d B: %d A: %d\n",pixel[0],pixel[1],pixel[2],pixel[3]);
			
			if      (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==    0) // red
			{
//				printf("red\n");
				f = 0;
			}
			else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==  255) // blue
			{
//				printf("blue\n");
				f = 1;
			}
			else if (pixel[0] ==  00 && pixel[1] == 255 && pixel[2] ==    0) // green
			{
//				printf("green\n");
				f = 2;
			}
			else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] ==    0) // yellow
			{
//				printf("yellow\n");
				f = 3;
			}
			else if (pixel[0] == 255 && pixel[1] == 140 && pixel[2] ==    0) // orange
			{
//				printf("orange\n");
				f = 4;
			}
			else if (pixel[0] ==   0 && pixel[1] == 255 && pixel[2] ==  255) // cyan
			{
//				printf("cyan\n");
				f = 5;
			}
			if (button == GLUT_LEFT_BUTTON)
				_2x2.rotateCW(c,f);
			else
				_2x2.rotateCounterCW(c,f);
		}
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, mat4());
	}
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
//		maxAllowedDept = float(h)/float(w);
	}
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 512, 512 );
	glutCreateWindow( "Rubik's Mini Cube" );
	
	init();
	
	glutDisplayFunc( display ); // set display callback function
	glutIdleFunc( idle );
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialCallBack);
	glutReshapeFunc(reshape);
	glutMouseFunc( mouse );
	
	glutMainLoop();
	return 0;
}
