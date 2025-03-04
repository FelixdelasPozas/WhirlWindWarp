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

/** \class Particle
 * \brief Implements a particle in the QGraphicsView
 *
 */
class Particle
{
  public:
    /** \brief Particle class constructor.
     * \param[in] state application state.
     * \param[in] generator random number generator.
     *
     */
    explicit Particle(State &state, Utils::NumberGenerator *generator);

    /** \brief Particle class virtual destructor.
     *
     */
    virtual ~Particle()
    {}

    /** \brief Advances the particles.
     *
     */
    void advance();

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

    Utils::NumberGenerator          *m_generator;  /** random number generator in [-1.1].         */
    State                           &m_state;      /** application state.                         */
    std::vector<float>               m_buffer;     /** data buffer.                               */

    /* Format of the buffer (Size per particle 7 floats) */
    /* - Position 2 floats                               */
    /* - Color RGBA 4 floats                             */
    /* - Width 1 float                                   */
};

#endif // PARTICLE_H_
