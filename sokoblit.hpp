/*
 * sokoblit.hpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * Main header file, which declares global-scope functions, structures and
 * constants.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

#ifndef   _SOKOBLIT_HPP_
#define   _SOKOBLIT_HPP_

#include "32blit.hpp"

#define  SOKOBLIT_LEVEL_MAX   22

/* Constants based on tiled tiles - tinker at your peril! */

#define TILED_PLAYER_HOME 76
#define TILED_EMPTY       34
#define TILED_CRATE       4
#define TILED_WALL        2
#define TILED_CRATE_HOME  36

typedef enum 
{
  MODE_MENU,
  MODE_TO_GAME,
  MODE_GAME,
  MODE_TO_MENU,
  MODE_MAX
} uimode_t;

extern uint8_t g_level;

blit::Point level_centre( uint8_t );


#endif /* _SOKOBLIT_HPP_ */

/* End of file sokoblit.hpp */
