/*
 File: OptionsDialog.cpp
 Created on: 27/05/2017
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
#include "OptionsDialog.h"
#include "State.h"

// Qt
#include <QSettings>

//--------------------------------------------------------------------
OptionsDialog::OptionsDialog()
{
  setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
  setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint);

  setupUi(this);

  connect(m_drawTails, SIGNAL(stateChanged(int)), this, SLOT(onDrawTailsChanged(int)));

  QSettings settings(COMPANY_NAME, APPLICATION_NAME, this);
  auto drawTails = settings.value(DRAW_TAILS_KEY, true).toBool();
  auto fadeTails = settings.value(TAIL_FADE_KEY, true).toBool();
  auto length    = settings.value(TAIL_LENGTH_KEY, 15).toInt();
  auto fps       = settings.value(FPS_KEY, 60).toInt();
  auto antialias = settings.value(ANTIALIAS_KEY, true).toBool();

  m_drawTails->setChecked(drawTails);
  m_fps->setValue(fps);
  m_antialias->setChecked(antialias);
  m_fadeTails->setChecked(fadeTails);
  m_tailSize->setValue(length);
}

//--------------------------------------------------------------------
void OptionsDialog::accept()
{
  QSettings settings(COMPANY_NAME, APPLICATION_NAME, this);
  settings.setValue(DRAW_TAILS_KEY, m_drawTails->isChecked());
  settings.setValue(TAIL_FADE_KEY, m_fadeTails->isChecked());
  settings.setValue(TAIL_LENGTH_KEY, m_tailSize->value());
  settings.setValue(FPS_KEY, m_fps->value());
  settings.setValue(ANTIALIAS_KEY, m_antialias->isChecked());
  settings.sync();

  QDialog::accept();
}

//--------------------------------------------------------------------
void OptionsDialog::onDrawTailsChanged(int state)
{
  auto value = m_drawTails->isChecked();

  m_fadeTails->setEnabled(value);
  m_tailSize->setEnabled(value);
  m_lengthLabel->setEnabled(value);
}
