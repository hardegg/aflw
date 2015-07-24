//STD
#include <iostream>
#include <fstream>
#include <math.h>

//QT
#include <QFileDialog>
#include <QKeyEvent>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QSplitter>
#include <QTabWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QShortcut>
#include <QButtonGroup>
#include <QIcon>
#include <QLineEdit>
#include <QRegExp>
#include <QValidator>
#include <QGraphicsSceneMouseEvent>
#include <QDirIterator>
#include <QtDebug>
#include <QInputDialog>
#include <QStatusBar>
#include <QSettings>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

//CV
#include <highgui.h>

//COIN
#include <Inventor/SbVec3d.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/SbRotation.h>
#include <Inventor/nodes/SoCoordinate3.h>

//gui 
#include "ImageViewer.h"
#include "main_window.h"
#include "pose_estimation_quarter_widget.h"
#include "FeaturePointRadioButton.h"

//Queries
#include "querys/FaceIDsByImageQuery.h"
#include "querys/FaceDataByIDsQuery.h"
#include "querys/DeleteFacesByIdsQuery.h"
#include "facedata/FeatureCoordTypes.h"
#include "querys/FileIDOfImagesWithFaces.h"
#include "querys/ImageByFaceIDQuery.h"
#include "querys/FaceDBQuery.h"
#include "util/ModernPosit.h"
#include "facedbtool/FaceRectCalculator.h"

#include "dbconn/SQLiteDBConnection.h"

#include "utils.h"

//*-- some globals :-) *-//
const std::string undefinedCoordinateText = std::string("-1;-1");
const double undefinedAngle = Pose::UNDEFINED_ANGLE;
const double focusFactor = 1.5;
const SbVec2d undefined(-1.,-1.);

const int MIN_NUM_FEATURES = 4;

