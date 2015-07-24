/*
 * PutFileReader.h
 *
 *  Created on: 01.05.2012
 *      Author: szweimueller
 */

#include "PutFileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "boost/lexical_cast.hpp"

#include <cv.h>

#include "../util/StringUtil.h"

const int PutFileData::numFeaturesFileFeatureCodes = 30;

const string PutFileData::featuresFileFeatureCodes[PutFileData::numFeaturesFileFeatureCodes] =
	// { "REC", "LEC", "NC", "CC", "RERC", "RELC", "RET", "REB", "LELC", "LERC", "LET", "LEB", "RBRC", "RBLC", "LBLC", "LBRC", "RNNT", "RN", "LNNT", "LN", "MRC", "MLC", "MCUL", "MCULI", "MCLL", "MCLLI", "RE", "LE", "FOL", "FOR" };
	{ "LEC", "REC", "NC", "CC", "LELC", "LERC", "LET", "LEB", "RERC", "RELC", "RET", "REB", "LBLC", "LBRC", "RBRC", "RBLC", "LNNT", "LN", "RNNT", "RN", "MLC", "MRC", "MCUL", "MCULI", "MCLL", "MCLLI", "LE", "RE", "FOR", "FOL" };

const string PutFileData::readFeaturesFileFeatureCodes[PutFileData::numFeaturesFileFeatureCodes] =
	{ "RE-C", "LE-C", "N-C", "FO-B", "REL-O", "REL-I", "REL-T", "REL-B", "LEL-O", "LEL-I", "LEL-T", "LEL-B", "REB-O", "REB-I", "LEB-O", "LEB-I", "RN-C", "RN-O", "LN-C", "LN-O", "M-OR", "M-OL", "M-OT", "M-IT", "M-OB", "M-IB", "REA-B", "LEA-B", "FO-L", "FO-R" };


const int PutFileData::numRegionsFileFeatureCodes = 5;

const string PutFileData::regionsFileFeatureCodes[PutFileData::numRegionsFileFeatureCodes] =
	{ "Face", "Left eye", "Right eye", "Lips", "Nose" };

const string PutFileData::readRegionsFileFeatureCodes[PutFileData::numRegionsFileFeatureCodes] =
	{ "Face", "Left eye", "Right eye", "Lips", "Nose" };


PutFileReader::PutFileReader()
{
}

PutFileReader::~PutFileReader()
{
}

bool PutFileReader::loadImage(std::string filename)
{
	fstream fstr;
	fstr.open(filename.c_str(), ios::in);

    if (fstr.is_open()) {
        //get filenumber
        PutFileData putData;
        string::size_type p = filename.find_last_of("\\");
        std::string filenumber;
        if (p != string::npos) {
            filenumber = filename.substr(p+1,8);
        } else {
            filenumber = -1;
        }

        putData.filenumber = filenumber;
        putData.imgFilename = filename;
    
        m_putDataMap.insert(std::pair<int, PutFileData> 
                    (boost::lexical_cast<int> (filenumber), putData));

	    fstr.close();
    } else {
        std::cout << "PutFileReader::loadImage -> Warning: illegal Path!" << std::endl;
    }
	return true;
}

bool PutFileReader::loadLandmarks(std::string filename)
{
    // std::cout << "loadLandmarks (" << filename << ")" << std::endl;

    //get filenumber
    std::string filenumber;
    string::size_type p = filename.find_last_of("\\");
    if (p != string::npos) {
        filenumber = filename.substr(p+1,8);
    } else {
        filenumber = -1;
    }
    
    //find entry with same key
    std::map<int, PutFileData>::iterator it;
    it = m_putDataMap.find(boost::lexical_cast<int>(filenumber));
    
    PutFileData putData;
    if (it != m_putDataMap.end()) {
        putData = it->second;
        m_putDataMap.erase(it);
    } else {
        std::cout << "Warning: No image for landmark entry found -> rejected File: " << filenumber << std::endl;
        return false;
    }

    cv::FileStorage fileStorage(filename, cv::FileStorage::READ);
    if (fileStorage.isOpened() == false) {
        std::cout << "PutFileReader::loadLandmarks -> illegal Filepath!" << std::endl;
        return false;
    }

    putData.numLandmarks = PutFileData::numFeaturesFileFeatureCodes;
    putData.landmarksFilename = filename;

    for (int i = 0;  i < putData.numLandmarks; i++)
    {        
        cv::FileNode landmarkFileNode = fileStorage[PutFileData::readFeaturesFileFeatureCodes[i]];
 
        double xCoord = static_cast<double> (landmarkFileNode["x"]);
        double yCoord = static_cast<double> (landmarkFileNode["y"]);

        putData.landmarks.push_back(std::pair<double, double> (xCoord, yCoord));
    }

    m_putDataMap.insert(std::pair<int, PutFileData> 
                (boost::lexical_cast<int> (filenumber), putData));

    fileStorage.release();

	return true;
}

