#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <SDL_image.h>
#include "string"
#include <SDL_mixer.h>


#define BLOCOS 18 // QUANTIDADE
#define TIROS 18

//configuracoes
GLuint loadTexture(const std::string&fileName)
{
    SDL_Surface *imagem = IMG_Load(fileName.c_str());//carrega imagem
    SDL_DisplayFormatAlpha(imagem);//formato alpha
    unsigned objetct(0); // cria textura
    glGenTextures(1, &objetct); // gera textura
    glBindTexture(GL_TEXTURE_2D, objetct); // tipo de textura, 2D

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagem->w, imagem->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagem->pixels);

    SDL_FreeSurface(imagem);

    return objetct;


}

//logica da colisao
bool colisao(float Ax, float Ay, float Acomp, float Aalt, float Bx, float By, float Bcomp, float Balt)
{
    if(Ay+Aalt < By) return false;
    else if(Ay > By+Balt) return false;
    else if(Ax+Acomp<Bx ) return false;
    else if(Ax+Acomp < Bx) return false;
    else if(Ax > Bx+Bcomp) return false;

    return true; // houve colisao
}

//struct dos inimigos e seus tiros
struct Bloco
{
    float x;
    float y;
    float comp;
    float alt;
    bool vivo;
    float tiro;
    float varia=0;
    float inicioX;
    float inicioY;
};

struct Tiro
{
    float x;
    float y;
    float comp;
    float alt;
    bool vivo;
    float tiro;
    float varia=0;

};


