
#include <GL/glew.h>

#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include "pose_estimation_quarter_widget.h"

#include <iostream>
#include <string>

//#include <math>

//#include <vnl/vnl_math.h>
#include <cvaux.h>


#define MATRIX_MULTIPLY(dst,src1,src2) \
{ \
   CvMatr32f tmp = new float[16]; \
   tmp[0]  = src1[0]*src2[0] + src1[1] * src2[4] + src1[2] * src2[8] + src1[3] * src2[12];\
   tmp[1]  = src1[0]*src2[1] + src1[1] * src2[5] + src1[2] * src2[9] + src1[3] * src2[13];\
   tmp[2]  = src1[0]*src2[2] + src1[1] * src2[6] + src1[2] * src2[10] + src1[3] * src2[14];\
   tmp[3]  = src1[0]*src2[3] + src1[1] * src2[7] + src1[2] * src2[11] + src1[3] * src2[15];\
   tmp[4]  = src1[4]*src2[0] + src1[5] * src2[4] + src1[6] * src2[8] + src1[7] * src2[12];\
   tmp[5]  = src1[4]*src2[1] + src1[5] * src2[5] + src1[6] * src2[9] + src1[7] * src2[13];\
   tmp[6]  = src1[4]*src2[2] + src1[5] * src2[6] + src1[6] * src2[10] + src1[7] * src2[14];\
   tmp[7]  = src1[4]*src2[3] + src1[5] * src2[7] + src1[6] * src2[11] + src1[7] * src2[15];\
   tmp[8]  = src1[8]*src2[0] + src1[9] * src2[4] + src1[10] * src2[8] + src1[11] * src2[12];\
   tmp[9]  = src1[8]*src2[1] + src1[9] * src2[5] + src1[10] * src2[9] + src1[11] * src2[13];\
   tmp[10] = src1[8]*src2[2] + src1[9] * src2[6] + src1[10] * src2[10] + src1[11] * src2[14];\
   tmp[11] = src1[8]*src2[3] + src1[9] * src2[7] + src1[10] * src2[11] + src1[11] * src2[15];\
   tmp[12] = src1[12]*src2[0] + src1[13] * src2[4] + src1[14] * src2[8] + src1[15] * src2[12];\
   tmp[13] = src1[12]*src2[1] + src1[13] * src2[5] + src1[14] * src2[9] + src1[15] * src2[13];\
   tmp[14] = src1[12]*src2[2] + src1[13] * src2[6] + src1[14] * src2[10] + src1[15] * src2[14];\
   tmp[15] = src1[12]*src2[3] + src1[13] * src2[7] + src1[14] * src2[11] + src1[15] * src2[15];\
   for (int i=0;i<16;++i) dst[i] = tmp[i]; \
   delete [] tmp; \
}

SoRenderManager * global_render_manager = NULL;

// dirty hack!
PoseEstimationQuarterWidget * global_quarter_widget = NULL;


// inventor callbacks

