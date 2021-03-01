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
#define MAX_LEN 256

double dinheiro = 100;

double custoBaseDormitorio = 10;
double prodBaseDormitorio = 0.2;
double taxaCrescimentoDormitorio = 1.07;
double custoBaseEstufa = 420;
double prodBaseEstufa = 2;
double taxaCrescimentoEstufa = 1.15;
double custoBaseQuimica = 1926;
double prodBaseQuimica = 9;
double taxaCrescimentoQuimica = 1.14;
double custoBaseFisica = 314159;
double prodBaseFisica = 36;
double taxaCrescimentoFisica = 1.13;
double custoBaseRobotica = 1000101;
double prodBaseRobotica = 216;
double taxaCrescimentoRobotica = 1.12;
double custoBaseNuclear = 713000000;
double prodBaseNuclear = 10000;
double taxaCrescimentoNuclear = 1.10;

double custoUpgradeVodka = 1;

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
ALLEGRO_BITMAP *background = NULL;
ALLEGRO_BITMAP *fecharBotao = NULL;
ALLEGRO_BITMAP *fecharBotao2 = NULL;
ALLEGRO_BITMAP *botaoLabs = NULL;
ALLEGRO_BITMAP *botaoLabs2 = NULL;
ALLEGRO_BITMAP *backgroundDinheiro = NULL;
ALLEGRO_BITMAP *backgroundLabs = NULL;
ALLEGRO_BITMAP *backgroundDinheiroPorSegundo = NULL;
ALLEGRO_BITMAP *backgroundMonitor = NULL;
ALLEGRO_BITMAP *backgroundUpgrade = NULL;
ALLEGRO_BITMAP *backgroundUpgrade2 = NULL;
ALLEGRO_BITMAP *botaoSavegame = NULL;
ALLEGRO_BITMAP *botaoSavegame2 = NULL;
ALLEGRO_BITMAP *botaoNovoJogo = NULL;
ALLEGRO_BITMAP *botaoNovoJogo2 = NULL;
ALLEGRO_BITMAP *led = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_SAMPLE *som_botao = NULL;
ALLEGRO_SAMPLE *som_semDinheiro = NULL;
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

double custoCalculo(double custoBase, double qtde, double taxaCrescimento){
    custo = custoBase*pow(taxaCrescimento,qtde);
    return custo;
}

