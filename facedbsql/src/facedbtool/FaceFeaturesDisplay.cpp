/*
 * FaceFeaturesDisplay.cpp
 *
 *  Created on: 02.08.2010
 *      Author: pwohlhart
 */

#include "FaceFeaturesDisplay.h"

//#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "boost/filesystem.hpp"
#include "highgui.h"

#include "../querys/ImageByFaceIDQuery.h"
#include "../querys/FaceDataByIDsQuery.h"
#include "../querys/FaceIDsByImageQuery.h"

#include "../facedata/FeaturesCoords.h"

namespace fs = boost::filesystem;

#ifndef M_PI
  #define M_PI           3.14159265358979323846
#endif

FaceFeaturesDisplay::FaceFeaturesDisplay() {

}

FaceFeaturesDisplay::~FaceFeaturesDisplay() {
}

void FaceFeaturesDisplay::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	std::vector<int> face_ids;
	int image_id = -1;
	std::string img_file_id;
	std::string img_db_id;

	if (vm.count("face-id") == 0)
	{
		if (vm.count("image-id") > 0)
		{
			image_id = vm["image-id"].as<int>();

			cout << "ERROR: query be image-id not implemented" << endl;
			return;
		}
		else
		{
			if ((vm.count("file-id") == 1) && (vm.count("db-id") == 1))
			{
				img_file_id = vm["file-id"].as<std::string>();
				img_db_id = vm["db-id"].as<std::string>();

				FaceIDsByImageQuery fidByImgQuery;
				fidByImgQuery.img_db_id = img_db_id;
				fidByImgQuery.img_file_id = img_file_id;
				fidByImgQuery.exec(sqlConn);

				if (fidByImgQuery.resultFaceIds.size() == 0)
				{
					cout << "ERROR: Couldn't find faces for image '" << img_db_id << "." << img_file_id << "'" << endl;
					return;
				}
				else
					face_ids = fidByImgQuery.resultFaceIds;
			}
			else
			{
				cout << "ERROR: face-id required." << endl;
				return;
			}
		}
	}
	else
		face_ids.push_back(vm["face-id"].as<int>());

	// get face data
	FaceDataByIDsQuery faceDataQuery;
	faceDataQuery.queryIds = face_ids;
	faceDataQuery.exec(sqlConn);

	if (faceDataQuery.data.size() == 0)
	{
		cout << "ERROR: Couldn't load face data for face-ids '";
		cout << face_ids[0];
		for (unsigned int i = 1; i < face_ids.size(); ++i)
			cout << "," << face_ids[i];
		cout << "'" << endl;
		return;
	}

	// get image data
	ImageByFaceIDQuery imageQuery;
	imageQuery.face_id = face_ids[0];
	imageQuery.exec(sqlConn);

	// load image
	std::string dbFile = vm["sqldb-file"].as<string>();
	std::string basePath = "";
	std::string::size_type pos = dbFile.find_last_of("\\/");
	if (pos != std::string::npos)
		basePath = dbFile.substr(0,pos+1);

	string fullFileName = basePath + imageQuery.resultImage.dbpath + imageQuery.resultImage.filepath;
	fs::path tmppath(fullFileName);
	fullFileName = tmppath.string();
	//fullFileName = tmppath.native_file_string();
	cout << "image path: " << fullFileName << endl;
	cv::Mat img = cv::imread(fullFileName);

	cout << "#faces: " << face_ids.size() << endl;
	cout << "  ";
	for (unsigned int i = 0; i < face_ids.size(); ++i)
	{
		FaceData *faceData = faceDataQuery.data[face_ids[i]];
		cout << face_ids[i] << ", ";

		if (!faceData->getFeaturesCoords())
			faceData->loadFeatureCoords(sqlConn);
		faceData->loadRects(sqlConn);

		// paint face data

		paintFaceData(faceData,img);
	}
	cout << endl;

	// show result

	cv::imshow(imageQuery.resultImage.filepath,img);
	cv::waitKey();

	cout << "done." << endl;
}