void
myKeyPressCB(void *myUserData, SoEventCallback *eventCB)
{
   const SoEvent *event = eventCB->getEvent();
   PoseEstimationQuarterWidget::Angles * angles = 
      (PoseEstimationQuarterWidget::Angles *) myUserData;

   const float angleIncrementRad = 1. * M_PI / 180.;

   //SbVec3f axis;
   //float angle;

   // Check for the Up and Down arrow keys being pressed.
   if (SO_KEY_PRESS_EVENT(event, LEFT_ARROW)) 
   {
      angles->m_currentYawAngle += angleIncrementRad;

      // implement a rotation around the y axis with 1 degree to the left
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationAzimuth");
      if (transformNode)
      {
         //transformNode->rotation.getValue(axis,angle);
         //std::cout << "current azimuth angle: " << angle << std::endl;
         //std::cout << "current azimuth axis: " << std::endl;
         //axis.print(stdout);
         //std::cout << std::endl;
         //transformNode->rotation.setValue(axis,angle + angleIncrementRad);

         /*angle += angleIncrement
         SoSFRotation oldSFRotation = transformNode->rotation;
         SbRotation oldRotation = oldSFRotation.getValue();

         SbRotation incrementalRotation(SbVec3f(0.0f,1.0f,0.0f),-angleIncrementRad);

         SbMatrix test;
         (oldRotation * incrementalRotation).getValue(test);

         std::cout << "current matrix: " << std::endl;
         test.print(stdout);

         SoSFRotation rot;
         rot.setValue(incrementalRotation * oldRotation);
         transformNode->rotation = rot;*/
      }
   } 
   else if (SO_KEY_PRESS_EVENT(event, RIGHT_ARROW)) 
   {
      angles->m_currentYawAngle -= angleIncrementRad;
      //std::cout << "current roll angle: " << angles->m_currentRollAngle << std::endl;

      // implement a rotation around the y axis with 1 degree to the right
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationAzimuth");
      if (transformNode)
      {
         //transformNode->rotation.getValue(axis,angle);
         //std::cout << "current azimuth angle: " << angle << std::endl;
         //std::cout << "current azimuth axis: " << std::endl;
         //axis.print(stdout);
         //std::cout << std::endl;
         //transformNode->rotation.setValue(axis,angle - angleIncrementRad);

         /*SoSFRotation oldSFRotation = transformNode->rotation;
         SbRotation oldRotation = oldSFRotation.getValue();

         SbRotation incrementalRotation(SbVec3f(0.0f,1.0f,0.0f),angleIncrementRad);

         SbMatrix test;
         (oldRotation * incrementalRotation).getValue(test);

         std::cout << "current matrix: " << std::endl;
         test.print(stdout);

         SoSFRotation rot;
         rot.setValue(incrementalRotation * oldRotation);
         transformNode->rotation = rot;*/
      }
   } 
   else if (SO_KEY_PRESS_EVENT(event, UP_ARROW)) 
   {
      angles->m_currentPitchAngle += angleIncrementRad;
      //std::cout << "current pitch angle: " << angles->m_currentPitchAngle << std::endl;

      // implement a rotation around the y axis with 1 degree to the left
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationElevation");
      if (transformNode)
      {
         /*transformNode->rotation.getValue(axis,angle);
         std::cout << "current elevation angle: " << angle << std::endl;
         std::cout << "current elevation axis: " << std::endl;
         axis.print(stdout);
         std::cout << std::endl;
         transformNode->rotation.setValue(axis,angle + angleIncrementRad);*/

         /*SoSFRotation oldSFRotation = transformNode->rotation;
         SbRotation oldRotation = oldSFRotation.getValue();

         SbRotation incrementalRotation(SbVec3f(1.0f,0.0f,0.0f),-angleIncrementRad);

         SbMatrix test;
         (oldRotation * incrementalRotation).getValue(test);

         std::cout << "current matrix: " << std::endl;
         test.print(stdout);

         SoSFRotation rot;
         rot.setValue(incrementalRotation * oldRotation);
         transformNode->rotation = rot;*/
      }
   } 
   else if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW)) 
   {
      angles->m_currentPitchAngle -= angleIncrementRad;
      //std::cout << "current pitch angle: " << angles->m_currentPitchAngle << std::endl;

      // implement a rotation around the y axis with 1 degree to the right
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationElevation");
      if (transformNode)
      {
         /*transformNode->rotation.getValue(axis,angle);
         std::cout << "current elevation angle: " << angle << std::endl;
         std::cout << "current elevation axis: " << std::endl;
         axis.print(stdout);
         std::cout << std::endl;
         transformNode->rotation.setValue(axis,angle - angleIncrementRad);*/

         /*SoSFRotation oldSFRotation = transformNode->rotation;
         SbRotation oldRotation = oldSFRotation.getValue();

         SbRotation incrementalRotation(SbVec3f(1.0f,0.0f,0.0f),angleIncrementRad);

         SbMatrix test;
         (oldRotation * incrementalRotation).getValue(test);

         std::cout << "current matrix: " << std::endl;
         test.print(stdout);

         SoSFRotation rot;
         rot.setValue(incrementalRotation * oldRotation);
         transformNode->rotation = rot;*/
      }
   }
   else if (SO_KEY_PRESS_EVENT(event, PAGE_UP)) 
   {
      angles->m_currentRollAngle += angleIncrementRad;
      //std::cout << "current yaw angle: " << angles->m_currentYawAngle << std::endl;

      // implement a rotation around the y axis with 1 degree to the right
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationTwist");
      if (transformNode)
      {
         /*transformNode->rotation.getValue(axis,angle);
         std::cout << "current twist angle: " << angle << std::endl;
         std::cout << "current twist axis: " << std::endl;
         axis.print(stdout);
         std::cout << std::endl;
         transformNode->rotation.setValue(axis,angle + angleIncrementRad);*/
      }
   }
   else if (SO_KEY_PRESS_EVENT(event, PAGE_DOWN)) 
   {
      angles->m_currentRollAngle -= angleIncrementRad;
      //std::cout << "current yaw angle: " << angles->m_currentYawAngle << std::endl;

      // implement a rotation around the y axis with 1 degree to the right
      SoRotation * transformNode = (SoRotation*) SoNode::getByName("MyFineAdjustmentRotationTwist");
      if (transformNode)
      {
         /*transformNode->rotation.getValue(axis,angle);
         std::cout << "current twist angle: " << angle << std::endl;
         std::cout << "current twist axis: " << std::endl;
         axis.print(stdout);
         std::cout << std::endl;
         transformNode->rotation.setValue(axis,angle - angleIncrementRad);*/
      }
   }
   else if (SO_KEY_PRESS_EVENT(event, H)) 
   {
      angles->m_currentYawAngle = 0.0;
      angles->m_currentPitchAngle = 0.0;
      angles->m_currentRollAngle = 0.0;
   }

   eventCB->setHandled();

   //if (global_render_manager)
   //   global_render_manager->render();
   if (global_quarter_widget)
      global_quarter_widget->updateGL();
}

