/*
 * FaceTracerFileReader.h
 *
 *  Created on: 01.07.2012
 *      Author: szweimueller
 */

#include "FaceTracerFileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <sstream>

#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

#include <cv.h>

#include "../util/StringUtil.h"
#include "../util/utils.h"
#include "../facedata/FaceMetadata.h"

#define RAD 3.14159265 / 180

const int FaceTracerFileData::numFeaturesFileFeatureCodes = 6;

const string FaceTracerFileData::featuresFileFeatureCodes[FaceTracerFileData::numFeaturesFileFeatureCodes] =
	{ "LELC", "LERC", "RELC", "RERC", "MLC", "MRC" };

const int FaceTracerFileData::numFieldCodes = 20;

const string FaceTracerFileData::fieldCodes[FaceTracerFileData::numFieldCodes] =
	{ "face_id", "crop_width", "crop_height", "crop_x0", "crop_y0", "yaw", "pitch", "roll", "left_eye_x0", "left_eye_y0", "left_eye_x1", "left_eye_y1", "right_eye_x0", "right_eye_y0", "right_eye_x1", "right_eye_y1", "mouth_x0", "mouth_y0", "mouth_x1", "mouth_y1" };

const int FaceTracerFileData::numAttributeFileFeatureCodes = 10;

const string FaceTracerFileData::attributeFileFeatureCodes[FaceTracerFileData::numAttributeFileFeatureCodes] =
	{ "gender", "race", "age", "hair_color", "eye_wear", "mustache", "expression", "blurry", "lightning", "enviroment" };

	
FaceTracerFileReader::FaceTracerFileReader()
{
}

FaceTracerFileReader::~FaceTracerFileReader()
{
}

vector<std::string> FaceTracerFileReader::getLineById(const int faceId, std::string filename) 
{
    // std::cout << "FaceTracerFileReader::getLineById('" << faceId << "', '" << filename << "') " << std::endl;    

    fstream fstr;
	fstr.open(filename.c_str(), ios::in);

    vector<std::string> stats;

    if (fstr.is_open()) {
	    std::string line;
        std::string stat;
	    getline(fstr,line); // skip first line
        
        while (!fstr.eof())
        {
            getline(fstr,line);

            string::size_type p;
            p = line.find("\t");
		    if (p == string::npos) {
			    p = line.size() - 1;
            }
            
            stat = line.substr(0,p);
			line = StringUtil::ltrim(line.substr(p+1));

			
			try
			{
				if (utils::convertToInt(stat) == faceId) {  
					// parse the line
					while (line.size() > 0)
					{
						p = line.find("\t");
						if (p == string::npos) {
							p = line.size();
						}

						stat = line.substr(0,p);
						line = StringUtil::ltrim(line.substr(p));

						stats.push_back(stat);
					}
				}
			}
			catch(...)
			{

			}
        }
	    fstr.close();
    }
    return stats;
}

bool FaceTracerFileReader::loadFaceindex(const int faceId, std::string filename, std::string foldername)
{
    std::cout << "loadFaceindex ('" << faceId << ", " << foldername << ", " << filename << "')" << std::endl;

    FaceTracerFileData faceTracerData;
    faceTracerData.faceId = boost::lexical_cast<std::string> (faceId);

    // returns stats from line by faceId
    vector<std::string> stats = getLineById(faceId, filename);
    if (stats.size() == 0) {
       std::cerr << "Error: No face id: " << faceId << " in file: " << filename << " found! " <<  std::endl;
       return false;
    }

    boost::format fmt("%06d");
    fmt % faceId;
    std::string faceIdString = boost::lexical_cast<std::string>(fmt);

    faceTracerData.faceindexFilename = filename;
    faceTracerData.faceIdFilename = faceIdString + ".jpg";
    faceTracerData.imageFoldername = foldername;
    // faceTracerData.faceIdFilename = stats.at(0); // imgurl;
    // faceTracerData.faceIdFilename = stats.at(1); // pageurl;

    m_faceTracerDataMap.insert(std::pair<int, FaceTracerFileData> 
                (boost::lexical_cast<int> (faceId), faceTracerData));

	return true;
}

