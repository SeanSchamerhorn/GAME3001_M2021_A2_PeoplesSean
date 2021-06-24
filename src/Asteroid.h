#pragma once
#ifndef __ASTEROID__
#define __ASTEROID__
#include "NavigationObject.h"

class Asteroid : public NavigationObject
{
public:
	// Constructor
	Asteroid();

	// Destructor
	~Asteroid();

	// Life Cycle functions Inherited from DisplayObject
	void draw() override;
	void update() override;
	void clean() override;
private:
	
};

#endif
