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
          c_player[l_level] = new Player( x, y );
          l_found = true;
          break;
        }
      }
      if ( l_found ) break;
    }
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

  /* Lastly all those lovely Player objects. */
  for( uint8_t l_level = 0; l_level < SOKOBLIT_LEVEL_MAX; l_level++ )
  {
    if ( nullptr != c_player[l_level] )
    {
      delete c_player[l_level];
      c_player[l_level] = nullptr;
    }
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
 * set_tile - updates the tile map with the new tile type. This takes into 
 *            account the fact that our logical tiles are in fact two game-tiles
 *            square!
 */

bool Game::set_tile( blit::Point p_location, uint8_t p_type )
{
  /* A very quick sanity check that we're on the actual map! */
  if ( ( p_location.x > ( c_game_map->bounds.w - 2 ) ) ||
       ( p_location.y > ( c_game_map->bounds.h - 2 ) ) )
  {
    return false;
  }

  /* Relatively simple here, we have four different tiles to set - but RESET */
  /* is a special case, we have to pick up the original floor tile.          */
  if ( TILED_RESET == p_type )
  {
    /* If it was originally a crate, use an empty floor. */
    if ( TILED_CRATE == at_game_map[c_game_map->offset( p_location )] )
    {
      c_game_map->tiles[c_game_map->offset( p_location )] = TILED_EMPTY;
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 0 ) )] = TILED_EMPTY+1;
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 0, 1 ) )] = TILED_EMPTY+16;
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 1 ) )] = TILED_EMPTY+17;
    }
    else
    {
      /* Just copy the original then. */
      c_game_map->tiles[c_game_map->offset( p_location )] = at_game_map[c_game_map->offset( p_location )];
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 0 ) )] = at_game_map[c_game_map->offset( p_location + blit::Point( 1, 0 ) )];
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 0, 1 ) )] = at_game_map[c_game_map->offset( p_location + blit::Point( 0, 1 ) )];
      c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 1 ) )] = at_game_map[c_game_map->offset( p_location + blit::Point( 1, 1 ) )];
    }
  }
  else
  {
    c_game_map->tiles[c_game_map->offset( p_location )] = p_type;
    c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 0 ) )] = p_type+1;
    c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 0, 1 ) )] = p_type+16;
    c_game_map->tiles[c_game_map->offset( p_location + blit::Point( 1, 1 ) )] = p_type+17;
  }

  /* All done. */
  return true;
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
  direction_t l_move = DIR_NONE;

  /* We only respond to user input when we're fully zoomed. */
  if ( c_zoom > 0 )
  {
    return;
  }

  /* Need to keep the player updating. */
  bool l_was_moving = c_player[g_level]->moving();
  bool l_was_pushing = c_player[g_level]->pushing();
  c_player[g_level]->update();

  /* We only pay attention to movement commands when the player isn't already */
  /* in motion - otherwise things will get ... confusing.                     */
  if ( c_player[g_level]->moving() )
  {
    return;
  }

  /* If we have just finished moving, and we were pushing a crate, we need */
  /* to park it where it was going...                                      */
  if ( l_was_moving && l_was_pushing )
  {
    blit::Point l_crate = level_tile_origin( g_level ) + c_player[g_level]->location();
    switch( c_player[g_level]->facing() )
    {
      case DIR_LEFT:
        l_crate -= blit::Point( 2, 0 );
        break;
      case DIR_RIGHT:
        l_crate += blit::Point( 2, 0 );
        break;
      case DIR_UP:
        l_crate -= blit::Point( 0, 2 );
        break;
      case DIR_DOWN:
        l_crate += blit::Point( 0, 2 );
        break;
    }
    set_tile( l_crate, TILED_CRATE );
  }

  /* So, find out what direction the player wants to go. */
  blit::Point l_location = level_tile_origin( g_level ) + c_player[g_level]->location();
  blit::Point l_target, l_crate_target;

  if ( ( blit::pressed( blit::Button::DPAD_LEFT ) ) || ( blit::joystick.x < -0.3f ) )
  {
    l_move = DIR_LEFT;
    l_target = l_location - blit::Point( 2, 0 );
    l_crate_target = l_location - blit::Point( 4, 0 );
  }
  if ( ( blit::pressed( blit::Button::DPAD_RIGHT ) ) || ( blit::joystick.x > 0.3f ) )
  {
    l_move = DIR_RIGHT;
    l_target = l_location + blit::Point( 2, 0 );
    l_crate_target = l_location + blit::Point( 4, 0 );
  }
  if ( ( blit::pressed( blit::Button::DPAD_UP ) ) || ( blit::joystick.y < -0.3f ) )
  {
    l_move = DIR_UP;
    l_target = l_location - blit::Point( 0, 2 );
    l_crate_target = l_location - blit::Point( 0, 4 );
  }
  if ( ( blit::pressed( blit::Button::DPAD_DOWN ) ) || ( blit::joystick.y > 0.3f ) )
  {
    l_move = DIR_DOWN;
    l_target = l_location + blit::Point( 0, 2 );
    l_crate_target = l_location + blit::Point( 0, 4 );
  }

  /* Ask the player to do that move, then. */
  if ( DIR_NONE != l_move )
  {
    /* Look at the destinations, work out the various flags. */
    bool l_blocked = false;
    bool l_pushing = false;

    /* Is our move target a wall? */
    uint8_t l_target_tile = c_game_map->tile_at( l_target );
    if ( TILED_WALL == l_target_tile )
    {
      l_blocked = true;
    }
    else if ( TILED_CRATE == l_target_tile )
    {
      /* If it's a crate we need to work out if *that* moves into free space. */
      uint8_t l_crate_tile = c_game_map->tile_at( l_crate_target );
      if ( ( TILED_WALL == l_crate_tile ) || ( TILED_CRATE == l_crate_tile ) )
      {
        /* It's like walking into a wall. */
        l_blocked = true;
      }
      else
      {
        /* Update the underlying tilemap to reflect that the crate has moved. */
        set_tile( l_target, TILED_RESET );

        /* Flag that we're also pushing a crate. */
        l_pushing = true;
      }
    }

    /* And finally, ask the player to move herself. */
    c_player[g_level]->move( l_move, l_blocked, l_pushing );
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
    /* Drop in the player for the current level. */
    c_player[g_level]->render();
  }

  /* Reset the alpha to what it was before. */
  blit::screen.alpha = l_previous_alpha;

  /* All done. */
  return;
}


/* End of file Game.cpp */
