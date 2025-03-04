/*
    File: WhirlWindWarp.h
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
#include <WhirlWindWarp.h>
#include <Utils.h>

//--------------------------------------------------------------------
WhirlWindWarp::WhirlWindWarp(const int numPoints, const bool drawTails, Utils::NumberGenerator *generator)
: m_generator{generator}
, m_particles{nullptr}
{
  m_state.initted = false;
  m_state.numPoints = numPoints;
  m_state.drawTails = drawTails;

  if(!generator)
    m_generator = new Utils::NumberGenerator(-1.f, 1.f);

  init();
}

//--------------------------------------------------------------------
void WhirlWindWarp::advance()
{
  preUpdateState();

  m_particles->advance();
  
  postUpdateState();
}

//--------------------------------------------------------------------
// Adjust a variable var about optimum op,
// with damp = dampening about op
// force = force of random perturbation.
float WhirlWindWarp::stars_perturb(float variable, float optimum, float damp, float force)
{
  return ((optimum) + (damp)*((variable)-(optimum)) + (force)* m_generator->get() / 4.0);
}

//--------------------------------------------------------------------
// Turns a forcefield on, and ensures its vars are suitable.
void WhirlWindWarp::turn_on_field(int ff)
{
  if (!m_state.enabled[ff])
  {
    // acc[ff]=0.0;
    m_state.acceleration[ff] = 0.02 * m_generator->get();
    m_state.velocity[ff] = 0.0;
    m_state.var[ff] = m_state.optimum[ff];
  }

  m_state.enabled[ff] = true;

  if (ff == 10)
  {
    turn_on_field(11);
    turn_on_field(12);
  }
  if (ff == 13)
  {
    turn_on_field(14);
    turn_on_field(15);
  }
}

//--------------------------------------------------------------------
void WhirlWindWarp::init()
{
  if(m_state.initted) return;

  m_state.initted = true;

  // Set up central (optimal) points for each different forcefield.
  m_state.optimum[1] = 1;
  m_state.name[1] = "Warp";
  m_state.optimum[2] = 0;
  m_state.name[2] = "Rotation";
  m_state.optimum[3] = 1;
  m_state.name[3] = "Horizontal asymptote";
  m_state.optimum[4] = 0;
  m_state.name[4] = "Vertical asymptote";
  m_state.optimum[5] = 1;
  m_state.name[5] = "Vertical asymptote right";
  m_state.optimum[6] = 1;
  m_state.name[6] = "Squirge x";
  m_state.optimum[7] = 1;
  m_state.name[7] = "Squirge y";
  m_state.optimum[0] = 0;
  m_state.name[0] = "Split number (inactive)";
  m_state.optimum[8] = 0;
  m_state.name[8] = "Split velocity x";
  m_state.optimum[9] = 0;
  m_state.name[9] = "Split velocity y";
  m_state.optimum[10] = 0;
  m_state.name[10] = "Horizontal wave amplitude";
  m_state.optimum[11] = m_generator->get() * 3.141;
  m_state.name[11] = "Horizontal wave phase (inactive)";
  m_state.optimum[12] = 0.01;
  m_state.name[12] = "Horizontal wave frequency (inactive)";
  m_state.optimum[13] = 0;
  m_state.name[13] = "Vertical wave amplitude";
  m_state.optimum[14] = m_generator->get() * 3.141;
  m_state.name[14] = "Vertical wave phase (inactive)";
  m_state.optimum[15] = 0.01;
  m_state.name[15] = "Vertical wave frequency (inactive)";

  /* Initialise parameters to optimum, all off */
  for (int i = 0; i < fs; ++i)
  {
    m_state.var[i] = m_state.optimum[i];
    m_state.enabled[i] = (m_generator->get() > 0.5 ? true : false);
    m_state.acceleration[i] = 0.02 * m_generator->get();
    m_state.velocity[i] = 0;
  }

  m_state.hue = 180 + 180 * m_generator->get();
  m_state.tailLenght = 5;

  if (!m_particles)
    m_particles = std::make_unique<Particle>(m_state, m_generator);
}

//--------------------------------------------------------------------
void WhirlWindWarp::preUpdateState()
{
  m_state.changedColor = false;

  if (!m_state.initted)
    init();
}

//--------------------------------------------------------------------
void WhirlWindWarp::postUpdateState()
{
  /* Adjust force fields */
  int numEnabled = 0;
  for (int i = 0; i < fs; ++i)
  {
    /* Adjust forcefield's parameter */
    if (m_state.enabled[i])
    {
      // This configuration produces var[f]s usually below 0.01.
      m_state.acceleration[i] = stars_perturb(m_state.acceleration[i], 0, 0.98, 0.005);
      m_state.velocity[i] = stars_perturb(m_state.velocity[i] + 0.03 * m_state.acceleration[i], 0, 0.995, 0.0);
      m_state.var[i] = m_state.optimum[i] + (m_state.var[i] - m_state.optimum[i]) * 0.9995 + 0.001 * m_state.velocity[i];
    }

    // Decide whether to turn this forcefield on or off.
    // prob_on makes the "splitting" effects less likely than the rest.
    auto prob_on = [this, i] () { return ((i == 8 || i == 9) ? 0.999975 : 0.9999); };

    if (!m_state.enabled[i] && (m_generator->get() > prob_on()))
    {
      turn_on_field(i);
    }
    else
    {
      if (m_state.enabled[i] && (m_generator->get() > 0.99) && fabs(m_state.var[i] - m_state.optimum[i]) < 0.0005
          && fabs(m_state.velocity[i]) < 0.005 /* && fabs(acc[f])<0.01 */)
      {
        /* We only turn it off if it has gently returned to its optimal (as opposed to rapidly passing through it). */
        m_state.enabled[i] = false;
      }
    }

    if (m_state.enabled[i]) ++numEnabled;
  }

  // Ensure at least three forcefields are on.
  // BUG: Picking randomly might not be enough since 0,11,12,14 and 15 do nothing!
  // But then what's wrong with a rare gentle twinkle?!
  while(numEnabled < 3)
  {
    auto index = std::rand() % fs;
    if(m_state.enabled[index]) continue;

    ++numEnabled;
    turn_on_field(index);
  }
}
