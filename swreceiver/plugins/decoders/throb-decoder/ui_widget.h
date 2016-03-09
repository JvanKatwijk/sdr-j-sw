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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *label;
    QwtPlot *throbscope;
    QLCDNumber *s2n_display;
    QLCDNumber *metrics_display;
    QLabel *throbTextbox;
    QComboBox *modeSelector;
    QPushButton *reverseSwitch;
    QLCDNumber *throb_ifMarker;
    QPushButton *afcSwitch_button;
    QSpinBox *threshold;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(582, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(440, 10, 101, 21));
        throbscope = new QwtPlot(frame);
        throbscope->setObjectName(QStringLiteral("throbscope"));
        throbscope->setGeometry(QRect(130, 10, 291, 111));
        s2n_display = new QLCDNumber(frame);
        s2n_display->setObjectName(QStringLiteral("s2n_display"));
        s2n_display->setGeometry(QRect(430, 40, 64, 23));
        s2n_display->setSegmentStyle(QLCDNumber::Flat);
        metrics_display = new QLCDNumber(frame);
        metrics_display->setObjectName(QStringLiteral("metrics_display"));
        metrics_display->setGeometry(QRect(500, 40, 64, 23));
        metrics_display->setSegmentStyle(QLCDNumber::Flat);
        throbTextbox = new QLabel(frame);
        throbTextbox->setObjectName(QStringLiteral("throbTextbox"));
        throbTextbox->setGeometry(QRect(10, 130, 551, 21));
        throbTextbox->setFrameShape(QFrame::Box);
        modeSelector = new QComboBox(frame);
        modeSelector->setObjectName(QStringLiteral("modeSelector"));
        modeSelector->setGeometry(QRect(10, 10, 85, 21));
        reverseSwitch = new QPushButton(frame);
        reverseSwitch->setObjectName(QStringLiteral("reverseSwitch"));
        reverseSwitch->setGeometry(QRect(10, 50, 81, 21));
        throb_ifMarker = new QLCDNumber(frame);
        throb_ifMarker->setObjectName(QStringLiteral("throb_ifMarker"));
        throb_ifMarker->setGeometry(QRect(460, 70, 64, 23));
        throb_ifMarker->setSegmentStyle(QLCDNumber::Flat);
        afcSwitch_button = new QPushButton(frame);
        afcSwitch_button->setObjectName(QStringLiteral("afcSwitch_button"));
        afcSwitch_button->setGeometry(QRect(10, 70, 81, 21));
        threshold = new QSpinBox(frame);
        threshold->setObjectName(QStringLiteral("threshold"));
        threshold->setGeometry(QRect(10, 90, 81, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "throbdecoder", 0));
        throbTextbox->setText(QApplication::translate("Form", "TextLabel", 0));
        modeSelector->clear();
        modeSelector->insertItems(0, QStringList()
         << QApplication::translate("Form", "throb1", 0)
         << QApplication::translate("Form", "throb2", 0)
         << QApplication::translate("Form", "throb4", 0)
         << QApplication::translate("Form", "throbx1", 0)
         << QApplication::translate("Form", "throbx2", 0)
        );
        reverseSwitch->setText(QApplication::translate("Form", "normal", 0));
        afcSwitch_button->setText(QApplication::translate("Form", "afc off", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
