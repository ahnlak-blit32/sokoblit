/*
 * Game.hpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Game class handles everything to do with the game itself, letting the
 * player scurry around pushing boxes and generally living the dream.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

#ifndef   _GAME_HPP_
#define   _GAME_HPP_

#include "32blit.hpp"
#include "sokoblit.hpp"

/* Constants based on tiled tiles - tinker at your peril! */

#define TILED_PLAYER_HOME 76

#define ANIMATION_FRAMES  3

typedef enum
{
  DIR_DOWN,
  DIR_LEFT,
  DIR_UP,
  DIR_RIGHT
} direction_t;

typedef struct
{
  blit::Point location;
  direction_t direction;
  uint8_t     animation;
} player_t;

class Game
{
  private:
    uint8_t         c_zoom;
    blit::Surface  *c_game_sprites;
    blit::TileMap  *c_game_map;
    uint8_t        *c_game_tiles;

    player_t        c_player[SOKOBLIT_LEVEL_MAX];

    blit::Rect      level_rect( uint8_t );
    blit::Point     level_tile_origin( uint8_t );

  public:
                    Game( void );
                   ~Game( void );
    blit::Mat3      map_transform( uint8_t );
    void            update( uint32_t );
    void            render( uint32_t, uint8_t );
};

#endif /* _GAME_HPP_ */

/* End of file Game.hpp */
