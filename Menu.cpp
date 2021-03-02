/*
 * Menu.cpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Menu class handles everything to do with the game menu, which allows
 * the player to choose which levels to play.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

/* System headers. */

#include <cstring>

/* Local headers. */

#include "32blit.hpp"
#include "Menu.hpp"
#include "assets_tiled.hpp"


/* Functions. */

/*
 * Menu - constructor, where we load up our tile map stuff.
 */

Menu::Menu( void )
{
  /* Load up the spritesheet we'll be using. */
  c_menu_sprites = blit::Surface::load( at_menu_sprites );

  /* And the tile map, too - copied into a malleable chunk of memory. */
  c_menu_tiles = new uint8_t[at_menu_map_length];
  if ( nullptr != c_menu_tiles )
  {
    memcpy( c_menu_tiles, at_menu_map, at_menu_map_length );
    c_menu_map = new blit::TileMap( c_menu_tiles, nullptr, blit::Size( 100, 75 ), c_menu_sprites );
  }

  /* All done! */
  return;
}


/*
 * ~Menu - destructor, just tidy up what we allocated.
 */

Menu::~Menu( void )
{
  /* Free the tilemap. */
  if ( nullptr != c_menu_map )
  {
    delete c_menu_map;
    c_menu_map = nullptr;
  }
  if ( nullptr != c_menu_tiles )
  {
    delete c_menu_tiles;
    c_menu_tiles = nullptr;
  }

  /* And the sprites. */
  if ( nullptr != c_menu_sprites )
  {
    delete c_menu_sprites;
    c_menu_sprites = nullptr;
  }

  /* All done. */
  return;
}


/*
 * update - updates the display state of the menu; the zoom factor is used for
 *          the transitioning from game to menu, and back.
 */

void Menu::update( uint32_t p_time, uint8_t p_zoom )
{
  /* All done. */
  return;
}


/*
 * render - draws the current state of the menu; largely handled by the tilemap.
 */

void Menu::render( uint32_t p_time )
{
  /* All done. */
  return;
}


/* End of file Menu.cpp */
