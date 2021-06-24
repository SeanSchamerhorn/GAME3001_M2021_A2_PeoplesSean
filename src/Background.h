#pragma once
#ifndef __BACKGROUND__
#define __BACKGROUND__
#include "DisplayObject.h"

class Background : public DisplayObject
{
public:
	// Constructor
	Background();

	// Destructor
	~Background();

	// Life Cycle functions Inherited from DisplayObject
	void draw() override;
	void update() override;
	void clean() override;
private:

};
#endif

