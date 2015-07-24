

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>
#include <QDir>

#include <Inventor/SbVec2d.h>
#include <Inventor/SbVec3d.h>
#include <Inventor/SbVec3f.h>

#include <map>
#include <vector>

#include <cv.h>

class ImageViewerGLWidget;
class ImageViewer;
class PoseEstimationQuarterWidget;
class SQLiteDBConnection;
class FaceData;
class FaceMetadata;
class FeaturePointRadioButton;

class QPushButton;
class QRadioButton;
class QSplitter;
class QGroupBox;
class QCheckBox;
class QButtonGroup;
class QLineEdit;
class QProgressBar;
class QLabel;
class QLineEdit;
class QGraphicsSceneMouseEvent;
class QAction;


#include "facedata/FeatureCoordTypes.h"
#include "util/MeanFace3DModel.h"


#define MAIN_WINDOW_NR_DIFFERENT_DISPLAY_MODES 3

const int NumFeatureCodes = 21;

const std::string FeatureCodes[NumFeatureCodes] =
	{ "LBLC", "LBC", "LBRC", "RBLC", "RBC", "RBRC", "LELC", "LEC", "LERC", "RELC", "REC", "RERC", "LE", "LN", "NC", "RN", "RE", "MLC", "MC", "MRC", "CC" };

typedef std::string FeatureCode;

enum MainWindowSelectFeatureMode
{
  LeftBrowLeftCornerFeature, LeftBrowCenterFeature, LeftBrowRightCornerFeature,
  RightBrowLeftCornerFeature, RightBrowCenterFeature, RightBrowRightCornerFeature,
  LeftEyeLeftCornerFeature, LeftEyeFeature, LeftEyeRightCornerFeature,
  RightEyeLeftCornerFeature, RightEyeFeature, RightEyeRightCornerFeature,
  LeftEarFeature,
  LeftNoseFeature, NoseFeature, RightNoseFeature,
  RightEarFeature,
  MouthLeftCornerFeature,
  MouthFeature, 
  MouthRightCornerFeature,  
  ChinFeature,  
  NoFeature
};

//-----------------------------------------------------------------------------
// MAIN WINDOW
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
   Q_OBJECT

public:

   explicit MainWindow();
   
   virtual ~MainWindow();
   
public slots:

signals:

   void newOriginalImageAvailable(IplImage *);
   void newOriginalImageFilename(const std::string&);
   void clearGLWidgets();
   
protected:
   virtual void keyPressEvent ( QKeyEvent * event );

private slots:
   void on_mousePress( QGraphicsSceneMouseEvent * event );

   void openDatabase(QString dbFilename = QString());
  
   void loadNextImage(int fileIndex = -1);
   void gotoNextUnlabeledImage();
   void loadPreviousImage();

   void faceAutoZoom();

   void quit();
   void save();
   
   void touchFeatures();
   void getUnlabeledImages();

   void toggleFeatureButton(FeatureCode featCode, bool checked);
   void toggleFeatureButton(MainWindowSelectFeatureMode featCode, bool checked);

   void clearFeatures();
   void clearMetaData();
   void clearAngles();
   bool calculatePose();

   void updateMarkerImage();
   void updateProgressBar();
   void newAnnotation();
   void delAnnotation();

   void nextAnnotation(int annoIdx =-1);
   void prevAnnotation();

   void parseDirectoryIndexTextBox();
   void parseFaceIndexTextBox();

   void clear();

   //*-- FILTER --*//
   void findImage();
   void findFace();
   void filterFacesByQuery();

   //*-- RECENT FILE --*//
   void openRecentFile();

   void enableFeaturePointButtons(bool enable);

