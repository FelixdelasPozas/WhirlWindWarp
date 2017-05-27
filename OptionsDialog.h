/*
 File: OptionsDialog.h
 Created on: 27/05/2017
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

#ifndef OPTIONSDIALOG_H_
#define OPTIONSDIALOG_H_

// Qt
#include <QDialog>
#include <ui_OptionsDialog.h>

/** \class OptionsDialog
 * \brief Implements the configuration dialog for the screensaver.
 *
 */
class OptionsDialog
: public QDialog
, private Ui::OptionsDialog
{
    Q_OBJECT
  public:
    /** \brief OptionsDialog class constructor.
     *
     */
    explicit OptionsDialog();

    /** \brief OptionsDialog class virtual destructor.
     *
     */
    virtual ~OptionsDialog()
    {}

  public slots:
    virtual void accept() override;

  private slots:
    /** \brief Updates the UI when the value of draw tails checkbox changes.
     *
     */
    void onDrawTailsChanged(int state);
};

#endif // OPTIONSDIALOG_H_
