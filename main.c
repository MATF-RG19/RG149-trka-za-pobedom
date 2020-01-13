#include <GL/glut.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>
#include<stdio.h>
#include "image.h"

#define PI 3.14

//Imena fajlova sa teksturama
#define FILENAME0 "pesak.bmp"
#define FILENAME1 "milica.bmp"
#define GAME_OVER "game_over.bmp"
#define START_GAME "start.bmp"
#define WINNER "winner.bmp"

//Identifikatori tekstura
static GLuint names[5];

//indikator da li su postavljene prepreke na prvu ravan jer se prilikom
//pokretanja igrice prepreke postavljaju samo na drugi deo ravni
static int first = 1;

//brzina kretanja
static float speed = 0.55;

//ubacujem tip u strukturu prepreke
// 0 oznacava dijamant a 1 kocku
//Struktura koja oznacava prepreku
typedef struct{
  float x;
  float y;
  float z;
  int type;
}Obstacle;
//Nizovi koji sadrze koordinate prepreka i njihov tip na svakoj ravni
Obstacle obstacles1[50];
Obstacle obstacles2[50];
static int pos1;
static int pos2;

//callback funkcije
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_release(unsigned char key, int x, int y);

//indikatori za pocetak i kraj igre
static int start=0;
static int end=0;

//brojac za ukupan broj poena koje je igrac postigao
static int score=0;

//dimenzije staze
static float x_plane1=10;
static float y_plane1=1;
static float z_plane1=50;
static float x_plane2=10;
static float y_plane2=1;
static float z_plane2=150;

//koordinate glavnog objekta
static float x_coord =0;
static float y_coord =0.75;
static float z_coord =5;
float rotate_object=0;

//duzina staze
static float lenght = 100;

//ugao za koji se ruke i noge pomeraju
static float angle = 0;

static float width = 30;
//z koordinate ravni levo i desno od glavne ravni
static float z_coord_left_first = 50;
static float z_coord_right_first = 50;
static float z_coord_left_second = 150;
static float z_coord_right_second = 150;

//pomocni niz za kreiranje igraca
static int possible_moves[]={0,0};

//Fnkcije za inicijalizaciju
static void initialize(int argc, char** argv);
static void set_camera();
static void set_lights();

//funkcije za postavljanje i iscrtavanje glavnog objekta i prepreka
static void draw_plane();
static void draw_main_object();
static void draw_obstacles(int type);
static void move_objects();
static void set_obstacles(int type);
static void set_first();

//funkcija koja se koristi u resavanju kolizije
static float distance(Obstacle o);

//funkcija koja proverava koliziju
static void resolve_collision();

//Funkcije koje postavljaju pocetni i krajnji ekran
static void press_start();
static void game_over();

//Funkcija koja predstavlja ekran ukoliko je igrac pobedio
static void winner();

