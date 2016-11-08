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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLCDNumber *rttyStrengthMeter;
    QLCDNumber *rttyFreqCorrection;
    QLCDNumber *rttyBaudRate;
    QLCDNumber *rttyGuess;
    QLabel *rttytextBox;
    QComboBox *rttyWidthSelect;
    QComboBox *rttyBaudrateSelect;
    QComboBox *rttyParitySelect;
    QComboBox *rttyMsbSelect;
    QComboBox *rttyAfconTrigger;
    QComboBox *rttyReverseTrigger;
    QComboBox *rttyNbitsTrigger;
    QComboBox *rttyStopbitsTrigger;
    QLabel *label;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(596, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        rttyStrengthMeter = new QLCDNumber(frame);
        rttyStrengthMeter->setObjectName(QStringLiteral("rttyStrengthMeter"));
        rttyStrengthMeter->setGeometry(QRect(20, 10, 64, 23));
        rttyStrengthMeter->setSegmentStyle(QLCDNumber::Flat);
        rttyFreqCorrection = new QLCDNumber(frame);
        rttyFreqCorrection->setObjectName(QStringLiteral("rttyFreqCorrection"));
        rttyFreqCorrection->setGeometry(QRect(20, 30, 64, 23));
        rttyFreqCorrection->setSegmentStyle(QLCDNumber::Flat);
        rttyBaudRate = new QLCDNumber(frame);
        rttyBaudRate->setObjectName(QStringLiteral("rttyBaudRate"));
        rttyBaudRate->setGeometry(QRect(20, 50, 64, 23));
        rttyBaudRate->setSegmentStyle(QLCDNumber::Flat);
        rttyGuess = new QLCDNumber(frame);
        rttyGuess->setObjectName(QStringLiteral("rttyGuess"));
        rttyGuess->setGeometry(QRect(20, 70, 64, 23));
        rttyGuess->setSegmentStyle(QLCDNumber::Flat);
        rttytextBox = new QLabel(frame);
        rttytextBox->setObjectName(QStringLiteral("rttytextBox"));
        rttytextBox->setGeometry(QRect(20, 130, 571, 21));
        rttytextBox->setFrameShape(QFrame::Box);
        rttyWidthSelect = new QComboBox(frame);
        rttyWidthSelect->setObjectName(QStringLiteral("rttyWidthSelect"));
        rttyWidthSelect->setGeometry(QRect(140, 10, 91, 21));
        rttyBaudrateSelect = new QComboBox(frame);
        rttyBaudrateSelect->setObjectName(QStringLiteral("rttyBaudrateSelect"));
        rttyBaudrateSelect->setGeometry(QRect(140, 30, 91, 21));
        rttyParitySelect = new QComboBox(frame);
        rttyParitySelect->setObjectName(QStringLiteral("rttyParitySelect"));
        rttyParitySelect->setGeometry(QRect(140, 50, 91, 21));
        rttyMsbSelect = new QComboBox(frame);
        rttyMsbSelect->setObjectName(QStringLiteral("rttyMsbSelect"));
        rttyMsbSelect->setGeometry(QRect(140, 70, 91, 21));
        rttyAfconTrigger = new QComboBox(frame);
        rttyAfconTrigger->setObjectName(QStringLiteral("rttyAfconTrigger"));
        rttyAfconTrigger->setGeometry(QRect(250, 10, 85, 21));
        rttyReverseTrigger = new QComboBox(frame);
        rttyReverseTrigger->setObjectName(QStringLiteral("rttyReverseTrigger"));
        rttyReverseTrigger->setGeometry(QRect(250, 30, 85, 21));
        rttyNbitsTrigger = new QComboBox(frame);
        rttyNbitsTrigger->setObjectName(QStringLiteral("rttyNbitsTrigger"));
        rttyNbitsTrigger->setGeometry(QRect(250, 50, 85, 21));
        rttyStopbitsTrigger = new QComboBox(frame);
        rttyStopbitsTrigger->setObjectName(QStringLiteral("rttyStopbitsTrigger"));
        rttyStopbitsTrigger->setGeometry(QRect(250, 70, 85, 21));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(470, 10, 111, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        rttytextBox->setText(QApplication::translate("Form", "TextLabel", 0));
        rttyWidthSelect->clear();
        rttyWidthSelect->insertItems(0, QStringList()
         << QApplication::translate("Form", "170", 0)
         << QApplication::translate("Form", "300", 0)
         << QApplication::translate("Form", "450", 0)
         << QApplication::translate("Form", "600", 0)
         << QApplication::translate("Form", "850", 0)
         << QApplication::translate("Form", "900", 0)
         << QApplication::translate("Form", "1200", 0)
        );
        rttyBaudrateSelect->clear();
        rttyBaudrateSelect->insertItems(0, QStringList()
         << QApplication::translate("Form", "45", 0)
         << QApplication::translate("Form", "50", 0)
         << QApplication::translate("Form", "75", 0)
         << QApplication::translate("Form", "100", 0)
         << QApplication::translate("Form", "300", 0)
        );
        rttyParitySelect->clear();
        rttyParitySelect->insertItems(0, QStringList()
         << QApplication::translate("Form", "par none", 0)
         << QApplication::translate("Form", "par one", 0)
         << QApplication::translate("Form", "par odd", 0)
         << QApplication::translate("Form", "par even", 0)
         << QApplication::translate("Form", "par zero", 0)
        );
        rttyMsbSelect->clear();
        rttyMsbSelect->insertItems(0, QStringList()
         << QApplication::translate("Form", "msb false", 0)
         << QApplication::translate("Form", "msb true", 0)
        );
        rttyAfconTrigger->clear();
        rttyAfconTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "afc off", 0)
         << QApplication::translate("Form", "afc on", 0)
        );
        rttyReverseTrigger->clear();
        rttyReverseTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        rttyNbitsTrigger->clear();
        rttyNbitsTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "5 bits", 0)
         << QApplication::translate("Form", "7 bits", 0)
         << QApplication::translate("Form", "8 bits", 0)
        );
        rttyStopbitsTrigger->clear();
        rttyStopbitsTrigger->insertItems(0, QStringList()
         << QApplication::translate("Form", "1 stop", 0)
         << QApplication::translate("Form", "1.5 stop", 0)
         << QApplication::translate("Form", "2 stop", 0)
        );
        label->setText(QApplication::translate("Form", "rtty decoder", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
