#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <vector>



//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Yi-Chan Huang

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char* WINDOWTITLE = { "Solar System With Movable Astronaut" };
const char* GLUITITLE = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE = { true };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE        0x1b
#define MS_PER_CYCLE    100000
float Time;


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };

// size of the 3d box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

// which projection:

enum Projections
{
    ORTHO,
    PERSP
};


// which button:

enum ButtonVals
{
    RESET,
    QUIT
};

enum Views
{
    INSIDE,
    OUTSIDE
};

enum Modes
{
    GAMMING,
    VIEWING
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH = { 3. };


// the color numbers:
// this order must match the radio button order


enum Colors
{
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    MAGENTA,
    WHITE,
    BLACK
};

char* ColorNames[] =
{
    "Red",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Magenta",
    "White",
    "Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
    { 1., 0., 0. },        // red
    { 1., 1., 0. },        // yellow
    { 0., 1., 0. },        // green
    { 0., 1., 1. },        // cyan
    { 0., 0., 1. },        // blue
    { 1., 0., 1. },        // magenta
    { 1., 1., 1. },        // white
    { 0., 0., 0. },        // black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int        MainWindow;                // window id for main graphics window
float    Scale;                    // scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int        WhichColor;                // index into Colors[ ]
int        WhichProjection;        // ORTHO or PERSP
int        Xmouse, Ymouse;            // mouse values
float    Xrot, Yrot;


#define PLANETNUM    8
#define STARNUM 500

GLuint Planet[PLANETNUM];
GLuint Sun;
float orbit[PLANETNUM] = {8.25f, 11.5f, 15.f, 25.f, 40.f, 60.f, 85.f, 115.f};
float p_size[PLANETNUM] = { 0.4, 1, 2, 1.5, 5 , 4, 3, 3};

GLuint MissionShip;
GLuint BrokenShip;

float starpos[STARNUM][3];

float bx, by, bz;

GLuint SunTex;
GLuint PlanetTex[PLANETNUM];

int glms; //global time
int View;
int Mode;
bool LIGHT_0_On = true;
bool LIGHT_1_On = true;
bool FREEZE = false;
bool tt = false;


// function prototypes:

void	Animate();
void	Display();
void    DoViewMenu(int);
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoShadowMenu();
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void            Axes(float);
void            HsvRgb(float[3], float[3]);
unsigned char*  BmpToTexture(char*, int*, int*);
int             LoadObjFile(char* name);
int				ReadInt(FILE*);
short			ReadShort(FILE*);
float			Dot(float[3], float[3]);
void            OsuSphere(float radius, int slices, int stacks);


// utility to create an array from 3 separate values:

// draw the complete scene:
float Grey[] = { 0.3, 0.3, 0.3, 1. };
float White[] = { 1.,1.,1.,1. };
float Red[] = { 1.,0.,0.,1. };
float Green[] = { 0.,1.,0.,1. };
float Blue[] = { 0.,0.,1.,1. };
float Yellow[] = { 0.,1.,1.,1. };

struct Camera
{
    float ex, ey, ez;
    float cx, cy, cz;
    float vx, vy, vz;
    float ux, uy, uz;
    float kx, ky, kz;
};

Camera cam;

int random_gen(int range_bottom, int range_top)
{
    int number;
    int temp;
    temp = range_top - range_bottom;
    number = rand() % temp + range_bottom;
    return number;
}

float cude_cal(float num)
{
    float res = num * num * num;
    return res;
}

void rotate_cam(float & vx, float & vy, float & vz, float ax, float ay, float az, float angle) {
    float ca = cos(angle);
    float sa = sin(angle);
    float crossx = -vy * az + vz * ay;
    float crossy = -vz * ax + vx * az;
    float crossz = -vx * ay + vy * ax;
    float dot = ax * vx + ay * vy + az * vz;
    float rx = vx * ca + crossx * sa + dot * ax * (1 - ca);
    float ry = vy * ca + crossy * sa + dot * ay * (1 - ca);
    float rz = vz * ca + crossz * sa + dot * az * (1 - ca);
    vx = rx;
    vy = ry;
    vz = rz;
}

void normalize_vec(float& vx, float& vy, float& vz)
{
    int length = sqrt(vx * vx + vy * vy + vz * vz);
    vx = vx / length;
    vy = vy / length;
    vz = vz / length;
}

float* Array3(float a, float b, float c)
{
    static float array[4];
    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = 1.;
    return array;
}
// utility to create an array from a multiplier and an array:
float* Array4(float r, float g, float b, float a)
{
    static float array[4];
    array[0] = r;
    array[1] = g;
    array[2] = b;
    array[3] = a;
    return array;
}


float* MulArray3(float factor, float array0[3])
{
    static float array[4];
    array[0] = factor * array0[0];
    array[1] = factor * array0[1];
    array[2] = factor * array0[2];
    array[3] = 1.;
    return array;
}

void SetMaterial(float r, float g, float b, float shininess)
{
    glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
    glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
    glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
    glMaterialf(GL_BACK, GL_SHININESS, 2.f);
    glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
    glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
    glLightfv(ilight, GL_POSITION, Array3(x, y, z));
    glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
    glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
    glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
    glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
    glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
    glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
    glEnable(ilight);
}

void SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
    glLightfv(ilight, GL_POSITION, Array3(x, y, z));
    glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
    glLightf(ilight, GL_SPOT_EXPONENT, 1.);
    glLightf(ilight, GL_SPOT_CUTOFF, 30.);
    glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
    glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
    glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
    glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
    glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
    glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
    glEnable(ilight);
}

void SetParallelLight(int ilight, float x, float y, float z, float r, float g, float b)
{
    glLightfv(ilight, GL_POSITION, Array4(x, y, z, 0.));
    glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
    glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
    glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
    glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
    glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
    glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
    glEnable(ilight);
}

void DrawCircle(float r)
{
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; i++)
    {
        glVertex3f(r * cos((2 * M_PI * i) / 100), 0., r * sin((2 * M_PI * i) / 100));
    }
    glEnd();
}