bool PutFileReader::loadRegions(std::string filename)
{
 //   std::cout << "loadRegions (" << filename << ")" << std::endl;

	////get filenumber  
 //   std::string filenumber;
 //   string::size_type p = filename.find_last_of("\\");
 //   if (p != string::npos) {
 //       filenumber = filename.substr(p+1,8);
 //   } else {
 //       filenumber = -1;
 //   }

 //   //find enty with same key
 //   std::map<int, PutFileData>::iterator it;
 //   it = m_putDataMap.find(boost::lexical_cast<int>(filenumber));
 //   
 //   PutFileData putData;
 //   if (it != m_putDataMap.end()) {
 //       putData = it->second;
 //       m_putDataMap.erase(it);
 //   } else {
 //       std::cout << "Warning: No image for regions entry found -> rejected File: " << filenumber << std::endl;
 //       return false;
 //   }
 //     
 //   cv::FileStorage fileStorage(filename, cv::FileStorage::READ);
 //   /*if (fileStorage.isOpened() == false) {
 //       return false;
 //   }*/

 //   putData.numRegions = PutFileData::numRegionsFileFeatureCodes;
 //   putData.regionsFilename = filename;
 //    
 //   for (int i = 0;  i < putData.numRegions; i++)
 //   {
 //       cv::FileNode regionFileNode = fileStorage[PutFileData::readRegionsFileFeatureCodes[i]];

 //       int x = static_cast<int> (regionFileNode["x"]);
 //       int y = static_cast<int> (regionFileNode["y"]);
 //       int width = static_cast<int> (regionFileNode["width"]);
 //       int height = static_cast<int> (regionFileNode["height"]);

 //       putData.regions.push_back(cv::Rect(x, y, width, height));
 //   }

 //   m_putDataMap.insert(std::pair<int, PutFileData> 
 //               (boost::lexical_cast<int> (filenumber), putData));

 //   fileStorage.release();

	return true;
}

bool PutFileReader::printPutData()
{
    if (m_putDataMap.size() != 0) {
        std::cout << "===============================" << std::endl;   
        std::cout << "|                             |" << std::endl;
        std::cout << "|          PUT Data           |" << std::endl;
        std::cout << "|                             |" << std::endl;
        std::cout << "===============================" << std::endl;
        for (std::map<int, PutFileData>::iterator it = m_putDataMap.begin(); it != m_putDataMap.end(); ++it)
        {
            std::cout << "| Number:       " << it->second.filenumber << std::endl;  
            std::cout << "| ImageFile:    " << it->second.imgFilename  << std::endl;
            std::cout << "| LandmarksFile:" << it->second.landmarksFilename  << std::endl;
            std::cout << "| RegionsFile:  " << it->second.regionsFilename  << std::endl;
            std::cout << "| numLandmars:  " << it->second.numLandmarks  << std::endl;
            if (it->second.landmarks.size() != 0) {
                for (int i = 0; i < it->second.numLandmarks; i++)
                {
                    std::cout << "| " << i+1 << ". " << PutFileData::readFeaturesFileFeatureCodes[i] << ": "
                        << it->second.landmarks.at(i).first << " : " 
                        << it->second.landmarks.at(i).second  << std::endl;
                }
            }
            // std::cout << "| numRegions:  " << it->second.numRegions  << std::endl;
            if (it->second.regions.size() != 0) {
                for (int i = 0; i < it->second.numRegions; i++)
                {
                    std::cout << "| " << i+1 << ". " << PutFileData::readRegionsFileFeatureCodes[i]  << ": "
                        << it->second.regions.at(i).x << " : " 
                        << it->second.regions.at(i).y  << " - " 
                        << it->second.regions.at(i).width  << " : " 
                        << it->second.regions.at(i).height << std::endl;
                }
            }
            std::cout << "===============================" << std::endl;
        }
        std::cout << "===============================" << std::endl;
    }
    return true;
}

bool PutFileReader::drawLandmarks()
{
    if (m_putDataMap.size() != 0) {
        for (std::map<int, PutFileData>::iterator it = m_putDataMap.begin(); it != m_putDataMap.end(); ++it)
        {
            // load image
			std::string path = it->second.imgFilename;
			cv::Mat img = cv::imread(path, -1);
			if (!img.data) {
				std::wcerr << "Could not load image: " << path.c_str() << " failed." << std::endl;
			} else { 
			
				if (it->second.landmarks.size() != 0) {
					for (int i = 0; i < it->second.numLandmarks; i++)
					{
						// draw marker at landmark coord
						int x = static_cast<int> (it->second.landmarks.at(i).first);
						int y = static_cast<int> (it->second.landmarks.at(i).second);

						const char* marker = "+";
						cv::putText(img, marker, cvPoint(x, y),  cv::FONT_HERSHEY_PLAIN, 0.75f, cvScalar(255, 0, 0, 0));

						// insert featurecoordstypes at landmark coord
						std::string featureCodes = " " + it->second.featuresFileFeatureCodes[i];
						cv::putText(img, featureCodes.c_str(), cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, 0.75f, cvScalar(255, 0, 0, 0));
					}
				}
             
				imshow("TET", img);
				waitKey();

				// save image to build/debug/save! folder must exist!!
				std::string fileSaveName = "../sample_scripts/mt/put/image";

				fileSaveName = fileSaveName + it->second.filenumber + ".jpg";
				if (!imwrite(fileSaveName, img)) {
					std::cerr << "Could not write image: " << fileSaveName << std::endl;
					return false;
				}
			}
        }
    }
    return true;
}