int main(int argc, char **argv) {

   initialize(argc,argv);
   glutMainLoop();

  return 0;
}
static void initialize(int argc, char** argv){

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  //Kreira se  prozor
  glutInitWindowSize(600,600);
  glutInitWindowPosition(100,100);
  glutCreateWindow("Trka za pobedom");

  Image *image;
  //Ukljucujem teksture
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  //Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz fajla
  image = image_init(0,0);


  //Kreira se prva tekstura
  image_read(image, FILENAME0);

  //Generisu se identifikatori tekstura
  glGenTextures(5, names);

  glBindTexture(GL_TEXTURE_2D, names[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width , image->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

  //Kreira se druga tekstura
  image_read(image, FILENAME1);
  glBindTexture(GL_TEXTURE_2D, names[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

  //Kreira se tekstura za pocetak igrice
  image_read(image, START_GAME);

  glBindTexture(GL_TEXTURE_2D, names[2]);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               image->width, image->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

  //Kreira se tekstura za kraj igrice
  image_read(image, GAME_OVER);

    glBindTexture(GL_TEXTURE_2D, names[3]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    //Kreira se tekstura ako je igrac pobedio odnosno osvojio 100 poena
    image_read(image, WINNER);

    glBindTexture(GL_TEXTURE_2D, names[4]);
    glTexParameteri(GL_TEXTURE_2D,
                                   GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                                   GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                image->width, image->height, 0,
                GL_RGB, GL_UNSIGNED_BYTE, image->pixels);



  //Iskljucujemo aktivnu teksturu
  glBindTexture(GL_TEXTURE_2D, 0);

  //Unistava se objekat za citanje tekstura iz fajla
  image_done(image);

  //Registruju se callback funkcije
  glutDisplayFunc(press_start);
  glutReshapeFunc(on_reshape);
  glutKeyboardFunc(on_keyboard);
  glutKeyboardUpFunc(on_release);

  srand(time(NULL));

  glutFullScreen();

  //Inicijalizacija OpenGL-a
  glClearColor(1.0,0.5,0.0,0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

}
static void on_keyboard(unsigned char key, int x, int y){
  switch(key){
    case 27:
        //zavrsavamo igru
        exit(0);
         break;
    case 'S':
    case 's':
       //pocinjemo igru
       if(!start && !end){
         //podesava se pozadina i poziva on_display funkcija kao i
         //funkcija koja pomera objekte
         glClearColor(1.0,0.5,0.0,0);
         glutDisplayFunc(on_display);
         glutTimerFunc(50,move_objects,0);
         start=1;
       }
         break;
    case 'P':
    case 'p':
         //pauziramo igru
         start=0;
         break;
    case 'A':
    case 'a':
         //pomeranje ulevo
         possible_moves[0]=1;
         glutPostRedisplay();
         break;
    case 'D':
    case 'd':
         //pomeranje udesno
         possible_moves[1]=1;
         glutPostRedisplay();
         break;
  }
}

static void on_reshape(int width, int height){


  glViewport( 0,0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, width/(float)height,1,50);

}

static void on_release(unsigned char key, int x, int y){
  switch(key){
    case 'a':
    case 'A':
       possible_moves[0]-=1;
       break;
    case 'd':
    case 'D':
       possible_moves[1]-=1;
       break;
  }
}

static void on_display(void){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Postavljam kameru i svetlo
  set_camera();
  set_lights();

  //Prikaz trenutnog broja poena na ekranu
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(1,1,1);
  glRasterPos3f(x_coord+3, y_coord+5, 5);
  char score_display[10] = "SCORE: ";
  char score_value[10];
  sprintf(score_value, " %d/100", score);
  strcat(score_display, score_value);

  int len = (int)strlen(score_display);

  for(int i=0; i<len;i++){
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score_display[i]);
  }
  glPopMatrix();
  
  glEnable(GL_LIGHTING);

  //Iscrtavaju se ravni i glavni objekat
  draw_plane();
  draw_main_object();

  //crtamo prepreke
  draw_obstacles(1);
  draw_obstacles(2);

//funkcijom set_first() smo postavili prepreke na deo ravni koji
//se ne vidi pri pokretanju programa a kasnije se ostale prepreke
//dodaju u tajmeru
  if(first){
    set_first();
    set_obstacles(2);
    first = 0;
  }
  //crtamo prepreke
  draw_obstacles(1);
  draw_obstacles(2);

  glutSwapBuffers();
}

static void set_camera(){

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt( x_coord, y_coord+5, z_coord-5,
             x_coord, y_coord, z_coord + 7,
              0,1,0);
}


static void set_lights(){

    GLfloat light_position[] = {0,1,-0.3,0};
    GLfloat light_ambient[] = { 0.5,0.5,0.5,1};
    GLfloat light_diffuse[] = {0.9, 0.9,0.9,1};
    GLfloat light_specular[] = {1,1,1,1};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION, light_position);
    glLightfv(GL_LIGHT0,GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR, light_specular);
}
//Iscrtavamo coveka
static void draw_main_object(){

//  boja za glavu
  GLfloat material_diffuse1[] = {0.714, 0.1284, 0.18144, 1.0};
// boja za telo
  GLfloat material_diffuse2[] = {0.8, 0.8, 0.1, 1};
// boja za noge i ruke
  GLfloat material_diffuse3[] = {0.5, 0.8, 1, 1};

  //crtamo glavu
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse1);
  glPushMatrix();
  glTranslatef(x_coord, y_coord+1.5, z_coord);
  glutSolidSphere(0.3, 10, 10);
  glPopMatrix();

  //crtamo telo
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, material_diffuse2);
  glPushMatrix();
  glTranslatef(x_coord, y_coord + 0.9, z_coord);
  glScalef(0.6, 0.8,0.4);
  glutSolidCube(1);
  glPopMatrix();

  //desna noga
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse3);
  glPushMatrix();
  glTranslatef(x_coord+0.15, y_coord+0.3, z_coord);
  glScalef(0.5, 1.7,0.3);
  glPushMatrix();
  glRotatef(20 * sin(30 * angle * PI/180),1,0,0);
  glutSolidCube(0.5);
  glPopMatrix();
  glPopMatrix();

  //leva noga
  glPushMatrix();
  glTranslatef(x_coord-0.15, y_coord+0.3, z_coord);
  glScalef(0.5, 1.7,0.3);
  glPushMatrix();
  glRotatef(20 * sin(30 * angle * PI/180 + PI),1,0,0);
  glutSolidCube(0.5);
  glPopMatrix();
  glPopMatrix();

  //desna ruka
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse3);
  glPushMatrix();
  glTranslatef(x_coord+0.35, y_coord+1, z_coord);
  glRotatef(20, 0,0,1);
  glScalef(0.2, 0.5, 0.1);
  glPushMatrix();
  glRotatef(20 * sin(30 * angle * PI / 180 + PI), 1, 0, 0);
  glutSolidCube(1);
  glPopMatrix();
  glPopMatrix();

  //leva ruka
  glPushMatrix();
  glTranslatef(x_coord-0.35, y_coord+1, z_coord);
  glRotatef(-20, 0,0,1);
  glScalef(0.2, 0.5, 0.1);
  glPushMatrix();
  glRotatef(20 * sin(30 * angle * PI / 180), 1, 0, 0);
  glutSolidCube(1);
  glPopMatrix();
  glPopMatrix();

}

