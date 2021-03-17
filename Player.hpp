/*
 * Player.hpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Player class handles the player on an individual level. This is where
 * we work out things like animations, position and suchlike.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

#ifndef   _PLAYER_HPP_
#define   _PLAYER_HPP_

#include "32blit.hpp"
#include "sokoblit.hpp"

#define ANIMATION_FRAMES  3

typedef enum
{
  DIR_NONE,
  DIR_DOWN,
  DIR_LEFT,
  DIR_UP,
  DIR_RIGHT
} direction_t;

class Player
{
  private:
    blit::Point   c_location;
    direction_t   c_direction;
    uint8_t       c_animation;
    uint8_t       c_steps;
    bool          c_blocked;
    bool          c_pushing;

  public:
                  Player( uint16_t, uint16_t );
                 ~Player( void );
    bool          moving( void );
    bool          pushing( void );
    void          render( void );
    void          update( void );
    blit::Point   location( void );
    direction_t   facing( void );
    void          move( direction_t, bool, bool );
};

#endif /* _PLAYER_HPP_ */

/* End of file Player.hpp */
