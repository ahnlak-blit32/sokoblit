/*
 * sokoblit.cpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * Entry point to the game; this is also where we declare a few helper functions
 * which, if I was doing this properly, would be tidily wrapped up in some sort
 * of utility singleton.
 * But I'm not.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

/* System headers. */


/* Local headers. */

#include "32blit.hpp"
#include "sokoblit.hpp"

#include "Menu.hpp"


/* Global variables (yes, I know...) */

Menu     *g_menu = nullptr;
uimode_t  g_mode = MODE_TO_MENU;
uint8_t   g_level = 8;


/* Functions. */

/*
 * level_centre - works out the centre of a level, in world co-ordinates. Done
 *                here so everything has a common view about it!
 */

blit::Point level_centre( uint8_t p_level )
{
  blit::Point l_point = blit::Point( 0, 0 );

  /* The first 10 levels are pretty easy. */
  if ( p_level >= 1 && p_level <= 5 )
  {
    l_point.x = ( blit::screen.bounds.w * p_level ) - ( blit::screen.bounds.w / 2 );
    l_point.y = blit::screen.bounds.h * 0.5;
  }
  if ( p_level >= 6 && p_level <= 10 )
  {
    l_point.x = ( blit::screen.bounds.w * ( p_level - 5 ) ) - ( blit::screen.bounds.w / 2 );
    l_point.y = blit::screen.bounds.h * 1.5;
  }

  /* And the last 10, too */
  if ( p_level >= 13 && p_level <= 17 )
  {
    l_point.x = ( blit::screen.bounds.w * ( p_level - 12 ) ) - ( blit::screen.bounds.w / 2 );
    l_point.y = blit::screen.bounds.h * 3.5;
  }
  if ( p_level >= 18 && p_level <= 22 )
  {
    l_point.x = ( blit::screen.bounds.w * ( p_level - 17 ) ) - ( blit::screen.bounds.w / 2 );
    l_point.y = blit::screen.bounds.h * 4.5;
  }

  /* The side middle two.... we'll just hard code. */
  if ( 11 == p_level )
  {
    l_point.x = blit::screen.bounds.w * 0.5;
    l_point.y = blit::screen.bounds.h * 2.5;
  }

  if ( 12 == p_level )
  {
    l_point.x = blit::screen.bounds.w * 4.5;
    l_point.y = blit::screen.bounds.h * 2.5;
  }

  /* And return what we worked out. */
  return l_point;
}


/*
 * init - called when the game is launched, we create our globals, initialise
 *        the screen and that sort of thing.
 */

void init( void )
{
  /* Switch into hires mode, please. */
  blit::set_screen_mode( blit::ScreenMode::hires );

  /* Create the menu and game objects that handle everything. */
  g_menu = new Menu();
}


/*
 * render - called every frame to draw the world view
 */

void render( uint32_t p_time )
{
  /* Clear the screen down, so that whichever render does the work gets */
  /* a clean slate to work from.                                        */
  blit::screen.pen = blit::Pen( 0, 0, 0 );
  blit::screen.clear();

  /* Work out which tilemap(s) we should render, and render them. */
  //if ( MODE_GAME != g_mode )
  {
    if ( nullptr != g_menu )
    {
        g_menu->render( p_time );
    }
  }

  /* All done */
  return;
}


/*
 * update - called every 10ms to update the world view.
 */

void update( uint32_t p_time )
{
  static uint8_t l_progress = 0;

  /* If we're transitioning, update the progress. */
  if ( MODE_TO_GAME == g_mode )
  {
    /* Decrease progress, and if we hit zero we've reached GAME. */
    if ( 0 == --l_progress )
    {
      g_mode = MODE_GAME;
    }
  }

  if ( MODE_TO_MENU == g_mode )
  {
    /* Increase progress, and if we hit 100 we've reached MENU. */
    if ( 100 == ++l_progress )
    {
      g_mode = MODE_MENU;
    }
  }

  /* Check the menu button, which is a universal toggle. */
  if ( blit::buttons.pressed & blit::Button::MENU )
  {
    /* Only acts if we're in a steady state. */
    if ( MODE_MENU == g_mode )
    {
      g_mode = MODE_TO_GAME;
    }
    if ( MODE_GAME == g_mode )
    {
      g_mode = MODE_TO_MENU;
    }
  }

  /* Only bother updating object that are active. */
  if ( MODE_GAME != g_mode )
  {
    if ( nullptr != g_menu )
    {
      g_menu->update( p_time, l_progress );
    }
  }

  /* All done. */
  return;
}

/* End of file sokoblit.cpp */
