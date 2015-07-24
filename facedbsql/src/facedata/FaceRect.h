/*
 * FaceRect.h
 *
 *  Created on: 06.12.2011
 *      Author: pwohlhart
 */

#ifndef FACERECT_H_
#define FACERECT_H_

class FaceRect {
public:
	FaceRect();
	virtual ~FaceRect();

	int x;
	int y;
	int w;
	int h;
	int annotType;

	bool equals(const FaceRect& other);
};

#endif /* FACERECT_H_ */
