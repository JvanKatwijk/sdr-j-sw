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
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QComboBox *oliviaTones;
    QComboBox *oliviaBW;
    QSpinBox *oliviaSyncMargin;
    QSpinBox *oliviaSyncIntegLen;
    QComboBox *oliviaSquelch;
    QSpinBox *oliviasquelchValue;
    QComboBox *oliviaReverse;
    QLabel *oliviatextBox;
    QLabel *olivia_status2_textBox;
    QLabel *olivia_status1_textBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(566, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        oliviaTones = new QComboBox(frame);
        oliviaTones->setObjectName(QStringLiteral("oliviaTones"));
        oliviaTones->setGeometry(QRect(10, 10, 91, 21));
        oliviaBW = new QComboBox(frame);
        oliviaBW->setObjectName(QStringLiteral("oliviaBW"));
        oliviaBW->setGeometry(QRect(10, 30, 91, 21));
        oliviaSyncMargin = new QSpinBox(frame);
        oliviaSyncMargin->setObjectName(QStringLiteral("oliviaSyncMargin"));
        oliviaSyncMargin->setGeometry(QRect(10, 50, 91, 21));
        oliviaSyncMargin->setValue(4);
        oliviaSyncIntegLen = new QSpinBox(frame);
        oliviaSyncIntegLen->setObjectName(QStringLiteral("oliviaSyncIntegLen"));
        oliviaSyncIntegLen->setGeometry(QRect(10, 70, 91, 21));
        oliviaSyncIntegLen->setValue(8);
        oliviaSquelch = new QComboBox(frame);
        oliviaSquelch->setObjectName(QStringLiteral("oliviaSquelch"));
        oliviaSquelch->setGeometry(QRect(210, 30, 111, 21));
        oliviasquelchValue = new QSpinBox(frame);
        oliviasquelchValue->setObjectName(QStringLiteral("oliviasquelchValue"));
        oliviasquelchValue->setGeometry(QRect(210, 50, 111, 21));
        oliviasquelchValue->setValue(5);
        oliviaReverse = new QComboBox(frame);
        oliviaReverse->setObjectName(QStringLiteral("oliviaReverse"));
        oliviaReverse->setGeometry(QRect(210, 10, 111, 21));
        oliviatextBox = new QLabel(frame);
        oliviatextBox->setObjectName(QStringLiteral("oliviatextBox"));
        oliviatextBox->setGeometry(QRect(10, 120, 541, 21));
        oliviatextBox->setFrameShape(QFrame::Box);
        olivia_status2_textBox = new QLabel(frame);
        olivia_status2_textBox->setObjectName(QStringLiteral("olivia_status2_textBox"));
        olivia_status2_textBox->setGeometry(QRect(350, 60, 161, 21));
        olivia_status2_textBox->setFrameShape(QFrame::Box);
        olivia_status1_textBox = new QLabel(frame);
        olivia_status1_textBox->setObjectName(QStringLiteral("olivia_status1_textBox"));
        olivia_status1_textBox->setGeometry(QRect(350, 30, 161, 21));
        olivia_status1_textBox->setFrameShape(QFrame::Box);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(350, 10, 161, 21));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(110, 10, 67, 21));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(110, 30, 81, 21));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(110, 50, 81, 21));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(110, 70, 67, 21));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(220, 70, 101, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        oliviaTones->clear();
        oliviaTones->insertItems(0, QStringList()
         << QApplication::translate("Form", "32", 0)
         << QApplication::translate("Form", "16", 0)
         << QApplication::translate("Form", "128", 0)
         << QApplication::translate("Form", "64", 0)
         << QApplication::translate("Form", "8", 0)
         << QApplication::translate("Form", "4", 0)
        );
        oliviaBW->clear();
        oliviaBW->insertItems(0, QStringList()
         << QApplication::translate("Form", "1000", 0)
         << QApplication::translate("Form", "500", 0)
         << QApplication::translate("Form", "250", 0)
         << QApplication::translate("Form", "125", 0)
         << QApplication::translate("Form", "2000", 0)
        );
        oliviaSquelch->clear();
        oliviaSquelch->insertItems(0, QStringList()
         << QApplication::translate("Form", "squelch off", 0)
         << QApplication::translate("Form", "squelch on", 0)
        );
        oliviaReverse->clear();
        oliviaReverse->insertItems(0, QStringList()
         << QApplication::translate("Form", "normal", 0)
         << QApplication::translate("Form", "reverse", 0)
        );
        oliviatextBox->setText(QApplication::translate("Form", "TextLabel", 0));
        olivia_status2_textBox->setText(QApplication::translate("Form", "TextLabel", 0));
        olivia_status1_textBox->setText(QApplication::translate("Form", "TextLabel", 0));
        label->setText(QApplication::translate("Form", "Oliviadecoder", 0));
        label_2->setText(QApplication::translate("Form", "Tones", 0));
        label_3->setText(QApplication::translate("Form", "Bandwidth", 0));
        label_4->setText(QApplication::translate("Form", "syncMargin", 0));
        label_5->setText(QApplication::translate("Form", "syncInteg", 0));
        label_6->setText(QApplication::translate("Form", "squelch value", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
