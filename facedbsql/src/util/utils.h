/*! \file
* \brief some utils as type conversions, log2, randperm, deinterlacing
*
* Copyright (c) 2011 Martin Koestinger / ICG
*
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <functional>
#include <map>
#include <algorithm>

#include <cv.h>

#ifdef LIBCFG
	#include <libconfig.h++>
#endif 

namespace utils {

//-----------------------------------------------------------------------------
// conversion error
//-----------------------------------------------------------------------------
class BadConversion : public std::runtime_error {
public:
BadConversion(std::string const& s)
 : std::runtime_error(s)
 { }
};
//-----------------------------------------------------------------------------
// string to ...
//-----------------------------------------------------------------------------
 inline double convertToDouble(std::string const& s)
 {
   std::istringstream i(s);
   double x;
   if (!(i >> x))
     throw BadConversion("convertToDouble(\"" + s + "\")");
   return x;
 } 

 inline int convertToInt(std::string const& s)
 {
   std::istringstream i(s);
   int x;
   if (!(i >> x))
     throw BadConversion("convertToInt(\"" + s + "\")");
   return x;
 } 

 inline float convertToFloat(std::string const& s)
 {
   std::istringstream i(s);
   float x;
   if (!(i >> x))
     throw BadConversion("convertToFloat(\"" + s + "\")");
   return x;
 } 
//-----------------------------------------------------------------------------
// stringify ... number to string
//-----------------------------------------------------------------------------
inline std::string stringify(double x, bool fixed = false)
{ 
std::ostringstream o;
if(fixed)
	o.setf(std::ios::fixed);
if (!(o << x))
 throw BadConversion("stringify(double)");

return o.str();
}
//-----------------------------------------------------------------------------
#ifdef LIBCFG
template<typename _Tp> inline  std::vector<_Tp > cfgArray2Vec(libconfig::Setting &setting)
{
	if(setting.isArray())
	{
		std::vector<_Tp > vec;
		for(int counter=0; counter < setting.getLength(); ++counter)
			vec.push_back(setting[counter]);

		return vec;
	}

	if(setting.isScalar())
	{
		std::vector<_Tp > vec;
		vec.push_back(setting);
		return vec;
	}

	throw BadConversion("cfgArray2Vec(...)");
}
#endif 
//-----------------------------------------------------------------------------
// round
//-----------------------------------------------------------------------------
inline int round(double r) {
    return int(r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}
//-----------------------------------------------------------------------------
// template to export a cv::Mat to csv
//-----------------------------------------------------------------------------
template<typename _Tp> static void cvMat2CSV(const cv::Mat &output, const std::string filename)
{
	std::ofstream data;
	data.open(filename.c_str());
	
	for (int y = 0; y < output.rows; y++)
	{
	  for (int x = 0; x < output.cols; x++)
	  {
		  data << output.at<_Tp>(y,x) << ",";
	  }
	  data << std::endl;
	 }
	 data.close();
}
//-----------------------------------------------------------------------------
// Calculates log2 of number.  
//-----------------------------------------------------------------------------
inline double log2( double n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2.0 );  
}
//-----------------------------------------------------------------------------
// LOG2
//-----------------------------------------------------------------------------
inline unsigned int log2 (unsigned int val) {
    unsigned int ret = -1;
    while (val != 0) {
        val >>= 1;
        ret++;
    }
    return ret;
}
//-----------------------------------------------------------------------------
// value index struct, e.g. to sort vector
//-----------------------------------------------------------------------------
template <class T>
struct ValueIndex {
    T val;
	unsigned int index;
	bool operator<(const ValueIndex& a) const { return val<a.val; }
};
//-----------------------------------------------------------------------------
// random permutation of a vector
//-----------------------------------------------------------------------------
inline void randperm(std::vector<int> &p, size_t n, const CvRNG* pRNG)
{
	//double cvRandReal( pRNG );
	//std::vector<double> 

	std::vector<ValueIndex<double> > tmpvec(n);
	p.resize(n);

	for(unsigned int c = 0; c < n; ++c)
	{
		ValueIndex<double> tmp;
		tmp.val = cvRandReal(const_cast<CvRNG*>(pRNG));
		tmp.index = c;
		tmpvec.at(c) = tmp;
	}

	std::sort(tmpvec.begin(),tmpvec.end());
	
	for(unsigned int c = 0; c < n; ++c)
	{
		p.at(c) = tmpvec.at(c).index;
	}
}
//-----------------------------------------------------------------------------
// simple deinterlace discards even or odd rows
//-----------------------------------------------------------------------------
inline void deinterlace(cv::Mat image, bool preserveEvenRows = true)
{
  size_t step = image.step;
  
  uchar *data;
  uchar *lookahead;

  if(preserveEvenRows)
  {
	data = image.data;
	lookahead = image.data + step;
  }
  else
  {
	data = image.data + step;
	lookahead = image.data;
  }

  for(size_t rowCounter = 0; rowCounter + 1 < image.rows; ++++rowCounter)
  {
    memcpy(data,lookahead,step);

	data += 2 * step;
	lookahead += 2* step;
  }
}
//-----------------------------------------------------------------------------
// median
//-----------------------------------------------------------------------------
template<typename _Tp> cv::Scalar median(cv::Mat mat)
{
	CV_Assert( mat.channels() <= 4);

	cv::Scalar median;
	_Tp value;

	for(size_t chCounter = 0; chCounter < mat.channels(); ++chCounter)
	{
		std::vector<_Tp> values(mat.rows*mat.cols);

		for (int y = 0; y < mat.rows; y++)
		{
			for (int x = 0; x < mat.cols; x++)
			{
				value = mat.at<_Tp>(y,x);
				values.at(y*mat.cols + x) = value;
			}
		}

		if(values.size() < 1)
			return median;

		std::sort(values.begin(), values.end());
		median[chCounter] = values.at(values.size()/2);
	}

	return median;
}
//-----------------------------------------------------------------------------
// Similar to matlab quiver. Draws velocity arrows
//-----------------------------------------------------------------------------
template<typename _Tp> void quiver(cv::Mat &velx, cv::Mat &vely, cv::Mat &img, cv::Size blockSize = cv::Size(1,1))
{
	float xv, yv;

	cv::Point blockOrigin;

	for (int y = 0; y < velx.rows; y++)
	{
		for (int x = 0; x < velx.cols; x++)
		{
			xv = velx.at<_Tp>(y,x);
			yv = vely.at<_Tp>(y,x);

			blockOrigin.y = (y + 1) * blockSize.height - blockSize.height / 2;
			blockOrigin.x = (x + 1) * blockSize.width  - blockSize.width  / 2;

			cv::line(img,blockOrigin,blockOrigin+cv::Point(xv,yv),cv::Scalar(255,0,0));
			if(abs(xv + xv) > 1)
				cv::circle(img,blockOrigin+cv::Point(xv,yv),2,cv::Scalar(255,0,0));
		}
	 }
}
//-----------------------------------------------------------------------------
// replaces the line feed character
//-----------------------------------------------------------------------------
struct replace_lf //: public unary_function<TYPE, void> 
{
 void operator() (char& x) 
 {
   if(x==10)
       x=32;
 }
};
//-----------------------------------------------------------------------------
// Functor for deleting pointers in a map.
//-----------------------------------------------------------------------------
template<class KEY, class POINTER>
struct DeleteMapFntor
{
	// Overloaded () operator.
	// This will be called by for_each() function.
	bool operator()(const std::pair<KEY,POINTER> &mapItem) const
	{
		// Assuming the second item of map is to be deleted
		delete mapItem.second;
		return true;
	}
};
//-----------------------------------------------------------------------------
// Functor for deleting pointers in a vector.
//-----------------------------------------------------------------------------
template<class POINTER>
struct DeleteVectorFntor
{
	bool operator()(const POINTER &vectorItem) const
	{
		delete vectorItem;
		return true;
	}
};
//-----------------------------------------------------------------------------

}

#endif

