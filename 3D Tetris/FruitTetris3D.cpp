/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/
#include "include/Angel.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;

/////////////////////////Addignment2 Add/////////////////////////////
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;
const int NumVertices = 36;
point4 points[NumVertices];
color4 colors[NumVertices];
vec3 arm_pos = vec3(-8, 0 ,0);

// Viewing transformation parameters
mat4 Model;		
mat4 View;  	
mat4 Projection;
GLuint MatrixNum;
mat4  m_view;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int      Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };
/////////////////////Addignment2 Add/////////////////////////////////

// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int speed;
int xsize = 900; 
int ysize = 1220;
bool freez = true;
bool isInside = true;
bool fall = false;
bool drop = false;
enum Text {
	rem_time,text_max
};
float gui[text_max];


// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilepos = vec2(5, 19); // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
vec2 newtilepos;
vec2 tPosition;
int curr_tile = 0;
int curr_Ishape = 0;
int curr_Sshape = 0;
int curr_Lshape = 0;
	
// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
vec2 allRotationsLshape[4][4] = 
	{{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(-1,-1)},//4
	{vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(1, -1)},//5     
	{vec2(1, 1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},//6  
	{vec2(-1,1), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};//7

vec2 allRotationsSshape[2][4] = 
	{{vec2(0, 0), vec2(0,-1), vec2(1, 0), vec2(-1,-1)},//2
	{vec2(0, 1), vec2(0, 0), vec2(1, 0), vec2(1, -1)}};//3

vec2 allRotationsIshape[2][4] = 
	{{vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2(1, 0)},//0
	{vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(0, -2)}};//1

// colors
vec4 white  = vec4(0.7, 0.7, 0.7, 0.7);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0); 
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); //1
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0); //2
vec4 green  = vec4(0.0, 1.0, 0.0, 1.0); //3
vec4 red    = vec4(1.0, 0.0, 0.0, 1.0); //4
vec4 purple = vec4(1.0, 0.0, 1.0, 1.0); //5
vec4 grey   = vec4(0.4, 0.4, 0.4, 0.4);
vec4 newcolours[6*24];
vec4 isGrey[6*24];

//board[x][y] represents whether the cell (x,y) is occupied
bool board[20][10]; 
int mcolor[20][10];

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[7200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vao;
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)
/////////////////////////Addignment2 Add/////////////////////////////
///////////////////////import for example///////////////////////////
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white right
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white up
    color4( 0.6, 0.0, 0.0, 0.6 ),  // white down
};

// Parameters controlling the size of the Robot's arm
#define MAX_TIME 5
const GLfloat BASE_HEIGHT      = 1.5;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 11.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 11.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;
const GLfloat BOARD_WIDTH = 10;

