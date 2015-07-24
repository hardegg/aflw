/*
 * ModernPosit.cpp
 *
 *  Created on: 22.10.2010
 *      Author: pwohlhart
 *
 *
 *  Derived from Matlab implementation http://www.cfar.umd.edu/~daniel/modernPosit.m
 *
 */

#include "ModernPosit.h"

#include <iostream>

ModernPosit::ModernPosit() {
}

ModernPosit::~ModernPosit() {
}

void ModernPosit::run(cv::Mat &rot, cv::Point3f &trans, std::vector<cv::Point2f> imagePts, std::vector<cv::Point3f> worldPts, double focalLength, cv::Point2f center, int maxIterations)
{
	int nbPoints = imagePts.size();

	std::vector<cv::Point2f>::iterator imagePtsIt;
	std::vector<cv::Point2f> centeredImage;
	for (imagePtsIt = imagePts.begin(); imagePtsIt != imagePts.end(); ++imagePtsIt)
		centeredImage.push_back(*imagePtsIt - center);
	for (imagePtsIt = centeredImage.begin(); imagePtsIt != centeredImage.end(); ++imagePtsIt)
	{
		imagePtsIt->x /= focalLength;
		imagePtsIt->y /= focalLength;
	}

	std::vector<double> ui(nbPoints);
	std::vector<double> vi(nbPoints);
	std::vector<double> oldUi(nbPoints);
	std::vector<double> oldVi(nbPoints);
	std::vector<double> deltaUi(nbPoints);
	std::vector<double> deltaVi(nbPoints);

	/*double ui[nbPoints];
	double vi[nbPoints];
	double oldUi[nbPoints];
	double oldVi[nbPoints];
	double deltaUi[nbPoints];
	double deltaVi[nbPoints];*/
	for (int i = 0; i < nbPoints; ++i)
	{
		ui[i] = centeredImage[i].x;
		vi[i] = centeredImage[i].y;
	}

	cv::Mat homogeneousWorldPts(nbPoints,4,CV_32F);
	for (int i = 0; i < nbPoints; ++i)
	{
		cv::Point3f worldPoint = worldPts[i];
		homogeneousWorldPts.at<float>(i,0) = worldPoint.x;
		homogeneousWorldPts.at<float>(i,1) = worldPoint.y;
		homogeneousWorldPts.at<float>(i,2) = worldPoint.z;
		homogeneousWorldPts.at<float>(i,3) = 1; // homogeneous
	}

	cv::Mat objectMat;
	cv::invert(homogeneousWorldPts,objectMat,cv::DECOMP_SVD);

	/*
	std::cout << "objectmat: " << std::endl;
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < nbPoints; ++i)
			std::cout << objectMat.at<float>(j,i) << " ";
		std::cout << std::endl;
	}
	*/

	bool converged = false;
	int iterationCount = 0;

	double Tx = 0.0;
	double Ty = 0.0;
	double Tz = 0.0;
	double r1T[4];
	double r2T[4];
	double r1N[4];
	double r2N[4];
	double r3[4];

	while ((!converged) && ((maxIterations < 0 ) || (iterationCount < maxIterations)))
	{
		// r1T= objectMat * ui; % pose vectors
		// r2T = objectMat * vi;
		for (int j=0; j < 4; ++j)
		{
			r1T[j] = 0;
			r2T[j] = 0;
			for (int i=0; i < nbPoints; ++i)
			{
				r1T[j] += ui[i] * objectMat.at<float>(j,i);
				r2T[j] += vi[i] * objectMat.at<float>(j,i);
			}
		}

		// Tz1 = 1/sqrt(r1T(1)*r1T(1) + r1T(2)*r1T(2)+ r1T(3)*r1T(3)); % 1/Tz1 is norm of r1T
		// Tz2 = 1/sqrt(r2T(1)*r2T(1) + r2T(2)*r2T(2)+ r2T(3)*r2T(3)); % 1/Tz2 is norm of r2T
		double Tz1,Tz2;
		Tz1 = 1/sqrt(r1T[0]*r1T[0] + r1T[1]*r1T[1]+ r1T[2]*r1T[2]);
		Tz2 = 1/sqrt(r2T[0]*r2T[0] + r2T[1]*r2T[1]+ r2T[2]*r2T[2]);

		// Tz = sqrt(Tz1*Tz2); % geometric average instead of arithmetic average of classicPosit.m
		Tz = sqrt(Tz1*Tz2);

		for (int j=0; j < 4; ++j)
		{
			r1N[j] = r1T[j]*Tz;
			r2N[j] = r2T[j]*Tz;
		}

		// DEBUG
		for (int j=0; j < 3; ++j)
		{
			if ((r1N[j] > 1.0) || (r1N[j] < -1.0))
			{
				//std::cout << "WARNING: r1N[" << j << "] == " << r1N[j] << std::endl;
				r1N[j] = std::max(-1.0,std::min(1.0,r1N[j]));
			}
			if ((r2N[j] > 1.0) || (r2N[j] < -1.0))
			{
				//std::cout << "WARNING: r2N[" << j << "] == " << r2N[j] << std::endl;
				r2N[j] = std::max(-1.0,std::min(1.0,r2N[j]));
			}
		}

		// r1 = r1N(1:3);
		// r2 = r2N(1:3);
		// r3 = cross(r1,r2);
		// r3T= [r3; Tz];
		r3[0] = r1N[1]*r2N[2] - r1N[2]*r2N[1];
		r3[1] = r1N[2]*r2N[0] - r1N[0]*r2N[2];
		r3[2] = r1N[0]*r2N[1] - r1N[1]*r2N[0];
		r3[3] = Tz;

		Tx = r1N[3];
		Ty = r2N[3];

		// wi= homogeneousWorldPts * r3T /Tz;
		
		std::vector<double> wi(nbPoints);
		//double wi[nbPoints];
		
		for (int i = 0; i < nbPoints; ++i)
		{
			wi[i] = 0;
			for (int j = 0; j < 4; ++j)
				wi[i] += homogeneousWorldPts.at<float>(i,j) * r3[j] / Tz;
		}

		// oldUi = ui;
		// oldVi = vi;
		// ui = wi .* centeredImage(:,1)
		// vi = wi .* centeredImage(:,2)
		// deltaUi = ui - oldUi;
		// deltaVi = vi - oldVi;
		for (int i = 0; i < nbPoints; ++i)
		{
			oldUi[i] = ui[i];
		    oldVi[i] = vi[i];
		    ui[i] = wi[i] * centeredImage[i].x;
		    vi[i] = wi[i] * centeredImage[i].y;
			deltaUi[i] = ui[i] - oldUi[i];
			deltaVi[i] = vi[i] - oldVi[i];
		}

		// delta = focalLength * focalLength * (deltaUi' * deltaUi + deltaVi' * deltaVi)
		double delta = 0.0;
		for (int i = 0; i < nbPoints; ++i)
			delta += deltaUi[i] * deltaUi[i] + deltaVi[i] * deltaVi[i];
		delta = delta*focalLength * focalLength;

		/*
		std::cout << "delta: " << delta << std::endl ;
		std::cout << "r1N: " << r1N[0] << " " << r1N[1] << " " << r1N[2] << " " << r1N[3] << std::endl;
		std::cout << "r2N: " << r2N[0] << " " << r2N[1] << " " << r2N[2] << " " << r2N[3] << std::endl;
		std::cout << "r1T: " << r1T[0] << " " << r1T[1] << " " << r1T[2] << " " << r1T[3] << std::endl;
		std::cout << "r2T: " << r2T[0] << " " << r2T[1] << " " << r2T[2] << " " << r2T[3] << std::endl;
		std::cout << "r3: " << r3[0] << " " << r3[1] << " " << r3[2] << " " << r3[3] << std::endl;
		*/

		// converged = (count>0 & delta < 1)
		converged = (iterationCount > 0) && (delta < 0.01);
		++iterationCount;

		//std::cout << "delta " << delta << std::endl;
	}
	// trans = [Tx; Ty; Tz];
	// rot = [r1'; r2'; r3'];
	//std::cout << "iter count " << iterationCount << std::endl;

	trans.x = Tx;
	trans.y = Ty;
	trans.z = Tz;

	rot.create(3,3,CV_32F);
	for (int i = 0; i < 3; ++i)
	{
		rot.at<float>(0,i) = r1N[i];
		rot.at<float>(1,i) = r2N[i];
		rot.at<float>(2,i) = r3[i];
	}
}
