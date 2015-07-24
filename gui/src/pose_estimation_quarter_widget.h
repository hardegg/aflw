
#ifndef __POSE_ESTIMATION_QUARTER_WIDGET_H__
#define __POSE_ESTIMATION_QUARTER_WIDGET_H__

#include <Quarter/QuarterWidget.h>
#include <cv.h>

class SoSeparator;
class SbMatrix;
class SbVec3f;
class SoCoordinate3;

using namespace SIM::Coin3D::Quarter;

class PoseEstimationQuarterWidget : public QuarterWidget
{
public:
   explicit PoseEstimationQuarterWidget(QWidget *parent = 0);

   virtual ~PoseEstimationQuarterWidget();

   void setOpenGLTexture(GLuint textureName) { m_textureName = textureName; }

   void applyTransformationToSceneGraph(const double& roll, const double& pitch, const double& yaw);

   SoCoordinate3 * getModelCoords();

   struct Angles
   {
      double m_currentRollAngle;
      double m_currentPitchAngle;
      double m_currentYawAngle;
   };

protected:
   virtual void initializeGL();
   virtual void paintGL();

private:

   void createRollPitchYawMatrix(float * matrix, const double& roll, const double& pitch, const double& yaw);

   SoSeparator * m_root;
   GLuint m_textureName;

   Angles m_angles;

   CvMatr32f m_fullRotation;
   CvMatr32f m_rotateCoordSystem;
   CvMatr32f m_rotateCoordSystemInverse;
   CvMatr32f m_rollPitchYawRotation;
};

#endif // __POSE_ESTIMATION_QUARTER_WIDGET_H__
