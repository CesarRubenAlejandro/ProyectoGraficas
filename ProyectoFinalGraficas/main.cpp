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
#include "imageloader.h"
#include "Image.h"

using namespace std;

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
int segundos = 60;

// variables de texturas
const int TEXTURE_COUNT=6;
const int TEXTURE_MENU = 0;
const int TEXTURE_INSTRUCCIONES = 1;
const int TEXTURE_CREDITOS = 2;
const int TEXTURE_FONDO = 3;
const int TEXTURE_BLOQUE = 4;
const int TEXTURE_STATS = 5;
static GLuint texName[TEXTURE_COUNT];
int bannerSeleccionado = TEXTURE_MENU;

void getParentPath()
{
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '\\'; i--) {
        fullPath.erase(i,1);
    }
}

//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image,int k)
{
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

void initRendering()
{
    glClearColor(0,0,.3,0);

    GLuint i=0;
    GLfloat ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat directedLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat directedLightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, directedLight);
    glLightfv(GL_LIGHT0, GL_POSITION, directedLightPos);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

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


void dibujar_paredes(){
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

    //ACTIVAR LA MATRIZ DE TEXTURAS
    //glMatrixMode(GL_TEXTURE);
    //glPushMatrix();

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
    glColor3ub(100,100,100);
    glScalef(2,40,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //pared horizontal
    glPushMatrix();
    glColor3ub(100,100,100);
    glScalef(40,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 1
    glPushMatrix();
    glColor3ub(100,100,100);
    glTranslatef(-25,25,0);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 2
    glPushMatrix();
    glColor3ub(100,100,100);
    glTranslated(25,25,0);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 3
    glPushMatrix();
    glColor3ub(100,100,100);
    glTranslatef(25,-25,0);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    //Muro 4
    glPushMatrix();
    glColor3ub(100,100,100);
    glTranslatef(-25,-25,0);
    glScalef(18,2,0.1);
    glutSolidCube(1);
    glPushMatrix();
    glLineWidth(2);
    glColor3ub(255,255,255);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    //glPopMatrix(); // POP a matriz de texturas


  glPopMatrix();
}

void dibujar_banner() {

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
     // desplegar las opciones de primera linea
    char opcionesArriba[10]="";
    sprintf(opcionesArriba,"%s","Vidas: ");
    // agregar cantidad de vidas al string
    char vidasString[3];
    itoa (vidas,vidasString,10);
    strcat(opcionesArriba, vidasString);

    glColor3ub(70,140,120);
    glRasterPos2f(-30,10);
    for (int k=0; opcionesArriba[k]!='\0'; k++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opcionesArriba[k]);
    }

    // desplegar las opciones de segunda linea
    char opcionesAbajo[10]="";
    sprintf(opcionesAbajo,"%s","Segs: ");
    // agregar cantidad de vidas al string
    char segundosString[3];
    itoa (segundos,segundosString,10);
    strcat(opcionesAbajo, segundosString);


    glRasterPos2f(-30,-10);
    for (int k=0; opcionesAbajo[k]!='\0'; k++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opcionesAbajo[k]);
    }

    // dibujar textura
    glColor3ub(255,255,255);
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

    //colorear fondo (rectangulo)
    //glColor3ub(200,120,0);
    //glRectd(-viewportGameWidth+statsSquare,-bannerHeight,viewportGameWidth+statsSquare,bannerHeight);
}

void display(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);


  glPushMatrix();
  glViewport(0,bannerHeight,viewportGameWidth,viewportGameHeight);
  dibujar_paredes();
  glPopMatrix();

  glPushMatrix();
  glViewport(0,0,viewportGameWidth-statsSquare,bannerHeight);
  dibujar_banner();
  glPopMatrix();

  glPushMatrix();
  glViewport(viewportGameWidth-statsSquare,0,statsSquare,bannerHeight);
  dibujar_stats();
  glPopMatrix();

  glutSwapBuffers();
}

void init(){
  glClearColor(0,0,.3,0);
  glEnable(GL_DEPTH_TEST);
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
  //init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
