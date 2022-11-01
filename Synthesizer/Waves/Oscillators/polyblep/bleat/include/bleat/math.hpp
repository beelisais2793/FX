#pragma once 

namespace bleat {
namespace math {

template <class T>
constexpr T lerp(T a, T b, T x)
{
	return (x * (b - a)) + a;
}

template <class T>
constexpr T inverse_lerp(T a, T b, T x)
{
	return (x - a) / (b - a);
}

}}