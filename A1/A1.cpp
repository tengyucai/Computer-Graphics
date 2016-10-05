#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
static const size_t CUBE_NUM_VERTICES = 36;
static const size_t ACTIVE_CUBE_NUM_VERTICES = 18;
static const size_t MAX_HEIGHT = 8;
static const size_t NUM_COLOURS = 8;
static const float SCALE_UPPER_LIMIT = 1.5f;
static const float SCALE_LOWER_LIMIT = 0.5f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ),
	  grid( DIM ),
	  active_row( 0 ),
	  active_col( 0 ),
	  mouse_x_position( 0.0f ),
	  rotation_angle( 0.0f ),
	  scale_factor( 1.0f )
{	
	colours = new float*[NUM_COLOURS];
	for (int i = 0; i < NUM_COLOURS; ++i) {
		colours[i] = new float[3];
	}
	initColours();
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initColours()
{
	// White
	colours[0][0] = 1.0f;
	colours[0][1] = 1.0f;
	colours[0][2] = 1.0f;
	
	// Red
	colours[1][0] = 1.0f;
	colours[1][1] = 0.0f;
	colours[1][2] = 0.0f;
	
	// Green
	colours[2][0] = 0.0f;
	colours[2][1] = 1.0f;
	colours[2][2] = 0.0f;
	
	// Dark blue
	colours[3][0] = 0.0f;
	colours[3][1] = 0.0f;
	colours[3][2] = 1.0f;
	
	// Yellow
	colours[4][0] = 1.0f;
	colours[4][1] = 1.0f;
	colours[4][2] = 0.0f;
	
	// Pink
	colours[5][0] = 1.0f;
	colours[5][1] = 0.0f;
	colours[5][2] = 1.0f;
	
	// Light blue
	colours[6][0] = 0.0f;
	colours[6][1] = 1.0f;
	colours[6][2] = 1.0f;
	
	// Black
	colours[7][0] = 0.0f;
	colours[7][1] = 0.0f;
	colours[7][2] = 0.0f;
}

void A1::initActiveCube()
{
	float verts[] = {
		0.5f, 0.0f, 0.5f,
		0.25f, 1.0f, 0.25f,
		0.25f, 1.0f, 0.75f,

		0.5f, 0.0f, 0.5f,
		0.25f, 1.0f, 0.25f,
		0.75f, 1.0f, 0.25f,

		0.5f, 0.0f, 0.5f,
		0.75f, 1.0f, 0.25f,
		0.75f, 1.0f, 0.75f,

		0.5f, 0.0f, 0.5f,
		0.25f, 1.0f, 0.75f,
		0.75f, 1.0f, 0.75f,

		0.25f, 1.0f, 0.25f,
		0.25f, 1.0f, 0.75f,
		0.75f, 1.0f, 0.25f,

		0.75f, 1.0f, 0.75f,
		0.25f, 1.0f, 0.75f,
		0.75f, 1.0f, 0.25f,
	};
	
	for (int i = 0; i < ACTIVE_CUBE_NUM_VERTICES; ++i) {
		verts[ 3*i ] += active_col;
		verts[ 3*i + 1 ] += grid.getHeight( active_col, active_row );
		verts[ 3*i + 2 ] += active_row;
	}

	glGenVertexArrays( 1, &m_active_cube_vao );
	glBindVertexArray( m_active_cube_vao );
	
	glGenBuffers( 1, &m_active_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_active_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr );
	
	glDrawArrays( GL_TRIANGLES, 0, ACTIVE_CUBE_NUM_VERTICES );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers( 1, &m_active_cube_vbo );
	glDeleteVertexArrays( 1, &m_active_cube_vao );

	CHECK_GL_ERRORS;
}

void A1::initCubeAt(float x, float y, float z)
{
   float verts[]  = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	};

	for (int i = 0; i < CUBE_NUM_VERTICES; ++i) {
		verts[ 3*i ] += x;
		verts[ 3*i + 1 ] += y;
		verts[ 3*i + 2 ] += z;
	}

	glGenVertexArrays( 1, &m_cube_vao );
	glBindVertexArray( m_cube_vao );
	
	glGenBuffers( 1, &m_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr );
	
	glDrawArrays( GL_TRIANGLES, 0, CUBE_NUM_VERTICES );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers( 1, &m_cube_vbo );
	glDeleteVertexArrays( 1, &m_cube_vao );

	CHECK_GL_ERRORS;
}

