#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <cmath>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

static const float MAX_FOV = 160.0f;
static const float MIN_FOV = 5.0f;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	reset();
}

void A2::reset()
{
	// Init variables
	cur_mode = InteractionMode::MODEL_ROTATE;
	fov = 30.0f;
	near = 7.5f;
	far = 12.5f;
	mouse_x_position = 0.0f;
	
	// Init Model, View and Projection matrics
	ModelTR = mat4();
	ModelS = mat4();
	View = mat4(vec4(1, 0, 0, 0), 
					vec4(0, 1, 0, 0), 
					vec4(0, 0, -1, 0), 
					vec4(0, 0, 10, 1));
	setProjectionMatrix();

	// Init cube and gnomons
	initGnomons();
	initCube();
	
	// Init Viewport
	initViewportBorder();
	viewport_start_x = m_framebufferWidth * 0.05;
	viewport_end_x = m_framebufferWidth * 0.95;
	viewport_start_y = m_framebufferHeight * 0.05;
	viewport_end_y = m_framebufferHeight * 0.95;
	setViewportMatrix();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

void A2::initCube()
{
	cube_vertices_data.clear();
	cube_vertices_data.push_back(make_pair(vec4(1.0f, 1.0f, 1.0f, 1), 
														vec4(1.0f, 1.0f, -1.0f, 1))); 
	cube_vertices_data.push_back(make_pair(vec4(1.0f, 1.0f, 1.0f, 1), 
														vec4(-1.0f, 1.0f, 1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(1.0f, 1.0f, 1.0f, 1), 
														vec4(1.0f, -1.0f, 1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(-1.0f, 1.0f, 1.0f, 1), 
														vec4(-1.0f, -1.0f, 1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(-1.0f, 1.0f, 1.0f, 1), 
														vec4(-1.0f, 1.0f, -1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(1.0f, 1.0f, -1.0f, 1), 
														vec4(-1.0f, 1.0f, -1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(-1.0f, 1.0f, -1.0f, 1), 
														vec4(-1.0f, -1.0f, -1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(1.0f, 1.0f, -1.0f, 1), 
														vec4(1.0f, -1.0f, -1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(-1.0f, -1.0f, -1.0f, 1), 
														vec4(1.0f, -1.0f, -1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(-1.0f, -1.0f, -1.0f, 1), 
														vec4(-1.0f, -1.0f, 1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(1.0f, -1.0f, -1.0f, 1), 
														vec4(1.0f, -1.0f, 1.0f, 1)));
	cube_vertices_data.push_back(make_pair(vec4(1.0f, -1.0f, 1.0f, 1), 
														vec4(-1.0f, -1.0f, 1.0f, 1)));
}

void A2::initGnomons()
{
	model_coordinate.clear();
	model_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
													 vec4(0.5f, 0.0f, 0.0f, 1)));
	model_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
													 vec4(0.0f, 0.5f, 0.0f, 1)));
	model_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
										  			 vec4(0.0f, 0.0f, 0.5f, 1)));

	world_coordinate.clear();				  			 
	world_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
													 vec4(0.5f, 0.0f, 0.0f, 1)));
	world_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
													 vec4(0.0f, 0.5f, 0.0f, 1)));
	world_coordinate.push_back(make_pair(vec4(0.0f, 0.0f, 0.0f, 1), 
										  			 vec4(0.0f, 0.0f, 0.5f, 1)));
}

void A2::initViewportBorder()
{
	viewport_border.clear();
	viewport_border.push_back(make_pair(vec4(-1.0f, -1.0f, 1.0f, 1), 
													vec4(1.0f, -1.0f, 1.0f, 1)));
	viewport_border.push_back(make_pair(vec4(-1.0f, -1.0f, 1.0f, 1), 
													vec4(-1.0f, 1.0f, 1.0f, 1)));
	viewport_border.push_back(make_pair(vec4(1.0f, 1.0f, 1.0f, 1), 
													vec4(1.0f, -1.0f, 1.0f, 1)));
	viewport_border.push_back(make_pair(vec4(1.0f, 1.0f, 1.0f, 1), 
													vec4(-1.0f, 1.0f, 1.0f, 1)));
}

void A2::setProjectionMatrix() {
	Projection = mat4(
		vec4(1/tan(radians(fov/2)), 0.0f, 					   0.0f,						    0.0f),
		vec4(0.0f,						 1/tan(radians(fov/2)), 0.0f, 						 0.0f),
		vec4(0.0f,						 0.0f,						(far+near)/(far-near),   1.0f),
		vec4(0.0f, 						 0.0f,						-2*far*near/(far-near),  0.0f)
	);
}

