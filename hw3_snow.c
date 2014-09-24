/*
 *  Lab 3: Crystal Cave
 *  
 *  Christopher Jordan w/code from William Schreuder via ex9
 *
 *  
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a crystal
 *     
 *     
 *     
 */
static void crystal(double x,double y,double z,
                 double dx,double dy,double dz,
                 double rx, double ry, double rz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,rx,ry,rz);
   glScaled(dx,dy,dz);
   
   //  Crystal
   glBegin(GL_TRIANGLE_FAN);

   //  Front Top
   glColor3f(0.0,0.0,1.0);
   glVertex3f(+0,+5,+0);
   glVertex3f(-1,+0,+1);
   glVertex3f(+1,+0,+1);

   //  Right Top
   glColor3f(0.0,0.0,0.9);
   glVertex3f(+0,+5,+0);
   glVertex3f(+1,+0,+1);
   glVertex3f(+1,+0,-1);

   //  Back Top
   glColor3f(0.0,0.0,1.0);
   glVertex3f(+0,+5,+0);
   glVertex3f(+1,+0,-1);
   glVertex3f(-1,+0,-1);

   //  Left Top
   glColor3f(0.0,0.0,0.9);
   glVertex3f(+0,+5,+0);
   glVertex3f(-1,+0,-1);
   glVertex3f(-1,+0,+1);

   //  Front Bottom
   glColor3f(0.0,0.0,0.9);
   glVertex3f(+0,-5,+0);
   glVertex3f(-1,+0,+1);
   glVertex3f(+1,+0,+1);

   //  Right Bottom
   glColor3f(0.0,0.0,0.8);
   glVertex3f(+0,-5,+0);
   glVertex3f(+1,+0,+1);
   glVertex3f(+1,+0,-1);

   //  Back Bottom
   glColor3f(0.0,0.0,0.9);
   glVertex3f(+0,-5,+0);
   glVertex3f(+1,+0,-1);
   glVertex3f(-1,+0,-1);

   //  Left Bottom
   glColor3f(0.0,0.0,0.8);
   glVertex3f(+0,-5,+0);
   glVertex3f(-1,+0,-1);
   glVertex3f(-1,+0,+1);

   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a snowflake
 *     at (x,y,z)
 *     dimentions (crystal(0,0,0, 1,1,1 , 0);dx,dy,dz)
 *     rotated th about the y axis
 */
 static void snowflake(double x,double y,double z,
   double dx, double dy, double dz, double r)
 {
   crystal(x,y,z, dx,dy,dz, r,r+1,r, 0);
   crystal(x,y,z, dx/1.5,dy/1.5,dz/1.5, r+1,r,r+1, 45);
   crystal(x,y,z, dx/1.5,dy/1.5,dz/1.5, r-1,r,r-1, 45);
   crystal(x,y,z, dx/1.5,dy/1.5,dz/1.5, r+1,r,r+1, 45);
   crystal(x,y,z, dx/1.5,dy/1.5,dz/1.5, r-1,r,r+1, 45);
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   int j,k,l;
   double i;

   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Crystals
   /*l = 0;
   for (i=-6.28; i<=6.28;){
      //for (j=-1;j<=1;j++)
         for (k=-5; k<=5; k++){
            crystal(i,sin(i),k, 0.2,0.2,0.2 , 0);
         }
      i += 0.5;
      l++;
   }*/

   //  Crystals
   /*for (i=-10; i<=10; i++)
      for (j=-1;j<=1;j++)
         for (k=-5; k<=5; k++)
            crystal(i,0,k, 0.2,0.2,0.2 , 0);*/  

   // Messing around
   snowflake(0,0,0, 0.6,0.6,0.6, 0);

   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Projections");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
