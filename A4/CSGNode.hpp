#pragma once

#include "GeometryNode.hpp"

class CSGNode : public GeometryNode {
public:
	CSGNode( const std::string & name, GeometryNode *a, GeometryNode *b );
	virtual ~CSGNode();
	Intersection virtual *intersect(const glm::vec4 &eye, const glm::vec4 &ray) = 0;

protected:
	GeometryNode *m_a;
	GeometryNode *m_b;
};

class UnionNode : public CSGNode {
public:
	UnionNode(const std::string & name, GeometryNode *a, GeometryNode *b );
	virtual ~UnionNode();
	Intersection virtual *intersect(const glm::vec4 &eye, const glm::vec4 &ray);
};

class IntersectionNode : public CSGNode {
public:
	IntersectionNode(const std::string & name, GeometryNode *a, GeometryNode *b );
	virtual ~IntersectionNode();
	Intersection virtual *intersect(const glm::vec4 &eye, const glm::vec4 &ray);
};

class DifferenceNode : public CSGNode {
public:
	DifferenceNode(const std::string & name, GeometryNode *a, GeometryNode *b );
	virtual ~DifferenceNode();
	Intersection virtual *intersect(const glm::vec4 &eye, const glm::vec4 &ray);
};
