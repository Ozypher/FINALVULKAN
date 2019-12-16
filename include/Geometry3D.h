#ifndef _H_GEOMETRY_3D_
#define _H_GEOMETRY_3D_
#include "gfc_matrix.h"

typedef Vector3D Point;
typedef struct Line{
	Point start;
	Point end;

	inline Line() {}
	inline Line(const Point& s, const Point& e) :
		start(s), end(e){ }
}Line;
float Length(const Line& line);
float LengthSq(const Line& line);
#endif // !_H_GEOMETRY_3D_
