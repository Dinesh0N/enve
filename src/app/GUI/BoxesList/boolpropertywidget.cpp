// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "boolpropertywidget.h"
#include "GUI/mainwindow.h"
#include "GUI/global.h"

BoolPropertyWidget::BoolPropertyWidget(QWidget *parent) :
    QWidget(parent) {
    setFixedSize(MIN_WIDGET_DIM, MIN_WIDGET_DIM);
}

void BoolPropertyWidget::setTarget(BoolProperty *property) {
    mTarget = property;
    mTargetContainer = nullptr;
}

void BoolPropertyWidget::setTarget(BoolPropertyContainer *property) {
    mTargetContainer = property;
    mTarget = nullptr;
}

void BoolPropertyWidget::mousePressEvent(QMouseEvent *) {
    if(mTargetContainer) {
        mTargetContainer->setValue(!mTargetContainer->getValue());
    }
    if(mTarget) {
        mTarget->setValue(!mTarget->getValue());
    }
    Document::sInstance->actionFinished();
}

void BoolPropertyWidget::paintEvent(QPaintEvent *) {
    if(!mTarget && !mTargetContainer) return;
    QPainter p(this);
    if(mTarget) {
        if(mTarget->SWT_isDisabled()) p.setOpacity(.5);
    } else if(mTargetContainer) {
        if(mTargetContainer->SWT_isDisabled()) p.setOpacity(.5);
    }

    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::white);
    if(mHovered) {
        p.setPen(Qt::white);
    } else {
        p.setPen(Qt::black);
    }

    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 5., 5.);

    bool value;
    if(mTargetContainer) {
        value = mTargetContainer->getValue();
    } else {
        value = mTarget->getValue();
    }
    if(value) {
        p.setPen(QPen(Qt::black, 2.));
        p.drawLine(QPoint(6, height()/2), QPoint(width()/2, height() - 6));
        p.drawLine(QPoint(width()/2, height() - 6), QPoint(width() - 6, 6));
    }

    p.end();
}

void BoolPropertyWidget::enterEvent(QEvent *) {
    mHovered = true;
    update();
}

void BoolPropertyWidget::leaveEvent(QEvent *) {
    mHovered = false;
    update();
}

