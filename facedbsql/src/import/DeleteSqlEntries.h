/*
 * DeleteSqlEntries.h
 *
 *  Created on: 11.08.2012
 *      Author: szweimueller
 */

#ifndef DELETESQLENTRIES_H_
#define DELETESQLENTRIES_H_

#include "../dbconn/SQLiteDBConnection.h"

#include <string>
#include <vector>


class DeleteSqlEntries {
public:
    DeleteSqlEntries();
    virtual ~DeleteSqlEntries();

    bool deleteDBName(std::string sqlDBFile, std::string dbName);
    bool insertDBName(std::string sqlDBFile, std::string dbName);

    std::vector<int> getEntryCount(SQLiteDBConnection *sqlConn, std::string stmtStr);
    bool deleteAllSqlEntries(std::string sqlDBFile);

    // IN
	std::vector<int> m_faceIds;
    std::vector<int> m_imageIds;

	// OUT
	std::vector<bool> m_faceRowAffected;
    std::vector<bool> m_imgRowAffected;
};

#endif /* DELETESQLENTRIES_H_ */
