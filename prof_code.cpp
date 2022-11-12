/* Simple geometry viewer:  Left mouse: rotate;  Right mouse:   translate;  ESC to quit. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
//추가해야함 #include <iostream>
#include <GL/glut.h>
#include <iostream>
// you may try "#include <GL/glut.h>" if "#include <GLUT/glut.h>" wouldn't work
//#include <GL/glut.h>
//test
//GLdouble rotMatrix[4][16];
//추가해야함 const int NO_SPHERE = 17;
const int NO_SPHERE = 32; // g_sphere[]의 구의 개수
const int WALL_ID = 1000;
/* 추가해야함
const int WALL_WIDTH=16;
const int WALL_HIGHT=16;
*/
/*추가해야함
int rotate_x=180, rotate_y=80;
int choice=2;*/
const int rotate_x = 0;
const int rotate_y = 90; // 초기 initRotate에서 공과 평면이 회전하는 각도, rotate_y만 90도 회전하도록 한다.

const float planeWidth = 10; // plane 가로
const float planeHeight = 20; // plane 세로
const float planeDepth = 0.2; // plane 두께
int choice = 1;

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

/* Viewer state */
float sdepth = 20; //멀리서보는지 가까이서 보는지
float zNear = 1.0, zFar = 100.0;
float aspect = 5.0 / 4.0;
float xcam = 0, ycam = 0;
long xsize, ysize;
int downX, downY;
bool leftButton = false, middleButton = false, rightButton = false;
int i, j;
GLfloat light0Position[] = { 0, 1, 0, 1.0 };
int displayMenu, mainMenu;

void MyIdleFunc(void) { glutPostRedisplay(); } /* things to do while idle */
void RunIdleFunc(void) { glutIdleFunc(MyIdleFunc); }
void PauseIdleFunc(void) { glutIdleFunc(NULL); }
void renderScene();
/* 추가해야 함
void InitObjects();
void auto_restart_game();
void restart_game();
float get_distance(float x1, float x2, float z1, float z2) {
   float deltax=x2-x1;
   float deltaz=z2-z1;
   float ret;

   ret=sqrt(deltax*deltax+deltaz*deltaz);
   return (ret);
}
*/

class CSphere
{
public:
    float center_x, center_y, center_z;
    float color_r, color_g, color_b;
    float dir_x, dir_y, dir_z;
    float speed;

public:
    GLdouble m_mRotate[16];
    CSphere()
    {
        center_x = center_y = center_z = 0.0;
    }

