#include "Filter.hpp"
Filter& Filter::Cutoff(double c) 
{
	cutoff = c;
	return *this;
}