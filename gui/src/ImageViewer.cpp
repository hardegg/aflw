#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QDebug>

#include "ImageViewer.h"


ClickableImage::ClickableImage( const QPixmap & pixmap) : QGraphicsPixmapItem(pixmap)
{
	setAcceptedMouseButtons ( Qt::LeftButton );
}

void ClickableImage::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	QPointF pt = event->pos();
	emit mousePressed(event);
}

ImageViewer::ImageViewer(QWidget * parent) :  QGraphicsView(parent), m_modeText(NULL), m_markerSize(10,10), m_meshPoitSize(m_markerSize/5), m_fontName("Arial"), m_faceIdText(NULL), m_border(15)
{
	
}

ImageViewer::~ImageViewer()
{
	
}

void ImageViewer::mousePressedOnImageSlot(QGraphicsSceneMouseEvent * event)
{
	emit mousePressedOnImage(event);
}

void ImageViewer::setImage(const QString &fileName)
{
	qDebug() << fileName;
	QPixmap pixmap(fileName);
	m_imageSize = pixmap.size();

	if(m_imageSize.width() == 0 || m_imageSize.height() == 0)
	{
		QMessageBox::warning(this, tr("ERROR"), tr("Could not read image!"), QMessageBox::Ok);	
		//return;
	}

	ClickableImage *item = new ClickableImage(pixmap);

	QObject::connect(item, SIGNAL(mousePressed(QGraphicsSceneMouseEvent*)),
      this, SLOT(mousePressedOnImageSlot(QGraphicsSceneMouseEvent*)) );
	
	QGraphicsScene *oldscene = this->scene();
	clearMarkers();

	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->addItem(item);
	setScene(scene);
    show();

	fitInView (scene->sceneRect(),Qt::KeepAspectRatio);

	if(oldscene != 0)
	{
		delete oldscene;
	}
}

void ImageViewer::fitToWindow()
{
	const QGraphicsScene *tscene = this->scene();
	if(tscene != 0)
	{
		fitInView (tscene->sceneRect(),Qt::KeepAspectRatio);
		resizeMarkers();
	}
}

void ImageViewer::normalSize()
{
	
}

void ImageViewer::zoomOnFace(const QRectF &rect)
{
	centerOn(rect.center());
	fitInView(rect,Qt::KeepAspectRatio);
	scale ( 0.6, 0.6 );
	resize();
}

void ImageViewer::zoomOut()
{
	scale ( 0.8, 0.8 );
	resize();
}

void ImageViewer::zoomIn()
{
	scale ( 1.25, 1.25 );
	resize();
}

QSize ImageViewer::getImageSize()
{
	return m_imageSize;
}

void ImageViewer::resize()
{
	resizeMarkers();
	resizeText();
}

void ImageViewer::resizeText()
{
	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());

	QFont font(m_fontName, bbSizeInScene.y()*2,QFont::Bold);

	if(m_modeText != 0)
		m_modeText->setFont(font);

	//*-- RESCALE TEXT --*//
	QMap<int,QGraphicsTextItem*>::const_iterator markerIT = m_text.constBegin();
	while (markerIT != m_text.constEnd()) {
		(*markerIT)->setFont(font);
        ++markerIT;
	}
}

void ImageViewer::resizeMarkers()
{
	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());

	//*-- RESCALE MARKERS --*//
	QMap<int,QGraphicsEllipseItem*>::const_iterator markerIT = m_markers.constBegin();
	while (markerIT != m_markers.constEnd()) {
		QPointF pt = (*markerIT)->rect().center();
		QRectF rect(pt - bbSizeInScene/2, pt + bbSizeInScene/2);
		(*markerIT)->setRect(rect);
        ++markerIT;
	}
}

void ImageViewer::setFaceIdText(int id)
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;

	if(m_faceIdText != 0)
	{
		tscene->removeItem(m_faceIdText);
		delete m_faceIdText;	
	}

	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());

	QFont font(m_fontName, bbSizeInScene.y());
	QFontMetrics fm(font);

	int textHeightInPixels = fm.height();
	QRectF sceneRect = tscene->sceneRect();
	
	m_faceIdText = new QGraphicsTextItem();
	QString text;
	text.setNum(id);
	m_faceIdText->setPlainText(text);
	QPointF pt(m_border/currentSceneToViewTransform.m22(),m_imageSize.height()-(textHeightInPixels+m_border/currentSceneToViewTransform.m22()));
	m_faceIdText->setPos ( pt );
	m_faceIdText->setDefaultTextColor(QColor(255,255,255,255));
	m_faceIdText->setFont(font);
	tscene->addItem(m_faceIdText);
}