static void draw_plane(){

    //prva ravan
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,1,0);
    int first=0;
    int second=0;
    for(int i=0;i<=5; i++){

      glTexCoord2f(first, second);
      first = (first + 1) % 2;
      glVertex3f(-5, -0.2, z_plane1 - 50 + i * 20);

      glTexCoord2f(first, second);
      first = (first + 1) % 2;
      second = (second +1) % 2;
      glVertex3f(5, -0.2, z_plane1 - 50 + i * 20);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(0, -y_plane1/2, z_plane1);
    glScalef(x_plane1, y_plane1/2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    //druga ravan
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_plane2 - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(5, -0.2, z_plane2 - 50 + i * 20);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(0, -y_plane2 / 2, z_plane2);
    glScalef(x_plane2, y_plane2 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    //prva ravan desno od glavne
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
   first = 0;
   second = 0;
   for (int i = 0; i <= 5; i++)
   {
       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       glVertex3f(-5, -0.2, z_coord_right_first - 50 + i * 20);

       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       second = (second + 1) % 2;
       glVertex3f(-35, -0.2, z_coord_right_first - 50 + i * 20);
   }

   glEnd();

   glDisable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);
   glTranslatef(-20, -y_plane1 / 2, z_coord_right_first);
   glScalef(width, y_plane1 / 2, lenght);
   glutSolidCube(1);
   glPopMatrix();

   //druga ravan desno od glavne
   glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_coord_right_second - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(-35, -0.2, z_coord_right_second - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(-20, -y_plane1 / 2, z_coord_right_second);
    glScalef(width, y_plane1 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    //prva ravan levo od glavne
    glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, names[1]);
   glEnable(GL_TEXTURE_2D);

   glBegin(GL_QUAD_STRIP);
   glNormal3f(0, 1, 0);
   first = 0;
   second = 0;
   for (int i = 0; i <= 5; i++)
   {
       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       glVertex3f(5, -0.2, z_coord_left_first - 50 + i * 20);

       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       second = (second + 1) % 2;
       glVertex3f(35, -0.2, z_coord_left_first - 50 + i * 20);
   }

   glEnd();

   glDisable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);
   glTranslatef(20, -y_plane1 / 2, z_coord_left_first);
   glScalef(width, y_plane1 / 2, lenght);
   glutSolidCube(1);
   glPopMatrix();

   //druga ravan levo od glavne
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, names[1]);
   glEnable(GL_TEXTURE_2D);

   glBegin(GL_QUAD_STRIP);
   glNormal3f(0, 1, 0);
   first = 0;
   second = 0;
   for (int i = 0; i <= 5; i++)
   {
       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       glVertex3f(5, -0.2, z_coord_left_second - 50 + i * 20);

       glTexCoord2f(first, second);
       first = (first + 1) % 2;
       second = (second + 1) % 2;
       glVertex3f(35, -0.2, z_coord_left_second - 50 + i * 20);
   }

   glEnd();

   glDisable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);
   glTranslatef(20, -y_plane1 / 2, z_coord_left_second);
   glScalef(width, y_plane1 / 2, lenght);
   glutSolidCube(1);
   glPopMatrix();

}

