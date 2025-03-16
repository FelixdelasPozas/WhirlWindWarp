/*
 File: Particle.h
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

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <Utils.h>

// C++
#include <vector>
#include <math.h>

class State;
namespace Utils
{
  class NumberGenerator;
}

/** \struct Particle
 * \brief Particle components.
 *
 */
struct __attribute__((__packed__)) Particle
{
  float x; /** x posision.          */
  float y; /** y position.          */
  float r; /** red component.       */
  float g; /** green component.     */
  float b; /** blue component.      */
  float a; /** alpha component.     */
  float w; /** particle/trail width */
};

/** \class Particle
 * \brief Implements a particle in the QGraphicsView
 *
 */
class Particles
{
  public:
    /** \brief Particle class constructor.
     * \param[in] state application state.
     * \param[in] generator random number generator.
     * \param[in] drawTrails true to generate points for the trail and false otherwise. 
     *
     */
    explicit Particles(State &state, Utils::NumberGenerator *generator, const Utils::Configuration &config);

    /** \brief Particle class virtual destructor.
     *
     */
    virtual ~Particles()
    {}

    /** \brief Advances the particles.
     * \param[in] timeIncrement Passed time since last frame.
     *
     */
    void advance();

    /** \brief Returns the buffer pointer.
     *
     */
    inline const float *buffer() const
    { return m_buffer.data(); }

  private:
    /** \brief Initializes the particle container with random numbers.
     *
     */
    void init();

    /** \brief Resets the values of the given point index.
     * \param[in] idx point index.
     *
     */
    void reset(const int idx);

    State                      &m_state;     /** application state.                   */
    Utils::NumberGenerator     *m_generator; /** random number generator in [-1.1].   */
    std::vector<float>          m_buffer;    /** data buffer.                         */
    const Utils::Configuration &m_config;    /** application configuration reference. */
};

#endif // PARTICLE_H_
