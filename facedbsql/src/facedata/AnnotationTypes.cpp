/*
 * AnnotationTypes.cpp
 *
 *  Created on: 15.01.2013
 *      Author: pwohlhart
 */

#include "AnnotationTypes.h"

#include <sstream>

AnnotationTypes::AnnotationTypes(SQLiteDBConnection *sqlConn) {

	stringstream sqlStmtStrm;
	sqlStmtStrm << "SELECT annot_type_id,code,description FROM AnnotationType";

	SQLiteStmt *queryStmt = sqlConn->prepare(sqlStmtStrm.str());

	int res = 0;
	do {
		res = sqlConn->step(queryStmt);
		bool allOK = true;
		if (res == SQLITE_ROW)
		{
			AnnotType at;
			allOK = allOK && queryStmt->readIntColumn(0,at.id);
			allOK = allOK && queryStmt->readStringColumn(1,at.code);
			allOK = allOK && queryStmt->readStringColumn(2,at.description);
			if (allOK)
				_types[at.id] = at;
			else
				break;
		}
	} while (res == SQLITE_ROW);

}

AnnotationTypes::~AnnotationTypes() {
	// TODO Auto-generated destructor stub
}

int AnnotationTypes::getIDByCode(string code)
{
	for (_typesIt = _types.begin(); _typesIt != _types.end(); ++_typesIt)
	{
		if (code.compare((*_typesIt).second.code) == 0)
			return (*_typesIt).first;
	}
	return -1;
}

