#include <GL/glut.h>
#include<stdlib.h>
#include<time.h>

typedef struct{
  float x;
  float y;
  float z;
}Obstacle;

Obstacle obstacles1[50];
Obstacle obstacles2[50];
static int pos1;
static int pos2;

static int window_width, window_height;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);

static int start=0;
static int end=0;

static float x_plane1=10;
static float y_plane1=1;
static float z_plane1=50;
static float x_plane2=10;
static float y_plane2=1;
static float z_plane2=150;

static float x_coord =0;
static float y_coord =0;
static float z_coord =5;
static float r=1;
float rotate_object=0;

static float lenght = 100;



static void initialize(int argc, char** argv);
static void set_camera();
static void set_lights();


static void draw_plane();
static void draw_main_object();
static void draw_obstacles(int type);
static void move_objects();
static void set_obstacles(int type);



int main(int argc, char **argv) {

   initialize(argc,argv);
   glutMainLoop();

  return 0;
}
static void initialize(int argc, char** argv){

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowSize(600,600);
  glutInitWindowPosition(100,100);
  glutCreateWindow("Trka za pobedom");

  glutDisplayFunc(on_display);
  glutReshapeFunc(on_reshape);
  glutKeyboardFunc(on_keyboard);

  srand(time(NULL));

  glClearColor(0.7,0.2,0.2,0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

}
static void on_keyboard(unsigned char key, int x, int y){
  switch(key){
    case 27:
        exit(0);
         break;
    case 'S':
    case 's':
       if(!start && !end){
         glutTimerFunc(50,move_objects,0);
         start=1;
             }
         break;
    case 'P':
    case 'p':
         start=0;
         break;
    case 'A':
    case 'a':
         x_coord+=1;
         glutPostRedisplay();
         break;
    case 'D':
    case 'd':
         x_coord-=1;
         glutPostRedisplay();
         break;
  }
}

static void on_reshape(int width, int height){
  window_width = width;
  window_height = height;

  glViewport( 0,0, window_width, window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, window_width/(float)window_height,1,100);

}

static void on_display(void){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  set_camera();
  set_lights();

  draw_plane();
  draw_main_object();

  if(!start)
    set_obstacles(1);

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

    GLfloat material_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat material_diffuse[] = {0.1, 0.35, 0.9, 1.0};
    GLfloat material_specular[] = {0.45, 0.55, 0.45, 1.0};
    GLfloat material_emission[] = {0, 0, 0, 0};
    GLfloat shininess = 0.25;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glTranslatef(0, -y_plane1 / 2, z_plane1 );
    glScalef(x_plane1, -y_plane1 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();


    GLfloat material_ambient1[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat material_diffuse1[] = {0.1, 0.35, 0.1, 1.0};
    GLfloat material_specular1[] = {0.45, 0.55, 0.45, 1.0};
    GLfloat material_emission1[] = {0, 0, 0, 0};
    GLfloat shininess1 = 0.25;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_EMISSION, material_emission1);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess1);

    glPushMatrix();
    glTranslatef(0, -y_plane2 / 2, z_plane2 );
    glScalef(x_plane2, -y_plane2 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();
}

static void move_objects(int value){

  if(value != 0)
    return;

  z_plane1 -=0.5;
  z_plane2 -= 0.5;

  for (int i = 0; i < pos1; i++)
        obstacles1[i].z -= 0.5;

  for (int i = 0; i < pos2; i++)
        obstacles2[i].z -= 0.5;
  if(z_plane1+50 < 0){
    z_plane1 = 200;
    set_obstacles(1);
  }
  if(z_plane2+50<0){
    z_plane2 = 200;
    set_obstacles(2);
  }

  rotate_object +=30;
  if(rotate_object >= 360)
    rotate_object -=360;

  glutPostRedisplay();
  if(start && !end)
    glutTimerFunc(50, move_objects,0);
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

     glPushMatrix();
     glTranslatef(o.x, o.y, o.z);
     glScalef(2,2,2);
     glutSolidCube(1);
     glPopMatrix();
  }
}


static void set_obstacles(int type) {

  if(type==1)
    pos1=0;
  else
   pos2=0;

  for(int i=0; i<=9;i++){
    int num = (int)rand() % 5;
    for(int j=0; j<num; j++){
      Obstacle o;
      int free_positions[]={0,0,0,0,0};
      int positions[]={0,2,4,-2,-4};
      int pos = (int)rand() % 5 ;

      if(free_positions[pos]==0){
        o.x = positions[pos];
        o.y = 0.5;
        if(type == 1){
          o.z = z_plane1-100+i*20;
          obstacles1[pos1++] = o;
        }
        else{
          o.z = z_plane2-100+i*20;
          obstacles2[pos2++] = o;
        }
      }
    }

  }
}
