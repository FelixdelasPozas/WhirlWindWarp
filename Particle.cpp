/*
 File: Particle.cpp
 Created on: 25/05/2017
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
#include "Particle.h"
#include "State.h"

// C++
#include <cmath>

// Qt
#include <QSettings>
#include <QGraphicsScene>
#include <QPainter>

//--------------------------------------------------------------------
Particle::Particle(State &state, const unsigned int number, NumberGenerator* generator)
: m_generator   (generator)
, m_state       (state)
, m_tailLength  {state.tailLenght}
{
  init(number);

  QSettings settings(COMPANY_NAME, APPLICATION_NAME);
  m_drawTails  = settings.value(DRAW_TAILS_KEY, true).toBool();
  m_fadeTails  = settings.value(TAIL_FADE_KEY, true).toBool();
  m_tailLength = settings.value(TAIL_LENGTH_KEY, 15).toInt();
}

//--------------------------------------------------------------------
void Particle::advance(int phase)
{
  if(phase == 0) return;

  for(int i = 0; i < m_state.numPoints; ++i)
  {
    // Move a star according to acting forcefields.
    if(m_tailLength != 0)
    {
      if(m_tailX[i].size() == m_tailLength) m_tailX[i].removeFirst();
      if(m_tailY[i].size() == m_tailLength) m_tailY[i].removeFirst();

      m_tailX[i] << m_x[i];
      m_tailY[i] << m_y[i];
    }

    double x = m_x[i];
    double y = m_y[i];

    // In theory all these if checks are unnecessary,
    // since each forcefield effect should do nothing when its var = op.
    // But the if's are good for efficiency because this function
    // is called once for every point.
    //
    // Squirge towards edges (makes a leaf shape, previously split the screen in 4 but now only 1 :)
    // These ones must go first, to avoid x+1.0 < 0
    if (m_state.enabled[6])
    {
      // x = mysgn(x) * pow(fabs(x),var[6]);
      // y = mysgn(y) * pow(fabs(y),var[6]);
      x = -1.0 + 2.0 * ::pow((x + 1.0) / 2.0, m_state.var[6]);
    }

    if (m_state.enabled[7])
    {
      y = -1.0 + 2.0 * pow((y + 1.0) / 2.0, m_state.var[7]);
    }

    /* Warping in/out */
    if (m_state.enabled[1])
    {
      x = x * m_state.var[1];
      y = y * m_state.var[1];
    }

    /* Rotation */
    if (m_state.enabled[2])
    {
      auto nx = x * cos(1.1 * m_state.var[2]) + y * sin(1.1 * m_state.var[2]);
      auto ny = -x * sin(1.1 * m_state.var[2]) + y * cos(1.1 * m_state.var[2]);
      x = nx;
      y = ny;
    }

    /* Asymptotes (looks like a plane with a horizon; equivalent to 1D warp) */
    if (m_state.enabled[3])
    {
      /* Horizontal asymptote */
      y = y * m_state.var[3];
    }

    if (m_state.enabled[4])
    {
      /* Vertical asymptote */
      x = x + m_state.var[4] * x; /* this is the same maths as the last, but with op=0 */
    }

    if (m_state.enabled[5])
    {
      /* Vertical asymptote at right of screen */
      x = (x - 1.0) * m_state.var[5] + 1.0;
    }

    /* Splitting (whirlwind effect): */
    auto num_splits = [this] () { return 2 + static_cast<int>(fabs(this->m_state.var[0]) * 1000); };
    auto thru       = [this, i] (int splits) { return static_cast<float>(static_cast<int>(splits * static_cast<float>(i)/static_cast<float>(this->m_state.numPoints)))/static_cast<float>(splits-1); };

    if (m_state.enabled[8])
    {
      x = x + 0.5 * m_state.var[8] * (-1.0 + 2.0 * thru(num_splits()));
    }

    if (m_state.enabled[9])
    {
      y = y + 0.5 * m_state.var[9] * (-1.0 + 2.0 * thru(num_splits()));
    }

    /* Waves */
    if (m_state.enabled[10])
    {
      y = y + 0.4 * m_state.var[10] * sin(300.0 * m_state.var[12] * x + 600.0 * m_state.var[11]);
    }

    if (m_state.enabled[13])
    {
      x = x + 0.4 * m_state.var[13] * sin(300.0 * m_state.var[15] * y + 600.0 * m_state.var[14]);
    }

    if (x <= -0.9999 || x >= +0.9999 || y <= -0.9999 || y >= +0.9999 || fabs(x) < .0001 || fabs(y) < .0001)
    {
      // If moved off screen, create a new one.
      reset(i);
    }
    else
    {
      if(m_generator->get() > 0.99)
      {
        // reset at random.
        reset(i);
      }
      else
      {
        m_x[i] = x;
        m_y[i] = y;
      }
    }

    if (!m_state.changedColor && (m_generator->get() > 0.75))
    {
      // Change one of the allocated colours to something near the current hue.
      // By changing a random colour, we sometimes get a tight colour spread, sometime a diverse one.
      m_color[i] = QColor::fromHsvF(m_state.hue/360.0, .6 + .4 * m_generator->get(), .6 + .4 * m_generator->get());
      m_state.hue = m_state.hue + 0.5 + m_generator->get() * 9.0;
      if (m_state.hue < 0) m_state.hue += 360;
      if (m_state.hue >= 360) m_state.hue -= 360;

      m_state.changedColor = true;
    }
  }
  setPos(0, 0);

}

