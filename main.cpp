/* Simple geometry viewer:  Left mouse: rotate;  Right mouse:   translate;  ESC to quit. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <GLUT/glut.h>
// you may try "#include <GL/glut.h>" if "#include <GLUT/glut.h>" wouldn't work
//#include <GL/glut.h>

//GLdouble rotMatrix[4][16];
const int NO_SPHERE=3;
const int WALL_ID=1000;

int rotate_x=0, rotate_y=0;
int choice=1;

GLfloat BoxVerts[][3] = {
    {-1.0,-1.0,-1.0},
    {-1.0,-1.0,1.0},
    {-1.0,1.0,-1.0},
    {-1.0,1.0,1.0},
    {1.0,-1.0,-1.0},
    {1.0,-1.0,1.0},
    {1.0,1.0,-1.0},
    {1.0,1.0,1.0}
};

GLfloat bNorms[][3] = {
    {-1.0,0.0,0.0},
    {1.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,-1.0,0.0},
    {0.0,0.0,-1.0},
    {0.0,0.0,1.0}
};

int cubeIndices[][4] = {
    {0,1,3,2},
    {4,6,7,5},
    {2,3,7,6},
    {0,4,5,1},
    {0,2,6,4},
    {1,5,7,3}
};

/* Viewer state */
float sdepth = 10;
float zNear=1.0, zFar=100.0;
float aspect = 5.0/4.0;
float xcam = 0, ycam = 0;
long xsize, ysize;
int downX, downY;
bool leftButton = false, middleButton = false, rightButton = false;
int i,j;
GLfloat light0Position[] = { 0, 1, 0, 1.0};
int displayMenu, mainMenu;

void MyIdleFunc(void) { glutPostRedisplay();} /* things to do while idle */
void RunIdleFunc(void) {   glutIdleFunc(MyIdleFunc); }
void PauseIdleFunc(void) {   glutIdleFunc(NULL); }
void renderScene();


class CSphere
{
public:
    float center_x, center_y, center_z;
    float color_r,color_g,color_b;
    float dir_x,dir_y,dir_z;
    float speed;

public :
    GLdouble m_mRotate[16];
    CSphere()
    {
        center_x=center_y=center_z=0.0;
    }

    void init()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void setCenter(float x, float y, float z)
    {
        center_x=x;    center_y=y;    center_z=z;
    }

    void setColor(float r, float g, float b)
    {
        color_r=r; color_g=g; color_b=b;
    }

    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslated(center_x,center_y,center_z);
        glColor3f(color_r, color_g, color_b);
        glutSolidSphere(0.5,20,16);
    }
};

class CWall
{

public:
    float width, height, depth;
    float center_x, center_y, center_z;
    float color_r,color_g,color_b;

    GLfloat Verts[8][3];

public :
    GLdouble m_mRotate[16];
    CWall(float w=0.0, float h=0.0, float d=0.0)
    {
        width=w; height=h; depth=d;
        color_r=0.0; color_g=1.0; color_b=0.0;

        int i,j;
        float coef;
        for (i=0;i<8;i++) {
            for (j=0;j<3;j++) {
                if (j==0) coef=w/2.0;
                if (j==1) coef=h/2.0;
                if (j==2) coef=d/2.0;
                Verts[i][j]=coef*BoxVerts[i][j];
            }
        }
    }

    void init()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void setSize(float w, float h, float d)
    {
        width=w;
        height=h;
        depth=d;
    }

    void setCenter(float x, float y, float z)
    {
        center_x=x;    center_y=y;    center_z=z;
    }

    void setColor(float r, float g, float b)
    {
        color_r=r; color_g=g; color_b=b;
    }

    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslatef(center_x,center_y,center_z);

        glColor3f(color_r, color_g, color_b);

        int i;
        int v1,v2,v3,v4;

        for (i = 0 ; i < 6 ; i++) {
            v1 = cubeIndices[i][0];
            v2 = cubeIndices[i][1];
            v3 = cubeIndices[i][2];
            v4 = cubeIndices[i][3];

            glBegin (GL_QUADS) ;
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v1][0],Verts[v1][1],Verts[v1][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v2][0],Verts[v2][1],Verts[v2][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v3][0],Verts[v3][1],Verts[v3][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v4][0],Verts[v4][1],Verts[v4][2]);
            glEnd () ;
        }
    }
};

