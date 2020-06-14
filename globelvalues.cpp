#include "calibframe.h"
#include <qglobal.h>
#include "screensaver.h"

CalibFrame *pframe = NULL;
ScreenSaver *screenSaver = NULL;
QString elementPath = "";
QString errorMessage = "";
QString str = elementPath + "/config.db";
