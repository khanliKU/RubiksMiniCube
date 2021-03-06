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

// Declare Vertex Array Object Arrays and Buffer Arrays
GLuint vao[3];
GLuint buffer[3]; // try it

// Rotation animation speed
float rotationSpeed = 5;

int window[2] = {512,512};

// Model-view, projection and current c ube color matrices uniform location
GLuint  ModelView, Projection, currentCubeColor;

// Declare uniform variables to be passed to Shaders
GLuint vPosition[3], vColor[3];

// enumerate colors and create a color4 array as colors array
enum color {ORANGE,RED,YELLOW,GREEN,BLUE,MAGENTA,BLACK,CYAN,NEUTRAL};

// RGBA olors
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

//
/*
 * color castig array
 * These 4x4 matrices are used for the picking algorithm
 *
 */
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

// Axis enumeration
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };

//----------------------------------------------------------------------------


/*
 * Axis struct
 * contains i, j, k unit vectors with different colors.
 */
struct axis
{
	bool enableAxis;
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
	// Vertices of a cube centered at origin
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

/*
 * Rubik struct
 * Holds the necessary information about cube location and orientations
 * in a 2x2x2 Rubik's cube.
 */
struct Rubik
{
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
	
	// Initialize Model Views of the cubes [0-7]
	// General view orientation [8]
	// Inverse of general view orientation [9]
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
	
	/*
	 * THIS IS WHERE THE MAGIC HAPPENS
	 *
	 * cubes 2d array hold the information about which face of a given cube
	 * is facing which face of the Rubik's Cube
	 *
	 * Each line represents a cubes orientation.
	 * Each column represents a face of a cube.
	 * -1 means that face of the cube is not visible (facing inward in Rubik's Cube)
	 * cubes[0][1] = 2 means that 1st face of the 0th cube is facing 2nd face of the rubisk cube
	 * (not the case in initial conditions just an example)
	 */
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
	
	int initialCubes[8][6] ={
		{ 0, -1,  2, -1, -1,  5},
		{ 0, -1, -1,  3, -1,  5},
		{ 0,  1, -1,  3 ,-1, -1},
		{ 0,  1,  2, -1, -1, -1},
		{-1, -1,  2, -1,  4,  5},
		{-1, -1, -1,  3,  4,  5},
		{-1,  1, -1,  3,  4, -1},
		{-1,  1,  2, -1,  4, -1}
	};
	
	// Remaining rotation in degrees
	// implemented this way because of animation issues
	int rotating = 0;
	
	// cubes to rotate on a user command
	bool cubesToRotate[8] = {false,false,false,false,false,false,false,false};
	
	// rotation axe specified by the user
	int rotationAxe;
	
	// number of random rotations remaining
	int remainingShuffle;
	
	// initializer
	void init()
	{
		model_view[8] = mat4();
		model_view[9] = mat4();
		remainingShuffle = 0;
	}
	
	// sets remainingShuffle to 50
	void restart()
	{
		remainingShuffle = 50;
	}
	
	// given cube and face returns which face of the Rubik's Cube is selected
	int getQuad(int c, int f)
	{
		for (int i=0; i<6 ; i++)
		{
			if (cubes[c][i] == f)
				return i;
		}
		return -1;
	}
	
	/*
	 * Clockwise rotation algorithm
	 *
	 * Uses getQuad to determine which face is clicked
	 * Has a different sequence for all 6 faces of the Rubik's cube.
	 * Modifies cubes 2x2 array, cubesToRotate and rotating as accordingly
	 */
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
	
	/*
	 * Counter clockwise rotation algorithm
	 *
	 * Uses getQuad to determine which face is clicked
	 * Has a different sequence for all 6 faces of the Rubik's cube.
	 * Modifies cubes 2x2 array, cubesToRotate and rotating as accordingly
	 */
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
	
	// returns the appropriate rotation matrix for rotating and rotation axis parameters.
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
	
	// generates random rotations
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
	
