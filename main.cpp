
//  main.cpp
//  arkanoid
//
//  Created by Junhong Park on 02/11/2020.
//  Copyright © 2020 Junhong Park. All rights reserved.
//

/* Simple geometry viewer:  Left mouse: rotate;  Right mouse:   translate;  ESC to quit. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <GLUT/glut.h>
// you may try "#include <GL/glut.h>" if "#include <GLUT/glut.h>" wouldn't work
//#include <GL/glut.h>
using namespace std;
//GLdouble rotMatrix[4][16];
const int NO_SPHERE = 17;
const int WALL_ID = 1000;
const int WALL_WIDTH = 20;
const int WALL_HIGHT = 14;

int rotate_x = 180, rotate_y = 80;
int choice = 2;

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
float sdepth = 30;
float zNear=1.0, zFar=100-.0;
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
void InitObjects();
void auto_restart_game();
void restart_game();

float get_distance(float x1, float x2, float z1, float z2){
    
    float deltax = x2 - x1;
    float deltaz = z2 - z1;
    float ret;
    
    ret = sqrt(deltax * deltax + deltaz * deltaz);
    return (ret);
}
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
    
    bool hasIntersected(float x, float z)
    {
        float deltaX;
        float deltaZ;
        
        deltaX = this->center_x - x;
        deltaZ = this->center_z - z;
        if (sqrt(deltaX * deltaX + deltaZ * deltaZ) <= 0.8)
            return (true);
        return (false);
    }
    int currentTime, previousTime=-1;
    
    void hitBy(CSphere hitSphere)
    {
        float deltaX;
        float deltaZ;
        float distance;
        float hit_angle;
        float temp;
        
        deltaX = hitSphere.center_x - this->center_x;
        deltaZ = hitSphere.center_z - this->center_z;
        distance = sqrt(deltaX * deltaX + deltaZ * deltaZ);
        float k_x = deltaX / distance;
        float k_z = deltaZ / distance;
        float v_x = -dir_x;
        float v_z = -dir_z;
        
        float original_speed = sqrt(v_x*v_x + v_z*v_z);
        dir_x = v_x + 2 * ((k_x * v_x) + (k_z * v_z)) * k_x;
        dir_z = v_z + 2 * ((k_x * v_x) + (k_z * v_z)) * k_z;
        float new_speed = sqrt(dir_x*dir_x + dir_z*dir_z);
        dir_x *= original_speed / new_speed;
        dir_z *= original_speed / new_speed;
//        hit_angle = acosf(deltaX / distance);
//        temp = cos(hit_angle) * dir_x - sin(hit_angle) * dir_z;
//        dir_z = sin(hit_angle) * dir_x + cos(hit_angle) * dir_z;
//        dir_x = temp;
        
       
        
        
        
        
        
//        if (deltaZ > 0)
//            hit_angle = M_PI + M_PI - hit_angle;
//        // 두 구가 서로 끼이는 것을 방지! 끼이면 살짝 먼 곳으로 움직이던 구를 옮겨준다.
//        while (get_distance(this->center_x + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 * 3 * dir_z, hitSphere.center_z) < 0.85) {
//
//            deltaX = hitSphere.center_x - this->center_x;
//            deltaZ = hitSphere.center_z - this->center_z;
//            distance = sqrt(deltaX * deltaX + deltaZ * deltaZ);
//            float k_x = deltaX / distance;
//            float k_z = deltaZ / distance;
//            float v_x = -dir_x;
//            float v_z = -dir_z;
//
//            float original_speed = sqrt(v_x*v_x + v_z*v_z);
//            dir_x = v_x + 2 * ((k_x * v_x) + (k_z * v_z)) * k_x;
//            dir_z = v_z + 2 * ((k_x * v_x) + (k_z * v_z)) * k_z;
//            float new_speed = sqrt(dir_x*dir_x + dir_z*dir_z);
//            dir_x *= original_speed / new_speed;
//            dir_z *= original_speed / new_speed;
//
//            if (get_distance(this->center_x + 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z) > get_distance(this->center_x - 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z - 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z))
//            {
//                this->center_x += 0.03;
//                this->center_z += 0.03;
//            }
//            else
//            {
//                this->center_x -= 0.03;
//                this->center_z -= 0.03;
//            }
//        }
        //std::cout << timeDelta << std::endl;
    }
    
    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslated(center_x,center_y,center_z);
        glColor3f(color_r, color_g, color_b);
        glutSolidSphere(0.4,20,16);
    }
};

class CWall : CSphere
{
    
public:
    float width, height, depth;
    float center_x, center_y, center_z;
    float color_r,color_g,color_b;
    float up_wall = 8;
    float left_wall = 8;
    float right_wall = -8;
    
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
                Verts[i][j] = coef * BoxVerts[i][j];
            }
        }
    }
    
    //    bool CSphere :: hasIntersected (CSphere & ball) {} // 두 구 사이에 충돌이 있는지 확인
    //    void CSphere :: hitBy (CSphere & ball) {} // 두 구 사이에 충돌이있을 경우 수행해야하는 작업.
    //    bool CWall :: hasIntersected (CSphere & ball) {} // 구와 벽 사이에 충돌이 있는지 확인
    //    void CWall :: hitBy (CSphere & ball) {} // 구와 벽 사이에 충돌이있는 경우 수행해야하는 작업
    
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
    
    bool hasUpIntersected(CSphere *sphere)
    {
        if (sphere->center_z + 0.4 >= WALL_HIGHT / 2)
            return (true);
        return (false);
    }
    
    bool hasRightLeftIntersected(CSphere *sphere)
    {
        if (sphere->center_x + 0.4 >= WALL_WIDTH / 2 || sphere->center_x - 0.4 <= -1 * WALL_WIDTH / 2)
            return (true);
        return (false);
    }
    
    void hitBy(CSphere *sphere)
    {
        if (hasUpIntersected(sphere))
        {
            sphere->dir_z = -(sphere->dir_z);
        }
        else if (hasRightLeftIntersected(sphere))
        {
            sphere->dir_x = -(sphere->dir_x);
        }
    }
};

CSphere g_sphere[NO_SPHERE];
CWall g_wall(WALL_WIDTH ,0.2, WALL_HIGHT);
CWall test_wall(1 ,1, 1);
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
    test_wall.draw();
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
            if (space_flag) {
                g_sphere[0].center_x = g_sphere[1].center_x;
                g_sphere[0].center_z = -3.8;
                space_flag=0;
            }
            else {
                space_flag=1;
                g_sphere[0].dir_x = 0;
                g_sphere[0].dir_z = 1;
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
    
    if (id < NO_SPHERE) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_sphere[id].m_mRotate);
    }
    
    if (id==WALL_ID) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_wall.m_mRotate);
    }
    glPopMatrix();
}

int k = 0;

void MotionCallback(int x, int y) {
    int tdx=x-downX,tdy=0,tdz=y-downY,id=choice-1;
    if ((leftButton || rightButton) && k < 1) {
        rotate_x += 1;
        rotate_y += 1;
        restart_game();
        k++;
        for (i=0;i<NO_SPHERE;i++) rotate(i);
        rotate(WALL_ID);
    } else if (leftButton) {
        if (id < NO_SPHERE && g_sphere[id].center_x - 0.425 > -1 * (WALL_WIDTH / 2) && g_sphere[id].center_x + 0.425 < WALL_WIDTH / 2)
            g_sphere[id].setCenter(g_sphere[id].center_x - tdx/100.0,g_sphere[id].center_y,g_sphere[id].center_z);
        else if (g_sphere[id].center_x - 0.425 > -1 * (WALL_WIDTH / 2))
            g_sphere[id].setCenter(WALL_WIDTH / 2 - 1, g_sphere[id].center_y, g_sphere[id].center_z);
        else if (g_sphere[id].center_x + 0.425 < WALL_WIDTH / 2)
            g_sphere[id].setCenter(-1 * (WALL_WIDTH / 2) + 1, g_sphere[id].center_y, g_sphere[id].center_z);


        if (space_flag == 0)
        {
            g_sphere[0].center_x = g_sphere[id].center_x;
            g_sphere[0].center_z = -3.8;
        }
    }
        downX = x;   downY = y;
    glutPostRedisplay();
}

void initRotate() {
    int     i;
    
    i = 0;
    while (i < NO_SPHERE) {
        g_sphere[i].init();
        i++;
    }
    g_wall.init();
    test_wall.init();
}

void InitGL() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
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

void restart_game()
{
    space_flag = 0;
    InitObjects();
}

void auto_restart_game()
{
    int finish_idx;
    finish_idx = 3;
    while (finish_idx < NO_SPHERE)
    {
        if (g_sphere[finish_idx].center_z == 100)
            finish_idx++;
        else
            break;
        if (finish_idx == NO_SPHERE)
        {
            space_flag = 0;
            InitObjects();
        }
    }
}

void lose_game()
{
    if (g_sphere[0].center_z < -10)
    {
        space_flag = 0;
    }
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
    int temp_time;
    
    if (space_flag)
    {
        g_sphere[0].setCenter(
                              x + timeDelta*0.012 * g_sphere[0].dir_x,
                              y,
                              z + timeDelta*0.012 * g_sphere[0].dir_z);
    }
    
    // 공이 닿는 지점을 검사하고, 닿았을 경우 반사를 실행
    int idx;
    idx = 1;
    temp_time = -1;
    while (idx < NO_SPHERE) {
        if (g_sphere[0].hasIntersected(g_sphere[idx].center_x, g_sphere[idx].center_z) == true)
        {
            if (temp_time + 2 < currentTime)
            {
                g_sphere[0].hitBy(g_sphere[idx]);
                temp_time = currentTime;
            }
            if (idx != 1)
                g_sphere[idx].center_z = 100;
        }
        idx++;
    }
    
    // 벽에 대한 반사 실행
    g_wall.hitBy(&g_sphere[0]);
    
    // 게임에서 졌을 경우 (공이 밑으로 떨어졌을 때)
    lose_game();
    
    // 게임이 끝났을 경우
    auto_restart_game();
    
    glutPostRedisplay();
    previousTime=currentTime;
    
}

void InitObjects()
{
    // specify initial colors and center positions of each spheres
    g_sphere[0].setColor(0.8,0.2,0.2); g_sphere[0].setCenter(0.0,0.0,-3.8);
    g_sphere[1].setColor(1.0,1.0,1.0); g_sphere[1].setCenter(0.0,0.0,-4.75);
    g_sphere[2].setColor(0.2,0.2,0.8); g_sphere[2].setCenter(0.0,0.0,-100.0);
    g_sphere[16].setColor(0.2,0.2,0.8); g_sphere[15].setCenter(0.0,0.0,-1.0);
    g_sphere[3].setColor(0.2,0.2,0.8); g_sphere[3].setCenter(-2.0,0.0,1.0);
    g_sphere[4].setColor(0.2,0.2,0.8); g_sphere[4].setCenter(2.0,0.0,1.0);
    g_sphere[5].setColor(0.2,0.2,0.8); g_sphere[5].setCenter(-4.0,0.0,3.0);
    g_sphere[6].setColor(0.2,0.2,0.8); g_sphere[6].setCenter(4.0,0.0,3.0);
    g_sphere[7].setColor(0.2,0.2,0.8); g_sphere[7].setCenter(-6.0,0.0,3.5);
    g_sphere[8].setColor(0.2,0.2,0.8); g_sphere[8].setCenter(6.0,0.0,3.5);
    g_sphere[9].setColor(0.2,0.2,0.8); g_sphere[9].setCenter(0.0,0.0,5.5);
    g_sphere[10].setColor(0.2,0.2,0.8); g_sphere[10].setCenter(-2.0,0.0,3.5);
    g_sphere[11].setColor(0.2,0.2,0.8); g_sphere[11].setCenter(2.0,0.0,3.5);
    g_sphere[12].setColor(0.2,0.2,0.8); g_sphere[12].setCenter(-4.0,0.0,5.5);
    g_sphere[13].setColor(0.2,0.2,0.8); g_sphere[13].setCenter(4.0,0.0,5.5);
    g_sphere[14].setColor(0.2,0.2,0.8); g_sphere[14].setCenter(6.0,0.0,5.5);
    g_sphere[15].setColor(0.2,0.2,0.8); g_sphere[15].setCenter(-6.0,0.0,5.5);
    
    // specify initial colors and center positions of a wall
    g_wall.setColor(0.0,1.0,0.0); g_wall.setCenter(0.0,-0.5,0.0);
    test_wall.setColor(0.0, 0.0, 0.0); test_wall.setCenter(0, -0.5, 0);
}


using namespace std;
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    InitObjects();
    InitGL();
    glutMainLoop();
    return 0;
}
