/*
 * FeaturePointRadioButton.h
 *
 *  Created on: 09.08.2010
 *      Author: koestinger
 */

#include "FeaturePointRadioButton.h"



//-----------------------------------------------------------------------------
// CTOR
//-----------------------------------------------------------------------------
FeaturePointRadioButton::FeaturePointRadioButton(MainWindowSelectFeatureMode featCode, const QString &text, QWidget *parent) 
	: QRadioButton(text,parent)
	, featCodeEnum_(featCode)
	, featCode_("")
{
	QObject::connect(this, SIGNAL(toggled(bool)),this, SLOT(catchToggled(bool)));
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FeaturePointRadioButton::FeaturePointRadioButton(FeatureCode featCode, const QString &text, QWidget *parent) 
	: QRadioButton(text,parent)
	, featCodeEnum_(NoFeature)
	, featCode_(featCode)
{
	QObject::connect(this, SIGNAL(toggled(bool)),this, SLOT(catchToggled(bool)));
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FeaturePointRadioButton::catchToggled(bool checked)
{
	if(featCodeEnum_==NoFeature)
		emit toggled(featCode_,QAbstractButton::isChecked());
	else
		emit toggled(featCodeEnum_,QAbstractButton::isChecked());
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