CSphere g_sphere[3];
CWall g_wall(11,0.2,11);

void ReshapeCallback(int width, int height)
{
    xsize = width;
    ysize = height;
    aspect = (float)xsize/(float)ysize;
    glViewport(0, 0, xsize, ysize);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(64.0, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutPostRedisplay();
}



void DisplayCallback(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    for (i=0;i<NO_SPHERE;i++) g_sphere[i].draw();
    g_wall.draw();

    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int space_flag=0;

void KeyboardCallback(unsigned char ch, int x, int y)
{
    switch (ch)
    {
    case '1' : choice=1; break;
    case '2' : choice=2; break;
    case '3' : choice=3; break;

    case 32 :
        if (space_flag) space_flag=0;
        else {
            space_flag=1;
            g_sphere[0].dir_x = g_sphere[2].center_x - g_sphere[0].center_x;
            g_sphere[0].dir_y = g_sphere[2].center_y - g_sphere[0].center_y;
            g_sphere[0].dir_z = g_sphere[2].center_z - g_sphere[0].center_z;
        }
        break; // SPACE_KEY

    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y)
{
    downX = x; downY = y;
    leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
    middleButton = ((button == GLUT_MIDDLE_BUTTON) &&  (state == GLUT_DOWN));
    rightButton = ((button == GLUT_RIGHT_BUTTON) &&  (state == GLUT_DOWN));
    glutPostRedisplay();
}

void rotate(int id)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRotated(((double)rotate_y), 1.0, 0.0, 0.0);
    glRotated(((double)rotate_x), 0.0, 1.0, 0.0);

    if (id<NO_SPHERE) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_sphere[id].m_mRotate);
    }

    if (id==WALL_ID) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_wall.m_mRotate);
    }
    glPopMatrix();
}

void MotionCallback(int x, int y) {
    int tdx=x-downX,tdy=0,tdz=y-downY,id=choice-1;
    if (leftButton) {
        rotate_x += x-downX;
        rotate_y += y-downY;
        for (i=0;i<NO_SPHERE;i++) rotate(i);
        rotate(WALL_ID);
    } else if (rightButton) {
        if (id<NO_SPHERE) g_sphere[id].setCenter(g_sphere[id].center_x+tdx/100.0,g_sphere[id].center_y+tdy/100.0,g_sphere[id].center_z+tdz/100.0);
    }
    downX = x;   downY = y;
    glutPostRedisplay();
}

void initRotate() {
    g_sphere[0].init();
    g_sphere[1].init();
    g_sphere[2].init();
    g_wall.init();
}

void InitGL() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("OpenGL Applications");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.7, 0.7, 0.7, 0.0);
    glPolygonOffset(1.0, 1.0);
    glDisable(GL_CULL_FACE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glLightfv (GL_LIGHT0, GL_POSITION, light0Position);
    glEnable(GL_LIGHT0);
    initRotate();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);

    glutIdleFunc(renderScene);
    glutReshapeFunc(ReshapeCallback);
    glutDisplayFunc(DisplayCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
}

int currentTime, previousTime=-1;
void renderScene()
{
    int timeDelta;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (previousTime==-1) timeDelta=0;
    else timeDelta = currentTime - previousTime;
    float x=g_sphere[0].center_x;
    float y=g_sphere[0].center_y;
    float z=g_sphere[0].center_z;

    if (space_flag) g_sphere[0].setCenter(
        x+timeDelta*0.002*g_sphere[0].dir_x,
        y+timeDelta*0.002*g_sphere[0].dir_y,
        z+timeDelta*0.002*g_sphere[0].dir_z);
    glutPostRedisplay();
    previousTime=currentTime;

}

void InitObjects()
{
    // specify initial colors and center positions of each spheres
    g_sphere[0].setColor(0.8,0.2,0.2); g_sphere[0].setCenter(0.0,0.0,0.0);
    g_sphere[1].setColor(0.2,0.8,0.2); g_sphere[1].setCenter(1.0,0.0,0.0);
    g_sphere[2].setColor(0.2,0.2,0.8); g_sphere[2].setCenter(0.0,0.0,1.0);

    // specify initial colors and center positions of a wall
    g_wall.setColor(0.0,1.0,0.0); g_wall.setCenter(0.0,-0.6,0.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    InitObjects();
    InitGL();
    glutMainLoop();
    return 0;
}
