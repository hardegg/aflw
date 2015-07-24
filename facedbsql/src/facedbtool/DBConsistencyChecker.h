/*
 * DBConsistencyChecker.h
 *
 *  Created on: 05.12.2011
 *      Author: pwohlhart
 */

#ifndef DBCONSISTENCYCHECKER_H_
#define DBCONSISTENCYCHECKER_H_

#include <vector>

#include "boost/program_options.hpp"
#include "../dbconn/SQLiteDBConnection.h"

#include "DupImageFinder.h"

namespace po = boost::program_options;

class DBConsistencyChecker {
public:
	DBConsistencyChecker();
	virtual ~DBConsistencyChecker();

	void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);

protected:
	SQLiteDBConnection *_sqlConn;
	bool _doRepair;

	void findImgsWOFaces();
};

#endif /* DBCONSISTENCYCHECKER_H_ */
