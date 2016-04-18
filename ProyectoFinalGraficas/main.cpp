#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <fstream>
#include <string>
#include "glm.h"
#include "imageloader.h"
#include "Image.h"


using namespace std;

//Amount of models and model ids
#define MODEL_COUNT 10
#define PLAYER_MOD 0
#define PILDORA_MOD 1
#define BACTERIA_MOD 2
#define BEBE_MOD 3
#define FRUTA_MOD 4
#define VERDURA_MOD 5
#define JABON_MOD 6
#define ANTI_MOD 7
#define FRUTA2_MOD 8

GLMmodel models[MODEL_COUNT];

string fullPath = __FILE__;
// variables de dimenciones
double orthosize = 4;
double windowWidth = 500;
double windowHeight = 650;
double viewportGameWidth = 500;
double viewportGameHeight = 500;
double bannerHeight = 150;
double statsSquare = 100;

// variables de juego
int vidas = 2;
double segundos = 60.0;
bool juegoIniciado = false;
bool ganoJuego = false;
bool perdioJuego = false;
bool juegoPausado = false;
int itemsRecogidos = 0;
int totalItems = 6;
int consejoActual = 0;
int angulo =0;

// variables jugador
float posXJugador = -25;
float posYJugador = 28;
float velocidadPersonaje = 0.75;
float hitBox = 2.5;

// variables items
// item 1
float posXIt1 = -3;
float posYIt1 = 15;
bool activoIt1 = true;
// item 2
float posXIt2 = 22;
float posYIt2 = 28;
bool activoIt2 = true;
// item 3
float posXIt3 = 2.5;
float posYIt3 = -2;
bool activoIt3 = true;
// item 4
float posXIt4 = -25;
float posYIt4 = -5;
bool activoIt4 = true;
// item 5
float posXIt5 = 0;
float posYIt5 = -25;
bool activoIt5 = true;
// item 6
float posXIt6 = -28;
float posYIt6 = -28;
bool activoIt6 = true;

// variables de enemigos
float posXPildora = 30;
float posYPildora = 28;
float posXBacteria = 30;
float posYBacteria = -28;
float posXBebe = -30;
float posYBebe = -28;
float velocidadPildora = 1.25;
float velocidadBebe = 1.03;
float velocidadBacteria = 1.51;

// variables de texturas
const int TEXTURE_COUNT=12;
const int TEXTURE_MENU = 0;
const int TEXTURE_INSTRUCCIONES = 1;
const int TEXTURE_CREDITOS = 2;
const int TEXTURE_FONDO = 3;
const int TEXTURE_BLOQUE = 4;
const int TEXTURE_STATS = 5;
const int TEXTURE_C1 = 6;
const int TEXTURE_C2 = 7;
const int TEXTURE_C3 = 8;
const int TEXTURE_C4 = 9;
const int TEXTURE_GANAR = 10;
const int TEXTURE_PERDER = 11;


static GLuint texName[TEXTURE_COUNT];
int bannerSeleccionado = TEXTURE_MENU;

void getParentPath(){
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '\\'; i--) {
        fullPath.erase(i,1);
    }
}

bool checaColisionParedes(float x, float y){
    // revisar si la siguiente posicion del jugador no colisiona con algun muro

    // PARED SUPERIOR DERECHA
    return ((x >= 14 && x <= 32) && (y >= 22.5 && y <= 25)) ||
    // PARED SUPERIOR IZQUIERDA
    ((x <= -14 && x >= -32) && (y >= 22.5 && y <= 25)) ||
    // PARED INFERIOR DERECHA
    ((x >= 14 && x <= 32) && (y <= -22.5 && y >= -25)) ||
    // PARED INFERIOR IZQUIERDA
    ((x <= -14 && x >= -32) && (y <= -22.5 && y >= -25)) ||
    // IZQ CRUZ CENTRAL
    ((x >= -19 && x <= -1.5) && (y <= 5 && y >= 0)) ||
    // ARRIBA CRUZ CENTRAL
    ((x >= -1.5 && x <= 1.5) && (y <= 22 && y >= 5)) ||
    // DERECHA CRUZ CENTRAL
    ((x >= 1.5 && x <= 19) && (y <= 5 && y >= 0)) ||
    // ABAJO CRUZ CENTRAL
    ((x >= -1.5 && x <= 1.5) && (y >= -17 && y <= 0));
}

