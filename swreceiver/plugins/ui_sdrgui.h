/********************************************************************************
** Form generated from reading UI file 'sdrgui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRGUI_H
#define UI_SDRGUI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include "qwt_plot.h"
#include "qwt_scale_widget.h"
#include "qwt_text_label.h"

QT_BEGIN_NAMESPACE

class Ui_SDR
{
public:
    QPushButton *add_one;
    QPushButton *add_two;
    QPushButton *add_four;
    QPushButton *add_five;
    QPushButton *add_six;
    QPushButton *add_seven;
    QPushButton *add_eight;
    QPushButton *add_nine;
    QPushButton *add_zero;
    QPushButton *dec_50;
    QPushButton *dec_5;
    QPushButton *inc_500;
    QPushButton *inc_50;
    QPushButton *inc_5;
    QLCDNumber *lcdFrequency;
    QPushButton *dec_500;
    QPushButton *add_three;
    QPushButton *add_correct;
    QPushButton *HzSelector;
    QPushButton *add_clear;
    QSlider *LFspectrumamplitudeSlider;
    QComboBox *quickbandSelect;
    QComboBox *LFplotterView;
    QSlider *HFspectrumamplitudeSlider;
    QComboBox *streamOutSelector;
    QLCDNumber *IQBalanceDisplay;
    QPushButton *startButton;
    QPushButton *khzSelector;
    QwtPlot *hfscope;
    QwtPlot *lfscope;
    QComboBox *HFplotterView;
    QPushButton *QuitButton;
    QPushButton *pauseButton;
    QPushButton *HFdumpButton;
    QLCDNumber *attenuationLevelDisplay;
    QLCDNumber *bandwidthDisplay;
    QLCDNumber *bandoffsetDisplay;
    QLabel *systemindicator;
    QComboBox *AGC_select;
    QSpinBox *symbol_shifter;
    QLCDNumber *agc_Display;
    QSpinBox *agc_ThresholdSlider;
    QFrame *rigFrame;
    QFrame *decoderFrame;
    QPushButton *IFdumpButton;
    QComboBox *workingrateSelect;
    QLabel *timeDisplay;
    QLCDNumber *lcd_SampleRate;
    QSpinBox *mouseIncrementSlider;
    QPushButton *setFrequencyinMiddle;
    QComboBox *inputModeSelect;
    QComboBox *rigSelectButton;
    QComboBox *decoderSelectButton;
    QPushButton *LFdumpButton;
    QLCDNumber *peakIndicator;
    QSlider *attenuationSlider;
    QSlider *balanceSlider;
    QSlider *bandwidthSlider;
    QSlider *bandoffsetSlider;

    void setupUi(QDialog *SDR)
    {
        if (SDR->objectName().isEmpty())
            SDR->setObjectName(QString::fromUtf8("SDR"));
        SDR->resize(882, 615);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(255, 248, 228, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        SDR->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        SDR->setFont(font);
        add_one = new QPushButton(SDR);
        add_one->setObjectName(QString::fromUtf8("add_one"));
        add_one->setGeometry(QRect(340, 300, 40, 31));
        QFont font1;
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        add_one->setFont(font1);
        add_one->setDefault(false);
        add_one->setFlat(false);
        add_two = new QPushButton(SDR);
        add_two->setObjectName(QString::fromUtf8("add_two"));
        add_two->setGeometry(QRect(380, 300, 41, 31));
        add_two->setFont(font1);
        add_four = new QPushButton(SDR);
        add_four->setObjectName(QString::fromUtf8("add_four"));
        add_four->setGeometry(QRect(340, 330, 40, 31));
        add_four->setFont(font1);
        add_five = new QPushButton(SDR);
        add_five->setObjectName(QString::fromUtf8("add_five"));
        add_five->setGeometry(QRect(380, 330, 40, 31));
        add_five->setFont(font1);
        add_six = new QPushButton(SDR);
        add_six->setObjectName(QString::fromUtf8("add_six"));
        add_six->setGeometry(QRect(420, 330, 40, 31));
        add_six->setFont(font1);
        add_seven = new QPushButton(SDR);
        add_seven->setObjectName(QString::fromUtf8("add_seven"));
        add_seven->setGeometry(QRect(340, 360, 40, 31));
        add_seven->setFont(font1);
        add_eight = new QPushButton(SDR);
        add_eight->setObjectName(QString::fromUtf8("add_eight"));
        add_eight->setGeometry(QRect(380, 360, 40, 31));
        add_eight->setFont(font1);
        add_nine = new QPushButton(SDR);
        add_nine->setObjectName(QString::fromUtf8("add_nine"));
        add_nine->setGeometry(QRect(420, 360, 40, 31));
        QFont font2;
        font2.setBold(true);
        font2.setItalic(true);
        font2.setUnderline(false);
        font2.setWeight(75);
        add_nine->setFont(font2);
        add_zero = new QPushButton(SDR);
        add_zero->setObjectName(QString::fromUtf8("add_zero"));
        add_zero->setGeometry(QRect(380, 390, 40, 31));
        add_zero->setFont(font1);
        dec_50 = new QPushButton(SDR);
        dec_50->setObjectName(QString::fromUtf8("dec_50"));
        dec_50->setGeometry(QRect(290, 360, 51, 31));
        QFont font3;
        font3.setPointSize(8);
        dec_50->setFont(font3);
        dec_5 = new QPushButton(SDR);
        dec_5->setObjectName(QString::fromUtf8("dec_5"));
        dec_5->setGeometry(QRect(290, 390, 51, 31));
        dec_5->setFont(font3);
        inc_500 = new QPushButton(SDR);
        inc_500->setObjectName(QString::fromUtf8("inc_500"));
        inc_500->setGeometry(QRect(460, 330, 51, 31));
        inc_500->setFont(font3);
        inc_50 = new QPushButton(SDR);
        inc_50->setObjectName(QString::fromUtf8("inc_50"));
        inc_50->setGeometry(QRect(460, 360, 50, 31));
        inc_50->setFont(font3);
        inc_5 = new QPushButton(SDR);
        inc_5->setObjectName(QString::fromUtf8("inc_5"));
        inc_5->setGeometry(QRect(460, 390, 50, 31));
        inc_5->setFont(font3);
        lcdFrequency = new QLCDNumber(SDR);
        lcdFrequency->setObjectName(QString::fromUtf8("lcdFrequency"));
        lcdFrequency->setGeometry(QRect(130, 320, 161, 41));
        QPalette palette1;
        QBrush brush2(QColor(255, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush2);
        QBrush brush3(QColor(255, 255, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush3);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        lcdFrequency->setPalette(palette1);
        QFont font4;
        font4.setPointSize(16);
        font4.setBold(true);
        font4.setItalic(true);
        font4.setWeight(75);
        lcdFrequency->setFont(font4);
        lcdFrequency->setAutoFillBackground(false);
        lcdFrequency->setFrameShape(QFrame::NoFrame);
        lcdFrequency->setFrameShadow(QFrame::Plain);
        lcdFrequency->setLineWidth(0);
        lcdFrequency->setMidLineWidth(0);
        lcdFrequency->setDigitCount(9);
        lcdFrequency->setSegmentStyle(QLCDNumber::Flat);
        dec_500 = new QPushButton(SDR);
        dec_500->setObjectName(QString::fromUtf8("dec_500"));
        dec_500->setGeometry(QRect(290, 330, 51, 31));
        dec_500->setFont(font3);
        add_three = new QPushButton(SDR);
        add_three->setObjectName(QString::fromUtf8("add_three"));
        add_three->setGeometry(QRect(420, 300, 40, 31));
        add_three->setFont(font1);
        add_correct = new QPushButton(SDR);
        add_correct->setObjectName(QString::fromUtf8("add_correct"));
        add_correct->setGeometry(QRect(290, 300, 51, 31));
        QFont font5;
        font5.setBold(true);
        font5.setWeight(75);
        add_correct->setFont(font5);
        HzSelector = new QPushButton(SDR);
        HzSelector->setObjectName(QString::fromUtf8("HzSelector"));
        HzSelector->setGeometry(QRect(420, 390, 40, 31));
        HzSelector->setFont(font5);
        add_clear = new QPushButton(SDR);
        add_clear->setObjectName(QString::fromUtf8("add_clear"));
        add_clear->setGeometry(QRect(460, 300, 51, 31));
        add_clear->setFont(font5);
        LFspectrumamplitudeSlider = new QSlider(SDR);
        LFspectrumamplitudeSlider->setObjectName(QString::fromUtf8("LFspectrumamplitudeSlider"));
        LFspectrumamplitudeSlider->setGeometry(QRect(10, 130, 20, 151));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        LFspectrumamplitudeSlider->setPalette(palette2);
        LFspectrumamplitudeSlider->setMinimum(5);
        LFspectrumamplitudeSlider->setMaximum(100);
        LFspectrumamplitudeSlider->setValue(30);
        LFspectrumamplitudeSlider->setOrientation(Qt::Vertical);
        LFspectrumamplitudeSlider->setTickPosition(QSlider::TicksBothSides);
        LFspectrumamplitudeSlider->setTickInterval(10);
        quickbandSelect = new QComboBox(SDR);
        quickbandSelect->setObjectName(QString::fromUtf8("quickbandSelect"));
        quickbandSelect->setGeometry(QRect(510, 300, 71, 21));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Andale Mono"));
        font6.setPointSize(8);
        quickbandSelect->setFont(font6);
        LFplotterView = new QComboBox(SDR);
        LFplotterView->setObjectName(QString::fromUtf8("LFplotterView"));
        LFplotterView->setGeometry(QRect(360, 280, 121, 21));
        LFplotterView->setFont(font6);
        HFspectrumamplitudeSlider = new QSlider(SDR);
        HFspectrumamplitudeSlider->setObjectName(QString::fromUtf8("HFspectrumamplitudeSlider"));
        HFspectrumamplitudeSlider->setGeometry(QRect(10, 0, 20, 121));
        QPalette palette3;
        QBrush brush4(QColor(255, 85, 0, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Button, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush4);
        HFspectrumamplitudeSlider->setPalette(palette3);
        HFspectrumamplitudeSlider->setMinimum(10);
        HFspectrumamplitudeSlider->setMaximum(100);
        HFspectrumamplitudeSlider->setSliderPosition(50);
        HFspectrumamplitudeSlider->setOrientation(Qt::Vertical);
        HFspectrumamplitudeSlider->setTickPosition(QSlider::TicksBothSides);
        HFspectrumamplitudeSlider->setTickInterval(20);
        streamOutSelector = new QComboBox(SDR);
        streamOutSelector->setObjectName(QString::fromUtf8("streamOutSelector"));
        streamOutSelector->setGeometry(QRect(630, 400, 241, 21));
        QFont font7;
        font7.setPointSize(8);
        font7.setBold(false);
        font7.setItalic(true);
        font7.setWeight(50);
        streamOutSelector->setFont(font7);
        IQBalanceDisplay = new QLCDNumber(SDR);
        IQBalanceDisplay->setObjectName(QString::fromUtf8("IQBalanceDisplay"));
        IQBalanceDisplay->setGeometry(QRect(780, 340, 41, 21));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::Base, brush);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        IQBalanceDisplay->setPalette(palette4);
        IQBalanceDisplay->setAutoFillBackground(false);
        IQBalanceDisplay->setFrameShadow(QFrame::Plain);
        IQBalanceDisplay->setLineWidth(0);
        IQBalanceDisplay->setDigitCount(3);
        IQBalanceDisplay->setSegmentStyle(QLCDNumber::Flat);
        startButton = new QPushButton(SDR);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(820, 280, 51, 41));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::Base, brush);
        palette5.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        startButton->setPalette(palette5);
        startButton->setFont(font5);
        startButton->setAutoFillBackground(true);
        khzSelector = new QPushButton(SDR);
        khzSelector->setObjectName(QString::fromUtf8("khzSelector"));
        khzSelector->setGeometry(QRect(340, 390, 41, 31));
        khzSelector->setFont(font5);
        hfscope = new QwtPlot(SDR);
        hfscope->setObjectName(QString::fromUtf8("hfscope"));
        hfscope->setGeometry(QRect(30, 0, 841, 131));
        hfscope->setAutoFillBackground(false);
        hfscope->setFrameShape(QFrame::Panel);
        hfscope->setFrameShadow(QFrame::Raised);
        hfscope->setLineWidth(3);
        lfscope = new QwtPlot(SDR);
        lfscope->setObjectName(QString::fromUtf8("lfscope"));
        lfscope->setGeometry(QRect(30, 130, 841, 151));
        lfscope->setFrameShape(QFrame::Panel);
        lfscope->setFrameShadow(QFrame::Raised);
        lfscope->setLineWidth(3);
        HFplotterView = new QComboBox(SDR);
        HFplotterView->setObjectName(QString::fromUtf8("HFplotterView"));
        HFplotterView->setGeometry(QRect(240, 280, 121, 21));
        HFplotterView->setFont(font6);
        QuitButton = new QPushButton(SDR);
        QuitButton->setObjectName(QString::fromUtf8("QuitButton"));
        QuitButton->setGeometry(QRect(820, 320, 51, 41));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette6.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        QuitButton->setPalette(palette6);
        QuitButton->setFont(font5);
        QuitButton->setAutoFillBackground(true);
        pauseButton = new QPushButton(SDR);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        pauseButton->setGeometry(QRect(820, 360, 51, 41));
        QFont font8;
        font8.setPointSize(9);
        font8.setBold(true);
        font8.setItalic(true);
        font8.setWeight(75);
        pauseButton->setFont(font8);
        HFdumpButton = new QPushButton(SDR);
        HFdumpButton->setObjectName(QString::fromUtf8("HFdumpButton"));
        HFdumpButton->setGeometry(QRect(10, 280, 81, 21));
        attenuationLevelDisplay = new QLCDNumber(SDR);
        attenuationLevelDisplay->setObjectName(QString::fromUtf8("attenuationLevelDisplay"));
        attenuationLevelDisplay->setGeometry(QRect(780, 320, 41, 21));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::Base, brush);
        palette7.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette7.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette7.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette7.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        attenuationLevelDisplay->setPalette(palette7);
        attenuationLevelDisplay->setAutoFillBackground(false);
        attenuationLevelDisplay->setFrameShadow(QFrame::Plain);
        attenuationLevelDisplay->setLineWidth(0);
        attenuationLevelDisplay->setDigitCount(3);
        attenuationLevelDisplay->setSegmentStyle(QLCDNumber::Flat);
        bandwidthDisplay = new QLCDNumber(SDR);
        bandwidthDisplay->setObjectName(QString::fromUtf8("bandwidthDisplay"));
        bandwidthDisplay->setGeometry(QRect(770, 360, 51, 21));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::Base, brush);
        palette8.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette8.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette8.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette8.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        bandwidthDisplay->setPalette(palette8);
        bandwidthDisplay->setAutoFillBackground(false);
        bandwidthDisplay->setFrameShape(QFrame::NoFrame);
        bandwidthDisplay->setFrameShadow(QFrame::Plain);
        bandwidthDisplay->setLineWidth(0);
        bandwidthDisplay->setDigitCount(5);
        bandwidthDisplay->setSegmentStyle(QLCDNumber::Flat);
        bandoffsetDisplay = new QLCDNumber(SDR);
        bandoffsetDisplay->setObjectName(QString::fromUtf8("bandoffsetDisplay"));
        bandoffsetDisplay->setGeometry(QRect(770, 380, 51, 21));
        QPalette palette9;
        palette9.setBrush(QPalette::Active, QPalette::Base, brush);
        palette9.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette9.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette9.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette9.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        bandoffsetDisplay->setPalette(palette9);
        bandoffsetDisplay->setAutoFillBackground(false);
        bandoffsetDisplay->setFrameShape(QFrame::NoFrame);
        bandoffsetDisplay->setFrameShadow(QFrame::Raised);
        bandoffsetDisplay->setLineWidth(2);
        bandoffsetDisplay->setDigitCount(5);
        bandoffsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        systemindicator = new QLabel(SDR);
        systemindicator->setObjectName(QString::fromUtf8("systemindicator"));
        systemindicator->setGeometry(QRect(630, 300, 191, 21));
        QFont font9;
        font9.setPointSize(9);
        systemindicator->setFont(font9);
        systemindicator->setFrameShape(QFrame::Panel);
        systemindicator->setFrameShadow(QFrame::Raised);
        systemindicator->setLineWidth(2);
        AGC_select = new QComboBox(SDR);
        AGC_select->setObjectName(QString::fromUtf8("AGC_select"));
        AGC_select->setGeometry(QRect(200, 300, 91, 21));
        AGC_select->setFont(font6);
        symbol_shifter = new QSpinBox(SDR);
        symbol_shifter->setObjectName(QString::fromUtf8("symbol_shifter"));
        symbol_shifter->setGeometry(QRect(10, 300, 41, 21));
        symbol_shifter->setKeyboardTracking(false);
        symbol_shifter->setMinimum(-15);
        symbol_shifter->setMaximum(15);
        agc_Display = new QLCDNumber(SDR);
        agc_Display->setObjectName(QString::fromUtf8("agc_Display"));
        agc_Display->setGeometry(QRect(120, 300, 81, 20));
        QPalette palette10;
        palette10.setBrush(QPalette::Active, QPalette::Base, brush);
        palette10.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette10.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette10.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        agc_Display->setPalette(palette10);
        agc_Display->setAutoFillBackground(false);
        agc_Display->setFrameShape(QFrame::NoFrame);
        agc_Display->setFrameShadow(QFrame::Plain);
        agc_Display->setLineWidth(0);
        agc_Display->setSegmentStyle(QLCDNumber::Flat);
        agc_ThresholdSlider = new QSpinBox(SDR);
        agc_ThresholdSlider->setObjectName(QString::fromUtf8("agc_ThresholdSlider"));
        agc_ThresholdSlider->setGeometry(QRect(50, 300, 61, 20));
        agc_ThresholdSlider->setMinimum(-150);
        agc_ThresholdSlider->setMaximum(0);
        agc_ThresholdSlider->setValue(-115);
        rigFrame = new QFrame(SDR);
        rigFrame->setObjectName(QString::fromUtf8("rigFrame"));
        rigFrame->setGeometry(QRect(10, 360, 181, 251));
        rigFrame->setFrameShape(QFrame::Box);
        rigFrame->setFrameShadow(QFrame::Raised);
        rigFrame->setLineWidth(2);
        decoderFrame = new QFrame(SDR);
        decoderFrame->setObjectName(QString::fromUtf8("decoderFrame"));
        decoderFrame->setGeometry(QRect(200, 420, 671, 191));
        decoderFrame->setFrameShape(QFrame::Box);
        decoderFrame->setFrameShadow(QFrame::Raised);
        decoderFrame->setLineWidth(2);
        IFdumpButton = new QPushButton(SDR);
        IFdumpButton->setObjectName(QString::fromUtf8("IFdumpButton"));
        IFdumpButton->setGeometry(QRect(90, 280, 71, 21));
        workingrateSelect = new QComboBox(SDR);
        workingrateSelect->setObjectName(QString::fromUtf8("workingrateSelect"));
        workingrateSelect->setGeometry(QRect(10, 320, 71, 21));
        workingrateSelect->setFont(font3);
        timeDisplay = new QLabel(SDR);
        timeDisplay->setObjectName(QString::fromUtf8("timeDisplay"));
        timeDisplay->setGeometry(QRect(620, 280, 201, 21));
        timeDisplay->setFrameShape(QFrame::Panel);
        timeDisplay->setFrameShadow(QFrame::Raised);
        timeDisplay->setLineWidth(2);
        lcd_SampleRate = new QLCDNumber(SDR);
        lcd_SampleRate->setObjectName(QString::fromUtf8("lcd_SampleRate"));
        lcd_SampleRate->setGeometry(QRect(10, 340, 71, 21));
        QPalette palette11;
        palette11.setBrush(QPalette::Active, QPalette::Base, brush);
        palette11.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette11.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette11.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette11.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        lcd_SampleRate->setPalette(palette11);
        lcd_SampleRate->setAutoFillBackground(false);
        lcd_SampleRate->setFrameShadow(QFrame::Plain);
        lcd_SampleRate->setLineWidth(0);
        lcd_SampleRate->setDigitCount(6);
        lcd_SampleRate->setSegmentStyle(QLCDNumber::Flat);
        mouseIncrementSlider = new QSpinBox(SDR);
        mouseIncrementSlider->setObjectName(QString::fromUtf8("mouseIncrementSlider"));
        mouseIncrementSlider->setGeometry(QRect(510, 400, 51, 21));
        mouseIncrementSlider->setKeyboardTracking(false);
        mouseIncrementSlider->setMaximum(1000);
        mouseIncrementSlider->setValue(20);
        setFrequencyinMiddle = new QPushButton(SDR);
        setFrequencyinMiddle->setObjectName(QString::fromUtf8("setFrequencyinMiddle"));
        setFrequencyinMiddle->setGeometry(QRect(480, 280, 61, 21));
        setFrequencyinMiddle->setFont(font6);
        inputModeSelect = new QComboBox(SDR);
        inputModeSelect->setObjectName(QString::fromUtf8("inputModeSelect"));
        inputModeSelect->setGeometry(QRect(540, 280, 81, 21));
        inputModeSelect->setFont(font6);
        rigSelectButton = new QComboBox(SDR);
        rigSelectButton->setObjectName(QString::fromUtf8("rigSelectButton"));
        rigSelectButton->setGeometry(QRect(200, 360, 91, 31));
        decoderSelectButton = new QComboBox(SDR);
        decoderSelectButton->setObjectName(QString::fromUtf8("decoderSelectButton"));
        decoderSelectButton->setGeometry(QRect(200, 390, 91, 31));
        LFdumpButton = new QPushButton(SDR);
        LFdumpButton->setObjectName(QString::fromUtf8("LFdumpButton"));
        LFdumpButton->setGeometry(QRect(160, 280, 81, 21));
        QFont font10;
        font10.setPointSize(10);
        LFdumpButton->setFont(font10);
        peakIndicator = new QLCDNumber(SDR);
        peakIndicator->setObjectName(QString::fromUtf8("peakIndicator"));
        peakIndicator->setGeometry(QRect(80, 320, 41, 23));
        QFont font11;
        font11.setPointSize(12);
        peakIndicator->setFont(font11);
        peakIndicator->setDigitCount(2);
        peakIndicator->setSegmentStyle(QLCDNumber::Flat);
        attenuationSlider = new QSlider(SDR);
        attenuationSlider->setObjectName(QString::fromUtf8("attenuationSlider"));
        attenuationSlider->setGeometry(QRect(510, 320, 261, 19));
        QPalette palette12;
        palette12.setBrush(QPalette::Active, QPalette::Base, brush);
        palette12.setBrush(QPalette::Active, QPalette::Window, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette12.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette12.setBrush(QPalette::Disabled, QPalette::Window, brush);
        attenuationSlider->setPalette(palette12);
        attenuationSlider->setAutoFillBackground(true);
        attenuationSlider->setOrientation(Qt::Horizontal);
        attenuationSlider->setTickPosition(QSlider::NoTicks);
        balanceSlider = new QSlider(SDR);
        balanceSlider->setObjectName(QString::fromUtf8("balanceSlider"));
        balanceSlider->setGeometry(QRect(510, 340, 261, 19));
        balanceSlider->setMinimum(-40);
        balanceSlider->setMaximum(40);
        balanceSlider->setOrientation(Qt::Horizontal);
        bandwidthSlider = new QSlider(SDR);
        bandwidthSlider->setObjectName(QString::fromUtf8("bandwidthSlider"));
        bandwidthSlider->setGeometry(QRect(510, 360, 261, 19));
        bandwidthSlider->setMaximum(1000);
        bandwidthSlider->setOrientation(Qt::Horizontal);
        bandoffsetSlider = new QSlider(SDR);
        bandoffsetSlider->setObjectName(QString::fromUtf8("bandoffsetSlider"));
        bandoffsetSlider->setGeometry(QRect(510, 380, 261, 19));
        bandoffsetSlider->setMinimum(-500);
        bandoffsetSlider->setMaximum(500);
        bandoffsetSlider->setOrientation(Qt::Horizontal);

        retranslateUi(SDR);

        QMetaObject::connectSlotsByName(SDR);
    } // setupUi

    void retranslateUi(QDialog *SDR)
    {
        SDR->setWindowTitle(QApplication::translate("SDR", "jan sdr", 0, QApplication::UnicodeUTF8));
        SDR->setWindowIconText(QApplication::translate("SDR", "QUIT", 0, QApplication::UnicodeUTF8));
        add_one->setText(QApplication::translate("SDR", "1", 0, QApplication::UnicodeUTF8));
        add_two->setText(QApplication::translate("SDR", "2", 0, QApplication::UnicodeUTF8));
        add_four->setText(QApplication::translate("SDR", "4", 0, QApplication::UnicodeUTF8));
        add_five->setText(QApplication::translate("SDR", "5", 0, QApplication::UnicodeUTF8));
        add_six->setText(QApplication::translate("SDR", "6", 0, QApplication::UnicodeUTF8));
        add_seven->setText(QApplication::translate("SDR", "7", 0, QApplication::UnicodeUTF8));
        add_eight->setText(QApplication::translate("SDR", "8", 0, QApplication::UnicodeUTF8));
        add_nine->setText(QApplication::translate("SDR", "9", 0, QApplication::UnicodeUTF8));
        add_zero->setText(QApplication::translate("SDR", "0", 0, QApplication::UnicodeUTF8));
        dec_50->setText(QApplication::translate("SDR", "-10Hz", 0, QApplication::UnicodeUTF8));
        dec_5->setText(QApplication::translate("SDR", "-1 Hz", 0, QApplication::UnicodeUTF8));
        inc_500->setText(QApplication::translate("SDR", "+100 Hz", 0, QApplication::UnicodeUTF8));
        inc_50->setText(QApplication::translate("SDR", "+10Hz", 0, QApplication::UnicodeUTF8));
        inc_5->setText(QApplication::translate("SDR", "+1 Hz", 0, QApplication::UnicodeUTF8));
        dec_500->setText(QApplication::translate("SDR", "-100 Hz", 0, QApplication::UnicodeUTF8));
        add_three->setText(QApplication::translate("SDR", "3", 0, QApplication::UnicodeUTF8));
        add_correct->setText(QApplication::translate("SDR", "Corr", 0, QApplication::UnicodeUTF8));
        HzSelector->setText(QApplication::translate("SDR", "Hz", 0, QApplication::UnicodeUTF8));
        add_clear->setText(QApplication::translate("SDR", "Clear", 0, QApplication::UnicodeUTF8));
        quickbandSelect->clear();
        quickbandSelect->insertItems(0, QStringList()
         << QApplication::translate("SDR", "usb", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "am", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "lsb", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "drm", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "300", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "1000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "1500", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "2000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "2500", 0, QApplication::UnicodeUTF8)
        );
        LFplotterView->clear();
        LFplotterView->insertItems(0, QStringList()
         << QApplication::translate("SDR", "spectrumView", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "waterfallView", 0, QApplication::UnicodeUTF8)
        );
        streamOutSelector->clear();
        streamOutSelector->insertItems(0, QStringList()
         << QApplication::translate("SDR", "select output", 0, QApplication::UnicodeUTF8)
        );
        startButton->setText(QApplication::translate("SDR", "START", 0, QApplication::UnicodeUTF8));
        khzSelector->setText(QApplication::translate("SDR", "khz", 0, QApplication::UnicodeUTF8));
        HFplotterView->clear();
        HFplotterView->insertItems(0, QStringList()
         << QApplication::translate("SDR", "spectrumView", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "waterfallView", 0, QApplication::UnicodeUTF8)
        );
        QuitButton->setText(QApplication::translate("SDR", "QUIT", 0, QApplication::UnicodeUTF8));
        pauseButton->setText(QApplication::translate("SDR", "PAUSE", 0, QApplication::UnicodeUTF8));
        HFdumpButton->setText(QApplication::translate("SDR", "hf dump", 0, QApplication::UnicodeUTF8));
        systemindicator->setText(QApplication::translate("SDR", "JFF-SDR V6.0 Portaudio", 0, QApplication::UnicodeUTF8));
        AGC_select->clear();
        AGC_select->insertItems(0, QStringList()
         << QApplication::translate("SDR", "off AGC", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "slow", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "fast", 0, QApplication::UnicodeUTF8)
        );
        IFdumpButton->setText(QApplication::translate("SDR", "if dump", 0, QApplication::UnicodeUTF8));
        workingrateSelect->clear();
        workingrateSelect->insertItems(0, QStringList()
         << QApplication::translate("SDR", "6000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "4000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "9600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "8000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "12000", 0, QApplication::UnicodeUTF8)
        );
        timeDisplay->setText(QApplication::translate("SDR", "TextLabel", 0, QApplication::UnicodeUTF8));
        setFrequencyinMiddle->setText(QApplication::translate("SDR", "middle", 0, QApplication::UnicodeUTF8));
        inputModeSelect->clear();
        inputModeSelect->insertItems(0, QStringList()
         << QApplication::translate("SDR", "I and Q", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "I Only", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "Q and I", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "Q Only", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SDR", "test", 0, QApplication::UnicodeUTF8)
        );
        LFdumpButton->setText(QApplication::translate("SDR", "lf dump", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SDR: public Ui_SDR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRGUI_H
