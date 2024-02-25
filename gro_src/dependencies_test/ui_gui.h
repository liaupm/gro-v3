/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Gui
{
public:
    QAction *actionStartStop;
    QAction *actionOpen;
    QAction *actionReload;
    QAction *actionReloadSeed;
    QAction *actionStep;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionScreenshot;
    QAction *actionMoreCells;
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionResetZoom;
    QAction *actionDump;
    QAction *actionCenter;
    QAction *actionAutoZoom;
    QAction *action10steps;
    QAction *action100steps;
    QAction *action1step;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSimulation;
    QMenu *menuHelp;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Gui)
    {
        if (Gui->objectName().isEmpty())
            Gui->setObjectName(QString::fromUtf8("Gui"));
        Gui->resize(851, 900);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Gui->sizePolicy().hasHeightForWidth());
        Gui->setSizePolicy(sizePolicy);
        actionStartStop = new QAction(Gui);
        actionStartStop->setObjectName(QString::fromUtf8("actionStartStop"));
        actionStartStop->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/start.png"), QSize(), QIcon::Normal, QIcon::On);
        actionStartStop->setIcon(icon);
        actionStartStop->setIconVisibleInMenu(false);
        actionOpen = new QAction(Gui);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionOpen->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/open.png"), QSize(), QIcon::Normal, QIcon::On);
        actionOpen->setIcon(icon1);
        actionOpen->setIconVisibleInMenu(false);
        actionReload = new QAction(Gui);
        actionReload->setObjectName(QString::fromUtf8("actionReload"));
        actionReload->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/reload.png"), QSize(), QIcon::Normal, QIcon::On);
        actionReload->setIcon(icon2);
        actionReload->setIconVisibleInMenu(false);
        actionReloadSeed = new QAction(Gui);
        actionReloadSeed->setObjectName(QString::fromUtf8("actionReloadSeed"));
        actionReloadSeed->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/reloadSeed.png"), QSize(), QIcon::Normal, QIcon::On);
        actionReloadSeed->setIcon(icon3);
        actionReloadSeed->setIconVisibleInMenu(false);
        actionStep = new QAction(Gui);
        actionStep->setObjectName(QString::fromUtf8("actionStep"));
        actionStep->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/icons/step.png"), QSize(), QIcon::Normal, QIcon::On);
        actionStep->setIcon(icon4);
        actionStep->setIconVisibleInMenu(false);
        actionHelp = new QAction(Gui);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAbout = new QAction(Gui);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionScreenshot = new QAction(Gui);
        actionScreenshot->setObjectName(QString::fromUtf8("actionScreenshot"));
        actionScreenshot->setEnabled(false);
        actionMoreCells = new QAction(Gui);
        actionMoreCells->setObjectName(QString::fromUtf8("actionMoreCells"));
        actionZoomIn = new QAction(Gui);
        actionZoomIn->setObjectName(QString::fromUtf8("actionZoomIn"));
        actionZoomIn->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/icons/zoom_in.png"), QSize(), QIcon::Normal, QIcon::On);
        actionZoomIn->setIcon(icon5);
        actionZoomIn->setIconVisibleInMenu(false);
        actionZoomOut = new QAction(Gui);
        actionZoomOut->setObjectName(QString::fromUtf8("actionZoomOut"));
        actionZoomOut->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/icons/zoom_out.png"), QSize(), QIcon::Normal, QIcon::On);
        actionZoomOut->setIcon(icon6);
        actionZoomOut->setIconVisibleInMenu(false);
        actionResetZoom = new QAction(Gui);
        actionResetZoom->setObjectName(QString::fromUtf8("actionResetZoom"));
        actionDump = new QAction(Gui);
        actionDump->setObjectName(QString::fromUtf8("actionDump"));
        actionCenter = new QAction(Gui);
        actionCenter->setObjectName(QString::fromUtf8("actionCenter"));
        actionCenter->setCheckable(false);
        actionCenter->setEnabled(false);
        QIcon icon7;
        QString iconThemeName = QString::fromUtf8("center.png");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon7 = QIcon::fromTheme(iconThemeName);
        } else {
            icon7.addFile(QString::fromUtf8(":/icons/icons/center.png"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionCenter->setIcon(icon7);
        actionCenter->setIconVisibleInMenu(false);
        actionAutoZoom = new QAction(Gui);
        actionAutoZoom->setObjectName(QString::fromUtf8("actionAutoZoom"));
        actionAutoZoom->setCheckable(true);
        actionAutoZoom->setChecked(false);
        actionAutoZoom->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/icons/zoom_auto.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAutoZoom->setIcon(icon8);
        action10steps = new QAction(Gui);
        action10steps->setObjectName(QString::fromUtf8("action10steps"));
        action10steps->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/icons/x10.png"), QSize(), QIcon::Normal, QIcon::Off);
        action10steps->setIcon(icon9);
        action100steps = new QAction(Gui);
        action100steps->setObjectName(QString::fromUtf8("action100steps"));
        action100steps->setEnabled(false);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/icons/x100.png"), QSize(), QIcon::Normal, QIcon::Off);
        action100steps->setIcon(icon10);
        action1step = new QAction(Gui);
        action1step->setObjectName(QString::fromUtf8("action1step"));
        action1step->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/icons/x1.png"), QSize(), QIcon::Normal, QIcon::Off);
        action1step->setIcon(icon11);
        centralWidget = new QWidget(Gui);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        Gui->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Gui);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 851, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuSimulation = new QMenu(menuBar);
        menuSimulation->setObjectName(QString::fromUtf8("menuSimulation"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        Gui->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Gui);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
        mainToolBar->setSizePolicy(sizePolicy1);
        mainToolBar->setAcceptDrops(false);
        Gui->addToolBar(Qt::LeftToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Gui);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        Gui->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSimulation->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionReload);
        menuFile->addAction(actionScreenshot);
        menuFile->addAction(actionDump);
        menuSimulation->addAction(actionStartStop);
        menuSimulation->addAction(actionStep);
        menuSimulation->addAction(actionMoreCells);
        menuHelp->addAction(actionHelp);
        menuHelp->addAction(actionAbout);
        menuView->addAction(actionZoomIn);
        menuView->addAction(actionZoomOut);
        menuView->addAction(actionResetZoom);
        menuView->addAction(actionCenter);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionReload);
        mainToolBar->addAction(actionReloadSeed);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionStartStop);
        mainToolBar->addAction(actionStep);
        mainToolBar->addAction(action1step);
        mainToolBar->addAction(action10steps);
        mainToolBar->addAction(action100steps);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionZoomIn);
        mainToolBar->addAction(actionZoomOut);
        mainToolBar->addAction(actionAutoZoom);
        mainToolBar->addAction(actionCenter);
        mainToolBar->addSeparator();

        retranslateUi(Gui);
        QObject::connect(actionStartStop, SIGNAL(triggered()), Gui, SLOT(startStop()));
        QObject::connect(actionOpen, SIGNAL(triggered()), Gui, SLOT(open()));
        QObject::connect(actionReload, SIGNAL(triggered()), Gui, SLOT(reload()));
        QObject::connect(actionReloadSeed, SIGNAL(triggered()), Gui, SLOT(reloadSeed()));
        QObject::connect(actionStep, SIGNAL(triggered()), Gui, SLOT(stepToTime()));
        QObject::connect(actionHelp, SIGNAL(triggered()), Gui, SLOT(help()));
        QObject::connect(actionAbout, SIGNAL(triggered()), Gui, SLOT(about()));
        QObject::connect(actionScreenshot, SIGNAL(triggered()), Gui, SLOT(snapshot()));
        QObject::connect(actionMoreCells, SIGNAL(triggered()), Gui, SLOT(moreCells()));
        QObject::connect(actionZoomIn, SIGNAL(triggered()), Gui, SLOT(zoomIn()));
        QObject::connect(actionZoomOut, SIGNAL(triggered()), Gui, SLOT(zoomOut()));
        QObject::connect(actionResetZoom, SIGNAL(triggered()), Gui, SLOT(resetZoom()));
        QObject::connect(actionDump, SIGNAL(triggered()), Gui, SLOT(dump()));
        QObject::connect(actionCenter, SIGNAL(triggered()), Gui, SLOT(centerCamera()));
        QObject::connect(actionAutoZoom, SIGNAL(toggled(bool)), Gui, SLOT(toggleAutoZoom(bool)));
        QObject::connect(action10steps, SIGNAL(triggered()), Gui, SLOT(step()));
        QObject::connect(action100steps, SIGNAL(triggered()), Gui, SLOT(step()));
        QObject::connect(action1step, SIGNAL(triggered()), Gui, SLOT(step()));

        QMetaObject::connectSlotsByName(Gui);
    } // setupUi

    void retranslateUi(QMainWindow *Gui)
    {
        Gui->setWindowTitle(QApplication::translate("Gui", "gro : the cell programming language", nullptr));
        actionStartStop->setText(QApplication::translate("Gui", "Start", nullptr));
#ifndef QT_NO_TOOLTIP
        actionStartStop->setToolTip(QApplication::translate("Gui", "Start or stop gro ", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionStartStop->setShortcut(QApplication::translate("Gui", "Return", nullptr));
#endif // QT_NO_SHORTCUT
        actionOpen->setText(QApplication::translate("Gui", "Open...", nullptr));
        actionOpen->setIconText(QApplication::translate("Gui", "Open", nullptr));
#ifndef QT_NO_TOOLTIP
        actionOpen->setToolTip(QApplication::translate("Gui", "Open a .gro file.", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionOpen->setShortcut(QApplication::translate("Gui", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionReload->setText(QApplication::translate("Gui", "Reload", nullptr));
#ifndef QT_NO_TOOLTIP
        actionReload->setToolTip(QApplication::translate("Gui", "Reload current .gro file", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionReload->setShortcut(QApplication::translate("Gui", "Ctrl+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionReloadSeed->setText(QApplication::translate("Gui", "Change seed", nullptr));
#ifndef QT_NO_TOOLTIP
        actionReloadSeed->setToolTip(QApplication::translate("Gui", "Reload with a different seed", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionReloadSeed->setShortcut(QApplication::translate("Gui", "Ctrl+E", nullptr));
#endif // QT_NO_SHORTCUT
        actionStep->setText(QApplication::translate("Gui", "Step", nullptr));
#ifndef QT_NO_TOOLTIP
        actionStep->setToolTip(QApplication::translate("Gui", "Simulate until given time", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionStep->setShortcut(QApplication::translate("Gui", "0", nullptr));
#endif // QT_NO_SHORTCUT
        actionHelp->setText(QApplication::translate("Gui", "Online documentation", nullptr));
#ifndef QT_NO_SHORTCUT
        actionHelp->setShortcut(QApplication::translate("Gui", "Ctrl+D", nullptr));
#endif // QT_NO_SHORTCUT
        actionAbout->setText(QApplication::translate("Gui", "About", nullptr));
        actionScreenshot->setText(QApplication::translate("Gui", "Save Screenshot...", nullptr));
#ifndef QT_NO_TOOLTIP
        actionScreenshot->setToolTip(QApplication::translate("Gui", "Save screenshot", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionScreenshot->setShortcut(QApplication::translate("Gui", "Ctrl+I", nullptr));
#endif // QT_NO_SHORTCUT
        actionMoreCells->setText(QApplication::translate("Gui", "Increase Population Limit", nullptr));
#ifndef QT_NO_TOOLTIP
        actionMoreCells->setToolTip(QApplication::translate("Gui", "Increase population limit", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionMoreCells->setShortcut(QApplication::translate("Gui", "Ctrl+M", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoomIn->setText(QApplication::translate("Gui", "Zoom in", nullptr));
#ifndef QT_NO_SHORTCUT
        actionZoomIn->setShortcut(QApplication::translate("Gui", "Ctrl+Shift+=", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoomOut->setText(QApplication::translate("Gui", "Zoom Out", nullptr));
#ifndef QT_NO_TOOLTIP
        actionZoomOut->setToolTip(QApplication::translate("Gui", "Zoom out", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionZoomOut->setShortcut(QApplication::translate("Gui", "Ctrl+-", nullptr));
#endif // QT_NO_SHORTCUT
        actionResetZoom->setText(QApplication::translate("Gui", "Reset zoom", nullptr));
#ifndef QT_NO_SHORTCUT
        actionResetZoom->setShortcut(QApplication::translate("Gui", "Ctrl+0", nullptr));
#endif // QT_NO_SHORTCUT
        actionDump->setText(QApplication::translate("Gui", "Save classic gro dump", nullptr));
        actionCenter->setText(QApplication::translate("Gui", "Center view", nullptr));
#ifndef QT_NO_TOOLTIP
        actionCenter->setToolTip(QApplication::translate("Gui", "Center view", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionCenter->setShortcut(QApplication::translate("Gui", "Space", nullptr));
#endif // QT_NO_SHORTCUT
        actionAutoZoom->setText(QApplication::translate("Gui", "Auto Zoom", nullptr));
#ifndef QT_NO_TOOLTIP
        actionAutoZoom->setToolTip(QApplication::translate("Gui", "Toggle auto zoom", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAutoZoom->setShortcut(QApplication::translate("Gui", "Ctrl+A", nullptr));
#endif // QT_NO_SHORTCUT
        action10steps->setText(QApplication::translate("Gui", "10 steps", nullptr));
#ifndef QT_NO_TOOLTIP
        action10steps->setToolTip(QApplication::translate("Gui", "10 steps", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        action10steps->setShortcut(QApplication::translate("Gui", "2", nullptr));
#endif // QT_NO_SHORTCUT
        action100steps->setText(QApplication::translate("Gui", "100 steps", nullptr));
#ifndef QT_NO_SHORTCUT
        action100steps->setShortcut(QApplication::translate("Gui", "3", nullptr));
#endif // QT_NO_SHORTCUT
        action1step->setText(QApplication::translate("Gui", "1 step", nullptr));
#ifndef QT_NO_TOOLTIP
        action1step->setToolTip(QApplication::translate("Gui", "1 step", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        action1step->setShortcut(QApplication::translate("Gui", "1", nullptr));
#endif // QT_NO_SHORTCUT
        menuFile->setTitle(QApplication::translate("Gui", "File", nullptr));
        menuSimulation->setTitle(QApplication::translate("Gui", "Simulation", nullptr));
        menuHelp->setTitle(QApplication::translate("Gui", "Help", nullptr));
        menuView->setTitle(QApplication::translate("Gui", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Gui: public Ui_Gui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