// main program:

int
main(int argc, char* argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)

    glutInit(&argc, argv);


    // setup all the graphics stuff:

    InitGraphics();


    // create the display structures that will not change:

    InitLists();


    // init all the global variables used by Display( ):
    // this will also post a redisplay

    Reset();


    // setup all the user interface stuff:

    InitMenus();

    // draw the scene once and wait for some interaction:
    // (this will never return)

    glutSetWindow(MainWindow);
    glutMainLoop();

    // this is here to make the compiler happy:

    return 0;
}




// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate()
{
    // put animation stuff in here -- change some global variables
    // for Display( ) to find:

    // force a call to Display( ) next time it is convenient:

    int ms = glutGet(GLUT_ELAPSED_TIME);
    ms %= MS_PER_CYCLE;
    Time = (float)ms / (float)(MS_PER_CYCLE - 1);
    glms = ms;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// draw the complete scene:

int ms, s, min;
char str[20];
bool mission = false;
float timelimit = 300000;
 

void
Display()
{

    if (DebugOn != 0)
    {
        fprintf(stderr, "Display\n");
    }


    // set which window we want to do the graphics into:

    glutSetWindow(MainWindow);


    // erase the background:

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
    if (DepthBufferOn == 0)
        glDisable(GL_DEPTH_TEST);
#endif


    // specify shading to be flat:

    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);

    // set the viewport to a square centered in the window:

    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy;            // minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);


    // set the viewing volume:
    // remember that the Z clipping  values are actually
    // given as DISTANCES IN FRONT OF THE EYE
    // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO)
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    else
        gluPerspective(90., 1., 0.1, 1000.);


    // place the objects into the scene:

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (View == OUTSIDE) {

        gluLookAt(0, 8, 45, 0, 0, 0, 0.15, 1, 0);

        // rotate the scene:

        glRotatef((GLfloat)Yrot, 0., 1., 0.);
        glRotatef((GLfloat)Xrot, 1., 0., 0.);

        // uniformly scale the scene:

        if (Scale < MINSCALE)
            Scale = MINSCALE;
        glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

    }
    else {

        // set the eye position, look-at position, and up-vector:
        gluLookAt(cam.ex, cam.ey, cam.ez, cam.cx, cam.cy, cam.cz, cam.ux, cam.uy, cam.uz);
    }

    // set the fog parameters:

    if (DepthCueOn != 0)
    {
        glFogi(GL_FOG_MODE, FOGMODE);
        glFogfv(GL_FOG_COLOR, FOGCOLOR);
        glFogf(GL_FOG_DENSITY, FOGDENSITY);
        glFogf(GL_FOG_START, FOGSTART);
        glFogf(GL_FOG_END, FOGEND);
        glEnable(GL_FOG);
    }
    else
    {
        glDisable(GL_FOG);
    }


    // possibly draw the axes:

    if (AxesOn != 0)
    {
        glColor3fv(&Colors[WhichColor][0]);
        glCallList(AxesList);
    }


    // since we are using glScalef( ), be sure normals get unitized:

    glEnable(GL_NORMALIZE);

    // sun
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.2, White));
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    SetPointLight(GL_LIGHT0, 0., 0., 0., 1., 1., 1.);
    glEnable(GL_LIGHT0);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glRotatef(360 * Time, 0., 1., 0.);
    glCallList(Sun);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // other planets
    for (int i = 0; i < PLANETNUM; ++i)
    {
        glEnable(GL_LIGHTING);
        glPushMatrix();
        glRotatef(360 * Time * sqrt(cude_cal(38.)) / sqrt(cude_cal(orbit[i])), 0., 1., 0.);
        glRotated(orbit[i], 0, 1, 0);
        glTranslatef(orbit[i], 0., 0.);
        glRotatef(360 * 20 * i / 10 * Time, 0., 1., 0.);
        SetMaterial(1.0, i*0.2, 0., 2.);
        glCallList(Planet[i]);
        glPopMatrix();
        glDisable(GL_LIGHTING);
        
    }
    
    //background
    
    for (int i = 0; i < STARNUM*2; ++i)
    {
        glPushMatrix();
        glTranslatef(starpos[i][0], starpos[i][1], starpos[i][2]);
        glutSolidSphere(0.05, 50, 50);
        glPopMatrix();
    }
  
    // the planet orbit
    
    for (int i = 0; i < PLANETNUM; ++i)
    {
        DrawCircle(orbit[i]);
    }
    
    //
 
   

    glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(cam.ex - cam.vx * 0.1, cam.ey - cam.vy * 0.1, cam.ez - cam.vz * 0.1);
    
    glCallList(MissionShip);
    glDisable(GL_LIGHTING);
    glPopMatrix();
    glEndList();


    // draw some gratuitous text that is fixed on the screen:
    //
    // the projection matrix is reset to define a scene whose
    // world coordinate system goes from 0-100 in each axis
    //
    // this is called "percent units", and is just a convenience
    //
    // the modelview matrix is reset to identity as we don't
    // want to transform these coordinates
    

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 100., 0., 100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1., 1., 1.);
    DoRasterString(5., 5., 0., (char*)"Final Project -- Huang Yi Chan");

    //
    

   



    // swap the double-buffered framebuffers:

    glutSwapBuffers();


    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !

    glFlush();
}