    void init()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix(); // 현재 모델뷰 행렬을 저장, 그리고 그 밑으로 그려지는 것들에 대해 현재 모델뷰 행렬을 가지고 다시 누적시켜가면서 표현
        glLoadIdentity(); // 행렬을 단위행렬로 초기화한다.
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate); // 매트릭스 종류, 값을 받을 매트릭스
        glPopMatrix(); // 그동안 누적했던 행렬을 없애고 push한 행렬을 다시 불러온다.
    }

    void setCenter(float x, float y, float z)
    {
        center_x = x;    center_y = y;    center_z = z;
    }

    void setColor(float r, float g, float b)
    {
        color_r = r; color_g = g; color_b = b;
    }

    // 수정되지 않은 다른 사람의 hasIntersected, 공이 끼지 않게 하려면 개선해야함
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

    // 우리가 구현한 hitBy, 공이 끼지 않게 하려면 개선해야함
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

        float original_speed = sqrt(v_x * v_x + v_z * v_z);
        dir_x = v_x + 2 * ((k_x * v_x) + (k_z * v_z)) * k_x;
        dir_z = v_z + 2 * ((k_x * v_x) + (k_z * v_z)) * k_z;
        float new_speed = sqrt(dir_x * dir_x + dir_z * dir_z);
        dir_x *= original_speed / new_speed;
        dir_z *= original_speed / new_speed;
    }
    /*추가해야함
    bool hasIntersected(float x, float z)
    {
       float deltaX;
       float deltaZ;

       deltaX = this->center_x - x;
       deltaZ = this->center_z - z;
       if (sqrt(deltaX * deltaX + deltaZ * deltaZ) <= 0.85)
          return (true);
       return (false);
    }
    int currentTime, previousTime = -1;

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
       hit_angle = acosf(deltaX / distance);
       temp = cos(hit_angle) * dir_x - sin(hit_angle) * dir_z;
       dir_z = sin(hit_angle) * dir_x + cos(hit_angle) * dir_z;
       dir_x = temp;

       //        if (deltaZ > 0)
       //            hit_angle = M_PI + M_PI - hit_angle;
             // 두 구가 서로 끼이는 것을 방지! 끼이면 살짝 먼 곳으로 움직이던 구를 옮겨준다.
       while (get_distance(this->center_x + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 * 3 * dir_z, hitSphere.center_z) < 0.85) {

          temp = cos(hit_angle) * dir_x - sin(hit_angle) * dir_z;
          dir_z = sin(hit_angle) * dir_x + cos(hit_angle) * dir_z;
          dir_x = temp;
          if (get_distance(this->center_x + 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z + 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z) > get_distance(this->center_x - 0.03 + 0.03 * 3 * dir_x, hitSphere.center_x, this->center_z - 0.03 + 0.03 * 3 * dir_z, hitSphere.center_z))
          {
             this->center_x += 0.03;
             this->center_z += 0.03;
          }
          else
          {
             this->center_x -= 0.03;
             this->center_z -= 0.03;
          }
       }
       //std::cout << timeDelta << std::endl;
    }
    */
    void draw()
    {
        glLoadIdentity(); //단위행렬로 초기화
        glTranslatef(0.0, 0.0, -sdepth); //+Z방향이 화면에서 우리가 보는방향이므로 -를하면 축소되는 효과, wall과 위상을 맞추기위해 사용한듯
        glMultMatrixd(m_mRotate); // 마우스 이동에따라 motion콜백함수에서 m_mrotate행렬이 변형되는데 이 mult함수로 단위행렬에 곱하여 m_mrotate대로 회전한다.
        glTranslated(center_x, center_y, center_z); // 중앙으로 이동한다.
        glColor3f(color_r, color_g, color_b); //색 조정
        //추가해야함 glutSoilidSphere(0.4,20,16);
        glutSolidSphere(0.5, 20, 16); //0.5의 반지름 나머지 두 인자는 구를 나타내는 경선과 위선
    }
};

class CWall // 추가해야함 CSphere에서 상속
{

public:
    float width, height, depth;
    float center_x, center_y, center_z;
    float color_r, color_g, color_b;
    /*float up_wall=8;
    float left_wall=8;
    float right_wall=-8;

    */

