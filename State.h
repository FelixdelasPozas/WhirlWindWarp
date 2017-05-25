/*
		File: State.h
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

#ifndef STATE_H_
#define STATE_H_

// C++
#include <random>

// Qt
#include <QString>
#include <QMutex>

static const int fs = 16;      /** number of forcefields.    */

/** \struct State
 * \brief Implements screensaver state.
 *
 */
struct State
{
    QString name[fs];           /** The force fields and their parameters.             */
    bool    enabled[fs];        /** Is field on or off?                                */
    float   var[fs];            /** Current parameter.                                 */
    float   optimum[fs];        /** Optimum (central/mean) value.                      */
    float   acceleration[fs];   /** acceleration?                                      */
    float   velocity[fs];       /** velocity?                                          */
    int     numPoints;          /** Number of points.                                  */
    int     tailLenght;         /** tail length.                                       */
    bool    initted;            /** true if inited and false otherwise. ??             */
    bool    changedColor;       /** true if changed a point color in the last frame.   */
    int     hue;                /** hue value.                                         */
};

class NumberGenerator
{
  public:
    explicit NumberGenerator(const float min, const float max);

    const float get();
  private:
    std::default_random_engine             m_generator;    /** random number generator.               */
    std::uniform_real_distribution<double> m_distribution; /** uniform real distribution in [min,max] */
    QMutex                                 mutex;          /** mutex                                  */
};

#endif // STATE_H_
