/*
 * FeaturePointRadioButton.h
 *
 *  Created on: 09.08.2010
 *      Author: koestinger
 */

#ifndef __FEATURE_POINT_RADIO_BUTTON_H__
#define __FEATURE_POINT_RADIO_BUTTON_H__

#include <QObject>
#include <QRadioButton>

#include "main_window.h"

class FeaturePointRadioButton : public QRadioButton
{
	Q_OBJECT

public:
	FeaturePointRadioButton(MainWindowSelectFeatureMode featCode, const QString &text, QWidget *parent);
	FeaturePointRadioButton(FeatureCode featCode, const QString &text, QWidget *parent);
 
signals:
     void toggled(MainWindowSelectFeatureMode featCode, bool checked);
	 void toggled(FeatureCode featCode, bool checked);

protected:
	MainWindowSelectFeatureMode featCodeEnum_;
	FeatureCode featCode_;

protected slots:
	void catchToggled(bool checked);
};


#endif //FEATURE_POINT_RADIO_BUTTON