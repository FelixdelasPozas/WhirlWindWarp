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
     * \param[in] number total number of particles.
     * \param[in] generator random number generator.
     *
     */
    explicit Particle(State &state, const unsigned int number, Utils::NumberGenerator *generator);

    /** \brief Particle class virtual destructor.
     *
     */
    virtual ~Particle()
    {}

    void advance();

  private:
    /** \brief Initializes the particle container with random numbers.
     * \param[in] number number of points.
     *
     */
    void init(const unsigned int number);

    /** \brief Resets the values of the given point index.
     * \param[in] point point index.
     *
     */
    void reset(const int point);

    Utils::NumberGenerator          *m_generator;  /** random number generator in [-1.1].         */
    State                           &m_state;      /** application state.                         */
    std::vector<double>              m_x;          /** x position [-1.0, 1.0].                    */
    std::vector<double>              m_y;          /** y position [-1.0, 1.0].                    */
    std::vector<int>                 m_width;      /** trail width [1-3].                         */
    std::vector<std::vector<double>> m_tailX;      /** previous x positions.                      */
    std::vector<std::vector<double>> m_tailY;      /** previous y positions.                      */
    std::vector<Utils::rgb>          m_color;      /** color.                                     */
    bool                             m_drawTails;  /** true to draw point tails, false otherwise. */
    bool                             m_fadeTails;  /** true to fade the tails, false otherwise.   */
    size_t                           m_tailLength; /** number of previous positions to store.     */
};

#endif // PARTICLE_H_
