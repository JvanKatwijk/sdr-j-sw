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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *mfsktextBox;
    QSpinBox *mfskSquelch;
    QComboBox *mfskMode;
    QComboBox *mfskReverse;
    QComboBox *mfskAfcon;
    QLCDNumber *s2nDisplay;
    QLCDNumber *baudrateEstimate;
    QLCDNumber *cfDisplay;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(594, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(460, 10, 121, 21));
        mfsktextBox = new QLabel(frame);
        mfsktextBox->setObjectName(QStringLiteral("mfsktextBox"));
        mfsktextBox->setGeometry(QRect(0, 130, 581, 21));
        mfsktextBox->setFrameShape(QFrame::Box);
        mfskSquelch = new QSpinBox(frame);
        mfskSquelch->setObjectName(QStringLiteral("mfskSquelch"));
        mfskSquelch->setGeometry(QRect(20, 100, 59, 21));
        mfskSquelch->setValue(5);
        mfskMode = new QComboBox(frame);
        mfskMode->setObjectName(QStringLiteral("mfskMode"));
        mfskMode->setGeometry(QRect(20, 10, 85, 21));
        mfskReverse = new QComboBox(frame);
        mfskReverse->setObjectName(QStringLiteral("mfskReverse"));
        mfskReverse->setGeometry(QRect(20, 30, 85, 21));
        mfskAfcon = new QComboBox(frame);
        mfskAfcon->setObjectName(QStringLiteral("mfskAfcon"));
        mfskAfcon->setGeometry(QRect(20, 50, 85, 21));
        s2nDisplay = new QLCDNumber(frame);
        s2nDisplay->setObjectName(QStringLiteral("s2nDisplay"));
        s2nDisplay->setGeometry(QRect(140, 12, 64, 21));
        s2nDisplay->setSegmentStyle(QLCDNumber::Flat);
        baudrateEstimate = new QLCDNumber(frame);
        baudrateEstimate->setObjectName(QStringLiteral("baudrateEstimate"));
        baudrateEstimate->setGeometry(QRect(140, 30, 64, 23));
        baudrateEstimate->setSegmentStyle(QLCDNumber::Flat);
        cfDisplay = new QLCDNumber(frame);
        cfDisplay->setObjectName(QStringLiteral("cfDisplay"));
        cfDisplay->setGeometry(QRect(140, 50, 64, 21));
        cfDisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "mfsk decoder", 0));
        mfsktextBox->setText(QApplication::translate("Form", "TextLabel", 0));
        mfskMode->clear();
        mfskMode->insertItems(0, QStringList()
         << QApplication::translate("Form", "mfsk 16", 0)
         << QApplication::translate("Form", "mfsk 8", 0)
         << QApplication::translate("Form", "mfsk 4", 0)
         << QApplication::translate("Form", "mfsk 11", 0)
         << QApplication::translate("Form", "mfsk 22", 0)
         << QApplication::translate("Form", "mfsk 31", 0)
         << QApplication::translate("Form", "mfsk 32", 0)
         << QApplication::translate("Form", "mfsk 64", 0)
        );
        mfskReverse->clear();
        mfskReverse->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        mfskAfcon->clear();
        mfskAfcon->insertItems(0, QStringList()
         << QApplication::translate("Form", "afc off", 0)
         << QApplication::translate("Form", "afc on", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
