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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *showState;
    QLCDNumber *lineNumber;
    QLCDNumber *aptFreq;
    QLabel *workText;
    QComboBox *colorSetter;
    QPushButton *skipSetter;
    QComboBox *iocSetter;
    QComboBox *modeSetter;
    QSpinBox *startSetter;
    QSpinBox *stopSetter;
    QSpinBox *carrierSetter;
    QSpinBox *deviationSetter;
    QLabel *slantText;
    QSpinBox *lpmSetter;
    QComboBox *phaseSetter;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *autoContinueButton;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(574, 169);
        frame = new QFrame(Form);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 571, 171));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(440, 110, 121, 41));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        showState = new QLabel(frame);
        showState->setObjectName(QStringLiteral("showState"));
        showState->setGeometry(QRect(440, 40, 111, 31));
        showState->setFrameShape(QFrame::Box);
        lineNumber = new QLCDNumber(frame);
        lineNumber->setObjectName(QStringLiteral("lineNumber"));
        lineNumber->setGeometry(QRect(400, 80, 64, 23));
        lineNumber->setLineWidth(0);
        lineNumber->setSegmentStyle(QLCDNumber::Flat);
        aptFreq = new QLCDNumber(frame);
        aptFreq->setObjectName(QStringLiteral("aptFreq"));
        aptFreq->setGeometry(QRect(320, 80, 64, 23));
        aptFreq->setLineWidth(0);
        aptFreq->setSegmentStyle(QLCDNumber::Flat);
        workText = new QLabel(frame);
        workText->setObjectName(QStringLiteral("workText"));
        workText->setGeometry(QRect(490, 120, 151, 41));
        colorSetter = new QComboBox(frame);
        colorSetter->setObjectName(QStringLiteral("colorSetter"));
        colorSetter->setGeometry(QRect(220, 10, 101, 29));
        skipSetter = new QPushButton(frame);
        skipSetter->setObjectName(QStringLiteral("skipSetter"));
        skipSetter->setGeometry(QRect(450, 10, 101, 31));
        iocSetter = new QComboBox(frame);
        iocSetter->setObjectName(QStringLiteral("iocSetter"));
        iocSetter->setGeometry(QRect(10, 110, 101, 29));
        modeSetter = new QComboBox(frame);
        modeSetter->setObjectName(QStringLiteral("modeSetter"));
        modeSetter->setGeometry(QRect(120, 80, 101, 29));
        startSetter = new QSpinBox(frame);
        startSetter->setObjectName(QStringLiteral("startSetter"));
        startSetter->setGeometry(QRect(10, 10, 91, 21));
        startSetter->setMaximum(1000);
        startSetter->setValue(300);
        stopSetter = new QSpinBox(frame);
        stopSetter->setObjectName(QStringLiteral("stopSetter"));
        stopSetter->setGeometry(QRect(10, 30, 91, 21));
        stopSetter->setMaximum(1000);
        stopSetter->setValue(450);
        carrierSetter = new QSpinBox(frame);
        carrierSetter->setObjectName(QStringLiteral("carrierSetter"));
        carrierSetter->setGeometry(QRect(10, 90, 91, 21));
        carrierSetter->setMaximum(2500);
        carrierSetter->setValue(1900);
        deviationSetter = new QSpinBox(frame);
        deviationSetter->setObjectName(QStringLiteral("deviationSetter"));
        deviationSetter->setGeometry(QRect(10, 50, 91, 21));
        deviationSetter->setMaximum(1000);
        deviationSetter->setValue(400);
        slantText = new QLabel(frame);
        slantText->setObjectName(QStringLiteral("slantText"));
        slantText->setGeometry(QRect(460, 130, 151, 21));
        lpmSetter = new QSpinBox(frame);
        lpmSetter->setObjectName(QStringLiteral("lpmSetter"));
        lpmSetter->setGeometry(QRect(10, 70, 91, 21));
        lpmSetter->setMaximum(200);
        lpmSetter->setValue(120);
        phaseSetter = new QComboBox(frame);
        phaseSetter->setObjectName(QStringLiteral("phaseSetter"));
        phaseSetter->setGeometry(QRect(120, 10, 101, 29));
        resetButton = new QPushButton(frame);
        resetButton->setObjectName(QStringLiteral("resetButton"));
        resetButton->setGeometry(QRect(220, 80, 97, 31));
        saveButton = new QPushButton(frame);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        saveButton->setGeometry(QRect(120, 40, 131, 31));
        autoContinueButton = new QPushButton(frame);
        autoContinueButton->setObjectName(QStringLiteral("autoContinueButton"));
        autoContinueButton->setGeometry(QRect(260, 40, 151, 31));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        label->setText(QApplication::translate("Form", "fax decoder", 0));
        showState->setText(QString());
        workText->setText(QString());
        colorSetter->clear();
        colorSetter->insertItems(0, QStringList()
         << QApplication::translate("Form", "BW", 0)
         << QApplication::translate("Form", "GRAY", 0)
         << QApplication::translate("Form", "COLOR", 0)
        );
        skipSetter->setText(QApplication::translate("Form", "skip Mode", 0));
        iocSetter->clear();
        iocSetter->insertItems(0, QStringList()
         << QApplication::translate("Form", "Wefax576", 0)
         << QApplication::translate("Form", "Wefax288", 0)
         << QApplication::translate("Form", "HamColor", 0)
         << QApplication::translate("Form", "Ham288b", 0)
         << QApplication::translate("Form", "Color240", 0)
         << QApplication::translate("Form", "FAX480", 0)
        );
        modeSetter->clear();
        modeSetter->insertItems(0, QStringList()
         << QApplication::translate("Form", "FM", 0)
         << QApplication::translate("Form", "AM", 0)
        );
        slantText->setText(QString());
        phaseSetter->clear();
        phaseSetter->insertItems(0, QStringList()
         << QApplication::translate("Form", "phase", 0)
         << QApplication::translate("Form", "inverse", 0)
        );
        resetButton->setText(QApplication::translate("Form", "reset", 0));
        saveButton->setText(QApplication::translate("Form", "save on Done", 0));
        autoContinueButton->setText(QApplication::translate("Form", "continue", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