private:
   void initMaps();

   void openImageInternal(const QString &currentFilename);
   void openDirectoryInternal();
   
   void paintCurrentModeText();

   void initGUI();

   void splitRotationMatrixToRollPitchYaw(cv::Mat &rot_matrix, double& roll, double& pitch, double& yaw);
   void calculateRotationMatrixFromRollPitchYaw(cv::Mat &matrix, double roll, double pitch, double yaw);

   void removeCurrentSelectedFeaturePoint();

   void changeCurrentDisplayMode();
   void updateCurrentFolderIndex();
   void updateCurrentAnnotationIndex();
   
   void displayCurrentAnnotation();
   void clearAnnotion();
   void loadFeatureTypes();

   void toggleNaviButtons(bool enable);
   void enable(bool enable);

   void retrieveFaceDataById(std::vector<int> queryIds, bool &allOk);
   void prepareAndLoadFaceData(FaceData *fd);

   //*-- recent files --*//
   enum  { MaxRecentFiles = 5 };
   void updateRecentFileActions();
   void setCurrentFile(const QString &fileName);

   QString strippedName(const QString &fullFileName);
   QAction *recentFileActs[MaxRecentFiles];

   std::map<FeatureCode, int> m_featureCodeDBIdMap;
   std::map<FeatureCode, MainWindowSelectFeatureMode> m_featureCodeSelectModeMap;
   std::map<FeatureCode, std::string> m_featureCodeButtonText;
   std::map<std::string,std::string> m_textShorts;
   std::map<std::string,cv::Scalar> m_textColor;

   enum MainWindowDisplayMode
   {
      NoModel, TransformedFeaturePointsOnly, TransformedModel
   };
   
   //*-- IMAGE VIEWER WIDGET --*//
   ImageViewer *m_image;

   //*-- 3D HEAD MODEL WIDGET --*//
   PoseEstimationQuarterWidget * m_viewer;
   
   QString m_currentTestImageFilename;
   
   QDir m_dbBaseDir;
   QDir m_imgDir;

   QStringList m_currentFileList;
   int m_currentFileIndex;

   //*-- MENU --*//
   QMenu *m_toolsMenu;
   QMenu *m_findMenu;
   QMenu *m_viewMenu;
   QMenu *m_fileMenu;

   //*-- FILE MENU ACTs --*//
   QAction * m_openDBAction;
   QAction *m_reindexUnlabeledImagesAction;
   QAction *m_saveFeaturesAction;

   QAction *m_autosaveButton;
   QAction *m_autozoomButton;
   QAction *m_only_current_marker;

   QWidget * m_centralWidget;
   QPushButton * m_calculateButton;
   
   QPushButton * m_loadImageButton;
   QPushButton * m_loadULImageButton;
   QPushButton * m_loadPrevImageButton;
   QPushButton * m_nextFaceButton;
   QPushButton * m_prevFaceButton;
   QPushButton * m_newFaceButton;
   QPushButton * m_delFaceButton;
   QRadioButton * m_focusButton;
   QLineEdit * m_current_folder_index;
   QLineEdit * m_current_face_index;
   QProgressBar *m_current_folder_progress;
   QLabel *m_current_image_filename;
   QLabel *m_currentFaceIdLabel;

   QGroupBox *m_naviButtonGroupBox;
   QGroupBox *m_naviFaceButtonGroupBox;

   //-- META DATA GROUP BOX --//
   QGroupBox *m_metaDataGroupBox;

   QButtonGroup *m_genderButtonGroup;
   QRadioButton *m_femaleButton;
   QRadioButton *m_maleButton;
   QCheckBox *m_glassesButton;
   QCheckBox *m_occludedButton;
   QCheckBox *m_grayscaleButton;

   QLabel *m_statusBar;

   MainWindowSelectFeatureMode m_currentSelectFeatureMode;
   MainWindowDisplayMode m_currentDisplayMode;
   
   int m_numberOfSelectedFeatures;

   QGroupBox *m_buttonGroupBox;
   std::map<FeatureCode, FeaturePointRadioButton *> m_featureCodeButtonMap;
   std::map<std::string,SbVec2d> m_featuresMap; //stores the 2D coordinates of the current annotation
   std::map<std::string,SbVec3f> m_features3DModelMap; //stores the 2D coordinates of the current annotation

   std::vector<int> m_unlabeled_images;
   int m_unlabeled_index;

   //double m_fineAdjustmentAngleRoll;
   //double m_fineAdjustmentAnglePitch;
   //double m_fineAdjustmentAngleYaw;

   double m_currentRollAngle;
   double m_currentPitchAngle;
   double m_currentYawAngle;

   int m_touchedFeatures;

   cv::Mat m_tfm;

   //*-- DB STUFF --*//
   SQLiteDBConnection *m_sqlConn;
   bool m_sqlDbOpen;
   std::string m_img_db_id;
   std::string m_img_db_descr;

   FeatureCoordTypes m_fcTypes;
   std::vector<FaceData*> m_annotations;
   int m_curr_annotation;

   MeanFace3DModel m_meanFace3DModel;
};
  

#endif // __FACE_ANALYSIS_MAIN_WINDOW_H__
