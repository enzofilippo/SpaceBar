/*
 * This file is part of the enzofilippo distribution (https://github.com/enzofilippo).
 * Copyright (c) 2021 Enzo Filippo Centenaro da Silva.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(void)
{

    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *imagem = NULL;

    al_init();
    al_init_image_addon();

    janela = al_create_display(640, 480);

    imagem = al_load_bitmap("sprites/SpriteLogo.bmp");

    al_draw_bitmap(imagem, 0, 0, 0);
    al_flip_display();

    al_rest(10.0);

    al_destroy_display(janela);

    return 0;
}
