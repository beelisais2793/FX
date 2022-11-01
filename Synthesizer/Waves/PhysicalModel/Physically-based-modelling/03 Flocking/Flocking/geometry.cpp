#include <math.h>
#include "geometry.h"
#include <Windows.h>
#define INF 100000.0

bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

bool onSegment_glm(glm::vec3 p, glm::vec3 q, glm::vec3 r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

int orientation(Point p, Point q, Point r)
{
	float val = (q.y - p.y) * (r.x - p.x) -
		(q.x - p.x) * (r.y - p.y);

	if (fabs(val) < 1e-6) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool doIntersect_glm(glm::vec3 P1, glm::vec3 Q1, glm::vec3 P2, glm::vec3 Q2)
{
	Point p1 = { P1.x,P1.y,P1.z }, p2 = { P2.x,P2.y,P2.z }, q1 = { Q1.x,Q1.y,Q1.z }, q2 = { Q2.x,Q2.y,Q2.z };
	// Find the four orientations needed for general and special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool isInside(Point polygon[], int n, Point p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	Point extreme(INF, p.y);

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;

		if (doIntersect(polygon[i], polygon[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (orientation(polygon[i], p, polygon[next]) == 0)
				return onSegment(polygon[i], p, polygon[next]);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}

bool isInside_yz(Point polygon[], int n, Point p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	Point extreme(INF, p.z), new_p,new_pn, point_new;

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;
		new_p.x = polygon[i].x; new_pn.x = polygon[next].x;
		new_p.y = polygon[i].z; new_p.z = polygon[i].x;
		new_pn.y = polygon[next].z; new_pn.z = polygon[next].x; 
		point_new.x = p.x;
		point_new.y = p.z;
		point_new.z = p.x;
		if (doIntersect(new_p, new_pn, point_new, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (orientation(new_p, p, new_pn) == 0)
				return onSegment(new_p, p, new_pn);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}