bool checaColisionPersonaje(float x, float y){
    return ( x<(posXJugador+hitBox) && x >(posXJugador-hitBox)) &&
        (y<(posYJugador+hitBox) && y >(posYJugador-hitBox));
}

void movimientoEnemigo(float &x, float &y, float velocidad){
    if (posXJugador > x){
        if (!checaColisionParedes(x+velocidad,y)){
            x+=velocidad;
        }
    } else {
        if (!checaColisionParedes(x-velocidad,y)){
            x-=velocidad;
        }
    }

    if (posYJugador > y){
        if (!checaColisionParedes(x,y+velocidad)){
            y+=velocidad;
        }
    } else {
        if (!checaColisionParedes(x,y-velocidad)){
            y-=velocidad;
        }
    }
}

void reiniciarPosicionPersonajes(){
     posXPildora = 30;
     posYPildora = 28;
     posXBacteria = 30;
     posYBacteria = -28;
     posXBebe = -30;
     posYBebe = -28;
     posXJugador = -25;
     posYJugador = 28;
}

void reiniciarJuego(){
    reiniciarPosicionPersonajes();
    vidas = 2;
    segundos = 60.0;
    juegoIniciado = false;
    itemsRecogidos = 0;
    // items
    activoIt1 = true;
    activoIt2 = true;
    activoIt3 = true;
    activoIt4 = true;
    activoIt5 = true;
    activoIt6 = true;
}

void myTimer(int i) {
    angulo = (angulo + 10) % 360;
    if (juegoIniciado && !juegoPausado){
        segundos-=0.25;
        // MOVIMIENTO ENEMIGOS
        movimientoEnemigo(posXPildora,posYPildora,velocidadPildora);
        movimientoEnemigo(posXBacteria,posYBacteria,velocidadBacteria);
        movimientoEnemigo(posXBebe,posYBebe,velocidadBebe);


        // REVISAR COLISION ENEMIGOS CON JUGADOR
        if (checaColisionPersonaje(posXPildora,posYPildora) || checaColisionPersonaje(posXBacteria,posYBacteria)
            || checaColisionPersonaje(posXBebe,posYBebe)){
                char rutaSonido[200];
                sprintf(rutaSonido,"%s%s", fullPath.c_str() , "sonidos/golpe.wav");
                PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
                vidas--;
                reiniciarPosicionPersonajes();
            }
        // REVISAR COLISION DE ITEMS CON JUGADOR
        char rutaSonido[200];
        sprintf(rutaSonido,"%s%s", fullPath.c_str() , "sonidos/coin.wav");
        // item 1
        if (checaColisionPersonaje(posXIt1,posYIt1) && activoIt1){
            activoIt1 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }
        // item 2
        if (checaColisionPersonaje(posXIt2,posYIt2) && activoIt2){
            activoIt2 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }

        // item 3
        if (checaColisionPersonaje(posXIt3,posYIt3) && activoIt3){
            activoIt3 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }

        // item 4
        if (checaColisionPersonaje(posXIt4,posYIt4) && activoIt4){
            activoIt4 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }

        // item 5
        if (checaColisionPersonaje(posXIt5,posYIt5) && activoIt5){
            activoIt5 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }
        // item 6
        if (checaColisionPersonaje(posXIt6,posYIt6) && activoIt6){
            activoIt6 = false;
            itemsRecogidos++;
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
        }

        // PERDER SI VIDAS = 0
        if (vidas <=0){
            perdioJuego = true;
            reiniciarJuego();
        }
    }
    // REVISAR SI GANO O PERDIO
    if (segundos == 0){
        if (itemsRecogidos==totalItems){
            reiniciarJuego();
            char rutaSonido[200];
            sprintf(rutaSonido,"%s%s", fullPath.c_str() , "sonidos/tada.wav");
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
            ganoJuego=true;
        } else {
            char rutaSonido[200];
            sprintf(rutaSonido,"%s%s", fullPath.c_str() , "sonidos/golpe.wav");
            PlaySound(TEXT(rutaSonido), NULL, SND_FILENAME | SND_ASYNC);
            perdioJuego = true;
            reiniciarJuego();
        }
    }
    glutPostRedisplay();
    glutTimerFunc(250,myTimer,1);
}

//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image,int k){
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data

}