bool FaceTracerFileReader::loadFacestats(const int faceId, std::string filename)
{
    std::cout << "loadFacestats ('" << faceId << ", " << filename << "')" << std::endl;
    
    //find entry with same key
    std::map<int, FaceTracerFileData>::iterator it;
    it = m_faceTracerDataMap.find(boost::lexical_cast<int>(faceId));
    
    FaceTracerFileData faceTracerData;
    if (it != m_faceTracerDataMap.end()) {
        faceTracerData = it->second;
        m_faceTracerDataMap.erase(it);
    } else {
        std::wcerr << "Warning: No image for facestats entry found -> rejected File: " << faceId << std::endl;
        return false;
    }

    faceTracerData.numFacestats = FaceTracerFileData::numFeaturesFileFeatureCodes;
    faceTracerData.facestatsFilename = filename;

    // returns stats from line by faceId
    vector<std::string> stats = getLineById(faceId, filename);
    if (stats.size() == 0) {
       std::cerr << "Error: No face id: " << faceId << " in file: " << filename << " found! " <<  std::endl;
       return false;
    }

    if ((stats.size() + 1) < faceTracerData.numFieldCodes) {
        return false;
    }

    faceTracerData.crop = cv::Rect(utils::convertToInt(stats.at(2)), utils::convertToInt(stats.at(3)), utils::convertToInt(stats.at(0)), utils::convertToInt(stats.at(1)));
    faceTracerData.yaw = utils::convertToFloat(stats.at(4)) * RAD;
	faceTracerData.pitch = utils::convertToFloat(stats.at(5)) * RAD;
    faceTracerData.roll = utils::convertToFloat(stats.at(6)) * RAD;

    for (int i = 0;  i < faceTracerData.numFacestats; i++)
    {      
        double xCoord = utils::convertToDouble(stats.at(7 + (2 * i))) + faceTracerData.crop.x;
        double yCoord = utils::convertToDouble(stats.at(7 + (2 * i) + 1)) + faceTracerData.crop.y;

        faceTracerData.facestats.push_back(std::pair<double, double> (xCoord, yCoord));
    }
   
    m_faceTracerDataMap.insert(std::pair<int, FaceTracerFileData> 
                (boost::lexical_cast<int> (faceId), faceTracerData));

	return true;
}

bool FaceTracerFileReader::loadFacelabel(const int faceId, std::string filename)
{    
    std::cout << "loadFacelabel ('" << faceId << ", " << filename << "')" << std::endl;

    //find entry with same key
    std::map<int, FaceTracerFileData>::iterator it;
    it = m_faceTracerDataMap.find(boost::lexical_cast<int>(faceId));
    
    FaceTracerFileData faceTracerData;
    if (it != m_faceTracerDataMap.end()) {
        faceTracerData = it->second;
        m_faceTracerDataMap.erase(it);
    } else {
        std::wcerr << "Warning: No image for facelabels entry found -> rejected File: " << faceId << std::endl;
        return false;
    }

    faceTracerData.numFacelabels = FaceTracerFileData::numAttributeFileFeatureCodes;
    faceTracerData.facelabelFilename = filename;

	faceTracerData.sex = FaceMetadata::UNDEFINED;
	faceTracerData.glasses = FaceMetadata::UNDEFINED;
	faceTracerData.occluded = FaceMetadata::UNDEFINED;
	faceTracerData.bw = FaceMetadata::UNDEFINED;

    // returns stats from line by faceId
    vector<std::string> stats = getLineById(faceId, filename);
    while (stats.size() > 1)
    {
        std::string label = stats.back();
        stats.pop_back();
        std::string attribute = stats.back();
        stats.pop_back();

		if (attribute.compare("gender") == 0) {
			faceTracerData.sex = label.compare("male") ? FaceMetadata::FEMALE : FaceMetadata::MALE;
			std::cout << "eingetragen-> " << faceTracerData.sex << std::endl;
		} else if (attribute.compare("eye_wear") == 0) {
			faceTracerData.glasses = label.compare("none") ? FaceMetadata::CHECKED : FaceMetadata::UNCHECKED;
			std::cout << "eingetragen-> " << faceTracerData.glasses << std::endl;
		} else {

		}
    }

    m_faceTracerDataMap.insert(std::pair<int, FaceTracerFileData> 
                (boost::lexical_cast<int> (faceId), faceTracerData));

	return true;
}