    GLfloat Verts[8][3];

public:
    GLdouble m_mRotate[16];
    CWall(float w = 0.0, float h = 0.0, float d = 0.0)
    {
        width = w; height = h; depth = d;
        color_r = 0.0; color_g = 1.0; color_b = 0.0;

        int i, j;
        float coef;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 3; j++) {
                if (j == 0) coef = w / 2.0;
                if (j == 1) coef = h / 2.0;
                if (j == 2) coef = d / 2.0;
                Verts[i][j] = coef * BoxVerts[i][j]; // BoxVerts는 일종의 단위벡터, coef는 일종의 단위벡터에 곱해지는 크기
            }
        }
    }

    void init() //initGL에서 initrotate를 호출하고 거기서 호출됨
    { // openGL에서 행렬은 mode를 지정한 후, transelate, rotate등의 변환을 거치는데, 일반적으로 변환을 한 후 LoadIdentity로 초기화를 해줘야 한다. 안그러면 누적되니까.. 근데 이 방법 말고도
       //스택을 이용해서 현재 위치를 push로 저장한 후 변형을 한 후에 그냥 pop으로 받을수도 있다.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate); //매트릭스 종류, 값을 받을 매트릭스, m_mRotate엔 단위행렬이 들어감
        glPopMatrix();
    }

    void setSize(float w, float h, float d)
    {
        width = w;
        height = h;
        depth = d;

        int i, j;
        float coef;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 3; j++) {
                if (j == 0) coef = w / 2.0;
                if (j == 1) coef = h / 2.0;
                if (j == 2) coef = d / 2.0;
                Verts[i][j] = coef * BoxVerts[i][j]; // BoxVerts는 일종의 단위벡터, coef는 일종의 단위벡터에 곱해지는 크기
            }
        }
    }

    void setCenter(float x, float y, float z)
    {
        center_x = x;    center_y = y;    center_z = z;
    }

    void setColor(float r, float g, float b)
    {
        color_r = r; color_g = g; color_b = b;
    }

    void draw() //display 콜백함수에서 호출됨
    {
        glLoadIdentity();
        glTranslatef(0.0, 0.0, -sdepth); //  이 행렬이 현재의 모델뷰행렬에 곱해져서 평행이동을 한다.
        //모델뷰행렬에 1 0 0 0.0
        //            0 1 0 0.0
        //            0 0 1 -sdepth
//                              0 0 0   1  이 곱해지게 된다.
//화면에서 우리쪽이 z벡터가 +가 되는쪽이므로 sdepth가 커질수록 wall이 작아지는 효과를 얻을 수 있다.
        glMultMatrixd(m_mRotate); //현재 행렬에 rotate행렬을곱한다. m_mRotate는 motion콜백함수에서 마우스 움직임에따라 설정된다.
        glTranslatef(center_x, center_y, center_z); //wall을 center_x,y,z만큼의 위치로 평행이동

        glColor3f(color_r, color_g, color_b); //색 설정

        int i;
        int v1, v2, v3, v4;

        for (i = 0; i < 6; i++) {
            v1 = cubeIndices[i][0];
            v2 = cubeIndices[i][1];
            v3 = cubeIndices[i][2];
            v4 = cubeIndices[i][3];

            glBegin(GL_QUADS); //glbegin은 도형을 그리기 시작했다는 뜻
            glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
            glVertex3f(Verts[v1][0], Verts[v1][1], Verts[v1][2]);
            glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
            glVertex3f(Verts[v2][0], Verts[v2][1], Verts[v2][2]);
            glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
            glVertex3f(Verts[v3][0], Verts[v3][1], Verts[v3][2]);
            glNormal3f(bNorms[i][0], bNorms[i][1], bNorms[i][2]);
            glVertex3f(Verts[v4][0], Verts[v4][1], Verts[v4][2]);
            glEnd(); //glbegin과 마찬가지로 도형그리기 끝
        }
    }
    /*추가해야함
    bool hasUpIntersected(CSphere* sphere)
    {
       if (sphere->center_z + 0.425 >= WALL_HIGHT / 2)
          return (true);
       return (false);
    }

    bool hasRightLeftIntersected(CSphere* sphere)
    {
       if (sphere->center_x + 0.425 >= WALL_WIDTH / 2 || sphere->center_x - 0.425 <= -1 * WALL_WIDTH / 2)
          return (true);
       return (false);
    }

    void hitBy(CSphere* sphere)
    {
       if (hasUpIntersected(sphere))
       {
          sphere->dir_z = -(sphere->dir_z);
       }
       else if (hasRightLeftIntersected(sphere))
       {
          sphere->dir_x = -(sphere->dir_x);
       }
    }*/


};


/*추가해야함
CSphere g_sphere[NO_SPHERE];
CWall g_wall(WALL_WIDTH, 0.2, WALL_HIGHT);
*/

CSphere g_sphere[NO_SPHERE]; // 공 배열
CWall g_wall(planeWidth, planeDepth, planeHeight); // 바닥 평면
CWall boundary_wall[4]; // 가장자리 벽

