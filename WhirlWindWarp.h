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

#ifndef WHIRLWINDWARP_H_
#define WHIRLWINDWARP_H_

#include <Particle.h>

// C++
#include <memory>
#include <string>

namespace Utils
{
  class NumberGenerator;
}

static const int fs = 16; /** number of forcefields.    */

/** \struct State
 * \brief Implements screensaver state.
 *
 */
struct State
{
    std::string name[fs];           /** The force fields and their parameters.           */
    bool        enabled[fs];        /** Is field on or off?                              */
    float       var[fs];            /** Current parameter.                               */
    float       optimum[fs];        /** Optimum (central/mean) value.                    */
    float       acceleration[fs];   /** acceleration?                                    */
    float       velocity[fs];       /** velocity?                                        */
    int         numPoints;          /** Number of points.                                */
    bool        initted;            /** true if inited and false otherwise.              */
    bool        changedColor;       /** true if changed a point color in the last frame. */
    int         hue;                /** hue value.                                       */
};

/** \class WindWhirlWarp
 * \brief Main widget implementation.
 *
 */
class WhirlWindWarp
{
  public:
    /** \brief WindWhirlWarp class constructor.
     * \param[in] numPoints total number of points.
     * \param[in] drawTails true to draw particle trails and false otherwise.
     * \param[in] generator random number generator class pointer.
     *
     */
    explicit WhirlWindWarp(const int numPoints, const bool drawTails = false, Utils::NumberGenerator *generator = nullptr);

    /** \brief Updates the state and calls advance() on the scene.
     * \param[in] time_ Current time.
     *
     */
    void advance(const float time_);

    /** \brief Returns the buffer to use in OpenGL
     *
     */
    inline const float *buffer() const
    { return m_particles->buffer(); }

  private:
    /** \brief Initializes the particles buffer. 
     *
     */
    void init();

    /** \brief Updates the state prior to advancing the scene.
     *
     */
    void preUpdateState();

    /** \brief Updates the state after advancing the scene.
     *
     */
    void postUpdateState();

    /** \brief Helper method to enable a force field.
     * \param[in] ff force field index.
     */
    void turn_on_field(int ff);

    /** \brief Helper method to modify a force field value.
     * \param[in] var variable
     * \param[in] op optimum value.
     * \param[in] damp dampening.
     * \param[in] force field force.
     *
     */
    float stars_perturb(float var, float op, float damp, float force);

    Utils::NumberGenerator    *m_generator; /** random number generator in [-1,1]. */
    struct State               m_state;     /** application state.                 */
    std::unique_ptr<Particles> m_particles; /** particles */
};

#endif // WHIRLWINDWARP_H_
