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