void initRendering(){
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_COLOR_MATERIAL);

    glGenTextures(36, texName); //Make room for our texture
    Image* image;

    char  ruta[200];

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Menu_256.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_MENU);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Instrucciones_256.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_INSTRUCCIONES);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/Creditos_256.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_CREDITOS);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/fondo.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_FONDO);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/bloque.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_BLOQUE);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/stats.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_STATS);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/consejo1.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_C1);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/consejo2.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_C2);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/consejo3.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_C3);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/consejo4.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_C4);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/ganar.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_GANAR);

    sprintf(ruta,"%s%s", fullPath.c_str() , "Texturas/perder.bmp");
    image = loadBMP(ruta);
    loadTexture(image,TEXTURE_PERDER);

    //personaje jugador
    string rutaObj = fullPath + "objects/persona2.obj";
    models[PLAYER_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[PLAYER_MOD]);
    glmVertexNormals(&models[PLAYER_MOD], 90.0, GL_TRUE);

    //personaje pildora
    rutaObj = fullPath + "objects/pildora.obj";
    models[PILDORA_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[PILDORA_MOD]);
    glmVertexNormals(&models[PILDORA_MOD], 90.0, GL_TRUE);

    //personaje bacteria
    rutaObj = fullPath + "objects/germen.obj";
    models[BACTERIA_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[BACTERIA_MOD]);
    glmVertexNormals(&models[BACTERIA_MOD], 90.0, GL_TRUE);

    //personaje bebe
    rutaObj = fullPath + "objects/bebe2.obj";
    models[BEBE_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[BEBE_MOD]);
    glmVertexNormals(&models[BEBE_MOD], 90.0, GL_TRUE);

    // item fruta
    rutaObj = fullPath + "objects/manzana.obj";
    models[FRUTA_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[FRUTA_MOD]);
    glmVertexNormals(&models[FRUTA_MOD], 90.0, GL_TRUE);
    // item verdura
    rutaObj = fullPath + "objects/zanahoria.obj";
    models[VERDURA_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[VERDURA_MOD]);
    glmVertexNormals(&models[VERDURA_MOD], 90.0, GL_TRUE);
    // item jabon
    rutaObj = fullPath + "objects/jabon.obj";
    models[JABON_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[JABON_MOD]);
    glmVertexNormals(&models[JABON_MOD], 90.0, GL_TRUE);
    // item anti
    rutaObj = fullPath + "objects/anticonceptivo.obj";
    models[ANTI_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[ANTI_MOD]);
    glmVertexNormals(&models[ANTI_MOD], 90.0, GL_TRUE);
    // item fruta2
    rutaObj = fullPath + "objects/naranja.obj";
    models[FRUTA2_MOD] = *glmReadOBJ(rutaObj.c_str());
    glmUnitize(&models[FRUTA2_MOD]);
    glmVertexNormals(&models[FRUTA2_MOD], 90.0, GL_TRUE);

    delete image;
}

