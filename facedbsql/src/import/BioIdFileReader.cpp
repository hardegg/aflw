/*
 * BioIdFileReader.h
 *
 *  Created on: 08.04.2012
 *      Author: szweimueller
 */

#include "BioIdFileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "boost/lexical_cast.hpp"

#include <cv.h>

#include "../util/StringUtil.h"

const int BioIdFileData::numFeaturesFileFeatureCodes = 20;

const string BioIdFileData::featuresFileFeatureCodes[BioIdFileData::numFeaturesFileFeatureCodes] =
	{ "LEC", "REC", "MLC", "MRC", "LBLC", "LBRC", "RBLC", "RBRC", "LT", "LELC", "LERC", "RELC", "RERC", "RT", "NC", "LNNT", "RNNT", "MCUL", "MCLL", "CC" };


BioIdFileReader::BioIdFileReader()
{
}

BioIdFileReader::~BioIdFileReader()
{
}

bool BioIdFileReader::loadImage(std::string filename)
{
	fstream fstr;
	fstr.open(filename.c_str(), ios::in);

    if (fstr.is_open()) {
        //get filenumber
        BioIdFileData bioIdData;
        string::size_type p = filename.find_last_of("_");
        std::string filenumber;
        if (p != string::npos) {
            filenumber = filename.substr(p+1,4);
        } else {
            filenumber = -1;
        }

        bioIdData.filenumber = filenumber;
        bioIdData.imgFilename = filename;
    
        m_bioIdDataMap.insert(std::pair<int, BioIdFileData> 
                    (boost::lexical_cast<int> (filenumber), bioIdData));

	    fstr.close();
    } else {
        std::cout << "BioIdFileReader::loadImage -> Warning: illegal Path!" << std::endl;
    }
	return true;
}

bool BioIdFileReader::loadEyes(std::string filename)
{
	fstream fstr;
	fstr.open(filename.c_str(), ios::in);
    if (fstr.is_open()) {
        //get filenumber  
        std::string filenumber;
        string::size_type p = filename.find_last_of("_");
        if (p != string::npos) {
            filenumber = filename.substr(p+1,4);
        } else {
            filenumber = -1;
        }
    
        //find enty with same key
        std::map<int, BioIdFileData>::iterator it;
        it = m_bioIdDataMap.find(boost::lexical_cast<int>(filenumber));
    
        BioIdFileData bioIdData;
        if (it != m_bioIdDataMap.end()) {
            bioIdData = it->second;
            m_bioIdDataMap.erase(it);
        } else {
            std::cout << "Warning: No image found for eye entries -> rejected File: " << filename << std::endl;
            return false;
        }

        bioIdData.eyeFilename = filename;

	    std::string line;
	    std::getline(fstr,line); // skip first line
    
	    while (!fstr.eof())
	    {
		    std::getline(fstr,line);
            p = line.find("\t");
        
            vector<int> eyeCoordsVector;

		    // parse the line
            while (p != -1)
            {
                std::string str;
                p = line.find("\t");

                if (p == string::npos) {
                    str = line;
                    p = -1;
                    eyeCoordsVector.push_back(boost::lexical_cast<int>(str));
                } else {
                    str = line.substr(0,p);
		            line = StringUtil::ltrim(line.substr(p+1));
                    eyeCoordsVector.push_back(boost::lexical_cast<int>(str));
                }
            }

            if ( (eyeCoordsVector.size() > 0) && (bioIdData.filenumber.size() > 0) ) {
                bioIdData.leftEye = std::pair<int,int> 
                    (eyeCoordsVector.at(0), eyeCoordsVector.at(1));
                bioIdData.rightEye = std::pair<int,int> 
                    (eyeCoordsVector.at(2), eyeCoordsVector.at(3));
            
                m_bioIdDataMap.insert(std::pair<int, BioIdFileData> 
                    (boost::lexical_cast<int> (filenumber), bioIdData));
	        }
        }
	    fstr.close();
    } else {
        std::cout << "BioIdFileReader::loadEye -> Warning: illegal Path!" << std::endl;
    }
    return true;
}