//ANNOTATION_TYPE_CALC
const int ANNOTATION_TYPE_CALC = 1;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MainWindow::initMaps()
{
	for(int counter = 0; counter < NumFeatureCodes; ++counter)
	   m_featureCodeSelectModeMap[FeatureCodes[counter]] = static_cast<MainWindowSelectFeatureMode>(counter); //same value as enum MainWindowSelectFeatureMode!
	
	for(int counter = 0; counter < NumFeatureCodes; ++counter)
		m_featuresMap[FeatureCodes[counter]] = SbVec2d();
//			
	//*-- COLOR -*//
	m_textColor["LBLC"] = CV_RGB(0, 255, 255); //255 102 0
	m_textColor["LBC" ] = CV_RGB(0, 255, 255);
	m_textColor["LBRC"] = CV_RGB(0, 255, 255);
	m_textColor["RBLC"] = CV_RGB(255, 102, 0);
	m_textColor["RBC" ] = CV_RGB(255, 102, 0);
	m_textColor["RBRC"] = CV_RGB(255, 102, 0);
	m_textColor["LELC"] = CV_RGB(255, 0, 0);
	m_textColor["LEC" ] = CV_RGB(255, 0, 0);
	m_textColor["LERC"] = CV_RGB(255, 0, 0);
	m_textColor["RELC"] = CV_RGB(0, 255, 0);
	m_textColor["REC"] = CV_RGB(0, 255, 0);
	m_textColor["RERC"] = CV_RGB(0, 255, 0);
	m_textColor["LE"] = CV_RGB(150,0,255);
	m_textColor["LN"] = CV_RGB(255, 255, 0);
	m_textColor["NC"] = CV_RGB(255, 255, 0);
	m_textColor["RN"] = CV_RGB(255, 255, 0);
	m_textColor["RE"] = CV_RGB(150,0,255);
	m_textColor["MLC"] = CV_RGB(0, 0, 255);
	m_textColor["MC"] = CV_RGB(0, 0, 255);
	m_textColor["MRC"] = CV_RGB(0, 0, 255);
	m_textColor["CC"] = CV_RGB(150,0,255);

	//*-- TEXT -*//
	m_featureCodeButtonText["LBLC"] = std::string("Left Brow Left Corner (A) ");
	m_featureCodeButtonText["LBC" ] = std::string("Left Brow Center (B) ");
	m_featureCodeButtonText["LBRC"] = std::string("Left Brow Right Corner (C) ");
	m_featureCodeButtonText["RBLC"] = std::string("Right Brow Left Corner (D) ");
	m_featureCodeButtonText["RBC" ] = std::string("Right Brow Center (E) ");
	m_featureCodeButtonText["RBRC"] = std::string("Right Brow Right Corner (F) ");
	m_featureCodeButtonText["LELC"] = std::string("Left Eye Left Corner (G) ");
	m_featureCodeButtonText["LEC" ] = std::string("Left Eye Center (H) ");
	m_featureCodeButtonText["LERC"] = std::string("Left Eye Right Corner (I) ");
	m_featureCodeButtonText["RELC"] = std::string("Right Eye Left Corner (J) ");
	m_featureCodeButtonText["REC"] = std::string("Right Eye Center (K) ");
	m_featureCodeButtonText["RERC"] = std::string("Right Eye Right Corner (L) ");
	m_featureCodeButtonText["LE"] = std::string("Left Ear (M) ");
	m_featureCodeButtonText["LN"] = std::string("Nose Left (N) ");
	m_featureCodeButtonText["NC"] = std::string("Nose Center (O) ");
	m_featureCodeButtonText["RN"] = std::string("Nose Right (P) ");
	m_featureCodeButtonText["RE"] = std::string("Right Ear (Q) ");
	m_featureCodeButtonText["MLC"] = std::string("Mouth Left Corner (R) ");
	m_featureCodeButtonText["MC"] = std::string("Mouth Center (S) ");
	m_featureCodeButtonText["MRC"] = std::string("Mouth Right Corner (T) ");
	m_featureCodeButtonText["CC"] = std::string("Chin Center (U) ");

	//*-- TEXT ABR -*//
	m_textShorts.insert(std::make_pair<std::string,std::string>("LBLC", "A"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LBC" , "B"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LBRC", "C"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RBLC", "D"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RBC", "E"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RBRC", "F"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LELC", "G"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LEC", "H"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LERC", "I"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RELC", "J"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("REC", "K"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RERC", "L"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LE", "M"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("LN", "N"));
	m_textShorts.insert(std::make_pair<std::string,std::string>("NC", "O"));
	m_textShorts.insert(std::make_pair<std::string,std::string>("RN", "P"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("RE", "Q"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("MLC", "R"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("MC", "S"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("MRC", "T"));
    m_textShorts.insert(std::make_pair<std::string,std::string>("CC", "U"));
}
//-----------------------------------------------------------------------------
// CTOR
//-----------------------------------------------------------------------------
MainWindow::MainWindow()
   : QMainWindow()
   , m_currentTestImageFilename("")
//   , m_currentDirectory("")
   , m_currentFileIndex(0)
   , m_currentSelectFeatureMode(NoFeature)
   , m_currentDisplayMode(NoModel)
   , m_numberOfSelectedFeatures(0)
   , m_touchedFeatures(0x0)
   , m_unlabeled_index(0)
   , m_curr_annotation(0)
   , m_current_face_index(0)
   , m_nextFaceButton(0)
   , m_prevFaceButton(0)
   , m_sqlConn(0)
   , m_sqlDbOpen(false)
   , m_img_db_id("")
{
   m_sqlConn = new SQLiteDBConnection();
   m_img_db_id = ""; 

   m_image = new ImageViewer(this);

   initMaps();
   initGUI();
   
   clear();
}
//-----------------------------------------------------------------------------
// LOAD FEATURE TYPES OF DATABASE
//-----------------------------------------------------------------------------
void MainWindow::loadFeatureTypes()
{
	m_fcTypes.load(m_sqlConn);
	int code = -1;

	for(int counter = 0; counter < NumFeatureCodes; ++counter)
	{
		code = m_fcTypes.getIDByCode(FeatureCodes[counter]);

		if(code == -1)
		{
			qDebug() << "ERROR: couldn't read feature codes properly '";
			QMessageBox::warning(this, tr("ERROR"), tr("couldn't read feature codes properly."), QMessageBox::Ok);

		}
		m_featureCodeDBIdMap[FeatureCodes[counter]] = code;
	}
}
//-----------------------------------------------------------------------------
// DTOR
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
   if(m_sqlDbOpen)
	 m_sqlConn->close();
   delete m_sqlConn;
   m_sqlConn = 0;
}
//-----------------------------------------------------------------------------
void MainWindow::clearFeatures()
{
	m_numberOfSelectedFeatures = 0;
	m_calculateButton->setEnabled(false);

	m_currentSelectFeatureMode = LeftBrowLeftCornerFeature;
	m_featureCodeButtonMap[FeatureCodes[0]]->setChecked(true);

	for(int counter = 0; counter < NumFeatureCodes; ++counter)
	{
		m_featuresMap[FeatureCodes[counter]] = undefined;
		m_featureCodeButtonMap[FeatureCodes[counter]]->setText((m_featureCodeButtonText[FeatureCodes[counter]] + undefinedCoordinateText).c_str());
	}
  
	m_image->clearMarkers();
	paintCurrentModeText();
}
//-----------------------------------------------------------------------------
void MainWindow::clearMetaData()
{
	m_genderButtonGroup->setExclusive(false);
    m_femaleButton->setChecked(false); 
	m_maleButton->setChecked(false);
	m_genderButtonGroup->setExclusive(true);

	m_glassesButton->setChecked(false);
	m_occludedButton->setChecked(false);
	m_grayscaleButton->setChecked(false);

}
//-----------------------------------------------------------------------------
//void MainWindow::clearFineAdjustmentAngles()
//{
//   m_fineAdjustmentAngleRoll = 0.0;
//   m_fineAdjustmentAnglePitch = 0.0;
//   m_fineAdjustmentAngleYaw = 0.0;
//}
//-----------------------------------------------------------------------------
void MainWindow::clearAngles()
{
   m_currentRollAngle  = undefinedAngle;
   m_currentPitchAngle = undefinedAngle;
   m_currentYawAngle   = undefinedAngle;
   // reset the viewer
   m_viewer->applyTransformationToSceneGraph(0.0,0.0,0.0);
}
//-----------------------------------------------------------------------------
void MainWindow::initGUI()
{
	QMenuBar * menuBar = new QMenuBar;
	this->setMenuBar( menuBar );

	//*-- FILE MENU --*//
	m_fileMenu = new QMenu(tr("&File"), this);
	menuBar->addMenu(m_fileMenu);

	m_openDBAction = new QAction(tr("&Open Database"), this);
	m_openDBAction->setShortcut(tr("Ctrl+O"));
	m_openDBAction->setStatusTip(tr("Open an arbitrary image"));
	QObject::connect(m_openDBAction, SIGNAL(triggered()), 
					this, SLOT(openDatabase()));
	m_fileMenu->addAction(m_openDBAction);

	m_saveFeaturesAction = new QAction(tr("&Save"), this);
	m_saveFeaturesAction->setShortcut(tr("Ctrl+S"));
	m_saveFeaturesAction->setStatusTip(tr("Save current feature point set"));
	QObject::connect(m_saveFeaturesAction, SIGNAL(triggered()), 
		this, SLOT(save()));
	m_saveFeaturesAction->setEnabled(false);

	m_fileMenu->addAction(m_saveFeaturesAction);

	m_reindexUnlabeledImagesAction = new QAction(tr("&Reindex Unlabeled"), this);
	m_reindexUnlabeledImagesAction->setShortcut(tr("Ctrl+R"));
	m_reindexUnlabeledImagesAction->setStatusTip(tr("Reindex unlabeled images"));
	QObject::connect(m_reindexUnlabeledImagesAction, SIGNAL(triggered()), 
		this, SLOT(getUnlabeledImages()));
	QObject::connect(m_reindexUnlabeledImagesAction, SIGNAL(triggered()), 
		this, SLOT(updateProgressBar()));
	m_fileMenu->addAction(m_reindexUnlabeledImagesAction);
	m_reindexUnlabeledImagesAction->setEnabled(false);

	m_fileMenu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)  {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

	for (int i = 0; i < MaxRecentFiles; ++i)
        m_fileMenu->addAction(recentFileActs[i]);
	updateRecentFileActions();

	m_fileMenu->addSeparator();
   
	QAction * exitAction = m_fileMenu->addAction(tr("E&xit"));
	QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
	QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	//*-- VIEW MENU --*//
	QAction *zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	connect(zoomInAct, SIGNAL(triggered()), m_image, SLOT(zoomIn()));

	QAction *zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	connect(zoomOutAct, SIGNAL(triggered()), m_image, SLOT(zoomOut()));

	QAction *fitToWindowAct = new QAction(tr("Fit to &Window"), this);
	fitToWindowAct->setShortcut(tr("Ctrl+W"));
	connect(fitToWindowAct, SIGNAL(triggered()), m_image, SLOT(fitToWindow()));   

	QAction *fitToFaceAct = new QAction(tr("&Fit to Face"), this);
	fitToFaceAct->setShortcut(tr("Ctrl+F"));
	connect(fitToFaceAct, SIGNAL(triggered()), this, SLOT(faceAutoZoom()));   
   
	m_viewMenu = new QMenu(tr("&View"), this);
	m_viewMenu->addAction(zoomInAct);
	m_viewMenu->addAction(zoomOutAct);
	m_viewMenu->addSeparator();
	m_viewMenu->addAction(fitToWindowAct);
	m_viewMenu->addAction(fitToFaceAct);
	m_viewMenu->setEnabled(false);
	menuBar->addMenu(m_viewMenu);

	//*-- FIND MENU --*//
	QAction *findImageAct = new QAction(tr("&Quick Find File"), this);
	connect(findImageAct, SIGNAL(triggered()), this, SLOT(findImage()));   

	QAction *findFaceIdAct = new QAction(tr("&Quick Find Face"), this);
	connect(findFaceIdAct, SIGNAL(triggered()), this, SLOT(findFace()));   
	
	QAction *filterFacesAct = new QAction(tr("&Filter Faces"), this);
	connect(filterFacesAct, SIGNAL(triggered()), this, SLOT(filterFacesByQuery()));   

	m_findMenu = new QMenu(tr("&Filter"), this);
	m_findMenu->addAction(findImageAct);
	m_findMenu->addAction(findFaceIdAct);
	m_findMenu->addAction(filterFacesAct);

	m_findMenu->setEnabled(false);
	menuBar->addMenu(m_findMenu);

	//*-- TOOLS MENU --*//
	QAction *clearAct = new QAction(tr("Clear Features"), this);
	connect( clearAct, SIGNAL(triggered()), this, SLOT(clear()) );
	
	m_autosaveButton = new QAction(tr("Autosave"), this);
	m_autosaveButton->setCheckable(true);
	m_autosaveButton->setChecked(false);

	m_autozoomButton = new QAction(tr("Autozoom"), this);
	m_autozoomButton->setCheckable(true);
	m_autozoomButton->setChecked(false);

	m_only_current_marker = new QAction(tr("Show only current Marker shortcut"),this);
	m_only_current_marker->setCheckable(true);
	m_only_current_marker->setChecked(true);
	connect( m_only_current_marker, SIGNAL(triggered()), this, SLOT(updateMarkerImage()) );

	m_toolsMenu = new QMenu(tr("&Tools"), this);
	m_toolsMenu->addAction(m_autosaveButton);
	m_toolsMenu->addAction(m_autozoomButton);
	m_toolsMenu->addSeparator();
	m_toolsMenu->addAction(clearAct);
	m_toolsMenu->addAction(m_only_current_marker);
	m_toolsMenu->setEnabled(false);
	menuBar->addMenu(m_toolsMenu);

   m_centralWidget = new QWidget(this);
   setCentralWidget(m_centralWidget);

   QHBoxLayout * centralWidgetLayout = new QHBoxLayout();
   m_centralWidget->setLayout(centralWidgetLayout);

   QSplitter * m_viewerSplitterWidget = new QSplitter(Qt::Horizontal,m_centralWidget);  
   m_viewer = new PoseEstimationQuarterWidget(m_centralWidget);

   QWidget *buttonWidget = new QWidget(m_centralWidget);
   QVBoxLayout *v_layout = new QVBoxLayout(buttonWidget);
   buttonWidget->setLayout(v_layout);

   centralWidgetLayout->addWidget(buttonWidget,1);

   //-- NAVIGATION BUTTONS --//
   m_naviButtonGroupBox = new QGroupBox(tr("Directory Navigation"),buttonWidget);
   QVBoxLayout *v_layoutNaviButtonGroupBox  = new QVBoxLayout();  
   QHBoxLayout *h_layoutNaviButtonWidget = new QHBoxLayout();  

   m_current_image_filename = new QLabel(m_naviButtonGroupBox);
   //m_current_image_filename->setText("foobar");
  
   m_loadULImageButton = new QPushButton(QIcon("icons/label.png"),"",m_naviButtonGroupBox); //"Load Next UL Img"
   connect( m_loadULImageButton, SIGNAL(pressed()), this, SLOT(gotoNextUnlabeledImage()) );
   m_loadULImageButton->setEnabled(false);
   
   m_loadImageButton = new QPushButton(QIcon("icons/next.png"),"",m_naviButtonGroupBox); //Load Next Img
   connect( m_loadImageButton, SIGNAL(pressed()), this, SLOT(loadNextImage()) );
   m_loadImageButton->setEnabled(false);

   m_loadPrevImageButton = new QPushButton(QIcon("icons/back.png"),"",m_naviButtonGroupBox); //Load Prev. Img
   connect( m_loadPrevImageButton, SIGNAL(pressed()), this, SLOT(loadPreviousImage()) );
   m_loadPrevImageButton->setEnabled(false);
   
   h_layoutNaviButtonWidget->addWidget(m_loadPrevImageButton,1);
   h_layoutNaviButtonWidget->addWidget(m_loadImageButton,1);
   h_layoutNaviButtonWidget->addWidget(m_loadULImageButton,1); 

   m_current_folder_index = new QLineEdit(m_naviButtonGroupBox);
   m_current_folder_index->setText("");
   m_current_folder_index->setMaximumWidth(m_loadULImageButton->width());
   m_current_folder_index->setMaximumHeight(m_loadULImageButton->height());
   m_current_folder_index->setEnabled(false);

   QRegExp rx("^\\d*\\s/\\s\\d+$");
   QValidator *validator = new QRegExpValidator(rx, this);
   m_current_folder_index->setValidator(validator);

   connect( m_current_folder_index, SIGNAL(returnPressed()), this, SLOT(parseDirectoryIndexTextBox()) );

   h_layoutNaviButtonWidget->addWidget(m_current_folder_index);

   m_current_folder_progress = new QProgressBar(m_naviButtonGroupBox);
   updateProgressBar();
   
   v_layoutNaviButtonGroupBox->addWidget(m_current_image_filename);
   v_layoutNaviButtonGroupBox->addLayout(h_layoutNaviButtonWidget);

   v_layoutNaviButtonGroupBox->addWidget(m_current_folder_progress);
   
   m_naviButtonGroupBox->setLayout(v_layoutNaviButtonGroupBox);
   v_layout->addWidget(m_naviButtonGroupBox);

   //-- END NAVIGATION BUTTONS --//

	//-- FACE NAVI BUTTONS --//
	m_naviFaceButtonGroupBox = new QGroupBox(tr("Face Navigation"),buttonWidget);
	QHBoxLayout *h_layoutFaceButtonWidget = new QHBoxLayout();  

	m_nextFaceButton = new QPushButton(QIcon("icons/next.png"),"",m_naviFaceButtonGroupBox);
	connect( m_nextFaceButton, SIGNAL(pressed()), this, SLOT(nextAnnotation()) );
	m_nextFaceButton->setEnabled(false);

	m_prevFaceButton = new QPushButton(QIcon("icons/back.png"),"",m_naviFaceButtonGroupBox);
	connect( m_prevFaceButton, SIGNAL(pressed()), this, SLOT(prevAnnotation()) );
	m_prevFaceButton->setEnabled(false);

    m_newFaceButton = new QPushButton(QIcon("icons/new.png"),"",m_naviFaceButtonGroupBox);
	connect( m_newFaceButton, SIGNAL(pressed()), this, SLOT(newAnnotation()) );
	m_newFaceButton->setEnabled(false);

    m_delFaceButton = new QPushButton(QIcon("icons/delete.png"),"",m_naviFaceButtonGroupBox);
	connect( m_delFaceButton, SIGNAL(pressed()), this, SLOT(delAnnotation()) );
	m_delFaceButton->setEnabled(false); 


	h_layoutFaceButtonWidget->addWidget(m_prevFaceButton,1);
    h_layoutFaceButtonWidget->addWidget(m_nextFaceButton,1);
	h_layoutFaceButtonWidget->addWidget(m_newFaceButton,1);
	h_layoutFaceButtonWidget->addWidget(m_delFaceButton,1);
   
	m_current_face_index = new QLineEdit(m_naviFaceButtonGroupBox);
	m_current_face_index->setText("");
	m_current_face_index->setMaximumWidth(m_loadULImageButton->width());
	m_current_face_index->setMaximumHeight(m_loadULImageButton->height());
	m_current_face_index->setEnabled(false);

	m_current_face_index->setValidator(validator);
	connect(m_current_face_index, SIGNAL(returnPressed()), this, SLOT(parseFaceIndexTextBox()) );

	h_layoutFaceButtonWidget->addWidget(m_current_face_index);
	
	m_currentFaceIdLabel = new QLabel(m_naviFaceButtonGroupBox);
	h_layoutFaceButtonWidget->addWidget(m_currentFaceIdLabel);

	m_naviFaceButtonGroupBox->setLayout(h_layoutFaceButtonWidget);
	v_layout->addWidget(m_naviFaceButtonGroupBox);
	//-- END FACE NAVI BUTTONS --//

   m_buttonGroupBox = new QGroupBox(tr("Head Parts"),buttonWidget);
   m_buttonGroupBox->setFocusPolicy(Qt::NoFocus);
   m_buttonGroupBox->setCheckable(true);
   m_buttonGroupBox->setChecked(false);

   connect(m_buttonGroupBox, SIGNAL(toggled(bool)), this, SLOT(enableFeaturePointButtons(bool)));  

   v_layout->addWidget(m_buttonGroupBox,6);

   QVBoxLayout *vbox = new QVBoxLayout(m_buttonGroupBox);

    // the buttons for the different parts
    QRadioButton *button = 0;
 
	for(int counter = 0; counter < NumFeatureCodes; ++counter)
	{
		FeaturePointRadioButton *fprbutton = new FeaturePointRadioButton(m_featureCodeSelectModeMap[FeatureCodes[counter]],(m_featureCodeButtonText[FeatureCodes[counter]] + undefinedCoordinateText).c_str(),m_buttonGroupBox);
		fprbutton->setMinimumHeight(fprbutton->sizeHint().height()/2.0);
		m_featureCodeButtonMap[FeatureCodes[counter]] = fprbutton;
		vbox->addWidget(fprbutton,4);
		connect(fprbutton, SIGNAL(toggled(MainWindowSelectFeatureMode,bool)), this, SLOT(toggleFeatureButton(MainWindowSelectFeatureMode,bool)));  
	}

	m_currentSelectFeatureMode = LeftBrowLeftCornerFeature;
    m_featureCodeButtonMap[FeatureCodes[LeftBrowLeftCornerFeature]]->setChecked(true);
	m_focusButton = m_featureCodeButtonMap[FeatureCodes[LeftBrowLeftCornerFeature]];
	m_focusButton->setFocus();

	m_buttonGroupBox->setEnabled(false);
	m_buttonGroupBox->setLayout(vbox);

   //-- META DATA GROUP BOX --//
   m_metaDataGroupBox = new QGroupBox(tr("Meta Data"),buttonWidget);
   m_metaDataGroupBox->setFocusPolicy(Qt::NoFocus);
   m_metaDataGroupBox->setCheckable(true);
   m_metaDataGroupBox->setChecked(false);
   connect(m_metaDataGroupBox, SIGNAL(toggled(bool)), this, SLOT(enableFeaturePointButtons(bool)));  
   v_layout->addWidget(m_metaDataGroupBox,6);
   
   QVBoxLayout *vboxLayoutMetaData = new QVBoxLayout(m_metaDataGroupBox);
   vboxLayoutMetaData->setSizeConstraint(QLayout::SetMinAndMaxSize);

   m_genderButtonGroup = new QButtonGroup(m_metaDataGroupBox);

   m_femaleButton = new QRadioButton("&female", m_metaDataGroupBox);
   m_maleButton = new QRadioButton("&male", m_metaDataGroupBox);
   
   m_genderButtonGroup->addButton(m_maleButton);
   m_genderButtonGroup->addButton(m_femaleButton);

   vboxLayoutMetaData->addWidget(m_maleButton);
   vboxLayoutMetaData->addWidget(m_femaleButton);

   m_glassesButton = new QCheckBox ("glasses", m_metaDataGroupBox );
   vboxLayoutMetaData->addWidget(m_glassesButton);

   m_occludedButton = new QCheckBox ("occluded / artistic / painted", m_metaDataGroupBox );
   vboxLayoutMetaData->addWidget(m_occludedButton);

   m_grayscaleButton = new QCheckBox ("b/w", m_metaDataGroupBox );
   
   vboxLayoutMetaData->addWidget(m_grayscaleButton);

   m_metaDataGroupBox->setEnabled(false);
   m_metaDataGroupBox->setMaximumSize(vboxLayoutMetaData->minimumSize());

   connect( m_femaleButton, SIGNAL(clicked()), this, SLOT(touchFeatures()) );
   connect( m_maleButton, SIGNAL(clicked()), this, SLOT(touchFeatures()) );
   connect( m_glassesButton, SIGNAL(clicked()), this, SLOT(touchFeatures()) );
   connect( m_occludedButton, SIGNAL(clicked()), this, SLOT(touchFeatures()) );
   connect( m_grayscaleButton, SIGNAL(clicked()), this, SLOT(touchFeatures()) );

   //-- END OF META DATA GROUP BOX --//

   m_calculateButton = new QPushButton("Calculate Pose",buttonWidget);
   connect( m_calculateButton, SIGNAL(pressed()), this, SLOT(calculatePose()) );
   
   v_layout->addWidget(m_calculateButton,1);
   m_calculateButton->setEnabled(false);
  
   QWidget *centralWidget = new QWidget(this);
   QVBoxLayout *vLayoutCentralWidget = new QVBoxLayout();

   vLayoutCentralWidget->addWidget(m_image);  
   
   m_statusBar = new QLabel(centralWidget);
   m_statusBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
   vLayoutCentralWidget->addWidget(m_statusBar);
   m_statusBar->setVisible(false);


   centralWidget->setLayout(vLayoutCentralWidget);

   m_viewerSplitterWidget->addWidget(centralWidget);

   m_viewerSplitterWidget->addWidget(m_viewer);

   m_viewerSplitterWidget->setStretchFactor(0,12);
   m_viewerSplitterWidget->setStretchFactor(1,1);

   centralWidgetLayout->addWidget(m_viewerSplitterWidget,4);
}
//-----------------------------------------------------------------------------
void MainWindow::quit()
{
   // first, if autosave is enabled, we save the last feature results
   save();
   qApp->quit();
}
//-----------------------------------------------------------------------------
void MainWindow::toggleFeatureButton(FeatureCode featCode, bool checked)
{
   MainWindowSelectFeatureMode tmpFeatCode =	m_featureCodeSelectModeMap[featCode];
   if (checked && m_currentSelectFeatureMode != tmpFeatCode)
   {
      m_currentSelectFeatureMode = tmpFeatCode;

      updateMarkerImage();
   }
}
//-----------------------------------------------------------------------------
void MainWindow::toggleFeatureButton(MainWindowSelectFeatureMode featCode, bool checked)
{
	
   if (checked && m_currentSelectFeatureMode != featCode)
   {
      m_currentSelectFeatureMode = featCode;

      updateMarkerImage();
   }
}
//-----------------------------------------------------------------------------
void MainWindow::changeCurrentDisplayMode()
{
   qDebug() << "change current display mode!";
   m_currentDisplayMode = static_cast<MainWindowDisplayMode>(
      (static_cast<int>(m_currentDisplayMode) + 1) % MAIN_WINDOW_NR_DIFFERENT_DISPLAY_MODES );
   updateMarkerImage();
}
//-----------------------------------------------------------------------------
void MainWindow::keyPressEvent ( QKeyEvent * event )
{
   const double angleIncrementRad = 1. * M_PI / 180.;
 
	//m_textShorts
   qDebug() << "Key: " << event->key();

   const int key = event->key();
   switch(key)
   {
   case	Qt::Key_Tab:
	   qDebug() << "pressed tab";
	   break;
   case Qt::Key_Space:
      changeCurrentDisplayMode();
      break;
   case Qt::Key_Delete:
	  qDebug() << "removing current feature point";
      removeCurrentSelectedFeaturePoint();
      break;
   //case Qt::Key_PageUp:
   //   std::cout << "page up" << std::endl;
   //   m_fineAdjustmentAngleRoll += angleIncrementRad;
   //   break;
   //case Qt::Key_PageDown:
   //   std::cout << "page down" << std::endl;
   //   m_fineAdjustmentAngleRoll -= angleIncrementRad;
   //   break;
   //case Qt::Key_Left:
   //   //std::cout << "W" << std::endl;
   //   m_fineAdjustmentAngleYaw += angleIncrementRad;
   //   break;
   //case Qt::Key_Right:
   //   //std::cout << "S" << std::endl;
   //   m_fineAdjustmentAngleYaw -= angleIncrementRad;
   //   break;
   //case Qt::Key_Up:
   //   //std::cout << "Q" << std::endl;
   //   m_fineAdjustmentAnglePitch += angleIncrementRad;
   //   break;
   //case Qt::Key_Down:
   //   //std::cout << "E" << std::endl;
   //   m_fineAdjustmentAnglePitch -= angleIncrementRad;
   //   break;
   case Qt::Key_Enter:
      qDebug() << "Enter";
      break;
   case Qt::Key_Return:
      qDebug() << "Return";
      break;
   }

   // call the parent to handle the rest
   QMainWindow::keyPressEvent(event);
}
//-----------------------------------------------------------------------------
bool MainWindow::calculatePose()
{
	ModernPosit modernPosit;
	cv::Mat rot;
	cv::Point3f trans;

	FaceData *faceData = m_annotations.at(m_curr_annotation);

	QSize imgSize = m_image->getImageSize();

	double focalLength = static_cast<double>(imgSize.width()) * 1.5;
	cv::Point2f imgCenter = cv::Point2f(static_cast<float>(imgSize.width()) / 2.0f, static_cast<float>(imgSize.height()) / 2.0f);
	
	// prepare image pts and world pts
	FeaturesCoords *fc = faceData->getFeaturesCoords();
	std::vector<int> availableFeatureIds = fc->getFeatureIds();

	if (availableFeatureIds.size() < 2)
		return false;

	std::vector<cv::Point2f> imagePts;
	std::vector<cv::Point3f> worldPts;
	for (unsigned int i = 0; i < availableFeatureIds.size(); ++i)
	{
		int featureID = availableFeatureIds[i];
		cv::Point2f imgPt = fc->getCoords(featureID);
		if ((imgPt.x >= 0) && (imgPt.y >= 0))
		{
			imagePts.push_back(imgPt);
			std::string featureName = m_fcTypes.getCode(featureID);
			cv::Point3f worldPt = m_meanFace3DModel.getCoordsByCode(featureName);

			// adjust the world and model points to move them from opengl to pitch yaw roll coordinate system
			cv::Point3f worldPtOgl;	
			worldPtOgl.x = worldPt.z;
			worldPtOgl.y = -worldPt.x;
			worldPtOgl.z = -worldPt.y;

			worldPts.push_back(worldPtOgl);
		}
	}
	 
	modernPosit.run(rot,trans,imagePts,worldPts,focalLength,imgCenter);
	
	m_tfm = (cv::Mat_<float>(4,4) << 0,0,0,trans.x, 0,0,0,trans.y, 0,0,0,trans.z, 0,0,0,1);
	for(int i=0;i < 3; ++i)
		for(int j=0;j < 3; ++j)
			m_tfm.at<float>(i,j) = rot.at<float>(i,j);

	//std::cout << m_tfm << std::endl;
	
	// the transformation tfm is a combination of the rotation that links the model coordinate system (roll,pitch,yaw)
	// and the coordinate system that is used in the POSIT algorithm where the z axis is perpendicular to the image
	// plane pointing positively into the image -> we get tfm as the rotation transforming the model into this POSIT
	// coord system so we use rotateCoordSystem to split tfm! (rotation matrix -> inverse == transpose)
	cv::Mat rotateCoordSystemM = (cv::Mat_<float>(4, 4) << 0,0,-1,0, -1,0,0,0, 0,1,0,0, 0,0,0,1);
	cv::Mat tmp = rotateCoordSystemM * m_tfm;

	splitRotationMatrixToRollPitchYaw(tmp, m_currentRollAngle, m_currentPitchAngle, m_currentYawAngle);
	m_viewer->applyTransformationToSceneGraph(m_currentRollAngle,m_currentPitchAngle,m_currentYawAngle);
	  
	m_currentDisplayMode = TransformedFeaturePointsOnly;
	updateMarkerImage();
	return true;
}
//-----------------------------------------------------------------------------
/*
void MainWindow::calculateRotationMatrixFromRollPitchYaw(cv::Mat &matrix, double roll, double pitch, double yaw)
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

	std::cout << "MainWindow::calculateRotationMatrixFromRollPitchYaw" << std::endl;

   float data[] = {cosGamma*cosBeta, cosGamma*sinBeta*sinAlpha - sinGamma*cosAlpha, cosGamma*sinBeta*cosAlpha + sinGamma*sinAlpha, 0.0, 
	               sinGamma*cosBeta, sinGamma*sinBeta*sinAlpha + cosGamma*cosAlpha, sinGamma*sinBeta*cosAlpha - cosGamma*sinAlpha, 0.0,
	               -sinBeta, cosBeta*sinAlpha, cosBeta*cosAlpha, 0.0, 
	               0.0, 0.0, 0.0, 1.0};
   matrix = cv::Mat(4,4,CV_32F,data);
}
*/
//-----------------------------------------------------------------------------
void MainWindow::splitRotationMatrixToRollPitchYaw(cv::Mat &rot_matrix, double& roll, double& pitch, double& yaw)
{
	   // do we have to transpose here?
   const double a11 = rot_matrix.at<float>(0,0), a12 = rot_matrix.at<float>(0,1), a13 = rot_matrix.at<float>(0,2);
   const double a21 = rot_matrix.at<float>(1,0), a22 = rot_matrix.at<float>(1,1), a23 = rot_matrix.at<float>(1,2);
   const double a31 = rot_matrix.at<float>(2,0), a32 = rot_matrix.at<float>(2,1), a33 = rot_matrix.at<float>(2,2);

   //replaces vnl_math header
   const double epsilon = 2.2204460492503131e-16;
   double pi = 3.14159265358979323846;
   double pi_over_2 = 1.57079632679489661923;
   double pi_over_4 = 0.78539816339744830962;
   

   if ( abs(1.0 - a31) <= epsilon ) // special case a31 == +1
   {
      qDebug() << "gimbal lock case a31 == " << a31;
      pitch = -pi_over_2;
      yaw   = pi_over_4; // arbitrary value
      roll  = atan2(a12,a13) - yaw;
   }
   else if ( abs(-1.0 - a31) <= epsilon ) // special case a31 == -1
   {
      qDebug() << "gimbal lock case a31 == ";
      pitch = pi_over_2;
      yaw   = pi_over_4; // arbitrary value
      roll  = atan2(a12,a13) + yaw;
   }
   else // standard case a31 != +/-1
   {
      pitch = asin(-a31);
      //two cases depending on where pitch angle lies
      if ( (pitch < pi_over_2) && (pitch > -pi_over_2) )
      {
         roll = atan2(a32,a33);
         yaw  = atan2(a21,a11);
      }
      else if ( (pitch < 3.0 * pi_over_2) && (pitch > pi_over_2) )
      {
         roll = atan2(-a32,-a33);
         yaw  = atan2(-a21,-a11);
      }
      else
      {
         std::cerr << "this should never happen in pitch roll yaw computation!" << std::endl;
         roll = 2.0 * pi;
         yaw  = 2.0 * pi;
      }
   }
}
//-----------------------------------------------------------------------------
void MainWindow::removeCurrentSelectedFeaturePoint()
{
	//*-- UPDATE DATABASE OBJECT --*//
   touchFeatures();
	
	if(m_annotations.size()>0)
   {
	FeaturesCoords *fc  = m_annotations.at(m_curr_annotation)->getFeaturesCoords();	
	if(fc)
	{
		fc->setCoords(m_featureCodeDBIdMap[FeatureCodes[m_currentSelectFeatureMode]],cv::Point2f(undefined[0],undefined[1]));
	}
   }

   if (m_featuresMap[FeatureCodes[m_currentSelectFeatureMode]][0] != -1)
   {
	   --m_numberOfSelectedFeatures;
		m_featuresMap[FeatureCodes[m_currentSelectFeatureMode]] = undefined;
		m_featureCodeButtonMap[FeatureCodes[m_currentSelectFeatureMode]]->setText((m_featureCodeButtonText[FeatureCodes[m_currentSelectFeatureMode]] + undefinedCoordinateText).c_str());	
   }

    updateMarkerImage();

   // check if we can enable the button to calculate the pose model
   if (m_numberOfSelectedFeatures >= 4)
   {
      // enable calc button
      m_calculateButton->setEnabled(true);
   }
   else
   {
      // disable calc button
      m_calculateButton->setEnabled(false);
   }
}
//-----------------------------------------------------------------------------
void MainWindow::on_mousePress( QGraphicsSceneMouseEvent *event )
{	
   Qt::MouseButton mouseButton = event->button();
   QPointF pt = event->pos();

   if (mouseButton == Qt::LeftButton)
   {    
		touchFeatures();

		//*-- UPDATE DATABASE OBJECT --*//
		if(m_annotations.size() > 0 )
		{
			FeaturesCoords *fc  = m_annotations.at(m_curr_annotation)->getFeaturesCoords();	
			fc->setCoords(m_featureCodeDBIdMap[FeatureCodes[m_currentSelectFeatureMode]],cv::Point2f(pt.x(),pt.y()));
		}

		if (m_featuresMap[FeatureCodes[m_currentSelectFeatureMode]][0] == -1)
			++m_numberOfSelectedFeatures;

		m_featuresMap[FeatureCodes[m_currentSelectFeatureMode]] = SbVec2d((double)pt.x(), (double)pt.y());
		m_featureCodeButtonMap[FeatureCodes[m_currentSelectFeatureMode]]->setText(QString(m_featureCodeButtonText[FeatureCodes[m_currentSelectFeatureMode]].c_str()) + QString("%1").arg(pt.x()) + QString(";") + QString("%1").arg(pt.y()));	

		updateMarkerImage();

		// check if we can enable the button to calculate the pose model
		if (m_numberOfSelectedFeatures >= 4)
		{
			// enable calc button
			m_calculateButton->setEnabled(true);
		}     
   }

   m_featureCodeButtonMap[FeatureCodes[m_currentSelectFeatureMode]]->setFocus();
}
//-----------------------------------------------------------------------------
void MainWindow::prevAnnotation()
{
    save();
	
    --m_curr_annotation;

	if (m_curr_annotation < 0)
	{
	  QMessageBox msgBox;
	  msgBox.setText("First Face!");
	  msgBox.exec();
	  m_curr_annotation = 0;
	}

	displayCurrentAnnotation();
	updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
void MainWindow::nextAnnotation(int annoIdx)
{
    save();

	if(annoIdx != -1)
		m_curr_annotation = annoIdx;
	else
	  ++m_curr_annotation;

	if (m_curr_annotation == m_annotations.size())
	{
	  QMessageBox msgBox;
	  msgBox.setText("Last Face!");
	  msgBox.exec();
	  m_curr_annotation = m_annotations.size()-1;
	}

	if (m_curr_annotation >= m_annotations.size() || m_curr_annotation < 0)
	{
	  QMessageBox msgBox;
	  msgBox.setText("Index out of range!");
	  msgBox.exec();
	  m_curr_annotation = 0;
	  return;
	}

	displayCurrentAnnotation();
	updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
void MainWindow::newAnnotation()
{
   save();

   FaceData *newFace = new FaceData();
   
   newFace->dbID = m_img_db_id;
   QFileInfo tmpInfo(m_currentFileList.at(m_currentFileIndex));
   newFace->fileID = std::string(tmpInfo.fileName().toLocal8Bit());
   
   FaceDbImage *newFaceDbImage = new FaceDbImage();

   newFaceDbImage->db_id = m_img_db_id;
   newFaceDbImage->file_id = std::string(m_currentFileList.at(m_currentFileIndex).toLocal8Bit());
   newFaceDbImage->filepath = std::string(m_dbBaseDir.relativeFilePath(m_imgDir.absoluteFilePath(m_currentFileList.at(m_currentFileIndex))).toLocal8Bit());
   
   QSize imgSize = m_image->getImageSize();

   newFaceDbImage->width = imgSize.width();
   newFaceDbImage->height = imgSize.height();

   FaceMetadata *newFaceMetadata = new FaceMetadata(-1);
   FacePose *newFacePose = new FacePose();
   FeaturesCoords *newFeaturesCoords = new FeaturesCoords(-1);

   newFace->setDbImg(newFaceDbImage);
   newFace->setMetadata(newFaceMetadata);
   newFace->setPose(newFacePose);
   newFace->setFeatureCoords(newFeaturesCoords);
  
   m_annotations.push_back(newFace);
   m_curr_annotation = m_annotations.size()-1;

   displayCurrentAnnotation();
   updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
void MainWindow::delAnnotation()
{
	assert(m_sqlConn);

	if(!m_autosaveButton->isChecked())
	{
		QMessageBox msgBox;
	    msgBox.setText("Autosave is disabled");
		msgBox.setInformativeText("Do you really want to delete annotation?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();

		if(ret == QMessageBox::Discard)
		{
			 return;
		}
	}

	DeleteFacesByIdsQuery delFacesQuery;
	delFacesQuery.face_ids.push_back(m_annotations.at(m_curr_annotation)->ID);

	if(!delFacesQuery.exec(m_sqlConn))
	{
		QMessageBox::warning(this, tr("ERROR"), tr("could not delete annotation!"), QMessageBox::Ok);	
		return;
	}

	FaceData *tmpFace = m_annotations.at(m_curr_annotation);
	m_annotations.erase(m_annotations.begin() + m_curr_annotation);
	delete tmpFace;

	if(m_curr_annotation >= m_annotations.size())
	{
		m_curr_annotation = m_annotations.size() - 1;
	}

    displayCurrentAnnotation();
	updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
void MainWindow::faceAutoZoom()
{
	FaceData *fd = m_annotations.at(m_curr_annotation);
	FacePose *fp = fd->getPose();

	if(fp->yaw != Pose::UNDEFINED_ANGLE && fp->pitch != Pose::UNDEFINED_ANGLE && fp->roll != Pose::UNDEFINED_ANGLE)
	{
		FaceRectCalculator tmpRectCalc;
		cv::Rect faceRect = tmpRectCalc.calcSingleRect(fd,m_sqlConn);
		QRectF qRect(faceRect.x,faceRect.y,faceRect.width,faceRect.height);
		m_image->zoomOnFace(qRect);
	}
}
//-----------------------------------------------------------------------------
void MainWindow::updateMarkerImage()
{
	CvPoint p;
	m_image->clearMarkers();

    for(int counter = 0; counter < NumFeatureCodes; ++counter)
	{
		SbVec2d featureCoord = m_featuresMap[FeatureCodes[counter]];
   		if (featureCoord[0] != -1)
		{
			p.x = featureCoord[0];
			p.y = featureCoord[1];

			QPointF pt = QPointF(p.x,p.y);

 			if(!m_only_current_marker->isChecked() || m_currentSelectFeatureMode == counter)
			{			 
			  m_image->setMarker(counter,pt, QColor(m_textColor[FeatureCodes[counter]][2], m_textColor[FeatureCodes[counter]][1], m_textColor[FeatureCodes[counter]][0],125),QString::fromLocal8Bit(m_textShorts[FeatureCodes[counter]].c_str()));
			}
			else
			{
			  m_image->setMarker(counter,pt, QColor(m_textColor[FeatureCodes[counter]][2], m_textColor[FeatureCodes[counter]][1], m_textColor[FeatureCodes[counter]][0],125),QString(""));
			}
		}
   }

   if(m_autozoomButton->isChecked())
   {
	  faceAutoZoom();
   }

   paintCurrentModeText();

   QSize imgSize = m_image->getImageSize();
   const double center_x = 0.5 * imgSize.width();
   const double center_y = 0.5 * imgSize.height();
   const double distanceToImagePlane = focusFactor * imgSize.width();

   // also paint the current model that should be displayed
   switch(m_currentDisplayMode)
   {
	   case NoModel:
	       break;
	   case TransformedFeaturePointsOnly:
	   {
		   //std::cout << "TransformedFeaturePointsOnly" << m_tfm << std::endl;

		   //Project the model points with the estimated pose
		   std::vector<cv::Point2f> projectedPoints;
		   for(int counter = 0; counter < NumFeatureCodes; ++counter)
		   {
				cv::Point3f pt3d = m_meanFace3DModel.getCoordsByCode(FeatureCodes[counter]);
				cv::Mat mPt3d = (cv::Mat_<float>(4,1) << pt3d.z, -pt3d.x, -pt3d.y, 1); //OGL coords...
				mPt3d = m_tfm * mPt3d; //rotat and translate model in 3D
			
				if ( mPt3d.at<float>(2,0) != 0 )
				{
					cv::Point2f pt2d;
					pt2d.x = distanceToImagePlane * mPt3d.at<float>(0,0) / mPt3d.at<float>(2,0); //project to 2D
					pt2d.y = distanceToImagePlane * mPt3d.at<float>(1,0) / mPt3d.at<float>(2,0);
					QPointF pf = QPointF(center_x + pt2d.x,center_y +pt2d.y);
					QColor c = QColor(0,0,255);
					m_image->setBackprojection(counter,pf,c,""); //translate 2d
				}
		   }
		   break;
	   }
	   case TransformedModel:
	   {
		    //std::cout << "TransformedModel" << m_tfm << std::endl;

			SoCoordinate3 * fullModelCoords = m_viewer->getModelCoords();
			if (fullModelCoords == 0)
			{
				std::cerr << "couldn't get the model coordinates!" << std::endl;
				break;
			}
			else
			{
			   const SbVec3f * points = fullModelCoords->point.getValues(0);
			   for (int i=0; i<fullModelCoords->point.getNum(); i += 5)
			   {
				   cv::Mat mPt3d = (cv::Mat_<float>(4,1) << points[i][2],-points[i][0],-points[i][1],1);
				   mPt3d = m_tfm * mPt3d;

				   if ( mPt3d.at<float>(2,0) != 0 )
				   {
						cv::Point2f pt2d;
						pt2d.x = distanceToImagePlane * mPt3d.at<float>(0,0) / mPt3d.at<float>(2,0);
						pt2d.y = distanceToImagePlane * mPt3d.at<float>(1,0) / mPt3d.at<float>(2,0);
						QPointF pf = QPointF(center_x + pt2d.x,center_y +pt2d.y);
						QColor c = QColor(255,255,0,50);
						m_image->setMeshPoint(i,pf,c);
					}
			   }
			}
			break;
	   }
	   default:
	   {
		  qDebug() << "switch current display mode: this should never happen!";
		  break;
	   }
   }
}
//-----------------------------------------------------------------------------
void MainWindow::paintCurrentModeText()
{
   m_image->setModeText(QString::fromLocal8Bit(m_featureCodeButtonText[FeatureCodes[m_currentSelectFeatureMode]].c_str()),QColor(m_textColor[FeatureCodes[m_currentSelectFeatureMode]][2], m_textColor[FeatureCodes[m_currentSelectFeatureMode]][1], m_textColor[FeatureCodes[m_currentSelectFeatureMode]][0],125));
}
//-----------------------------------------------------------------------------
void MainWindow::openDatabase(QString dbFilename)
{
	if(dbFilename.isEmpty())
		dbFilename = QFileDialog::getOpenFileName(
		this,
		"Choose a database file",
		QDir::current().path(),
		"Sqlite3 db files (*.db *.sqlite)");

	qDebug() << "Database: " << dbFilename;

	if (dbFilename.isEmpty())
		return;

	if (m_sqlDbOpen)
	{
		m_sqlConn->close();
	}

	//*-- open sqlite3 database --*//
	QFileInfo fileInfo(dbFilename);
	m_sqlDbOpen = m_sqlConn->open(qPrintable(dbFilename));
	if (!m_sqlDbOpen)
	{
		qDebug() << "ERROR: couldn't open sqlite database '" << dbFilename << "'";
		QMessageBox::warning(this, tr("ERROR"), tr("couldn't open sqlite database."), QMessageBox::Ok);
		return;
	}

	setCurrentFile(dbFilename);

	//*-- check available dbs --*//
	FaceDBQuery dbQuery;
	dbQuery.exec(m_sqlConn);

	if(dbQuery.dbs.empty())
	{
		qDebug() << "ERROR: no image database found in db '" << dbFilename << "'";
		QMessageBox::warning(this, tr("ERROR"), tr("no image database found in db."), QMessageBox::Ok);
		return;
	}

	QString db_sub_dir;
	if(dbQuery.dbs.size()==1)
	{
		qDebug() << "ImageDatabase: " << dbQuery.dbs.at(0).path.c_str() << "'";
		db_sub_dir = dbQuery.dbs.at(0).path.c_str();
		m_img_db_id = dbQuery.dbs.at(0).db_id;
		m_img_db_descr = dbQuery.dbs.at(0).description;
	}
	else
	{ 
		// dialog to choose db if there is more than one
		QStringList items;
		for(int i = 0; i < dbQuery.dbs.size(); ++i)
			items.push_back(dbQuery.dbs.at(i).db_id.c_str());	

		bool ok;
		QString text = QInputDialog::getItem(this, tr("Choose image database"),
                                          tr("Database:"), items,0,false,&ok);
		if (ok && !text.isEmpty())
		{
			db_sub_dir = dbQuery.dbs.at(items.indexOf(text)).path.c_str();
			m_img_db_descr = dbQuery.dbs.at(items.indexOf(text)).description;
			m_img_db_id = qPrintable(text);
		}
		else
		{
			return;
		}
	}

	m_dbBaseDir = fileInfo.dir();
	qDebug() << fileInfo.baseName();

	//*-- change to subdir of image db --*//
	if (!m_dbBaseDir.cd(db_sub_dir))
	{
			qDebug() << "ERROR: couldn't change to subdir database '" << m_dbBaseDir.path() << "'";
			QMessageBox::warning(this, tr("ERROR"), tr("couldn't open db directory."), QMessageBox::Ok);

			m_sqlDbOpen = false;
			m_sqlConn->close();
			return;
	}

	//*-- check if there are subdirs --*//
	QStringList subdirs = m_dbBaseDir.entryList(QDir::AllDirs);
	
	if(subdirs.size()==0)
	{
		m_imgDir = m_dbBaseDir;
	}
	else
	{
		QString imgDirStr = QFileDialog::getExistingDirectory(this, tr("Choose image directory"),m_dbBaseDir.absolutePath(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
		m_imgDir = QDir(imgDirStr);

		//*-- check if the specified image directory is a sub-directory of the database base dir --*//
		QDir tmpDir = m_imgDir;
		bool subdir = false;

		if(m_dbBaseDir==m_imgDir)
		{
			subdir = true;
		}
		else
		{
			while(tmpDir.cdUp() && tmpDir.exists())
			{
				if(tmpDir==m_dbBaseDir)
				{
					subdir = true;
					break;
				}
			}
		}

		if(!subdir)
		{
			QMessageBox::warning(this, tr("ERROR"), tr("image directory hast to be a sub-directory of the base directory!"), QMessageBox::Ok);
			enable(false);
			m_openDBAction->setEnabled(true);
			return;
		}
	}

	//*-- load and bind feature types to interal ones --*//
	loadFeatureTypes();
	enable(true);

	// load mean face 3d model data
	m_meanFace3DModel.load(m_sqlConn);

	openDirectoryInternal();
}
//-----------------------------------------------------------------------------
void MainWindow::enable(bool enable)
{
	toggleNaviButtons(enable);
	//m_naviButtonGroupBox->setEnabled(enable);
	//m_naviFaceButtonGroupBox->setEnabled(enable);

	m_fileMenu->setEnabled(enable);
	m_viewMenu->setEnabled(enable);
	m_findMenu->setEnabled(enable);
	m_toolsMenu->setEnabled(enable);
	
	m_metaDataGroupBox->setEnabled(enable);
	m_buttonGroupBox->setEnabled(enable);

	m_reindexUnlabeledImagesAction->setEnabled(enable);
    m_saveFeaturesAction->setEnabled(enable);

	m_statusBar->setVisible(enable);

	//if(enable)
	//	m_openDBAction->setEnabled(false);
}
//-----------------------------------------------------------------------------
void MainWindow::toggleNaviButtons(bool enable)
{
	m_loadImageButton->setEnabled(enable);
	m_loadPrevImageButton->setEnabled(enable);
	m_loadULImageButton->setEnabled(enable);
	m_current_folder_index->setEnabled(enable);

	m_nextFaceButton->setEnabled(enable);
    m_prevFaceButton->setEnabled(enable);
	m_current_face_index->setEnabled(enable);
	m_newFaceButton->setEnabled(enable);
    m_delFaceButton->setEnabled(enable);
}
//-----------------------------------------------------------------------------
void MainWindow::openDirectoryInternal()
{
	this->setCursor(Qt::WaitCursor);
	m_currentFileList.clear();
	#ifdef WIN32 //QT QDir::entryList() is terrible slow under windows thus ...
		utils::ListDirectoryContents(qPrintable(m_imgDir.path()),m_currentFileList);
		m_currentFileList = m_currentFileList.filter(QRegExp(".jpg$|.png$|.pgm$",Qt::CaseInsensitive));
	#else
		//The filter should be case sensitive if the file system is case sensitive!
		QStringList filters;
		filters << "*.jpg" << "*.JPG" << "*.png" << "*.PNG" << "*.pgm" << "*.PGM";
		m_imgDir.setNameFilters ( filters);

		QDirIterator dirIT( m_imgDir, QDirIterator::Subdirectories);
		while (dirIT.hasNext())
		{
			m_currentFileList.push_back (dirIT.next());		
		}
	#endif
	m_currentFileList.sort();
	this->setCursor(Qt::ArrowCursor);

	if(m_currentFileList.empty())
	{
		qDebug() << "ERROR: no images in dir '" << qPrintable(m_imgDir.path()) << "'";
		QMessageBox::warning(this, tr("ERROR"), tr("no images in directory!"), QMessageBox::Ok);
		enable(false);
		return;
	}

	m_currentTestImageFilename = m_imgDir.absoluteFilePath(m_currentFileList.at(m_currentFileIndex));
	m_curr_annotation = 0;

	getUnlabeledImages();
	updateProgressBar();

	openImageInternal(m_currentTestImageFilename);

	this->setCursor(Qt::ArrowCursor);

	updateCurrentFolderIndex();
	updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
void MainWindow::updateProgressBar()
{
	if(!m_currentFileList.empty())
	{
	   m_current_folder_progress->setMaximum( m_currentFileList.size() );
	   m_current_folder_progress->setValue ( m_currentFileList.size()-m_unlabeled_images.size() );
	   m_current_folder_progress->setEnabled ( true );
	}
	else
	{
	   m_current_folder_progress->setMaximum( 10000 );
	   m_current_folder_progress->setMinimum( 1     );
	   m_current_folder_progress->setValue ( 1 );
	   m_current_folder_progress->setEnabled ( false );
	}
}
//-----------------------------------------------------------------------------
void MainWindow::getUnlabeledImages()
{
	if(!m_sqlConn)
		return;

	m_unlabeled_images.clear();
	m_unlabeled_index = 0;

	FileIDOfImagesWithFaces qurery;
	qurery.img_db_id = m_img_db_id;
	bool allOk = qurery.exec(m_sqlConn);
	if(!allOk)
		return;
	
	std::map<std::string,int>::const_iterator IT;
	
	for(int counter=0; counter < m_currentFileList.size(); ++counter)
	{
		QFileInfo tmpInfo(m_currentFileList.at(counter));
		std::string queryStr = std::string(tmpInfo.fileName().toLocal8Bit());
		IT = qurery.image_face_ids.find(queryStr);

		if(IT == qurery.image_face_ids.end())
			m_unlabeled_images.push_back(counter);
	}
}
//-----------------------------------------------------------------------------s
void MainWindow::gotoNextUnlabeledImage()
{
	if(m_unlabeled_images.empty() || m_unlabeled_index==m_unlabeled_images.size())
	{
		QMessageBox msgBox;
	    msgBox.setText("All images labeled!");
		msgBox.exec();
		m_unlabeled_index = 0;
		return;
	}

	m_currentFileIndex = m_unlabeled_images.at(m_unlabeled_index);
	++m_unlabeled_index;
	
   save();

   m_currentTestImageFilename = m_imgDir.absoluteFilePath(m_currentFileList.at(m_currentFileIndex));
   
   openImageInternal(m_currentTestImageFilename);

   updateCurrentFolderIndex();
   updateCurrentAnnotationIndex();
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MainWindow::updateCurrentAnnotationIndex()
{
	m_current_image_filename->setText(m_currentFileList.at(m_currentFileIndex));

	QString tmpStr;
	tmpStr.setNum(m_curr_annotation+1);

	QString tmpStr2;
	tmpStr2.setNum(m_annotations.size());

	QString tmpStr3(tmpStr + " / " + tmpStr2);

	m_current_face_index->setText(tmpStr3);

	m_image->setFaceIdText(m_annotations.at(m_curr_annotation)->ID);
	QString statusText;
	statusText.sprintf("FaceId: %d | Database: %s | %s",m_annotations.at(m_curr_annotation)->ID,m_img_db_id.c_str(),m_img_db_descr.c_str());
	m_statusBar->setText(statusText);
}
//-----------------------------------------------------------------------------
void MainWindow::updateCurrentFolderIndex()
{
	QString tmpStr;
	tmpStr.setNum(m_currentFileIndex+1);

	QString tmpStr2;
	tmpStr2.setNum(m_currentFileList.size());

	QString tmpStr3(tmpStr + " / " + tmpStr2);

	m_current_folder_index->setText(tmpStr3);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MainWindow::parseFaceIndexTextBox()
{
	QString tmp(m_current_face_index->text());

	QStringList	tmpStrList = tmp.split(" ");
	bool ok;
	int index = tmpStrList.at(0).toInt(&ok) - 1;

	if(ok && (index < m_annotations.size() && index >= 0))
	{
		m_curr_annotation = index;
		nextAnnotation(index + 1); //index in text box starts with one
	}
	else
	{
		m_current_face_index->setText(QString().setNum(m_curr_annotation));
	}
}
//-----------------------------------------------------------------------------
void MainWindow::parseDirectoryIndexTextBox()
{
	QString tmp(m_current_folder_index->text());

	QStringList	tmpStrList = tmp.split(" ");
	bool ok;
	int index = tmpStrList.at(0).toInt(&ok) - 1;

	if(ok && (index < m_currentFileList.size() && index >= 0))
	{
		loadNextImage(index);
	}
	else
	{
		updateCurrentFolderIndex();
		updateCurrentAnnotationIndex();
	}
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MainWindow::findFace()
{
	 bool ok;
	 QString text = QInputDialog::getText(this, tr("Quick Find"),
                                          tr("Face Id:"), QLineEdit::Normal,
                                          "12345", &ok);
     if (ok && !text.isEmpty())
	 {
		 bool ok;
		 int faceId = text.toInt(&ok);
		 if(ok)
		 {
			 ImageByFaceIDQuery query;
			 query.face_id = faceId;
			 ok = query.exec(m_sqlConn);
			 if(ok)
			 {
				 std::string fileId = query.resultImage.file_id;
				 QStringList res = m_currentFileList.filter( fileId.c_str(), Qt::CaseInsensitive);
				 if(!res.empty() && !fileId.empty())
				 {
					 int index = m_currentFileList.indexOf(res.at(0)); //first match...
					 if(index != -1)
					 {
						 loadNextImage(index);
						 return;
					 }
				 }
			 }
		 }
		 
		 QMessageBox::warning(this, tr("WARNING"), tr("Face not found!"), QMessageBox::Ok);
		 return;
	 }
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MainWindow::filterFacesByQuery()
{
	 bool allOK;

	QSettings settings;
    QString query = settings.value("sqlQuery").toString();

	if(query.isEmpty())
		query = "SELECT face_id FROM Faces WHERE db_id = 'flickr'";

	 query = QInputDialog::getText(this, tr("Filter"),
                                          tr("Query:"), QLineEdit::Normal,
                                          query, &allOK);
	 settings.setValue("sqlQuery",query);

	 if(!allOK)
		return;
	
	int ID;
	std::vector<int> faceIds;
	SQLiteStmt *filterQueryStmt = 0;

	this->setCursor(Qt::WaitCursor);

	filterQueryStmt = m_sqlConn->prepare(qPrintable(query));

	if(filterQueryStmt == 0)
	{
		this->setCursor(Qt::ArrowCursor);
		QMessageBox::warning(this, tr("WARNING"), tr("Syntax error!"), QMessageBox::Ok);
		return;
	}

	int res = 0;
	do {
		res = m_sqlConn->step(filterQueryStmt);
		if (res == SQLITE_ROW)
		{
			allOK = allOK && filterQueryStmt->readIntColumn(0,ID);
			if (allOK)
				faceIds.push_back(ID);
			else
				break;
		}
	} while (res == SQLITE_ROW);


	if(!allOK)
	{
		this->setCursor(Qt::ArrowCursor);
		QMessageBox::warning(this, tr("WARNING"), tr("Query not successful, first column must be int!"), QMessageBox::Ok);
		return;
	}

	if(faceIds.empty())
	{
		this->setCursor(Qt::ArrowCursor);
		QMessageBox::warning(this, tr("WARNING"), tr("Empty result set!"), QMessageBox::Ok);
		return;
	}

	retrieveFaceDataById(faceIds,allOK);
	this->setCursor(Qt::ArrowCursor);

	m_curr_annotation = 0;
	displayCurrentAnnotation();
	updateCurrentAnnotationIndex();
	m_naviButtonGroupBox->setEnabled(false);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void  MainWindow::findImage()
{
	 bool ok;

	 QString text = QInputDialog::getText(this, tr("Quick Find"),
                                          tr("File name:"), QLineEdit::Normal,
                                          QFileInfo(m_currentTestImageFilename).fileName(), &ok);
     if (ok && !text.isEmpty())
	 {
		 QStringList res = m_currentFileList.filter( text, Qt::CaseInsensitive);
		 if(res.empty())
		 {
			 QMessageBox::warning(this, tr("WARNING"), tr("File not found!"), QMessageBox::Ok);
			 return;
		 }
		 
		 int index = m_currentFileList.indexOf(res.at(0)); //first match...
		 if(index != -1)
		 {
			 loadNextImage(index);
		 } 	 
	 }

	 m_naviButtonGroupBox->setEnabled(true);
}
//-----------------------------------------------------------------------------
void MainWindow::loadNextImage(int fileIndex)
{
   if(fileIndex != -1)
	   m_currentFileIndex = fileIndex;
   else
      ++m_currentFileIndex;

   if (m_currentFileIndex == m_currentFileList.size())
   {
	  QMessageBox msgBox;
	  msgBox.setText("Last Image!");
	  msgBox.exec();
      m_currentFileIndex = m_currentFileList.size()-1;
   }

   if (m_currentFileIndex >= m_currentFileList.size() || m_currentFileIndex < 0)
   {
	  QMessageBox msgBox;
	  msgBox.setText("Index out of range!");
	  msgBox.exec();
	  m_currentFileIndex = 0;
	  return;
   }

   save();

   m_currentTestImageFilename = m_imgDir.absoluteFilePath(m_currentFileList.at(m_currentFileIndex));
   openImageInternal(m_currentTestImageFilename);
   
   updateCurrentFolderIndex();
   updateCurrentAnnotationIndex();
   m_focusButton->setFocus();
}
//-----------------------------------------------------------------------------
void MainWindow::loadPreviousImage()
{
   --m_currentFileIndex;
   if (m_currentFileIndex < 0)
   {
	  QMessageBox msgBox;
	  msgBox.setText("First Image!");
	  msgBox.exec();

      m_currentFileIndex = 0;
   }

   save();

   m_currentTestImageFilename = m_imgDir.absoluteFilePath(m_currentFileList.at(m_currentFileIndex));

   openImageInternal(m_currentTestImageFilename);
   updateCurrentFolderIndex();
   updateCurrentAnnotationIndex();
   m_focusButton->setFocus();
}
//-----------------------------------------------------------------------------
void MainWindow::save()
{	
	if (m_touchedFeatures == 0x0)
		return;

   if (!m_autosaveButton->isChecked() && m_numberOfSelectedFeatures >= MIN_NUM_FEATURES)
   {
	 QMessageBox msgBox;
	 msgBox.setText("Annotation has been modified");
	 msgBox.setInformativeText("Do you want to save your changes?");
	 msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
	 msgBox.setDefaultButton(QMessageBox::Save);
	 int ret = msgBox.exec();

	 if(ret == QMessageBox::Discard)
	 {
		 m_touchedFeatures = 0x0;
		 return;
	 }
   }

   // in case there is no useful annotation we do nothing!
   if (m_numberOfSelectedFeatures < MIN_NUM_FEATURES)
   {
      QMessageBox::warning(this, tr("WARNING"), tr("Not enough feature points selected!"), QMessageBox::Ok);
	  m_touchedFeatures = 0x0;
      return;
   }
   
   //face data is modified
   if(m_touchedFeatures & 0x1)
   {
		FaceMetadata *md = m_annotations.at(m_curr_annotation)->getMetadata();
		assert(md);

		//*-- update meta data --*//
		if(m_femaleButton->isChecked() || m_maleButton->isChecked())
		{
			md->bw = m_grayscaleButton->isChecked();
			md->glasses = m_glassesButton->isChecked();
			md->occluded = m_occludedButton->isChecked();
			if(m_femaleButton->isChecked())
				md->sex = FaceMetadata::FEMALE;
			else
				md->sex = FaceMetadata::MALE;
		}
		else
		{
			QMessageBox::warning(this, tr("WARNING"), tr("No meta data was specified!"), QMessageBox::Ok);
		}

		if(m_currentPitchAngle == undefinedAngle && m_currentRollAngle == undefinedAngle && m_currentYawAngle == undefinedAngle)
		{
			calculatePose();
		}

		if(m_currentPitchAngle == undefinedAngle && m_currentRollAngle == undefinedAngle && m_currentYawAngle == undefinedAngle)
		{
			QMessageBox::warning(this, tr("WARNING"), tr("Warning pose calculation returned invalid result, some feature points are likely to be wrong!"), QMessageBox::Ok);
		}

		FacePose *fp = m_annotations.at(m_curr_annotation)->getPose();
		assert(fp);

		fp->pitch = m_currentPitchAngle;
		fp->roll = m_currentRollAngle;
		fp->yaw = m_currentYawAngle;
		fp->annotTypeID = ANNOTATION_TYPE_CALC;

		bool ok = m_annotations.at(m_curr_annotation)->save(m_sqlConn);
		if(!ok)
		{
			QMessageBox::warning(this, tr("ERROR"), tr("could not save to database!"), QMessageBox::Ok);
			quit();
		}

		m_touchedFeatures = 0x0;
   }
   else
   {
	   return;
   }
}
//-----------------------------------------------------------------------------
void MainWindow::touchFeatures()
{
	//m_touchedFeatures = true;
	m_touchedFeatures |= 0x1;
}
//-----------------------------------------------------------------------------
void MainWindow::retrieveFaceDataById(std::vector<int> queryIds, bool &allOk)
{
	clearAnnotion();

	FaceDataByIDsQuery faceDataSqlQuery;
	faceDataSqlQuery.queryIds = queryIds;
	allOk = allOk && faceDataSqlQuery.exec(m_sqlConn);

	if(!allOk)
	{
		QMessageBox::warning(this, tr("ERROR"), tr("database error, could not query!"), QMessageBox::Ok);
		m_sqlConn->close();
		quit();
	}

	std::map<int,FaceData*> annotations = faceDataSqlQuery.data;
	std::map<int,FaceData*>::iterator FDMAPIT;
	
	for(size_t counter = 0; counter < queryIds.size(); ++counter)
	{
		FDMAPIT = annotations.find(queryIds.at(counter));
		if(FDMAPIT != annotations.end())
		{
			FaceData *currFaceAnnotation = FDMAPIT->second;
			if(currFaceAnnotation->dbID.compare(m_img_db_id) != 0)
				continue;

			//*-- FaceData has to be copied as it is destroyed on destruction of the query --*//
			FaceData *cpyCurrFaceAnno = new FaceData(*currFaceAnnotation);
			m_annotations.push_back(cpyCurrFaceAnno);
		}
	} 
}
//-----------------------------------------------------------------------------
void MainWindow::prepareAndLoadFaceData(FaceData *fd)
{	
	if (!fd->getFeaturesCoords())
		fd->loadFeatureCoords(m_sqlConn);

	FeaturesCoords *fc = fd->getFeaturesCoords();
	if(!fc) 
	{
		fc = new FeaturesCoords(fd->ID);
		fd->setFeatureCoords(fc);
	}

	FaceMetadata *md = fd->getMetadata();
	if(!md)
	{
		md = new FaceMetadata(-1);
		fd->setMetadata(md);
	}

	FacePose *fp = fd->getPose();
	if(!fp)
	{
		fp = new FacePose();
		fd->setPose(fp);
	}
}
//-----------------------------------------------------------------------------
void MainWindow::clear()
{
	clearFeatures();
	//clearFineAdjustmentAngles();
	clearAngles();
	clearMetaData();
	m_currentDisplayMode = NoModel;
	m_focusButton->setFocus();
}
//-----------------------------------------------------------------------------
void MainWindow::openImageInternal(const QString &currentFilename)
{
	m_image->setImage(currentFilename);
	QFileInfo tmpFileInfo(currentFilename);

	bool allOk;

	FaceIDsByImageQuery faceIdSqlQuery;
	faceIdSqlQuery.img_db_id = m_img_db_id;
	faceIdSqlQuery.img_file_id =  std::string(tmpFileInfo.fileName().toLocal8Bit());
	allOk = faceIdSqlQuery.exec(m_sqlConn);

	retrieveFaceDataById(faceIdSqlQuery.resultFaceIds, allOk);

	m_curr_annotation = 0;
	displayCurrentAnnotation();
}
//-----------------------------------------------------------------------------
void MainWindow::clearAnnotion()
{
   for(int counter = 0; counter < m_annotations.size(); ++counter)
   {
		delete m_annotations.at(counter);
   }
   m_annotations.clear();
   m_curr_annotation = 0;
}
//-----------------------------------------------------------------------------
void MainWindow::displayCurrentAnnotation()
{
	clear();
	if(m_annotations.size() == 0)
	{
		newAnnotation();
		return;
	}
	else
	{
		FaceData *currFace = m_annotations.at(m_curr_annotation);
		prepareAndLoadFaceData(currFace);

		//*-- check if displayed image is also suitable for the annotation --*//
		QFileInfo tmp(m_currentTestImageFilename);
		QString filename = tmp.fileName();
		QString dbfilename(currFace->fileID.c_str());

		if(filename.compare(dbfilename) != 0)
		{
			m_currentTestImageFilename = m_dbBaseDir.absoluteFilePath(currFace->getDbImg()->filepath.c_str());
			m_image->setImage(m_currentTestImageFilename);
			//m_currentFileIndex
		}

		FeaturesCoords *fc = currFace->getFeaturesCoords();
		FaceMetadata *md = currFace->getMetadata();
		FacePose *fp = currFace->getPose();

		assert(fc && md && fp);

		//*-- meta data --*//
		if(md->bw)
			m_grayscaleButton->setChecked(true);
		else
			m_grayscaleButton->setChecked(false);

		if(md->occluded)
			m_occludedButton->setChecked(true);
		else
			m_occludedButton->setChecked(false);

		if(md->glasses)
			m_glassesButton->setChecked(true);
		else
			m_glassesButton->setChecked(false);

		std::string gender = md->sex;

		if(md->sex == FaceMetadata::UNDEFINED)
		{
			m_genderButtonGroup->setExclusive(false);
			m_femaleButton->setChecked(false); 
			m_maleButton->setChecked(false);
			m_genderButtonGroup->setExclusive(true);
		}

		if(md->sex == FaceMetadata::MALE)
		{
			m_genderButtonGroup->setExclusive(true);
			m_maleButton->setChecked(true); 
		}

		if(md->sex == FaceMetadata::FEMALE)
		{
			m_genderButtonGroup->setExclusive(true);
			m_femaleButton->setChecked(true);
		}

		//*-- FACE POSE --*//
		if(fp->pitch != Pose::UNDEFINED_ANGLE && fp->yaw != Pose::UNDEFINED_ANGLE && fp->roll != Pose::UNDEFINED_ANGLE)
		{
			m_viewer->setFocus();
			m_currentRollAngle = fp->roll;
			m_currentPitchAngle = fp->pitch;
			m_currentYawAngle = fp->yaw;
			m_viewer->applyTransformationToSceneGraph(m_currentRollAngle,m_currentPitchAngle,m_currentYawAngle);
		}

		m_numberOfSelectedFeatures = 0;
		for(int counter = 0; counter < NumFeatureCodes; ++counter)
		{
			const std::string &code = FeatureCodes[counter];		
			cv::Point2f point = fc->getCoords(m_featureCodeDBIdMap[code]);

			if (point.x == -1.)
				continue;
			
			m_featuresMap[code] = SbVec2d(point.x, point.y);
			m_featureCodeButtonMap[code]->setText(QString(m_featureCodeButtonText[code].c_str()) + QString("%1").arg(point.x) + QString(";") + QString("%1").arg(point.y));
			++m_numberOfSelectedFeatures;
		}

		updateMarkerImage();

		// check if we can enable the button to calculate the pose model
		if (m_numberOfSelectedFeatures >= 4)
		{
			// enable calc button
			m_calculateButton->setEnabled(true);
		}

		QString currentFaceIdStr = QString("id: %1").arg(currFace->ID);
		//currentFaceIdStr.setNum(currFace->ID);
		m_currentFaceIdLabel->setText(currentFaceIdStr);
	}
}
//-----------------------------------------------------------------------------
void MainWindow::setCurrentFile(const QString &fileName)
 {
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

	settings.setValue("recentFileList",files);
 
    foreach (QWidget *widget, QApplication::topLevelWidgets())  {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}
//-----------------------------------------------------------------------------
void MainWindow::updateRecentFileActions()
 {
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
	QSettings::Status status = settings.status();
 
    int numRecentFiles = min(files.size(), int(MaxRecentFiles));

	for (int i = 0; i < numRecentFiles; ++i)  
	{
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}
//----------------------------------------------------------------------------- 
QString MainWindow::strippedName(const QString &fullFileName)
 {
    return QFileInfo(fullFileName).fileName();
}
//-----------------------------------------------------------------------------
void MainWindow::openRecentFile()
 {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openDatabase(action->data().toString());
}
//-----------------------------------------------------------------------------
void MainWindow::enableFeaturePointButtons(bool enable)
{
	if(enable)
		QObject::connect(m_image, SIGNAL(mousePressedOnImage(QGraphicsSceneMouseEvent*)),
			this, SLOT(on_mousePress(QGraphicsSceneMouseEvent*)) );
	else
		QObject::disconnect(m_image, SIGNAL(mousePressedOnImage(QGraphicsSceneMouseEvent*)),
			this, SLOT(on_mousePress(QGraphicsSceneMouseEvent*)) );

	m_metaDataGroupBox->setChecked(enable);
}
//-----------------------------------------------------------------------------
