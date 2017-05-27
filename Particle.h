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

// Qt
#include <QGraphicsItem>

class State;
class NumberGenerator;

/** \class Particle
 * \brief Implements a particle in the QGraphicsView
 *
 */
class Particle
: public QGraphicsItem
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

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    virtual void advance(int phase) override;

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

    NumberGenerator     *m_generator;  /** random number generator in [-1.1].     */
    State               &m_state;      /** application state.                     */
    QList<double>        m_x;          /** x position [-1.0, 1.0].                */
    QList<double>        m_y;          /** y position [-1.0, 1.0].                */
    QList<int>           m_width;      /** trail width [1-3].                     */
    QList<QList<double>> m_tailX;      /** previous x positions.                  */
    QList<QList<double>> m_tailY;      /** previous y positions.                  */
    int                  m_tailLength; /** number of previous positions to store. */
    QList<QColor>        m_color;      /** color.                                 */
};

#endif // PARTICLE_H_
