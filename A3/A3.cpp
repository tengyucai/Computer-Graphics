#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <deque>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

static const int LEFT_MOUSE = 0;
static const int RIGHT_MOUSE = 1;
static const int MIDDLE_MOUSE = 2;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Init variables
	cur_mode = POSITION_ORIENTATION;
	enable_circle = false;
	enable_z_buffer = true;
	enable_backface_culling = false;
	enable_frontface_culling = false;
	translation = mat4();
	rotation = mat4();
	
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	do_picking = false;

	head = getHead(m_rootNode);

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	// m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.position = vec3(0.0f, 0.0f, 1.0f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

void A3::resetPosition() {
	translation = mat4();
}

void A3::resetOrientation() {
	rotation = mat4();
}

void A3::resetJoints() {
	while (!undo_stack.empty()) {
		undo_stack.top()->undo();
		undo_stack.pop();
	}
	while (!redo_stack.empty()) redo_stack.pop();
}

void A3::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints();
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );

		//-- Set LightSource uniform for the scene:
		if ( !do_picking ) {
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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
		if ( ImGui::BeginMainMenuBar() ) {
			if ( ImGui::BeginMenu( "Application" ) ) {
				if ( ImGui::MenuItem( "Reset Position", "I" ) ) {
					resetPosition();
				}
			
				if ( ImGui::MenuItem( "Reset Orientation", "O" ) ) {
					resetOrientation();
				}
			
				if ( ImGui::MenuItem( "Reset Joints", "N" ) ) {
					resetJoints();
				}
			
				if ( ImGui::MenuItem( "Reset All", "A" ) ) {
					resetAll();
				}
			
				if ( ImGui::MenuItem( "Quit", "Q" ) ) {
					glfwSetWindowShouldClose(m_window, GL_TRUE);
				}
			
				ImGui::EndMenu();
			}
		
			if ( ImGui::BeginMenu( "Edit" ) ) {
				if ( ImGui::MenuItem( "Undo", "U" ) ) {
					undo();
				}
			
				if ( ImGui::MenuItem( "Redo", "R" ) ) {
					redo();
				}
			
				ImGui::EndMenu();
			}
		
			if ( ImGui::BeginMenu( "Options" ) ) {
				if ( ImGui::Checkbox( "Circle            (C)", &enable_circle ) ) {
				}
			
				if ( ImGui::Checkbox( "Z-buffer          (Z)", &enable_z_buffer ) ) {
				}
		
				if ( ImGui::Checkbox( "Backface culling  (B)", &enable_backface_culling ) ) {			
				}
			
				if ( ImGui::Checkbox( "Frontface culling (F)", &enable_frontface_culling ) ) {
				}
			
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// Create Button, and check if it was clicked:
		//if( ImGui::Button( "Quit Application" ) ) {
		//	glfwSetWindowShouldClose(m_window, GL_TRUE);
		//}
		if ( ImGui::RadioButton( "Position/Orientation (P)", cur_mode == POSITION_ORIENTATION ) ) {
			cur_mode = POSITION_ORIENTATION;
		}
		
		if ( ImGui::RadioButton( "Joints (J)", cur_mode == JOINTS ) ) {
			cur_mode = JOINTS;
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

		if (undo_stack.empty()) ImGui::Text( "End of undo stack!" );
		if (redo_stack.empty()) ImGui::Text( "End of redo stack!" );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (enable_z_buffer) glEnable( GL_DEPTH_TEST );
	
	if (enable_backface_culling || enable_frontface_culling) {
		glEnable(GL_CULL_FACE);
		if (enable_backface_culling && enable_frontface_culling) {
			glCullFace(GL_FRONT_AND_BACK);
		} else if (enable_backface_culling) {
			glCullFace(GL_BACK);
		} else {
			glCullFace(GL_FRONT);
		}
	} else {
		glDisable(GL_CULL_FACE);
	}
	
	renderSceneGraph(*m_rootNode);

	if (enable_z_buffer) glDisable( GL_DEPTH_TEST );
	if (enable_circle) renderArcCircle();
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types

	deque<mat4> transform_stack;

	mat4 prev_trans = m_rootNode->get_transform();
	m_rootNode->set_transform(translation * m_rootNode->get_transform() * rotation);
	root.renderSceneNode(m_shader, m_view, m_batchInfoMap, transform_stack, do_picking);
	m_rootNode->set_transform(prev_trans);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

vec3 A3::getArcballVector(double x, double y) {
	vec3 P = vec3(x / m_framebufferWidth * 2 - 1.0, -(y / m_framebufferHeight * 2 - 1.0), 0.0f);
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1*1) {
		P.z = sqrt(1*1 - OP_squared);
	} else {
		P = normalize(P);
	}
	return P;
}

mat4 A3::getArcballRotationMatrix(double prev_x, double prev_y, double cur_x, double cur_y) {
	vec3 S = getArcballVector(prev_x, prev_y);
	vec3 T = getArcballVector(cur_x, cur_y);
	float angle = acos(std::min(1.0f, dot(S, T))) / 10;
	vec3 axis_in_camera_coord = cross(S, T);
	vec4 axis_in_world_coord = inverse(m_view) * vec4(axis_in_camera_coord.x, axis_in_camera_coord.y, axis_in_camera_coord.z, 0);
	return rotate(rotation, degrees(angle), vec3(axis_in_world_coord.x, axis_in_world_coord.y, axis_in_world_coord.z));
}

SceneNode* A3::getNodeById(SceneNode *root, unsigned int id) {
	if (root->m_nodeId == id) return root;
	for (SceneNode *child : root->children) {
		SceneNode *tmp = getNodeById(child, id);
		if (tmp) return tmp;
	}
	return NULL;
}

SceneNode* A3::getParentById(SceneNode *root, unsigned int id) {
	for (SceneNode *child : root->children) {
		if (child->m_nodeId == id) return root;
		SceneNode *tmp = getParentById(child, id);
		if (tmp) return tmp;
	}
	return NULL;
}

SceneNode* A3::getHead(SceneNode *root) {
	if (root->m_name == "head") return root;
	for (auto child : root->children) {
		SceneNode *tmp = getHead(child);
		if (tmp) return tmp;
	}
	return NULL;
}

void A3::undo() {
	cout << "Undo" << endl;
	if (!undo_stack.empty()) {
		Command *undo_cmd = undo_stack.top();
		undo_stack.pop();
		redo_stack.push(undo_cmd);
		undo_cmd->undo();
	} else {
		cout << "undo stack empty!" << endl;
	}
}

void A3::redo() {
	cout << "Redo" << endl;
	if (!redo_stack.empty()) {
		Command *redo_cmd = redo_stack.top();
		redo_stack.pop();
		undo_stack.push(redo_cmd);
		redo_cmd->execute();
	} else {
		cout << "redo stack empty!" << endl;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		float x_diff = xPos - mouse_x_position;
		float y_diff = yPos - mouse_y_position;

		if (cur_mode == POSITION_ORIENTATION) {
			if (ImGui::IsMouseDown(LEFT_MOUSE)) {
				translation = translate(translation, vec3( x_diff / 100, -y_diff / 100, 0.0f ));
				//m_rootNode->translate(vec3( x_diff / 100, -y_diff / 100, 0.0f ));
			}
			if (ImGui::IsMouseDown(RIGHT_MOUSE)) {
				rotation = getArcballRotationMatrix(mouse_x_position, mouse_y_position, xPos, yPos);
			}
			if (ImGui::IsMouseDown(MIDDLE_MOUSE)) {
				translation = translate(translation, vec3( 0.0f, 0.0f, y_diff / 100 ));
				//m_rootNode->translate(vec3( 0.0f, 0.0f, y_diff / 100 ));
			}
		} else if (cur_mode == JOINTS) {
			if (ImGui::IsMouseDown(MIDDLE_MOUSE)) {
				for (auto node : selected_nodes) {
					if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
						node->rotate('y', y_diff);
					} else { // x-axis
						node->rotate('x', y_diff);
					}
				}
				cur_cmd->angle += y_diff;
			}
		}
		
		mouse_x_position = xPos;
		mouse_y_position = yPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (cur_mode == JOINTS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos( m_window, &xpos, &ypos);

			do_picking = true;

			uploadCommonSceneUniforms();
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glClearColor(0.35, 0.35, 0.35, 1.0);

			draw();

			CHECK_GL_ERRORS;

			xpos *= double(m_framebufferWidth) / double(m_windowWidth);
			ypos = m_windowHeight - ypos;
			ypos *= double(m_framebufferHeight) / double(m_windowHeight);

			GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
			glReadBuffer( GL_BACK );
			glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
			CHECK_GL_ERRORS;

			unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

			SceneNode *node = getNodeById(m_rootNode.get(), what);
			if (node != NULL) {
				SceneNode *parent = getParentById(m_rootNode.get(), what);
				if (parent->m_nodeType == NodeType::JointNode) {
					parent->isSelected = !parent->isSelected;
					node->isSelected = parent->isSelected;
					if (parent->isSelected) {
						selected_nodes.insert((JointNode *)parent);
					} else {
						selected_nodes.erase((JointNode *)parent);
					}
				}
			}

			do_picking = false;

			CHECK_GL_ERRORS;
		} else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (actions == GLFW_PRESS) {
				cur_cmd = new Command({selected_nodes.begin(), selected_nodes.end()}, 0.0f);
			} else if (actions == GLFW_RELEASE) {
				undo_stack.push(cur_cmd);
				cur_cmd = NULL;
				while (!redo_stack.empty()) redo_stack.pop();
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		} else if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		} else if ( key == GLFW_KEY_P ) {
			cur_mode = POSITION_ORIENTATION;
			eventHandled = true;
		} else if ( key == GLFW_KEY_J ) {
			cur_mode = JOINTS;
		} else if ( key == GLFW_KEY_I ) {
			resetPosition();
		} else if ( key == GLFW_KEY_O ) {
			resetOrientation();
		} else if ( key == GLFW_KEY_N ) {
			resetJoints();
		} else if ( key == GLFW_KEY_A ) {
			resetAll();
		} else if ( key == GLFW_KEY_C ) {
			enable_circle = !enable_circle;
		} else if ( key == GLFW_KEY_Z ) {
			enable_z_buffer = !enable_z_buffer;
		} else if ( key == GLFW_KEY_B ) {
			enable_backface_culling = !enable_backface_culling;
		} else if ( key == GLFW_KEY_F ) {
			enable_frontface_culling = !enable_frontface_culling;
		} else if ( key == GLFW_KEY_U ) {
			undo();
		} else if ( key == GLFW_KEY_R ) {
			redo();
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
