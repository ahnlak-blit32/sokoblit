/*
 * Menu.hpp - part of SokoBlit
 * 
 * Copyright (c) 2021 Pete Favelle / fsqaured limited <32blit@fsquared.co.uk>
 *
 * The Menu class handles everything to do with the game menu, which allows
 * the player to choose which levels to play.
 *
 * This software is distributed under the MIT License. See LICENSE for details.
 */

#ifndef   _MENU_HPP_
#define   _MENU_HPP_

#include "32blit.hpp"

class Menu
{
  private:
    uint8_t         c_zoom;
    blit::Surface  *c_menu_sprites;
    blit::Surface  *c_menu_splash;
    blit::TileMap  *c_menu_map;
    uint8_t        *c_menu_tiles;

    blit::Rect      level_rect( uint8_t );

  public:
                    Menu( void );
                   ~Menu( void );
    blit::Mat3      map_transform( uint8_t ); 
    void            update( uint32_t );
    void            render( uint32_t, uint8_t );
};

#endif /* _MENU_HPP_ */

/* End of file Menu.hpp */