void ImageViewer::setMarker(int markerId, QPointF &pt, QColor color, QString text)
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;

	//remove old marker
	if(m_markers.contains ( markerId ))
	{
		tscene->removeItem(m_markers[markerId]);
		delete m_markers[markerId];
		m_markers.remove(markerId);
	}
	
	//remove old text hint
	if(m_text.contains ( markerId ))
	{
		tscene->removeItem(m_text[markerId]);
		delete m_markers[markerId];
		m_text.remove(markerId);
	}

	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());
	QRectF rect(pt - bbSizeInScene/2, pt + bbSizeInScene/2);
	
	QBrush brush(color,Qt::SolidPattern ); //filled
    QPen pen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); 

	QGraphicsEllipseItem *marker = new QGraphicsEllipseItem();
	marker->setPen(pen);
	marker->setBrush(brush);
	marker->setRect(rect);

	if(!text.isEmpty())
	{
		QFont font(m_fontName, rect.height()*2,QFont::Bold);
		QGraphicsTextItem *textItem = new QGraphicsTextItem();
		textItem->setPlainText(text);
		textItem->setPos ( pt );
		textItem->setDefaultTextColor(color);
		textItem->setFont(font);
		tscene->addItem(textItem);

		m_text[markerId] = textItem;
	}

	tscene->addItem(marker);
	m_markers[markerId] = marker;
}

void ImageViewer::setMeshPoint(int markerId, QPointF &pt, QColor color)
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;

	//remove old marker
	if(m_marker_mesh.contains ( markerId ))
	{
		tscene->removeItem(m_marker_mesh[markerId]);
		delete m_marker_mesh[markerId];
		m_marker_mesh.remove(markerId);
	}

	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_meshPoitSize.width()/currentSceneToViewTransform.m11(),m_meshPoitSize.height()/currentSceneToViewTransform.m22());
	QRectF rect(pt - bbSizeInScene/2, pt + bbSizeInScene/2);
	
	QBrush brush(color,Qt::SolidPattern ); //filled
	QPen pen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); 

	QGraphicsEllipseItem *marker = new QGraphicsEllipseItem();
	marker->setPen(pen);
	marker->setBrush(brush);
	marker->setRect(rect);

	tscene->addItem(marker);
	m_marker_mesh[markerId] = marker;
}

void ImageViewer::setBackprojection(int markerId, QPointF &pt, QColor color, QString text)
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;

	//remove old marker
	if(m_marker_backprojected.contains(markerId))
	{
		tscene->removeItem(m_marker_backprojected[markerId]);
		delete m_marker_backprojected[markerId];
		m_marker_backprojected.remove(markerId);
	}

	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());
	QRectF rect(pt - bbSizeInScene/2, pt + bbSizeInScene/2);
	
	QBrush brush(color,Qt::SolidPattern ); //filled
    QPen pen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); 

	QGraphicsEllipseItem *marker = new QGraphicsEllipseItem();
	marker->setPen(pen);
	marker->setBrush(brush);
	marker->setRect(rect);

	tscene->addItem(marker);
	m_marker_backprojected[markerId] = marker;
}

void ImageViewer::setModeText(QString text, QColor color)
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;

	if(m_modeText != 0)
	{
		tscene->removeItem(m_modeText);
		delete m_modeText;
	}
	
	color.setAlpha(255);

	QTransform currentSceneToViewTransform = transform();
	QPointF bbSizeInScene(m_markerSize.width()/currentSceneToViewTransform.m11(),m_markerSize.height()/currentSceneToViewTransform.m22());

	QPointF pt(m_border/currentSceneToViewTransform.m11(),m_border/currentSceneToViewTransform.m22());

	QFont font(m_fontName, bbSizeInScene.y()*2,QFont::Bold);
	m_modeText = new QGraphicsTextItem();
	m_modeText->setPlainText(text);
	m_modeText->setPos(pt);
	m_modeText->setDefaultTextColor(color);
	m_modeText->setFont(font);
	tscene->addItem(m_modeText);
}

void ImageViewer::clearMesh()
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;
	QMap<int,QGraphicsEllipseItem*>::const_iterator ib = m_marker_mesh.constBegin();
	while (ib != m_marker_mesh.constEnd()) {
		tscene->removeItem(*ib);
        ++ib;
	}
	m_marker_mesh.clear();
}

void ImageViewer::clearBackprojection()
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;
	QMap<int,QGraphicsEllipseItem*>::const_iterator ib = m_marker_backprojected.constBegin();
	while (ib != m_marker_backprojected.constEnd()) {
		tscene->removeItem(*ib);
        ++ib;
	}
	m_marker_backprojected.clear();
}

void ImageViewer::clearMarkers()
{
	QGraphicsScene *tscene = scene();
	if(tscene == 0)
		return;
	
	//*-- clear markers  --*//
	QMap<int,QGraphicsEllipseItem*>::const_iterator i = m_markers.constBegin();
	while (i != m_markers.constEnd()) {
		tscene->removeItem(*i);
        ++i;
	}
	m_markers.clear();

	//*-- clear backprojected points  --*//
	clearBackprojection();

	//*-- clear mesh --*//
	clearMesh();

	//*-- clear text  --*//
	QMap<int,QGraphicsTextItem*>::const_iterator it = m_text.constBegin();
	while (it != m_text.constEnd()) {
		tscene->removeItem(*it);
        ++it;
	}
	m_text.clear();

	//*-- clear mode text  --*//
	if(m_modeText != 0)
	{
		tscene->removeItem(m_modeText);
		m_modeText = 0;
	}

	//*-- clear face id text --*//
	if(m_faceIdText != 0)
	{
		tscene->removeItem(m_faceIdText);
		m_faceIdText = 0;
	}
}

void ImageViewer::setMarkerSize(const QSize &size)
{
	m_markerSize = size;
}

QSize ImageViewer::getMarkerSize()
{
	
	return m_markerSize;
}