int Index = 0;
void
quad( int a, int b, int c, int d )
{
	// Modified to make the arm grey
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void base(mat4 transform)
{
    mat4 instance = ( Translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ) *
		 Scale( BASE_WIDTH,
				BASE_HEIGHT,
				BASE_WIDTH ) );

    glUniformMatrix4fv( MatrixNum, 1, GL_TRUE, transform * m_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

void upper_arm(mat4 transform)
{
    mat4 instance = ( Translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) *
		      Scale( UPPER_ARM_WIDTH,
			     	UPPER_ARM_HEIGHT,
			     	UPPER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( MatrixNum, 1, GL_TRUE, transform * m_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

void lower_arm(mat4 transform)
{
    mat4 instance = ( Translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) *
		      Scale( LOWER_ARM_WIDTH,
			     	LOWER_ARM_HEIGHT,
			     	LOWER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( MatrixNum, 1, GL_TRUE, transform * m_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

void
initRobot( void )
{
    colorcube();
    // Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" ); //removed the 81 at the end of the names since we have different shader names
    glUseProgram( program );
    
  //  GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

 //   GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );
}
/////////////////////////Addignment2 Add/////////////////////////////
///////////////////////import for example///////////////////////////

vec2 tilPos() {
	vec2 til;
	float math_pi = 3.1415926535898;
	
	til.x = -cos(math_pi/180 * (180/2 - (Theta[UpperArm] + Theta[LowerArm]))) * UPPER_ARM_HEIGHT
                - sin(math_pi/180 * Theta[LowerArm]) * LOWER_ARM_HEIGHT + arm_pos.x/2.3;	 
	til.y = sin(math_pi/180 * (180/2 - (Theta[UpperArm] + Theta[LowerArm]))) * UPPER_ARM_HEIGHT
				+ cos((-math_pi/180) * Theta[LowerArm]) * LOWER_ARM_HEIGHT + arm_pos.y + BASE_HEIGHT;
	til.x = int(til.x);
	til.y = int(til.y);

	return til;
}

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 
	if(freez)
		tilepos = tilPos();

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x; 
		GLfloat y = tilepos.y + tile[i].y;

		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), 16.5, 1); // bottom left
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), 16.5, 1); // top left
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), 16.5, 1); // bottom right
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), 16.5, 1); // top right

		vec4 p5 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), -16.5, 1); // bottom left
		vec4 p6 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), -16.5, 1); // top left
		vec4 p7 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), -16.5, 1); // bottom right
		vec4 p8 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), -16.5, 1); // top right

		// Two points are used by two triangles each
		vec4 newpoints[36] = {p1, p2, p3, p2, p3, p4, 
							  p3, p4, p7, p4, p7, p8, 
							  p5, p6, p7, p6, p7, p8, 
							  p1, p2, p5, p2, p6, p5, 
							  p1, p5, p3, p5, p7, p3, 
							  p2, p6, p4, p6, p8, p4, 
							 };  
		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, 36*i*sizeof(vec4), 36*sizeof(vec4), newpoints); 
	}

	tPosition.x = tilepos.x;
	tPosition.y = tilepos.y;
	int a = 19-int(tilepos.y);
	int b = int(tilepos.x);
	if(curr_tile == 0){
		if ( tPosition.x < 2 || tPosition.x > 8 || tPosition.y < 0 || tPosition.y > 19 || 
			board[a+1-1][b]==true || board[a+1-1][b-1] == true || board[a+1-1][b-2] == true || board[a+1-1][b+1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}else if(curr_tile == 1){
		if ( tPosition.x < 0 || tPosition.x > 9 || tPosition.y < 2 || tPosition.y > 18 || 
			board[a+1-1][b]==true || board[a+1-1][b-1] == true || board[a+1-1][b-2] == true || board[a+1-1][b+1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}else if(curr_tile == 2){
		if ( tPosition.x < 1 || tPosition.x > 8 || tPosition.y < 1 || tPosition.y > 19 || 
			board[a+1-1][b]==true || board[a+1-1][b+1] == true || board[a+1+1-1][b] == true || board[a+1+1-1][b-1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}else if(curr_tile == 3){
		if ( tPosition.x < 0 || tPosition.x > 8 || tPosition.y < 1 || tPosition.y > 18 || 
			board[a+1-1][b]==true || board[a-1+1-1][b] == true || board[a+1-1][b+1] == true || board[a+1+1-1][b+1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}else if(curr_tile == 4){
		if ( tPosition.x < 1 || tPosition.x > 8 || tPosition.y < 1 || tPosition.y > 19 || 
			board[a+1-1][b]==true || board[a+1-1][b+1] == true || board[a+1-1][b-1] == true || board[a+1+1-1][b-1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}else if(curr_tile == 5){
		if ( tPosition.x < 0 || tPosition.x > 8 || tPosition.y < 1 || tPosition.y > 18 ||
			board[a+1-1][b]==true || board[a-1+1-1][b] == true || board[a+1+1-1][b] == true || board[a+1+1-1][b+1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}else if(curr_tile == 6){
		if ( tPosition.x < 1 || tPosition.x > 8 || tPosition.y < 0 || tPosition.y > 18 || 
			board[a+1-1][b]==true || board[a+1-1][b+1] == true || board[a+1-1][b-1] == true || board[a-1+1-1][b+1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}else{
		if ( tPosition.x < 1 || tPosition.x > 9 || tPosition.y < 1 || tPosition.y > 18 || 
			board[a+1-1][b]==true || board[a+1+1-1][b] == true || board[a-1+1-1][b] == true || board[a-1+1-1][b-1] == true){
			for (int i = 0; i < 6*24; i++)
					isGrey[i] = grey;
			isInside = false;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(isGrey), isGrey); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else{
			isInside = true;
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	glBindVertexArray(0);
}

void checkempty(){

	int count = 0;
	vec4 curr_color[36];

	for(int i=18; i>-1; i--){
		for(int j=0; j<10; j++){
			count=0;
			int tmp = (19-i)*360+j*36;
			for(int r=0; r<36; r++){
				curr_color[r] = boardcolours[tmp+r];
			}
			if(board[i][j]){
				if(!board[i+1][j]){	
					for(int l=0; l<19-i; l++){
						if(!board[i+1+l][j]){
							count+=1;
						}
					}
					for(int k=0; k<36; k++){
						boardcolours[k+tmp] = black;
					}
					int han = 360*count;
					for(int u=0; u<36; u++){
						boardcolours[tmp-han+u] = curr_color[u];
					}
					board[i][j] = false;
					board[i+count][j] = true;
				}
			}
		}
		
	}

}

bool gameover(){
	int count = 0;
	for(int i=0; i<10; i++){
		if(board[0][i]){
			count++;
		}
	}
	if (count > 0){
		return true;
	}else{
		return false;
	}
}

// Called at the start of play and every time a tile is placed
void newtile()
{
	if(!isInside){
		return;
	}

	tilepos = tilPos();

	// Update the geometry VBO of current tile
	int ranTile = rand()%8;//Random pick a number
	// #2 S-0 tile
	if(ranTile == 2){
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsSshape[0][i];
		updatetile(); 
		curr_tile = 2;
		curr_Sshape = 0;
	// #3 S-1 tile
	}else if(ranTile == 3){
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsSshape[1][i]; 
		updatetile(); 
		curr_tile = 3;
		curr_Sshape = 1;
	// #4 L-0 tile
	}else if(ranTile == 4){
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[0][i]; 
		updatetile(); 
		curr_tile = 4;
		curr_Lshape = 0;
	// #5 L-1 tile
	}else if(ranTile == 5){
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[1][i]; 
		updatetile(); 
		curr_tile = 5;
		curr_Lshape = 1;
	// #6 L-2 tile
	}else if(ranTile == 6){
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[2][i]; 
		updatetile(); 
		curr_tile = 6;
		curr_Lshape = 2;
	// #7 L-3 tile
	}else{
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[3][i]; 
		updatetile(); 
		curr_tile = 7;
		curr_Lshape = 3;
	}
	
	// Update the color VBO of current tile
	//Random pick a number
		int color;
		for (int i = 0; i < 6*24; i++){
			if(i+1==1||i+1==37||i+1==73||i+1==109){
			color = rand()%5;
			}
			if(color == 0){
				newcolours[i] = orange;
			}else if(color == 1){
				newcolours[i] = green;		
			}else if(color == 2){
				newcolours[i] = red;		
			}else if(color ==3){
				newcolours[i] = yellow;		
			}else{
				newcolours[i] = purple;
			}
		}
	

	
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[590]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[590]; // One colour per vertex

	// Vertical lines (front)
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 16.50, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 16.50, 1);
	}
	
	// Vertical lines (back)
	for (int i = 0; i < 11; i++){
		gridpoints[64+2*i] = vec4((33.0 + (33.0 * i)), 33.0, -16.50, 1);
		gridpoints[65+2*i] = vec4((33.0 + (33.0 * i)), 693.0, -16.50, 1);
	}

	// Horizontal lines (front)
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 16.50, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 16.50, 1);
	}

	
	// Horizontal lines (back)
	for (int i = 0; i < 21; i++){
		gridpoints[64 + 22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), -16.50, 1);
		gridpoints[65 + 22 + 2*i] = vec4(363.0, (33.0 + (33.0 * i)), -16.50, 1);
	}
	

	// Add lines to connect the two boards
	for (int i = 0; i < 20 + 1; i++){
		for (int k = 0; k < 10 + 1; k++) {
			gridpoints[2*(64 + 11*i + k) + 0] = vec4(33.0 + (k * 33.0), 33.0 + (i * 33.0), 16.50, 1);
			gridpoints[2*(64 + 11*i + k) + 1] = vec4(33.0 + (k * 33.0), 33.0 + (i * 33.0), -16.50, 1);
		}
	}

	// Make all grid lines white
	for (int i = 0; i < 590; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, (590)*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, (590)*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}

void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[7200];
	for (int i = 0; i < 7200; i++)
		boardcolours[i] = black;

	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), 16.50, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), 16.50, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), 16.50, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), 16.50, 1);

			vec4 p5 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), -16.50, 1);
			vec4 p6 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), -16.50, 1);
			vec4 p7 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), -16.50, 1);
			vec4 p8 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), -16.50, 1);

			
			int count = 0; // using this we move to the next point in the loop from 0 to 35 (6*6 = 36)
			
			// Front
			boardpoints[36*(10*i + j) + count] = p1; count++;
			boardpoints[36*(10*i + j) + count] = p2; count++;
			boardpoints[36*(10*i + j) + count] = p3; count++;
			boardpoints[36*(10*i + j) + count] = p2; count++;
			boardpoints[36*(10*i + j) + count] = p3; count++;
			boardpoints[36*(10*i + j) + count] = p4; count++;

			// Back
			boardpoints[36*(10*i + j) + count] = p5; count++;
			boardpoints[36*(10*i + j) + count] = p6; count++;
			boardpoints[36*(10*i + j) + count] = p7; count++;
			boardpoints[36*(10*i + j) + count] = p6; count++;
			boardpoints[36*(10*i + j) + count] = p7; count++;
			boardpoints[36*(10*i + j) + count] = p8; count++;

			// Left side
			boardpoints[36*(10*i + j) + count] = p1; count++;
			boardpoints[36*(10*i + j) + count] = p2; count++;
			boardpoints[36*(10*i + j) + count] = p5; count++;
			boardpoints[36*(10*i + j) + count] = p2; count++;
			boardpoints[36*(10*i + j) + count] = p5; count++;
			boardpoints[36*(10*i + j) + count] = p6; count++;

			// Right side
			boardpoints[36*(10*i + j) + count] = p3; count++;
			boardpoints[36*(10*i + j) + count] = p4; count++;
			boardpoints[36*(10*i + j) + count] = p7; count++;
			boardpoints[36*(10*i + j) + count] = p4; count++;
			boardpoints[36*(10*i + j) + count] = p7; count++;
			boardpoints[36*(10*i + j) + count] = p8; count++;

			// Top
			boardpoints[36*(10*i + j) + count] = p2; count++;
			boardpoints[36*(10*i + j) + count] = p4; count++;
			boardpoints[36*(10*i + j) + count] = p6; count++;
			boardpoints[36*(10*i + j) + count] = p4; count++;
			boardpoints[36*(10*i + j) + count] = p6; count++;
			boardpoints[36*(10*i + j) + count] = p8; count++;

			// Bottom
			boardpoints[36*(10*i + j) + count] = p1; count++;
			boardpoints[36*(10*i + j) + count] = p3; count++;
			boardpoints[36*(10*i + j) + count] = p5; count++;
			boardpoints[36*(10*i + j) + count] = p3; count++;
			boardpoints[36*(10*i + j) + count] = p5; count++;
			boardpoints[36*(10*i + j) + count] = p7; count++;

			
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 

	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 6*24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 6*24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void init()
{
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");
	MatrixNum = glGetUniformLocation(program, "v_MVP");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();
	initRobot();

	// Robot arm angles
	Theta[LowerArm] = -10; // Give the arm its initial angles
	Theta[UpperArm] = -55;

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");
	gui[rem_time] = MAX_TIME;

    // Set up camera
	vec3 eye	= vec3(-8, 25, 28);
	vec3 center = vec3(0, 10, 0);
	vec3 up 	= vec3(0, 1, 0);
	View = LookAt(eye, center, up);

	// Game initialization
	newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

// Rotates the current tile, if there is room
void rotate()
{  
	if(curr_tile == 0 || curr_tile == 1){
		if(curr_Ishape == 0){
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsIshape[1][i];
			updatetile();
			curr_tile = 1;
			curr_Ishape = 1;
		}else{
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsIshape[0][i];
			updatetile();
			curr_tile = 0;
			curr_Ishape = 0;
		}
	}else if(curr_tile == 2 || curr_tile == 3){
		if(curr_Sshape == 0){
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsSshape[1][i];
			updatetile();
			curr_tile = 3;
			curr_Sshape = 1;
		}else{
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsSshape[0][i];
			updatetile();
			curr_tile = 2;
			curr_Sshape = 0;
		}
	}else{
		if(curr_Lshape == 0){
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsLshape[1][i];
			updatetile();
			curr_tile = 5;
			curr_Lshape = 1;
		}else if(curr_Lshape == 1){
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsLshape[2][i];
			updatetile();
			curr_tile = 6;
			curr_Lshape = 2;
		}else if(curr_Lshape == 2){
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsLshape[3][i];
			updatetile();
			curr_tile = 7;
			curr_Lshape = 3;
		}else{
			for (int i = 0; i < 4; i++)
				tile[i] = allRotationsLshape[0][i];
			updatetile();
			curr_tile = 4;
			curr_Lshape = 0;
		}	
	}   	
}

bool compare(vec4 a, vec4 b){
	if(a[0]==b[0] && a[1]==b[1] && a[2]==b[2] && a[3]==b[3]){
		return true;
	}else{
		return false;
	}
}

void scancolor(){
	//init mcolor
	for(int i=0; i<20; i++){
		for(int j=0; j<10; j++){
			mcolor[i][j] = 0;
		}
	}
	for(int i=0; i<20; i++){
		for(int j=0; j<10; j++){
			int tmp = (19-i)*60+j*6+3;

			if(compare(boardcolours[tmp],orange)){
				mcolor[i][j] = 1;
			}else if(compare(boardcolours[tmp],yellow)){
				mcolor[i][j] = 2;
			}else if(compare(boardcolours[tmp],green)){
				mcolor[i][j] = 3;
			}else if(compare(boardcolours[tmp],red)){
				mcolor[i][j] = 4;
			}else if(compare(boardcolours[tmp],purple)){
				mcolor[i][j] = 5;
			}else{
			}

		}
	}
}

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
bool checkfullrow()
{
	checkempty();
	for(int i=0; i<20; i++){
		int checkx =0;
		for(int j=0; j<10; j++){
			if(board[i][j])
				checkx++;		
		}
		if(checkx==10){
			for(int k=0; k<10; k++){
					
					board[i][k] = false;
			}
			updatetile();
			for (int l = (19-i)*360 ; l < (19-i)*360+360; l++){
				boardcolours[l] = black;
			}
			checkempty();
			glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
			glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(vColor);
			updatetile();	
		}
	}
}

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool movetile(vec2 direction)
{
	if(tilepos.y > 3){
		tilepos = tilepos+direction;
		return true;
	}else{
		return false;
	}
}

void moveleft(){
	int tmpx = int(tilepos.x);
	int tmpy = 19-int(tilepos.y);
	if(curr_tile == 0){
		if(tilepos.x == 2 || board[tmpy][tmpx-3])
			return;
	}else if(curr_tile == 1){
		if(tilepos.x == 0 || board[tmpy][tmpx-1])
			return;
	}else if(curr_tile == 2){
		if(tilepos.x == 1 || board[tmpy][tmpx-2])
			return;
	}else if(curr_tile == 3){
		if(tilepos.x == 0 || board[tmpy][tmpx-1])
			return;
	}else if(curr_tile == 4){
		if(tilepos.x == 1 || board[tmpy][tmpx-2])
			return;
	}else if(curr_tile == 5){
		if(tilepos.x == 0 || board[tmpy][tmpx-1])
			return;
	}else if(curr_tile == 6){
		if(tilepos.x == 1 || board[tmpy][tmpx-2])
			return;
	}else{
		if(tilepos.x == 1 || board[tmpy][tmpx-2])
			return;
	}
	tilepos.x -= 1.0;
	updatetile();
}

void moveright(){
	int tmpx = int(tilepos.x);
	int tmpy = 19-int(tilepos.y);
	if(curr_tile == 0){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else if(curr_tile == 1){
		if(tilepos.x == 9 || board[tmpy][tmpx+1])
			return;
	}else if(curr_tile == 2){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else if(curr_tile == 3){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else if(curr_tile == 4){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else if(curr_tile == 5){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else if(curr_tile == 6){
		if(tilepos.x == 8 || board[tmpy][tmpx+2])
			return;
	}else{
		if(tilepos.x == 9 || board[tmpy][tmpx+1])
			return;
	}

	tilepos.x += 1.0;
	updatetile();
}

// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{
	initBoard();
	for(int k=0; k<20; k++){
		for(int h=0; h<10; h++){
			board[k][h]=false;
		}
	}
}

// Draw Text
template<class Text>
void drawText(Text s_string, float a, float b) {
	drop = false;
	stringstream s_time(s_string);
	glRasterPos2f(a, b);
	char cha;
	while(s_time >> cha) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cha);
}

// Draws the game
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat left = -11.5, right = 11.5, bottom = -15, top = 15, zNear = 47, zFar = -28; 
	Projection = Ortho( left, right, bottom, top, zNear, zFar );

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);
	
	mat4 MVP_Rob = Projection * View * Translate(arm_pos);
	
	// import from example1.cpp
	glBindVertexArray(vao);

	m_view = RotateY(Theta[Base] );
    base(MVP_Rob); // Added a parameter to the robot parts functions to make multiplication easier

    m_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) * RotateZ(Theta[LowerArm]) );
    lower_arm(MVP_Rob);

    m_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) * RotateZ(Theta[UpperArm]) );
    upper_arm(MVP_Rob);

	m_view *= Translate(0.0, UPPER_ARM_HEIGHT, 0.0);

	//board
	mat4 Model = mat4();
	Model *= ( Translate(0, 10, 0) * Scale(1.0/31, 1.0/31, 1.0/31) * Translate(-200, -363, 0) );
	mat4 Mod_view_pro = Projection * View * Model;
	glUniformMatrix4fv(MatrixNum, 1, GL_TRUE, Mod_view_pro);

	//show text
	if(gui[rem_time] >= 0) {
		gui[rem_time] -= 1.0/60;
	}else{
		gui[rem_time] = 5;
		isInside = true;
		if(freez){
			newtile();
		}
	}
	stringstream s_time;
	s_time << "TimeRemaining: " << gui[rem_time] << " Seconds";
	drawText(s_time.str(), -0.28, 0.90);

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 6*24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	for(int i=0; i<7200; i+=36){
		if(boardcolours[i].x == 0.0 && boardcolours[i].y == 0.0 && boardcolours[i].z == 0.0){

		}else{
			glDrawArrays(GL_TRIANGLES, i, 36);
		}
	}

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 590); // Draw the grid lines (21+11 = 32 lines)

	glutSwapBuffers();
}

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