void ReshapeCallback(int width, int height)
{

    xsize = width;
    ysize = height;
    aspect = (float)xsize / (float)ysize;

    glViewport(0, 0, xsize, ysize); //처음 생성되는 윈도우는 InitGL의 glutInitWindowsize에서 결정되지만 실행되는 프로그램 자체는 여기 이 함수의 해상도에 따라 조절된다. 뭔소린가 싶으면
    // 각각 1920,1080으로 실행해 보면 알기쉬울듯
    glMatrixMode(GL_PROJECTION); //openGL은 모델링을 GL_MODELVIEW, GL_PROJECTION, GL_TEXTURE 3개의 행렬로 화면에표시한다. 만약 (1,1,1)에 뭘 그린다면 해당 좌표는 GL_MODELVIEW 매트릭스를
    // 곱해서 실제적인 위치를 지정한다. 따라서 내부적인 위치정보는 하나더라도 GL_MODELVIEW 매트릭스가 변경된다면 전혀 다른 위치에 그려진다. 이렇게 그려진 모델은
       //GL_PROJECTION 행렬과 곱해져 화면상에 투영된다. 즉 GL_MODELVIEW가 모델링에 대한 실존 위치라면 GL_PROJECTION은 해당 모델링을 최종적으로 어떻게 표시할지 결정한다.

/*
GL_MODELVIEW에 관해: 특정좌표에 도형을 그린다면 GL_MODELVIEW를 곱해 실제적인 위치를 지정, 만약 이게 변경된다면 전혀다른 좌표에 그려짐
glTranslatef, glScalef, glRotatef등을 통해 물체의 위치를 변환 또는 제어함 이를 모델링 변환 이라고 함,모델뷰행렬이란 좌표계 변환을 통해 물체의 위치와 방향을 결정하기 위한
4x4행렬각 버텍스들은 하나의 열로 구성된 행렬로 만들어져서 모델뷰행렬과 곱해짐

GL_PROJECTION에 관해: GL_MODELVIEW가 그려진 도형의 위치라면 도형은 GL_PROJECTION의 행렬과 곱해져 최종적으로 어떻게 화면에 뿌릴것인가를 계산한다.
직교투영 glOrtho 원근투영 gluPerspectivef


*/


    glLoadIdentity(); // 단위행렬로 초기화한다.
    gluPerspective(64.0, aspect, zNear, zFar); //원근투영

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // 단위행렬로 초기화한다.   이전에 적용했던 회전값을 무시하고 다시 설정하기위해 리셋을 해야한다. 이 코드가 없으면 회전이 계속 누적적용된다.
    //이와같이 glMatrixMode(GL_MODELVIEW)이후에 glLoadIdentity()가 나오는게 이러한 이유이다.

    glutPostRedisplay();
}



void DisplayCallback(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //glclear는 버퍼들을 미리설정된 값으로 바꾼다.   
    //GL_COLOR_BUFFER_BIT는 initgl의 glclearcolor에서 설정된 값이다. 
    //한편 3차원을 2차원에 그리기위해 depth라는 개념이 있다. 같은 x,y라도 depth차이에 따라 그려지고 말고가 정해진다.
    //이 줄 자체가 하나의 관용구 비슷하게 쓰이는데, 3차원그림 전체를 지우는 역할을 한다.
    glMatrixMode(GL_MODELVIEW);



    for (i = 0; i < NO_SPHERE; i++) g_sphere[i].draw(); //공 그리기
    g_wall.draw(); // 벽 그리기
    for (int i = 0; i < 4; i++) boundary_wall[i].draw(); // boundary_wall 그리기

    glutSwapBuffers(); // front버퍼와 back버퍼를 swapping 하기 위한것, 프론트버퍼내용이 화면에 뿌려지는 동안 새로운 내용이 백버퍼에 쓰이고 백버퍼에 기록이 다 되면 프론트와 백이 바뀐다.
    //백버퍼에 그림을 다 그렸으면 전면버퍼와 통째로 교체한다. 전면과 후면이 일시에 교체되므로 백버퍼에 미리 준비해둔 그림이 나타난다.
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 빼도 상관없는듯?
}

int space_flag = 0;

void KeyboardCallback(unsigned char ch, int x, int y)
{
    switch (ch)
    {
    case '1': choice = 1; break;
    case '2': choice = 2; break;
    case '3': choice = 3; break;

    case 32: //스페이스바
        if (space_flag) space_flag = 0;
        else {
            space_flag = 1;
            g_sphere[0].dir_x = g_sphere[1].center_x - g_sphere[0].center_x;//sphere[0]은 스페이스를 누르면 움직이는 빨간 공
            g_sphere[0].dir_y = g_sphere[0].center_y;
            g_sphere[0].dir_z = g_sphere[0].center_z - g_sphere[1].center_z;
        }
        break; // SPACE_KEY

    case 27: //ESC키
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y)
{ // GLUT_DOWN=마우스를 누르고 있는 상태, LEFT,RIGHT,MIDDLE 각각 왼쪽 오른쪽 휠 이런거
    downX = x; downY = y;
    leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
    middleButton = ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN));
    rightButton = ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN));
    glutPostRedisplay();
}

