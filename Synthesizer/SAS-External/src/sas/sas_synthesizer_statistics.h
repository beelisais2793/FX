/* libsas - library for Structured Additive Synthesis
   Copyright (C) 1999-2001 Sylvain Marchand
   Copyright (C) 2001-2002 SCRIME, université Bordeaux 1

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef __SAS_SYNTHESIZER_STATISTICS_H__
#define __SAS_SYNTHESIZER_STATISTICS_H__

#include "sas_synthesizer.h"

/* Concrete data type of structures containing instantaneous
   information about a SAS synthesizer. */
struct sas_synthesizer_statistics_s
{
  int number_of_sources;
  int number_of_active_tracks;
  int number_of_masked_tracks;
  int number_of_audible_tracks;
};

/* Fills 'stats' with current information about 's'. */
extern void
sas_synthesizer_statistics (sas_synthesizer_t s,
			    struct sas_synthesizer_statistics_s * stats);

#endif
