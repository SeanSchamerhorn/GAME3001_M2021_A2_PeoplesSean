#include "PathNode.h"
#include "TextureManager.h"

Node::Node()
{
	TextureManager::Instance().load("../Assets/textures/target.png", "node");
	auto size = TextureManager::Instance().getTextureSize("node");
	setWidth(size.x);
	setHeight(size.y);

	getRigidBody()->isColliding = false;
	setType(NODE);
}

Node::~Node()
= default;


void Node::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the nodes
	TextureManager::Instance().draw("node", x, y, 0, 255, true);
}

void Node::update()
{
}

void Node::clean()
{
}
