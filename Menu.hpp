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
    blit::TileMap  *c_menu_map;
    uint8_t        *c_menu_tiles;

  public:
                    Menu( void );
                   ~Menu( void );
    void            update( uint32_t, uint8_t );
    void            render( uint32_t );
};

#endif /* _MENU_HPP_ */

/* End of file Menu.hpp */
