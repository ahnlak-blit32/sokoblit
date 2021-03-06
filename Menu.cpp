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
#include "sokoblit.hpp"

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
    c_menu_map = new blit::TileMap( c_menu_tiles, nullptr, blit::Size( 256, 256 ), c_menu_sprites );
  }

  /* And a few other defaults. */
  c_zoom = 100;

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
 * level_rect - determines the screen Rect for a given level, taking into 
 *              account the current zoom.
 */

blit::Rect Menu::level_rect( uint8_t p_level )
{
  blit::Rect l_rect = blit::Rect( 0, 0, 0, 0 );

  /* Work out the zoomed size of it. */
  l_rect.w = blit::screen.bounds.w / ( 1.0f + c_zoom / 25.0f );
  l_rect.h = blit::screen.bounds.h / ( 1.0f + c_zoom / 25.0f );

  /* And then work out the location, too - start with the map location. */
  blit::Point l_levelloc = level_centre( p_level );
  blit::Vec2 l_centre = blit::Vec2( l_levelloc.x, l_levelloc.y ); 
  l_centre.x -= blit::screen.bounds.w / 2;
  l_centre.y -= blit::screen.bounds.h / 2;
  l_centre -= l_centre * ( 1.0f - c_zoom / 100.0f );

  /* Apply the zoom transform. */
  l_rect.x = l_centre.x / ( 1.0f + c_zoom / 25.0f );
  l_rect.y = l_centre.y / ( 1.0f + c_zoom / 25.0f );

  /* All done. */
  return l_rect;
}


/*
 * map_transform - callback for the tilemap render, where we apply a suitable
 *                 level of zoom.
 */

blit::Mat3 Menu::map_transform( uint8_t p_scanline )
{
  blit::Mat3  l_transform = blit::Mat3::identity();
  blit::Point l_levelloc = level_centre( g_level );

  /* Centre things, scaled on zoom. */
  blit::Vec2 l_centre = blit::Vec2( l_levelloc.x, l_levelloc.y );
  l_centre += ( blit::Vec2( 800, 600 ) - l_centre ) * c_zoom / 100.0f;
  l_transform *= blit::Mat3::translation( l_centre );

  /* Then work out the scale vector, based on the current zoom percentage. */
  float l_scale = 1.0f + ( c_zoom / 25.0f );
  l_transform *= blit::Mat3::scale( blit::Vec2( l_scale, l_scale ) );

  /* Lastly, transform to the centre of the screen. */
  l_transform *= blit::Mat3::translation( blit::Vec2( blit::screen.bounds.w / 2 * -1, blit::screen.bounds.h / 2 * -1 ) );

  /* Return the transform we've ended up with. */
  return l_transform;
}


/*
 * update - updates the display state of the menu; the zoom factor is used for
 *          the transitioning from game to menu, and back.
 */

void Menu::update( uint32_t p_time, uint8_t p_zoom )
{
  /* Save the zoom factor, making sure it's a sensible value. */
  c_zoom = ( p_zoom > 100 ) ? 100 : p_zoom;

  /* All done. */
  return;
}


/*
 * render - draws the current state of the menu; largely handled by the tilemap.
 */

void Menu::render( uint32_t p_time )
{
  /* Set the alpha for all of this depending on the zoom. */
  uint8_t l_previous_alpha = blit::screen.alpha;
  blit::screen.alpha = c_zoom < 20 ? 0 : ( c_zoom - 20 ) * 3.18;

  /* Ask the tilemap to draw itself, as a suitble zoom & alpha. */
  if ( nullptr != c_menu_map )
  {
    c_menu_map->draw( &blit::screen, blit::screen.clip, std::bind( &Menu::map_transform, this, std::placeholders::_1 ) );
  }

  /* Draw a pulsing rectangle around the current level. */
  blit::Rect l_level = level_rect( g_level );
  blit::screen.pen = blit::Pen( 250, ( p_time % 255 ), 150 + ( p_time % 105 ) );
  blit::screen.h_span( l_level.tl(), l_level.w );
  blit::screen.h_span( l_level.bl(), l_level.w );
  blit::screen.v_span( l_level.tl(), l_level.h );
  blit::screen.v_span( l_level.tr(), l_level.h );

  /* Reset the alpha to what it was before. */
  blit::screen.alpha = l_previous_alpha;

  /* All done. */
  return;
}


/* End of file Menu.cpp */
