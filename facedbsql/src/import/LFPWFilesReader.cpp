/*
 * LFPWFilesReader.cpp
 *
 *  Created on: 08.10.2012
 *      Author: Elena Wirtl
 */

#include "LFPWFilesReader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "cv.h"
#include "highgui.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

	const int LFPWFilesReader::fileNameLength = 6;
	
	const int LFPWFilesReader::numFeaturesFileFeatureCodes = 21;

	const string LFPWFilesReader::extension = ".jpg";
	
	const string LFPWFilesReader::valitatorFileName = "LFPW_invalid.txt";

LFPWFilesReader::LFPWFilesReader() {

}

LFPWFilesReader::~LFPWFilesReader() {
}

void LFPWFilesReader::load(std::string lfpwFilesDir,int annotTypeID_LFPW)
{
	annotTypeID = annotTypeID_LFPW;
	
	readValidFile(lfpwFilesDir);
	
	//Name of the files where the feature points are specified
	boost::filesystem::path p_test(lfpwFilesDir);
	boost::filesystem::path test_file(lfpwFilesDir);
    p_test /= "test";
    test_file /= "kbvt_lfpw_v1_test.csv";
    readFile(test_file.string(), p_test.string(), true);
    
    boost::filesystem::path p_train(lfpwFilesDir);
    boost::filesystem::path train_file(lfpwFilesDir);
    p_train /= "train";
    train_file /= "kbvt_lfpw_v1_train.csv";
    readFile(train_file.string(), p_train.string(), false);
}

void LFPWFilesReader::readValidFile(std::string dir)
{
   std::ifstream input;
   std::string line;
   
   boost::filesystem::path p(dir);
   p /= valitatorFileName;
   std::string file = p.string();
   std::vector<std::string> boostVec;
	
   input.open(file.c_str());

   for (int i = 0; std::getline(input,line); i++)
   {
      boost::split( boostVec, line, boost::is_any_of(",") );
      if(i == 1)  //test
      {  
          _invalidTest = boostVec; 
      }
      else if(i == 3)  //train
      {
         _invalidTrain = boostVec;
      } 
      
   }

}

void LFPWFilesReader::readFile(std::string filename, std::string dir, bool test)
{
	std::ifstream input;
	std::string line;
	
	input.open(filename.c_str());
	int name = 0;
	int num_zeros;
	
	if(!input.is_open())
	    std::cout << " Could not open file: " << filename << std::endl;
	

	while (std::getline(input,line))
	{
		std::vector<std::string> tokens;
        boost::split( tokens, line, boost::is_any_of("\t") );
              
        //There up to 4 lines per image
        // average and worker_[0-2]
        // we take the average ones
        // first entry is the link to the picture
        // second entry is the average/worker_[0_2] thing
		if (tokens[1].compare("average") == 0)
		{
		       
		    //Get the file name
		    // the names are numbers
		    // counted up from 1
			FeaturesCoords fc(0);
			std::stringstream str_name;
			name++;
			str_name << name;
			
			
			bool filevalid = validImg(str_name.str(),test);
			
			
			//to get the right amount of zeroes
			// there should be a better way ...
			num_zeros = fileNameLength - str_name.str().length();
			//char zeros[num_zeros+1];	
			//int zero = 0;
			//for(int i = 0; i < num_zeros; i++)
			//   zeros[i] = '0';
			//zeros[num_zeros]= '\0';
			
			std::string numbers = std::string( num_zeros, '0');
			numbers.append( str_name.str()); 
			
            
            //the filenames for the test files have
            // the word 'test' befor the numbers
            // and the others 'train'
			std::stringstream filenameS;
			std::string filepath;
			if(test)
			{
			   filenameS << "test" << numbers << extension;
			   filepath = "test";
			}
			else
			{
			   filenameS << "train" << numbers << extension;
			   filepath = "train";
			}
		
			std::string filename = filenameS.str();
		

            boost::filesystem::path p(dir);
            p /= filename;
            std::string file = p.string();
            
			std::ifstream file_exists(file.c_str());
			
			          
            //Add the file only if it exists
            if (file_exists && filevalid) 
            {
            
               cv::Mat img = cv::imread(file,-1);
			   bool outOfRange = false;
			   bool visibility = true;
			   
			   //save for point which should be interpolated
			   cv::Point2f pt_save;
			   pt_save.x = 0;
 			   pt_save.y = 0;
			   
			
			   //Now get all of the feature points
			   // first entry is the link to the picture
               // second entry is the average/worker_[0_2] thing 
               // after that the features starts, first ist the
               // x position then the y and after that the
               // t which is the visibility of the feature
               // there are 35 feature points in the lfpw database    
			   for (int i = 2; i < tokens.size(); i=i+3)
			   {
			      // (i+1)/3 is the number for the feature
			      //Coordinates for the feature points
                  cv::Point2f pt;
				  std::stringstream xStr;
				  xStr << tokens[i];
				  xStr >> pt.x;
				  std::stringstream yStr;
				  yStr << tokens[i+1];
				  yStr >> pt.y;
				  
				  // visibility
				  int t;
				  std::stringstream tStr;
				  tStr << tokens[i+2];
				  tStr >> t;
				
				  
			   	  int featureID = 0;
			   	  // t==0 means feature is visible
			   	  // else it is occlured because 
			   	  // of glasses or hairs or it 
			   	  // is not visible because of the
			   	  // view point
			   	  //0: visible
			   	  //1: obscured by hair/glasses/etc.
			   	  //2: hidden because of viewing angle
			   	  //3: hidden because of image crop
			   	  
			   	  if(t == 0)
			   	  {
			   	    // ((i+1)/3) represents the lfpw feature number
				    featureID = getAFLWFeatureNumber((i+1)/3);
				    
				    //zero means it is a feature point to be differantiated
				    if(featureID == 0)
				       pt_save = pt;
				    
				    //if is one off the center eyebrow points
				    if((featureID == 2 || featureID == 5) && (pt_save.x != 0) && (pt_save.y != 0))
				    {
				        pt.x = (pt.x + pt_save.x) / 2;
 					    pt.y = (pt.y + pt_save.y) / 2;
 		
 				        pt_save.x = 0;
 					    pt_save.y = 0;
				    }
				    else if((featureID == 2 || featureID == 5))
				    {
				       featureID = 0;
				    }
			 	  }
			 	  else if (t==1)
			 	  {
			 	     //means occlured
			 	     visibility = false;
			 	  }
			 	  //save the meta data!

	            // Only if featureID > 0 then there is really a feature
				if (featureID > 0)
				  fc.setCoords(featureID,pt,annotTypeID);
				
				// Check if the feature coords are in the range of the image
				// if they are out of bounds, then they aren't inserted into the database
				if( (pt.x > img.cols) || (pt.y > img.rows) || ((t==0) && ((pt.x < 0) || (pt.y < 0))))
				    outOfRange = true;
			}
			
			if(!outOfRange)
			{
			  _filenames.push_back(filename);
			  _filepaths.push_back(filepath);
			  _totalfilenames.push_back(file);
			  _visibility.push_back(visibility);
			
			  // line processed, filename available, feature coords are ready -> store
			  _featureCoords.push_back(fc);
			}
			
			outOfRange = false;
			
		  }
		}

	}
	
}

