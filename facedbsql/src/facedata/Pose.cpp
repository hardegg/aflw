/*
 * Pose.cpp
 *
 *  Created on: 06.12.2012
 *      Author: pwohlhart
 */

#include "Pose.h"

const double Pose::UNDEFINED_ANGLE = - 2. * 3.14159265358979323846;

Pose::Pose() :
		yaw(UNDEFINED_ANGLE),
		pitch(UNDEFINED_ANGLE),
		roll(UNDEFINED_ANGLE),
		annotTypeID(0)
{

}

Pose::~Pose() {

}

bool Pose::equals(const Pose& other)
{
	return (yaw == other.yaw) && (pitch == other.pitch) && (roll = other.roll) && (annotTypeID == other.annotTypeID);
}




