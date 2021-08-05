#include "Background.h"
#include "TextureManager.h"

Background::Background()
{
	TextureManager::Instance().load("../Assets/textures/star_background.png", "background");
	auto size = TextureManager::Instance().getTextureSize("background");
	setWidth(size.x);
	setHeight(size.y);

	getRigidBody()->isColliding = false;
	setType(BACKGROUND);
}

Background::~Background()
= default;

void Background::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the background
	TextureManager::Instance().draw("background", x, y, 0, 255, false);
}

void Background::update()
{
}

void Background::clean()
{
}
