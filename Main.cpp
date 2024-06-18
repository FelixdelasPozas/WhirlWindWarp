/*
 File: Main.cpp
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

// Project
#include "WhirlWindWarp.h"
#include "State.h"
#include "version.h"

// C++
#include <iostream>
#include <windows.h>
#include <winuser.h>

//-----------------------------------------------------------------
void getScreenResolution()
{

}

//-----------------------------------------------------------------
void showHelp()
{
  std::cout << "WhirlWindWarp v" << WHIRLWINDWARP_VERSION << std::endl;
}

//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
  int resultValue = 0;

  // QCommandLineParser uses unix-like '-' for flags, while windows uses '/'. There is no way for QCommandLineParser to adapt to it
  // so we'll need to change all characters first. Also the 'c' option can have an optional value, can be present or not, and
  // QCommandLineParser also doesn't support that so we'll need to truncate the argument to void that value (parent window handler?).
  for(int i = 1; i < argc; ++i)
  {
    bool dash = false;
    bool clear = false;
    for(char *j = argv[i]; *j; ++j)
    {


      if(*j == '/')
      {
        *j = '-';
        dash = true;
      }
      else
      {
        if(dash)
        {
          if(*j == 'c' || *j == 'C') clear = true;
          dash = false;
        }
        else
        {
          if(clear)
          {
            *j = '\0';
            break;
          }
        }
      }
    }
  }

  if(argc == 1)
  {
    showHelp();
    return 0;
  }

  // Note: custom help dialog is shown instead of the one created by parser due to '/' instead of '-'. Also the
  // configuration parameter has an optional 'undocumented' value passed by windows screensaver configurator.
  // We don't have a settings dialog so we'll pass on any value given.
  // auto startOption   = QCommandLineOption{QStringList() << "s" << "S", "Starts the screensaver."};
  // auto previewOption = QCommandLineOption{QStringList() << "p" << "P", "Shows the preview in dialog handle <handle>.", "unusedvalue", "None"};
  // auto configOption  = QCommandLineOption{QStringList() << "c" << "C", "Shows the configuration dialog."};
  // auto helpgOption   = QCommandLineOption{QStringList() << "h" << "H", "Shows the screensaver help."};

  return resultValue;
}
