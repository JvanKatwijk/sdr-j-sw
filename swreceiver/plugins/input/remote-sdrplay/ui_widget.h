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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *label;
    QSpinBox *ppmControl;
    QComboBox *rateSelector;
    QLabel *state;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QLabel *connectedLabel;
    QLineEdit *hostLineEdit;
    QSlider *gainSlider;
    QLCDNumber *gainDisplay;
    QCheckBox *agcControl;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(162, 223);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 161, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 190, 111, 21));
        ppmControl = new QSpinBox(frame);
        ppmControl->setObjectName(QStringLiteral("ppmControl"));
        ppmControl->setGeometry(QRect(10, 10, 101, 21));
        ppmControl->setMinimum(-99);
        ppmControl->setMaximum(99);
        rateSelector = new QComboBox(frame);
        rateSelector->setObjectName(QStringLiteral("rateSelector"));
        rateSelector->setGeometry(QRect(10, 90, 101, 21));
        state = new QLabel(frame);
        state->setObjectName(QStringLiteral("state"));
        state->setGeometry(QRect(10, 171, 101, 20));
        connectButton = new QPushButton(frame);
        connectButton->setObjectName(QStringLiteral("connectButton"));
        connectButton->setGeometry(QRect(10, 30, 101, 21));
        disconnectButton = new QPushButton(frame);
        disconnectButton->setObjectName(QStringLiteral("disconnectButton"));
        disconnectButton->setGeometry(QRect(10, 70, 101, 21));
        connectedLabel = new QLabel(frame);
        connectedLabel->setObjectName(QStringLiteral("connectedLabel"));
        connectedLabel->setGeometry(QRect(10, 120, 91, 20));
        hostLineEdit = new QLineEdit(frame);
        hostLineEdit->setObjectName(QStringLiteral("hostLineEdit"));
        hostLineEdit->setGeometry(QRect(10, 50, 101, 21));
        QFont font;
        font.setPointSize(10);
        hostLineEdit->setFont(font);
        gainSlider = new QSlider(frame);
        gainSlider->setObjectName(QStringLiteral("gainSlider"));
        gainSlider->setGeometry(QRect(130, 40, 18, 160));
        gainSlider->setOrientation(Qt::Vertical);
        gainDisplay = new QLCDNumber(frame);
        gainDisplay->setObjectName(QStringLiteral("gainDisplay"));
        gainDisplay->setGeometry(QRect(113, 10, 41, 23));
        gainDisplay->setDigitCount(3);
        gainDisplay->setSegmentStyle(QLCDNumber::Flat);
        agcControl = new QCheckBox(frame);
        agcControl->setObjectName(QStringLiteral("agcControl"));
        agcControl->setGeometry(QRect(80, 150, 51, 25));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "remote sdrplay", 0));
        rateSelector->clear();
        rateSelector->insertItems(0, QStringList()
         << QApplication::translate("Form", "96000", 0)
         << QApplication::translate("Form", "48000", 0)
         << QApplication::translate("Form", "192000", 0)
         << QApplication::translate("Form", "256000", 0)
         << QApplication::translate("Form", "384000", 0)
         << QApplication::translate("Form", "512000", 0)
        );
        state->setText(QString());
        connectButton->setText(QApplication::translate("Form", "connect", 0));
        disconnectButton->setText(QApplication::translate("Form", "disconnect", 0));
        connectedLabel->setText(QString());
        agcControl->setText(QApplication::translate("Form", "agc", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
