/*
 * DeleteSqlEntries.h
 *
 *  Created on: 11.08.2012
 *      Author: szweimueller
 */

#include "DeleteSqlEntries.h"

#include <boost/lexical_cast.hpp>

DeleteSqlEntries::DeleteSqlEntries()
{
}

DeleteSqlEntries::~DeleteSqlEntries()
{
}

bool DeleteSqlEntries::deleteDBName(std::string sqlDBFile, std::string dbName)
{
    SQLiteDBConnection sqlConn;

    std::cout << "\nDeleteSqlEntries::deleteDBName: '" << dbName << "' in SQL-DB: '" << sqlDBFile << "'\n" << std::endl;

    if (sqlConn.open(sqlDBFile)) {
        sqlConn.exec("PRAGMA synchronous = OFF;");
	    sqlConn.exec("BEGIN TRANSACTION;");

        string deleteDBNameSqlStmtStr = "DELETE FROM [Databases] WHERE db_id = (?1)";
	    SQLiteStmt *deleteDBNameSqlStmt = sqlConn.prepare(deleteDBNameSqlStmtStr);

        deleteDBNameSqlStmt->reset();
        deleteDBNameSqlStmt->bind(1,dbName);

        bool isOK = true;   
        isOK = isOK && (sqlConn.step(deleteDBNameSqlStmt) == SQLITE_DONE);
		if (!isOK) {
			std::cout << "DBNameInSqlFile::deleteDBName -> Error: could not DELETE " << dbName << " from Databases. " << std::endl;
        }
                
        sqlConn.exec("COMMIT;");
		sqlConn.finalize(deleteDBNameSqlStmt);
		delete deleteDBNameSqlStmt;
        sqlConn.close();

    } else {
        std::cout << "DBNameInSqlFile::deleteDBName -> Warning: Failed to open sql db file '" << sqlDBFile << "'" << std::endl;
        return false;
    }
    
    return true;
}

bool DeleteSqlEntries::insertDBName(std::string sqlDBFile, std::string dbName)
{
    SQLiteDBConnection sqlConn;

    if (sqlConn.open(sqlDBFile)) {
        sqlConn.exec("PRAGMA synchronous = OFF;");
	    sqlConn.exec("BEGIN TRANSACTION;");

        string insertDatabaseSqlStmtStr = "INSERT INTO Databases(db_id,path,description) VALUES (?1,?2,?3)";
        SQLiteStmt *insertDatabaseSqlStmt = sqlConn.prepare(insertDatabaseSqlStmtStr);       

        insertDatabaseSqlStmt->reset();
        insertDatabaseSqlStmt->bind(1,dbName);
        insertDatabaseSqlStmt->bind(2,dbName + "/");
	    insertDatabaseSqlStmt->bind(3,dbName + " database");	
        bool isOK = true;
        isOK = isOK && (sqlConn.step(insertDatabaseSqlStmt) == SQLITE_DONE);
		if (!isOK) {
			std::cout << "DBNameInSqlFile::insertDBName -> Error: could not ADD " << dbName << " to Databases. " << std::endl;
        }
       
        sqlConn.exec("COMMIT;");
		sqlConn.finalize(insertDatabaseSqlStmt);
		delete insertDatabaseSqlStmt;
        sqlConn.close();

    } else {
        std::cout << "DBNameInSqlFile::insertDBName -> Warning: Failed to open sql db file '" << sqlDBFile << "'" << std::endl;
        return false;
    }

    return true;
}

std::vector<int> DeleteSqlEntries::getEntryCount(SQLiteDBConnection *sqlConn, std::string stmtStr)
{
        std::vector<int> idVector;
        SQLiteStmt *countStmt = sqlConn->prepare(stmtStr);
        countStmt->reset();

        int rc = 0;
        do
        {
            rc = sqlConn->step(countStmt);
            if (rc == SQLITE_ROW)
            {
                int faceId;
                countStmt->readIntColumn(0, faceId);
                idVector.push_back(faceId);
                std::cout << " id pushed to vector: " << faceId << std::endl;
            }
        } while (rc == SQLITE_ROW);

        sqlConn->finalize(countStmt);
        delete countStmt;

        return idVector;
}

