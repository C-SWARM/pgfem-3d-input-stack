/*

              ####### #         ###   #     #   ###   ######
              #       #          #     #   #     #    #     #
              #       #          #      # #      #    #     #
              #####   #          #       #       #    ######
              #       #          #      # #      #    #   #
              #       #          #     #   #     #    #    #
              ####### #######   ###   #     #   ###   #     #

                   
                  Copyright: 1994 Petr Krysl

   Czech Technical University in Prague, Faculty of Civil Engineering,
      Dept. Structural Mechanics, 166 29 Prague, Czech Republic,
                  email: pk@power2.fsv.cvut.cz
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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "Econfig.h"
#include <stdlib.h>
#include <limits.h>
#include "Edither.h"

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

static float one_minus_lower_bound = 1.;
static float lower_bound = 0;
static double inv_long_max = 0;
static EPixel dither_background = 0x0;
static int seed = 1;

#ifdef HAVE_RANDOM
extern void srandom();
/* extern int srandom(); */
/* extern long random(); */
/* extern int random();  */
#   define RANDOM_RETURNS_MAX LONG_MAX
#else  /* !AIXV3 */
#   ifdef HAVE_RAND
#      include <stdlib.h>
#      define random() rand()
#      define srandom(_seed) srand(_seed)
#      define RANDOM_RETURNS_MAX 32767
#   else /* everything else */
       I am really bad off without any random generator;
#      define RANDOM_RETURNS_MAX LONG_MAX
#   endif
#endif

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

void DitherInit(double smallest_cos, EPixel dither_bground)
{
  lower_bound = smallest_cos;
  one_minus_lower_bound = 1 - lower_bound;
  inv_long_max = 1.0/RANDOM_RETURNS_MAX;
  dither_background = dither_bground;
  srandom(seed);
  seed = random();
}

/* ========================================================================= */

EPixel DitherColorValue(float cos_between_normal_and_light,
			     EPixel normal_color)
{
  double adjst_cos;

  adjst_cos =
    lower_bound + one_minus_lower_bound * cos_between_normal_and_light;
  adjst_cos = max(lower_bound, adjst_cos);
  if (random()*inv_long_max < adjst_cos)
    return normal_color;
  else
    return dither_background;
}