void rotate(int id)
{
    glMatrixMode(GL_MODELVIEW); //모델뷰 모드로 사용하겠다.
    glPushMatrix(); // 현재 행렬 저장
    glLoadIdentity(); // 단위행렬로 초기화 (위의 현재행렬과 다름)

    glRotated(((double)rotate_y), 1.0, 0.0, 0.0); //단위행렬로 초기화된걸 rotate연산
    glRotated(((double)rotate_x), 0.0, 1.0, 0.0); // 마찬가지

    if (id < NO_SPHERE) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_sphere[id].m_mRotate); //매트릭스 종류, 값을 받을 메트릭스, 즉 modelview_matrix종류의 g_sphere...에 바로 위에서 rotate한 행렬을 받아오겠다. 
    }

    if (id == WALL_ID) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_wall.m_mRotate);

        for (int i = 0; i < 4; i++) glGetDoublev(GL_MODELVIEW_MATRIX, boundary_wall[i].m_mRotate); // test_wall 그리기
    }

    glPopMatrix();
}

//추가해야함 int k=0;

void MotionCallback(int x, int y) { // 구현이 다름
    int tdx = x - downX, tdy = 0, tdz = y - downY, id = 0;
    /*
    if (leftButton) { //왼쪽마우스를 누르면 벽과 구를 회전, 여길 없애면 마우스 눌러서 화면전환 안할수 있음
       rotate_x += x - downX;
       rotate_y += y - downY;
       for (i = 0; i < NO_SPHERE; i++) rotate(i);
       rotate(WALL_ID);
    }*/
    if (rightButton) { // 붉은공의 위치변경, 이부분을 잘 만지면 arkanoid에서 흰색공 위치 조절 가능
        if (space_flag == 0)
        {
            if (id < NO_SPHERE)
            {
                g_sphere[id].setCenter(g_sphere[id].center_x + tdx / 100.0, 0, g_sphere[id].center_z);
                g_sphere[id + 1].setCenter(g_sphere[id + 1].center_x + tdx / 100.0, 0, g_sphere[id + 1].center_z);
            }
        }
        else
        {
            if (id < NO_SPHERE)
            {
                g_sphere[id + 1].setCenter(g_sphere[id + 1].center_x + tdx / 100.0, 0, g_sphere[id + 1].center_z);
            }
        }

    }
    downX = x;   downY = y;
    glutPostRedisplay();
}

void initRotate() { // 구현이 살짝 다름 initGL에서 호출
    g_sphere[0].init();
    g_sphere[1].init();
    g_sphere[2].init();
    g_wall.init();

    for (int i = 0; i < 4; i++) boundary_wall[i].init();
    // 초기에 공과 벽을 z축을 중심으로 한 번 회전시켜 위에서 아래로 내려보는 것 처럼 만든다
    for (i = 0; i < NO_SPHERE; i++) rotate(i);
    rotate(WALL_ID);
}

void InitGL() {
    //opengl의 기본 설정값
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // 깊이버퍼가 있고 rgb색상버퍼가 있는 이중버퍼창의 모드를 설정
    glutInitWindowSize(1024, 768);
    glutCreateWindow("OpenGL Applications"); // Displaymode에서 설정한 버퍼대로 창을 띄워라
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
    glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
    glEnable(GL_LIGHT0);
    initRotate(); //구와 wall의 init호출





    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);



    //이하 opengl의 이벤트 처리부분
    //콜백함수란: 이벤트 처리기라고도 한다.w를 누르면 앞으로 가고, esc를 누르면 꺼지는거와 같이 어떤 반응에대한 결과를 나타내는 함수인듯, 디스플레이콜백함수의 경우 프레임마다 호출될것이다.

    glutIdleFunc(renderScene); // 어플리케이션의 휴면(idle)시간에 호출되는 함수(renderscene)를 호출하도록한다. 즉 애니메이션이 된다.
    glutReshapeFunc(ReshapeCallback); // GLUT는 처음 윈도우를 열때, 윈도우 위치를 옮길때, 윈도우 크기를 조절할때 reshapeevent가 발생한걸로 본다. 매개변수속의 함수로 원하는 함수효과를 할수있다.
    // reshapeevent 가 발생하면 괄호안의 파라미터를통해 변경된 윈도우의 폭과 높이를 콜백함수로 전달한다. 그래서 width와 height를 그냥 받아서 쓰는듯.
    glutDisplayFunc(DisplayCallback); // 이 함수는 DisplayCallback이라는 함수를 디스플레이이벤트에 대한 콜백함수로 사용하는 함수, 매개변수로 전달한 함수는 디스플레이이벤트마다 호출된다.
    glutKeyboardFunc(KeyboardCallback); // 키보드가 눌렸을경우 작동하는 콜백함수
    glutMouseFunc(MouseCallback); // 마우스가 눌렸을경우 작동하는 콜백함수
    glutMotionFunc(MotionCallback); // 버튼을 누른상태에서 마우스를 움직일때 작동하는 콜백함수, 아무런 버튼도 누르지 않은 상태에서 마우스를 움직이면 glutPassivemotionfunc이다.
}

