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

// C++
#include <vector>
#include <math.h>

class State;
class NumberGenerator;

/** \struct rgb
 * \brief Contains the red/green/blue color values. 
 */
struct rgb
{
  double r; // a fraction between 0 and 1
  double g; // a fraction between 0 and 1
  double b; // a fraction between 0 and 1

  rgb(): r{0}, g{0}, b{0}{};
  rgb(const double r_, const double g_, const double b_): r{r_}, g{g_}, b{b_}{};
};

/** \struct hsv
 * \brief Contains the hue/saturation/value color values. 
 */
struct hsv
{
  double h; // angle in degrees
  double s; // a fraction between 0 and 1
  double v; // a fraction between 0 and 1

  hsv(): h{0}, s{0}, v{0}{};
  hsv(const double h_, const double s_, const double v_): h{h_}, s{s_}, v{v_}{};
};

/** \brief Converts rgb to hsv 
 * 
 */
hsv rgb2hsv(rgb in);

/** \brief Converts hsv to rgb
 * 
 */
rgb hsv2rgb(hsv in);

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
    explicit Particle(State &state, const unsigned int number, NumberGenerator *generator);

    /** \brief Particle class virtual destructor.
     *
     */
    virtual ~Particle()
    {}

    virtual void paint();
    virtual void advance(int phase);

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

    NumberGenerator                 *m_generator;  /** random number generator in [-1.1].         */
    State                           &m_state;      /** application state.                         */
    std::vector<double>              m_x;          /** x position [-1.0, 1.0].                    */
    std::vector<double>              m_y;          /** y position [-1.0, 1.0].                    */
    std::vector<int>                 m_width;      /** trail width [1-3].                         */
    std::vector<std::vector<double>> m_tailX;      /** previous x positions.                      */
    std::vector<std::vector<double>> m_tailY;      /** previous y positions.                      */
    std::vector<rgb>                 m_color;      /** color.                                     */
    bool                             m_drawTails;  /** true to draw point tails, false otherwise. */
    bool                             m_fadeTails;  /** true to fade the tails, false otherwise.   */
    size_t                           m_tailLength; /** number of previous positions to store.     */
};

#endif // PARTICLE_H_
