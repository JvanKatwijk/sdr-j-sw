/********************************************************************************
** Form generated from reading UI file 'configurator.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURATOR_H
#define UI_CONFIGURATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *devices;
    QPushButton *decoders;
    QLabel *deviceDirectory;
    QLabel *decoderDirectory;
    QLabel *label;
    QPushButton *saveButton;
    QPushButton *quitButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(618, 300);
        devices = new QPushButton(Dialog);
        devices->setObjectName(QStringLiteral("devices"));
        devices->setGeometry(QRect(50, 70, 95, 31));
        decoders = new QPushButton(Dialog);
        decoders->setObjectName(QStringLiteral("decoders"));
        decoders->setGeometry(QRect(50, 150, 95, 31));
        deviceDirectory = new QLabel(Dialog);
        deviceDirectory->setObjectName(QStringLiteral("deviceDirectory"));
        deviceDirectory->setGeometry(QRect(50, 110, 541, 31));
        QFont font;
        font.setPointSize(10);
        deviceDirectory->setFont(font);
        deviceDirectory->setFrameShape(QFrame::Panel);
        decoderDirectory = new QLabel(Dialog);
        decoderDirectory->setObjectName(QStringLiteral("decoderDirectory"));
        decoderDirectory->setGeometry(QRect(50, 190, 541, 31));
        decoderDirectory->setFont(font);
        decoderDirectory->setFrameShape(QFrame::Panel);
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 20, 321, 31));
        saveButton = new QPushButton(Dialog);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        saveButton->setGeometry(QRect(280, 230, 95, 31));
        quitButton = new QPushButton(Dialog);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setGeometry(QRect(280, 20, 95, 51));
        QPalette palette;
        QBrush brush(QColor(255, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        quitButton->setPalette(palette);
        quitButton->setAutoFillBackground(true);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0));
        devices->setText(QApplication::translate("Dialog", "devices", 0));
        decoders->setText(QApplication::translate("Dialog", "decoder", 0));
        deviceDirectory->setText(QString());
        decoderDirectory->setText(QString());
        label->setText(QApplication::translate("Dialog", "jff-5.0 configurator", 0));
        saveButton->setText(QApplication::translate("Dialog", "save", 0));
        quitButton->setText(QApplication::translate("Dialog", "QUIT", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURATOR_H
