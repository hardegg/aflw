/*
 * DBCreator.h
 *
 *  Created on: 31.08.2010
 *      Author: pwohlhart
 */

#ifndef DBCREATOR_H_
#define DBCREATOR_H_

#include <boost/program_options.hpp>
#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../facedata/FeatureCoordTypes.h"

namespace po = boost::program_options;

class DBCreator {
public:
	DBCreator();
	virtual ~DBCreator();

	void run(po::variables_map &vm);
private:
	SQLiteDBConnection *_sqlConn;
};

#endif /* DBCREATOR_H_ */
