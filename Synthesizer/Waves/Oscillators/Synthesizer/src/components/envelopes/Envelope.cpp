#include "Envelope.hpp"

Sample operator>>(Sample b, Envelope& a)
{
    return a.Apply(b);
}