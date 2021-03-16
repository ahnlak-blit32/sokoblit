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
#include "Player.hpp"

/* Constants based on tiled tiles - tinker at your peril! */

#define TILED_PLAYER_HOME 76

#define ANIMATION_FRAMES  3

class Game
{
  private:
    uint8_t         c_zoom;
    blit::Surface  *c_game_sprites;
    blit::TileMap  *c_game_map;
    uint8_t        *c_game_tiles;

    Player         *c_player[SOKOBLIT_LEVEL_MAX+1];

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
