#include <GL/glut.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include "image.h"

#define PI 3.14

//Imena fajlova sa teksturama
#define FILENAME0 "pesak.bmp"
#define FILENAME1 "milica.bmp"

//Identifikatori tekstura
static GLuint names[2];

//niz koji cuva y koordinate pri skoku loptice
static float jump_positions[180];
static float speed = 0.45;

//ubacujem tip u strukturu prepreke
// 0 oznacava dijamant a 1 kocku
typedef struct{
  float x;
  float y;
  float z;
  int type;
}Obstacle;

Obstacle obstacles1[50];
Obstacle obstacles2[50];
static int pos1;
static int pos2;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_release(unsigned char key, int x, int y);

//indikatori za pocetak i kraj igre
static int start=0;
static int end=0;


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
static float r=1;
float rotate_object=0;

//duzina staze
static float lenght = 100;

static float width = 30;

static float z_coord_left_first = 50;
static float z_coord_right_first = 50;
static float z_coord_left_second = 150;
static float z_coord_right_second = 150;


static int possible_moves[]={0,0};


static void initialize(int argc, char** argv);
static void set_camera();
static void set_lights();


static void draw_plane();
static void draw_main_object();
static void draw_obstacles(int type);
static void move_objects();
static void set_obstacles(int type);
static void set_first();
static void resolve_collision();


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

  int k = 0;
  for (float i = 0; i <= 18; i += 0.1)
      jump_positions[k++] = 2 * sin(i);

  Image *image;
  //Ukljucujem teksture
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  //Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz fajla
  image = image_init(0,0);


  //Kreira se prva tekstura
  image_read(image, FILENAME0);

  //Generisu se identifikatori tekstura
  glGenTextures(2, names);

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

  //Iskljucujemo aktivnu teksturu
  glBindTexture(GL_TEXTURE_2D, 0);

  //Unistava se objekat za citanje tekstura iz fajla
  image_done(image);

  //Registruju se callback funkcije
  glutDisplayFunc(on_display);
  glutReshapeFunc(on_reshape);
  glutKeyboardFunc(on_keyboard);
  glutKeyboardUpFunc(on_release);

  srand(time(NULL));

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

  set_camera();
  set_lights();

  draw_plane();
  draw_main_object();
//funkcijom set_first() smo postavili prepreke na deo ravni koji
//se ne vidi pri pokretanju programa a kasnije se ostale prepreke
//dodaju u tajmeru
  if(!start){
    set_first();
    set_obstacles(2);
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

static void draw_main_object(){

  GLfloat material_ambient[] = {0.1225, 0.1275, 0.254, 1.0};
  GLfloat material_diffuse[] = {0.714, 0.1284, 0.18144, 1.0};
  GLfloat material_specular[] = {0.193548, 0.171906, 0.166721, 1.0};
  GLfloat shininess = 0.9;

  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  glPushMatrix();
  glTranslatef(x_coord, r, z_coord+2);
  glRotatef(rotate_object, 1, 0, 0);
  glutSolidSphere(r, 30, 30);
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

    z_plane1 -= speed;
    z_plane2 -= speed;
    z_coord_left_first -= speed;
    z_coord_left_second -= speed;
    z_coord_right_first -= speed;
    z_coord_right_second -= speed;

    for (int i = 0; i < pos1; i++)
        obstacles1[i].z -= speed;

    for (int i = 0; i < pos2; i++)
        obstacles2[i].z -= speed;

    if (possible_moves[0] && x_coord < 4.5)
        x_coord += 0.2;

    if (possible_moves[1] && x_coord > -4.5)
        x_coord -= 0.2;


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

    resolve_collision();

    glutPostRedisplay();
    if (start && !end)
        glutTimerFunc(50, move_objects, 0);
}

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

static void set_obstacles(int type) {

  if(type==1)
    pos1=0;
  else
   pos2=0;

  for(int i=0; i<=9;i++){
    int num = (int)rand() % 5;

    if( num==0)
       num=2;
    int diamond=0;
    int free_positions[]={0,0,0,0,0};

    for(int j=0; j<num; j++){
      Obstacle o;
      int positions[]={0,2,4,-2,-4};
      int pos = (int)rand() % 5 ;

      if(free_positions[pos]==0){
        free_positions[pos] = 1;
        int t = (int)rand()%2;
        if(t==0 && !diamond){
          o.type=0;
          o.y=0.5;
          diamond=1;
        }
        else{
          o.type=1;
          o.y=0.5;
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

static void set_first(){

  for(int i=0;i<2;i++){
    int num = (int)rand()%5;
    if(num<4)
      num++;
    int diamond=0;
    for(int j=0;j<num;j++){
      Obstacle o;
      int free_positions[] = {0, 0, 0, 0, 0};
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
          o.y=0.5;
        }
        o.x=positions[pos];
        o.z=z_plane1+50-i*20;
        obstacles1[pos1++]=o;
      }

    }
  }
}

static void resolve_collision()
{
    if (z_plane1 < z_plane2)
    {
        for (int i = 0; i < pos1; i++)
        {
            Obstacle o = obstacles1[i];
            float x = powf((o.x - x_coord), 2);
          //  float y = powf((o.y - y_coord), 2);
            float z = powf((o.z - z_coord - 1.5), 2);
            if (x < 4 && z < 4)
            {
                if (obstacles1[i].type == 0)
                {
                    score++;
                }
                else if (obstacles1[i].type == 1)
                {
                    start = 0;
                }
                else
                    start = 0;
            }
        }
    }
    else
    {
        for (int i = 0; i < pos2; i++)
        {
            Obstacle o = obstacles2[i];
            float x = powf((o.x - x_coord), 2);
          //  float y = powf((o.y - y_coord), 2);
            float z = powf((o.z - z_coord - 1.5), 2);
            if (x < 4 && z < 4)
            {
                if (obstacles2[i].type == 0){
                    score++;
                }
                else if (obstacles2[i].type == 1){
                    start = 0;
                }
                else
                    start = 0;
            }
        }
    }
}
