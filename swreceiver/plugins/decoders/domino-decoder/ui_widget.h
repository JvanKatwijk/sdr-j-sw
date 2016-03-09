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
    QLCDNumber *domino_showS2N;
    QComboBox *dominoMode;
    QComboBox *dominoReverse;
    QComboBox *dominoFec;
    QSpinBox *dominoSquelch;
    QLabel *dominotextBox;
    QLabel *dominotextBox2;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(591, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(460, 10, 131, 21));
        domino_showS2N = new QLCDNumber(frame);
        domino_showS2N->setObjectName(QStringLiteral("domino_showS2N"));
        domino_showS2N->setGeometry(QRect(20, 10, 64, 23));
        domino_showS2N->setSegmentStyle(QLCDNumber::Flat);
        dominoMode = new QComboBox(frame);
        dominoMode->setObjectName(QStringLiteral("dominoMode"));
        dominoMode->setGeometry(QRect(20, 30, 85, 21));
        dominoReverse = new QComboBox(frame);
        dominoReverse->setObjectName(QStringLiteral("dominoReverse"));
        dominoReverse->setGeometry(QRect(20, 50, 85, 21));
        dominoFec = new QComboBox(frame);
        dominoFec->setObjectName(QStringLiteral("dominoFec"));
        dominoFec->setGeometry(QRect(20, 70, 85, 21));
        dominoSquelch = new QSpinBox(frame);
        dominoSquelch->setObjectName(QStringLiteral("dominoSquelch"));
        dominoSquelch->setGeometry(QRect(20, 90, 81, 21));
        dominotextBox = new QLabel(frame);
        dominotextBox->setObjectName(QStringLiteral("dominotextBox"));
        dominotextBox->setGeometry(QRect(10, 130, 571, 21));
        dominotextBox->setFrameShape(QFrame::Box);
        dominotextBox2 = new QLabel(frame);
        dominotextBox2->setObjectName(QStringLiteral("dominotextBox2"));
        dominotextBox2->setGeometry(QRect(380, 90, 201, 21));
        dominotextBox2->setFrameShape(QFrame::Box);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "domino decoder", 0));
        dominoMode->clear();
        dominoMode->insertItems(0, QStringList()
         << QApplication::translate("Form", "domino8", 0)
         << QApplication::translate("Form", "domino5", 0)
         << QApplication::translate("Form", "domino4", 0)
         << QApplication::translate("Form", "domino11", 0)
         << QApplication::translate("Form", "domino16", 0)
         << QApplication::translate("Form", "domino22", 0)
        );
        dominoReverse->clear();
        dominoReverse->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        dominoFec->clear();
        dominoFec->insertItems(0, QStringList()
         << QApplication::translate("Form", "no fec", 0)
         << QApplication::translate("Form", "fec", 0)
        );
        dominotextBox->setText(QApplication::translate("Form", "TextLabel", 0));
        dominotextBox2->setText(QApplication::translate("Form", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