bool FaceTracerFileReader::printFaceTracerData()
{
    if (m_faceTracerDataMap.size() != 0) {
        std::cout << "===============================" << std::endl;   
        std::cout << "|                             |" << std::endl;
        std::cout << "|     FACE TRACER Data        |" << std::endl;
        std::cout << "|                             |" << std::endl;
        std::cout << "===============================" << std::endl;
        for (std::map<int, FaceTracerFileData>::iterator it = m_faceTracerDataMap.begin(); it != m_faceTracerDataMap.end(); ++it)
        {
            std::cout << "| id:           " << it->second.faceId << std::endl;  
            std::cout << "| Image id:     " << it->second.faceIdFilename << std::endl;
			std::cout << "| FaceindexFile:" << it->second.faceindexFilename << std::endl;
            std::cout << "| FacestatsFile:" << it->second.facestatsFilename << std::endl;
            std::cout << "| FacelabelFile:" << it->second.facelabelFilename << std::endl;
            std::cout << "| Facestats:  " << it->second.numFacestats << std::endl;
            if (it->second.facestats.size() != 0) {
                for (int i = 0; i < it->second.numFacestats; i++)
                {
                    std::cout << "| " << i+1 << ". " << FaceTracerFileData::fieldCodes[i] << ": "
                        << it->second.facestats.at(i).first << " : " 
                        << it->second.facestats.at(i).second << std::endl;
                }
            }
            std::cout << "| Facelabels:  " << it->second.numFacelabels  << std::endl;
            if (it->second.facelabels.size() != 0) {
                for (int i = 0; i < it->second.numFacelabels; i++)
                {
                    std::cout << "| " << i+1 << ". " << FaceTracerFileData::attributeFileFeatureCodes[i] << ": "
                        << it->second.facelabels.at(i).first << " " << it->second.facelabels.at(i).second << std::endl;
                }
            }
            std::cout << "===============================" << std::endl;
        }
        std::cout << "===============================" << std::endl;
    }
    return true;
}

//cv::Mat FaceTracerFileReader::getImageFromUrl(const char* url)
//{
//    std::cout << "getImageFromUrl ('" << url << "')" << std::endl;
//
//    cv::Mat img;
//
//    CvCapture* camera = cvCaptureFromFile(url);
//    if (!camera) {
//        std::cerr << "cvCaptureFromFile failed." << std::endl;
//    } else {
//        img = cvQueryFrame(camera);
//
//        //double cam_w = cvGetCaptureProperty(camera, CV_CAP_PROP_FRAME_WIDTH);
//        //double cam_h = cvGetCaptureProperty(camera, CV_CAP_PROP_FRAME_HEIGHT);
//    }
//    cvReleaseCapture(&camera);
//    return img;
//}

bool FaceTracerFileReader::drawFacestats(const int faceId)
{
    if (m_faceTracerDataMap.size() != 0) {
        // find entry with same key
        std::map<int, FaceTracerFileData>::iterator it;
        it = m_faceTracerDataMap.find(boost::lexical_cast<int>(faceId));

        //load image 
        std::string path = it->second.imageFoldername + '/' + it->second.faceIdFilename;
        cv::Mat img = cv::imread(path, -1);
        if (!img.data) {
            std::wcerr << "Could not load image: " << path.c_str() << " failed." << std::endl;
        } else { 

            for (unsigned int i = 0; i < it->second.facestats.size(); i++)
            {
                // draw marker at landmark coord
                CvFont font;
                cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);

                int x = static_cast<int> (it->second.facestats.at(i).first);
                int y = static_cast<int> (it->second.facestats.at(i).second);

                const char* marker = "+";
                // cv::putText(img, marker, cvPoint(x, y), &font, cvScalar(255, 0, 0, 0));
                cv::putText(img, marker, cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, 0.75f, cv::Scalar(255, 0, 0));

                // insert featurecoordstypes at landmark coord
                std::string featureCodes = " " + it->second.featuresFileFeatureCodes[i];
                // cv::putText(img, featureCodes.c_str(), cvPoint(x, y), &font, cvScalar(255, 0, 0, 0));
                cv::putText(img, featureCodes.c_str(), cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, 0.75f, cv::Scalar(255, 0, 0));
            }

            imshow("TET", img);
            waitKey();
            
            // save image to: sample_scripts/mt/facetracer/  !!folder must exist!!
            std::string fileSaveName = "../sample_scripts/mt/facetracer/image";
            fileSaveName.append(it->second.faceIdFilename);

            if (!imwrite(fileSaveName, img)) {
                std::cerr << "Could not write image: " << fileSaveName << std::endl;
                return false;
            } 
        }
    }
    return true;
}