static void move_objects(int value){
    if (value != 0)
        return;
    //pomeraju se z koordinate svih ravni
    z_plane1 -= speed;
    z_plane2 -= speed;
    z_coord_left_first -= speed;
    z_coord_left_second -= speed;
    z_coord_right_first -= speed;
    z_coord_right_second -= speed;

    //pomeraju se z koordinate prepreka
    for (int i = 0; i < pos1; i++)
        obstacles1[i].z -= speed;

    for (int i = 0; i < pos2; i++)
        obstacles2[i].z -= speed;
    // possible_moves[0] je indikator da je pritisnut taster za kretanje
    // ulevo i ako covek i dalje moze da se krece na tu stranu tj ako nije dosao do kraja
   // staze, onda se pomera, inace ostaje u mestu. Analogno za kretanje udesno
    if (possible_moves[0] && x_coord < 4.5)
        x_coord += 0.2;

    if (possible_moves[1] && x_coord > -4.5)
        x_coord -= 0.2;

    //uvecava se ugao rotacije
    angle += 1;
    if(angle > 360)
       angle=0;

    //Sredjujem bag koji se javlja a to je razmak izmedju 2 ravni.
   //Kada jedna izadje iz vidokruga kamere vracamo je na kraj druge ravni
   //i ponovo postavljamo prepreke
    if (z_plane1 + 50 <= 0){
        z_plane1 = 150;
        set_obstacles(1);
    }
    if (z_plane2 + 50 <= 0){
        z_plane2 = 150;
        set_obstacles(2);
    }

    if (z_coord_left_first + 50 <= 0){
        z_coord_left_first = 150;
    }
    if (z_coord_left_second + 50 <= 0){
        z_coord_left_second = 150;
    }

    if (z_coord_right_first + 50 <= 0){
        z_coord_right_first = 150;
    }
    if (z_coord_right_second + 50 <= 0){
        z_coord_right_second = 150;
    }

    // Kugla se rotira sve vreme
    rotate_object += 30;
    if (rotate_object >= 360)
        rotate_object += -360;

    //Svaki put proveravamo da li je doslo do kolizije
    resolve_collision();

    glutPostRedisplay();
    if (start && !end)
        glutTimerFunc(50, move_objects, 0);
}