/* 추가해아함
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

int currentTime, previousTime = -1;

*/

int currentTime, previousTime = -1; // 거기엔 없음
void renderScene() // 구현 다름, 어플리케이션의 휴면시간에 호출되는 함수, glutidleFunc()에서 호출된다.
{

    int timeDelta;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (previousTime == -1) timeDelta = 0;
    else timeDelta = currentTime - previousTime;
    int temp_time;

    float x = g_sphere[0].center_x;
    float y = g_sphere[0].center_y;
    float z = g_sphere[0].center_z;

    if (space_flag) g_sphere[0].setCenter(
        x + timeDelta * 0.002 * g_sphere[0].dir_x, // 해당구체의 속도, 0.002가 기본값, 
        y + timeDelta * 0.002 * g_sphere[0].dir_y,
        z + timeDelta * 0.002 * g_sphere[0].dir_z);
    glutPostRedisplay(); // 윈도우를 다시그리도록 요청, 바로 디스플레이콜백함수(renderscene)가 호출되진 않고 메인루프(아마 glutMainloop?)에서 호출시점을 결정한다. 이게 없으면 연결이 부자연스러움

    // renderScene에서 공 사이의 충돌을 처리하는 부분
    // 공이 닿는 지점을 검사하고, 닿았을 경우 반사를 실행
    int idx;
    idx = 1;
    temp_time = -1;
    while (idx < NO_SPHERE) {
        if (g_sphere[0].hasIntersected(g_sphere[idx].center_x, g_sphere[idx].center_z) == true)
        {
            if (temp_time + 1 < currentTime)
            {
                g_sphere[0].hitBy(g_sphere[idx]);
                temp_time = currentTime;
            }
            // 밑에 코드는 충돌된 공의 z 좌표를 100으로 설정하여 화면 밖으로 나가게 한다. 즉, 화면상에서 공을 없애는 효과
            //if (idx != 1)
                //g_sphere[idx].center_z = 100;
        }
        idx++;
    }

    previousTime = currentTime;

}

