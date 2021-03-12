/*
 * This file is part of the enzofilippo distribution (https://github.com/enzofilippo).
 * Copyright (c) 2021 Enzo Filippo Centenaro da Silva and Vitor Mateus Romancini do Amaral.
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
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FPS 11.0
#define LARGURA_TELA 640
#define ALTURA_TELA 360

int sair = 0;
int fecharMenu = 0;

double dinheiro = 0;
double custoBaseDormitorio = 10;
double prodBaseDormitorio = 0.4;
double taxaCrescimentoDormitorio = 1.07;
double custoBaseEstufa = 420;
double prodBaseEstufa = 10;
double taxaCrescimentoEstufa = 1.15;
double custoBaseQuimica = 1926;
double prodBaseQuimica = 180;
double taxaCrescimentoQuimica = 1.14;
double custoBaseFisica = 314159;
double prodBaseFisica = 3600;
double taxaCrescimentoFisica = 1.13;
double custoBaseRobotica = 1000101;
double prodBaseRobotica = 21600;
double taxaCrescimentoRobotica = 1.12;
double custoBaseNuclear = 713000000;
double prodBaseNuclear = 10000;
double taxaCrescimentoNuclear = 1.10;

double custoUpgradeVodka = 250000;

double upgradeMultiplier = 1; //novo jogo - n�vel 0

float red_x;
float red_y;

double incrementoPorFrame, dinheiroPorSegundo, custo, prod;
double custoDormitorio, custoEstufa, custoQuimica, custoFisica, custoRobotica, custoNuclear;
double qtdeDormitorio, qtdeEstufa, qtdeQuimica, qtdeFisica, qtdeRobotica, qtdeNuclear;
double prodDormitorio, prodEstufa, prodQuimica, prodFisica, prodRobotica, prodNuclear;
int upgradeVodka;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *filaEventos = NULL;
ALLEGRO_EVENT_QUEUE *filaEventosTimer = NULL;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_FONT *fonteMenor = NULL;
ALLEGRO_BITMAP *backgroundInterface = NULL;
ALLEGRO_BITMAP *backgroundMenu = NULL;
ALLEGRO_BITMAP *botaoFechar = NULL;
ALLEGRO_BITMAP *botaoFechar2 = NULL;
ALLEGRO_BITMAP *botaoLabs = NULL;
ALLEGRO_BITMAP *botaoLabs2 = NULL;
ALLEGRO_BITMAP *backgroundDinheiro = NULL;
ALLEGRO_BITMAP *backgroundLabs = NULL;
ALLEGRO_BITMAP *backgroundDinheiroPorSegundo = NULL;
ALLEGRO_BITMAP *backgroundMonitor = NULL;
ALLEGRO_BITMAP *botaoUpgradeVodka = NULL;
ALLEGRO_BITMAP *botaoUpgradeVodka2 = NULL;
ALLEGRO_BITMAP *botaoSavegame = NULL;
ALLEGRO_BITMAP *botaoSavegame2 = NULL;
ALLEGRO_BITMAP *botaoNovoJogo = NULL;
ALLEGRO_BITMAP *botaoNovoJogo2 = NULL;
ALLEGRO_BITMAP *labInicial = NULL;
ALLEGRO_BITMAP *labDormitorio = NULL;
ALLEGRO_BITMAP *labEstufa = NULL;
ALLEGRO_BITMAP *labQuimica = NULL;
ALLEGRO_BITMAP *labFisica = NULL;
ALLEGRO_BITMAP *labRobotica = NULL;
ALLEGRO_BITMAP *labNuclear = NULL;
ALLEGRO_BITMAP *satelite = NULL;
ALLEGRO_BITMAP *satelite2 = NULL;
ALLEGRO_BITMAP *led = NULL;
ALLEGRO_BITMAP *botaoMenuNovojogo = NULL;
ALLEGRO_BITMAP *botaoMenuNovojogo2 = NULL;
ALLEGRO_BITMAP *botaoMenuContinuar = NULL;
ALLEGRO_BITMAP *botaoMenuContinuar2 = NULL;
ALLEGRO_BITMAP *botaoMenuTutorial = NULL;
ALLEGRO_BITMAP *botaoMenuTutorial2 = NULL;
ALLEGRO_BITMAP *botaoMenuSair = NULL;
ALLEGRO_BITMAP *botaoMenuSair2 = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_SAMPLE *som_botao = NULL;
ALLEGRO_SAMPLE *som_semDinheiro = NULL;
ALLEGRO_SAMPLE *som_satelite = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;

FILE *save;

int noBotaoFecharAnterior;
int noBotaoLabsAnterior;
int noBotaoSateliteAnterior;
int noBotaoSavegameAnterior;
int noBotaoNovoJogoAnterior;

void error_msg(char *text){
	al_show_native_message_box(NULL,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

double custo_calculo(double custoBase, double qtde, double taxaCrescimento){
    custo = custoBase*pow(taxaCrescimento,qtde);
    return custo;
}

double prod_calculo(double prodBase, double qtde, double multiplier){
    prod = (prodBase*qtde)*multiplier;
    return prod;
}

int saveGame(double d,double uM,double qD,double qE,double qQ,double qF,double qR,double qN,int uV){

    save = fopen("save.txt","w");
    fprintf(save,"%.2f\n",d);
    fprintf(save,"%.2f\n",uM);
    fprintf(save,"%.2f\n",qD);
    fprintf(save,"%.2f\n",qE);
    fprintf(save,"%.2f\n",qQ);
    fprintf(save,"%.2f\n",qF);
    fprintf(save,"%.2f\n",qR);
    fprintf(save,"%.2f\n",qN);
    fprintf(save,"%d\n",uV);
    fclose(save);

    return 0;

}

int interface_game(){

    double dadosSave[9], dado;
    save = fopen("save.txt","r");
    for(int i=0;i<9;i++){
        fscanf(save,"%lf",&dado);
        dadosSave[i] = dado;
    }
    fclose(save);

    dinheiro = dadosSave[0];
    upgradeMultiplier = dadosSave[1];
    qtdeDormitorio = dadosSave[2];
    qtdeEstufa = dadosSave[3];
    qtdeQuimica= dadosSave[4];
    qtdeFisica= dadosSave[5];
    qtdeRobotica= dadosSave[6];
    qtdeNuclear= dadosSave[7];
    upgradeVodka= dadosSave[8];
    prodDormitorio = prod_calculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
    prodEstufa = prod_calculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
    prodQuimica = prod_calculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
    prodFisica = prod_calculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
    prodRobotica = prod_calculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
    prodNuclear = prod_calculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
    custoDormitorio = custo_calculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
    custoEstufa = custo_calculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
    custoQuimica = custo_calculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
    custoFisica = custo_calculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
    custoRobotica = custo_calculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
    custoNuclear = custo_calculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);

    al_draw_bitmap(backgroundInterface, 0, 0, 0);
    al_draw_bitmap(labInicial, 259, 32, 0);

    if(upgradeVodka){
        al_draw_bitmap(led, 555, 50, 0);
    }

    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 277, ALLEGRO_ALIGN_LEFT, "DORMITORIO: %.0f", qtdeDormitorio);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoDormitorio);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 305, ALLEGRO_ALIGN_LEFT, "ESTUFA: %.0f", qtdeEstufa);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 315, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoEstufa);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 333, ALLEGRO_ALIGN_LEFT, "QUIMICA: %.0f", qtdeQuimica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 343, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoQuimica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 277, ALLEGRO_ALIGN_LEFT, "FISICA: %.0f", qtdeFisica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoFisica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 305, ALLEGRO_ALIGN_LEFT, "ROBOTICA: %.0f", qtdeRobotica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 315, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoRobotica);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 333, ALLEGRO_ALIGN_LEFT, "NUCLEAR: %.0f", qtdeNuclear);
    al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 343, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoNuclear);

    if(qtdeDormitorio>0){
        al_draw_bitmap(labDormitorio, 280, 36, 0);
    }
    if(qtdeEstufa>0){
        al_draw_bitmap(labEstufa, 6, 139, 0);
    }
    if(qtdeQuimica>0){
        al_draw_bitmap(labQuimica, 119, 41, 0);
    }
    if(qtdeFisica>0){
        al_draw_bitmap(labFisica, 8, 41, 0);
    }
    if(qtdeRobotica>0){
        al_draw_bitmap(labRobotica, 204, 148, 0);
    }
    if(qtdeNuclear>0){
        al_draw_bitmap(labNuclear, 349, 131, 0);
    }

    al_flip_display();

    return 1;
}

int interface_menu(){
    al_draw_bitmap(backgroundMenu, 0, 0, 0);

    while(!fecharMenu){

        al_flip_display();

        while(!al_is_event_queue_empty(filaEventos)){ //eventos de mouse
            ALLEGRO_EVENT evento;
            al_wait_for_event(filaEventos, &evento);

            if (evento.mouse.x >= red_x*241 &&  //bot�o menu novo jogo
                evento.mouse.x <= red_x*399 &&
                evento.mouse.y <= red_y*158 &&
                evento.mouse.y >= red_y*141) {

                al_draw_bitmap(botaoMenuNovojogo, 241, 141, 0);
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    noBotaoFecharAnterior = 1;
                    saveGame(0,1,0,0,0,0,0,0,0);
                    fecharMenu=1;
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(botaoMenuNovojogo2, 241, 141, 0);
                }
            }

            if (evento.mouse.x >= red_x*238 &&  //bot�o menu continuar
                evento.mouse.x <= red_x*402 &&
                evento.mouse.y <= red_y*188 &&
                evento.mouse.y >= red_y*171) {

                al_draw_bitmap(botaoMenuContinuar, 238, 171, 0);
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    noBotaoFecharAnterior = 1;
                    fecharMenu=1;
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(botaoMenuContinuar2, 238, 171, 0);
                }
            }

            if (evento.mouse.x >= red_x*248 &&  //bot�o menu tutorial
                evento.mouse.x <= red_x*391 &&
                evento.mouse.y <= red_y*218 &&
                evento.mouse.y >= red_y*201) {

                al_draw_bitmap(botaoMenuTutorial, 248, 201, 0);
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(botaoMenuTutorial2, 248, 201, 0);
                }
            }

            if (evento.mouse.x >= red_x*287 &&  //bot�o para sair
                evento.mouse.x <= red_x*352 &&
                evento.mouse.y <= red_y*248 &&
                evento.mouse.y >= red_y*231) {

                al_draw_bitmap(botaoMenuSair, 287, 231, 0);
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                fecharMenu = 1;
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(botaoMenuSair2, 287, 231, 0);
                }
            }

        }
    }
    return 1;
}

int inicializar(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    if(!al_install_audio()){
        error_msg("Falha ao inicializar o audio");
        return 0;
    }

    if(!al_init_acodec_addon()){
        error_msg("Falha ao inicializar o codec de audio");
        return 0;
    }

    if (!al_reserve_samples(5)){
        error_msg("Falha ao reservar amostrar de audio");
        return 0;
    }

    som_botao = al_load_sample( "assets/buttonSound.ogg" );
    if (!som_botao){
        error_msg( "Audio nao carregado" );
        return 0;
    }

    som_semDinheiro = al_load_sample( "assets/semDinheiro.ogg" );
    if (!som_botao){
        error_msg( "Audio nao carregado" );
        al_destroy_sample(som_botao);
        return 0;
    }

    som_satelite = al_load_sample( "assets/satelite.ogg" );
    if (!som_satelite){
        error_msg( "Audio nao carregado" );
        al_destroy_sample(som_botao);
        al_destroy_sample(som_semDinheiro);
        return 0;
    }

    musica = al_load_audio_stream("assets/soundtrack.ogg", 4, 1024);
    if (!musica){
        al_destroy_sample(som_botao);
        al_destroy_sample(som_semDinheiro);
        al_destroy_sample(som_satelite);
        error_msg( "Audio nao carregado" );
        return 0;
    }

    al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
    al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);

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

    ALLEGRO_MONITOR_INFO info;
	int res_x_comp, res_y_comp;
	al_get_monitor_info(0, &info);
	res_x_comp = info.x2 - info.x1;
	res_y_comp = info.y2 - info.y1;
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	janela = al_create_display(res_x_comp, res_y_comp);

    //janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }

    red_x = res_x_comp / (float) LARGURA_TELA;
	red_y = res_y_comp / (float) ALTURA_TELA;
	ALLEGRO_TRANSFORM transformar;
	al_identity_transform(&transformar);
	al_scale_transform(&transformar, red_x, red_y);
	al_use_transform(&transformar);

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

    fonte = al_load_font("assets/gamer.ttf", 32, 0);
    if (!fonte){
        error_msg("Falha ao carregar \"assets/gamer.ttf\"");
        al_destroy_display(janela);
        return 0;
    }
    fonteMenor = al_load_font("assets/gamer.ttf", 16, 0);
    if (!fonte){
        error_msg("Falha ao carregar \"assets/gamer.ttf\"");
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

    backgroundInterface = al_load_bitmap("assets/background_interface.png");
    backgroundMenu = al_load_bitmap("assets/background_menu.png");
    botaoFechar = al_load_bitmap("assets/sprite_botao_fechar.png");
    botaoFechar2 = al_load_bitmap("assets/sprite_botao_fechar_2.png");
    botaoLabs = al_load_bitmap("assets/sprite_botao_labs.png");
    botaoLabs2 = al_load_bitmap("assets/sprite_botao_labs_2.png");
    backgroundDinheiro = al_load_bitmap("assets/background_dinheiro.png");
    backgroundLabs = al_load_bitmap("assets/background_lab.png");
    backgroundDinheiroPorSegundo = al_load_bitmap("assets/background_dinheiroporsegundo.png");
    backgroundMonitor = al_load_bitmap("assets/background_monitor.png");
    botaoUpgradeVodka = al_load_bitmap("assets/sprite_upgrade_vodka.png");
    botaoUpgradeVodka2 = al_load_bitmap("assets/sprite_upgrade_vodka_2.png");
    botaoSavegame = al_load_bitmap("assets/sprite_botao_savegame.png");
    botaoSavegame2 = al_load_bitmap("assets/sprite_botao_savegame_2.png");
    botaoNovoJogo = al_load_bitmap("assets/sprite_botao_novojogo.png");
    botaoNovoJogo2 = al_load_bitmap("assets/sprite_botao_novojogo_2.png");
    led = al_load_bitmap("assets/sprite_luz_ligada.png");
    labInicial = al_load_bitmap("assets/lab_inicial.png");
    labDormitorio = al_load_bitmap("assets/lab_dormitorio.png");
    labEstufa = al_load_bitmap("assets/lab_estufa.png");
    labQuimica = al_load_bitmap("assets/lab_quimica.png");
    labFisica = al_load_bitmap("assets/lab_fisica.png");
    labRobotica = al_load_bitmap("assets/lab_robotica.png");
    labNuclear = al_load_bitmap("assets/lab_nuclear.png");
    satelite = al_load_bitmap("assets/sprite_satelite.png");
    satelite2 = al_load_bitmap("assets/sprite_satelite_2.png");
    botaoMenuNovojogo = al_load_bitmap("assets/sprite_botao_menu_novojogo.png");
    botaoMenuNovojogo2 = al_load_bitmap("assets/sprite_botao_menu_novojogo_2.png");
    botaoMenuContinuar = al_load_bitmap("assets/sprite_botao_menu_continuar.png");
    botaoMenuContinuar2 = al_load_bitmap("assets/sprite_botao_menu_continuar_2.png");
    botaoMenuTutorial = al_load_bitmap("assets/sprite_botao_menu_tutorial.png");
    botaoMenuTutorial2 = al_load_bitmap("assets/sprite_botao_menu_tutorial_2.png");
    botaoMenuSair = al_load_bitmap("assets/sprite_botao_menu_sair.png");
    botaoMenuSair2 = al_load_bitmap("assets/sprite_botao_menu_sair_2.png");

    al_register_event_source(filaEventos, al_get_keyboard_event_source());
    al_register_event_source(filaEventos, al_get_display_event_source(janela));
    al_register_event_source(filaEventos, al_get_mouse_event_source());
    al_register_event_source(filaEventosTimer, al_get_timer_event_source(timer));

    al_start_timer(timer);

    return 1;
}

int main(void){

    if(!inicializar()){
        return -1;
    }

    if(!interface_menu()){
        return -1;
    }

    if(!interface_game()){
        return -1;
    }

    while(!sair){

        while(!al_is_event_queue_empty(filaEventos)){ //eventos de mouse
            ALLEGRO_EVENT evento;
            al_wait_for_event(filaEventos, &evento);

            if (evento.mouse.x >= red_x*6 &&  //bot�o para fechar o jogo
                evento.mouse.x <= red_x*21 &&
                evento.mouse.y <= red_y*21 &&
                evento.mouse.y >= red_y*6) {

                al_draw_bitmap(botaoFechar, 6, 6, 0);
                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Sério que você vai");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "fechar o jogo? AFF");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "Vou chamar o Putin");
                noBotaoFecharAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                sair = 1;
                }
            }else{
                if(noBotaoFecharAnterior){
                    al_draw_bitmap(botaoFechar2, 6, 6, 0);
                    al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                    al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Não sei o que seria");
                    al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "da estação sem você ");
                    al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "camarada! <3");
                }
            }

            if (evento.mouse.x >= red_x*622 &&  //bot�o para clicar no savegame
                evento.mouse.x <= red_x*635 &&
                evento.mouse.y <= red_y*268 &&
                evento.mouse.y >= red_y*255) {

                noBotaoSavegameAnterior = 1;
                al_draw_bitmap(botaoSavegame, 622, 255, 0);

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    saveGame(dinheiro,upgradeMultiplier,qtdeDormitorio,qtdeEstufa,qtdeQuimica,qtdeFisica,qtdeRobotica,qtdeNuclear,upgradeVodka);
                }
            }else{
                if(noBotaoSavegameAnterior){
                    al_draw_bitmap(botaoSavegame2, 622, 255, 0);
                }
            }

            if (evento.mouse.x >= red_x*608 &&  //bot�o para clicar no novojogo
                evento.mouse.x <= red_x*631 &&
                evento.mouse.y <= red_y*314 &&
                evento.mouse.y >= red_y*291) {

                noBotaoNovoJogoAnterior = 1;
                al_draw_bitmap(botaoNovoJogo , 608, 280, 0);

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    int iniciarNovoJogo = al_show_native_message_box(
                        janela,
                        "URSS - Ultimate Restless Space Station",
                        "Previet Camarada",
                        "Tem certeza que quer nos abandonar? "
                        "Ficaremos a mercê do esquecimento eterno, "
                        "ou pior, dos capitalistas! "
                        "Escolha com sabedoria. Tudo que foi salvo será apagado.",
                        NULL,
                        ALLEGRO_MESSAGEBOX_YES_NO
                        );
                    if (iniciarNovoJogo){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        saveGame(0,1,0,0,0,0,0,0,0);
                        al_destroy_sample(som_botao);
                        al_destroy_audio_stream(musica);
                        al_destroy_timer(timer);
                        al_destroy_font(fonte);
                        al_destroy_display(janela);
                        al_destroy_event_queue(filaEventos);
                        al_destroy_event_queue(filaEventosTimer);
                        inicializar();
                        interface_game();
                    }

                }
            }else{
                if(noBotaoNovoJogoAnterior){
                    al_draw_bitmap(botaoNovoJogo2, 608, 280, 0);
                }
            }

            if (evento.mouse.x >= red_x*263 &&  //bot�o para clicar no satélite
                evento.mouse.x <= red_x*281 &&
                evento.mouse.y <= red_y*146 &&
                evento.mouse.y >= red_y*128) {

                noBotaoSateliteAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    al_play_sample(som_satelite, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    al_draw_bitmap(satelite, 263, 128, 0);
                    dinheiro += upgradeMultiplier;
                    al_draw_bitmap(backgroundDinheiro, 263, 6, 0);
                    al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA - 38, 0, ALLEGRO_ALIGN_RIGHT, "%.2f", dinheiro);

                }
            }else{
                if(noBotaoSateliteAnterior){
                    al_draw_bitmap(satelite2, 263, 128, 0);
                }
            }

            if (evento.mouse.x >= red_x*2 &&  //bot�o para comprar dormit�rio
                evento.mouse.x <= red_x*21 &&
                evento.mouse.y <= red_y*299 &&
                evento.mouse.y >= red_y*279) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "O que será que esse");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "botão faz?");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "");
                al_draw_bitmap(botaoLabs, 2, 279, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoDormitorio = custo_calculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
                    if(custoDormitorio<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeDormitorio += 1;
                        if(qtdeDormitorio==1){
                            al_draw_bitmap(labDormitorio, 280, 36, 0);
                        }
                        dinheiro -= custoDormitorio;
                        prodDormitorio = prod_calculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 279, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 277, ALLEGRO_ALIGN_LEFT, "DORMITORIO: %.0f", qtdeDormitorio);
                        custoDormitorio = custo_calculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoDormitorio);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Eu nem sabia que tinha");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "camas no espaço...");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "Mas foi você que pagou");
                    }else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }

                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 279, 0);
                }
            }

            if (evento.mouse.x >= red_x*2 &&  //bot�o para comprar estufa
                evento.mouse.x <= red_x*21 &&
                evento.mouse.y <= red_y*327 &&
                evento.mouse.y >= red_y*307) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Estufa em...");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "Quem disse que não dá");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "pra plantar no espaço?");
                al_draw_bitmap(botaoLabs, 2, 307, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoEstufa = custo_calculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
                    if(custoEstufa<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeEstufa += 1;
                        if(qtdeEstufa==1){
                            al_draw_bitmap(labEstufa, 6, 139, 0);
                        }
                        dinheiro -= custoEstufa;
                        prodEstufa = prod_calculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 307, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 305, ALLEGRO_ALIGN_LEFT, "ESTUFA: %.0f", qtdeEstufa);
                        custoEstufa = custo_calculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 315, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoEstufa);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Pra plantar soja não");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "precisa TCC!!!");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "¯\\_(^_^)_/¯ ");
                    }
                    else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 307, 0);
                }
            }

            if (evento.mouse.x >= red_x*2 &&  //bot�o para comprar qu�mica
                evento.mouse.x <= red_x*21 &&
                evento.mouse.y <= red_y*355 &&
                evento.mouse.y >= red_y*335) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "O que o álcool disse");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "para o outro álcool?");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "Etanóis!!! KKKKKKKKKKKKK");
                al_draw_bitmap(botaoLabs, 2, 335, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoQuimica = custo_calculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
                    if(custoQuimica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeQuimica += 1;
                        if(qtdeQuimica==1){
                            al_draw_bitmap(labQuimica, 119, 41, 0);
                        }
                        dinheiro -= custoQuimica;
                        prodQuimica = prod_calculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 335, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 333, ALLEGRO_ALIGN_LEFT, "QUIMICA: %.0f", qtdeQuimica);
                        custoQuimica = custo_calculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 343, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoQuimica);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Agora sim! Da pra");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "fazer muita vodka");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "para os cientistas");
                    }else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 2, 335, 0);
                }
            }

            if (evento.mouse.x >= red_x*165 &&  //bot�o para comprar f�sica
                evento.mouse.x <= red_x*184 &&
                evento.mouse.y <= red_y*299 &&
                evento.mouse.y >= red_y*279) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "O que podemos fazer");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "com um lab de física?");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "BOMBAS ESPACIAIS!!!");
                al_draw_bitmap(botaoLabs, 165, 279, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoFisica = custo_calculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
                    if(custoFisica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeFisica += 1;
                        if(qtdeFisica==1){
                            al_draw_bitmap(labFisica, 8, 41, 0);
                        }
                        dinheiro -= custoFisica;
                        prodFisica = prod_calculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 279, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 277, ALLEGRO_ALIGN_LEFT, "FISICA: %.0f", qtdeFisica);
                        custoFisica = custo_calculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 287, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoFisica);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "boa meu camarada");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "mais espaço...Gagarin!!");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "pode trazer o TANQUE!!");
                    }
                    else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 279, 0);
                }
            }

            if (evento.mouse.x >= red_x*165 &&  //bot�o para comprar rob�tica
                evento.mouse.x <= red_x*184 &&
                evento.mouse.y <= red_y*327 &&
                evento.mouse.y >= red_y*307) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "o bom dos robôs é");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "que eles fazem tudo");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "vamos é dormir");
                al_draw_bitmap(botaoLabs, 165, 307, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoRobotica = custo_calculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
                    if(custoRobotica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeRobotica += 1;
                        if(qtdeRobotica==1){
                            al_draw_bitmap(labRobotica, 204, 148, 0);
                        }
                        dinheiro -= custoRobotica;
                        prodRobotica = prod_calculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 307, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 305, ALLEGRO_ALIGN_LEFT, "ROBOTICA: %.0f", qtdeRobotica);
                        custoRobotica = custo_calculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 315, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoRobotica);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Ya! mais robôs!!!");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "estranho, pq aquele robô");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "está dizendo matar hum...");
                    }else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 307, 0);
                }
            }

            if (evento.mouse.x >= red_x*165 &&  //bot�o para comprar nuclear
                evento.mouse.x <= red_x*184 &&
                evento.mouse.y <= red_y*355 &&
                evento.mouse.y >= red_y*335) {

                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "com mais espaço, po-");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "demos colocar um reator");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "o que pode dar errado?");
                al_draw_bitmap(botaoLabs, 165, 335, 0);
                noBotaoLabsAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    custoNuclear = custo_calculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);
                    if(custoNuclear<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeNuclear += 1;
                        if(qtdeNuclear==1){
                            al_draw_bitmap(labNuclear, 349, 131, 0);
                        }
                        dinheiro -= custoNuclear;
                        prodNuclear = prod_calculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 335, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 333, ALLEGRO_ALIGN_LEFT, "NUCLEAR: %.0f", qtdeNuclear);
                        custoNuclear = custo_calculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 343, ALLEGRO_ALIGN_LEFT, "custo: %.2f", custoNuclear);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Kalashnikov, faça");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "suas AK-47 atirarem");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "projéteis radioativos!!!");
                    }else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoLabs2, 165, 335, 0);
                }
            }

            if (evento.mouse.x >= red_x*570 &&  //botão upgrade Vodka
                evento.mouse.x <= red_x*637 &&
                evento.mouse.y <= red_y*75 &&
                evento.mouse.y >= red_y*50) {

                al_draw_bitmap(botaoUpgradeVodka, 569, 49, 0);
                noBotaoLabsAnterior = 1;
                al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "Compre o upgrade");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "para passar de nível");
                al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "");

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    if(!upgradeVodka){
                        if(custoUpgradeVodka<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        dinheiro -= custoUpgradeVodka;
                        upgradeMultiplier = 2;
                        prodDormitorio = prod_calculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
                        prodEstufa = prod_calculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
                        prodQuimica = prod_calculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
                        prodFisica = prod_calculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
                        prodRobotica = prod_calculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
                        prodNuclear = prod_calculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
                        al_draw_bitmap(backgroundMonitor, 390, 274, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,273 , ALLEGRO_ALIGN_LEFT, "a");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,285 , ALLEGRO_ALIGN_LEFT, "a");
                        al_draw_textf(fonteMenor, al_map_rgb(85, 255, 0), 393,297 , ALLEGRO_ALIGN_LEFT, "a");
                        al_draw_bitmap(led, 553, 50, 0);
                        upgradeVodka = 1;
                        }else{
                        al_play_sample(som_semDinheiro, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        }
                    }
                }
            }else{
                if(noBotaoLabsAnterior){
                    al_draw_bitmap(botaoUpgradeVodka2, 569, 49, 0);
                    noBotaoLabsAnterior=1;
                }
            }

        }
        while(!al_is_event_queue_empty(filaEventosTimer)){ //atualizar dinheiro na tela
            ALLEGRO_EVENT evento;
            al_wait_for_event(filaEventosTimer, &evento);

            dinheiroPorSegundo = prodDormitorio + prodEstufa + prodQuimica + prodFisica + prodRobotica + prodNuclear;
            incrementoPorFrame = dinheiroPorSegundo/FPS;
            dinheiro += incrementoPorFrame;

            al_draw_bitmap(backgroundDinheiroPorSegundo, 88, 6, 0);
            al_draw_textf(fonte, al_map_rgb(153, 229, 80), 256, 0, ALLEGRO_ALIGN_RIGHT, "%.2f", dinheiroPorSegundo);
            al_draw_bitmap(backgroundDinheiro, 263, 6, 0);
            al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA - 38, 0, ALLEGRO_ALIGN_RIGHT, "%.2f", dinheiro);

        }
        al_flip_display();
    }

    al_destroy_sample(som_botao);
    al_destroy_audio_stream(musica);
    al_destroy_timer(timer);
    al_destroy_font(fonte);
    al_destroy_display(janela);
    al_destroy_event_queue(filaEventos);
    al_destroy_event_queue(filaEventosTimer);

    return 0;
}
