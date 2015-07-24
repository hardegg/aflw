#ifndef IMAGEVIEWER_H_
#define IMAGEVIEWER_H_

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

#include "cv.h"

class QGraphicsScene;
class QGraphicsTextItem;

class ClickableImage : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	public:
		ClickableImage( const QPixmap & pixmap);

	signals:
		void mousePressed(QGraphicsSceneMouseEvent*);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
};


class ImageViewer : public QGraphicsView
{
   Q_OBJECT

public:
	ImageViewer(QWidget * parent = 0 );
	virtual ~ImageViewer();

	QSize getImageSize();

	void setMarkerSize(const QSize &size);
	QSize getMarkerSize();

signals:
	void mousePressedOnImage(QGraphicsSceneMouseEvent * event);

public slots:
	void setImage(const QString &fileName);
	void updateTexture(IplImage* image) {};
   
	void fitToWindow();
	void normalSize();
	void zoomOut();
	void zoomIn();

	void zoomOnFace(const QRectF &rect);

	void clearMarkers();
	void setMarker(int markerId, QPointF &pt, QColor color, QString text);

	void setBackprojection(int markerId, QPointF &pt, QColor color, QString text);
	void clearBackprojection();

	void setMeshPoint(int markerId, QPointF &pt, QColor color);
	void clearMesh();

	void setModeText(QString text, QColor color);
	void setFaceIdText(int id);

protected:
	QMap<int,QGraphicsEllipseItem*> m_markers;
	QMap<int,QGraphicsTextItem*> m_text;

	QMap<int,QGraphicsEllipseItem*> m_marker_backprojected;
	QMap<int,QGraphicsEllipseItem*> m_marker_mesh;

	QGraphicsTextItem *m_faceIdText;
	QGraphicsTextItem *m_modeText;
	QSize m_imageSize;
	QSize m_markerSize;
	
	QSize m_meshPoitSize;
	QString m_fontName;

	int m_border;

	void resizeMarkers();
	void resizeText();

	void resize();

private slots:
	void mousePressedOnImageSlot(QGraphicsSceneMouseEvent * event);
   
private:

};


#endif /*IMAGEVIEWERGLWIDGET_H_*/