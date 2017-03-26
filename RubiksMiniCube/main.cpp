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
GLuint vao[8];
GLuint buffer[8]; // try it

// Model-view and projection matrices uniform location
GLuint  ModelView, ModelViews[9], Projection;
mat4  model_view;

// Declare uniform cariable to be passed to Fragment Shader
GLuint currentCubeColor;
int currentCube;

// Initialize Center of Gravity of the object
vec3 centerOfGs[8] = {
	vec3(-0.25, -0.25 ,  0.25),
	vec3(-0.25,  0.25 ,  0.25),
	vec3( 0.25,  0.25 ,  0.25),
	vec3( 0.25, -0.25 ,  0.25),
	vec3(-0.25, -0.25 , -0.25),
	vec3(-0.25,  0.25 , -0.25),
	vec3( 0.25,  0.25 , -0.25),
	vec3( 0.25, -0.25 , -0.25)
};
// RGBA olors

// enumerate colors and create a color4 array as colors array
enum color {ORANGE,RED,YELLOW,GREEN,BLUE,MAGENTA,WHITE,CYAN,BLACK};
color4 color[9] = {
	color4( 1.0, 0.549, 0.0, 1.0 ),  // orange
	color4( 1.0, 0.0, 0.0, 1.0 ),    // red
	color4( 1.0, 1.0, 0.0, 1.0 ),    // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),    // green
	color4( 0.0, 0.0, 1.0, 1.0 ),    // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),    // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),    // white
	color4( 0.0, 1.0, 1.0, 1.0 ),    // cyan
	color4( 0.0, 0.0, 0.0, 1.0 )     // black
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Yaxis;

GLfloat  Theta[NumAxes] = { 0, 0, 0 }; // ??

//----------------------------------------------------------------------------

/* color menu function
 * changes color on user request
 * 8 predefined colors and a random color
 */

// struct CUBE
struct cube
{
	int numberOfVertices = 36;
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
struct cube cube;

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// generate cube
	cube.generateCube();
	
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );
	
	// Create a vertex array object
	glGenVertexArrays( 8, vao );
	// Bind vao for SPHERE
	glBindVertexArray( vao[0] );
	// Initialize a buffer object
	glGenBuffers( 1, &buffer[0]);
	glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube.vertices)+sizeof(cube.colors), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cube.vertices), cube.vertices );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cube.vertices), sizeof(cube.colors), cube.colors );
	
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	GLuint vColor = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube.vertices)));
	
	
	
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
	
	//  Generate tha model-view matrix 0
	model_view = (RotateX( Theta[Xaxis] ) *
						RotateY( Theta[Yaxis] ) *
						RotateZ( Theta[Zaxis] ) *
						Translate( centerOfGs[0] ));
	glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
	glBindVertexArrayAPPLE(vao[0]);
	glDrawArrays( GL_TRIANGLES, 0, cube.numberOfVertices);
	
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
idle( void )
{
	if ( Theta[Axis] > 360.0 )
	{
		Theta[Axis] -= 360.0;
	}
	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key,int x, int y )
{
	printf("test");
	// quits on 'q' command
	if(key == 'Q' | key == 'q')
		exit(0);
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
		Theta[Zaxis]-=4;
	
	// decrease angular velocity
	if (key == GLUT_KEY_LEFT)
		Theta[Zaxis]+=4;
	
	// increase velocity
	if (key == GLUT_KEY_UP)
		Theta[Xaxis]-=4;
	
	// decrease velocity
	if (key == GLUT_KEY_DOWN)
		Theta[Xaxis]+=4;
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
	
	glutMainLoop();
	return 0;
}
