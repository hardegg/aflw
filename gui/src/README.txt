Binary Package (Windows)
------------------------

+ Install Microsoft Visual Studio 2010 redistributable (x86)
+ Run PoseAnnotationGuiSQL.exe

BUILD AND INSTALL
-----------------

- Dependencies
Boost 1.46 (http://www.boost.org)
OpenCV 2.3.2 (opencv.willowgarage.com/)
Coin3D 3.1.3 (http://www.coin3d.org)
GLEW 1.5.2 (http://glew.sourceforge.net)
QUARTER 1.0.0 (http://www.coin3d.org/lib/quarter)
QT 4.7 (qt.nokia.com)

- Windows:

Install the libraries. Ensure that the environment variables (COINDIR, GLEWROOT, QUARTERDIR, QTDIR) are properly set.
For facedbsql set the FaceDBSql_DIR environment variable to your install directory
(where there are two subdirectories lib/ and include/facedbsql/ are located).

- Linux: 

If you didn't install the FaceDBSql Library to a default location (/usr/local/)
set the environment variable FaceDBSql_DIR to your install directory
(where there are two subdirectories lib/ and include/facedbsql/)

TROUBLESHOOTING
---------------

+ QT DLL entry point not found, check with dependency walker that proper dlls get loaded. Many software packages as Matlab or even
  Miktex come with there non standard version of Qt. This is especially problematic if these libraries are in the path.

+ Coin Build Error in vers 3.1.3
  transitions.push_back(PImpl::StateTransition((ScXMLObject*)NULL, PRIVATE(this)->initializer.get()));

