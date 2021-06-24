#include "Asteroid.h"
#include "TextureManager.h"

Asteroid::Asteroid()
{
	TextureManager::Instance().load("../Assets/textures/asteroid_small.png", "roid");
	auto size = TextureManager::Instance().getTextureSize("roid");
	setWidth(size.x);
	setHeight(size.y);

	getRigidBody()->isColliding = false;
	setType(ASTEROID);
}

Asteroid::~Asteroid()
= default;


void Asteroid::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the asteroids
	TextureManager::Instance().draw("roid", x, y, 0, 255, true);
	
}

void Asteroid::update()
{
}

void Asteroid::clean()
{
}
