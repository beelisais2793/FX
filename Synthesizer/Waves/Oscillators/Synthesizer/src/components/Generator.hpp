#pragma once
#include "Component.hpp"

struct Generator : public Component
{
	virtual Sample NextSample() = 0;
	operator Sample() { return NextSample(); }
	virtual Sample GetSample() = 0;
};