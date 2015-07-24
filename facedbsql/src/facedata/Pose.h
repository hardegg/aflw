/*
 * Pose.h
 *
 *  Created on: 06.12.2012
 *      Author: pwohlhart
 */

#ifndef POSE_H_
#define POSE_H_

class Pose {
public:
	Pose();
	virtual ~Pose();

	double yaw;
	double pitch;
	double roll;

	int annotTypeID;
	static const double UNDEFINED_ANGLE;

	bool equals(const Pose& other);
};

#endif /* POSE_H_ */
