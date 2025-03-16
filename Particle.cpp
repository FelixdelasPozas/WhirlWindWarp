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

constexpr float FPS60 = 1.f/60.f;

//--------------------------------------------------------------------
Particles::Particles(State &state, Utils::NumberGenerator* generator, const Utils::Configuration &config)
: m_state    {state}
, m_generator{generator}
, m_config   {config}
{
  assert(generator);
  init();
}

//--------------------------------------------------------------------
void Particles::advance()
{
  const int multiplier = m_config.show_trails ? 2:1;

  for(int i = 0; i < m_state.numPoints; ++i)
  {
    Particle *pos = reinterpret_cast<Particle*>(m_buffer.data()) + (i * multiplier);

    if(m_config.show_trails)
      memcpy(pos+1, pos, sizeof(Particle));

    double x = pos->x;
    double y = pos->y;

    // In theory all these if checks are unnecessary,
    // since each forcefield effect should do nothing when its var = op.
    // But the if's are good for efficiency because this function
    // is called once for every point.
    //
    // Squirge towards edges (makes a leaf shape, previously split the screen in 4 but now only 1 :)
    // These ones must go first, to avoid x+1.0 < 0
    if (m_state.enabled[6])
    {
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
        pos->x = x;
        pos->y = y;
      }
    }

    if (!m_state.changedColor && (m_generator->get() > 0.75))
    {
      const Utils::hsv hsvColor(m_state.hue, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());

      // Change one of the allocated colours to something near the current hue.
      // By changing a random colour, we sometimes get a tight colour spread, sometime a diverse one.
      const auto rgbColor = Utils::hsv2rgb(hsvColor);
      pos->r = rgbColor.r;
      pos->g = rgbColor.g;
      pos->b = rgbColor.b;
      pos->a = 1.f;

      m_state.hue = m_state.hue + 0.5 + m_generator->get() * 9.0;
      if (m_state.hue < 0) m_state.hue += 360;
      if (m_state.hue >= 360) m_state.hue -= 360;

      m_state.changedColor = true;
    }
  }
}

//--------------------------------------------------------------------
void Particles::init()
{
  const int multiplier = m_config.show_trails ? 2 : 1;
  m_buffer = std::vector<float>((multiplier*m_state.numPoints*sizeof(Particle)),0);

  std::vector<size_t> index[m_state.numPoints];
  std::iota(index->begin(), index->end(), 0);

  auto resetBuffer = [&](const size_t pos)
  {
    reset(pos); 
  };
  std::for_each(std::execution::par_unseq, index->cbegin(), index->cend(), resetBuffer);
}

//--------------------------------------------------------------------
void Particles::reset(const int idx)
{
  const int multiplier = m_config.show_trails ? 2 : 1;
  const auto pos = reinterpret_cast<Particle*>(m_buffer.data()) + (idx * multiplier);

  memset(pos, 0, multiplier * sizeof(Particle));
  pos->x = m_generator->get();
  pos->y = m_generator->get();

  Utils::hsv hsvColor((m_generator->get() + 1.0) * 180.0, 0.6 + 0.4 * m_generator->get(), 0.6 + 0.4 * m_generator->get());
  const auto rgbColor = Utils::hsv2rgb(hsvColor);
  pos->r = rgbColor.r;
  pos->g = rgbColor.g;
  pos->b = rgbColor.b;
  pos->a = 1.f;

  pos->w = m_config.point_size * (m_generator->get() + 1);

  if(m_config.show_trails)
    memcpy(pos+1, pos, sizeof(Particle));
};
