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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QComboBox *analogDecoderSelect;
    QScrollBar *adaptiveFilterslider;
    QPushButton *adaptiveFilterButton;
    QLabel *analogMode;
    QLCDNumber *signalStrength;
    QScrollBar *squelchSlider;
    QLCDNumber *showIF;
    QLabel *label;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(599, 158);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        analogDecoderSelect = new QComboBox(frame);
        analogDecoderSelect->setObjectName(QStringLiteral("analogDecoderSelect"));
        analogDecoderSelect->setGeometry(QRect(20, 10, 101, 21));
        adaptiveFilterslider = new QScrollBar(frame);
        adaptiveFilterslider->setObjectName(QStringLiteral("adaptiveFilterslider"));
        adaptiveFilterslider->setGeometry(QRect(260, 10, 161, 16));
        adaptiveFilterslider->setMinimum(2);
        adaptiveFilterslider->setMaximum(20);
        adaptiveFilterslider->setValue(3);
        adaptiveFilterslider->setSliderPosition(3);
        adaptiveFilterslider->setOrientation(Qt::Horizontal);
        adaptiveFilterButton = new QPushButton(frame);
        adaptiveFilterButton->setObjectName(QStringLiteral("adaptiveFilterButton"));
        adaptiveFilterButton->setGeometry(QRect(160, 10, 81, 21));
        analogMode = new QLabel(frame);
        analogMode->setObjectName(QStringLiteral("analogMode"));
        analogMode->setGeometry(QRect(20, 100, 161, 21));
        signalStrength = new QLCDNumber(frame);
        signalStrength->setObjectName(QStringLiteral("signalStrength"));
        signalStrength->setGeometry(QRect(20, 40, 64, 23));
        signalStrength->setSegmentStyle(QLCDNumber::Flat);
        squelchSlider = new QScrollBar(frame);
        squelchSlider->setObjectName(QStringLiteral("squelchSlider"));
        squelchSlider->setGeometry(QRect(260, 40, 160, 16));
        squelchSlider->setMinimum(2);
        squelchSlider->setMaximum(200);
        squelchSlider->setValue(15);
        squelchSlider->setOrientation(Qt::Horizontal);
        showIF = new QLCDNumber(frame);
        showIF->setObjectName(QStringLiteral("showIF"));
        showIF->setGeometry(QRect(20, 80, 64, 23));
        showIF->setSegmentStyle(QLCDNumber::Flat);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(450, 10, 111, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        analogDecoderSelect->clear();
        analogDecoderSelect->insertItems(0, QStringList()
         << QApplication::translate("Form", "am", 0)
         << QApplication::translate("Form", "usb", 0)
         << QApplication::translate("Form", "lsb", 0)
         << QApplication::translate("Form", "isb", 0)
         << QApplication::translate("Form", "fm", 0)
         << QApplication::translate("Form", "coherent", 0)
        );
        adaptiveFilterButton->setText(QApplication::translate("Form", "filter", 0));
        analogMode->setText(QString());
        label->setText(QApplication::translate("Form", "analog decoder", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
