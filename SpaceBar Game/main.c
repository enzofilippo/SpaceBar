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

// Inclui o cabeçalho da bibilioteca Allegro 5
#include <allegro5/allegro.h>

// Inclui o cabeçalho do add-on para uso de imagens
#include <allegro5/allegro_image.h>

int main(void)
{
    // Variável representando a janela principal
    ALLEGRO_DISPLAY *janela = NULL;

    // Variável representando a imagem
    ALLEGRO_BITMAP *imagem = NULL;

    // Inicializa a Allegro
    al_init();

    // Inicializa o add-on para utilização de imagens
    al_init_image_addon();

    // Configura a janela
    janela = al_create_display(640, 480);

    // Carrega a imagem
    imagem = al_load_bitmap("sprites/SpriteLogo.bmp");

    // Desenha a imagem na tela
    al_draw_bitmap(imagem, 0, 0, 0);

    // Atualiza a tela
    al_flip_display();

    // Segura a execução por 10 segundos
    al_rest(10.0);

    // Finaliza a janela
    al_destroy_display(janela);

    return 0;
}
