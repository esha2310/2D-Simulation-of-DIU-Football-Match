#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <cstdlib>
#include <string>
using namespace std;

// ─── clouds / bus / road ───
float m = 0, x1 = 0, x2 = -400, o = 0;

// ─── day-night ───
bool  isNight   = false;
float dayTime   = 1.0f;
bool  prevNight = false;

// ─── PHASE ─────
int gamePhase = 0;
bool nightHappened = false;

// ─── ball ───3.4
float bx=250, by=100, bdx=0.6f, bdy=0.3f;
bool  isGoal    = false;
int   goalTimer = 0;
float globalCheer = 0;

// ════ PLAYER BASE POSITIONS ════4
static const float PX[12]={59,120,150,200,250,200, 300,250,350,400,300,465};
static const float PY[12]={105,50,150,80,180,120,  120,50,180,80,150,105};
static const float PDX0[12]={0,0.28f,-0.22f,0.32f,-0.28f,0.22f,
                              -0.32f,0.22f,-0.28f,0.22f,-0.22f,0};
static const float PDY0[12]={0.18f,0.18f,0.28f,-0.22f,0.32f,-0.22f,
                               0.22f,-0.32f,0.28f,0.18f,-0.32f,-0.18f};
int pNum[12]={1,9,10,7,8,11, 1,10,9,7,8,11};

float curPX[12], curPY[12];
float movDX[12], movDY[12];

int  pState[12];
int  pDelay[12]={0,18,36,54,72,90, 0,18,36,54,72,90};
int  transTimer=0;
bool transActive=false;
bool transIsNight=false;
float exitTX[12];

// ════ UNIVERSITY STUDENTS ═object grouping═══5
struct Student {
    float x, y, dx;
    int   gender;
    int   shirtR,shirtG,shirtB;
    int   pantR,pantG,pantB;
};
Student students[3] = {
    {80,  80,  0.22f, 0,  255,255,255,  50,50,150},
    {250, 130,-0.18f, 1,  220,100,150, 100,60,120},
    {400, 60,  0.20f, 0,  200,230,255,  40,80,40}
};

// ═════════════════ GRAPHICS ALGORITHMS (EP1) ═════════════════6

// Algorithm 1: DDA Line Drawing Algorithm 6.1

void drawLineDDA(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = fmaxf(fabsf(dx), fabsf(dy));
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = x1, y = y1;

    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2f(x, y);
        x += xInc;
        y += yInc;
    }
    glEnd();
}

//  Algorithm 2: Midpoint Circle Algorithm 6.2

