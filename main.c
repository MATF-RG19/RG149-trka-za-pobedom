#include <GL/glut.h>
#include<stdlib.h>

static int window_width, window_height;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);

int main(int argc, char **argv) {

      glutInit(&argc, argv);
      glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

      glutInitWindowSize(600,600);
      glutInitWindowPosition(100,100);
      glutCreateWindow("Trka za pobedom");

      glutDisplayFunc(on_display);
      glutReshapeFunc(on_reshape);
      glutKeyboardFunc(on_keyboard);

      glClearColor(0.7,0.2,0.2,0);

      glLineWidth(1.5);

      glutMainLoop();

  return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
  switch(key){
    case 97:
     exit(0);
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
  gluLookAt( 2,2,2,
             0,0,0,
             0,1,0 );

glBegin(GL_LINES);
  //X-osa
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(7,0,0);

  //Y-osa
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(0,7,0);

  //Z-osa
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(0,0,7);
glEnd();

 glutSwapBuffers();
}
