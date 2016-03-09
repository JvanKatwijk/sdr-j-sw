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
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLCDNumber *metricsDisplay;
    QLabel *label;
    QComboBox *hell_modeSelector;
    QScrollArea *hellText;
    QWidget *scrollAreaWidgetContents;
    QPushButton *hell_switchBlackback;
    QPushButton *hell_switchWidth;
    QSpinBox *hell_rateCorrector;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(568, 166);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 661, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        metricsDisplay = new QLCDNumber(frame);
        metricsDisplay->setObjectName(QStringLiteral("metricsDisplay"));
        metricsDisplay->setGeometry(QRect(120, 20, 64, 23));
        metricsDisplay->setSegmentStyle(QLCDNumber::Flat);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 130, 91, 21));
        hell_modeSelector = new QComboBox(frame);
        hell_modeSelector->setObjectName(QStringLiteral("hell_modeSelector"));
        hell_modeSelector->setGeometry(QRect(30, 20, 85, 21));
        hellText = new QScrollArea(frame);
        hellText->setObjectName(QStringLiteral("hellText"));
        hellText->setGeometry(QRect(30, 60, 521, 71));
        hellText->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 519, 69));
        hellText->setWidget(scrollAreaWidgetContents);
        hell_switchBlackback = new QPushButton(frame);
        hell_switchBlackback->setObjectName(QStringLiteral("hell_switchBlackback"));
        hell_switchBlackback->setGeometry(QRect(200, 20, 95, 21));
        hell_switchWidth = new QPushButton(frame);
        hell_switchWidth->setObjectName(QStringLiteral("hell_switchWidth"));
        hell_switchWidth->setGeometry(QRect(300, 20, 95, 21));
        hell_rateCorrector = new QSpinBox(frame);
        hell_rateCorrector->setObjectName(QStringLiteral("hell_rateCorrector"));
        hell_rateCorrector->setGeometry(QRect(410, 20, 59, 21));
        hell_rateCorrector->setMinimum(-100);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "hell decoder", 0));
        hell_modeSelector->clear();
        hell_modeSelector->insertItems(0, QStringList()
         << QApplication::translate("Form", "feldhell", 0)
         << QApplication::translate("Form", "slowhell", 0)
         << QApplication::translate("Form", "hellx5", 0)
         << QApplication::translate("Form", "hellx9", 0)
         << QApplication::translate("Form", "fskhell", 0)
         << QApplication::translate("Form", "fskh105", 0)
         << QApplication::translate("Form", "fskh245", 0)
         << QApplication::translate("Form", "hell80", 0)
        );
        hell_switchBlackback->setText(QApplication::translate("Form", "background", 0));
        hell_switchWidth->setText(QApplication::translate("Form", "width", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
