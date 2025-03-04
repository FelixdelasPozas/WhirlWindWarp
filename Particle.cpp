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
#include <Particle.h>
#include <WhirlWindWarp.h>

// C++
#include <cmath>
#include <algorithm>
#include <execution>
#include <cassert>

//--------------------------------------------------------------------
Particle::Particle(State &state, Utils::NumberGenerator* generator)
: m_generator   (generator)
, m_state       (state)
{
  assert(generator);
  init();
}

//--------------------------------------------------------------------
void Particle::advance()
{
  // TODO
  for(int i = 0; i < m_state.numPoints; ++i)
  {
    // Move a star according to acting forcefields.
    if(m_state.tailLenght != 0)
    {
      // if(m_tailX[i].size() == m_tailLength) m_tailX[i].erase(m_tailX[i].begin());
      // if(m_tailY[i].size() == m_tailLength) m_tailY[i].erase(m_tailY[i].begin());

      // m_tailX[i].push_back(m_x[i]);
      // m_tailY[i].push_back(m_y[i]);
    }

    auto pos = m_buffer.data() + (i * 7);

    double x = pos[0];
    double y = pos[1];

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
      x = -1.0 + 2.0 * std::pow((x + 1.0) / 2.0, m_state.var[6]);
    }

    if (m_state.enabled[7])
    {
      y = -1.0 + 2.0 * std::pow((y + 1.0) / 2.0, m_state.var[7]);
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
      const auto nx = x * std::cos(1.1 * m_state.var[2]) + y * std::sin(1.1 * m_state.var[2]);
      const auto ny = -x * std::sin(1.1 * m_state.var[2]) + y * std::cos(1.1 * m_state.var[2]);
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
    auto num_splits = [this] () { return 2 + static_cast<int>(std::fabs(this->m_state.var[0]) * 1000); };
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
      y = y + 0.4 * m_state.var[10] * std::sin(300.0 * m_state.var[12] * x + 600.0 * m_state.var[11]);
    }

    if (m_state.enabled[13])
    {
      x = x + 0.4 * m_state.var[13] * std::sin(300.0 * m_state.var[15] * y + 600.0 * m_state.var[14]);
    }

    if(x <= -1.f || x >= 1.f || y <= -1.f || y >= 1.f || fabs(x) < .0001 || fabs(y) < .0001)
    {
      // If moved off screen or too centered to move, create a new one.
      reset(i);
    }
    else
    {
      if(m_generator->get() > 0.995)
      {
        reset(i);
      }
      else
      {
        pos[0] = x;
        pos[1] = y;
      }
    }

    if (!m_state.changedColor && (m_generator->get() > 0.75))
    {
      const Utils::hsv hsvColor(m_state.hue, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());

      // Change one of the allocated colours to something near the current hue.
      // By changing a random colour, we sometimes get a tight colour spread, sometime a diverse one.
      const auto rgbColor = Utils::hsv2rgb(hsvColor);
      pos[2] = rgbColor.r;
      pos[3] = rgbColor.g;
      pos[4] = rgbColor.b;
      pos[5] = 1.f;

      m_state.hue = m_state.hue + 0.5 + m_generator->get() * 9.0;
      if (m_state.hue < 0) m_state.hue += 360;
      if (m_state.hue >= 360) m_state.hue -= 360;

      m_state.changedColor = true;
    }
  }
}

//--------------------------------------------------------------------
void Particle::init()
{
  const auto elements = (1 + (m_state.drawTails ? m_state.tailLenght : 0)) * m_state.numPoints * 7;
  
  m_buffer = std::vector<float>((elements*7),0);

  std::vector<size_t> index[elements];
  std::iota(index->begin(), index->end(), 0);

  auto resetBuffer = [&](const size_t pos)
  {
    const int num = 1 + (m_state.drawTails ? m_state.tailLenght : 0);
    reset(pos * num * 7); 
  };
  std::for_each(std::execution::par_unseq, index->cbegin(), index->cend(), resetBuffer);
}

//--------------------------------------------------------------------
void Particle::reset(const int idx)
{
  auto pos = m_buffer.data() + (7*idx);

  memset(pos, 0, sizeof(float) * 7);
  pos[0] = m_generator->get();
  pos[1] = m_generator->get();

  Utils::hsv hsvColor((m_generator->get() + 1.0) * 180.0, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());
  const auto rgbColor = Utils::hsv2rgb(hsvColor);
  pos[2] = rgbColor.r;
  pos[3] = rgbColor.g;
  pos[4] = rgbColor.b;
  pos[5] = 1.f;

  pos[6] = 1 + (m_generator->get() + 1);
};

//--------------------------------------------------------------------
// void Particle::paint()
// {
  // for(int i = 0; i < m_state.numPoints; ++i)
  // {
  //   auto color = m_color[i];

  //   if(m_drawTails && m_tailX[i].size() > 0)
  //   {
  //     for(int j = m_tailX[i].size() - 1; j > 0; --j)
  //     {
  //       rgb fadeColor = color;;
  //       if(m_fadeTails)
  //       {
  //         const auto factor = std::pow(0.75, m_tailX[i].size()-j);
  //         fadeColor = fadeColor * factor;
  //       }

  //       painter->drawLine(transform(m_tailX[i].at(j)) * width, transform(m_tailY[i].at(j)) * height, transform(m_tailX[i].at(j-1)) * width, transform(m_tailY[i].at(j-1)) * height);
  //     }

  //     pen.setColor(m_color[i]);
  //     painter->setPen(pen);
  //     painter->drawLine(transform(m_x[i]) * width, transform(m_y[i]) * height, transform(m_tailX[i].last()) * width, transform(m_tailY[i].last()) * height);
  //   }

  //   pen.setColor(m_color[i]);
  //   painter->setPen(pen);
  //   painter->drawPoint(transform(m_x[i]) * width, transform(m_y[i]) * height);
  // }
// }

