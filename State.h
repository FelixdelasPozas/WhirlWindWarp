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

static const QString ANTIALIAS_KEY   = QString("Antialias");
static const QString DRAW_TAILS_KEY  = QString("Draw Tails");
static const QString TAIL_LENGTH_KEY = QString("Tail Length");
static const QString TAIL_FADE_KEY   = QString("Tail Fade");
static const QString FPS_KEY         = QString("FPS");

static const QString COMPANY_NAME     = QString("Felix de las Pozas Alvarez");
static const QString APPLICATION_NAME = QString("WhirlWindWarp");

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

/** \class NumberGenerator
 * \brief Implements a shared random number generator between the given limits.
 *
 */
class NumberGenerator
{
  public:
    /** \brief NumberGenerator class constructor.
     * \param[in] min lower limit.
     * \param[in] max upper limit.
     *
     */
    explicit NumberGenerator(const float min, const float max);

    /** \brief Returns a random number.
     *
     */
    const float get();
  private:
    std::default_random_engine             m_generator;    /** random number generator.               */
    std::uniform_real_distribution<double> m_distribution; /** uniform real distribution in [min,max] */
    QMutex                                 mutex;          /** mutex                                  */
};

#endif // STATE_H_
