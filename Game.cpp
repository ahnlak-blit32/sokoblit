/*
 * Game.cpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Game class handles everything to do with the game itself, letting the
 * player scurry around pushing boxes and generally living the dream.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */


/* System headers. */

#include <cstring>

/* Local headers. */

#include "32blit.hpp"
#include "sokoblit.hpp"

#include "Game.hpp"
#include "assets_tiled.hpp"


/* Functions. */

/*
 * Game - constructor, where we load up our tile map stuff.
 */

Game::Game( void )
{
  /* Load up the spritesheet we'll be using, attach it to the screen too. */
  c_game_sprites = blit::Surface::load( at_game_sprites );
  blit::screen.sprites = c_game_sprites;

  /* And the tile map, too - copied into a malleable chunk of memory. */
  c_game_tiles = new uint8_t[at_game_map_length];
  if ( nullptr == c_game_tiles )
  {
    /* Erk, this is bad, bail out now. */
    return;
  }
  memcpy( c_game_tiles, at_game_map, at_game_map_length );
  c_game_map = new blit::TileMap( c_game_tiles, nullptr, blit::Size( 256, 256 ), c_game_sprites );

  /* Scan each level to find the player's starting location. */
  for( uint8_t l_level = 0; l_level < SOKOBLIT_LEVEL_MAX; l_level++ )
  {
    blit::Point l_origin = level_tile_origin( l_level );
    blit::Point l_tile;

    /* Work through all the tiles, looking for the player. Sprites are 2x2 */
    /* though, so we only need to check alternate rows/columns...          */
    bool l_found = false;
    for ( uint8_t y = 0; y < 30; y += 2 )
    {
      l_tile.y = l_origin.y + y;
      for ( uint8_t x = 0; x < 40; x += 2 )
      {
        l_tile.x = l_origin.x + x;
        if ( c_game_map->tile_at( l_tile ) == TILED_PLAYER_HOME )
        {
          c_player[l_level].location.x = x * 8;
          c_player[l_level].location.y = y * 8;
          l_found = true;
          break;
        }
      }
      if ( l_found ) break;
    }
    c_player[l_level].direction = DIR_DOWN;
    c_player[l_level].animation = 0;
  }

  /* And a few other defaults. */
  c_zoom = 1;

  /* All done! */
  return;
}


/*
 * ~Game - destructor, just tidy up what we allocated.
 */

Game::~Game( void )
{
  /* Free the tilemap. */
  if ( nullptr != c_game_map )
  {
    delete c_game_map;
    c_game_map = nullptr;
  }
  if ( nullptr != c_game_tiles )
  {
    delete c_game_tiles;
    c_game_tiles = nullptr;
  }

  /* And the sprites. */
  if ( nullptr != c_game_sprites )
  {
    delete c_game_sprites;
    c_game_sprites = nullptr;
  }

  /* All done. */
  return;
}


/*
 * level_rect - determines the screen Rect for a given level, taking into 
 *              account the current zoom.
 */

blit::Rect Game::level_rect( uint8_t p_level )
{
  blit::Rect l_rect = blit::Rect( 0, 0, 0, 0 );

  /* Work out the zoomed size of it. */
  l_rect.w = blit::screen.bounds.w / ( 1.0f + c_zoom / 25.0f ) - 2;
  l_rect.h = blit::screen.bounds.h / ( 1.0f + c_zoom / 25.0f ) - 2;

  /* And then work out the location, too - start with the map location. */
  blit::Point l_levelloc = level_centre( p_level );
  blit::Vec2 l_centre = blit::Vec2( l_levelloc.x, l_levelloc.y ); 
  l_centre.x -= blit::screen.bounds.w / 2;
  l_centre.y -= blit::screen.bounds.h / 2;
  l_centre -= l_centre * ( 1.0f - c_zoom / 100.0f );

  /* Apply the zoom transform. */
  l_rect.x = l_centre.x / ( 1.0f + c_zoom / 25.0f ) + 1;
  l_rect.y = l_centre.y / ( 1.0f + c_zoom / 25.0f ) + 1;

  /* All done. */
  return l_rect;
}


/*
 * level_tile_origin - determines the origin (in tiles) of the specified level,
 *                     so that we can easily index into the tilemap.
 */

blit::Point Game::level_tile_origin( uint8_t p_level )
{
  blit::Point l_origin = blit::Point( 0, 0 );

  /* Top two rows are easy. */
  if ( ( p_level >= 1 ) && ( p_level <= 5 ) )
  {
    l_origin.x = 40 * ( p_level - 1 );
    l_origin.y = 0;
  }
  if ( ( p_level >= 6 ) && ( p_level <= 10 ) )
  {
    l_origin.x = 40 * ( p_level - 6 );
    l_origin.y = 30;
  }

  /* And the last two rows. */
  if ( ( p_level >= 13 ) && ( p_level <= 17 ) )
  {
    l_origin.x = 40 * ( p_level - 13 );
    l_origin.y = 90;
  }
  if ( ( p_level >= 18 ) && ( p_level <= 22 ) )
  {
    l_origin.x = 40 * ( p_level - 18 );
    l_origin.y = 120;
  }

  /* Two edge cases we may as well hard code. */
  if ( 11 == p_level )
  {
    l_origin.x = 0;
    l_origin.y = 60;
  }
  if ( 12 == p_level )
  {
    l_origin.x = 160;
    l_origin.y = 60;
  }

  /* And return the origin. */
  return l_origin;
}


