/*
 File: Main.cpp
 Created on: 02/04/2017
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Project
#include "WhirlWindWarp.h"
#include "OptionsDialog.h"
#include "State.h"

// Qt
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QIcon>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QWindow>
#include <QHBoxLayout>
#include <QTimer>
#include <QSettings>

// C++
#include <iostream>

//-----------------------------------------------------------------
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  const char symbols[] =
  { 'I', 'E', '!', 'X' };
//  QString output = QString("[%1] %2 (%3:%4 -> %5)").arg( symbols[type] ).arg( msg ).arg(context.file).arg(context.line).arg(context.function);
  QString output = QString("[%1] %2").arg(symbols[type]).arg(msg);
  std::cerr << output.toStdString() << std::endl;
  if (type == QtFatalMsg) abort();
}

//-----------------------------------------------------------------
void showConfig()
{
  OptionsDialog dialog;
  dialog.exec();
}

//-----------------------------------------------------------------
void showHelp()
{
  auto message = QString("WhirlWindWarp screensaver command line options:\n");
  message += QString("/s or /S   Starts the screensaver.\n");
  message += QString("/c or /C   Shows the configuration dialog.\n");
  message += QString("/h or /H   Shows this help dialog.\n");

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  msgBox.setText(message);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
  qInstallMessageHandler(myMessageOutput);
  int resultValue = 0;

  // QCommandLineParser uses unix-like '-' for flags, while windows uses '/'. There is no way for QCommandLineParser to adapt to it
  // so we'll need to change all characters first. Also the 'c' option can have an optional value, and QCommandLineParser also doesn't
  // support that, so we'll need to truncate the argument.
  for(int i = 1; i < argc; ++i)
  {
    bool dash = false;
    bool clear = false;
    for(char *j = argv[i]; *j; ++j)
    {
      if(*j == '/')
      {
        *j = '-';
        dash = true;
      }
      else
      {
        if(dash)
        {
          if(*j == 'c' || *j == 'C') clear = true;
          dash = false;
        }
        else
        {
          if(clear)
          {
            *j = '\0';
            break;
          }
        }
      }
    }
  }

  QApplication app(argc, argv);

  if(argc == 1)
  {
    showConfig();
    return 0;
  }

  // allow only one instance running
  QSharedMemory guard;
  guard.setKey("WindWhirlWarp");

  if (!guard.create(1))
  {
    return 0;
  }

  // Note: custom help dialog is shown instead of the one created by parser due to '/' instead of '-'. Also the
  // configuration parameter has an optional 'undocumented' value passed by windows screensaver configurator.
  // We don't have a settings dialog so we'll pass on any value given.
  auto startOption   = QCommandLineOption{QStringList() << "s" << "S", "Starts the screensaver."};
  auto previewOption = QCommandLineOption{QStringList() << "p" << "P", "Shows the preview in dialog handle <handle>.", "unusedvalue", "None"};
  auto configOption  = QCommandLineOption{QStringList() << "c" << "C", "Shows the configuration dialog."};
  auto helpgOption   = QCommandLineOption{QStringList() << "h" << "H", "Shows the screensaver help."};

  QCommandLineParser parser;
  parser.setApplicationDescription("WhirlWindWarp Screensaver");
  parser.addOption(startOption);
  parser.addOption(configOption);
  parser.addOption(previewOption);
  parser.addOption(helpgOption);
  parser.process(app);

  if (parser.isSet(startOption))
  {
    QSettings settings(COMPANY_NAME, APPLICATION_NAME);
    auto fps       = settings.value(FPS_KEY, 60).toInt();
    auto antialias = settings.value(ANTIALIAS_KEY, true).toBool();

    NumberGenerator random{-1.0, 1.0};

    auto rectangle = QApplication::desktop()->geometry();

    QGraphicsScene scene;
    scene.setSceneRect(rectangle);
    scene.setBackgroundBrush(Qt::black);

    QApplication::setOverrideCursor(Qt::BlankCursor);

    WhirlWindWarp screenSaver(&random, &scene);
    if(antialias) screenSaver.setRenderHint(QPainter::Antialiasing);
    screenSaver.showFullScreen();
    screenSaver.setGeometry(rectangle);
    screenSaver.move(0, 0);

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &screenSaver, SLOT(advance()));
    timer.start(1000 / fps);

    resultValue = app.exec();

    QApplication::restoreOverrideCursor();
  }
  else
  {
    if (parser.isSet(configOption))
    {
      showConfig();
    }
    else
    {
      showHelp();
    }
  }

  return resultValue;
}