void
myMouseCB(void *userData, SoEventCallback *eventCB)
{
   const SoEvent *event = eventCB->getEvent();
   SoSeparator * scenegraph = (SoSeparator*) userData;

   if (SO_MOUSE_PRESS_EVENT(event, BUTTON2)) 
   {
      std::cout << "mouse button 2" << std::endl;

      if (global_render_manager)
      {
         // attempting raypick in the event_cb() callback method
         SoRayPickAction rp( global_render_manager->getViewportRegion() );
         rp.setPoint(event->getPosition());
         rp.setPickAll(true);

         std::cout << "event position: " << event->getPosition()[0] << " " << event->getPosition()[1]
            << std::endl;
         
         rp.apply(global_render_manager->getSceneGraph());

         const SoPickedPointList& pickedPoints = rp.getPickedPointList();
         std::cout << "# of picked points: " << pickedPoints.getLength() << std::endl;
         for (int i=0; i<pickedPoints.getLength(); ++i)
         {
            SoPickedPoint * pickedPoint = pickedPoints[i];
            SbVec3f point = pickedPoint->getPoint();
            std::cout << "point " << i << " : " << point[0] << " " << point[1] << " " <<
               point[2] << std::endl;
         }
      }

      eventCB->setHandled();
   }
   else if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) 
   {
      //std::cout << "mouse button 1" << std::endl;
      eventCB->setHandled();
   }
   else if (SO_MOUSE_PRESS_EVENT(event, BUTTON3)) 
   {
      //std::cout << "mouse button 3" << std::endl;
      eventCB->setHandled();
   }
}