void FaceFeaturesDisplay::paintFaceData(FaceData *faceData,cv::Mat &img)
{
	// get feature coordinates
	FeaturesCoords *fcoords = faceData->getFeaturesCoords();

	// get convex hull of feature points
	std::vector<int> fcoordTypeIds = fcoords->getFeatureIds();

	std::vector<cv::Point2f> points;
	std::vector<int>::iterator it;
	for (it = fcoordTypeIds.begin(); it != fcoordTypeIds.end(); ++it)
	{
		cv::Point2f pt = fcoords->getCoords(*it);
		if ((pt.x >= 0) && (pt.y >= 0))
		{
			// paint little cross on the point
			cv::line(img,cv::Point(pt.x,pt.y-1),cv::Point(pt.x,pt.y+1),CV_RGB(255.0,255.0,0.0));
			cv::line(img,cv::Point(pt.x-1,pt.y),cv::Point(pt.x+1,pt.y),CV_RGB(255.0,255.0,0.0));
			points.push_back(pt);
		}
	}
	cv::Mat pointsMat = cv::Mat(points);
	std::vector<cv::Point2f> convexHull;
	cv::convexHull(pointsMat,convexHull);

	std::vector<cv::Point2f>::iterator hullIt;
	std::vector<cv::Point2f>::iterator nextIt;
	for (hullIt = convexHull.begin(); hullIt != convexHull.end(); ++hullIt)
	{
		nextIt = hullIt + 1;
		if (nextIt == convexHull.end())
			nextIt = convexHull.begin();


		cv::line(img,*hullIt,*nextIt,CV_RGB(255.0, 0.0, 0.0));
	}

	paintEllipse(faceData, img);

	paintRectangles(faceData, img);
}

void FaceFeaturesDisplay::paintEllipse(FaceData *faceData, cv::Mat &img)
{
	if (faceData->hasEllipse())
	{
		vector<FaceEllipse> ellipses = faceData->getEllipses();
		for (int i = 0; i < ellipses.size(); ++i)
		{
			FaceEllipse ellipse = ellipses[i];

			cv::Scalar col;
			switch (ellipse.annot_type_id)
			{
			case 0:
				col = cv::Scalar(0.0,0.0,255.0);
				break;
			case 1:
				col = cv::Scalar(0.0,255.0,0);
				break;
			case 2:
				col = cv::Scalar(255.0,0.0,0.0);
				break;
			case 3:
				col = cv::Scalar(0.0,255.0,255.0);
				break;
			}


			cv::Point2f cnt(ellipse.x,ellipse.y);
			cv::Size axes(ellipse.ra,ellipse.rb);
			double angle = -static_cast<double>(ellipse.theta)*180.0/M_PI;
			cv::ellipse(img,cnt,axes,angle,0.0,360.0,col,2);

			double ct = cos(ellipse.theta);
			double st = sin(ellipse.theta);

			// major axis
			double dx = ellipse.ra * ct;
			double dy = ellipse.ra * st;
			cv::Point2d x1(ellipse.x - dx,ellipse.y - dy);
			cv::Point2d x2(ellipse.x + dx,ellipse.y + dy);
			cv::line(img,x1,x2,col,2,CV_AA);

			// minor axis
			dx = - ellipse.rb * st;
			dy = ellipse.rb * ct;
			x1.x = ellipse.x - dx;
			x1.y = ellipse.y - dy;
			x2.x = ellipse.x + dx;
			x2.y = ellipse.y + dy;
			cv::line(img,x1,x2,col,2,CV_AA);
		}
	}
}

void FaceFeaturesDisplay::paintRectangles(FaceData *faceData, cv::Mat &img)
{
	vector<FaceRect> faceRects = faceData->getRects();
	for (int i = 0; i < faceRects.size(); ++i)
	{
		FaceRect fr = faceRects[i];
		cv::Scalar col = CV_RGB(128,128,255); // let it depend on annotType ?
		cv::rectangle(img,cv::Rect(fr.x,fr.y,fr.w,fr.h),col);
		stringstream rectTitleStrm;
		rectTitleStrm << fr.annotType;
		cv::putText(img,rectTitleStrm.str(),cv::Point(fr.x,fr.y),cv::FONT_HERSHEY_DUPLEX,1.0,col,2.0);
	}
}
