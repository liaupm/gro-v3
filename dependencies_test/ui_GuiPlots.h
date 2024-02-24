/********************************************************************************
** Form generated from reading UI file 'GuiPlots.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUIPLOTS_H
#define UI_GUIPLOTS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_GuiPlots
{
public:
    QWidget *centralwidget;
    QCustomPlot *plot;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GuiPlots)
    {
        if (GuiPlots->objectName().isEmpty())
            GuiPlots->setObjectName(QString::fromUtf8("GuiPlots"));
        GuiPlots->resize(800, 600);
        centralwidget = new QWidget(GuiPlots);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        plot = new QCustomPlot(centralwidget);
        plot->setObjectName(QString::fromUtf8("plot"));
        plot->setGeometry(QRect(150, 130, 481, 291));
        GuiPlots->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GuiPlots);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        GuiPlots->setMenuBar(menubar);
        statusbar = new QStatusBar(GuiPlots);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        GuiPlots->setStatusBar(statusbar);

        retranslateUi(GuiPlots);

        QMetaObject::connectSlotsByName(GuiPlots);
    } // setupUi

    void retranslateUi(QMainWindow *GuiPlots)
    {
        GuiPlots->setWindowTitle(QApplication::translate("GuiPlots", "Plots window", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GuiPlots: public Ui_GuiPlots {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUIPLOTS_H