bool BioIdFileReader::loadPoints(std::string filename)
{
	fstream fstr;
	fstr.open(filename.c_str(), ios::in);
    
    if (fstr.is_open()) {
        //get filenumber
        string::size_type p = filename.find_last_of("_");
        string filenumber;
        if (p != string::npos) {
            filenumber = filename.substr(p+1,4);
        } else {
            filenumber = -1;
        }

        //find enty with same key
        std::map<int, BioIdFileData>::iterator it;
        it = m_bioIdDataMap.find(boost::lexical_cast<int>(filenumber));
    
        BioIdFileData bioIdData;
        if (it != m_bioIdDataMap.end()) {
            bioIdData = it->second;
            m_bioIdDataMap.erase(it);
        } else {
            std::cout << "Warning: No image found for point entries -> rejected File: " << filename << std::endl;
            return false;
        }

        bioIdData.pointsFilename = filename;

	    std::string line;
	    std::getline(fstr,line); // skip first line
        std::getline(fstr,line);
        p = line.find(" ");
        if (p != string::npos) {
            bioIdData.numPoints = boost::lexical_cast<int>(line.substr(p+1));
        }
	    std::getline(fstr,line); // skip { line

        int count = 0;
        while (count < bioIdData.numPoints)
	    {
		    getline(fstr,line);
            p = line.find(" ");

            double xCoord = boost::lexical_cast<double> (line.substr(0, p));
            double yCoord = boost::lexical_cast<double> (line.substr(p + 1));
 
            if ( bioIdData.filenumber.size() > 0 ) {
                bioIdData.points.push_back(std::pair<double, double> (xCoord, yCoord));  
	        }
            ++count;
        }
    
        m_bioIdDataMap.insert(std::pair<int, BioIdFileData> 
                    (boost::lexical_cast<int> (filenumber), bioIdData));

	    fstr.close();
	} else {
        std::cout << "BioIdFileReader::loadPoints -> Warning: illegal Path!" << std::endl;
    }
    return true;
}

bool BioIdFileReader::printBioIdData()
{
    if (m_bioIdDataMap.size() != 0) {
        std::cout << "===============================" << std::endl;   
        std::cout << "|                             |" << std::endl;
        std::cout << "|         Bio Id Data         |" << std::endl;
        std::cout << "|                             |" << std::endl;
        std::cout << "===============================" << std::endl;
        for (std::map<int, BioIdFileData>::iterator it = m_bioIdDataMap.begin(); it != m_bioIdDataMap.end(); ++it)
        {
            std::cout << "| Number:     " << it->second.filenumber << std::endl;  
            std::cout << "| ImageFile:  " << it->second.imgFilename  << std::endl;
            std::cout << "| EyeFile:    " << it->second.eyeFilename  << std::endl;
            std::cout << "| LeftEye:    " << it->second.leftEye.first << " : " 
                << it->second.leftEye.second << std::endl;
            std::cout << "| RightEye:   " << it->second.rightEye.first << " : " 
                << it->second.rightEye.second  << std::endl;
            std::cout << "| PointsFile: " << it->second.pointsFilename  << std::endl;
            std::cout << "| numPoints:  " << it->second.numPoints  << std::endl;
            if (it->second.points.size() != 0) {
                for (int i = 0; i < it->second.numPoints; i++)
                {
                   std::cout << "| " << i+1 << ". " << BioIdFileData::featuresFileFeatureCodes[i]  << ": "
                        << " : " << it->second.points.at(i).second  << std::endl;
                }
            }
            std::cout << "===============================" << std::endl;
        }
        std::cout << "===============================" << std::endl;
    }
    return true;
}

bool BioIdFileReader::drawLandmarks()
{
    if (m_bioIdDataMap.size() != 0) {
        for (std::map<int, BioIdFileData>::iterator it = m_bioIdDataMap.begin(); it != m_bioIdDataMap.end(); ++it)
        {
            // load image
			std::string path = it->second.imgFilename;
			cv::Mat img = cv::imread(path, -1);
			if (!img.data) {
				std::wcerr << "Could not load image: " << path.c_str() << " failed." << std::endl;
			} else { 

				if (it->second.points.size() != 0) {
					for (int i = 0; i < it->second.numPoints; i++)
					{
						// draw marker at landmark coord
						int x = static_cast<int> (it->second.points.at(i).first);
						int y = static_cast<int> (it->second.points.at(i).second);

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
				std::string fileSaveName = "D:/Uni/Masterprojekt/aflw/facedbsql/build/Debug/mt/bioid/image";

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