/*
 * map_transform - callback for the tilemap render, where we apply a suitable
 *                 level of zoom.
 */

blit::Mat3 Game::map_transform( uint8_t p_scanline )
{
  blit::Mat3  l_transform = blit::Mat3::identity();
  blit::Point l_levelloc = level_centre( g_level );

  /* Centre things, scaled on zoom. */
  blit::Vec2 l_centre = blit::Vec2( l_levelloc.x, l_levelloc.y );

  /* At a zoom level of 0, this can be a lot simpler. */
  if ( 0 == c_zoom )
  {
    /* Centreing is easy. */
    l_transform *= blit::Mat3::translation( l_centre );
  }
  else
  {
    /* Centre has to reflect the zoom level. */
    l_centre += ( blit::Vec2( 800, 600 ) - l_centre ) * c_zoom / 100.0f;
    l_transform *= blit::Mat3::translation( l_centre );

    /* Then work out the scale vector, based on the current zoom percentage. */
    float l_scale = 1.0f + ( c_zoom / 25.0f );
    l_transform *= blit::Mat3::scale( blit::Vec2( l_scale, l_scale ) );
  }

  /* Lastly, transform to the centre of the screen. */
  l_transform *= blit::Mat3::translation( blit::Vec2( blit::screen.bounds.w / 2 * -1, blit::screen.bounds.h / 2 * -1 ) );

  /* Return the transform we've ended up with. */
  return l_transform;
}


/*
 * update - updates the display state of the game
 */

void Game::update( uint32_t p_time )
{
  /* We only respond to user input when we're fully zoomed. */
  if ( c_zoom > 0 )
  {
    return;
  }

  /* So, the only inputs are left/right/up/down around the levels. */
  if ( blit::buttons.pressed & blit::Button::DPAD_LEFT )
  {
    /* Left is mostly simple, long as you're not at the edge already. */
    if ( ( 1 != g_level ) && ( 6 != g_level ) && ( 11 != g_level ) &&
         ( 13 != g_level ) && ( 18 != g_level ) )
    {
      g_level--;
    }
  }

  if ( blit::buttons.pressed & blit::Button::DPAD_RIGHT )
  {
    /* Same as right. */
    if ( ( 5 != g_level ) && ( 10 != g_level ) && ( 12 != g_level ) &&
         ( 17 != g_level ) && ( 22 != g_level ) )
    {
      g_level++;
    }
  }

  /* Up is ... a little messier. */
  if ( blit::buttons.pressed & blit::Button::DPAD_UP )
  {
    /* Simple options first. */
    if ( ( 6 <= g_level ) && 
         ( ( 11 >= g_level ) || ( 17 <= g_level ) ) )
    {
      g_level -= 5;
    }

    /* And five edge cases. */
    else if ( ( 12 == g_level ) || ( 13 == g_level ) )
    {
      g_level -= 2;
    }
    else if ( ( 14 <= g_level ) && ( 16 >= g_level ) )
    {
      g_level -= 7;
    }
  }

  /* As is down. */
  if ( blit::buttons.pressed & blit::Button::DPAD_DOWN )
  {
    /* Simple options first. */
    if ( ( 17 >= g_level ) && 
         ( ( 6 >= g_level ) || ( 12 <= g_level ) ) )
    {
      g_level += 5;
    }

    /* And five edge cases. */
    else if ( ( 10 == g_level ) || ( 11 == g_level ) )
    {
      g_level += 2;
    }
    else if ( ( 7 <= g_level ) && ( 9 >= g_level ) )
    {
      g_level += 7;
    }
  }

  /* All done. */
  return;
}


/*
 * render - draws the current state of the game; largely handled by the tilemap.
 *          the zoom factor is used for the transitioning from game to game, and back.
 */

void Game::render( uint32_t p_time, uint8_t p_zoom )
{
  /* Save the zoom factor, making sure it's a sensible value. */
  c_zoom = ( p_zoom > 100 ) ? 100 : p_zoom;

  /* Set the alpha for all of this depending on the zoom. */
  uint8_t l_previous_alpha = blit::screen.alpha;
  blit::screen.alpha = 255 - ( c_zoom * 1.5 );

  /* Ask the base tilemap to draw itself, as a suitble zoom & alpha. */
  if ( nullptr != c_game_map )
  {
    c_game_map->draw( &blit::screen, blit::screen.clip, std::bind( &Game::map_transform, this, std::placeholders::_1 ) );
  }

  /* We only draw the more dynamic elements when we're full sized. */
  if ( 0 == c_zoom )
  {
    /* Drop in the player, we remember that location per level. */
    blit::screen.sprite( blit::Rect( 0, 4, 2, 2 ), c_player[g_level].location );
    /*__RETURN__*/
  }

  /* Reset the alpha to what it was before. */
  blit::screen.alpha = l_previous_alpha;

  /* All done. */
  return;
}


/* End of file Game.cpp */
