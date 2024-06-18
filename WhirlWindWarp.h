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

// Project
#include <State.h>

/** \class WindWhirlWarp
 * \brief Main widget implementation.
 *
 */
class WhirlWindWarp
{
  public:
    /** \brief WindWhirlWarp class constructor.
     * \param[in] scene graphics scene.
     * \param[in] parent raw pointer of the QWidget parent of this one.
     *
     */
    explicit WhirlWindWarp(NumberGenerator *generator);

    /** \brief WindWhirlWarp class virtual destructor.
     *
     */
    virtual ~WhirlWindWarp();

    /** \brief Updates the state and calls advance() on the scene.
     *  Returns the number of milliseconds of the operation.
     *
     */
    void advance();

  private:
    /** \brief Initializes the state
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

    NumberGenerator *m_generator; /** random number generator in [-1,1]. */
    struct State     m_state;     /** application state.                 */
};

#endif // WHIRLWINDWARP_H_
