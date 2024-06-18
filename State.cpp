/*
		File: State.cpp
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


// Project
#include "State.h"

// C++
#include <cstdlib>
#include <ctime>

//--------------------------------------------------------------------
NumberGenerator::NumberGenerator(const float min, const float max)
: m_distribution{min, max}
{
  std::srand(std::time(0));
  m_generator.discard(std::rand() % 200);
}

//--------------------------------------------------------------------
const float NumberGenerator::get()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_distribution(m_generator);
}



