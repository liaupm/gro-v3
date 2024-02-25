#-------------------------------------------------
#
# Project created by QtCreator 2012-05-23T09:21:54
#
#-------------------------------------------------

CONFIG += warn_off
CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
QT += core gui printsupport

#================================= NO GUI =============================================
# uncomment this line to compile grong (the no gui version)
# CONFIG += nogui

contains ( CONFIG, nogui ) {
  QMAKE_CXXFLAGS += -DNOGUI
  QT -= core gui
  TARGET = grong
  TEMPLATE = app
} 
else {
  QT += core gui widgets
  TARGET = gro
  TEMPLATE = app
}


#================================= OS FLAGS =============================================
win32 {
  QMAKE_CXXFLAGS += -O0 -DWIN -DNDEBUG
  contains ( CONFIG, nogui ) {
    CONFIG += console
  }
  CONFIG -= app_bundle
}

macx {
  QMAKE_CXXFLAGS += -fast
  QMAKE_MAC_SDK = macosx10.12
}


#================================= FILES =============================================
ICON = groicon.icns

SOURCES += mainDependencyTest.cpp\
    CellPlacer.cpp \
    CellPlating.cpp \
	CellsFile.cpp \
    Checkpoint.cpp \
    CoordsSelector.cpp \
    GlobalParams.cpp \
    GroElement.cpp\
	CclAdapter.cpp\
    PetriDish.cpp \
    SignalPlacer.cpp \
    Snapshot.cpp \
    Timer.cpp \
    GroThread.cpp \
    MessageHandler.cpp \
    Theme.cpp \
    GraphicsHandler.cpp \
	Parser.cpp \
    World.cpp \
    Gro.cpp \
    GroPainter.cpp \
    Cell.cpp \
	GroWidget.cpp \


HEADERS  += GroWidget.h \



contains ( CONFIG, nogui ) {
  SOURCES -= GroThread.cpp GroWidget.cpp GroPainter.cpp gui.cpp Themes.cpp
  HEADERS -= gui.h GroThread.h GroWidget.h GroPainter.h
}
else {
	FORMS += gui.ui
}


macx {
  CONFIG(debug, debug|release) {
        LIBS += -L../ccl_Debug/ -lccl -L ../CellEngine/build/mac/debug -lcellengine -L../cellsignals/build/mac/debug -lsignals -L../CellPro/build/mac/debug -lcellpro
        PRE_TARGETDEPS += ../ccl_Debug/libccl.a
  } else {
        LIBS += -L../ccl_Release/ -lccl -L ../CellEngine/build/mac/release -lcellengine -L../cellsignals/build/mac/release -lsignals -L../CellPro/build/mac/release -lcellpro
        PRE_TARGETDEPS += ../ccl_Release/libccl.a
  }

  #DEPENDPATH += ../Chipmunk-5.3.5/
  INCLUDEPATH += ../ccl-master/ ../CellEngine/include ../cellsignals/include ../CellPro/include ../Utility/include

  #makelinks.commands += echo Making links in $$OUT_PWD;
  #makelinks.commands += ln -s -f ../GRO/examples;
  #makelinks.commands += ln -s -f ../GRO/include;
}

linux {
  CONFIG(debug, debug|release) {
        LIBS += -L../ccl_Debug/ -lccl -L../CellEngine/build/linux/debug -lcellengine -L../CellPro/build/linux/debug -lcellpro -L../cellsignals/build/linux/debug -lsignals -lrt
        PRE_TARGETDEPS += ../ccl_Debug/libccl.a
  } else {
        LIBS += -L../ccl_Release/ -lccl -L../CellEngine/build/linux/release -lcellengine -L../CellPro/build/linux/release -lcellpro -L../cellsignals/build/linux/release -lsignals -lrt
        PRE_TARGETDEPS += ../ccl_Release/libccl.a
  }
  #LIBS += -L/home/tin/copyfiles/PhD/EvLIT/PLASWIRES/GRO/BiGRO2/CellEngine/build/ -lcellengine -L/home/tin/copyfiles/PhD/EvLIT/PLASWIRES/GRO/BiGRO2/cellsignal/cellsignal-0.0.3/build/ -lcellsignal
  #LIBS += -L../CellEngine/build/ -lcellengine -L../cellsignal/cellsignal-0.0.3/build/ -lcellsignal

  #../CellEngine/build/libcellengine.a ../cellsignal/cellsignal-0.0.3/build/libcellsignal.a
  #DEPENDPATH += ../Chipmunk-5.3.5/
  INCLUDEPATH += ../ccl-master/ ../CellEngine/include/ ../cellsignals/include/ ../CellPro/include ../Utility/include

  #makelinks.commands += echo Making links in $$OUT_PWD;
  #makelinks.commands += ln -s -f ../GRO/examples;
  #makelinks.commands += ln -s -f ../GRO/include;
}

win32 {
  CONFIG(debug, debug|release) {
        LIBS += -L../ccl_Debug/debug -lccl -L../CellEngine/build/win/debug -lcellengine -L../cellsignals/build/win/debug -lsignals -L../CellPro/build/win/debug -lcellpro -L../Utility/build/win/debug -lutility
        PRE_TARGETDEPS += ../ccl_Debug/debug/libccl.a
  } else {
        LIBS += -L../ccl_Release/release -lccl -L../CellEngine/build/win/release -lcellengine -L../cellsignals/build/win/release -lsignals -L../CellPro/build/win/release -lcellpro -L../Utility/build/win/release -lutility
        PRE_TARGETDEPS += ../ccl_Release/release/libccl.a
  }

  INCLUDEPATH += ../ccl-master/ ../CellEngine/include/ ../cellsignals/include/ ../CellPro/include ../Utility/include
  #QMAKE_POST_LINK = copy ..\GRO\include\*.* ..\..\;
}

QMAKE_EXTRA_TARGETS += makelinks
POST_TARGETDEPS += makelinks


OTHER_FILES += \
    include/gro.gro \
    include/standard.gro \
    icons/stop.png \
    icons/step.png \
    icons/start.png \
    icons/reload.png \
    icons/open.png \
    changelog.txt \
    error.tiff \
    LICENSE.txt \

!contains ( CONFIG, nogui ) {
  RESOURCES += icons.qrc
}

QMAKE_CLEAN += Makefile moc_GroPainter.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Statistics-Debug/release/ -lStatistics
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Statistics-Debug/debug/ -lStatistics
#else:unix: LIBS += -L$$PWD/../build-Statistics-Debug/ -lStatistics

#INCLUDEPATH += $$PWD/../Statistics
#DEPENDPATH += $$PWD/../Statistics

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-Statistics-Debug/release/Statistics.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-Statistics-Debug/debug/Statistics.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../build-Statistics-Debug/libStatistics.a

FORMS += \
    GuiPlots.ui