void DoAxesMenu(int id)
{
    AxesOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoViewMenu(int id)
{
    View = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoModeMenu(int id)
{
    Mode = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void
DoColorMenu(int id)
{
    WhichColor = id - RED;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
    DebugOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
    DepthBufferOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void
DoDepthFightingMenu(int id)
{
    DepthFightingOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void
DoDepthMenu(int id)
{
    DepthCueOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}



// main menu callback:

void
DoMainMenu(int id)
{
    switch (id)
    {
    case RESET:
        Reset();
        break;

    case QUIT:
        // gracefully close out the graphics:
        // gracefully close the graphics window:
        // gracefully exit the program:
        glutSetWindow(MainWindow);
        glFinish();
        glutDestroyWindow(MainWindow);
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
    WhichProjection = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void
DoShadowsMenu(int id)
{
    ShadowsOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

    char c;            // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c;            // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
    // get # of milliseconds since the start of the program:

    int ms = glutGet(GLUT_ELAPSED_TIME);

    // convert it to seconds:

    return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
    glutSetWindow(MainWindow);

    int numColors = sizeof(Colors) / (3 * sizeof(int));
    int colormenu = glutCreateMenu(DoColorMenu);
    for (int i = 0; i < numColors; i++)
    {
        glutAddMenuEntry(ColorNames[i], i);
    }

    int axesmenu = glutCreateMenu(DoAxesMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthcuemenu = glutCreateMenu(DoDepthMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int viewmenu = glutCreateMenu(DoViewMenu);
    glutAddMenuEntry("Astronaut View", INSIDE);
    glutAddMenuEntry("Outside View", OUTSIDE);

    int modemenu = glutCreateMenu(DoModeMenu);
    glutAddMenuEntry("View", VIEWING);
    glutAddMenuEntry("Game", GAMMING);

    int debugmenu = glutCreateMenu(DoDebugMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int projmenu = glutCreateMenu(DoProjectMenu);
    glutAddMenuEntry("Orthographic", ORTHO);
    glutAddMenuEntry("Perspective", PERSP);

    int shadowsmenu = glutCreateMenu(DoShadowsMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);


    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Axes", axesmenu);
    glutAddSubMenu("Projection", projmenu);
    glutAddSubMenu("View", viewmenu);
    glutAddMenuEntry("Reset", RESET);
    glutAddSubMenu("Debug", debugmenu);
    glutAddMenuEntry("Quit", QUIT);

    // attach the pop-up menu to the right mouse button:

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//    also setup display lists and callback functions

void
InitGraphics()
{
    // request the display modes:
    // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // set the initial window configuration:

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

    // open the window and set its title:

    MainWindow = glutCreateWindow(WINDOWTITLE);
    glutSetWindowTitle(WINDOWTITLE);

    // set the framebuffer clear values:

    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

    // setup the callback functions:
    // DisplayFunc -- redraw the window
    // ReshapeFunc -- handle the user resizing the window
    // KeyboardFunc -- handle a keyboard input
    // MouseFunc -- handle the mouse button going down or up
    // MotionFunc -- handle the mouse moving with a button down
    // PassiveMotionFunc -- handle the mouse moving with a button up
    // VisibilityFunc -- handle a change in window visibility
    // EntryFunc    -- handle the cursor entering or leaving the window
    // SpecialFunc -- handle special keys on the keyboard
    // SpaceballMotionFunc -- handle spaceball translation
    // SpaceballRotateFunc -- handle spaceball rotation
    // SpaceballButtonFunc -- handle spaceball button hits
    // ButtonBoxFunc -- handle button box hits
    // DialsFunc -- handle dial rotations
    // TabletMotionFunc -- handle digitizing tablet motion
    // TabletButtonFunc -- handle digitizing tablet button hits
    // MenuStateFunc -- declare when a pop-up menu is in use
    // TimerFunc -- trigger something to happen a certain time from now
    // IdleFunc -- what to do when nothing else is going on

    glutSetWindow(MainWindow);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(NULL);
    glutVisibilityFunc(Visibility);
    glutEntryFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpaceballMotionFunc(NULL);
    glutSpaceballRotateFunc(NULL);
    glutSpaceballButtonFunc(NULL);
    glutButtonBoxFunc(NULL);
    glutDialsFunc(NULL);
    glutTabletMotionFunc(NULL);
    glutTabletButtonFunc(NULL);
    glutMenuStateFunc(NULL);
    glutTimerFunc(-1, NULL, 0);
    glutIdleFunc(Animate);

    unsigned char* Texture;
    int Width, Height;

    char bmp[PLANETNUM][20];
    strcpy(bmp[0], "0.bmp");
    strcpy(bmp[1], "1.bmp");
    strcpy(bmp[2], "2.bmp");
    strcpy(bmp[3], "3.bmp");
    strcpy(bmp[4], "4.bmp");
    strcpy(bmp[5], "5.bmp");
    strcpy(bmp[6], "6.bmp");
    strcpy(bmp[7], "7.bmp");

    Texture = BmpToTexture("sun.bmp", &Width, &Height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &SunTex);
    glBindTexture(GL_TEXTURE_2D, SunTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

    for (int i = 0; i < PLANETNUM; ++i)
    {
        Texture = BmpToTexture(bmp[i], &Width, &Height);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &PlanetTex[i]);
        glBindTexture(GL_TEXTURE_2D, PlanetTex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
    }

    //init starpos
    for (int i = 0; i < STARNUM; ++i)
    {
        starpos[i][0] = random_gen(-50, 50);
        starpos[i][1] = random_gen(-50, 50);
        starpos[i][2] = random_gen(-50, 50);
        
    }
    

    //init camera
    cam.ex = 40;
    cam.ey = 8;
    cam.ez = 0;
    cam.cx = 0;
    cam.cy = 0;
    cam.cz = 0;
    cam.vx = cam.cx - cam.ex;
    cam.vy = cam.cy - cam.ey;
    cam.vz = cam.cz - cam.ez;
    cam.ux = 0;
    cam.uy = 1;
    cam.uz = 0;
    bx = -20;
    by = 0;
    bz = 0;

    // init glew (a window must be open to do this):

#ifdef WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "glewInit Error\n");
    }
    else
        fprintf(stderr, "GLEW initialized OK\n");
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
    glutSetWindow(MainWindow);

    // The Sun
    Sun = glGenLists(1);
    glNewList(Sun, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, SunTex);
    OsuSphere(7., 100, 100);
    glDisable(GL_TEXTURE_2D);
    glEndList();
    
    // Planets
    for (int i = 0; i < PLANETNUM; ++i)
    {
        Planet[i] = glGenLists(1);
        glNewList(Planet[i], GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, PlanetTex[i]);
        OsuSphere(p_size[i], 100, 100);
        glDisable(GL_TEXTURE_2D);
        glEndList();
    }


    //Astronaut
    MissionShip = glGenLists(1);
    glNewList(MissionShip, GL_COMPILE);
    glEnable(GL_LIGHTING);
    glPushMatrix();
    SetMaterial(0.75, 0.2, 1, 2);
    glRotatef(270, 0, 1, 0);
    glScalef(0.01, 0.01, 0.01);
    LoadObjFile("amongus.obj");
    glDisable(GL_LIGHTING);
    glPopMatrix();
    glEndList();

    // create the axes:
    

    AxesList = glGenLists(1);
    glNewList(AxesList, GL_COMPILE);
    glLineWidth(AXES_WIDTH);
    Axes(1.5);
    glLineWidth(1.);
    glEndList();
}




// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

    switch (c)
    {
    case 'o':
    case 'O':
        WhichProjection = ORTHO;
        break;

    case 'p':
    case 'P':
        WhichProjection = PERSP;
        break;

    case 'q':
    case 'Q':
    case ESCAPE:
        DoMainMenu(QUIT);    // will not return here
        break;                // happy compiler
    case 'f':
    case 'F':
        FREEZE = !FREEZE;
        if (FREEZE)
            glutIdleFunc(NULL);
        else
            glutIdleFunc(Animate);
        break;

     // turn on the light or not
    case '1':
        LIGHT_1_On = !LIGHT_1_On;
        break;

    //direction control  
    case 'n':
        cam.ex += cam.vx * 0.005;
        cam.ey += cam.vy * 0.005;
        cam.ez += cam.vz * 0.005;
        cam.cx += cam.vx * 0.005;
        cam.cy += cam.vy * 0.005;
        cam.cz += cam.vz * 0.005;
        break;
    case 'm':
        
        cam.ex -= cam.vx * 0.005;
        cam.ey -= cam.vy * 0.005;
        cam.ez -= cam.vz * 0.005;
        cam.cx -= cam.vx * 0.005;
        cam.cy -= cam.vy * 0.005;
        cam.cz -= cam.vz * 0.005;
        break;
    case 'a':
        rotate_cam(cam.vx, cam.vy, cam.vz, cam.ux, cam.uy, cam.uz, 0.01);
        cam.cx = cam.ex + cam.vx;
        cam.cy = cam.ey + cam.vy;
        cam.cz = cam.ez + cam.vz;
        break;
    case 'd':
        rotate_cam(cam.vx, cam.vy, cam.vz, cam.ux, cam.uy, cam.uz, -0.01);
        cam.cx = cam.ex + cam.vx;
        cam.cy = cam.ey + cam.vy;
        cam.cz = cam.ez + cam.vz;
        break;
    case 'w':
        cam.kx = cam.uy * cam.vz - cam.uz * cam.vy;
        cam.ky = cam.uz * cam.vx - cam.ux * cam.vz;
        cam.kz = cam.ux * cam.vy - cam.uy * cam.vx;
        normalize_vec(cam.kx, cam.ky, cam.kz);
        rotate_cam(cam.ux, cam.uy, cam.uz, cam.kx, cam.ky, cam.kz, -0.01);
        rotate_cam(cam.vx, cam.vy, cam.vz, cam.kx, cam.ky, cam.kz, -0.01);
        normalize_vec(cam.ux, cam.uy, cam.uz);
        cam.cx = cam.ex + cam.vx;
        cam.cy = cam.ey + cam.vy;
        cam.cz = cam.ez + cam.vz;
        break;
    case 's':
        cam.kx = cam.uy * cam.vz - cam.uz * cam.vy;
        cam.ky = cam.uz * cam.vx - cam.ux * cam.vz;
        cam.kz = cam.ux * cam.vy - cam.uy * cam.vx;
        normalize_vec(cam.kx, cam.ky, cam.kz);
        rotate_cam(cam.ux, cam.uy, cam.uz, cam.kx, cam.ky, cam.kz, 0.01);
        rotate_cam(cam.vx, cam.vy, cam.vz, cam.kx, cam.ky, cam.kz, 0.01);
        normalize_vec(cam.ux, cam.uy, cam.uz);
        cam.cx = cam.ex + cam.vx;
        cam.cy = cam.ey + cam.vy;
        cam.cz = cam.ez + cam.vz;
        break;

    default:
        fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
    }

    // force a call to Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
    int b = 0;            // LEFT, MIDDLE, or RIGHT

    if (DebugOn != 0)
        fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


    // get the proper button bit mask:

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        b = LEFT;        break;

    case GLUT_MIDDLE_BUTTON:
        b = MIDDLE;        break;

    case GLUT_RIGHT_BUTTON:
        b = RIGHT;        break;

    case SCROLL_WHEEL_UP:
        Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        break;

    case SCROLL_WHEEL_DOWN:
        Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        break;

    default:
        b = 0;
        fprintf(stderr, "Unknown mouse button: %d\n", button);
    }


    // button down sets the bit, up clears the bit:

    if (state == GLUT_DOWN)
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b;        // set the proper bit
    }
    else
    {
        ActiveButton &= ~b;        // clear the proper bit
    }
}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "MouseMotion: %d, %d\n", x, y);


    int dx = x - Xmouse;        // change in mouse coords
    int dy = y - Ymouse;

    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
    }


    if ((ActiveButton & MIDDLE) != 0)
    {
        Scale += SCLFACT * (float)(dx - dy);

        // keep object from turning inside-out or disappearing:

        if (Scale < MINSCALE)
            Scale = MINSCALE;
    }

    Xmouse = x;            // new current position
    Ymouse = y;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
    ActiveButton = 0;
    AxesOn = 0;
    View = 1;
    Mode = 0;
    DebugOn = 0;
    DepthCueOn = 0;
    Scale = 1.0;
    WhichColor = WHITE;
    WhichProjection = PERSP;
    Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
    if (DebugOn != 0)
        fprintf(stderr, "ReSize: %d, %d\n", width, height);

    // don't really need to do anything since window size is
    // checked each time in Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
    if (DebugOn != 0)
        fprintf(stderr, "Visibility: %d\n", state);

    if (state == GLUT_VISIBLE)
    {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
    0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
    -.5f, .5f, .5f, -.5f
};

static int xorder[] = {
    1, 2, -3, 4
};

static float yx[] = {
    0.f, 0.f, -.5f, .5f
};

static float yy[] = {
    0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
    1, 2, 3, -2, 4
};

static float zx[] = {
    1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
    .5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
    1, 2, 3, 4, -5, 6
};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;


//function

void
Axes(float length)
{
    glBegin(GL_LINE_STRIP);
    glVertex3f(length, 0., 0.);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., length, 0.);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., length);
    glEnd();

    float fact = LENFRAC * length;
    float base = BASEFRAC * length;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++)
    {
        int j = xorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 5; i++)
    {
        int j = yorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++)
    {
        int j = zorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
    }
    glEnd();

}

struct bmfh
{
    short bfType;
    int bfSize;
    short bfReserved1;
    short bfReserved2;
    int bfOffBits;
} FileHeader;

struct bmih
{
    int biSize;
    int biWidth;
    int biHeight;
    short biPlanes;
    short biBitCount;
    int biCompression;
    int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    int biClrUsed;
    int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

// read a BMP file into a Texture:

unsigned char*
BmpToTexture(char* filename, int* width, int* height)
{
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
        return NULL;
    }

    FileHeader.bfType = ReadShort(fp);


    // if bfType is not 0x4d42, the file is not a bmp:

    if (FileHeader.bfType != 0x4d42)
    {
        fprintf(stderr, "File '%s' is the wrong type of file: 0x%0x\n", filename, FileHeader.bfType);
        fclose(fp);
        return NULL;
    }

    FileHeader.bfSize = ReadInt(fp);
    FileHeader.bfReserved1 = ReadShort(fp);
    FileHeader.bfReserved2 = ReadShort(fp);
    FileHeader.bfOffBits = ReadInt(fp);

    InfoHeader.biSize = ReadInt(fp);
    InfoHeader.biWidth = ReadInt(fp);
    InfoHeader.biHeight = ReadInt(fp);

    int nums = InfoHeader.biWidth;
    int numt = InfoHeader.biHeight;

    InfoHeader.biPlanes = ReadShort(fp);
    InfoHeader.biBitCount = ReadShort(fp);
    InfoHeader.biCompression = ReadInt(fp);
    InfoHeader.biSizeImage = ReadInt(fp);
    InfoHeader.biXPelsPerMeter = ReadInt(fp);
    InfoHeader.biYPelsPerMeter = ReadInt(fp);
    InfoHeader.biClrUsed = ReadInt(fp);
    InfoHeader.biClrImportant = ReadInt(fp);

    fprintf(stderr, "Image size in file '%s' is: %d x %d\n", filename, nums, numt);

    unsigned char* texture = new unsigned char[3 * nums * numt];
    if (texture == NULL)
    {
        fprintf(stderr, "Cannot allocate the texture array!\b");
        return NULL;
    }

    // extra padding bytes:

    int numextra = 4 * (((3 * InfoHeader.biWidth) + 3) / 4) - 3 * InfoHeader.biWidth;

    // we do not support compression:

    if (InfoHeader.biCompression != birgb)
    {
        fprintf(stderr, "Image file '%s' has the wrong type of image compression: %d\n", filename, InfoHeader.biCompression);
        fclose(fp);
        return NULL;
    }

    rewind(fp);
    fseek(fp, 14 + 40, SEEK_SET);

    if (InfoHeader.biBitCount == 24)
    {
        unsigned char* tp = texture;
        for (int t = 0; t < numt; t++)
        {
            for (int s = 0; s < nums; s++, tp += 3)
            {
                *(tp + 2) = fgetc(fp);		// b
                *(tp + 1) = fgetc(fp);		// g
                *(tp + 0) = fgetc(fp);		// r
            }

            for (int e = 0; e < numextra; e++)
            {
                fgetc(fp);
            }
        }
    }

    fclose(fp);

    *width = nums;
    *height = numt;
    return texture;
}

int
ReadInt(FILE* fp)
{
    unsigned char b3, b2, b1, b0;
    b0 = fgetc(fp);
    b1 = fgetc(fp);
    b2 = fgetc(fp);
    b3 = fgetc(fp);
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short
ReadShort(FILE* fp)
{
    unsigned char b1, b0;
    b0 = fgetc(fp);
    b1 = fgetc(fp);
    return (b1 << 8) | b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
    // guarantee valid input:

    float h = hsv[0] / 60.f;
    while (h >= 6.)	h -= 6.;
    while (h < 0.) 	h += 6.;

    float s = hsv[1];
    if (s < 0.)
        s = 0.;
    if (s > 1.)
        s = 1.;

    float v = hsv[2];
    if (v < 0.)
        v = 0.;
    if (v > 1.)
        v = 1.;

    // if sat==0, then is a gray:

    if (s == 0.0)
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    // get an rgb from the hue itself:

    float i = (float)floor(h);
    float f = h - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * f);
    float t = v * (1.f - (s * (1.f - f)));

    float r = 0., g = 0., b = 0.;			// red, green, blue
    switch ((int)i)
    {
    case 0:
        r = v;	g = t;	b = p;
        break;

    case 1:
        r = q;	g = v;	b = p;
        break;

    case 2:
        r = p;	g = v;	b = t;
        break;

    case 3:
        r = p;	g = q;	b = v;
        break;

    case 4:
        r = t;	g = p;	b = v;
        break;

    case 5:
        r = v;	g = p;	b = q;
        break;
    }


    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

float
Dot(float v1[3], float v2[3])
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// delimiters for parsing the obj file:
#define OBJDELIMS        " \t"

struct Vertex
{
    float x, y, z;
};


struct Normal
{
    float nx, ny, nz;
};


struct TextureCoord
{
    float s, t, p;
};


struct face
{
    int v, n, t;
};

void     Cross(float[3], float[3], float[3]);
char* ReadRestOfLine(FILE*);
void     ReadObjVTN(char*, int*, int*, int*);
float    Unit(float[3]);
float    Unit(float[3], float[3]);

int
LoadObjFile(char* name)
{
    char* cmd;        // the command string
    char* str;        // argument string

    std::vector <struct Vertex> Vertices(10000);
    std::vector <struct Normal> Normals(10000);
    std::vector <struct TextureCoord> TextureCoords(10000);

    Vertices.clear();
    Normals.clear();
    TextureCoords.clear();

    struct Vertex sv;
    struct Normal sn;
    struct TextureCoord st;


    // open the input file:

    FILE* fp = fopen(name, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open .obj file '%s'\n", name);
        return 1;
    }


    float xmin = 1.e+37f;
    float ymin = 1.e+37f;
    float zmin = 1.e+37f;
    float xmax = -xmin;
    float ymax = -ymin;
    float zmax = -zmin;

    glBegin(GL_TRIANGLES);

    for (; ; )
    {
        char* line = ReadRestOfLine(fp);
        if (line == NULL)
            break;


        // skip this line if it is a comment:

        if (line[0] == '#')
            continue;


        // skip this line if it is something we don't feel like handling today:

        if (line[0] == 'g')
            continue;

        if (line[0] == 'm')
            continue;

        if (line[0] == 's')
            continue;

        if (line[0] == 'u')
            continue;


        // get the command string:

        cmd = strtok(line, OBJDELIMS);


        // skip this line if it is empty:

        if (cmd == NULL)
            continue;


        if (strcmp(cmd, "v") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sv.x = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.y = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.z = atof(str);

            Vertices.push_back(sv);

            if (sv.x < xmin)    xmin = sv.x;
            if (sv.x > xmax)    xmax = sv.x;
            if (sv.y < ymin)    ymin = sv.y;
            if (sv.y > ymax)    ymax = sv.y;
            if (sv.z < zmin)    zmin = sv.z;
            if (sv.z > zmax)    zmax = sv.z;

            continue;
        }


        if (strcmp(cmd, "vn") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sn.nx = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.ny = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.nz = atof(str);

            Normals.push_back(sn);

            continue;
        }


        if (strcmp(cmd, "vt") == 0)
        {
            st.s = st.t = st.p = 0.;

            str = strtok(NULL, OBJDELIMS);
            st.s = atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.t = atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.p = atof(str);

            TextureCoords.push_back(st);

            continue;
        }


        if (strcmp(cmd, "f") == 0)
        {
            struct face vertices[10];
            for (int i = 0; i < 10; i++)
            {
                vertices[i].v = 0;
                vertices[i].n = 0;
                vertices[i].t = 0;
            }

            int sizev = (int)Vertices.size();
            int sizen = (int)Normals.size();
            int sizet = (int)TextureCoords.size();

            int numVertices = 0;
            bool valid = true;
            int vtx = 0;
            char* str;
            while ((str = strtok(NULL, OBJDELIMS)) != NULL)
            {
                int v, n, t;
                ReadObjVTN(str, &v, &t, &n);

                // if v, n, or t are negative, they are wrt the end of their respective list:

                if (v < 0)
                    v += (sizev + 1);

                if (n < 0)
                    n += (sizen + 1);

                if (t < 0)
                    t += (sizet + 1);


                // be sure we are not out-of-bounds (<vector> will abort):

                if (t > sizet)
                {
                    if (t != 0)
                        fprintf(stderr, "Read texture coord %d, but only have %d so far\n", t, sizet);
                    t = 0;
                }

                if (n > sizen)
                {
                    if (n != 0)
                        fprintf(stderr, "Read normal %d, but only have %d so far\n", n, sizen);
                    n = 0;
                }

                if (v > sizev)
                {
                    if (v != 0)
                        fprintf(stderr, "Read vertex coord %d, but only have %d so far\n", v, sizev);
                    v = 0;
                    valid = false;
                }

                vertices[vtx].v = v;
                vertices[vtx].n = n;
                vertices[vtx].t = t;
                vtx++;

                if (vtx >= 10)
                    break;

                numVertices++;
            }


            // if vertices are invalid, don't draw anything this time:

            if (!valid)
                continue;

            if (numVertices < 3)
                continue;


            // list the vertices:

            int numTriangles = numVertices - 2;

            for (int it = 0; it < numTriangles; it++)
            {
                int vv[3];
                vv[0] = 0;
                vv[1] = it + 1;
                vv[2] = it + 2;

                // get the planar normal, in case vertex normals are not defined:

                struct Vertex* v0 = &Vertices[vertices[vv[0]].v - 1];
                struct Vertex* v1 = &Vertices[vertices[vv[1]].v - 1];
                struct Vertex* v2 = &Vertices[vertices[vv[2]].v - 1];

                float v01[3], v02[3], norm[3];
                v01[0] = v1->x - v0->x;
                v01[1] = v1->y - v0->y;
                v01[2] = v1->z - v0->z;
                v02[0] = v2->x - v0->x;
                v02[1] = v2->y - v0->y;
                v02[2] = v2->z - v0->z;
                Cross(v01, v02, norm);
                Unit(norm, norm);
                glNormal3fv(norm);

                for (int vtx = 0; vtx < 3; vtx++)
                {
                    if (vertices[vv[vtx]].t != 0)
                    {
                        struct TextureCoord* tp = &TextureCoords[vertices[vv[vtx]].t - 1];
                        glTexCoord2f(tp->s, tp->t);
                    }

                    if (vertices[vv[vtx]].n != 0)
                    {
                        struct Normal* np = &Normals[vertices[vv[vtx]].n - 1];
                        glNormal3f(np->nx, np->ny, np->nz);
                    }

                    struct Vertex* vp = &Vertices[vertices[vv[vtx]].v - 1];
                    glVertex3f(vp->x, vp->y, vp->z);
                }
            }
            continue;
        }


        if (strcmp(cmd, "s") == 0)
        {
            continue;
        }

    }

    glEnd();
    fclose(fp);

    fprintf(stderr, "Obj file range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
        xmin, ymin, zmin, xmax, ymax, zmax);
    fprintf(stderr, "Obj file center = (%8.3f,%8.3f,%8.3f)\n",
        (xmin + xmax) / 2., (ymin + ymax) / 2., (zmin + zmax) / 2.);
    fprintf(stderr, "Obj file  span = (%8.3f,%8.3f,%8.3f)\n",
        xmax - xmin, ymax - ymin, zmax - zmin);

    return 0;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
    float tmp[3];

    tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
    tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
    tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];

    vout[0] = tmp[0];
    vout[1] = tmp[1];
    vout[2] = tmp[2];
}

float
Unit(float v[3])
{
    float dist;

    dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    if (dist > 0.0)
    {
        dist = sqrt(dist);
        v[0] /= dist;
        v[1] /= dist;
        v[2] /= dist;
    }

    return dist;
}

float
Unit(float vin[3], float vout[3])
{
    float dist;

    dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];

    if (dist > 0.0)
    {
        dist = sqrt(dist);
        vout[0] = vin[0] / dist;
        vout[1] = vin[1] / dist;
        vout[2] = vin[2] / dist;
    }
    else
    {
        vout[0] = vin[0];
        vout[1] = vin[1];
        vout[2] = vin[2];
    }

    return dist;
}

char*
ReadRestOfLine(FILE* fp)
{
    static char* line;
    std::vector<char> tmp(1000);
    tmp.clear();

    for (; ; )
    {
        int c = getc(fp);

        if (c == EOF && tmp.size() == 0)
        {
            return NULL;
        }

        if (c == EOF || c == '\n')
        {
            delete[] line;
            line = new char[tmp.size() + 1];
            for (int i = 0; i < (int)tmp.size(); i++)
            {
                line[i] = tmp[i];
            }
            line[tmp.size()] = '\0';    // terminating null
            return line;
        }
        else
        {
            tmp.push_back(c);
        }
    }

    return "";
}

void
ReadObjVTN(char* str, int* v, int* t, int* n)
{
    // can be one of v, v//n, v/t, v/t/n:

    if (strstr(str, "//"))                // v//n
    {
        *t = 0;
        sscanf(str, "%d//%d", v, n);
        return;
    }
    else if (sscanf(str, "%d/%d/%d", v, t, n) == 3)    // v/t/n
    {
        return;
    }
    else
    {
        *n = 0;
        if (sscanf(str, "%d/%d", v, t) == 2)        // v/t
        {
            return;
        }
        else                        // v
        {
            *n = *t = 0;
            sscanf(str, "%d", v);
        }
    }
}
struct point {
    float x, y, z;        // coordinates
    float nx, ny, nz;    // surface normal
    float s, t;        // texture coords
};

int        NumLngs, NumLats;
struct point* Pts;

struct point* PtsPointer(int lat, int lng)
{
    if (lat < 0)    lat += (NumLats - 1);
    if (lng < 0)    lng += (NumLngs - 1);
    if (lat > NumLats - 1)    lat -= (NumLats - 1);
    if (lng > NumLngs - 1)    lng -= (NumLngs - 1);
    return &Pts[NumLngs * lat + lng];
}

void
DrawPoint(struct point* p)
{
    glNormal3f(p->nx, p->ny, p->nz);
    glTexCoord2f(p->s, p->t);
    glVertex3f(p->x, p->y, p->z);
}

void
OsuSphere(float radius, int slices, int stacks)
{
    // set the globals:

    NumLngs = slices;
    NumLats = stacks;
    if (NumLngs < 3)
        NumLngs = 3;
    if (NumLats < 3)
        NumLats = 3;

    // allocate the point data structure:

    Pts = new struct point[NumLngs * NumLats];

    // fill the Pts structure:

    for (int ilat = 0; ilat < NumLats; ilat++)
    {
        float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1);	// ilat=0/lat=0. is the south pole
                                            // ilat=NumLats-1, lat=+M_PI/2. is the north pole
        float xz = cosf(lat);
        float  y = sinf(lat);
        for (int ilng = 0; ilng < NumLngs; ilng++)				// ilng=0, lng=-M_PI and
                                            // ilng=NumLngs-1, lng=+M_PI are the same meridian
        {
            float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
            float x = xz * cosf(lng);
            float z = -xz * sinf(lng);
            struct point* p = PtsPointer(ilat, ilng);
            p->x = radius * x;
            p->y = radius * y;
            p->z = radius * z;
            p->nx = x;
            p->ny = y;
            p->nz = z;
            p->s = (lng + M_PI) / (2. * M_PI);
            p->t = (lat + M_PI / 2.) / M_PI;
        }
    }

    struct point top, bot;		// top, bottom points

    top.x = 0.;		top.y = radius;	top.z = 0.;
    top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
    top.s = 0.;		top.t = 1.;

    bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
    bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
    bot.s = 0.;		bot.t = 0.;

    // connect the north pole to the latitude NumLats-2:

    glBegin(GL_TRIANGLE_STRIP);
    for (int ilng = 0; ilng < NumLngs; ilng++)
    {
        float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
        top.s = (lng + M_PI) / (2. * M_PI);
        DrawPoint(&top);
        struct point* p = PtsPointer(NumLats - 2, ilng);	// ilat=NumLats-1 is the north pole
        DrawPoint(p);
    }
    glEnd();

    // connect the south pole to the latitude 1:

    glBegin(GL_TRIANGLE_STRIP);
    for (int ilng = NumLngs - 1; ilng >= 0; ilng--)
    {
        float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
        bot.s = (lng + M_PI) / (2. * M_PI);
        DrawPoint(&bot);
        struct point* p = PtsPointer(1, ilng);					// ilat=0 is the south pole
        DrawPoint(p);
    }
    glEnd();

    // connect the horizontal strips:

    for (int ilat = 2; ilat < NumLats - 1; ilat++)
    {
        struct point* p;
        glBegin(GL_TRIANGLE_STRIP);
        for (int ilng = 0; ilng < NumLngs; ilng++)
        {
            p = PtsPointer(ilat, ilng);
            DrawPoint(p);
            p = PtsPointer(ilat - 1, ilng);
            DrawPoint(p);
        }
        glEnd();
    }

    // clean-up:

    delete[] Pts;
    Pts = NULL;
}