int main(int argc, char* args[])
{

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Surface *tela;

    //variaveis do som
    int frequencia= 22050;
    Uint16 formato = AUDIO_S16SYS;
    int canais = 2; // 1-mono 2- stereo
    int buffer = 4096;
    Mix_OpenAudio(frequencia, formato, canais, buffer); // inicializa o audio
    Mix_Music *musica;
    musica = Mix_LoadMUS("yt1s.com - Electro  Nitro Fun  New Game Monstercat Release.mp3");
    Mix_VolumeMusic(5);


    //memoria
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // nome janela
    SDL_WM_SetCaption("Space X - a batalha do milenio", NULL);

    //tamanho janela
    tela=SDL_SetVideoMode(600,400, 32, SDL_OPENGL);

    //cor
    glClearColor(0,0,0,0);

    // area exibida
    glViewport(0,0,600,400);

    //sombra
    glShadeModel(GL_SMOOTH);

    // 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();//desenho geometrico

    // 3D
    glDisable(GL_DEPTH_TEST);

    // para uso da imagem
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //Mix_PlayMusic(som, 0);
    Mix_PlayMusic(musica, -1);

    // Variaveis da tela
    GLint   _w       = 400;
    GLint   _h       = 350;
    GLint   _Giw     = 10;
    GLint   _Gih     = 30;
    GLint   _Gfw     = 270;
    GLint   _Gfh     = 240;

    //EXECUTANDO
    bool executando = true;

    SDL_Event eventos;

    //PERSONAGEM
    float personX = 300;
    float personY = 350;
    float personComp = 50;
    float personAlt = 30;
    float distancia = 0;

    //movimento dos blocos
    bool direcao = true;
    float velXBloco=0.0;
    float velYBloco=0.0;
    int contador=0;
    float correcao=0;


    // tiro do personagem
    float inimigoX = personX;
    float inimigoY = personY;
    float inimigoComp = 20;
    float inimigoAlt = 35;
    float velX = 0.075;
    float velY = 0.075;

    //CALCULO DE INIMIGOS MORTOS
    float gameover = 0;

    //TELAS DO JOGO
    int inicio = 0;

    //VIDA
    int vida=3;
    bool pause=false;
    float num;

    //VALORES DOS STRUCT
    Bloco blocos[BLOCOS];
    Tiro tiros[TIROS];
    for(int n = 0, x = 40, y = 10; n< BLOCOS; n++, x+=90 )
    {
        if(x>560)
        {
            x = 40;
            y +=40;
        }
        blocos[n].x = x;
        blocos[n].y = y;
        blocos[n].comp = 60;
        blocos[n].alt = 40;
        blocos[n].vivo = true;
        blocos[n].tiro = 0;
    }
    for(int n = 0, x = 40, y = 10; n< TIROS; n++, x+=90 )
    {
        if(x>560)
        {
            x = 40;
            y +=40;
        }
        tiros[n].x = x;
        tiros[n].y = y;
        tiros[n].comp = 60;
        tiros[n].alt = 40;
        tiros[n].vivo = true;
        tiros[n].tiro = 0;
    }

    //variaveis boolean para movimento e tiro do personagem
    bool esq = false, dir = false;
    bool atirar = false;

    //-------TEXTURAS-------//

    //textura personagem
    unsigned int person_textura = 0;
    person_textura = loadTexture("rsH6n.png");

    //textura do tiro
    unsigned int bola_textura = 0;
    bola_textura = loadTexture("unnamed.png");

    //textura dos inimigo
    unsigned int bloco_textura = 0;
    bloco_textura = loadTexture("Gunship_mf_Sprite.png");

    //textura do fundo
    unsigned int fundo_textura = 0;
    fundo_textura = loadTexture("4e417977990fda9cded891b61a216130.png");

    //textura da vitoria
    unsigned int vitoria_textura = 0;
    vitoria_textura = loadTexture("Vitoria.png");

    //textura do tiro do nimigo
    unsigned int tiroinimigo_textura = 0;
    tiroinimigo_textura = loadTexture("tiroinimigo.png");

    //textura da tela inicial
    unsigned int inicio_textura = 0;
    inicio_textura = loadTexture("TelaInicial.png");

    //textura da tela de inicial
    unsigned int tutorial_textura = 0;
    tutorial_textura = loadTexture("TrueTutorial.png");

    //textura da tela de derrota
    unsigned int derrota_textura = 0;
    derrota_textura = loadTexture("Defeat.png");

    //textura das vidas
    unsigned int vida_textura = 0;
    vida_textura = loadTexture("heart pixel art 254x254.png");

    //textura da tela de pause
    unsigned int pause_textura = 0;
    pause_textura = loadTexture("Pause.png");

    //textura da tela de creditos
    unsigned int creditos_textura = 0;
    creditos_textura = loadTexture("TheCreditos.png");


    // loop do jogo
    while(executando)
    {
        //-----------------------MENU INICIAL--------------------//
        if(inicio==0)
        {
            //reset
            gameover=0;
            vida=3;
            personX=300;
            esq=false;
            dir=false;
            velXBloco=0;
            velYBloco=0;
            contador=0;

            direcao=true;
            for(int i = 0; i <BLOCOS; i++)
            {
                blocos[i].vivo = true;
            }
            for(int k = 0; k <TIROS; k++)
            {
                tiros[k].tiro=0;
                tiros[k].vivo = true;
                tiros[k].varia = 0;
            }

            // eventos
            while(SDL_PollEvent(&eventos))
            {
                // fecha com o x da janela
                if(eventos.type == SDL_QUIT)
                {
                    executando = false;
                }
                if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                {
                    executando = false;
                }

                //tecla enter
                if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                {
                    if(eventos.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        inicio++;

                    }
                    else if(eventos.key.keysym.sym == SDLK_RETURN)
                    {
                        inicio++;
                    }

                }



            }
            // RENDERIZACAO
            glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

            //inicia matriz
            glPushMatrix();

            // dimensoes da matriz
            glOrtho(0, 600, 400, 0, -1, 1);


            glColor4ub(255, 255, 255, 255);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, inicio_textura);


            glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

            glTexCoord2d(0,0);
            glVertex2f(0, 0);//primeiro ponto
            glTexCoord2d(1,0);
            glVertex2f(600, 0); //segundo ponto
            glTexCoord2d(1,1);
            glVertex2f(600,400 );
            glTexCoord2d(0,1);
            glVertex2f(0,400 );

            glDisable(GL_TEXTURE_2D);
            glEnd();

            //fecha matrix
            glPopMatrix();

            // ANIMACAO
            SDL_GL_SwapBuffers();
        }
        //-----------------------TUTORIAL--------------------//
        if(inicio==1)
        {

            // eventos
            while(SDL_PollEvent(&eventos))
            {
                // fecha com o x da janela
                if(eventos.type == SDL_QUIT)
                {
                    executando = false;
                }
                if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                {
                    executando = false;
                }

                //tecla enter
                if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                {
                    if(eventos.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        inicio++;

                    }
                    else if(eventos.key.keysym.sym == SDLK_RETURN)
                    {
                        inicio++;
                    }

                }


            }
            // RENDERIZACAO
            glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

            //inicia matriz
            glPushMatrix();

            // dimensoes da matriz
            glOrtho(0, 600, 400, 0, -1, 1);


            glColor4ub(255, 255, 255, 255);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tutorial_textura);


            glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

            glTexCoord2d(0,0);
            glVertex2f(0, 0);//primeiro ponto
            glTexCoord2d(1,0);
            glVertex2f(600, 0); //segundo ponto
            glTexCoord2d(1,1);
            glVertex2f(600,400 );
            glTexCoord2d(0,1);
            glVertex2f(0,400 );

            glDisable(GL_TEXTURE_2D);
            glEnd();

            //fecha matrix
            glPopMatrix();

            // ANIMACAO
            SDL_GL_SwapBuffers();
        }
        //-----------------------O JOGO----------------------//
        if(inicio==2)
        {
            //JOGO
            if(pause==true)
            {
                // eventos
                while(SDL_PollEvent(&eventos))
                {
                    if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                    {
                        if(eventos.key.keysym.sym == SDLK_p)
                        {
                            pause=false;

                        }
                    }
                }


                // RENDERIZACAO
                glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

                //inicia matriz
                glPushMatrix();

                // dimensoes da matriz
                glOrtho(0, 600, 400, 0, -1, 1);


                glColor4ub(255, 255, 255, 255);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, pause_textura);


                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                glTexCoord2d(0,0);
                glVertex2f(0, 0);//primeiro ponto
                glTexCoord2d(1,0);
                glVertex2f(600, 0); //segundo ponto
                glTexCoord2d(1,1);
                glVertex2f(600,400 );
                glTexCoord2d(0,1);
                glVertex2f(0,400 );

                glDisable(GL_TEXTURE_2D);
                glEnd();

                //fecha matrix
                glPopMatrix();

                // ANIMACAO
                SDL_GL_SwapBuffers();
            }
            else if(pause==false)
            {
                // eventos
                while(SDL_PollEvent(&eventos))
                {
                    // fecha com o x da janela
                    if(eventos.type == SDL_QUIT)
                    {
                        executando = false;
                    }
                    if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                    {
                        executando = false;
                    }

                    //anda ou nao
                    if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                    {
                        if(eventos.key.keysym.sym == SDLK_LEFT)
                        {
                            esq = true;
                        }
                        else if(eventos.key.keysym.sym == SDLK_RIGHT)
                        {
                            dir = true;
                        }
                    }
                    else if(eventos.type == SDL_KEYUP)
                    {
                        if(eventos.key.keysym.sym == SDLK_LEFT)
                        {
                            esq = false;
                        }
                        else if(eventos.key.keysym.sym == SDLK_RIGHT)
                        {
                            dir = false;
                        }
                    }
                    //tecla tiro
                    if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                    {
                        if(eventos.key.keysym.sym == SDLK_SPACE)
                        {
                            atirar = true;
                            if(inimigoY + distancia == personY)
                                inimigoX = personX;

                        }

                    }
                    if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                    {
                        if(eventos.key.keysym.sym == SDLK_r)
                        {
                            inicio=0;
                        }

                    }
                    //pause
                    if(eventos.type == SDL_KEYDOWN)//tecla pressionada
                    {
                        if(eventos.key.keysym.sym == SDLK_p)
                            pause=true;
                    }

                }

                // LOGICA

                // movimento do personagem

                if(esq == true)// se seta esquerda for pressionada
                {
                    personX -= 0.12;

                }
                else if(dir == true)
                {
                    personX += 0.12;

                }

                if(personX<0)
                {
                    personX = 0;
                }
                else if(personX + personComp>600)
                {
                    personX = 600 - personComp;
                }


                // RENDERIZACAO
                glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

                //inicia matriz
                glPushMatrix();

                // dimensoes da matriz
                glOrtho(0, 600, 400, 0, -1, 1);


                //imagem de fundo
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, fundo_textura);
                glColor4ub(255, 255, 255, 255);

                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                glTexCoord2d(0,0);
                glVertex2f(0, 0);//primeiro ponto
                glTexCoord2d(1,0);
                glVertex2f(600, 0); //segundo ponto
                glTexCoord2d(1,1);
                glVertex2f(600,400 );
                glTexCoord2d(0,1);
                glVertex2f(0,400 );

                glDisable(GL_TEXTURE_2D);
                glEnd();



                //inicia desenho do personagem
                glColor4ub(255, 255, 255, 255);//branco

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, person_textura);


                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                glTexCoord2d(0,0);
                glVertex2f(personX, personY);//primeiro ponto
                glTexCoord2d(1,0);
                glVertex2f(personX+personComp, personY);//segundo ponto
                glTexCoord2d(1,1);
                glVertex2f(personX+personComp, personY+personAlt);
                glTexCoord2d(0,1);
                glVertex2f(personX, personY+personAlt);


                glEnd();

                glDisable(GL_TEXTURE_2D);

                float Ytotal;
                float Xtotal;

                glColor4ub(255, 255, 255, 255);//branco

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, bola_textura);

                // inicia o desenho do tiro do personagem
                if(atirar  == true)
                {
                    glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                    glTexCoord2d(0,0);
                    glVertex2f(inimigoX+14, inimigoY+distancia);//primeiro ponto
                    glTexCoord2d(1,0);
                    glVertex2f(inimigoX+inimigoComp+14, inimigoY+distancia);//segundo ponto
                    glTexCoord2d(1,1);
                    glVertex2f(inimigoX+14+inimigoComp, inimigoY+inimigoAlt+distancia);
                    glTexCoord2d(0,1);
                    glVertex2f(inimigoX+14, inimigoY+inimigoAlt+distancia);

                    //fecha
                    glDisable(GL_TEXTURE_2D);
                    glEnd();


                    distancia -= 0.15;
                    if(personY+distancia<=0)
                    {
                        atirar = false;
                        distancia=0;

                    }

                    Ytotal = inimigoY+distancia;
                    Xtotal = inimigoX+14;
                    for(int n = 0; n < BLOCOS; n++)
                    {
                        if(blocos[n].vivo == true)
                        {
                            //COLISAO DOS INIMIGOS COM O TIRO DO PERSONAGEM
                            if(colisao(Xtotal, Ytotal, inimigoComp, inimigoAlt, blocos[n].x+correcao, blocos[n].y, blocos[n].comp, blocos[n].alt) == true)
                            {
                                blocos[n].vivo = false;
                                atirar = false;
                                inimigoX = personX;
                                distancia = 0;
                                gameover++;
                                break;
                            }
                        }
                    }


                }

                //imagem de fundo

                glColor4ub(255, 255, 255, 255);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, fundo_textura);


                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                glTexCoord2d(0,0);
                glVertex2f(0, 0);//primeiro ponto
                glTexCoord2d(1,0);
                glVertex2f(600, 0); //segundo ponto
                glTexCoord2d(1,1);
                glVertex2f(600,400 );
                glTexCoord2d(0,1);
                glVertex2f(0,400 );

                glDisable(GL_TEXTURE_2D);
                glEnd();

                // VIDA -- CORACOES
                glColor4ub(255, 255, 255, 255);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, vida_textura);
                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON
                for(int j=1; j<=vida; j++)
                {

                    glTexCoord2d(0,0);
                    glVertex2f(0+40*(j-1), 360);
                    glTexCoord2d(1,0);
                    glVertex2f(40*j, 360);

                    glTexCoord2d(1,1);
                    glVertex2f(40*j,400 );
                    glTexCoord2d(0,1);
                    glVertex2f(0+40*(j-1),400 );
                }


                glDisable(GL_TEXTURE_2D);
                glEnd();


                // cor
                glColor4ub(255, 255, 255, 255);//branco

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, person_textura);

                //inicia desenho do personagem
                glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                glTexCoord2d(0,0);
                glVertex2f(personX, personY);//primeiro ponto
                glTexCoord2d(1,0);
                glVertex2f(personX+personComp, personY);//segundo ponto
                glTexCoord2d(1,1);
                glVertex2f(personX+personComp, personY+personAlt);
                glTexCoord2d(0,1);
                glVertex2f(personX, personY+personAlt);

                //fecha
                glEnd();

                glDisable(GL_TEXTURE_2D);

                glColor4ub(255, 255, 255, 255);//branco

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, bola_textura);

                // inicia o desenho do tiro
                if(atirar  == true)
                {
                    glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

                    glTexCoord2d(0,0);
                    glVertex2f(inimigoX+14, inimigoY+distancia);//primeiro ponto
                    glTexCoord2d(1,0);
                    glVertex2f(inimigoX+inimigoComp+14, inimigoY+distancia);//segundo ponto
                    glTexCoord2d(1,1);
                    glVertex2f(inimigoX+14+inimigoComp, inimigoY+inimigoAlt+distancia);
                    glTexCoord2d(0,1);
                    glVertex2f(inimigoX+14, inimigoY+inimigoAlt+distancia);



                    glEnd();


                    distancia -= 0.15;
                    if(personY+distancia<=0)
                    {
                        atirar = false;
                        distancia=0;

                    }

                    Ytotal = inimigoY+distancia;
                    Xtotal = inimigoX+14;
                    for(int n = 0; n < BLOCOS; n++)
                    {
                        if(blocos[n].vivo == true)
                        {
                            if(colisao(Xtotal, Ytotal, inimigoComp, inimigoAlt, blocos[n].x+velXBloco, blocos[n].y+velYBloco, blocos[n].comp, blocos[n].alt) == true)
                            {
                                blocos[n].vivo = false;
                                atirar = false;
                                inimigoX = personX;
                                distancia = 0;
                                gameover++;
                                break;
                            }
                        }
                    }


                }
                glDisable(GL_TEXTURE_2D);


                //------------------BLOCOS-------------//


                // inicia desenho dos blocos

                glColor4ub(255, 255, 255, 255);//branco


                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, bloco_textura);
                glBegin(GL_QUADS);

                for(int n = 0; n <BLOCOS; n++)
                {
                    if(blocos[n].vivo == true)
                    {

                        glTexCoord2d(0,0);
                        glVertex2f(blocos[n].x+velXBloco+correcao, blocos[n].y+velYBloco);
                        glTexCoord2d(1,0);
                        glVertex2f(blocos[n].x+velXBloco+correcao+ blocos[n].comp, blocos[n].y+velYBloco);
                        glTexCoord2d(1,1);
                        glVertex2f(blocos[n].x+velXBloco+correcao+ blocos[n].comp, blocos[n].y + blocos[n].alt+velYBloco);
                        glTexCoord2d(0,1);
                        glVertex2f(blocos[n].x+velXBloco+correcao, blocos[n].y + blocos[n].alt+velYBloco);

                        if((blocos[n].y+blocos[n].alt+velYBloco)>=400)
                        {
                            inicio=3;
                        }

                        if(direcao==true)
                        {
                            contador++;
                            if(contador%15==0)
                                velXBloco+=0.02;
                        }
                        else if (direcao==false)
                        {
                            contador--;
                            if(contador%15==0)
                                velXBloco-=0.02;
                        }
                        if(contador==38000)
                        {
                            direcao=false;
                            velYBloco+=30;

                        }
                        else if(contador==-33000)
                        {
                            direcao=true;
                            velYBloco+=30;

                        }



                    }


                }
                glDisable(GL_TEXTURE_2D);

                glEnd();



                // inicia desenho dos tiros dos blocos

                glColor4ub(255, 255, 255, 255);//branco

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, tiroinimigo_textura);
                glBegin(GL_QUADS);

                //tiro.inicio
                for(int n = 0; n <BLOCOS; n++)
                {
                    if(tiros[n].vivo == true)
                    {
                        if(n==1+tiros[n-1].varia|n==17+tiros[n-1].varia||n==19+tiros[n-1].varia||n==6+tiros[n-1].varia||n==9+tiros[n-1].varia||n==3+tiros[n-1].varia)
                        {
                            if(tiros[n].y+tiros[n].tiro>=400)
                            {
                                if(blocos[n].vivo==false)
                                    tiros[n].vivo=false;
                                tiros[n].tiro = 0;
                                tiros[n].varia++;
                            }

                            glTexCoord2d(0,0);
                            glVertex2f(tiros[n].x+28+correcao, tiros[n].y+tiros[n].tiro+velYBloco);
                            glTexCoord2d(1,0);
                            glVertex2f(tiros[n].x+28+correcao + tiros[n].comp-50, tiros[n].y+tiros[n].tiro+velYBloco);
                            glTexCoord2d(1,1);
                            glVertex2f(tiros[n].x+28+correcao + tiros[n].comp-50, tiros[n].y+ tiros[n].alt-15+tiros[n].tiro+velYBloco);
                            glTexCoord2d(0,1);
                            glVertex2f(tiros[n].x+28+correcao, tiros[n].y + tiros[n].alt-15+tiros[n].tiro+velYBloco);
                            tiros[n].tiro+=0.048;

                        }
                        //COLISAO DOS TIROS COM O PERSONAGEM
                        if(colisao(tiros[n].x+30+correcao, tiros[n].y+tiros[n].tiro+velYBloco, tiros[n].comp-50, tiros[n].alt,personX, personY, personComp, personAlt) == true)
                        {
                            vida--;
                            tiros[n].tiro+=1000;
                            if(vida==0)
                                inicio++;
                        }
                    }

                }
                glDisable(GL_TEXTURE_2D);
                glEnd();


                if(gameover==BLOCOS)
                    inicio=4;



                //fecha matrix
                glPopMatrix();

                // ANIMACAO
                SDL_GL_SwapBuffers();
            }





        }
        //-----------------------TELA DERROTA--------------------//
        if(inicio==3)
        {
            gameover=0;
            vida=3;
            personX=300;
            esq=false;
            dir=false;
            for(int j = 0; j <BLOCOS; j++)
            {
                blocos[j].vivo = true;
            }
            for(int k = 0; k <TIROS; k++)
            {
                tiros[k].tiro=0;
                tiros[k].vivo = true;
            }
            // eventos
            while(SDL_PollEvent(&eventos))
            {
                // fecha com o x da janela
                if(eventos.type == SDL_QUIT)
                {
                    executando = false;
                }
                if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                {
                    executando = false;
                }

                //tecla enter
                if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                {
                    if(eventos.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        inicio=0;

                    }
                    else if(eventos.key.keysym.sym == SDLK_RETURN)
                    {
                        inicio=0;
                    }

                }



            }
            // RENDERIZACAO
            glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

            //inicia matriz
            glPushMatrix();

            // dimensoes da matriz
            glOrtho(0, 600, 400, 0, -1, 1);


            glColor4ub(255, 255, 255, 255);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, derrota_textura);


            glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

            glTexCoord2d(0,0);
            glVertex2f(0, 0);//primeiro ponto
            glTexCoord2d(1,0);
            glVertex2f(600, 0); //segundo ponto
            glTexCoord2d(1,1);
            glVertex2f(600,400 );
            glTexCoord2d(0,1);
            glVertex2f(0,400 );

            glDisable(GL_TEXTURE_2D);
            glEnd();

            //fecha matrix
            glPopMatrix();

            // ANIMACAO
            SDL_GL_SwapBuffers();
        }
        //-----------------------TELA VITORIA--------------------//
        if(inicio==4)
        {
            //Vitoria
            gameover=0;
            vida=3;
            personX=300;
            esq=false;
            dir=false;
            for(int i = 0; i <BLOCOS; i++)
            {
                blocos[i].vivo = true;
            }
            for(int k = 0; k <TIROS; k++)
            {
                tiros[k].tiro=0;
                tiros[k].vivo = true;
            }
            // eventos
            while(SDL_PollEvent(&eventos))
            {
                // fecha com o x da janela
                if(eventos.type == SDL_QUIT)
                {
                    executando = false;
                }
                if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                {
                    executando = false;
                }

                //tecla press
                if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                {
                    if(eventos.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        inicio=0;

                    }
                    else if(eventos.key.keysym.sym == SDLK_RETURN)
                    {
                        inicio=0;
                    }
                    else if(eventos.key.keysym.sym == SDLK_c)
                    {
                        inicio=5;
                    }
                }



            }
            // RENDERIZACAO
            glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

            //inicia matriz
            glPushMatrix();

            // dimensoes da matriz
            glOrtho(0, 600, 400, 0, -1, 1);


            glColor4ub(255, 255, 255, 255);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, vitoria_textura);


            glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

            glTexCoord2d(0,0);
            glVertex2f(0, 0);//primeiro ponto
            glTexCoord2d(1,0);
            glVertex2f(600, 0); //segundo ponto
            glTexCoord2d(1,1);
            glVertex2f(600,400 );
            glTexCoord2d(0,1);
            glVertex2f(0,400 );

            glDisable(GL_TEXTURE_2D);
            glEnd();

            //fecha matrix
            glPopMatrix();

            // ANIMACAO
            SDL_GL_SwapBuffers();
        }

        //-----------------------TELA CREDITOS--------------------//
        if(inicio==5)
        {
            while(SDL_PollEvent(&eventos))
            {
                // fecha com o x da janela
                if(eventos.type == SDL_QUIT)
                {
                    executando = false;
                }
                if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym  == SDLK_ESCAPE)
                {
                    executando = false;
                }

                //tecla enter
                if(eventos.type == SDL_KEYDOWN)// tecla pressionada
                {
                    if(eventos.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        inicio=0;

                    }
                    else if(eventos.key.keysym.sym == SDLK_RETURN)
                    {
                        inicio=0;
                    }

                }

            }

            // RENDERIZACAO
            glClear(GL_COLOR_BUFFER_BIT);// LIMPA O BUFFER

            //inicia matriz
            glPushMatrix();

            // dimensoes da matriz
            glOrtho(0, 600, 400, 0, -1, 1);


            glColor4ub(255, 255, 255, 255);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, creditos_textura);


            glBegin(GL_QUADS);// GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

            glTexCoord2d(0,0);
            glVertex2f(0, 0);//primeiro ponto
            glTexCoord2d(1,0);
            glVertex2f(600, 0); //segundo ponto
            glTexCoord2d(1,1);
            glVertex2f(600,400 );
            glTexCoord2d(0,1);
            glVertex2f(0,400 );

            glDisable(GL_TEXTURE_2D);
            glEnd();

            //fecha matrix
            glPopMatrix();

            // ANIMACAO
            SDL_GL_SwapBuffers();
        }


    }

    printf("\n Executando \n");


    Mix_FreeMusic(musica);
    Mix_CloseAudio();

    glDisable(GL_BLEND);

    SDL_Quit();

    return 0;
    //-----------------------FIM DO PROGRAMA--------------------//
}
