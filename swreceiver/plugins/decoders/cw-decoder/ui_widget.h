/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *cwCharbox;
    QLabel *cwTextbox;
    QSpinBox *SquelchLevel;
    QSpinBox *WPM;
    QSpinBox *FilterDegree;
    QLCDNumber *actualWPM;
    QLabel *label;
    QPushButton *Tracker;
    QLCDNumber *noiseLeveldisplay;
    QLCDNumber *agcPeakdisplay;
    QLCDNumber *spaceLengthdisplay;
    QLCDNumber *dotLengthdisplay;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(599, 169);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 171));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        cwCharbox = new QLabel(frame);
        cwCharbox->setObjectName(QStringLiteral("cwCharbox"));
        cwCharbox->setGeometry(QRect(20, 110, 101, 21));
        cwCharbox->setFrameShape(QFrame::Box);
        cwCharbox->setLineWidth(2);
        cwTextbox = new QLabel(frame);
        cwTextbox->setObjectName(QStringLiteral("cwTextbox"));
        cwTextbox->setGeometry(QRect(20, 130, 571, 21));
        cwTextbox->setFrameShape(QFrame::Box);
        SquelchLevel = new QSpinBox(frame);
        SquelchLevel->setObjectName(QStringLiteral("SquelchLevel"));
        SquelchLevel->setGeometry(QRect(20, 10, 59, 21));
        SquelchLevel->setValue(5);
        WPM = new QSpinBox(frame);
        WPM->setObjectName(QStringLiteral("WPM"));
        WPM->setGeometry(QRect(20, 30, 59, 21));
        WPM->setValue(30);
        FilterDegree = new QSpinBox(frame);
        FilterDegree->setObjectName(QStringLiteral("FilterDegree"));
        FilterDegree->setGeometry(QRect(20, 50, 59, 21));
        FilterDegree->setValue(12);
        actualWPM = new QLCDNumber(frame);
        actualWPM->setObjectName(QStringLiteral("actualWPM"));
        actualWPM->setGeometry(QRect(90, 33, 64, 20));
        actualWPM->setSegmentStyle(QLCDNumber::Flat);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(480, 20, 91, 21));
        Tracker = new QPushButton(frame);
        Tracker->setObjectName(QStringLiteral("Tracker"));
        Tracker->setGeometry(QRect(20, 70, 81, 21));
        noiseLeveldisplay = new QLCDNumber(frame);
        noiseLeveldisplay->setObjectName(QStringLiteral("noiseLeveldisplay"));
        noiseLeveldisplay->setGeometry(QRect(210, 70, 64, 23));
        noiseLeveldisplay->setSegmentStyle(QLCDNumber::Flat);
        agcPeakdisplay = new QLCDNumber(frame);
        agcPeakdisplay->setObjectName(QStringLiteral("agcPeakdisplay"));
        agcPeakdisplay->setGeometry(QRect(290, 70, 64, 23));
        agcPeakdisplay->setSegmentStyle(QLCDNumber::Flat);
        spaceLengthdisplay = new QLCDNumber(frame);
        spaceLengthdisplay->setObjectName(QStringLiteral("spaceLengthdisplay"));
        spaceLengthdisplay->setGeometry(QRect(210, 30, 64, 23));
        spaceLengthdisplay->setSegmentStyle(QLCDNumber::Flat);
        dotLengthdisplay = new QLCDNumber(frame);
        dotLengthdisplay->setObjectName(QStringLiteral("dotLengthdisplay"));
        dotLengthdisplay->setGeometry(QRect(290, 30, 64, 23));
        dotLengthdisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        cwCharbox->setText(QString());
        cwTextbox->setText(QString());
        label->setText(QApplication::translate("Form", "cw decoder", 0));
        Tracker->setText(QApplication::translate("Form", " Tracker on", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