	// resets the Rubik's Cube to initial state
	void reset()
	{
		if (rotating == 0 && remainingShuffle == 0)
		{
			for (int c = 0; c < 8 ; c++)
			{
				for (int f = 0; f < 6 ; f++)
				{
					cubes[c][f] = initialCubes[c][f];
				}
				model_view[c] = model_view[8] * Translate(centerOfGs[c]);
			}
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
	
	// initialize Rubik's Cube
	_2x2.init();
	
	// enable or disable Axis
	axis.enableAxis = false;
	
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );
	currentCubeColor = glGetUniformLocation( program, "cubeColor" );
	
	// Create a vertex array object
	glGenVertexArrays( 2, vao );
	
	// --------------------------------------------------------------------------
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
	
	// --------------------------------------------------------------------------
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
	
	// --------------------------------------------------------------------------
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
	
	// set linewidth
	glLineWidth(5);

	// Set current program object
	glUseProgram( program );
	
	// Set projection matrix
	mat4  projection;
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
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
	cout << "r:\treset" << endl;
	cout << "q:\tquit" << endl;
}

//----------------------------------------------------------------------------

void
display( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// perform rotation if needed
	if (_2x2.rotating == 0 && _2x2.remainingShuffle > 0)
	{
		_2x2.shuffle();
	}
	
	// Generate tha model-view matrix i for cube i
	// display cube
	for (int i = 0; i<8;i++)
	{
		if (_2x2.cubesToRotate[i])
		{
			_2x2.model_view[i] = _2x2.model_view[8] *
							_2x2.rotation() *
							_2x2.model_view[9] *
							_2x2.model_view[i];
		}
		
		glBindVertexArray( vao[0] );
		glUniformMatrix4fv( ModelView, 1, GL_TRUE, _2x2.model_view[i] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
		glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);

		glBindVertexArray( vao[1] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[BLACK]);
		glDrawArrays( GL_LINES, 0, 24);
		
	}
	
	// display axis
	if (axis.enableAxis)
	{
		glBindVertexArray( vao[2] );
		glUniformMatrix4fv( ModelView, 1, GL_TRUE, _2x2.model_view[8] );
		glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
		glDrawArrays( GL_LINES, 0, axis.numberOfVertices);
	}
	
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
idle( void )
{
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key,int x, int y )
{
	// quits on 'q' command
	if(key == 'Q' | key == 'q')
	{
		exit(0);
	}
	else if((key == 'S' | key == 's')
	   && _2x2.remainingShuffle <= 0)
	{
		printf("Shuffling... \n");
		_2x2.restart();
	} else if (key == 'R' | key == 'r')
	{
		_2x2.reset();
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void specialCallBack(int key, int x, int y)
{
	// Array of rotation angles (in degrees) for each coordinate axis
	GLfloat Theta[NumAxes] = { 0, 0, 0 };
	
	// increase angular velocity
	if (key == GLUT_KEY_RIGHT)
		Theta[Yaxis]+=rotationSpeed;
	
	// decrease angular velocity
	if (key == GLUT_KEY_LEFT)
		Theta[Yaxis]-=rotationSpeed;
	
	// increase velocity
	if (key == GLUT_KEY_UP)
		Theta[Xaxis]-=rotationSpeed;
	
	// decrease velocity
	if (key == GLUT_KEY_DOWN)
		Theta[Xaxis]+=rotationSpeed;
	
	// perform rotation accordingly
	
	_2x2.model_view[8] = (RotateX( Theta[Xaxis] ) *
						  RotateY( Theta[Yaxis] ) *
						  RotateZ( Theta[Zaxis] )) * _2x2.model_view[8];
	
	for (int i = 0; i<8;i++)
	{
		_2x2.model_view[i] = _2x2.model_view[8] *
							_2x2.model_view[9] *
							_2x2.model_view[i];
	}
	
	_2x2.model_view[9] = _2x2.model_view[9] * (RotateZ( -Theta[Zaxis] ) *
											   RotateY( -Theta[Yaxis] ) *
											   RotateX( -Theta[Xaxis] ));
}

//----------------------------------------------------------------------------

/*
 * Picking algorithm
 
 * Uses colorCast array and currentCubeColor uniform location.
 * When user clicks on the cube first draws all cubes in a different collor
 * then gets the pixel that is clicked, checks its color.
 * if that color is a valid color then only draws that cube with different colors
 * on each face. Then gets the pixel with the same location. Checks the color
 * and calls rotates:
 * Counter clockwise if left button
 * Clockwise if the right button.
 */

void mouse( int button, int state, int x, int y )
{
	int c = -1; //cube
	int f = -1; //face
	unsigned char pixel[4];
	
	if (state == GLUT_DOWN &&
		(button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
		&& _2x2.rotating == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray( vao[0] );
		for (int i = 0; i<8;i++)
		{
			glUniformMatrix4fv( ModelView, 1, GL_TRUE, _2x2.model_view[i] );
			glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[i]);
			glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
		}
		
		// openGL coordinate system starts from bottom left, not top left
		glReadPixels(x, window[1] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		
		if      (pixel[0] == 255 && pixel[1] == 140 && pixel[2] ==    0) // orange
		{
			c = 0;
		}
		else if (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==    0) // red
		{
			c = 1;
		}
		else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] ==    0) // yellow
		{
			c = 2;
		}
		else if (pixel[0] ==  00 && pixel[1] == 255 && pixel[2] ==    0) // green
		{
			c = 3;
		}
		else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==  255) // blue
		{
			c = 4;
		}
		else if (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==  255) // magenta
		{
			c = 5;
		}
		else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==    0) // black
		{
			c = 6;
		}
		else if (pixel[0] ==   0 && pixel[1] == 255 && pixel[2] ==  255) // cyan
		{
			c = 7;
		}
		
		if (c > -1)
		{
			// ekranı sıfırla
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniformMatrix4fv( ModelView, 1, GL_TRUE, _2x2.model_view[c] );
			glUniformMatrix4fv( currentCubeColor, 1,GL_TRUE, colorCast[NEUTRAL]);
			glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
			glReadPixels(x,window[1] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
			
			if      (pixel[0] == 255 && pixel[1] ==   0 && pixel[2] ==    0) // red
			{
				f = 0;
			}
			else if (pixel[0] ==   0 && pixel[1] ==   0 && pixel[2] ==  255) // blue
			{
				f = 1;
			}
			else if (pixel[0] ==  00 && pixel[1] == 255 && pixel[2] ==    0) // green
			{
				f = 2;
			}
			else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] ==    0) // yellow
			{
				f = 3;
			}
			else if (pixel[0] == 255 && pixel[1] == 140 && pixel[2] ==    0) // orange
			{
				f = 4;
			}
			else if (pixel[0] ==   0 && pixel[1] == 255 && pixel[2] ==  255) // cyan
			{
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
	
	window[0] = w;
	window[1] = h;
	
	mat4  projection;
	if (w <= h)
		projection = Ortho(-1.0, 1.0, -1.0 * (GLfloat) h / (GLfloat) w,
						   1.0 * (GLfloat) h / (GLfloat) w, -1.0, 1.0);
	else  projection = Ortho(-1.0* (GLfloat) w / (GLfloat) h, 1.0 *
							 (GLfloat) w / (GLfloat) h, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( window[0], window[1] );
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