void drawCircleMidpoint(int r) {
    int x = 0, y = r;
    int p = 1 - r;
    glPointSize(2.0f);
    glBegin(GL_POINTS);

    while(x <= y) {
        glVertex2i(x, y); glVertex2i(-x, y);
        glVertex2i(x, -y); glVertex2i(-x, -y);
        glVertex2i(y, x); glVertex2i(-y, x);
        glVertex2i(y, -x); glVertex2i(-y, -x);

        x++;
        if(p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
    glEnd();
}

// ═════════════════ TRANSITIONS & UTILS ═════════════════ 7

void beginNightExit(){
    transActive=true; transIsNight=true; transTimer=0;
    for(int i=0;i<12;i++){
        pState[i]=0;
        exitTX[i]=(i<6)?-40.0f:540.0f;
    }
}
void tickTransitions(){
    if(!transActive) return;
    transTimer++;
    int doneOut=0, doneIn=0;
    for(int i=0;i<12;i++){
        if(pState[i]==0&&transTimer>=pDelay[i]) pState[i]=1;
        if(pState[i]==2&&transTimer>=pDelay[i]) pState[i]=3;
        if(pState[i]==1){
            float dx=exitTX[i]-curPX[i];
            if(fabsf(dx)>2.0f) curPX[i]+=(dx>0?1.0f:-1.0f)*1.6f;
            else{ curPX[i]=exitTX[i]; pState[i]=2; }
        }
        if(pState[i]==3){
            float dx=PX[i]-curPX[i], dy=PY[i]-curPY[i];
            float dist=sqrtf(dx*dx+dy*dy);
            if(dist>2.0f){ curPX[i]+=dx/dist*1.6f; curPY[i]+=dy/dist*0.6f; }
            else{
                curPX[i]=PX[i]; curPY[i]=PY[i];
                movDX[i]=PDX0[i]; movDY[i]=PDY0[i];
                pState[i]=0;
            }
        }
        if(pState[i]==2) doneOut++;
        if(pState[i]==0) doneIn++;
    }
    if(transIsNight&&doneOut==12)  transActive=false;
    if(!transIsNight&&doneIn==12)  transActive=false;
}
bool allOnField(){
    for(int i=0;i<12;i++) if(pState[i]!=0) return false;
    return true;
}
//---------------------keyboard input function------------8
void myInit(){
    glClearColor(0.5f,0,0,1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0,500,0,500,-10,10);
}
void mouse(int btn,int st,int mx,int my){
    if(btn==GLUT_LEFT_BUTTON&&st==GLUT_DOWN){
        float ox=mx/2.0f, oy=500-(my/2.0f);
        if(ox>=380&&ox<=490&&oy>=450&&oy<=480) isNight=!isNight;
    }
}
void keyboard(unsigned char k,int,int){
    if(k=='n'||k=='N'||k=='d'||k=='D'||k==' ') isNight=!isNight;
}

// ═════════════════ PRIMITIVES & MODULES ═════════════════ 9

void circ(float x,float y,float r,int R,int G,int B){
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(R,G,B); glVertex2f(x,y);
    for(float a=0;a<=360;a+=0.5f)
        glVertex2f(x+sinf(a*3.1416f/180)*r, y+cosf(a*3.1416f/180)*r);
    glEnd();
}

void sunDraw(float r,float dt){
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255,(int)(10+200*dt),0); glVertex2f(0,0);
    for(int a=0;a<=360;a++){
        glColor3ub((int)(240*dt+255*(1-dt)),(int)(215*dt+50*(1-dt)),0);
        glVertex2f(r*sinf(a*3.1416f/180), r*cosf(a*3.1416f/180));
    }
    glEnd();
}

//--------------bus wheel (alternative color spokes)
void wheel(float r){
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(100,100,100); glVertex2f(0,0);
    for(int a=0;a<=360;a+=15){
        if(a%30==0) glColor3ub(50,50,50); else glColor3ub(255,255,255);
        glVertex2f(r*sinf(a*3.1416f/180), r*cosf(a*3.1416f/180));
    }
    glEnd();
}

// ═════════════════ TRANSFORMATION FUNCTIONS  ═════════════════ 10

//  Modular Drawing: Bus 1 using Matrix Transformations
void drawBus1(float xPos) {
    glPushMatrix();
    glTranslatef(xPos, 0, 0); //  Translation

    glColor3ub(56,132,63);
    glBegin(GL_POLYGON);glVertex2d(250,230);glVertex2d(250,240);glVertex2d(256,255);glVertex2d(310,255);glVertex2d(310,230);glEnd();
    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);glVertex2d(308,253);glVertex2d(308,243);glVertex2d(265,243);glVertex2d(265,253);glEnd();

    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);glVertex2d(267,251);glVertex2d(267,245);glVertex2d(275,245);glVertex2d(275,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(277,251);glVertex2d(277,245);glVertex2d(285,245);glVertex2d(285,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(287,251);glVertex2d(287,245);glVertex2d(295,245);glVertex2d(295,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(297,251);glVertex2d(297,245);glVertex2d(305,245);glVertex2d(305,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(258,247);glVertex2d(258,233);glVertex2d(264,233);glVertex2d(264,247);glEnd();

    //  Wheels with Translation + Rotation
    glPushMatrix();
    glTranslatef(266, 227, 0);
    glRotatef(-xPos * 3.0f, 0, 0, 1); // Wheel 1 Rotation
    wheel(4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(296, 227, 0);
    glRotatef(-xPos * 3.0f, 0, 0, 1); // Wheel 2 Rotation
    wheel(4);
    glPopMatrix();

    glColor3ub(200,20,20);
    glBegin(GL_POLYGON);glVertex2d(272,241);glVertex2d(272,233);glVertex2d(296,233);glVertex2d(296,241);glEnd();
    glColor3ub(255,255,255);glRasterPos2f(274,235);
    string s="D I U"; for(char c:s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,c);

    glPopMatrix();
}

//  Modular Drawing: Bus 2 using Matrix Transformations
void drawBus2(float xPos) {
    glPushMatrix();
    glTranslatef(xPos, 0, 0); //  Translation

    glColor3ub(56,132,63);
    glBegin(GL_POLYGON);glVertex2d(370,230);glVertex2d(430,230);glVertex2d(430,240);glVertex2d(424,255);glVertex2d(370,255);glEnd();
    glColor3ub(1,1,1);
    glBegin(GL_POLYGON);glVertex2d(372,243);glVertex2d(415,243);glVertex2d(415,253);glVertex2d(372,253);glEnd();

    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);glVertex2d(375,251);glVertex2d(375,245);glVertex2d(383,245);glVertex2d(383,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(385,251);glVertex2d(385,245);glVertex2d(393,245);glVertex2d(393,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(395,251);glVertex2d(395,245);glVertex2d(403,245);glVertex2d(403,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(405,251);glVertex2d(405,245);glVertex2d(413,245);glVertex2d(413,251);glEnd();
    glBegin(GL_POLYGON);glVertex2d(416,247);glVertex2d(416,233);glVertex2d(422,233);glVertex2d(422,247);glEnd();

    //  Wheels with Translation + Rotation
    glPushMatrix();
    glTranslatef(384, 227, 0);
    glRotatef(xPos * 3.0f, 0, 0, 1);
    wheel(4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(414, 227, 0);
    glRotatef(xPos * 3.0f, 0, 0, 1);
    wheel(4);
    glPopMatrix();

    glColor3ub(200,20,20);
    glBegin(GL_POLYGON);glVertex2d(376,241);glVertex2d(376,233);glVertex2d(400,233);glVertex2d(400,241);glEnd();
    glColor3ub(255,255,255);glRasterPos2f(378,235);
    string s="D I U"; for(char c:s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,c);

    glPopMatrix();
}

// ════════════════════════════════════════════════════character grawing function 11

void drawPlayer(float x,float y,int team,int num){
    circ(x,y+17,4,255,160,122);
    if(team==1) glColor3ub(255,255,0);
    else glColor3ub(135,206,235);
    glBegin(GL_POLYGON);glVertex2d(x-5,y+5);glVertex2d(x+5,y+5);glVertex2d(x+6,y+14);glVertex2d(x-6,y+14);glEnd();
    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);glVertex2d(x-5,y);glVertex2d(x+5,y);glVertex2d(x+5,y+5);glVertex2d(x-5,y+5);glEnd();
    glColor3ub(255,160,122); glLineWidth(2);
    glBegin(GL_LINES);glVertex2d(x-2,y);glVertex2d(x-3,y-6);glVertex2d(x+2,y);glVertex2d(x+3,y-6);
    glVertex2d(x-6,y+13);glVertex2d(x-9,y+5);glVertex2d(x+6,y+13);glVertex2d(x+9,y+5);glEnd();
    glColor3ub(0,0,0);
    char buf[4]; sprintf(buf,"%d",num);
    glRasterPos2f(num<10?x-2.5f:x-4.5f,y+7);
    for(int i=0;buf[i];i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,buf[i]);
}

void drawSpectator(float x,float y,bool goal){
    float jy=goal?y+sinf(globalCheer)*16.0f:y; //lafalafi
    circ(x,jy+17,4,255,160,122);
    glColor3ub(148,0,211);
    glBegin(GL_POLYGON);glVertex2d(x-5,jy+5);glVertex2d(x+5,jy+5);glVertex2d(x+6,jy+14);glVertex2d(x-6,jy+14);glEnd();
    glColor3ub(50,50,50);
    glBegin(GL_POLYGON);glVertex2d(x-5,jy);glVertex2d(x+5,jy);glVertex2d(x+5,jy+5);glVertex2d(x-5,jy+5);glEnd();
    glColor3ub(255,160,122); glLineWidth(2);
    glBegin(GL_LINES);glVertex2d(x-2,jy);glVertex2d(x-3,jy-6);glVertex2d(x+2,jy);glVertex2d(x+3,jy-6);glEnd();
    glBegin(GL_LINES);
    if(goal){ glVertex2d(x-6,jy+13);glVertex2d(x-13,jy+24);glVertex2d(x+6,jy+13);glVertex2d(x+13,jy+24); }
    else { glVertex2d(x-6,jy+13);glVertex2d(x-10,jy+5);glVertex2d(x+6,jy+13);glVertex2d(x+10,jy+5); }
    glEnd();
}

void drawStudent(float x,float y,int gender, int sR,int sG,int sB, int pR,int pG,int pB){
    float t = glutGet(GLUT_ELAPSED_TIME)*0.002f; // animation
    float legSwing = sinf(t + x*0.05f)*4.0f;
    circ(x,y+17,4,255,190,140);
    if(gender==0) circ(x,y+20,3,60,30,10); else circ(x,y+20,3.5f,30,10,10);
    glColor3ub(sR,sG,sB); glBegin(GL_POLYGON);glVertex2d(x-5,y+5);glVertex2d(x+5,y+5);glVertex2d(x+6,y+14);glVertex2d(x-6,y+14);glEnd();
    if(gender==0){
        glColor3ub(pR,pG,pB); glBegin(GL_POLYGON);glVertex2d(x-5,y-1);glVertex2d(x+5,y-1);glVertex2d(x+5,y+5);glVertex2d(x-5,y+5);glEnd();
    } else {
        glColor3ub(pR,pG,pB); glBegin(GL_POLYGON);glVertex2d(x-6,y-3);glVertex2d(x+6,y-3);glVertex2d(x+5,y+5);glVertex2d(x-5,y+5);glEnd();
        glColor3ub(sR+30,sG+30,sB+30); glLineWidth(1.5f); glBegin(GL_LINES);glVertex2d(x-5,y+13);glVertex2d(x-9,y+6);glEnd();
    }
    glColor3ub(200,160,110); glLineWidth(2);
    glBegin(GL_LINES);glVertex2d(x-2,y-1);glVertex2d(x-2+legSwing,y-9);glVertex2d(x+2,y-1);glVertex2d(x+2-legSwing,y-9);glEnd();
    glColor3ub(255,190,140); glLineWidth(2);
    glBegin(GL_LINES);glVertex2d(x-5,y+13);glVertex2d(x-8-legSwing*0.4f,y+6);glVertex2d(x+5,y+13);glVertex2d(x+8+legSwing*0.4f,y+6);glEnd();
    glColor3ub(80,50,20); glLineWidth(1.5f); glBegin(GL_LINES);glVertex2d(x+5,y+13);glVertex2d(x+7,y+4);glEnd();
    glColor3ub(50,80,160); glBegin(GL_POLYGON);glVertex2d(x+6,y+4);glVertex2d(x+12,y+4);glVertex2d(x+12,y+11);glVertex2d(x+6,y+11);glEnd();
    glColor3ub(255,255,255); glLineWidth(1.0f); glBegin(GL_LINES);glVertex2d(x+7,y+8);glVertex2d(x+11,y+8);glEnd();
}

void drawBall(float x,float y){
    circ(x,y,4,255,255,255);
    circ(x,y,1.5f,0,0,0);
}

// ════════════════════ DISPLAY FUNCTION ════════════════════ 12

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky
    glBegin(GL_QUADS);
    glColor3ub((int)(255*dayTime),(int)(255*dayTime),(int)(255*dayTime+30*(1-dayTime)));
    glVertex2f(500,250);glVertex2f(0,250);
    glColor3ub((int)(101*dayTime),(int)(193*dayTime),(int)(246*dayTime+15*(1-dayTime)));
    glVertex2f(0,500);glVertex2f(500,500);
    glEnd();

    //  Sun Object Scaling and Translation
    if(dayTime>0.05f) {
        glPushMatrix();
        glTranslatef(350, 150 + dayTime * 200, 0); // Position
        glScalef(dayTime + 0.3f, dayTime + 0.3f, 1.0f); // Scaling the sun as it sets
        sunDraw(30, dayTime);
        glPopMatrix();
    }

    // Clouds
    float cx[]={100,110,90,100,180,190,170,180,260,270,250,260};
    float cy[]={425,430,430,440,445,450,450,460,445,450,450,460};
    for(int i=0;i<12;i++) circ(m+cx[i],cy[i],10,255,255,255);

    // Background Layers
    glColor3ub(51,255,51);
    glBegin(GL_QUADS);glVertex2d(100,220);glVertex2d(100,100);glVertex2d(0,100);glVertex2d(0,220);glEnd();
    glColor3ub(251,255,203);
    glBegin(GL_POLYGON);glVertex2d(0,50);glVertex2d(0,100);glVertex2d(75,230);glVertex2d(100,230);glEnd();
    glBegin(GL_POLYGON);glVertex2d(0,220);glVertex2d(0,250);glVertex2d(500,250);glVertex2d(500,220);glEnd();

    // Mosque
    glColor3ub(191,191,191);glBegin(GL_POLYGON);glVertex2d(400,250);glVertex2d(460,250);glVertex2d(490,330);glVertex2d(370,330);glEnd();
    glColor3ub(1,1,1);glBegin(GL_POLYGON);glVertex2d(405,310);glVertex2d(455,310);glVertex2d(455,317);glVertex2d(405,317);glEnd();
    glColor3ub(88,9,9);glBegin(GL_POLYGON);glVertex2d(405,300);glVertex2d(455,300);glVertex2d(455,250);glVertex2d(405,250);glEnd();
    glColor3ub(191,191,191);glBegin(GL_POLYGON);glVertex2d(415,250);glVertex2d(445,250);glVertex2d(445,290);glVertex2d(415,290);glEnd();
    if(dayTime<0.3f) glColor3ub(255,255,100);
    else glColor3ub(0,150,250);
    glBegin(GL_POLYGON);glVertex2d(420,250);glVertex2d(440,250);glVertex2d(440,275);glVertex2d(420,275);glEnd();
    glBegin(GL_POLYGON);glVertex2d(440,275);glVertex2d(420,275);glVertex2d(430,280);glEnd();
    glColor3ub(1,1,1);glBegin(GL_POLYGON);glVertex2d(364,329);glVertex2d(496,329);glVertex2d(496,341);glVertex2d(364,341);glEnd();
    glColor3ub(191,191,191);glBegin(GL_POLYGON);glVertex2d(365,330);glVertex2d(495,330);glVertex2d(495,340);glVertex2d(365,340);glEnd();

    // Buildings
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(60,250);glVertex2d(130,250);glVertex2d(130,400);glVertex2d(60,400);glEnd();
    glColor3ub(188,56,56);
    int wx[]={65,100,65,100,65,100,65,100,65,100,65,100};
    int wy[]={395,395,370,370,345,345,320,320,295,295,270,270};
    for(int i=0;i<12;i++){glBegin(GL_POLYGON);glVertex2d(wx[i],wy[i]);glVertex2d(wx[i],wy[i]-20);glVertex2d(wx[i]+20,wy[i]-20);glVertex2d(wx[i]+20,wy[i]);glEnd();}
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(130,250);glVertex2d(180,250);glVertex2d(180,390);glVertex2d(130,400);glEnd();
    glColor3ub(1,1,1);glBegin(GL_LINES);glVertex2d(130,375);glVertex2d(180,368);glEnd();
    glColor3ub(127,16,16);
    glBegin(GL_POLYGON);glVertex2d(135,395);glVertex2d(135,380);glVertex2d(150,377);glVertex2d(150,391);glEnd();
    glBegin(GL_POLYGON);glVertex2d(160,390);glVertex2d(160,377);glVertex2d(175,374);glVertex2d(175,386);glEnd();
    glBegin(GL_LINES);glVertex2d(130,350);glVertex2d(180,343);glEnd();
    glBegin(GL_POLYGON);glVertex2d(135,370);glVertex2d(135,355);glVertex2d(150,352);glVertex2d(150,366);glEnd();
    glBegin(GL_POLYGON);glVertex2d(160,365);glVertex2d(160,350);glVertex2d(175,347);glVertex2d(175,361);glEnd();
    glColor3ub(180,56,56);glBegin(GL_POLYGON);glVertex2d(130,250);glVertex2d(130,345);glVertex2d(165,340);glVertex2d(177,335);glVertex2d(177,250);glEnd();
    glColor3ub(0,102,204);glRasterPos2f(133,315);
    {string s="Daffodil";for(char c:s)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,c);}
    glColor3ub(200,200,200);glRasterPos2f(133,305);
    {string s="International";for(char c:s)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,c);}
    glColor3ub(51,204,51);glRasterPos2f(133,295);{string s="University";for(char c:s)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,c);}
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(180,250);glVertex2d(266,250);glVertex2d(266,390);glVertex2d(180,390);glEnd();
    glColor3ub(188,56,56);
    int bwx[]={185,227,185,227,185,227,185,227,185,227};
    int bwy[]={385,385,363,363,338,338,312,312,286,286};
    for(int i=0;i<10;i++){glBegin(GL_POLYGON);glVertex2d(bwx[i],bwy[i]);glVertex2d(bwx[i],bwy[i]-15);glVertex2d(bwx[i]+35,bwy[i]-15);glVertex2d(bwx[i]+35,bwy[i]);glEnd();}
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(266,250);glVertex2d(266,367);glVertex2d(293,367);glVertex2d(293,250);glEnd();
    glColor3ub(188,56,56);
    {int h[]={363,338,312,286};for(int i=0;i<4;i++){glBegin(GL_POLYGON);glVertex2d(269,h[i]);glVertex2d(269,h[i]-15);glVertex2d(289,h[i]-15);glVertex2d(289,h[i]);glEnd();}}
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(293,250);glVertex2d(293,343);glVertex2d(320,343);glVertex2d(320,250);glEnd();
    glColor3ub(188,56,56);
    {int h[]={338,312,286};for(int i=0;i<3;i++){glBegin(GL_POLYGON);glVertex2d(296,h[i]);glVertex2d(296,h[i]-15);glVertex2d(316,h[i]-15);glVertex2d(316,h[i]);glEnd();}}
    glColor3ub(233,233,233);glBegin(GL_POLYGON);glVertex2d(316,250);glVertex2d(316,316);glVertex2d(348,316);glVertex2d(348,250);glEnd();
    glColor3ub(188,56,56);
    glBegin(GL_POLYGON);glVertex2d(323,312);glVertex2d(323,297);glVertex2d(343,297);glVertex2d(343,312);glEnd();
    glBegin(GL_POLYGON);glVertex2d(323,286);glVertex2d(323,271);glVertex2d(343,271);glVertex2d(343,286);glEnd();

    // Field Background
    glColor3ub(60,185,72);
    glBegin(GL_POLYGON);glVertex2d(0,50);glVertex2d(0,0);glVertex2d(500,0);glVertex2d(500,220);glVertex2d(93,220);glEnd();

    // Field Lines
    glColor3ub(255,255,255);

    // [EP1] Using custom DDA Algorithm for the Middle Line instead of standard glBegin(GL_LINES)
    drawLineDDA(296, 205, 230, 0);

    // Normal GL_LINES for other boundaries
    glBegin(GL_LINES);
    glVertex2d(108,205);glVertex2d(8,0); glVertex2d(485,205);glVertex2d(108,205);
    glVertex2d(470,0);glVertex2d(485,205); glVertex2d(28,40);glVertex2d(68,40);
    glVertex2d(68,40);glVertex2d(126,160); glVertex2d(126,160);glVertex2d(86,160);
    glVertex2d(473,40);glVertex2d(433,40); glVertex2d(433,40);glVertex2d(442,160);
    glVertex2d(442,160);glVertex2d(482,160);
    glEnd();

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d(47,80);glVertex2d(47,110); glVertex2d(71,130);glVertex2d(71,160); glVertex2d(47,110);glVertex2d(71,160);
    glVertex2d(476,80);glVertex2d(476,110); glVertex2d(480,130);glVertex2d(480,160); glVertex2d(476,110);glVertex2d(480,160);
    glEnd();
    glLineWidth(1);
// Midpoint Circle Algorithm + Scaling Transformation for Ellipse **************************
    glPushMatrix();
    glTranslatef(263, 102, 0);         // Translate to center
    glScalef(1.0f, 15.0f/35.0f, 1.0f); // Scale to form Ellipse shape
    drawCircleMidpoint(35);            // Custom Algorithm
    glPopMatrix();

    // Trees
    for(int i=0,tx=122;i<13;i++,tx+=30){
        glColor3ub(153,0,0);glBegin(GL_POLYGON);glVertex2d(tx,210);glVertex2d(tx,217);glVertex2d(tx+2,217);glVertex2d(tx+2,210);glEnd();
        glColor3ub(0,102,0);
        glBegin(GL_POLYGON);glVertex2d(tx-5,216);glVertex2d(tx+1,225);glVertex2d(tx+7,216);glEnd();
        glBegin(GL_POLYGON);glVertex2d(tx-5,220);glVertex2d(tx+1,229);glVertex2d(tx+7,220);glEnd();
    }
    if(m<250)m+=0.3f; else m=-50;

    // ── phase / transition logic ──────────────────
    bool jN=(isNight&&!prevNight), jD=(!isNight&&prevNight);
    prevNight=isNight;

    if(jN && gamePhase==0){
        nightHappened=true; gamePhase=1; beginNightExit();
    }
    if(jD && gamePhase==1){
        gamePhase=2;
        for(int i=0;i<12;i++) pState[i]=2;
        transActive=false;
    }

    tickTransitions();

    // ── day content ──────────────────────────────
    if(dayTime>0.1f){
        // Road Person
        glColor3ub(255,160,122);glLineWidth(4);
        glBegin(GL_LINES);glVertex2d(o+264,260);glVertex2d(o+256,257);glEnd();
        glBegin(GL_LINES);glVertex2d(o+266,260);glVertex2d(o+275,257);glEnd();
        glColor3ub(197,30,255);glBegin(GL_POLYGON);glVertex2d(o+260,250);glVertex2d(o+270,250);glVertex2d(o+266,265);glVertex2d(o+264,265);glEnd();
        circ(o+265,267,3,0,0,0);
        glColor3ub(149,149,149);
        glBegin(GL_POLYGON);glVertex2d(o+260,250);glVertex2d(o+265,250);glVertex2d(o+260,240);glEnd();
        glBegin(GL_POLYGON);glVertex2d(o+265,250);glVertex2d(o+270,250);glVertex2d(o+268,240);glEnd();
        glColor3ub(0,0,105);glLineWidth(2);
        glBegin(GL_LINES);glVertex2d(o+263,240);glVertex2d(o+258,240);glEnd();
        glBegin(GL_LINES);glVertex2d(o+270,240);glVertex2d(o+265,240);glEnd();
        if(o<250)o+=0.2f; else o=-250;

        // Phase 0: Game
        if(gamePhase==0){
            globalCheer+=0.4f; if(globalCheer>360)globalCheer=0;
            bool cel=(isGoal&&goalTimer>0);
            drawSpectator(25, 90, cel); drawSpectator(40,140, cel);
            drawSpectator(485,90, cel); drawSpectator(490,140,cel);

            for(int i=0;i<12;i++){
                if(pState[i]==2) continue;
                drawPlayer(curPX[i],curPY[i],(i<6)?1:2,pNum[i]);
            }

            if(allOnField()){
                drawBall(bx,by);
                if(isGoal){
                    goalTimer--;
                    bdx*=0.90f; bdy*=0.90f; bx+=bdx; by+=bdy;
                    glColor3ub(255,220,0); glRasterPos2f(196,120);
                    {string s="GOAL!!!";for(char c:s)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,c);}
                    if(goalTimer<=0){isGoal=false;bx=250;by=100;bdx=0.6f;bdy=0.3f;}
                } else {
                    bdx*=0.987f; bdy*=0.987f;
                    if(fabsf(bdx)<0.12f&&fabsf(bdy)<0.12f) bdx=(rand()%2==0)?0.5f:-0.5f;
                    bx+=bdx; by+=bdy;
                    if(by<4){by=4;bdy=fabsf(bdy);}
                    if(by>208){by=208;bdy=-fabsf(bdy);}
                    if(!isGoal&&bx-4<72&&by>75&&by<165){
                        isGoal=true;goalTimer=150;bdx=fabsf(bdx)*0.25f;bdy*=0.25f;bx=62;
                    } else if(!isGoal&&bx+4>474&&by>75&&by<165){
                        isGoal=true;goalTimer=150;bdx=-fabsf(bdx)*0.25f;bdy*=0.25f;bx=478;
                    } else {
                        if(bx<8){bx=8;bdx=fabsf(bdx);}
                        if(bx>492){bx=492;bdx=-fabsf(bdx);}
                    }
                    for(int i=0;i<12;i++){
                        float d=sqrtf((bx-curPX[i])*(bx-curPX[i])+(by-curPY[i])*(by-curPY[i]));
                        if(d<13){ bdx=(i<6)?(1.4f+(rand()%8)/10.0f):(-1.4f-(rand()%8)/10.0f); bdy=((rand()%20)-10)/12.0f; }
                    }
                }
                for(int i=0;i<12;i++){
                    curPX[i]+=movDX[i]; curPY[i]+=movDY[i];
                    if(i==0||i==11){
                        if(curPY[i]<78||curPY[i]>132) movDY[i]=-movDY[i];
                    } else {
                        if(curPX[i]<78 ||curPX[i]>452) movDX[i]=-movDX[i];
                        if(curPY[i]<18 ||curPY[i]>188) movDY[i]=-movDY[i];
                    }
                }
            }
        }
        else if(gamePhase==1){
            for(int i=0;i<12;i++){
                if(pState[i]==2) continue;
                drawPlayer(curPX[i],curPY[i],(i<6)?1:2,pNum[i]);
            }
        }
        else if(gamePhase==2){
            for(int i=0;i<3;i++){
                drawStudent(students[i].x, students[i].y, students[i].gender, students[i].shirtR, students[i].shirtG, students[i].shirtB, students[i].pantR, students[i].pantG, students[i].pantB);
                students[i].x += students[i].dx;
                if(students[i].x < 30 || students[i].x > 470) students[i].dx = -students[i].dx;
            }
        }
    }

    // ───  Modular Bus Rendering with Transformations ───
    drawBus1(x1);
    drawBus2(x2);

    float bmod=(dayTime<0.3f)?0.3f:1.0f;
    if(x1>-330)x1-=0.7f*bmod; else x1=260;
    if(x2<150) x2+=0.8f*bmod; else x2=-450;

    // Night overlay & Lights
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0,0,20,(int)((1-dayTime)*200));
    glBegin(GL_QUADS);glVertex2f(0,0);glVertex2f(500,0);glVertex2f(500,500);glVertex2f(0,500);glEnd();

    if(dayTime<0.3f){
        glColor4ub(255,255,150,140);
        glBegin(GL_POLYGON);glVertex2d(x1+250,240);glVertex2d(x1+150,270);glVertex2d(x1+150,210);glVertex2d(x1+250,230);glEnd();
        glBegin(GL_POLYGON);glVertex2d(x2+430,240);glVertex2d(x2+530,270);glVertex2d(x2+530,210);glVertex2d(x2+430,230);glEnd();
    }
    glDisable(GL_BLEND);

    // Day/Night Button
    glColor3ub(200,200,200);
    glBegin(GL_POLYGON);glVertex2d(380,450);glVertex2d(490,450);glVertex2d(490,480);glVertex2d(380,480);glEnd();
    glColor3ub(0,0,0);glRasterPos2f(395,460);
    {string s=isNight?"Make Day":"Make Night";for(char c:s)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,c);}

    glFlush();
}

void update(int){
    if(isNight &&dayTime>0)dayTime-=0.005f;
    if(!isNight&&dayTime<1)dayTime+=0.005f;
    dayTime=fmaxf(0,fminf(1,dayTime));
    glutPostRedisplay();
    glutTimerFunc(15,update,0);
}

int main(int argc,char**argv){
    srand(42);
    for(int i=0;i<12;i++){
        curPX[i]=PX[i]; curPY[i]=PY[i];
        movDX[i]=PDX0[i]; movDY[i]=PDY0[i];
        pState[i]=0;
    }
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(1000,1000);
    glutInitWindowPosition(100,0);
    glutCreateWindow("DIU Football Match - Graphics Algorithms & Transformations(1093,1084,1322,1095)");
    myInit();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(25,update,0);
    update(0);
    glutMainLoop();
}