PoseEstimationQuarterWidget::PoseEstimationQuarterWidget(QWidget *parent) 
   : QuarterWidget() //QuarterWidget(parent)
   , m_root(0)
   , m_textureName(0)
{
	m_angles.m_currentRollAngle = 0.0;
	m_angles.m_currentPitchAngle = 0.0;
	m_angles.m_currentYawAngle = 0.0;

	m_fullRotation = new float[16];
	m_rotateCoordSystem = new float[16];
	m_rotateCoordSystemInverse = new float[16];
	m_rollPitchYawRotation = new float[16];

	// relates opengl and roll,pitch,yaw coordinate systems
	m_rotateCoordSystem[0]  =  0.0; m_rotateCoordSystem[1]  =  0.0; m_rotateCoordSystem[2]  = 1.0; m_rotateCoordSystem[3]  = 0.0;
	m_rotateCoordSystem[4]  = -1.0; m_rotateCoordSystem[5]  =  0.0; m_rotateCoordSystem[6]  = 0.0; m_rotateCoordSystem[7]  = 0.0;
	m_rotateCoordSystem[8]  =  0.0; m_rotateCoordSystem[9]  = -1.0; m_rotateCoordSystem[10] = 0.0; m_rotateCoordSystem[11] = 0.0;
	m_rotateCoordSystem[12] =  0.0; m_rotateCoordSystem[13] =  0.0; m_rotateCoordSystem[14] = 0.0; m_rotateCoordSystem[15] = 1.0;

	m_rotateCoordSystemInverse[0]  = 0.0; m_rotateCoordSystemInverse[1]  = -1.0; m_rotateCoordSystemInverse[2]  =  0.0; m_rotateCoordSystemInverse[3]  = 0.0;
	m_rotateCoordSystemInverse[4]  = 0.0; m_rotateCoordSystemInverse[5]  =  0.0; m_rotateCoordSystemInverse[6]  = -1.0; m_rotateCoordSystemInverse[7]  = 0.0;
	m_rotateCoordSystemInverse[8]  = 1.0; m_rotateCoordSystemInverse[9]  =  0.0; m_rotateCoordSystemInverse[10] =  0.0; m_rotateCoordSystemInverse[11] = 0.0;
	m_rotateCoordSystemInverse[12] = 0.0; m_rotateCoordSystemInverse[13] =  0.0; m_rotateCoordSystemInverse[14] =  0.0; m_rotateCoordSystemInverse[15] = 1.0;

	//*-- IMPORTS 3D SHAPE AND SETUP --*//
	m_root = new SoSeparator;
	m_root->ref();

	SoRenderManager * renderManager = this->getSoRenderManager();
	global_render_manager = renderManager;
   
   
	global_quarter_widget = this;

	this->setNavigationModeFile();  

	SoFile *file = new SoFile;
	file->name.setValue("data/meanFace2.wrl");

	SoMatrixTransform *transform = new SoMatrixTransform;
	SbName name("MyPoseFittingTransform");
	transform->setName(name);

	SoRotation *rotation = new SoRotation;
	SoSeparator *separator = new SoSeparator;

	separator->addChild(transform);
	separator->addChild(rotation);
	separator->addChild(file);
	m_root->addChild(separator);

	//// keyboard event callback
	//  SoEventCallback *myEventCB = new SoEventCallback;
	//  myEventCB->addEventCallback(
	//        SoKeyboardEvent::getClassTypeId(), 
	//        myKeyPressCB, &m_angles);

	// mouse event callback
	SoEventCallback *myEventCB2 = new SoEventCallback;
	myEventCB2->addEventCallback(
	      SoMouseButtonEvent::getClassTypeId(),
	      myMouseCB, file);
	m_root->addChild(myEventCB2);

    this->setSceneGraph(m_root);
	this->viewAll();
}

PoseEstimationQuarterWidget::~PoseEstimationQuarterWidget()
{
   delete [] m_rotateCoordSystem;
   delete [] m_rotateCoordSystemInverse;
   delete [] m_fullRotation;
   delete [] m_rollPitchYawRotation;

   global_render_manager = NULL;
   
   // Clean up resources.
   m_root->unref();
}


void PoseEstimationQuarterWidget::initializeGL()
{
   QuarterWidget::initializeGL();
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      // Problem: glewInit failed, something is seriously wrong.
      std::cout << "Error: glewInit failed with: " << glewGetErrorString(err)
         << std::endl;
   }
}

SoCoordinate3 * PoseEstimationQuarterWidget::getModelCoords()
{
   SoCoordinate3 * model = (SoCoordinate3*) SoNode::getByName("MyCoords");
   if (model)
      return model;
   else
   {
      std::cerr << "Couldn't extract model coordinates" << std::endl;
      return 0;
   }
}

void PoseEstimationQuarterWidget::applyTransformationToSceneGraph(const double& roll, const double& pitch, const double& yaw)
{
   m_angles.m_currentRollAngle = roll;
   m_angles.m_currentPitchAngle = pitch;
   m_angles.m_currentYawAngle = yaw;

   updateGL();
}