double prodCalculo(double prodBase, double qtde, double multiplier){
    prod = (prodBase*qtde)*multiplier;
    return prod;
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

    som_botao = al_load_sample( "resources/buttonSound.ogg" );
    if (!som_botao){
        error_msg( "Audio nao carregado" );
        return 0;
    }

    som_semDinheiro = al_load_sample( "resources/semDinheiro.ogg" );
    if (!som_botao){
        error_msg( "Audio nao carregado" );
        al_destroy_sample(som_botao);
        return 0;
    }

    musica = al_load_audio_stream("resources/soundtrack.ogg", 4, 1024);
    if (!musica){
        al_destroy_sample(som_botao);
        al_destroy_sample(som_semDinheiro);
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

    background = al_load_bitmap("resources/BackgroundFinal.png");
    fecharBotao = al_load_bitmap("resources/SpriteFechar.png");
    fecharBotao2 = al_load_bitmap("resources/SpriteFechar2.png");
    botaoLabs = al_load_bitmap("resources/SpriteBotaoLabs.png");
    botaoLabs2 = al_load_bitmap("resources/SpriteBotaoLabs2.png");
    backgroundDinheiro = al_load_bitmap("resources/SpriteBackgroundDinheiro.png");
    backgroundLabs = al_load_bitmap("resources/SpriteBackgroundLabs.png");
    backgroundDinheiroPorSegundo = al_load_bitmap("resources/SpriteBackgroundDinheiroPorSegundo.png");
    backgroundMonitor = al_load_bitmap("resources/SpriteBackgroundMonitor.png");
    backgroundUpgrade = al_load_bitmap("resources/SpriteBotaoUpgrade.png");
    backgroundUpgrade2 = al_load_bitmap("resources/SpriteBotaoUpgrade2.png");
    botaoSavegame = al_load_bitmap("resources/spriteSavegame.png");
    botaoSavegame2 = al_load_bitmap("resources/spriteSavegame2.png");
    botaoNovoJogo = al_load_bitmap("resources/SpriteBotaoNovojogo.png");
    botaoNovoJogo2 = al_load_bitmap("resources/SpriteBotaoNovojogo2.png");
    led = al_load_bitmap("resources/SpriteLuzLigada.png");

    al_draw_bitmap(background, 0, 0, 0);

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
    prodDormitorio = prodCalculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
    prodEstufa = prodCalculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
    prodQuimica = prodCalculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
    prodFisica = prodCalculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
    prodRobotica = prodCalculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
    prodNuclear = prodCalculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
    custoDormitorio = custoCalculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
    custoEstufa = custoCalculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
    custoQuimica = custoCalculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
    custoFisica = custoCalculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
    custoRobotica = custoCalculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
    custoNuclear = custoCalculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);

    if(upgradeVodka){
        al_draw_bitmap(led, 553, 50, 0);
    }

    al_register_event_source(filaEventos, al_get_keyboard_event_source());
    al_register_event_source(filaEventos, al_get_display_event_source(janela));
    al_register_event_source(filaEventos, al_get_mouse_event_source());
    al_register_event_source(filaEventosTimer, al_get_timer_event_source(timer));

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

    al_flip_display();

    al_start_timer(timer);

    return 1;
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

int main(void){

    int sair = 0;

    if (!inicializar()){
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

                al_draw_bitmap(fecharBotao, 6, 6, 0);
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
                    al_draw_bitmap(fecharBotao2, 6, 6, 0);
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
                    }

                }
            }else{
                if(noBotaoNovoJogoAnterior){
                    al_draw_bitmap(botaoNovoJogo2, 608, 280, 0);
                }
            }

            if (evento.mouse.x >= red_x*5 &&  //bot�o para clicar no satélite
                evento.mouse.x <= red_x*564 &&
                evento.mouse.y <= red_y*271 &&
                evento.mouse.y >= red_y*32) {

                noBotaoSateliteAnterior = 1;

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    dinheiro += upgradeMultiplier;
                    al_draw_bitmap(backgroundDinheiro, 263, 6, 0);
                    al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA - 38, 0, ALLEGRO_ALIGN_RIGHT, "%.2f", dinheiro);
                }
            }else{
                if(noBotaoSateliteAnterior){

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
                    custoDormitorio = custoCalculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
                    if(custoDormitorio<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeDormitorio += 1;
                        dinheiro -= custoDormitorio;
                        prodDormitorio = prodCalculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 279, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 277, ALLEGRO_ALIGN_LEFT, "DORMITORIO: %.0f", qtdeDormitorio);
                        custoDormitorio = custoCalculo(custoBaseDormitorio, qtdeDormitorio, taxaCrescimentoDormitorio);
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
                    custoEstufa = custoCalculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
                    if(custoEstufa<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeEstufa += 1;
                        dinheiro -= custoEstufa;
                        prodEstufa = prodCalculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 307, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 305, ALLEGRO_ALIGN_LEFT, "ESTUFA: %.0f", qtdeEstufa);
                        custoEstufa = custoCalculo(custoBaseEstufa, qtdeEstufa, taxaCrescimentoEstufa);
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
                    custoQuimica = custoCalculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
                    if(custoQuimica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeQuimica += 1;
                        dinheiro -= custoQuimica;
                        prodQuimica = prodCalculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 53, 335, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 53, 333, ALLEGRO_ALIGN_LEFT, "QUIMICA: %.0f", qtdeQuimica);
                        custoQuimica = custoCalculo(custoBaseQuimica, qtdeQuimica, taxaCrescimentoQuimica);
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
                    custoFisica = custoCalculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
                    if(custoFisica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeFisica += 1;
                        dinheiro -= custoFisica;
                        prodFisica = prodCalculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 279, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 277, ALLEGRO_ALIGN_LEFT, "FISICA: %.0f", qtdeFisica);
                        custoFisica = custoCalculo(custoBaseFisica, qtdeFisica, taxaCrescimentoFisica);
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
                    custoRobotica = custoCalculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
                    if(custoRobotica<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeRobotica += 1;
                        dinheiro -= custoRobotica;
                        prodRobotica = prodCalculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 307, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 305, ALLEGRO_ALIGN_LEFT, "ROBOTICA: %.0f", qtdeRobotica);
                        custoRobotica = custoCalculo(custoBaseRobotica, qtdeRobotica, taxaCrescimentoRobotica);
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
                    custoNuclear = custoCalculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);
                    if(custoNuclear<=dinheiro){
                        al_play_sample(som_botao, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                        qtdeNuclear += 1;
                        dinheiro -= custoNuclear;
                        prodNuclear = prodCalculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
                        al_draw_bitmap(backgroundLabs, 216, 335, 0);
                        al_draw_textf(fonteMenor, al_map_rgb(55, 68, 89), 216, 333, ALLEGRO_ALIGN_LEFT, "NUCLEAR: %.0f", qtdeNuclear);
                        custoNuclear = custoCalculo(custoBaseNuclear, qtdeNuclear, taxaCrescimentoNuclear);
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

                al_draw_bitmap(backgroundUpgrade, 569, 49, 0);
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
                        prodDormitorio = prodCalculo(prodBaseDormitorio, qtdeDormitorio, upgradeMultiplier);
                        prodEstufa = prodCalculo(prodBaseEstufa, qtdeEstufa, upgradeMultiplier);
                        prodQuimica = prodCalculo(prodBaseQuimica, qtdeQuimica, upgradeMultiplier);
                        prodFisica = prodCalculo(prodBaseFisica, qtdeFisica, upgradeMultiplier);
                        prodRobotica = prodCalculo(prodBaseRobotica, qtdeRobotica, upgradeMultiplier);
                        prodNuclear = prodCalculo(prodBaseNuclear, qtdeNuclear, upgradeMultiplier);
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
                    al_draw_bitmap(backgroundUpgrade2, 569, 49, 0);
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
