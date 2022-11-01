/* Copyright 2013-2015 Matt Tytel
 *
 * cursynth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cursynth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cursynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cursynth_strings.h"

namespace mopo {

  const char* CursynthStrings::filter_strings_[] = {
    "low pass",
    "high pass",
    "band pass",
    "all pass"
  };

  const char* CursynthStrings::legato_strings_[] = {
    "off",
    "on"
  };

  const char* CursynthStrings::portamento_strings_[] = {
    "off",
    "auto",
    "on"
  };

  const char* CursynthStrings::wave_strings_[] = {
    "sin",
    "triangle",
    "square",
    "down saw",
    "up saw",
    "three step",
    "four step",
    "eight step",
    "three pyramid",
    "five pyramid",
    "nine pyramid",
    "white noise",
  };
} // namespace mopo
