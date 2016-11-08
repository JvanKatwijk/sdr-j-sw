/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "qwt_dial.h"

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QwtDial *pskPhaseDisplay;
    QLCDNumber *pskIFdisplay;
    QLCDNumber *pskQualitydisplay;
    QComboBox *pskAfconTrigger;
    QComboBox *pskReverseTrigger;
    QComboBox *pskModeTrigger;
    QSpinBox *pskSquelchLevelTrigger;
    QSpinBox *pskFilterDegreeTrigger;
    QLabel *psktextBox;
    QLabel *label;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(595, 180);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 591, 181));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        pskPhaseDisplay = new QwtDial(frame);
        pskPhaseDisplay->setObjectName(QStringLiteral("pskPhaseDisplay"));
        pskPhaseDisplay->setGeometry(QRect(10, 10, 121, 121));
        pskPhaseDisplay->setLineWidth(4);
        pskIFdisplay = new QLCDNumber(frame);
        pskIFdisplay->setObjectName(QStringLiteral("pskIFdisplay"));
        pskIFdisplay->setGeometry(QRect(170, 10, 64, 23));
        pskIFdisplay->setSegmentStyle(QLCDNumber::Flat);
        pskQualitydisplay = new QLCDNumber(frame);
        pskQualitydisplay->setObjectName(QStringLiteral("pskQualitydisplay"));
        pskQualitydisplay->setGeometry(QRect(170, 40, 64, 23));
        pskQualitydisplay->setSegmentStyle(QLCDNumber::Flat);
        pskAfconTrigger = new QComboBox(frame);
        pskAfconTrigger->setObjectName(QStringLiteral("pskAfconTrigger"));
        pskAfconTrigger->setGeometry(QRect(260, 10, 81, 21));
        pskReverseTrigger = new QComboBox(frame);
        pskReverseTrigger->setObjectName(QStringLiteral("pskReverseTrigger"));
        pskReverseTrigger->setGeometry(QRect(260, 30, 81, 21));
        pskModeTrigger = new QComboBox(frame);
        pskModeTrigger->setObjectName(QStringLiteral("pskModeTrigger"));
        pskModeTrigger->setGeometry(QRect(260, 50, 81, 21));
        pskSquelchLevelTrigger = new QSpinBox(frame);
        pskSquelchLevelTrigger->setObjectName(QStringLiteral("pskSquelchLevelTrigger"));
        pskSquelchLevelTrigger->setGeometry(QRect(260, 70, 81, 21));
        pskSquelchLevelTrigger->setMinimum(3);
        pskFilterDegreeTrigger = new QSpinBox(frame);
        pskFilterDegreeTrigger->setObjectName(QStringLiteral("pskFilterDegreeTrigger"));
        pskFilterDegreeTrigger->setGeometry(QRect(260, 90, 81, 21));
        pskFilterDegreeTrigger->setMinimum(10);
        pskFilterDegreeTrigger->setValue(12);
        psktextBox = new QLabel(frame);
        psktextBox->setObjectName(QStringLiteral("psktextBox"));
        psktextBox->setGeometry(QRect(20, 140, 561, 21));
        psktextBox->setFrameShape(QFrame::Panel);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(490, 20, 91, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        pskAfconTrigger->clear();
        pskAfconTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "Afc off", 0)
         << QApplication::translate("Form", "Afc on", 0)
        );
        pskReverseTrigger->clear();
        pskReverseTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        pskModeTrigger->clear();
        pskModeTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "psk31", 0)
         << QApplication::translate("Form", "psk63", 0)
         << QApplication::translate("Form", "qpsk31", 0)
         << QApplication::translate("Form", "qpsk63", 0)
         << QApplication::translate("Form", "psk125", 0)
         << QApplication::translate("Form", "qpsk125", 0)
        );
        psktextBox->setText(QString());
        label->setText(QApplication::translate("Form", "psk decoder", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
