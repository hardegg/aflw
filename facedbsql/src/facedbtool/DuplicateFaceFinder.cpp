/*
 * DuplicateFinder.cpp
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#include "DuplicateFaceFinder.h"

#include "../facedata/FaceDbImage.h"
#include "../facedata/FaceMetadata.h"
#include "../facedata/FaceRect.h"
#include "../querys/ImagesWithFacesQuery.h"
#include "../querys/FaceIDsByImageQuery.h"
#include "../querys/FaceDataByIDsQuery.h"
#include "../querys/FaceRectsByFaceIDQuery.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "cv.h"

DuplicateFaceFinder::DuplicateFaceFinder() : _sqlConn(0)
{
}

DuplicateFaceFinder::~DuplicateFaceFinder()
{
}

void DuplicateFaceFinder::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	_sqlConn = sqlConn;

	_fcoordTypes.load(_sqlConn);
	_fCoordTypeIds = _fcoordTypes.getTypeIds();

	std::string imgDBName = "";
	if (vm.count("imgdbname"))
		imgDBName = vm["imgdbname"].as<string>();

	bool doDelete = false;
	if (vm.count("dup-action"))
		doDelete = vm["dup-action"].as<string>().compare("delete") == 0;

	bool reportToFile = false;
	ofstream reportFile;
	if (vm.count("report-file") > 0)
	{
		string reportFileName = vm["report-file"].as<string>();
		reportFile.open(reportFileName.c_str());
		reportToFile = true;
	}

	bool reportSQL = false;
	if ((vm.count("report-format") > 0) && (reportToFile))
	{
		reportSQL = vm["report-format"].as<string>() == "sql";
		reportFile << "BEGIN TRANSACTION;" << endl;
		reportFile << "DROP TABLE IF EXISTS \"NearDuplicates\"" << endl;
		reportFile << "CREATE TABLE \"NearDuplicates\" (\"face_id\" INTEGER PRIMARY KEY NOT NULL )" << endl;
	}

	ImagesWithFacesQuery imgsQuery;
	imgsQuery.db_id = imgDBName;
	if (!imgsQuery.exec(sqlConn))
	{
		std::cout << "ERROR: Image query failed" << std::endl;
		return;
	}

	std::vector<int> exactDupFacesIds;
	std::vector<int> nearDupFacesIds;

	std::vector<FaceDbImage>::iterator it;
	for (it = imgsQuery.resultImages.begin(); it != imgsQuery.resultImages.end(); ++it)
	{
		FaceIDsByImageQuery fidByImgQuery;
		fidByImgQuery.img_db_id = it->db_id;
		fidByImgQuery.img_file_id = it->file_id;
		fidByImgQuery.exec(sqlConn);

		if (fidByImgQuery.resultFaceIds.size() > 1)
		{
			//	cout << it->db_id << ", " << it->file_id << "#faces = " << fidByImgQuery.resultFaceIds.size() << endl;
			std::vector<int> exactDups;
			std::vector<int> nearDups;
			findDups(fidByImgQuery.resultFaceIds,exactDups,nearDups);

			if (nearDups.size() + exactDups.size() > 0)
			{
				cout << "img '" << it->db_id << "," << it->file_id << "', " << it->filepath << endl;

				std::vector<int>::iterator face_idIt;
				cout << "  face_ids: ";
				for (face_idIt = fidByImgQuery.resultFaceIds.begin(); face_idIt != fidByImgQuery.resultFaceIds.end(); ++face_idIt)
					 cout << *face_idIt << ", ";
				cout << endl;


				cout << "  exact duplicates: ";
				for (face_idIt = exactDups.begin(); face_idIt != exactDups.end(); ++face_idIt)
					cout << *face_idIt << ",";
				cout << endl;
				cout << "  near duplicates: ";
				for (face_idIt = nearDups.begin(); face_idIt != nearDups.end(); ++face_idIt)
					cout << *face_idIt << ",";
				cout << endl;

				if (reportToFile)
				{
					if (reportSQL)
					{
						std::sort( nearDups.begin(), nearDups.end() );
						int lastID = -1;
						for (face_idIt = nearDups.begin(); face_idIt != nearDups.end(); ++face_idIt)
						{
							if (*face_idIt != lastID)
								reportFile << "INSERT INTO NearDuplicates (face_id) VALUES (" <<  *face_idIt << ");" << endl;
						}
					}
					else
					{
						reportFile << it->db_id << "," << it->file_id << "," << it->filepath << " (" << it->image_id << "): ";
						for (face_idIt = exactDups.begin(); face_idIt != exactDups.end(); ++face_idIt)
							reportFile << *face_idIt << "!, ";
						for (face_idIt = nearDups.begin(); face_idIt != nearDups.end(); ++face_idIt)
							reportFile << *face_idIt << ", ";
						reportFile << endl;
					}
				}

				exactDupFacesIds.insert(exactDupFacesIds.end(),exactDups.begin(),exactDups.end());
				nearDupFacesIds.insert(nearDupFacesIds.end(),nearDups.begin(),nearDups.end());
			}

		}
	}

	if (doDelete)
	{
		cout << "Deleting exact duplicate faces" << endl;
		bool allOK = true;
		cout << "  preparing statements" << endl;
		SQLiteStmt *delFaceStmt = _sqlConn->prepare("DELETE FROM Faces WHERE face_id = ?1");
		SQLiteStmt *delEllipseStmt = _sqlConn->prepare("DELETE FROM FaceEllipse WHERE face_id = ?1");
		SQLiteStmt *delPoseStmt = _sqlConn->prepare("DELETE FROM FacePose WHERE face_id = ?1");
		SQLiteStmt *delRectStmt = _sqlConn->prepare("DELETE FROM FaceRect WHERE face_id = ?1");
		SQLiteStmt *delFeatureCoordsStmt = _sqlConn->prepare("DELETE FROM FeatureCoords WHERE face_id = ?1");
		cout << "  begin" << endl;
		_sqlConn->beginTransaction();
		cout << "  transaction" << endl;
		for (int i = 0; i < exactDupFacesIds.size(); ++i)
		{
			int face_id = exactDupFacesIds[i];

			delFaceStmt->reset();
			delFaceStmt->bind(1,face_id);
			allOK = allOK && (_sqlConn->step(delFaceStmt) == SQLITE_DONE);
			if (!allOK) break;

			delEllipseStmt->reset();
			delEllipseStmt->bind(1,face_id);
			allOK = allOK && (_sqlConn->step(delEllipseStmt) == SQLITE_DONE);
			if (!allOK) break;

			delPoseStmt->reset();
			delPoseStmt->bind(1,face_id);
			allOK = allOK && (_sqlConn->step(delPoseStmt) == SQLITE_DONE);
			if (!allOK) break;

			delRectStmt->reset();
			delRectStmt->bind(1,face_id);
			allOK = allOK && (_sqlConn->step(delRectStmt) == SQLITE_DONE);
			if (!allOK) break;

			delFeatureCoordsStmt->reset();
			delFeatureCoordsStmt->bind(1,face_id);
			allOK = allOK && (_sqlConn->step(delFeatureCoordsStmt) == SQLITE_DONE);
			if (!allOK) break;
		}
		if (allOK)
		{
			cout << "  commit" << endl;
			_sqlConn->commitTransaction();
		}
		else
		{
			cout << "  some error -> rollback" << endl;
			_sqlConn->rollbackTransaction();
		}
		cout << "done." << endl;

		delete delFaceStmt;
		delete delEllipseStmt;
		delete delPoseStmt;
		delete delRectStmt;
		delete delFeatureCoordsStmt;
	}

	if (reportToFile)
	{
		if (reportSQL)
			reportFile << "COMMIT;" << endl;
		reportFile.close();
	}

	_sqlConn = 0;
}

void DuplicateFaceFinder::findDups(std::vector<int> faceIds, std::vector<int> &duplicates, std::vector<int> &nearDuplicates)
{
	std::sort(faceIds.begin(),faceIds.end());

	FaceDataByIDsQuery faceDataQuery;
	faceDataQuery.queryIds = faceIds;
	if (faceDataQuery.exec(_sqlConn))
	{
		for (int i = faceIds.size()-1; i > 0; --i)
		{
			int f1Id = faceIds[i];
			// check if f1 is already marked as a duplicate
			if (std::find(duplicates.begin(),duplicates.end(),f1Id) != duplicates.end())
				continue;

			FaceData *f1 = faceDataQuery.data[f1Id];

			for (int j = i-1; j >= 0; --j)
			{
				int f2Id = faceIds[j];
				// check if f2 is already marked as a duplicate
				if (std::find(duplicates.begin(),duplicates.end(),f2Id) != duplicates.end())
					continue;

				FaceData *f2 = faceDataQuery.data[f2Id];

				// check if f2 is a duplicate of f1
				if (hasSameCoords(f1,f2))  // check coordinates
				{
					bool exactDup = true;
					// check if metadata is the same
					FaceMetadata * m1 = f1->getMetadata();
					FaceMetadata * m2 = f2->getMetadata();
					if (m1 && m2)
					{
						cout << "md " << f1->ID << ": sex=" << m1->sex << ", occ=" << m1->occluded << ", gl=" << m1->glasses << ", bw=" << (m1->bw) << ", type=" << m1->annot_type_id << endl;
						cout << "md " << f2->ID << ": sex=" << m2->sex << ", occ=" << m2->occluded << ", gl=" << m2->glasses << ", bw=" << (m2->bw) << ", type=" << m2->annot_type_id << endl;

						if (!m1->equals(*m2))
							exactDup = false;
					}
					else
					{
						exactDup = false;
						cout << "face " << f2->ID << " is a dup of " << f1->ID << endl;
						if ((m1 != 0) && (m2 == 0))
							cout << "   " << f1->ID << " has metadata, " << f2->ID << "not" << endl;
						if ((m1 == 0) && (m2 != 0))
							cout << "   " << f2->ID << " has metadata, " << f1->ID << "not" << endl;
					}

					// ellipse
					if (exactDup)
						if (f1->hasEllipse() && (f2->hasEllipse()))
						{
							vector<FaceEllipse> f1es = f1->getEllipses();
							if (f1es.size() != f2->getEllipses().size())
								exactDup = false;
							else
								for (int k = 0; k < f1es.size(); ++k)
								{
									FaceEllipse f2e = f2->getEllipse(f1es[k].annot_type_id);
									if (!f1es[k].equals(f2e))
										exactDup = false;
								}
						}
						else
							if (f1->hasEllipse() || f2->hasEllipse())
								exactDup = false;

					// pose
					if (exactDup)
						if (!haveSamePose(f1,f2))
							exactDup = false;

					if (exactDup)
						if (!haveSameRects(f1,f2))
							exactDup = false;

					if (exactDup)
						duplicates.push_back(f2Id);
					else
					{
						if (std::find(nearDuplicates.begin(),nearDuplicates.end(),f1Id) == nearDuplicates.end())
							nearDuplicates.push_back(f1Id);
						if (std::find(nearDuplicates.begin(),nearDuplicates.end(),f2Id) == nearDuplicates.end())
							nearDuplicates.push_back(f2Id);
					}
				}
				else {
					if (hasSimilarCoords(f1,f2))
					{
						if (std::find(nearDuplicates.begin(),nearDuplicates.end(),f1Id) == nearDuplicates.end())
							nearDuplicates.push_back(f1Id);
						if (std::find(nearDuplicates.begin(),nearDuplicates.end(),f2Id) == nearDuplicates.end())
							nearDuplicates.push_back(f2Id);
					}
				}
			}
		}
	}

}

bool DuplicateFaceFinder::hasSameCoords(FaceData *f2, FaceData *f1)
{
	//  make sure feature coords are loaded
	if (!f1->getFeaturesCoords())
		f1->loadFeatureCoords(_sqlConn);
	if (!f2->getFeaturesCoords())
		f2->loadFeatureCoords(_sqlConn);

	FeaturesCoords *f1coords = f1->getFeaturesCoords();
	FeaturesCoords *f2coords = f2->getFeaturesCoords();

	if ( (!f1coords) || (!f2coords) )
	{
		cout << "Couldn't load feature coordinates for all faces" << endl;
		return false;
	}

	// check if they are exactly the same
	bool same = true;
	std::vector<int>::iterator it;
	for (it = _fCoordTypeIds.begin(); it != _fCoordTypeIds.end(); ++it)
	{
		cv::Point2f pt1 = f1coords->getCoords(*it);
		cv::Point2f pt2 = f2coords->getCoords(*it);
		if ((pt1.x != pt2.x) || (pt1.y != pt2.y))
		{
			same = false;
			break;
		}
	}
	if (same)
		return true;

	return false;
}

bool DuplicateFaceFinder::hasSimilarCoords(FaceData *f2, FaceData *f1)
{
	//  make sure feature coords are loaded
	if (!f1->getFeaturesCoords())
		f1->loadFeatureCoords(_sqlConn);
	if (!f2->getFeaturesCoords())
		f2->loadFeatureCoords(_sqlConn);

	FeaturesCoords *f1coords = f1->getFeaturesCoords();
	FeaturesCoords *f2coords = f2->getFeaturesCoords();

	bool overlap = false;
	overlap = isContainedIn(f1coords,f2coords);
	if (!overlap)
		overlap = isContainedIn(f2coords,f1coords);
	return overlap;
}

bool DuplicateFaceFinder::isContainedIn(FeaturesCoords *f1coords, FeaturesCoords *f2coords)
{
	bool overlap = false;

	// check if one of the points of f2 lies in the convex hull of points of f1

	std::vector<cv::Point2f> f1Points;
	std::vector<int>::iterator it;
	for (it = _fCoordTypeIds.begin(); it != _fCoordTypeIds.end(); ++it)
	{
		cv::Point2f pt = f1coords->getCoords(*it);
		if ((pt.x >= 0) && (pt.y >= 0))
			f1Points.push_back(pt);
	}
	cv::Mat f1PointsMat = cv::Mat(f1Points);

	std::vector<cv::Point2f> f1ConvexHull;
	cv::convexHull(f1PointsMat,f1ConvexHull,false);

	cv::Mat f1ConvexHullMat = cv::Mat(f1ConvexHull);

	for (it = _fCoordTypeIds.begin(); it != _fCoordTypeIds.end(); ++it)
	{
		cv::Point2f pt = f2coords->getCoords(*it);
		if (cv::pointPolygonTest(f1ConvexHullMat,pt,false) >= 0)
		{
			overlap = true;
			break;
		}
	}

	return overlap;
}


bool DuplicateFaceFinder::haveSamePose(FaceData *f1, FaceData *f2)
{
	FacePose *fp1 = f1->getPose();
	FacePose *fp2 = f2->getPose();
	if (fp1 && fp2)
		return fp1->equals(*fp2);
	else
		return (fp1 == fp2);
}

bool DuplicateFaceFinder::haveSameRects(FaceData *f1, FaceData *f2)
{
	FaceRectsByFaceIDQuery frq;
	frq.setFaceID(f1->ID);
	frq.exec(_sqlConn);
	vector<FaceRect> f1rects = frq.faceRects;

	frq.clear();
	frq.setFaceID(f2->ID);
	frq.exec(_sqlConn);
	vector<FaceRect> f2rects = frq.faceRects;

	if (f1rects.size() != f2rects.size())
		return false;

	// check if for each rectangle in f1rects there is one exactly same in f2rects
	bool foundAll = true;
	int numRects = f1rects.size();
	vector<bool> isTaken(numRects,false);
	for (int i = 0; i < numRects; ++i)
	{
		bool foundOne = false;
		for (int j = 0; j < numRects; ++j)
		{
			if (isTaken[j])
				continue;

			if (f1rects[i].equals(f2rects[j]))
			{
				foundOne = true;
				isTaken[j] = true;
				break;
			}
		}
		if (!foundOne)
		{
			foundAll = false;
			break;
		}
	}

	return foundAll;
}
