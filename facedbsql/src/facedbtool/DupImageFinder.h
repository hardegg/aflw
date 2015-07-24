/*
 * DupImageFinder.h
 *
 *  Created on: 05.12.2011
 *      Author: pwohlhart
 */

#ifndef DUPIMAGEFINDER_H_
#define DUPIMAGEFINDER_H_

#include "boost/program_options.hpp"
#include "../dbconn/SQLiteDBConnection.h"

namespace po = boost::program_options;

class DupImageFinder {
public:
	DupImageFinder();
	virtual ~DupImageFinder();

	void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);

protected:
	SQLiteDBConnection *_sqlConn;

	void findDupImages(bool doDelete);
};

#endif /* DUPIMAGEFINDER_H_ */
