#pragma once
#include "poly.h"

inline polynomial operator * (polynomial lhs, const polynomial &rhs) {
	return lhs *= rhs;
}
inline polynomial operator * (polynomial lhs, const cmplx &rhs) {
	return lhs *= rhs;
}
inline polynomial operator * (const cmplx &rhs, polynomial lhs) {
	return lhs *= rhs;
}


inline polynomial operator + (polynomial lhs, const polynomial &rhs) {
	return lhs += rhs;
}
inline polynomial operator + (polynomial lhs, const cmplx &rhs) {
	return lhs += rhs;
}
inline polynomial operator + (const cmplx &rhs, polynomial lhs) {
	return lhs += rhs;
}


inline polynomial operator - (polynomial lhs, const polynomial &rhs) {
	return lhs -= rhs;
}
inline polynomial operator - (polynomial lhs, const cmplx &rhs) {
	return lhs -= rhs;
}
inline polynomial operator - (const cmplx &rhs, polynomial lhs) {
	return lhs -= rhs;
}


inline polynomial operator / (polynomial lhs, const polynomial &rhs) {
	return lhs /= rhs;
}
inline polynomial operator / (polynomial lhs, const cmplx &rhs) {
	return lhs /= rhs;
}
inline polynomial operator / (const cmplx &rhs, polynomial lhs) {
	return lhs /= rhs;
}


inline bool operator==(const polynomial& lhs, const polynomial& rhs) {
	return lhs.get_poly() == rhs.get_poly();
}
inline bool operator!=(const polynomial& lhs, const polynomial& rhs) {
	return !operator==(lhs, rhs);
}