void A2::setViewportMatrix() {
	float x_ratio = fabs(viewport_start_x - viewport_end_x) / m_framebufferWidth;
	float y_ratio = fabs(viewport_start_y - viewport_end_y) / m_framebufferHeight;
	float x_min = std::min(viewport_start_x, viewport_end_x);
	float y_min = std::min(viewport_start_y, viewport_end_y); 
	Viewport = mat4(
		vec4(x_ratio, 															0.0f, 														     0.0f, 0.0f),
		vec4(0.0f, 																y_ratio, 													     0.0f, 0.0f),
		vec4(2.0f*(-(1-x_ratio)/2 + (x_min/m_framebufferWidth)), -2.0f*(-(1-y_ratio)/2 + (y_min/m_framebufferWidth)), 1.0f, 0.0f),
		vec4(0.0f,																0.0f,																  0.0f, 1.0f)
	);
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	// Draw outer square:
	//setLineColour(vec3(1.0f, 0.7f, 0.8f));
	//drawLine(vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
	//drawLine(vec2(0.5f, -0.5f), vec2(0.5f, 0.5f));
	//drawLine(vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f));
	//drawLine(vec2(-0.5f, 0.5f), vec2(-0.5f, -0.5f));


	// Draw inner square:
	//setLineColour(vec3(0.2f, 1.0f, 1.0f));
	//drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	//drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	//drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	//drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));
	
	// Draw world coordinate:
	vector<vec3> world_coord_color;
	world_coord_color.push_back( vec3(1.0f, 0.0f, 0.0f) );
	world_coord_color.push_back( vec3(0.0f, 1.0f, 0.0f) );
	world_coord_color.push_back( vec3(0.0f, 0.0f, 1.0f) );
	for (int i = 0; i < world_coordinate.size(); ++i) {
		setLineColour( world_coord_color[i] );
		pair<vec4, vec4> line = world_coordinate[i];
		vec4 vertex1 = Projection * View * line.first;
		vec4 vertex2 = Projection * View * line.second;
		if (clip(vertex1, vertex2)) {
			vertex1 = Viewport * vec4(vertex1.x / vertex1.w, vertex1.y / vertex1.w, 1.0f, 1.0f);
			vertex2 = Viewport * vec4(vertex2.x / vertex2.w, vertex2.y / vertex2.w, 1.0f, 1.0f);
			drawLine(vec2(vertex1.x, vertex1.y),
						vec2(vertex2.x, vertex2.y));
		}
	}
	
	// Draw model coordinate:
	vector<vec3> model_coord_color;
	model_coord_color.push_back( vec3(1.0f, 1.0f, 0.0f) );
	model_coord_color.push_back( vec3(0.0f, 1.0f, 1.0f) );
	model_coord_color.push_back( vec3(1.0f, 0.0f, 1.0f) );
	for (int i = 0; i < model_coordinate.size(); ++i) {
		setLineColour( model_coord_color[i] );
		pair<vec4, vec4> line = model_coordinate[i];
		vec4 vertex1 = Projection * View * ModelTR * line.first;
		vec4 vertex2 = Projection * View * ModelTR * line.second;
		if (clip(vertex1, vertex2)) {
			vertex1 = Viewport * vec4(vertex1.x / vertex1.w, vertex1.y / vertex1.w, 1.0f, 1.0f);
			vertex2 = Viewport * vec4(vertex2.x / vertex2.w, vertex2.y / vertex2.w, 1.0f, 1.0f);
			drawLine(vec2(vertex1.x, vertex1.y),
						vec2(vertex2.x, vertex2.y));
		}
	}
	
	// Draw cube:
	setLineColour( vec3( 1.0f, 0.7f, 0.8f ) );
	for (int i = 0; i < cube_vertices_data.size(); ++i) {
		pair<vec4, vec4> line = cube_vertices_data[i];
		vec4 vertex1 = Projection * View * ModelTR * ModelS * line.first;
		vec4 vertex2 = Projection * View * ModelTR * ModelS * line.second;
		if (clip(vertex1, vertex2)) {
			vertex1 = Viewport * vec4(vertex1.x / vertex1.w, vertex1.y / vertex1.w, 1.0f, 1.0f);
			vertex2 = Viewport * vec4(vertex2.x / vertex2.w, vertex2.y / vertex2.w, 1.0f, 1.0f);
			drawLine(vec2(vertex1.x, vertex1.y),
						vec2(vertex2.x, vertex2.y));
		}
	}
	
	// Draw viewport
	setLineColour( vec3( 0.0f, 0.0f, 0.0f ) );
	for (int i = 0; i < viewport_border.size(); ++i) {
		pair<vec4, vec4> line = viewport_border[i];
		vec4 vertex1 = Viewport * line.first;
		vec4 vertex2 = Viewport * line.second;
		drawLine(vec2(vertex1.x, vertex1.y),
					vec2(vertex2.x, vertex2.y));
	}
}