//-1 means error or nothing found
// 0 means difference is to be calculated
// >0 ok
int LFPWFilesReader::getAFLWFeatureNumber(int lfpwFeatureNumber)
{
             int featureID = -1;
             switch(lfpwFeatureNumber)
		     {
 				  case 1:
 				  {
 				    // aflw	lfpw
			        // 1 lblc	1
			        featureID = 1;
          		    break;
          		  }
          		  case 2:
 				  {
 					// aflw	lfpw
 					// 6 RBRC	2
 					featureID = 6;
          		    break;
          		  }
          		  case 3:
 				  {
 					// aflw	lfpw
 					// 3 LBRC	3
			        featureID = 3;
          		    break;
          		  }
          		  case 4:
 				  {
 					// aflw	lfpw
 					// 4 RBLC	4
 					featureID = 4;
          		    break;
          		  }
          		  case 5:
 				  {
 					// aflw	lfpw
 					// 2 lbc	5,6
 					featureID = 0;
          		    break;
          		  }
          		  case 6:
 				  {
 					// aflw	lfpw
 					// 2 lbc	5,6
 					featureID = 2;
          		    break;
          		  }
          		  case 7:
 				  {
 					// aflw	lfpw
 					// 5 RBC	7,8
 					featureID = 0;
          		    break;
          		  }
          		  case 8:
 				  {
 					// aflw	lfpw
 					// 5 RBC	7,8
 					featureID = 5;
          		    break;
          		  }
          		  case 9:
 				  {
 					// aflw	lfpw
 					// 7 LELC	9
 					featureID = 7;
          		    break;
          		  }
          		  case 10:
 				  {
 					// aflw	lfpw
 					//12 RERC	10
 					featureID = 12;
          		    break;
          		  }
          		  case 11:
 				  {
 					// aflw	lfpw
 					// 9 LERC	11
 					featureID = 9;
          		    break;
          		  }
          		  case 12:
 				  {
 					// aflw	lfpw
 					//10 RELC	12
 					featureID = 10;
          		    break;
          		  }
          		  case 13:
          		  {
          		    // aflw lfpw
          		    // 31 new 13
          		    // LECT
          		    // left_eye_center_top
          		    featureID = 31;
          		  }
          		  case 14:
          		  {
          		    // aflw lfpw
          		    // 32 new 14
          		    // LECB
          		    // left_eye_center_bottom
          		    featureID = 32;
          		  }
          		  case 15:
          		  {
          		    // aflw lfpw
          		    // 29 new 15
          		    // RECT
          		    // right_eye_center_top
          		    featureID = 29;
          		  }
          		  case 16:
          		  {
          		    // aflw lfpw
          		    // 30 new 16
          		    // RECB
          		    // right_eye_center_bottom
          		    featureID = 30;
          		  }
          		  case 17:
 				  {
 					// aflw	lfpw
 					// 8 LEC	17
 					featureID = 8;
          		    break;
          		  }
          		  case 18:
 				  {
 					// aflw	lfpw
 					//11 REC	18
 					featureID = 11;
          		    break;
          		  }
          		  case 19:
 				  {
 					// aflw	lfpw
 					//14 LN	19
 					featureID = 14;
          		    break;
          		  }
          		  case 20:
 				  {
 					// aflw	lfpw
 					//16 RN	20
 					featureID = 16;
          		    break;
          		  }
          		  case 21:
 				  {
 					// aflw	lfpw
 					//15 NC	21
 					featureID = 15;
          		    break;
          		  }
          		  case 22:
 				  {
 					// aflw	lfpw
 					// 37 new NCL	21
 					// nose_center_bottom
 					featureID = 37;
          		    break;
          		  }
          		  case 23:
 				  {
 					// aflw	lfpw
 					//18 MLC	23
 					featureID = 18;
          		    break;
          		  }
          		  case 24:
 				  {
 					// aflw	lfpw
 					//20 MRC	24
 					featureID = 20;
          		    break;
          		  }
          		  case 25:
 				  {
 					// aflw	lfpw
 					// 26 MC	25
 					// mouth_center_upper_lip_outer_edge
 					featureID = 26;
          		    break;
          		  }
          		  case 26:
 				  {
 					// aflw	lfpw
 					// 33 MC	26
 					// mouth_center_upper_lip_inner_edge
 					featureID = 33;
          		    break;
          		  }
          		  case 27:
 				  {
 					// aflw	lfpw
 					// 34 new MC 27
 					// mouth_center_bottom_lip_top
 					featureID = 34;
          		    break; 
          		  }
          		  case 28:
 				  {
 					// aflw	lfpw
 					// 27 new MC 28
 					// mouth_center_bottom_lip_bottom
 					featureID = 27;
          		    break; 
          		  }
          		  case 29:
 				  {
 					// aflw	lfpw
 					// 38 new LET	29
 					// left_ear_top
 					featureID = 38;
          		    break;
          		  }
          		  case 30:
 				  {
 					// aflw	lfpw
 					// 39 new RET	30
 					// right_ear_top
 					featureID = 39;
          		    break;
          		  }
          		  case 31:
 				  {
 					// aflw	lfpw
 					//13 LE	31
 					featureID = 13;
          		    break;
          		  }
          		  case 32:
 				  {
 					// aflw	lfpw
 					//17 RE	32
 					featureID = 17;
          		    break;
          		  }
          		  case 33:
 				  {
 					// aflw	lfpw
 					// 40 new LEC	33
 					// left_ear_canal
 					featureID = 40;
          		    break;
          		  }
          		  case 34:
 				  {
 					// aflw	lfpw
 					// 41 new REC	34
 					// right_ear_canal
 					featureID = 41;
          		    break;
          		  }
          		  case 35:
 				  {
 					// aflw	lfpw
 					//21 CC	29
 					featureID = 21;
          		    break;
          		  }
          		  
          }
 //aflw 19 has no lfpw feature
 return featureID;

}


