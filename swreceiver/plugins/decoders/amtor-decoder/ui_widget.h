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
    QLabel *amtortextBox;
    QLCDNumber *amtorFreqCorrection;
    QLCDNumber *amtorStrengthMeter;
    QComboBox *amtorAfcon;
    QComboBox *amtorReverse;
    QComboBox *amtorFecError;
    QComboBox *amtorMessage;
    QLabel *label;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(593, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        amtortextBox = new QLabel(frame);
        amtortextBox->setObjectName(QStringLiteral("amtortextBox"));
        amtortextBox->setGeometry(QRect(10, 130, 571, 21));
        amtortextBox->setFrameShape(QFrame::Box);
        amtortextBox->setFrameShadow(QFrame::Plain);
        amtorFreqCorrection = new QLCDNumber(frame);
        amtorFreqCorrection->setObjectName(QStringLiteral("amtorFreqCorrection"));
        amtorFreqCorrection->setGeometry(QRect(10, 10, 64, 23));
        amtorFreqCorrection->setSegmentStyle(QLCDNumber::Flat);
        amtorStrengthMeter = new QLCDNumber(frame);
        amtorStrengthMeter->setObjectName(QStringLiteral("amtorStrengthMeter"));
        amtorStrengthMeter->setGeometry(QRect(10, 30, 64, 23));
        amtorStrengthMeter->setSegmentStyle(QLCDNumber::Flat);
        amtorAfcon = new QComboBox(frame);
        amtorAfcon->setObjectName(QStringLiteral("amtorAfcon"));
        amtorAfcon->setGeometry(QRect(150, 10, 85, 21));
        amtorReverse = new QComboBox(frame);
        amtorReverse->setObjectName(QStringLiteral("amtorReverse"));
        amtorReverse->setGeometry(QRect(150, 30, 85, 21));
        amtorFecError = new QComboBox(frame);
        amtorFecError->setObjectName(QStringLiteral("amtorFecError"));
        amtorFecError->setGeometry(QRect(150, 50, 85, 21));
        amtorMessage = new QComboBox(frame);
        amtorMessage->setObjectName(QStringLiteral("amtorMessage"));
        amtorMessage->setGeometry(QRect(150, 70, 85, 21));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(470, 10, 67, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        amtortextBox->setText(QApplication::translate("Form", "TextLabel", 0));
        amtorAfcon->clear();
        amtorAfcon->insertItems(0, QStringList()
         << QApplication::translate("Form", "afc off", 0)
         << QApplication::translate("Form", "afc on", 0)
        );
        amtorReverse->clear();
        amtorReverse->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        amtorFecError->clear();
        amtorFecError->insertItems(0, QStringList()
         << QApplication::translate("Form", "non strict", 0)
         << QApplication::translate("Form", "strict", 0)
        );
        amtorMessage->clear();
        amtorMessage->insertItems(0, QStringList()
         << QApplication::translate("Form", "all text", 0)
         << QApplication::translate("Form", "message", 0)
        );
        label->setText(QApplication::translate("Form", "amtor", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