void A1::reset()
{
	current_col = 0;
	initColours();
	
	active_row = 0;
	active_col = 0;
	
	mouse_x_position = 0.0f;
	rotation_angle = 0.0f;
	scale_factor = 1.0f;

	for (int c = 0; c < DIM; ++c) {
		for (int r = 0; r < DIM; ++r) {
			grid.setHeight( c, r, 0 );
			grid.setColour( c, r, current_col );
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		
		if( ImGui::Button( "Reset Application" ) ) {
			reset();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.
		
		for (int i = 0; i < NUM_COLOURS; ++i) {
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", colours[i] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				// Select this colour.
				current_col = i;
				grid.setColour( active_col, active_row, current_col );
			}
			ImGui::PopID();
		}

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::rotate( W, rotation_angle, vec3( 0.0f, 1.0f, 0.0f ) );
	W = glm::scale( W, vec3( scale_factor, scale_factor, scale_factor ) );
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		for (int c = 0; c < DIM; ++c) {
			for (int r = 0; r < DIM; ++r) {
				int colour = grid.getColour( c, r );
				glUniform3f( col_uni, 
								colours[colour][0],
								colours[colour][1],
								colours[colour][2]);
				int height = grid.getHeight( c, r );
				for (int h = 0; h < height; ++h) {
					initCubeAt( c, h, r );
				}
			}
		}

		// Highlight the active square.
		int colour = grid.getColour( active_col, active_row );
		glUniform3f( col_uni, 
						colours[colour][0],
						colours[colour][1],
						colours[colour][2]);
		initActiveCube();
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (ImGui::IsMouseDown(0)) {
			float diff = xPos - mouse_x_position;
			rotation_angle += diff / 180;
		}
		mouse_x_position = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	scale_factor += 0.05 * yOffSet;
	
	if ( scale_factor > SCALE_UPPER_LIMIT ) scale_factor = SCALE_UPPER_LIMIT;
	if ( scale_factor < SCALE_LOWER_LIMIT ) scale_factor = SCALE_LOWER_LIMIT;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if ( key == GLFW_KEY_SPACE ) {
			cout << "SPACE key press" << endl;

			if ( grid.getHeight( active_col, active_row ) < MAX_HEIGHT ) {
				grid.setHeight( active_col, 
						active_row, 
						grid.getHeight( active_col, active_row ) + 1 );
				if ( grid.getHeight( active_col, active_row ) == 0 ) {
					grid.setColour( active_col, active_row, current_col );
				}
			}
		} else if ( key == GLFW_KEY_BACKSPACE ) {
			cout << "BACKSPACE key press" << endl;

			if ( grid.getHeight( active_col, active_row ) > 0 ) {
				grid.setHeight( active_col,
						active_row,
						grid.getHeight( active_col, active_row ) - 1 );
			}
		} else if ( key == GLFW_KEY_RIGHT ) {
			cout << "LEFT key press" << endl;
			if ( active_col < DIM-1 ) {
				++active_col;
			}
			
			if ( grid.getHeight( active_col, active_row ) > 0 ) {
				current_col = grid.getColour( active_col, active_row );
			} else {
				grid.setColour( active_col, active_row, current_col );
			}
			
			if ( mods == GLFW_MOD_SHIFT ) {
				current_col = grid.getColour( active_col - 1, active_row );
				grid.setHeight( active_col, 
						active_row,
						grid.getHeight( active_col - 1, active_row ) );
				grid.setColour( active_col, active_row, current_col );
			}
		} else if ( key == GLFW_KEY_LEFT ) {
			cout << "RIGHT key press" << endl;
		
			if ( active_col > 0 ) {
				--active_col;
			}
			
			if ( grid.getHeight( active_col, active_row ) > 0 ) {
				current_col = grid.getColour( active_col, active_row );
			} else {
				grid.setColour( active_col, active_row, current_col );
			}
			
			if ( mods == GLFW_MOD_SHIFT ) {
				current_col = grid.getColour( active_col + 1, active_row );
				grid.setHeight( active_col, 
						active_row,
						grid.getHeight( active_col + 1, active_row ) );
				grid.setColour( active_col, active_row, current_col );
			}
		} else if ( key == GLFW_KEY_UP ) {
			cout << "UP key press" << endl;
			
			if ( active_row > 0 ) {
				--active_row;
			}
			
			if ( grid.getHeight( active_col, active_row ) > 0 ) {
				current_col = grid.getColour( active_col, active_row );
			} else {
				grid.setColour( active_col, active_row, current_col );
			}
			
			if ( mods == GLFW_MOD_SHIFT ) {
				current_col = grid.getColour( active_col, active_row + 1 );
				grid.setHeight( active_col, 
						active_row,
						grid.getHeight( active_col, active_row + 1) );
				grid.setColour( active_col, active_row, current_col );
			}
		} else if ( key == GLFW_KEY_DOWN ) {
			cout << "DOWN key press" << endl;
			
			if ( active_row < DIM-1 ) {
				++active_row;
			}
			
			if ( grid.getHeight( active_col, active_row ) > 0 ) {
				current_col = grid.getColour( active_col, active_row );
			} else {
				grid.setColour( active_col, active_row, current_col );
			}
			
			if ( mods == GLFW_MOD_SHIFT ) {
				current_col = grid.getColour( active_col, active_row - 1 );
				grid.setHeight( active_col, 
						active_row,
						grid.getHeight( active_col, active_row - 1 ) );
				grid.setColour( active_col, active_row, current_col );
			}
		} else if ( key == GLFW_KEY_R ) {
			cout << "R key press" << endl;
				
			reset();
		} else if ( key == GLFW_KEY_Q ) {
			cout << "Q key press" << endl;
		
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
	}

	return eventHandled;
}