//Iscrtavmao prepreke koje su postavljene u nizovima obstacles1
// i obstacles2 a type pokazuje na kojoj se ravni one iscrtavaju
static void draw_obstacles(int type){

  int i=0;
  if(type==1)
   i=pos1;
  else
   i=pos2;

  for(int j=0; j<i; j++){
    Obstacle o;

    if(type==1)
     o=obstacles1[j];
    else
     o=obstacles2[j];


     GLfloat material_ambient[] = {0.2125, 0.1275, 0.054, 1.0};
     GLfloat material_diffuse[] = {0.714, 0.4284, 0.18144, 1.0};
     GLfloat material_specular[] = {0.393548, 0.271906, 0.166721, 1.0};
     GLfloat shininess = 0.2;

     glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
     glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
     glMaterialf(GL_FRONT, GL_SHININESS, shininess);

     //Ako je tip prepreke 0 crtamo dijamant
     if( o.type==0){
     glPushMatrix();
     glTranslatef(o.x, o.y, o.z);
     glScalef(0.8,0.8,0.8);
     glutSolidIcosahedron();
     glPopMatrix();
     }
     //Ako je tip prepreke 1 crtamo kocku
     else if(o.type==1){
       glPushMatrix();
       glTranslatef(o.x,o.y,o.z);
       glScalef(2,2,2);
       glutSolidCube(1);
       glPopMatrix();
     }
   }
}
//funkcija koja postavlja koordinate prepreka
static void set_obstacles(int type) {

  if(type==1)
    pos1=0;
  else
   pos2=0;
  // u 10 redova na svakoj ravni se postavljaju prepreke
  // tako sto se uzima random broj od 0 do 4 i toliko prepreka se iscrtava
  // u svakom redu
  for(int i=0; i<=9;i++){
    int num = (int)rand() % 5;

    if( num==0)
       num=2;
    int diamond=0;
    //niz koji oznacava koje su pozicije u redu slobodne
    int free_positions[]={0,0,0,0,0};

    for(int j=0; j<num; j++){
      Obstacle o;
      // niz koji oznacava x koordinate pozicija koje odgovaraju slobodnim pozicijama
      // u nizu free_positions
      int positions[]={4,2,0,-2,-4};
      int pos = (int)rand() % 5;

      if(free_positions[pos]==0){
        free_positions[pos] = 1;
        // random broj koji odredjuje da li ce se crtati kocka ili dijamant
        int t = (int)rand()%2;
        if(t==0 && !diamond){
          o.type=0;
          o.y=0.5;
          diamond=1;
        }
        else{
          o.type=1;
          o.y=0.75;
        }
        o.x=positions[pos];

        if(type==1){
          o.z = z_plane1+50-i*10;
          obstacles1[pos1++] = o;
        }
        else{
          o.z = z_plane2+50-i*10;
          obstacles2[pos2++] = o;
        }
      }
    }

  }
}
// funkcija analogna funkciji set_obstacles
// samo sto se ona poziva samo na pocetku i postavlja prepreke samo
// na drugi deo ravni
static void set_first(){

  for(int i=0;i<=2;i++){
    int num = (int)rand()%5;
    if(num<4)
      num++;
    int diamond=0;
    int free_positions[] = {0, 0, 0, 0, 0};
    for(int j=0;j<num;j++){
      Obstacle o;
      int positions[] = {4, 2, 0, -2, -4};
      int pos = (int)rand() % 5;

      if(free_positions[pos] == 0){
        free_positions[pos]=1;
        int t=(int)rand()%2;
        if(t==0 && !diamond){
          o.type=0;
          o.y=0.5;
          diamond=1;
        }
        else{
          o.type=1;
          o.y=0.7;
        }
        o.x=positions[pos];
        o.z=z_plane1+50-i*20;
        obstacles1[pos1++]=o;
      }

    }
  }
}
// funkcija koja odredjuje razmak izmedju
// prepreka i coveka i koristi se u
// resavanju kolizije
static float distance(Obstacle o)
{
    float x = powf((o.x - x_coord), 2);
    float y = powf((o.y - y_coord), 2);
    float z = powf((o.z - z_coord), 2);

    return sqrt(x + y + z);
}

