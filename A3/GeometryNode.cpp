#include "GeometryNode.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/GlErrorCheck.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::renderSceneNode(
	const ShaderProgram & shader, 
	const glm::mat4 & viewMatrix, 
	BatchInfoMap & m_batchInfoMap,
	std::deque<glm::mat4> & stack,
	glm::mat4 T,
	bool do_picking) const {

	stack.push_back(trans);

	for (const SceneNode *node : this->children) {
		node->renderSceneNode(shader, viewMatrix, m_batchInfoMap, stack, T * trans, do_picking);
	}

	updateShaderUniforms(shader, viewMatrix, stack, T, do_picking);

	BatchInfo batchInfo = m_batchInfoMap[meshId];

	shader.enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	shader.disable();

	stack.pop_back();
}

void GeometryNode::updateShaderUniforms(
		const ShaderProgram & shader,
		const glm::mat4 & viewMatrix,
		std::deque<glm::mat4> & stack, 
		glm::mat4 T,
		bool do_picking) const {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		glm::mat4 modelView;
		// for (auto it = stack.rbegin(); it < stack.rend(); ++it) {
		// 	modelView = (*it) * modelView;
		// }
		//modelView = viewMatrix * modelView;
		modelView = viewMatrix * T * trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if ( do_picking ) {
			float r = float(m_nodeId & 0xff) / 255.0f;
			float g = float((m_nodeId >> 8) & 0xff) / 255.0f;
			float b = float((m_nodeId >> 16) & 0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		} else {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			glm::vec3 kd = material.kd;
			if (isSelected) {
				kd = glm::vec3( 1.0, 1.0, 0.0 );
			}
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			glm::vec3 ks = material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, material.shininess);
			CHECK_GL_ERRORS;
		}
	}
	shader.disable();

}
