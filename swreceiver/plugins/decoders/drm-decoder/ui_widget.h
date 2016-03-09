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
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *label;
    QLCDNumber *show_int_offset;
    QwtPlot *iqDisplay;
    QLCDNumber *show_small_offset;
    QLabel *timeSyncLabel;
    QLabel *label_2;
    QLabel *facSyncLabel;
    QLabel *label_3;
    QLabel *modeIndicator;
    QLabel *sdcSyncLabel;
    QLabel *label_4;
    QLabel *stationLabel;
    QLCDNumber *goodFrames;
    QLCDNumber *badFrames;
    QLabel *audioModelabel;
    QLCDNumber *snrDisplay;
    QLabel *spectrumIndicator;
    QLabel *messageLabel;
    QPushButton *channel_1;
    QPushButton *channel_2;
    QPushButton *channel_3;
    QPushButton *channel_4;
    QLCDNumber *timeOffsetDisplay;
    QLCDNumber *clockOffsetDisplay;
    QLCDNumber *timeDelayDisplay;
    QLCDNumber *angleDisplay;
    QLabel *faadSyncLabel;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(606, 180);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 181));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(490, 10, 101, 21));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        show_int_offset = new QLCDNumber(frame);
        show_int_offset->setObjectName(QStringLiteral("show_int_offset"));
        show_int_offset->setGeometry(QRect(50, 10, 41, 21));
        show_int_offset->setSegmentStyle(QLCDNumber::Flat);
        iqDisplay = new QwtPlot(frame);
        iqDisplay->setObjectName(QStringLiteral("iqDisplay"));
        iqDisplay->setGeometry(QRect(100, 10, 331, 131));
        show_small_offset = new QLCDNumber(frame);
        show_small_offset->setObjectName(QStringLiteral("show_small_offset"));
        show_small_offset->setGeometry(QRect(50, 30, 41, 21));
        show_small_offset->setSegmentStyle(QLCDNumber::Flat);
        timeSyncLabel = new QLabel(frame);
        timeSyncLabel->setObjectName(QStringLiteral("timeSyncLabel"));
        timeSyncLabel->setGeometry(QRect(560, 40, 31, 21));
        timeSyncLabel->setFrameShape(QFrame::Box);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(490, 40, 66, 21));
        facSyncLabel = new QLabel(frame);
        facSyncLabel->setObjectName(QStringLiteral("facSyncLabel"));
        facSyncLabel->setGeometry(QRect(560, 60, 31, 21));
        facSyncLabel->setFrameShape(QFrame::Box);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(490, 60, 66, 21));
        modeIndicator = new QLabel(frame);
        modeIndicator->setObjectName(QStringLiteral("modeIndicator"));
        modeIndicator->setGeometry(QRect(70, 100, 31, 21));
        modeIndicator->setFrameShape(QFrame::NoFrame);
        sdcSyncLabel = new QLabel(frame);
        sdcSyncLabel->setObjectName(QStringLiteral("sdcSyncLabel"));
        sdcSyncLabel->setGeometry(QRect(560, 80, 31, 21));
        sdcSyncLabel->setFrameShape(QFrame::Box);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(490, 80, 64, 21));
        stationLabel = new QLabel(frame);
        stationLabel->setObjectName(QStringLiteral("stationLabel"));
        stationLabel->setGeometry(QRect(10, 150, 111, 21));
        stationLabel->setFrameShape(QFrame::NoFrame);
        stationLabel->setLineWidth(0);
        goodFrames = new QLCDNumber(frame);
        goodFrames->setObjectName(QStringLiteral("goodFrames"));
        goodFrames->setGeometry(QRect(490, 150, 41, 16));
        goodFrames->setSegmentStyle(QLCDNumber::Flat);
        badFrames = new QLCDNumber(frame);
        badFrames->setObjectName(QStringLiteral("badFrames"));
        badFrames->setGeometry(QRect(440, 150, 41, 16));
        badFrames->setSegmentStyle(QLCDNumber::Flat);
        audioModelabel = new QLabel(frame);
        audioModelabel->setObjectName(QStringLiteral("audioModelabel"));
        audioModelabel->setGeometry(QRect(10, 131, 64, 20));
        audioModelabel->setFrameShape(QFrame::NoFrame);
        snrDisplay = new QLCDNumber(frame);
        snrDisplay->setObjectName(QStringLiteral("snrDisplay"));
        snrDisplay->setGeometry(QRect(10, 70, 51, 23));
        snrDisplay->setFrameShape(QFrame::NoFrame);
        snrDisplay->setFrameShadow(QFrame::Plain);
        snrDisplay->setLineWidth(0);
        snrDisplay->setSegmentStyle(QLCDNumber::Flat);
        spectrumIndicator = new QLabel(frame);
        spectrumIndicator->setObjectName(QStringLiteral("spectrumIndicator"));
        spectrumIndicator->setGeometry(QRect(10, 100, 31, 21));
        messageLabel = new QLabel(frame);
        messageLabel->setObjectName(QStringLiteral("messageLabel"));
        messageLabel->setGeometry(QRect(130, 150, 401, 21));
        channel_1 = new QPushButton(frame);
        channel_1->setObjectName(QStringLiteral("channel_1"));
        channel_1->setGeometry(QRect(440, 10, 41, 31));
        channel_2 = new QPushButton(frame);
        channel_2->setObjectName(QStringLiteral("channel_2"));
        channel_2->setGeometry(QRect(440, 40, 41, 31));
        channel_3 = new QPushButton(frame);
        channel_3->setObjectName(QStringLiteral("channel_3"));
        channel_3->setGeometry(QRect(440, 70, 41, 31));
        channel_4 = new QPushButton(frame);
        channel_4->setObjectName(QStringLiteral("channel_4"));
        channel_4->setGeometry(QRect(440, 100, 41, 31));
        timeOffsetDisplay = new QLCDNumber(frame);
        timeOffsetDisplay->setObjectName(QStringLiteral("timeOffsetDisplay"));
        timeOffsetDisplay->setGeometry(QRect(10, 30, 41, 21));
        timeOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        clockOffsetDisplay = new QLCDNumber(frame);
        clockOffsetDisplay->setObjectName(QStringLiteral("clockOffsetDisplay"));
        clockOffsetDisplay->setGeometry(QRect(10, 50, 41, 21));
        clockOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        timeDelayDisplay = new QLCDNumber(frame);
        timeDelayDisplay->setObjectName(QStringLiteral("timeDelayDisplay"));
        timeDelayDisplay->setGeometry(QRect(10, 10, 41, 21));
        timeDelayDisplay->setSegmentStyle(QLCDNumber::Flat);
        angleDisplay = new QLCDNumber(frame);
        angleDisplay->setObjectName(QStringLiteral("angleDisplay"));
        angleDisplay->setGeometry(QRect(50, 50, 41, 23));
        angleDisplay->setSegmentStyle(QLCDNumber::Flat);
        faadSyncLabel = new QLabel(frame);
        faadSyncLabel->setObjectName(QStringLiteral("faadSyncLabel"));
        faadSyncLabel->setGeometry(QRect(560, 100, 31, 21));
        faadSyncLabel->setFrameShape(QFrame::Box);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "drm decoder 0.1", 0));
        timeSyncLabel->setText(QString());
        label_2->setText(QApplication::translate("Form", "time sync", 0));
        facSyncLabel->setText(QString());
        label_3->setText(QApplication::translate("Form", "FAC sync", 0));
        modeIndicator->setText(QString());
        sdcSyncLabel->setText(QString());
        label_4->setText(QApplication::translate("Form", "SDC sync", 0));
        stationLabel->setText(QString());
        audioModelabel->setText(QString());
        spectrumIndicator->setText(QString());
        messageLabel->setText(QString());
        channel_1->setText(QString());
        channel_2->setText(QString());
        channel_3->setText(QString());
        channel_4->setText(QString());
        faadSyncLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
