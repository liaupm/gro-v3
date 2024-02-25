#if defined __cplusplus

//=================================== STD CONTAINERS =====================================

	#include <vector>
	#include <array>
	#include <set>
	#include <map>
	#include <list>
	#include <tuple>
	#include <string>
	#include <bitset>


//================================== STD OTHERS =========================================

//---meta
	#include <memory> //smart pointers
	#include <future> //concurrent update std::future, std::async
	#include <type_traits>
	#include <utility> //std::move, pair
	#include <assert.h> //exceptions
	#include <stdexcept>
	#include <functional>

//---math
	#include <algorithm> //std::transform
	#include <limits> //std::numeric_limits
	#include <cstdint>
	#include <numeric>
	#include <math.h>
	#include <cmath> //sqrt, cos, sin, abs
	#include <random>

//---print
	#include <iostream>
	#include <fstream>
	#include <sstream>
	#include <iomanip>  //std::setprecision()
	#include <unistd.h> //chdir, getcwd


//================================== QT =========================================
	#include <QtCore>
	#include <Qt>
	#include <QApplication> //quit

	#include <QCoreApplication>
	#include <QtWidgets/QApplication> //QApplication app
	#include <QMainWindow>

	#include <QObject>
	#include <QVector>
	#include <QString>
	#include <QSize>
	#include <QPoint>
	#include <QRect>
	#include <QColor>

	#include <QBrush>
	#include <QPen>
	#include <QImage>
	#include <QPixmap>
	#include <QPainter>
	#include <QPaintDevice>

	#include <QDir>
	#include <QFileDialog>
	#include <QInputDialog>
	#include <QTextOption>
	#include <QMessageBox>
	#include <QTextEdit>
	#include <QSplitter>
	#include <QWidget>

	#include <QEvent>
	#include <QMouseEvent>
	#include <QKeyEvent>

	#include <QMutex>
	#include <QMutexLocker>
	#include <QTimer>
	#include <QElapsedTimer>
	#include <QWaitCondition>
	#include <QThread>

    #include <QMediaPlayer>


//================================== LIBS =========================================

	#include "qcustomplot.h"
	#include "ccl.h" //connection to ccl module, Value

	#include "CellEngine.h" //physics
	#include "CESpace.h" //physics

#endif