bool A2::clip(vec4& A, vec4& C)
{
	for (int i = 0; i < 6; i++){
		int aOutcode = 0, cOutcode = 0;
		float aBC[6], cBC[6];
		
		aBC[0] = A.w + A.x;
		aBC[1] = A.w - A.x;
		aBC[2] = A.w + A.y;
		aBC[3] = A.w - A.y;
		aBC[4] = A.w + A.z;
		aBC[5] = A.w - A.z;

		cBC[0] = C.w + C.x;
		cBC[1] = C.w - C.x;
		cBC[2] = C.w + C.y;
		cBC[3] = C.w - C.y;
		cBC[4] = C.w + C.z;
		cBC[5] = C.w - C.z;
		
		for (int j = 0; j < 6; ++j) {
			if (aBC[j] < 0) aOutcode = aOutcode << 1 | 1;
			else aOutcode = aOutcode << 1 | 0;
				
			if (cBC[j] < 0) cOutcode = cOutcode << 1 | 1;
			else cOutcode = cOutcode << 1 | 0;
		}
		
		if (aOutcode & cOutcode) return false;
		if (!(aOutcode | cOutcode)) return true;
		
		float tHit = 0.0;
		tHit = aBC[i] / (aBC[i] - cBC[i]);
		if (aBC[i] < 0) A = (1 - tHit) * A + tHit * C;
		if (cBC[i] < 0) C = (1 - tHit) * A + tHit * C;
	}
   return true;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		for (int mode = VIEW_ROTATE; mode <= VIEWPORT; ++mode) {
			ImGui::PushID( mode );
			int tmp_cur_mode = static_cast<int>(cur_mode);
			if( ImGui::RadioButton( "##Mode", &tmp_cur_mode, mode ) ) {
				// Select this mode.
				cur_mode = static_cast<InteractionMode>(mode);
			}
			ImGui::SameLine();
			switch(mode) {
				case VIEW_ROTATE:
					ImGui::Text("O - Rotate View");
					break;
				case VIEW_TRANSLATE:
					ImGui::Text("N - Translate View");
					break;
				case PERSPECTIVE:
					ImGui::Text("P - Perspective");
					break;
				case MODEL_ROTATE:
					ImGui::Text("R - Rotate Model");
					break;
				case MODEL_TRANSLATE:
					ImGui::Text("T - Translate Model");
					break;
				case MODEL_SCALE:
					ImGui::Text("S - Scale Model");
					break;
				case VIEWPORT:
					ImGui::Text("V - Viewport");
					break;
				default:
					break;
			}
			ImGui::PopID();
		}
		
		if( ImGui::Button( "Reset Application" ) ) {
			reset();
		}

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Field of View: %.1f degree", fov );
		ImGui::Text( "Distance of near plane: %.1f", near );
		ImGui::Text( "Distance of far plane: %.1f", far );
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		float diff = xPos - mouse_x_position;
		switch(cur_mode) 
		{
			case InteractionMode::VIEW_ROTATE:
				if (ImGui::IsMouseDown(0)) {	
					View = inverse(rotate(mat4(), diff / 180, vec3( 1.0f, 0.0f, 0.0f ))) * View; 
				}
				if (ImGui::IsMouseDown(1)) {
					View = inverse(rotate(mat4(), diff / 180, vec3( 0.0f, 0.0f, 1.0f ))) * View; 
				}
				if (ImGui::IsMouseDown(2)) {
					View = inverse(rotate(mat4(), diff / 180, vec3( 0.0f, 1.0f, 0.0f ))) * View;
				}
				break;
			case InteractionMode::VIEW_TRANSLATE:
				if (ImGui::IsMouseDown(0)) {	
					View = inverse(translate(mat4(), vec3( diff / 100, 0.0f, 0.0f ))) * View; 
				}
				if (ImGui::IsMouseDown(1)) {
					View = inverse(translate(mat4(), vec3( 0.0f, 0.0f, diff / 100 ))) * View; 
				}
				if (ImGui::IsMouseDown(2)) {
					View = inverse(translate(mat4(), vec3( 0.0f, diff / 100, 0.0f ))) * View; 
				}
				break;
			case InteractionMode::MODEL_ROTATE:
				if (ImGui::IsMouseDown(0)) {	
					ModelTR = rotate(ModelTR, diff / 180, vec3( 1.0f, 0.0f, 0.0f )); 
				}
				if (ImGui::IsMouseDown(1)) {
					ModelTR = rotate(ModelTR, diff / 180, vec3( 0.0f, 0.0f, 1.0f )); 
				}
				if (ImGui::IsMouseDown(2)) {
					ModelTR = rotate(ModelTR, diff / 180, vec3( 0.0f, 1.0f, 0.0f )); 
				}
				break;
			case InteractionMode::MODEL_TRANSLATE:
				if (ImGui::IsMouseDown(0)) {	
					ModelTR = translate(ModelTR, vec3( diff / 100, 0.0f, 0.0f )); 
				}
				if (ImGui::IsMouseDown(1)) {
					ModelTR = translate(ModelTR, vec3( 0.0f, 0.0f, diff / 100 )); 
				}
				if (ImGui::IsMouseDown(2)) {
					ModelTR = translate(ModelTR, vec3( 0.0f, diff / 100, 0.0f )); 
				}
				break;
			case InteractionMode::MODEL_SCALE:
				if (ImGui::IsMouseDown(0)) {	
					ModelS = scale(ModelS, vec3( 1.0f + diff / 20, 1.0f, 1.0f )); 
				}
				if (ImGui::IsMouseDown(1)) {
					ModelS = scale(ModelS, vec3( 1.0f, 1.0f, 1.0f + diff / 20 )); 
				}
				if (ImGui::IsMouseDown(2)) {
					ModelS = scale(ModelS, vec3( 1.0f, 1.0f + diff / 20, 1.0f )); 
				}
				break;
			case InteractionMode::PERSPECTIVE:
				if (ImGui::IsMouseDown(0)) {	
					fov += diff / 50;
					if (fov > MAX_FOV) fov = MAX_FOV;
					if (fov < MIN_FOV) fov = MIN_FOV;
				}
				if (ImGui::IsMouseDown(1)) {
					far += diff / 50;
					if (far <= near + 0.1) far = near + 0.1;
				}
				if (ImGui::IsMouseDown(2)) {
					near += diff / 50;
					if (near <= 0.1) near = 0.1;
				}
				if (ImGui::IsMouseDown(0) ||
					ImGui::IsMouseDown(1) ||
					ImGui::IsMouseDown(2)) {
					setProjectionMatrix();
				}
				break;
			case InteractionMode::VIEWPORT:
				if (ImGui::IsMouseDown(0)) {
					viewport_end_x = std::max(0.0, std::min((double)m_framebufferWidth, xPos));
					viewport_end_y = std::max(0.0, std::min((double)m_framebufferHeight, yPos));
					setViewportMatrix();
				}
			default:
				break;
		}
		mouse_x_position = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (cur_mode == InteractionMode::VIEWPORT &&
		 button == GLFW_MOUSE_BUTTON_LEFT &&
		 actions == GLFW_PRESS) {
		glfwGetCursorPos(m_window, &viewport_start_x, &viewport_start_y);	 
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		switch(key)
		{
			case GLFW_KEY_O:
				cout << "Rotate View Mode" << endl;
				cur_mode = InteractionMode::VIEW_ROTATE;
				break;
			case GLFW_KEY_N:
				cout << "Translate View Mode" << endl;
				cur_mode = InteractionMode::VIEW_TRANSLATE;
				break;
			case GLFW_KEY_P:
				cout << "Perspective Mode" << endl;
				cur_mode = InteractionMode::PERSPECTIVE;
				eventHandled = true;
				break;
			case GLFW_KEY_R:
				cout << "Rotate Model Mode" << endl;
				cur_mode = InteractionMode::MODEL_ROTATE;
				break;
			case GLFW_KEY_T:
				cout << "Translate Model Mode" << endl;
				cur_mode = InteractionMode::MODEL_TRANSLATE;
				break;
			case GLFW_KEY_S:
				cout << "Scale Model Mode" << endl;
				cur_mode = InteractionMode::MODEL_SCALE;
				break;
			case GLFW_KEY_V:
				cout << "Viewport Mode" << endl;
				cur_mode = InteractionMode::VIEWPORT;
				break;
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			case GLFW_KEY_A:
				reset();
				break;
			default:
				break;
		}
	}

	return eventHandled;
}