void PoseEstimationQuarterWidget::createRollPitchYawMatrix(float * matrix, 
   const double& roll, const double& pitch, const double& yaw)
{
   const double alpha = roll;
   const double beta  = pitch;
   const double gamma = yaw;

   const double sinAlpha = sin(alpha);
   const double cosAlpha = cos(alpha);
   const double sinBeta  = sin(beta);
   const double cosBeta  = cos(beta);
   const double sinGamma = sin(gamma);
   const double cosGamma = cos(gamma);

   matrix[0] = cosGamma*cosBeta; matrix[4] = cosGamma*sinBeta*sinAlpha - sinGamma*cosAlpha; matrix[8]  = cosGamma*sinBeta*cosAlpha + sinGamma*sinAlpha; matrix[12] = 0.0;
   matrix[1] = sinGamma*cosBeta; matrix[5] = sinGamma*sinBeta*sinAlpha + cosGamma*cosAlpha; matrix[9]  = sinGamma*sinBeta*cosAlpha - cosGamma*sinAlpha; matrix[13] = 0.0;
   matrix[2] = -sinBeta;         matrix[6] = cosBeta*sinAlpha;                              matrix[10] = cosBeta*cosAlpha;                              matrix[14] = 0.0;
   matrix[3] = 0.0;              matrix[7] = 0.0;                                           matrix[11] = 0.0;                                           matrix[15] = 1.0;

/*
[ cos(b)*cos(c), cos(c)*sin(a)*sin(b) - cos(a)*sin(c), sin(a)*sin(c) + cos(a)*cos(c)*sin(b), 0]
[ cos(b)*sin(c), cos(a)*cos(c) + sin(a)*sin(b)*sin(c), cos(a)*sin(b)*sin(c) - cos(c)*sin(a), 0]
[       -sin(b),                        cos(b)*sin(a),                        cos(a)*cos(b), 0]
[             0,                                    0,                                    0, 1]
*/
}

void PoseEstimationQuarterWidget::paintGL()
{
   // draw everything the base class has to draw
   QuarterWidget::paintGL();

   SoCamera * camera = global_render_manager->getCamera();
   //std::cout << std::endl << "camera: pos = " << camera->position.getValue()[0] << ";" << camera->position.getValue()[1]
   //   << ";" << camera->position.getValue()[2] << std::endl;

   // do the additional stuff
   
   createRollPitchYawMatrix(m_rollPitchYawRotation, 
      m_angles.m_currentRollAngle, m_angles.m_currentPitchAngle,
      m_angles.m_currentYawAngle);

   MATRIX_MULTIPLY(m_fullRotation,m_rollPitchYawRotation,m_rotateCoordSystem);
   MATRIX_MULTIPLY(m_fullRotation,m_rotateCoordSystemInverse,m_fullRotation);

   const SbMatrix transformationMatrix(
      m_fullRotation[0], m_fullRotation[1], m_fullRotation[2], m_fullRotation[3],
      m_fullRotation[4], m_fullRotation[5], m_fullRotation[6], m_fullRotation[7],
      m_fullRotation[8], m_fullRotation[9], m_fullRotation[10], m_fullRotation[11],
      m_fullRotation[12], m_fullRotation[13], m_fullRotation[14], m_fullRotation[15] );

   //transformationMatrix.print(stdout);
   //std::cout << std::endl;

   SoMatrixTransform * transformNode = 
      (SoMatrixTransform*) SoNode::getByName("MyPoseFittingTransform");
   if (transformNode)
   {
      transformNode->matrix.setValue(transformationMatrix);
   }

   return;

   //std::cout << " orientation = " << std::endl;
   //camera->orientation.getValue().print(stdout);
   //std::cout << std::endl;

   //SbVec3f upvec(0, 1, 0); // init to default up vector
   //camera->orientation.getValue().multVec(upvec, upvec);
   //std::cout << "upvec:" << std::endl;
   //upvec.print(stdout);
   //std::cout << std::endl;

   //SbVec3f lookat(0, 0, -1); // init to default view direction vector
   //camera->orientation.getValue().multVec(lookat, lookat);
   //std::cout << "lookat:" << std::endl;
   //lookat.print(stdout);
   //std::cout << std::endl;

   //std::cout << "near distance: " << camera->nearDistance.getValue() << std::endl;
   //std::cout << "far distance: " << camera->farDistance.getValue() << std::endl;
   //std::cout << "focal distance: " << camera->focalDistance.getValue() << std::endl;
}

#undef MATRIX_MULTIPLY
