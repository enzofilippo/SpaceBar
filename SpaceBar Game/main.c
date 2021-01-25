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
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>

#define FPS 10.0
#define LARGURA_TELA 640
#define ALTURA_TELA 360

double dinheiro = 10;
double custoBaseDormitorio = 10;
double custoBaseEstufa = 1;
double custoBaseQuimica = 1;
double custoBaseFisica = 1;
double custoBaseRobotica = 1;
double custoBaseNuclear = 1;
double prodBaseDormitorio = 0.2;
double prodBaseEstufa = 1;
double prodBaseQuimica = 1;
double prodBaseFisica = 1;
double prodBaseRobotica = 1;
double prodBaseNuclear = 1;
double upgradeMultiplier = 1; //novo jogo - nível 0
double taxaCrescimento = 1.07;

double dinheiroPorSegundo, ganho, custo, prod;
double custoDormitorio, custoEstufa, custoQuimica, custoFisica, custoRobotica, custoNuclear;
double qtdeDormitorio, qtdeEstufa, qtdeQuimica, qtdeFisica, qtdeRobotica, qtdeNuclear;
double prodDormitorio, prodEstufa, prodQuimica, prodFisica, prodRobotica, prodNuclear;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *filaEventos = NULL;
ALLEGRO_EVENT_QUEUE *filaEventosTimer = NULL;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_FONT *fonteMenor = NULL;
ALLEGRO_BITMAP *background = NULL;
ALLEGRO_BITMAP *fecharBotao = NULL;
ALLEGRO_BITMAP *fecharBotao2 = NULL;
ALLEGRO_BITMAP *botaoLabs = NULL;
ALLEGRO_BITMAP *botaoLabs2 = NULL;
ALLEGRO_BITMAP *backgroundDinheiro = NULL;
ALLEGRO_BITMAP *backgroundUpgrades = NULL;
ALLEGRO_TIMER *timer = NULL;

int noBotaoFecharAnterior;
int noBotaoLabsAnterior;

void error_msg(char *text){
	al_show_native_message_box(NULL,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}


int inicializar(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

    al_init_font_addon();

    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return 0;
    }

    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar add-on allegro_image");
        return 0;
    }

    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }

    if (!al_install_mouse()){
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    al_set_window_title(janela, "Ultimate Restless Space Station - Feito por Enzo Filippo Centenaro e Vitor Mateus Romancini");

    background = al_load_bitmap("resources/BackgroundFinal.png");
    fecharBotao = al_load_bitmap("resources/SpriteFechar.png");
    fecharBotao2 = al_load_bitmap("resources/SpriteFechar2.png");
    botaoLabs = al_load_bitmap("resources/SpriteBotaoLabs.png");
    botaoLabs2 = al_load_bitmap("resources/SpriteBotaoLabs2.png");
    backgroundDinheiro = al_load_bitmap("resources/SpriteBackgroundDinheiro.png");
    backgroundUpgrades = al_load_bitmap("resources/SpriteBackgroundUpgrades.png");

    al_draw_bitmap(background, 0, 0, 0);
    al_flip_display();

    fonte = al_load_font("resources/gamer.ttf", 32, 0);
    if (!fonte){
        error_msg("Falha ao carregar \"resources/gamer.ttf\"");
        al_destroy_display(janela);
        return 0;
    }
    fonteMenor = al_load_font("resources/gamer.ttf", 16, 0);
    if (!fonte){
        error_msg("Falha ao carregar \"resources/gamer.ttf\"");
        al_destroy_display(janela);
        return 0;
    }

    filaEventos = al_create_event_queue();
    if (!filaEventos){
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }
    filaEventosTimer = al_create_event_queue();
    if (!filaEventos){
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    //registra fontes de eventos na fila. o da janela, do mouse, e do teclado
    al_register_event_source(filaEventos, al_get_keyboard_event_source());
    al_register_event_source(filaEventos, al_get_display_event_source(janela));
    al_register_event_source(filaEventos, al_get_mouse_event_source());
    al_register_event_source(filaEventosTimer, al_get_timer_event_source(timer));

    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 277, ALLEGRO_ALIGN_LEFT, "dormitorio: %.0f", qtdeDormitorio);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoBaseDormitorio);

    al_start_timer(timer);

    return 1;
}