void InitObjects()
{
    // specify initial colors and center positions of each spheres
    g_sphere[0].setColor(0.8, 0.2, 0.2); g_sphere[0].setCenter(0.0, 0.0, 6.0);
    g_sphere[1].setColor(0.8, 0.8, 0.8); g_sphere[1].setCenter(0.0, 0.0, 8.0);
    g_sphere[2].setColor(0.2, 0.2, 0.8); g_sphere[2].setCenter(0.0, 0.0, 0.0);
    g_sphere[3].setColor(0.2, 0.2, 0.8); g_sphere[3].setCenter(1.3, 0.0, 0.0);
    g_sphere[4].setColor(0.2, 0.2, 0.8); g_sphere[4].setCenter(2.6, 0.0, 0.0);
    g_sphere[5].setColor(0.2, 0.2, 0.8); g_sphere[5].setCenter(3.9, 0.0, -1.0);
    g_sphere[6].setColor(0.2, 0.2, 0.8); g_sphere[6].setCenter(3.9, 0.0, -2.3);
    g_sphere[7].setColor(0.2, 0.2, 0.8); g_sphere[7].setCenter(3.9, 0.0, -3.6);
    g_sphere[8].setColor(0.2, 0.2, 0.8); g_sphere[8].setCenter(3.9, 0.0, -4.9);
    g_sphere[9].setColor(0.2, 0.2, 0.8); g_sphere[9].setCenter(3.9, 0.0, -6.2);
    g_sphere[18].setColor(0.2, 0.2, 0.8); g_sphere[18].setCenter(3.9, 0.0, -7.5);
    g_sphere[10].setColor(0.2, 0.2, 0.8); g_sphere[10].setCenter(2.6, 0.0, -8.5);
    g_sphere[11].setColor(0.2, 0.2, 0.8); g_sphere[11].setCenter(1.3, 0.0, -8.5);
    g_sphere[12].setColor(0.2, 0.2, 0.8); g_sphere[12].setCenter(0.0, 0.0, -8.5);
    g_sphere[13].setColor(0.2, 0.2, 0.8); g_sphere[13].setCenter(-1.3, 0.0, -8.5);
    g_sphere[14].setColor(0.2, 0.2, 0.8); g_sphere[14].setCenter(-2.6, 0.0, -8.5);
    g_sphere[15].setColor(0.2, 0.2, 0.8); g_sphere[15].setCenter(-3.9, 0.0, -7.5);
    g_sphere[16].setColor(0.2, 0.2, 0.8); g_sphere[16].setCenter(-3.9, 0.0, -6.2);
    g_sphere[17].setColor(0.2, 0.2, 0.8); g_sphere[17].setCenter(-3.9, 0.0, -4.9);
    g_sphere[19].setColor(0.2, 0.2, 0.8); g_sphere[19].setCenter(-3.9, 0.0, -3.6);
    g_sphere[20].setColor(0.2, 0.2, 0.8); g_sphere[20].setCenter(-3.9, 0.0, -2.3);
    g_sphere[21].setColor(0.2, 0.2, 0.8); g_sphere[21].setCenter(-3.9, 0.0, -1.0);
    g_sphere[22].setColor(0.2, 0.2, 0.8); g_sphere[22].setCenter(-2.6, 0.0, 0.0);
    g_sphere[23].setColor(0.2, 0.2, 0.8); g_sphere[23].setCenter(-1.3, 0.0, 0.0);
    g_sphere[24].setColor(0.2, 0.2, 0.8); g_sphere[24].setCenter(1.5, 0.0, -6.0);
    g_sphere[25].setColor(0.2, 0.2, 0.8); g_sphere[25].setCenter(-1.5, 0.0, -6.0);
    g_sphere[26].setColor(0.2, 0.2, 0.8); g_sphere[26].setCenter(0.0, 0.0, -2.0);
    g_sphere[27].setColor(0.2, 0.2, 0.8); g_sphere[27].setCenter(-1.3, 0.0, -2.0);
    g_sphere[28].setColor(0.2, 0.2, 0.8); g_sphere[28].setCenter(1.3, 0.0, -2.0);
    g_sphere[29].setColor(0.2, 0.2, 0.8); g_sphere[29].setCenter(2.0, 0.0, -3.0);
    g_sphere[30].setColor(0.2, 0.2, 0.8); g_sphere[30].setCenter(-2.0, 0.0, -3.0);
    g_sphere[31].setColor(0.2, 0.2, 0.8); g_sphere[31].setCenter(0.0, 0.0, -4.2);


    // specify initial colors and center positions of a wall
    g_wall.setColor(0.0, 1.0, 0.0); g_wall.setCenter(0.0, -0.6, 0.0);

    boundary_wall[0].setSize(planeWidth, 1, 0.1);
    boundary_wall[0].setColor(0.0, 0.0, 0.0);
    boundary_wall[1].setSize(planeWidth, 1, 0.1);
    boundary_wall[1].setColor(0.0, 0.0, 0.0);
    boundary_wall[2].setSize(0.1, 1, planeHeight);
    boundary_wall[2].setColor(0.0, 0.0, 0.0);
    boundary_wall[3].setSize(0.1, 1, planeHeight);
    boundary_wall[3].setColor(0.0, 0.0, 0.0);

    boundary_wall[0].setCenter(0.0, 0.0, planeHeight / 2); // 위쪽 가장자리 벽
    boundary_wall[1].setCenter(0.0, 0.0, -(planeHeight / 2)); // 아래쪽 가장자리 벽
    boundary_wall[2].setCenter(planeWidth / 2, 0.0, 0.0); // 오른쪽 가장자리 벽
    boundary_wall[3].setCenter(-(planeWidth / 2), 0.0, 0.0); // 왼쪽 가장자리 벽

}

// 추가해야함 using namespace std; 
int main(int argc, char** argv)
{
    glutInit(&argc, argv); // glut 시작 초기화, 보통 그래픽못쓰는데서 한다거나 하면 오류 감지용
    InitObjects(); // 초기화면, 각 공의 위치를 세팅
    InitGL(); // opengl 관련 초기화
    glutMainLoop(); // 이벤트 루프를 돌리는것, 이벤트별로 콜백함수 등록을 마쳤으니 이벤트 루프로 진입하라.
    return 0;
}