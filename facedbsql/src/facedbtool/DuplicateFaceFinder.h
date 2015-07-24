/*
 * DuplicateFinder.h
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#ifndef DUPLICATEFINDER_H_
#define DUPLICATEFINDER_H_

#include "boost/program_options.hpp"
#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../facedata/FeatureCoordTypes.h"

namespace po = boost::program_options;


class DuplicateFaceFinder {
public:
	DuplicateFaceFinder();
	virtual ~DuplicateFaceFinder();

	void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);
private:
	SQLiteDBConnection *_sqlConn;
	FeatureCoordTypes _fcoordTypes;
	std::vector<int> _fCoordTypeIds;

	void findDups(std::vector<int> faceIds, std::vector<int> &exactDuplicates, std::vector<int> &nearDuplicates);

	bool hasSameCoords(FaceData *f2, FaceData *f1);
	bool hasSimilarCoords(FaceData *f2, FaceData *f1);

	bool isContainedIn(FeaturesCoords *f1coords, FeaturesCoords *f2coords);
	bool haveSamePose(FaceData *f1, FaceData *f2);
	bool haveSameRects(FaceData *f1, FaceData *f2);
};

#endif /* DUPLICATEFINDER_H_ */
