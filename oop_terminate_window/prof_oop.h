#pragma once
#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <GL/glut.h>


// ------------------------------------------------------- v const for starting postion v -------------------------------------------------------
const float planeWidth = 40; // plane 가로
const float planeHeight = 30; // plane 세로
const float planeDepth = 0.2; // plane 두께
const int WALL_ID = 1000;

const int NO_SPHERE = planeWidth * planeHeight; // target_sphere[]의 구의 최대 개수
int cnt_placed_sphere = 0; // 배치에 의해 만들어지는 실제 target_sphere[]의 구의 개수
const float radius_sphere = 0.5;

const float control_sphere_init_x = 0; // 시작 때, 흰 공의 x 좌표
const float control_sphere_init_y = -13.0; // 시작 때, 흰 공의 y좌표

const float hit_sphere_init_x = 0; // 시작 때, 빨간 공의 x 좌표
const float hit_sphere_init_y = control_sphere_init_y + 2 * radius_sphere + 1; // 시작 때 빨간 공의 y 좌표

// ------------------------------------------------------- v arrays for convenience v -------------------------------------------------------

const std::string sphere_place[30] = { // 가로 길이는 planeWidth, 세로 길이는 planeWidth; '.'이면 빈 배치, '.'이 아니면 공을 배치한다. 오류나길래 planeHeight==30으로 바꿈
    "........................................",
    "........................................",
    "........o.....o..........o.....o........",
    "........ooooooo..........ooooooo........",
    "........o.....o..........o.....o........",
    "........ooooooo..........ooooooo........",
    "...........o................o...........",
    ".....ooooooooooooo....ooooooooooooo.....",
    "........................................",
    "..........ooo..............ooo..........",
    ".........o...o............o...o.........",
    ".........o...o............o...o.........",
    "..........ooo..............ooo..........",
    "........................................",
    "........................................",
    "........................................",
    "..........ooo.....ooo.....oooo..........",
    ".........o...o...o...o...o....o.........",
    ".........o...o...o...o...o....o.........",
    ".........o...o...o...o...ooooo..........",
    ".........o...o...o...o...o..............",
    "..........ooo.....ooo....o..............",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
};

GLfloat BoxVerts[][3] = { // 바닥의 꼭짓점 좌표, 일종의 단위벡터로 바닥크기를 바꾸려면 CWALL의 생성자를 바꿔라
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





// ------------------------------------------------------- v variables for viewer state v -------------------------------------------------------

float sdepth = 30; //멀리서보는지 가까이서 보는지
float zNear = 1.0, zFar = 100.0;
float aspect = 5.0 / 4.0;
long xsize, ysize;
int i, j;
GLfloat light0Position[] = { 0, 1, 0, 1.0 };


// ------------------------------------------------------- v vars for play v -------------------------------------------------------
int Score = 0;
int Life = 5;
GLboolean leftPressed = false;
GLboolean rightPressed = false;

// ------------------------------------------------------- v StateCode for easy programming v -------------------------------------------------------
enum State {
    GAME_START,
    LIFE_DECREASE,
    GAME_PLAYING,
    GAME_OVER,
    GAME_CLEAR
};

State statecode = GAME_START;

// ------------------------------------------------------- v interface of CSphere v -------------------------------------------------------

class CSphere {

private:
    float center_x, center_y, center_z;
    float color_r, color_g, color_b;
    float dir_x, dir_y, dir_z; // 공의 x, y, z축 성분 속도
    float speed;

public:
    GLdouble m_mRotate[16];
    CSphere() ;
    void init();
    void setCenter(float x, float y, float z);
    void setColor(float r, float g, float b);
    
    bool hasIntersected(const CSphere& ball);
    void hitBy(const CSphere hitSphere);
    void draw();

    friend class CWall;
    friend void InitObjects();
    friend void KeyboardCallback(unsigned char ch, int x, int y);
    friend void renderScene();

};
// ------------------------------------------------------- v interface of CWall v -------------------------------------------------------
class CWall {
private:
    float width, height, depth;
    float center_x, center_y, center_z;
    float color_r, color_g, color_b;

    GLfloat Verts[8][3];

public:
    GLdouble m_mRotate[16];
    CWall(float w=0.0, float h=0.0, float d=0.0);
   
    void init();
    void setSize(float x, float y, float z);
    void setCenter(float x, float y, float z);
    void setColor(float x, float y, float z);
    void draw();
    bool hasUpIntersected(CSphere* sphere);
    bool hasDownIntersected(CSphere* sphere, CSphere* control_sphere);
    bool hasLeftIntersected(CSphere* sphere);
    bool hasRightIntersected(CSphere* sphere);
    void hitBy(CSphere* sphere, CSphere* control_sphere);
    
};



void InitObjects();
void ReshapeCallback(int width, int height);
void renderBitmapCharacter(float x, float y, float z, void* font, char* string);
void DisplayCallback(void);
void KeyboardCallback(unsigned char ch, int x, int y);
void SpecialCallback(int key, int x, int y);
void SpecialUpCallback(int key, int x, int y);
void initRotate();
void InitGL();
void renderScene();