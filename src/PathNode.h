#pragma once
#ifndef __PATH_NODE__
#define __PATH_NODE__
#include "NavigationObject.h"

class Node : public NavigationObject
{
public:
	// Constructor
	Node();

	// Destructor
	~Node();

	// Life Cycle functions Inherited from DisplayObject
	void draw() override;
	void update() override;
	void clean() override;
private:

};

#endif