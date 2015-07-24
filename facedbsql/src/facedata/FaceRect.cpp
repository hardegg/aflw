/*
 * FaceRect.cpp
 *
 *  Created on: 06.12.2011
 *      Author: pwohlhart
 */

#include "FaceRect.h"

FaceRect::FaceRect() :
	x(-1),
	y(-1),
	w(0),
	h(0),
	annotType(-1)
{
}

FaceRect::~FaceRect() {
}


bool FaceRect::equals(const FaceRect& other)
{
	return (x == other.x) && (y == other.y) && (w == other.w) && (h == other.h) && (annotType == other.annotType);
}
