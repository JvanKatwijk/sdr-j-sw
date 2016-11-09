/********************************************************************************
** Form generated from reading UI file 'sdrplay-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRPLAY_2D_WIDGET_H
#define UI_SDRPLAY_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_sdrplayWidget
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *statusLabel;
    QLCDNumber *api_version;
    QSlider *gainSlider;
    QLCDNumber *gainDisplay;
    QCheckBox *agcControl;
    QSpinBox *ppmControl;
    QComboBox *rateSelector;
    QLCDNumber *rateDisplay;
    QCheckBox *debugBox;

    void setupUi(QWidget *sdrplayWidget)
    {
        if (sdrplayWidget->objectName().isEmpty())
            sdrplayWidget->setObjectName(QStringLiteral("sdrplayWidget"));
        sdrplayWidget->resize(256, 241);
        frame = new QFrame(sdrplayWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 10, 211, 231));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 80, 101, 21));
        statusLabel = new QLabel(frame);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setGeometry(QRect(16, 180, 121, 21));
        api_version = new QLCDNumber(frame);
        api_version->setObjectName(QStringLiteral("api_version"));
        api_version->setGeometry(QRect(20, 40, 91, 21));
        api_version->setLineWidth(0);
        api_version->setSegmentStyle(QLCDNumber::Flat);
        gainSlider = new QSlider(frame);
        gainSlider->setObjectName(QStringLiteral("gainSlider"));
        gainSlider->setGeometry(QRect(170, 29, 20, 141));
        gainSlider->setOrientation(Qt::Vertical);
        gainDisplay = new QLCDNumber(frame);
        gainDisplay->setObjectName(QStringLiteral("gainDisplay"));
        gainDisplay->setGeometry(QRect(160, 0, 41, 23));
        gainDisplay->setDigitCount(3);
        gainDisplay->setSegmentStyle(QLCDNumber::Flat);
        agcControl = new QCheckBox(frame);
        agcControl->setObjectName(QStringLiteral("agcControl"));
        agcControl->setGeometry(QRect(100, 120, 51, 25));
        ppmControl = new QSpinBox(frame);
        ppmControl->setObjectName(QStringLiteral("ppmControl"));
        ppmControl->setGeometry(QRect(10, 15, 81, 21));
        ppmControl->setMinimum(-200);
        ppmControl->setMaximum(200);
        rateSelector = new QComboBox(frame);
        rateSelector->setObjectName(QStringLiteral("rateSelector"));
        rateSelector->setGeometry(QRect(0, 150, 151, 21));
        rateDisplay = new QLCDNumber(frame);
        rateDisplay->setObjectName(QStringLiteral("rateDisplay"));
        rateDisplay->setGeometry(QRect(0, 200, 151, 23));
        rateDisplay->setFrameShape(QFrame::NoFrame);
        rateDisplay->setDigitCount(7);
        rateDisplay->setSegmentStyle(QLCDNumber::Flat);
        debugBox = new QCheckBox(frame);
        debugBox->setObjectName(QStringLiteral("debugBox"));
        debugBox->setGeometry(QRect(10, 120, 91, 25));

        retranslateUi(sdrplayWidget);

        QMetaObject::connectSlotsByName(sdrplayWidget);
    } // setupUi

    void retranslateUi(QWidget *sdrplayWidget)
    {
        sdrplayWidget->setWindowTitle(QApplication::translate("sdrplayWidget", "SDRplay control", 0));
        label->setText(QApplication::translate("sdrplayWidget", "mirics-SDRplay", 0));
        statusLabel->setText(QString());
        agcControl->setText(QApplication::translate("sdrplayWidget", "agc", 0));
        rateSelector->clear();
        rateSelector->insertItems(0, QStringList()
         << QApplication::translate("sdrplayWidget", "48000", 0)
         << QApplication::translate("sdrplayWidget", "96000", 0)
         << QApplication::translate("sdrplayWidget", "192000", 0)
         << QApplication::translate("sdrplayWidget", "256000", 0)
         << QApplication::translate("sdrplayWidget", "372000", 0)
         << QApplication::translate("sdrplayWidget", "512000", 0)
        );
        debugBox->setText(QApplication::translate("sdrplayWidget", "debug", 0));
    } // retranslateUi

};

namespace Ui {
    class sdrplayWidget: public Ui_sdrplayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRPLAY_2D_WIDGET_H