double custoCalculo(double custoBase, int qtde){
    custo = custoBase*pow(taxaCrescimento,qtde);
    return custo;
}

double prodCalculo(double prodBase, int qtde, double multiplier){
    prod = (prodBase*qtde)*multiplier;
    return prod;
}

int main(void){

    int sair = 0;

    if (!inicializar()){
        return -1;
    }

    while(!sair){

        while(!al_is_event_queue_empty(filaEventos)){ //eventos de mouse
            ALLEGRO_EVENT evento;
            al_wait_for_event(filaEventos, &evento);

            if (evento.mouse.x >= 6 &&  //botão para fechar o jogo
                evento.mouse.x <= 21 &&
                evento.mouse.y <= 21 &&
                evento.mouse.y >= 6) {

                al_draw_bitmap(fecharBotao, 6, 6, 0);
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(fecharBotao2, 6, 6, 0);
                }
            }

            if (evento.mouse.x >= 2 &&  //botão para comprar dormitório
                evento.mouse.x <= 21 &&
                evento.mouse.y <= 299 &&
                evento.mouse.y >= 279) {

                al_draw_bitmap(botaoLabs, 2, 279, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                custoDormitorio = custoCalculo(custoBaseDormitorio, qtdeDormitorio);
                    if(custoDormitorio<=dinheiro){
                        qtdeDormitorio += 1;
                        dinheiro -= custoDormitorio;
                        prodDormitorio = prodCalculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
                        al_draw_bitmap(backgroundUpgrades, 53, 282, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 277, ALLEGRO_ALIGN_LEFT, "dormitorio: %.0f", qtdeDormitorio);
                        custoDormitorio = custoCalculo(custoBaseDormitorio, qtdeDormitorio);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoDormitorio);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 279, 0);
                }
            }

            if (evento.mouse.x >= 2 &&  //botão para comprar estufa
                evento.mouse.x <= 21 &&
                evento.mouse.y <= 327 &&
                evento.mouse.y >= 307) {

                al_draw_bitmap(botaoLabs, 2, 307, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 307, 0);
                }
            }

            if (evento.mouse.x >= 2 &&  //botão para comprar química
                evento.mouse.x <= 21 &&
                evento.mouse.y <= 355 &&
                evento.mouse.y >= 335) {

                al_draw_bitmap(botaoLabs, 2, 335, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 335, 0);
                }
            }

            if (evento.mouse.x >= 165 &&  //botão para comprar física
                evento.mouse.x <= 184 &&
                evento.mouse.y <= 299 &&
                evento.mouse.y >= 279) {

                al_draw_bitmap(botaoLabs, 165, 279, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 279, 0);
                }
            }

            if (evento.mouse.x >= 165 &&  //botão para comprar robótica
                evento.mouse.x <= 184 &&
                evento.mouse.y <= 327 &&
                evento.mouse.y >= 307) {

                al_draw_bitmap(botaoLabs, 165, 307, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 307, 0);
                }
            }

            if (evento.mouse.x >= 165 &&  //botão para comprar nuclear
                evento.mouse.x <= 184 &&
                evento.mouse.y <= 355 &&
                evento.mouse.y >= 335) {

                al_draw_bitmap(botaoLabs, 165, 335, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 335, 0);
                }
            }

        }
        while(!al_is_event_queue_empty(filaEventosTimer)){ //atualizar dinheiro na tela
            ALLEGRO_EVENT evento;
            al_wait_for_event(filaEventosTimer, &evento);

            ganho = prodDormitorio + prodEstufa + prodQuimica + prodFisica + prodRobotica + prodNuclear;
            dinheiroPorSegundo = ganho/FPS;
            dinheiro += dinheiroPorSegundo;

            al_draw_bitmap(backgroundDinheiro, 33, 6, 0);
            al_draw_textf(fonte, al_map_rgb(153, 229, 80), LARGURA_TELA - 10, 0, ALLEGRO_ALIGN_RIGHT, "%.2f", dinheiro);
        }
        al_flip_display();
    }

    al_destroy_timer(timer);
    al_destroy_font(fonte);
    al_destroy_display(janela);
    al_destroy_event_queue(filaEventos);

    return 0;
}
