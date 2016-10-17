#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	void renderSceneNode(const ShaderProgram & shader, const glm::mat4 & viewMatrix, 
		BatchInfoMap & m_batchInfoMap, std::deque<glm::mat4> & stack, glm::mat4 T, bool do_picking) const;

private:
	void updateShaderUniforms(const ShaderProgram & shader, const glm::mat4 & viewMatrix, 
		std::deque<glm::mat4> & stack, glm::mat4 T, bool do_picking) const;
};