void reshape(int width, int height){
  glViewport(0, 0, width, height);
  // reajustar variables de dimenciones
  windowWidth = width;
  windowHeight = height;
  bannerHeight = height/4.333; // proporcion
  statsSquare = width / 5;
  viewportGameWidth = width;
  viewportGameHeight = height - bannerHeight;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-orthosize, orthosize, -orthosize, orthosize, 0.1, orthosize);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void dibujaItems(){
    // ITEMS
    // item 1
    if (activoIt1){
        glPushMatrix();
        glTranslated(posXIt1, posYIt1, 0.2);
        glScaled(1.5, 1.5, 0.08);
        glRotated(angulo,1,0,0);
        glmDraw(&models[ANTI_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }
    // item 2
    if (activoIt2){
        glPushMatrix();
        glTranslated(posXIt2, posYIt2, 0.2);
        glScaled(1, 1, 0.05);
        glRotated(angulo,1,0,0);
        glmDraw(&models[FRUTA_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }

    // item 3
    if (activoIt3){
        glPushMatrix();
        glTranslated(posXIt3, posYIt3, 0.2);
        glScaled(2, 2, 0.08);
        glRotated(angulo,1,1,0);
        glmDraw(&models[FRUTA2_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }

     // item 4
    if (activoIt4){
        glPushMatrix();
        glTranslated(posXIt4, posYIt4, 0.2);
        glScaled(2, 2, 0.08);
        glRotated(angulo,1,0,0);
        glmDraw(&models[VERDURA_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }

    // item 5
    if (activoIt5){
        glPushMatrix();
        glTranslated(posXIt5, posYIt5, 0.2);
        glScaled(2, 2, 0.08);
        glRotated(angulo,1,1,0);
        glmDraw(&models[VERDURA_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }
    // item 6
    if (activoIt6){
        glPushMatrix();
        glTranslated(posXIt6, posYIt6, 0.2);
        glScaled(8, 8, 0.08);
        glRotated(angulo,1,0,0);
        glmDraw(&models[JABON_MOD],  GLM_COLOR|GLM_FLAT);
        glPopMatrix();
    }
}

void dibujaConsejo(){
    if(juegoPausado){
        if (consejoActual == 0){
            glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_C1]);
        } else if (consejoActual==1){
            glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_C2]);
        } else if(consejoActual==2){
            glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_C3]);
        } else {
            glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_C4]);
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-15, -10, 0.5);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(15, -10, 0.5);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(15, 10, 0.5);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-15, 10, 0.5);
        glEnd();
    }
}

void dibujaGanarPerder(){
    if (ganoJuego){
        // fondo
        glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_GANAR]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-20, -20, 0.5);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(20, -20, 0.5);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(20, 20, 0.5);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-20, 20, 0.5);
        glEnd();
    }
    if (perdioJuego) {
        // fondo
        glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_PERDER]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-20, -20, 0.5);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(20, -20, 0.5);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(20, 20, 0.5);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-20, 20, 0.5);
        glEnd();
    }
}

void dibujar_paredes(){
    glColor4ub(255, 255, 255,255);       // Color
    dibujaGanarPerder();
    // fondo
    glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_FONDO]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, -40, 0);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(40, -40, 0);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(40, 40, 0);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-40, 40, 0);
    glEnd();

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_BLOQUE]);
    //* Como se van a generar las coordenadas?
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    //* Activar la generación de coordenadas
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    //pared vertical
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(2,40,0.3);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //pared horizontal
    glPushMatrix();
    glScalef(40,2,0.3);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 1
    glPushMatrix();
    glTranslatef(-25,25,0.1);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 2
    glPushMatrix();
    glTranslated(25,25,0.1);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 3
    glPushMatrix();
    glTranslatef(25,-25,0.1);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 4
    glPushMatrix();
    glTranslatef(-25,-25,0.1);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    dibujaConsejo();
    dibujaItems();

    // JUGADOR
    glPushMatrix();
    glTranslated(posXJugador, posYJugador, 0.2);
    glScaled(3, 2, 0.08);
    glRotated(75,1,0,0);
    glRotated(-65,0,1,0);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR|GLM_FLAT);
    glPopMatrix();

    // PILDORA
    glPushMatrix();
    glTranslated(posXPildora, posYPildora, 0.2);
    glScaled(2, 2, 0.08);
    glRotated(25,1,0,0);
    glmDraw(&models[PILDORA_MOD],  GLM_COLOR|GLM_FLAT);
    glPopMatrix();

    // BACTERIA
    glPushMatrix();
    glTranslated(posXBacteria, posYBacteria, 0.2);
    glScaled(2, 2, 0.08);
    glRotated(angulo,1,0,0);
    glmDraw(&models[BACTERIA_MOD],  GLM_COLOR|GLM_FLAT);
    glPopMatrix();

    // BEBE
    glPushMatrix();
    glTranslated(posXBebe, posYBebe, 0.2);
    glScaled(2, 2, 0.08);
    glRotated(-50,1,0,0);
    glmDraw(&models[BEBE_MOD],  GLM_COLOR|GLM_FLAT);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0, 0.2);
    glScaled(0, 0, 0.08);
    //glRotated(25,1,0,0);
    glmDraw(&models[PILDORA_MOD],  GLM_TEXTURE);
    glPopMatrix();

    glPopMatrix();

}

void dibujar_banner() {
    glColor4ub(255, 255, 255,255);       // Color
    glBindTexture(GL_TEXTURE_2D, texName[bannerSeleccionado]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, -40, 0);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(40, -40, 0);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(40, 40, 0);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-40, 40, 0);
    glEnd();
}

