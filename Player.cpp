/*
 * Player.cpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Player class handles the player on an individual level. This is where
 * we work out things like animations, position and suchlike.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */


/* System headers. */

#include <cstring>

/* Local headers. */

#include "32blit.hpp"
#include "sokoblit.hpp"

#include "Player.hpp"


/* Functions. */

/*
 * Player - constructor, basic initialisation. Takes the co-ordinates of the
 *          starting location, in tiles.
 */

Player::Player( uint16_t p_x, uint16_t p_y )
{
  /* Save the location we've been given. */
  c_location.x = p_x;
  c_location.y = p_y;

  /* And set some defaults. */
  c_direction = DIR_DOWN;
  c_animation = 0;
  c_steps = 0;
  c_blocked = false;
  c_pushing = false;

  /* All done! */
  return;
}


/*
 * ~Player - destructor, just tidy up what we allocated.
 */

Player::~Player( void )
{
  /* All done. */
  return;
}


/*
 * moving - returns a boolean flag indicating if the player is in motion.
 */

bool Player::moving( void )
{
  /* Pretty simple access method. */
  return c_steps > 0;
}


/*
 * pushing - returns a boolean flag indicating if the player is shoving a crate.
 */

bool Player::pushing( void )
{
  /* Pretty simple access method. */
  return c_pushing;
}


/*
 * direction - which way is the player facing?
 */

direction_t Player::facing( void )
{
  return c_direction;
}


/*
 * render - draws the player onto the screen; assumes that the screen has an
 *          appropriate spritesheet with sprites in the right place!
 */

void Player::render( void )
{
  blit::Rect l_sprite = blit::Rect( 0, 4, 2, 2 );
  blit::Point l_location = c_location * 8;
  blit::Point l_crate_loc;

  /* Work out the correct rectangle to blit, based on the direction. Also the */
  /* precise location is offset if we're still moving.                        */
  switch( c_direction )
  {
    case DIR_DOWN:
      if ( !c_blocked )
      {
        l_location.y -= c_steps;
      }
      l_crate_loc = l_location + blit::Point( 0, 16 );
      break;    
    case DIR_LEFT:
      l_sprite.x = 6;
      l_sprite.y = 6;
      if ( !c_blocked )
      {
        l_location.x += c_steps;
      }
      l_crate_loc = l_location - blit::Point( 16, 0 );
      break;
    case DIR_UP:
      l_sprite.y = 6;
      if ( !c_blocked )
      {
        l_location.y += c_steps;
      }
      l_crate_loc = l_location - blit::Point( 0, 16 );
      break;
    case DIR_RIGHT:
      l_sprite.x = 6;
      if ( !c_blocked )
      {
        l_location.x -= c_steps;
      }
      l_crate_loc = l_location + blit::Point( 16, 0 );
      break;
  }

  /* Now the animation steps, which are just along the X axis. */
  l_sprite.x += ( ( c_steps % 3 ) * 2 );

  /* And just send the right sprite to the right location. */
  blit::screen.sprite( l_sprite, l_location );

  /* And the crate, if we're pushing that. */
  if ( c_pushing )
  {
    blit::screen.sprite( blit::Rect( 4, 0, 2, 2 ), l_crate_loc );
  }

  /* All done. */
  return;
}


/*
 * update - called every tick to keep the animation running and location 
 *          changing when the player is moving.
 */

void Player::update( void )
{
  static uint8_t l_delay = 2;

  if ( l_delay > 0 )
  {
    l_delay--;
    return;
  }
  l_delay = 2;

  /* Only need to do stuff if the steps are there. */
  if ( c_steps > 0 )
  {
    /* Just decrement the steps. */
    c_steps -= 2;
  }

  /* If we've reached the end of the movement, clear the blocked flag. */
  if ( 0 == c_steps )
  {
    c_blocked = false;
    c_pushing = false;
  }

  /* That's it, all done. */
  return;
}


/*
 * location  - returns the (tile-based) location of the player.
 */

blit::Point Player::location( void )
{
  /* Simple access method. */
  return c_location;
}


/*
 * move - starts the player moving in the direction specified. If the blocked
 *        flag is true, we run the animation but simply don't move.
 */

void Player::move( direction_t p_direction, bool p_blocked, bool p_pushing )
{
  /* Check that the new location makes sense, and switch to it. */
  if ( !p_blocked )
  {
    switch( p_direction )
    {
      case DIR_DOWN:
        if ( c_location.y < 28 )
        {
          c_location.y += 2;
        }
        else return;
        break;
      case DIR_LEFT:
        if ( c_location.x > 1 )
        {
          c_location.x -= 2;
        }
        else return;
        break;
      case DIR_UP:
        if ( c_location.y > 1 )
        {
          c_location.y -= 2;
        }
        else return;
        break;
      case DIR_RIGHT:
        if ( c_location.x < 38 )
        {
          c_location.x += 2;
        }
        else return;
        break;
    }
  }

  /* And then remember what direction we're moving... */
  c_direction = p_direction;
  c_steps = 16;
  c_blocked = p_blocked;
  c_pushing = p_pushing;

  /* All done. */
  return;
}


/* End of file Game.cpp */