//--------------------------------------------------------------------
void Particle::init(const unsigned int numPoints)
{
  m_x.reserve(numPoints);
  m_y.reserve(numPoints);
  m_width.reserve(numPoints);
  m_tailX.reserve(numPoints);
  m_tailY.reserve(numPoints);
  m_color.reserve(numPoints);

  for(unsigned int i = 0; i < numPoints; ++i)
  {
    m_x << m_generator->get();
    m_y << m_generator->get();

    m_width << 1 + (::rand() % 3);

    m_tailX << QList<double>();
    m_tailY << QList<double>();

    m_color << QColor::fromHsvF((m_generator->get() + 1.0)/2.0, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());
  }
}

//--------------------------------------------------------------------
void Particle::reset(const int point)
{
  m_x[point] = m_generator->get();
  m_y[point] = m_generator->get();

  m_width[point] = 1 + (::rand() % 3);

  m_tailX[point].clear();
  m_tailY[point].clear();

  m_color[point] = QColor::fromHsvF((m_generator->get() + 1.0)/2.0, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());
}

//--------------------------------------------------------------------
QRectF Particle::boundingRect() const
{
  return scene()->sceneRect();
}

//--------------------------------------------------------------------
QPainterPath Particle::shape() const
{
  QPainterPath path;
  path.addRect(boundingRect());
  return path;
}

//--------------------------------------------------------------------
void Particle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto width = scene()->width();
  auto height = scene()->height();

  auto transform = [] (double num) { return (num+1.0)/2.0; };

  for(int i = 0; i < m_state.numPoints; ++i)
  {
    auto color = m_color[i];

    QPen pen;
    pen.setWidth(m_width[i]);
    pen.setColor(color);

    if(m_drawTails && m_tailX[i].size() > 0)
    {
      for(int j = m_tailX[i].size() - 1; j > 0; --j)
      {
        if(m_fadeTails)
        {
          auto factor = pow(0.75, m_tailX[i].size()-j);
          pen.setColor(QColor::fromRgbF(m_color[i].redF() * factor, m_color[i].greenF() * factor, m_color[i].blueF() * factor));
        }

        painter->setPen(pen);
        painter->drawLine(transform(m_tailX[i].at(j)) * width, transform(m_tailY[i].at(j)) * height, transform(m_tailX[i].at(j-1)) * width, transform(m_tailY[i].at(j-1)) * height);
      }

      pen.setColor(m_color[i]);
      painter->setPen(pen);
      painter->drawLine(transform(m_x[i]) * width, transform(m_y[i]) * height, transform(m_tailX[i].last()) * width, transform(m_tailY[i].last()) * height);
    }

    pen.setColor(m_color[i]);
    painter->setPen(pen);
    painter->drawPoint(transform(m_x[i]) * width, transform(m_y[i]) * height);
  }
}