// u zavisnosti od toga na kojoj je ravni covek
// proverava se u svakom trenutku da li je doslo do kolizije sa
// preprekom ili dijamantom, tako sto se gleda razmak
// izmedju njih
static void resolve_collision(){
    if (z_plane1 < z_plane2){

        for (int i = 0; i < pos1; i++){
            Obstacle o = obstacles1[i];
            if (distance(o) < 1.22)
            {
                //ako je dijamant uvecava se broj poena za 5
                if (obstacles1[i].type == 0){
                    score+=5;
                    obstacles1[i].x = -100;
                }
                //ako je dostigao 100 poena, pobedio je,
                //zavrsava se igra
                if(score == 100){

                  start = 0;
                  glutDisplayFunc(winner);
                  glutPostRedisplay();
                }
                // ako je udario u kocku zavrsava se
                // igra i postavlja displej za kraj
                else if (obstacles1[i].type == 1){
                    start = 0;
                    glutDisplayFunc(game_over);
                    glutPostRedisplay();
                }
            }
        }
    }
    else{
        for (int i = 0; i < pos2; i++){
          Obstacle o = obstacles2[i];
          if (distance(o) < 1.22){

              if (obstacles2[i].type == 0){
                  score+=5;
                  obstacles2[i].x = -100;
              }
              if(score == 100){

                start = 0;
                glutDisplayFunc(winner);
                glutPostRedisplay();
              }
              else if (obstacles2[i].type == 1){
                  start = 0;
                  glutDisplayFunc(game_over);
                  glutPostRedisplay();
              }
            }
        }
    }
}

// funkcija koja postavlja teksture na pocetku igre
void press_start(void){
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,
              0, 0, 0,
              1, 0, 0);

    GLfloat light_position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0, 0);
    glVertex3f(-1, 0, -1);

    glTexCoord2f(0, 1);
    glVertex3f(1, 0, -1);

    glTexCoord2f(1, 1);
    glVertex3f(1, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(-1, 0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

// funkcija koja postavlja teksture za kraj igre
void game_over(void){
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,
              0, 0, 0,
              1, 0, 0);

    GLfloat light_position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0, 0);
    glVertex3f(-1, 0, -1);

    glTexCoord2f(0, 1);
    glVertex3f(1, 0, -1);

    glTexCoord2f(1, 1);
    glVertex3f(1, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(-1, 0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glColor3f(1,1,1);
    glRasterPos3f(0.6, 0.1, -0.2);
    char score_display[50] = "FINAL SCORE: ";
    char score_value[50];
    sprintf(score_value, " %d ", score);
    strcat(score_display, score_value);

    int len = (int)strlen(score_display);

    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score_display[i]);
    }
    
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

//funkcija koja postavlja teksture ako je igrac pobedio
void winner(void){
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 2, 0,
            0, 0, 0,
            1, 0, 0);

  GLfloat light_position[] = {0, 0, 0, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, names[4]);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);

  glTexCoord2f(0, 0);
  glVertex3f(-1, 0, -1);

  glTexCoord2f(0, 1);
  glVertex3f(1, 0, -1);

  glTexCoord2f(1, 1);
  glVertex3f(1, 0, 1);

  glTexCoord2f(1, 0);
  glVertex3f(-1, 0, 1);
  glEnd();

  glDisable(GL_TEXTURE_2D);

  glutSwapBuffers();

}