void shuffle(){
	vec2 tmp = tile[0];
	tile[0]=tile[1];
	tile[1]=tile[2];
	tile[2]=tile[3];
	tile[3]=tmp;

	updatetile();
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void idle(void)
{
	glutPostRedisplay();
}

void settile()
{
	gui[rem_time] = 5;
	if (!isInside && freez){
		return;
	}

	int a = 19-int(tilepos.y);
	int b = int(tilepos.x);
	int tmp = int(tilepos.y)*360 + int(tilepos.x)*36;
	checkempty();
	
	if(curr_tile == 0){
		board[a][b] = true;
		board[a][b-1] = true;
		board[a][b-2] = true;
		board[a][b+1] =true;
		int q = 0;
		for (int i = tmp-72; i < tmp+36-72; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-36; i < tmp+36-36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp+36; i < tmp+36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else if(curr_tile == 1){
		board[a][b] = true;
		board[a+1][b] = true;
		board[a+2][b] = true;
		board[a-1][b] = true;
		int q = 0;
		for (int i = tmp+360; i < tmp+36+360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		
		for (int i = tmp-360; i < tmp+36-360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-720; i < tmp+36-720; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else if(curr_tile == 2){
		board[a][b] = true;
		board[a][b+1] = true;
		board[a+1][b] = true;
		board[a+1][b-1] = true;
		int q = 0;
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp+36; i < tmp+36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360; i < tmp+36-360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		
		for (int i = tmp-360-36; i < tmp+36-360-36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}	
	}else if(curr_tile == 3){
		board[a][b] = true;
		board[a-1][b] = true;
		board[a][b+1] = true;
		board[a+1][b+1] = true;

		int q = 0;
		for (int i = tmp+360; i < tmp+36+360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp+36; i < tmp+36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360+36; i < tmp+36-360+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else if(curr_tile == 4){
		board[a][b] = true;
		board[a][b+1] = true;
		board[a][b-1] = true;
		board[a+1][b-1] = true;
		int q = 0;
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-36; i < tmp-36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		
		for (int i = tmp+36; i < tmp+36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360-36; i < tmp-36-360+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else if(curr_tile == 5){
		board[a][b] = true;
		board[a-1][b] = true;
		board[a+1][b] = true;
		board[a+1][b+1] = true;
		int q = 0;
		for (int i = tmp+360; i < tmp+36+360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360; i < tmp+36-360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360+36; i < tmp+36-360+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else if(curr_tile == 6){
		board[a][b] = true;
		board[a][b+1] = true;
		board[a][b-1] = true;
		board[a-1][b+1] = true;
		int q = 0;
		for (int i = tmp+36+360; i < tmp+36+36+360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-36; i < tmp+36-36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp+36; i < tmp+36+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}else{
		board[a][b] = true;
		board[a+1][b] = true;
		board[a-1][b] = true;
		board[a-1][b-1] = true;

		int q = 0;
		for (int i = tmp+360-36; i < tmp+36+360-36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp+360; i < tmp+36+360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp; i < tmp+36; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
		for (int i = tmp-360; i < tmp+36-360; i++){
			boardcolours[i] = newcolours[q];
			q++;
		}
	}	
	
	checkempty();
	updatetile();
	scancolor();
	freez = true;
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void falling(int data){
	int a = 19-int(tilepos.y);
	int b = int(tilepos.x);

	checkfullrow();
	checkempty();
	
	if(gameover()){
		cout << "!!!!!!!!!!!!!GAME OVER!!!!!!!!!!!!!!!!" << endl;
		exit (EXIT_SUCCESS);
	}
	
	if(curr_tile == 0){
		if(board[a+1][b]==true || board[a+1][b-1] == true || board[a+1][b-2] == true 
			|| board[a+1][b+1] == true || tilepos.y == 0.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}
	}else if(curr_tile == 1){
		if(board[a+1][b]==true || board[a+1+1][b] == true || board[a+2+1][b] == true 
			|| board[a-1+1][b] == true || tilepos.y == 2.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else if(curr_tile == 2){
		if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1+1][b] == true 
			|| board[a+1+1][b-1] == true || tilepos.y == 1.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else if(curr_tile == 3){
		if(board[a+1][b]==true || board[a-1+1][b] == true || board[a+1][b+1] == true 
			|| board[a+1+1][b+1] == true || tilepos.y == 1.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else if(curr_tile == 4){
		if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1][b-1] == true 
			|| board[a+1+1][b-1] == true || tilepos.y == 1.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else if(curr_tile == 5){
		if(board[a+1][b]==true || board[a-1+1][b] == true || board[a+1+1][b] == true 
			|| board[a+1+1][b+1] == true || tilepos.y == 1.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else if(curr_tile == 6){
		if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1][b-1] == true 
			|| board[a-1+1][b+1] == true || tilepos.y == 0.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}

	}else{
		if(board[a+1][b]==true || board[a+1+1][b] == true || board[a-1+1][b] == true 
			|| board[a-1+1][b-1] == true || tilepos.y == 1.0){
			if(!isInside || freez){
				return;
			}
			settile();
			newtile();
		}
	}

	glutTimerFunc(speed, falling, -1); 
	tilepos.y -= 1.0;
	updatetile();	
}

// Handle arrow key keypresses
void special(int key, int x, int y)
{
	if(key == GLUT_KEY_UP){
		rotate();
	}else if(key == GLUT_KEY_DOWN){
		movetile(vec2(0,-1));
		int a = 19-int(tilepos.y);
		int b = int(tilepos.x);
		//checkfullrow();
		//checkempty();

		if(gameover()){
			cout << "!!!!!!!!!!!!!GAME OVER!!!!!!!!!!!!!!!!" << endl;
			exit (EXIT_SUCCESS);
		}
	
		if(curr_tile == 0){
			if(board[a+1][b]==true || board[a+1][b-1] == true || board[a+1][b-2] == true 
				|| board[a+1][b+1] == true || tilepos.y == 0.0){
				settile();
				newtile();
			}
		}else if(curr_tile == 1){
			if(board[a+1][b]==true || board[a+1+1][b] == true || board[a+2+1][b] == true 
				|| board[a-1+1][b] == true || tilepos.y == 2.0){
				settile();
				newtile();
			}

		}else if(curr_tile == 2){
			if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1+1][b] == true 
				|| board[a+1+1][b-1] == true || tilepos.y == 1.0){
				settile();
				newtile();
			}

		}else if(curr_tile == 3){
			if(board[a+1][b]==true || board[a-1+1][b] == true || board[a+1][b+1] == true 
				|| board[a+1+1][b+1] == true || tilepos.y == 1.0){
				settile();
				newtile();
			}

		}else if(curr_tile == 4){
			if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1][b-1] == true 
				|| board[a+1+1][b-1] == true || tilepos.y == 1.0){
				settile();
				newtile();
			}

		}else if(curr_tile == 5){
			if(board[a+1][b]==true || board[a-1+1][b] == true || board[a+1+1][b] == true 
				|| board[a+1+1][b+1] == true || tilepos.y == 1.0){
				settile();
				newtile();
			}

		}else if(curr_tile == 6){
			if(board[a+1][b]==true || board[a+1][b+1] == true || board[a+1][b-1] == true 
			|| board[a-1+1][b+1] == true || tilepos.y == 0.0){
			settile();
			newtile();
		}

		}else{
			if(board[a+1][b]==true || board[a+1+1][b] == true || board[a-1+1][b] == true 
				|| board[a-1+1][b-1] == true || tilepos.y == 1.0){
				settile();
				newtile();
			}
		}

	}else if(key == GLUT_KEY_LEFT){
		if(glutGetModifiers() == GLUT_ACTIVE_CTRL){
				View = View * RotateY(-25);
		}else{
			moveleft();
		}

	}else if(key == GLUT_KEY_RIGHT){
		if(glutGetModifiers() == GLUT_ACTIVE_CTRL){
				View = View * RotateY(25);
		}else{
			moveright();
		}
	}
}

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restart();
			break;
		case ' ':
			if(!isInside){
				break;	
			}
			drop = true;
			isInside = true;
			falling(-1);
			freez = false;
			break;
		case 'a':
			Theta[LowerArm] = Theta[LowerArm] + 10;
			updatetile();
			break;
		case 'd':
			Theta[LowerArm] = Theta[LowerArm] - 10;
			updatetile();
			break;
		case 'w':
			Theta[UpperArm] = Theta[UpperArm] + 10;
			updatetile();
			break;
		case 's':
			Theta[UpperArm] = Theta[UpperArm] - 10;
			updatetile();
			break;

	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int game;
	cout << "----------------------------------------------------------------" << endl;
	cout << "Please Select Game Complexity: 0:Easy  1:Normal  2:Hard  3:Hell" << endl;
	cout << "----------------------------------------------------------------" << endl;
	cin >> game;
	if(game == 0){
		speed = 380;
	}else if(game == 1){
		speed = 280;
	}else if(game == 2){
		speed = 100;
	}else{
		speed = 30;
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();
	
	// Callback functions
	falling(-1);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	cout << "main over" << endl;
		
	//}
	glutMainLoop(); // Start main loop

	return 0;
}
