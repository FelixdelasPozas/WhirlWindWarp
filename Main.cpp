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
#include "State.h"

// Qt
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QIcon>
#include <QDebug>
#include <QDesktopWidget>

// C++
#include <iostream>

const int EXPECTED_FPS = 60;

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
int main(int argc, char *argv[])
{
  qInstallMessageHandler(myMessageOutput);

  QApplication app(argc, argv);

  // allow only one instance
  QSharedMemory guard;
  guard.setKey("WindWhirlWarp");

  if (!guard.create(1))
  {
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/WhirlWindWarp/app.ico"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("WindWhirlWarp screensaver is already running!");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    exit(0);
  }

  NumberGenerator random{-1.0, 1.0};

  auto rectangle = QApplication::desktop()->geometry();

  QGraphicsScene scene;
  scene.setSceneRect(rectangle);
  scene.setBackgroundBrush(Qt::black);

  QApplication::setOverrideCursor(Qt::BlankCursor);

  WhirlWindWarp screenSaver(&random, &scene);
  screenSaver.setRenderHint(QPainter::Antialiasing);
  screenSaver.showFullScreen();
  screenSaver.setGeometry(rectangle);
  screenSaver.move(0,0);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), &screenSaver, SLOT(advance()));
  timer.start(1000 / EXPECTED_FPS);

  auto resultValue = app.exec();

  QApplication::restoreOverrideCursor();

  qDebug() << "terminated with value" << resultValue;

  return resultValue;
}