void dibujar_stats() {
    glColor4ub(255, 255, 255,255);       // Color
    // desplegar las opciones de primera linea
    char opcionesArriba[10]="";
    sprintf(opcionesArriba,"%s","Vidas: ");
    // agregar cantidad de vidas al string
    char vidasString[3];
    itoa (vidas,vidasString,10);
    strcat(opcionesArriba, vidasString);

    glRasterPos2f(-30,15);
    for (int k=0; opcionesArriba[k]!='\0'; k++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opcionesArriba[k]);
    }

     // desplegar las opciones de segunda linea
    char opcionesMedio[10]="";
    sprintf(opcionesMedio,"%s","Items: ");
    // agregar cantidad de recogidos al string
    char recogidos[3];
    itoa (itemsRecogidos,recogidos,10);
    strcat(opcionesMedio, recogidos);
    strcat(opcionesMedio,"/6");

    glRasterPos2f(-30,-5);
    for (int k=0; opcionesMedio[k]!='\0'; k++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opcionesMedio[k]);
    }



    // desplegar las opciones de tercera linea
    char opcionesAbajo[10]="";
    sprintf(opcionesAbajo,"%s","Segs: ");
    // agregar cantidad de vidas al string
    char segundosString[3];
    itoa (segundos,segundosString,10);
    strcat(opcionesAbajo, segundosString);

    glRasterPos2f(-30,-25);
    for (int k=0; opcionesAbajo[k]!='\0'; k++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opcionesAbajo[k]);
    }

    // dibujar textura
    glBindTexture(GL_TEXTURE_2D, texName[TEXTURE_STATS]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, -40, 0);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(40, -40, 0);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(40, 40, 0);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-40, 40, 0);
    glEnd();
}

void display(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glViewport(0,bannerHeight,viewportGameWidth,viewportGameHeight);
  dibujar_paredes();

  glViewport(0,0,viewportGameWidth-statsSquare,bannerHeight);
  dibujar_banner();

  glViewport(viewportGameWidth-statsSquare,0,statsSquare,bannerHeight);
  dibujar_stats();

  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
  switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'c':
        case 'C':
            bannerSeleccionado = TEXTURE_CREDITOS;
            glutPostRedisplay();
            break;
        case 't':
        case 'T':
            bannerSeleccionado = TEXTURE_INSTRUCCIONES;
            glutPostRedisplay();
            break;
        case 'm':
        case 'M':
            bannerSeleccionado = TEXTURE_MENU;
            glutPostRedisplay();
            break;
        case 'i':
        case 'I':
            juegoIniciado = true;
            ganoJuego=false;
            perdioJuego=false;
            glutPostRedisplay();
            break;
        case 'r':
        case 'R':
            reiniciarJuego();
            juegoIniciado = true;
            ganoJuego=false;
            perdioJuego=false;
            glutPostRedisplay();
            break;
        case 'p':
        case 'P':
            if(juegoIniciado){
                if (!juegoPausado){
                    consejoActual++;
                    if (consejoActual>3){
                        consejoActual=0;
                    }
                }
                juegoPausado=!juegoPausado;
                glutPostRedisplay();
            }
            break;

    }
}

void mySpecialKeyboard (int key, int x, int y){
    if (juegoIniciado && !juegoPausado) {
        if (key == GLUT_KEY_UP && posYJugador < 30) {
            if(!checaColisionParedes(posXJugador, posYJugador+velocidadPersonaje)){
                posYJugador+= velocidadPersonaje;
                glutPostRedisplay();
            }
        } else if (key == GLUT_KEY_DOWN && posYJugador > -30){
            if (!checaColisionParedes(posXJugador, posYJugador-velocidadPersonaje)){
                posYJugador-=velocidadPersonaje;
                glutPostRedisplay();
            }

        } else if (key== GLUT_KEY_RIGHT && posXJugador < 30){
             if (!checaColisionParedes(posXJugador+velocidadPersonaje, posYJugador)){
                posXJugador+= velocidadPersonaje;
                glutPostRedisplay();
             }

        } else if (key==GLUT_KEY_LEFT && posXJugador > -30){
            if (!checaColisionParedes(posXJugador-velocidadPersonaje, posYJugador)){
                posXJugador-= velocidadPersonaje;
                glutPostRedisplay();
             }
        }
    }

}

int main(int argc, char **argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("Proyecto Final A01139764 A01036009");
  getParentPath();
  initRendering();
  glutTimerFunc(1000,myTimer,1);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(mySpecialKeyboard);
  glutMainLoop();
  return 0;
}
