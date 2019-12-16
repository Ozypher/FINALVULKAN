#ifndef __cube_h__
#define __cube_h__
#include "gfc_vector.h"
#include "gf3d_entity.h"
typedef struct {
	Vector3D	boxpos;
	Vector3D	boxext;

}box;

int gf3d_colliding(box a, box b);
void bullet();

#endif