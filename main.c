#include <GL/glut.h>
#include<stdlib.h>
#include<time.h>

#define TIMER 0

static int window_width, window_height;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);

static int start,end;

static int timer_active;

static float x_plane1=3;
static float y_plane1=0;
static float z_plane1=10;

static float x_coord =3;
static float y_coord =1;
static float z_coord =0;

static int possible_moves[] = {0,0};

static void move_planes(int value);


int main(int argc, char **argv) {

      srand(time(NULL));

      glutInit(&argc, argv);
      glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

      glutInitWindowSize(600,600);
      glutInitWindowPosition(100,100);
      glutCreateWindow("Trka za pobedom");

      glutDisplayFunc(on_display);
      glutReshapeFunc(on_reshape);
      glutKeyboardFunc(on_keyboard);

      glClearColor(0.7,0.2,0.2,0);
      glEnable(GL_DEPTH_TEST);

      glLineWidth(1.5);

      glutMainLoop();

  return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
  switch(key){
    case 27:
        exit(0);
         break;
    case 'S':
    case 's':
         start=1;
         if(!end)
         {
             if(!timer_active){
               glutTimerFunc(50,move_planes,timer_active);
               timer_active=1;
             }
         };
         break;
    case 'P':
    case 'p':
         timer_active=0;
         break;
    case 'A':
    case 'a':
         start=1;
         possible_moves[0]=1;
         glutPostRedisplay();
         break;
    case 'D':
    case 'd':
         start=1;
         possible_moves[1]=1;
         glutPostRedisplay();
         break;
  }
}

static void on_reshape(int width, int height){
  window_width = width;
  window_height = height;
}

static void on_display(void){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport( 0,0, window_width, window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, window_width/(float)window_height,1,25);

  //Podesavavamo tacku pogleda
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt( x_coord, y_coord+5, z_coord-7,
             x_coord, y_coord, z_coord + 5,
              0.0, 1.0, 0.0);
  //Iscrtavamo glavnu ravan
  glPushMatrix();
  glBegin(GL_QUAD_STRIP);
  glNormal3f(0,1,0);
  for (int i = 0; i <= 35; i += 7)
  {
      glColor3f(0.2, 0.2, 0.2);
      glVertex3f(0, 0.51, z_plane1 - 15 + i);
      glVertex3f(6, 0.51, z_plane1 - 15 + i);
  }
  glEnd();


  glTranslatef(x_plane1,y_plane1,z_plane1);
  glScalef(6,1,30);
  glutSolidCube(1);
  glPopMatrix();


  glPushMatrix();
  glTranslatef(x_coord,y_coord + 1.5, z_coord - 3.5);
  glColor3f(0,0,1);
  glutSolidSphere(0.2,20,20);
  glPopMatrix();

  glutSwapBuffers();
}

static void move_planes(int value){

  if(value)
    return;
  if(possible_moves[0] && x_coord < 5.5){
    x_coord += 0.1;
  }
  if(possible_moves[1] && x_coord > 1.5){
    x_coord -= 0.1;
  }
  z_plane1 -= 0.25;

  glutPostRedisplay();
  if(timer_active){
    glutTimerFunc(30,move_planes,0);
  }
}