bool DeleteSqlEntries::deleteAllSqlEntries(std::string sqlDBFile)
{
    std::cout << "deleteSqlEntries ('" << sqlDBFile << "')" << std::endl;
    SQLiteDBConnection sqlConn;
    
    bool allOK = true;
	if (sqlConn.open(sqlDBFile))
	{
        m_faceIds.clear();

        string faceCountStmtStr = "SELECT face_id FROM [Faces]";
        std::vector<int> m_faceIds = getEntryCount(&sqlConn, faceCountStmtStr);

        m_faceRowAffected = std::vector<bool>(m_faceIds.size(), false);

           
        sqlConn.exec("PRAGMA synchronous = OFF;");
		sqlConn.exec("BEGIN TRANSACTION;");

        std::string deleteDBSqlStmtStr = "DELETE FROM [Databases]";
	    SQLiteStmt *deleteDBSqlStmt = sqlConn.prepare(deleteDBSqlStmtStr);
        
        deleteDBSqlStmt->reset();
        allOK = allOK && (sqlConn.step(deleteDBSqlStmt) == SQLITE_DONE);
		if (!allOK) {
			std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all DATABASES entries from SQLDatabase. " << std::endl;
        }


        std::string deleteFaceSqlStmtStr = "DELETE FROM [Faces] WHERE face_id = ?1";
	    std::string deleteFeatureCoordsSqlStmtStr = "DELETE FROM [FeatureCoords] WHERE face_id = ?1";
        std::string deleteEllipseSqlStmtStr = "DELETE FROM [FaceEllipse] WHERE face_id = ?1";
        std::string deletePoseSqlStmtStr = "DELETE FROM [FacePose] WHERE face_id = ?1";
        std::string deleteMetaDataSqlStmtStr = "DELETE FROM [FaceMetadata] WHERE face_id = ?1";
        std::string deleteRectSqlStmtStr = "DELETE FROM [FaceRect] WHERE face_id = ?1";
	    SQLiteStmt *deleteFaceSqlStmt = sqlConn.prepare(deleteFaceSqlStmtStr);
	    SQLiteStmt *deleteFeatureCoordsSqlStmt = sqlConn.prepare(deleteFeatureCoordsSqlStmtStr);
	    SQLiteStmt *deleteEllipseSqlStmt = sqlConn.prepare(deleteEllipseSqlStmtStr);
	    SQLiteStmt *deletePoseSqlStmt = sqlConn.prepare(deletePoseSqlStmtStr);
        SQLiteStmt *deleteMetaDataSqlStmt = sqlConn.prepare(deleteMetaDataSqlStmtStr);
	    SQLiteStmt *deleteRectSqlStmt = sqlConn.prepare(deleteRectSqlStmtStr);

        int changes;
        for(unsigned int counter = 0; counter < m_faceIds.size(); ++counter)
	    {
            deleteFaceSqlStmt->reset();
            deleteFaceSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteFaceSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FACES entries from SQLDatabase. " << std::endl;
                break;
            }

            deleteFeatureCoordsSqlStmt->reset();
            deleteFeatureCoordsSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteFeatureCoordsSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FEATURECOORDS entries from SQLDatabase. " << std::endl;
                break;
            }
            
            deleteEllipseSqlStmt->reset();  
            deleteEllipseSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteEllipseSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FACEELLIPSE entries from SQLDatabase. " << std::endl;
                break;
            }

            deleteMetaDataSqlStmt->reset();
            deleteMetaDataSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteMetaDataSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FACEMETADATA entries from SQLDatabase. " << std::endl;
                break;
            }

            deletePoseSqlStmt->reset();
            deletePoseSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deletePoseSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FACEPOSE entries from SQLDatabase. " << std::endl;
                break;
            }

            deleteRectSqlStmt->reset();  
            deleteRectSqlStmt->bind(1,m_faceIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteRectSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all FACERECT entries from SQLDatabase. " << std::endl;
                break;            
            }
            
            changes = sqlConn.changes();
		    if(changes == 1) {
			    m_faceRowAffected.at(counter) = true;
		    }
		    else if(changes > 1) {
			    std::cout << "ERROR: MORE THAN ONE ROW AFFECTED DELETING A FACE" << std::endl;
			    allOK = false;
			    break;
		    }
		    else {
			    m_faceRowAffected.at(counter) = false;
            }
        }
        

        std::string deleteImageSqlStmtStr = "DELETE FROM [FaceImages] WHERE image_id = ?1";
        SQLiteStmt *deleteImageSqlStmt = sqlConn.prepare(deleteImageSqlStmtStr);
     
        m_imageIds.clear();

        string imagCountStmtStr = "SELECT image_id FROM [FaceImages]";
        std::vector<int> m_imageIds = getEntryCount(&sqlConn, imagCountStmtStr);
        
        m_imgRowAffected = std::vector<bool>(m_imageIds.size(), false);

        changes = 0;
        for (unsigned int counter = 0; counter < m_imageIds.size(); ++counter) 
        {
            deleteImageSqlStmt->reset();  
            deleteImageSqlStmt->bind(1,m_imageIds.at(counter));
            allOK = allOK && (sqlConn.step(deleteImageSqlStmt) == SQLITE_DONE);
		    if (!allOK) {
			    std::cout << "DeleteSqlEntries::deleteSqlEntries -> Error: could not DELETE all IMAGES entries from SQLDatabase. " << std::endl;
                break;
            }

            changes = sqlConn.changes();
            if(changes == 1) {
			    m_imgRowAffected.at(counter) = true;
		    }
		    else if(changes > 1) {
			    std::cout << "ERROR: MORE THAN ONE ROW AFFECTED DELETING A IMAGE" << std::endl;
			    allOK = false;
			    break;
		    }
		    else {
			    m_imgRowAffected.at(counter) = false;
            }
        }

        if (allOK) {
			sqlConn.exec("COMMIT;");
            std::cout << "DeleteSqlEntries::deleteSqlEntries -> Info: DELETING ALL SQL ENTRIES." << std::endl;
        }
		else {
			sqlConn.exec("ROLLBACK TRANSACTION;");
        }
        
	    sqlConn.exec("PRAGMA synchronous = NORMAL;");
       
        sqlConn.finalize(deleteDBSqlStmt);
		delete deleteDBSqlStmt;
		sqlConn.finalize(deleteFaceSqlStmt);
		delete deleteFaceSqlStmt;
        sqlConn.finalize(deleteFeatureCoordsSqlStmt);
		delete deleteFeatureCoordsSqlStmt;
        sqlConn.finalize(deletePoseSqlStmt);
		delete deletePoseSqlStmt;
        sqlConn.finalize(deleteMetaDataSqlStmt);
		delete deleteMetaDataSqlStmt;
        sqlConn.finalize(deleteImageSqlStmt);
		delete deleteImageSqlStmt;
        sqlConn.close();
    } else {
        std::cout << "DeleteSqlEntries::deleteSqlEntries -> Warning: Failed to open sql db file '" << sqlDBFile << "'" << std::endl;
        return false;
    }
    
    return allOK;
}