int LFPWFilesReader::numFaces()
{
	return _featureCoords.size();
}

FeaturesCoords LFPWFilesReader::getFeatureCoords(int fileNum)
{
	if (fileNum < _featureCoords.size())
		return _featureCoords[fileNum];
	else
		return FeaturesCoords(0);
}

std::string LFPWFilesReader::getFileName(int fileNum)
{
	if (fileNum < _filenames.size())
		return _filenames[fileNum];
	else
		return "";
}

bool LFPWFilesReader::getFileVisibility(int fileNum)
{
     if (fileNum < _visibility.size())
		return _visibility[fileNum];
	else
		return true;
}

std::string LFPWFilesReader::getFilePath(int fileNum)
{
	if (fileNum < _filepaths.size())
		return _filepaths[fileNum];
	else
		return "";
}

std::string LFPWFilesReader::getTotalFileName(int fileNum)
{
	if (fileNum < _totalfilenames.size())
		return _totalfilenames[fileNum];
	else
		return "";
}

// returns true is the number is not in the list
//  of the invalid images and else false.
bool LFPWFilesReader::validImg(string number, bool test)
{
   
   bool return_value = true;
   
   if(test)
   {
      for(int i = 0; (i < _invalidTest.size()) && (return_value) ; i++)
      {
       
       if  ( number.compare(_invalidTest[i]) == 0)
           return_value = false;
      }
   }
   else
   {
      for(int i = 0; (i < _invalidTrain.size()) && (return_value) ; i++)
      {
       if  ( number.compare(_invalidTrain[i]) == 0)
           return_value = false;
      }
   }
      
   return return_value;
}

