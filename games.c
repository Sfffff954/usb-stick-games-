#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "doom_builtin.h"
#include "racing.h"
#include "shooter.h"
#include "asteroids.h"
#include "pacman.h"
#include "worldmap_real.h"
#include "downhill.h"

#define TAB_SNAKE 0
#define TAB_PONG 1
#define TAB_BREAKOUT 2
#define TAB_TETRIS 3
#define TAB_DOOM 4
#define TAB_RACE 5
#define TAB_SHOOTER 6
#define TAB_ASTEROIDS 7
#define TAB_PACMAN 8
#define TAB_WORLDMAP 9
#define TAB_DOWNHILL 10
#define NUM_TABS 11
#define ID_TAB 200
#define ID_STATUS 210
#define TIMER_SNAKE 1
#define TIMER_PONG 2
#define TIMER_BREAKOUT 3
#define TIMER_TETRIS 4
#define TIMER_DOOM 5
#define TIMER_RACE 6
#define TIMER_SHOOTER 7
#define TIMER_ASTEROIDS 8
#define TIMER_PACMAN 9
#define TIMER_WORLDMAP 10
#define TIMER_DOWNHILL 11
#define C_BG RGB(14,14,20)
#define C_BDR RGB(44,44,60)
#define C_ACC RGB(60,150,255)
#define C_TXT RGB(220,220,230)
#define C_DIM RGB(90,90,110)
#define C_RED RGB(230,60,60)
#define C_YEL RGB(240,200,40)

static HWND g_hwnd=NULL,hTab=NULL,hStatus=NULL;
static HWND hPane[NUM_TABS];
static int g_curtab=0;
static HWND hSnW=NULL,hPnW=NULL,hBrW=NULL,hTtW=NULL;
static HWND hDoomWnd=NULL,hRaceWnd=NULL,hSSWnd=NULL,hAstWnd=NULL,hPMWnd=NULL,hWMWnd=NULL,hDHWnd=NULL;
static DWORD g_race_last=0;

static void fillR(HDC h,int x,int y,int w,int v,COLORREF c){HBRUSH b=CreateSolidBrush(c);RECT r={x,y,x+w,y+v};FillRect(h,&r,b);DeleteObject(b);}
static void drawTxt(HDC h,int x,int y,int w,int v,const char*s,COLORREF c,int bold){SetTextColor(h,c);SetBkMode(h,TRANSPARENT);HFONT f=CreateFontA(15,0,0,0,bold?FW_BOLD:FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");HFONT o=(HFONT)SelectObject(h,f);RECT r={x,y,x+w,y+v};DrawTextA(h,s,-1,&r,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);SelectObject(h,o);DeleteObject(f);}
static void drawCentTxt(HDC h,int x,int y,int w,int v,const char*s,COLORREF c,int bold){SetTextColor(h,c);SetBkMode(h,TRANSPARENT);HFONT f=CreateFontA(16,0,0,0,bold?FW_BOLD:FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");HFONT o=(HFONT)SelectObject(h,f);RECT r={x,y,x+w,y+v};DrawTextA(h,s,-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);SelectObject(h,o);DeleteObject(f);}
static void borderR(HDC h,int x,int y,int w,int v,COLORREF c){HPEN p=CreatePen(PS_SOLID,1,c);HPEN op=(HPEN)SelectObject(h,p);SelectObject(h,GetStockObject(NULL_BRUSH));Rectangle(h,x,y,x+w,y+v);SelectObject(h,op);DeleteObject(p);}
static void drawCirc(HDC h,int cx,int cy,int r,COLORREF c){HBRUSH b=CreateSolidBrush(c);HPEN p=CreatePen(PS_SOLID,1,c);HBRUSH ob=(HBRUSH)SelectObject(h,b);HPEN op=(HPEN)SelectObject(h,p);Ellipse(h,cx-r,cy-r,cx+r,cy+r);SelectObject(h,ob);SelectObject(h,op);DeleteObject(b);DeleteObject(p);}
static void drawLine(HDC h,int x1,int y1,int x2,int y2,COLORREF c,int dash){HPEN p=CreatePen(dash?PS_DASH:PS_SOLID,1,c);HPEN op=(HPEN)SelectObject(h,p);MoveToEx(h,x1,y1,NULL);LineTo(h,x2,y2);SelectObject(h,op);DeleteObject(p);}
static void drawGrid(HDC h,int cols,int rows,int cell,COLORREF c){for(int x=0;x<=cols;x++)drawLine(h,x*cell,0,x*cell,rows*cell,c,0);for(int y=0;y<=rows;y++)drawLine(h,0,y*cell,cols*cell,y*cell,c,0);}
static void overlayMsg(HDC hdc,int W,int H,const char*l1,const char*l2){fillR(hdc,W/2-130,H/2-30,260,60,RGB(20,20,30));borderR(hdc,W/2-130,H/2-30,260,60,C_ACC);drawCentTxt(hdc,W/2-120,H/2-28,240,30,l1,C_ACC,1);if(l2&&l2[0])drawCentTxt(hdc,W/2-120,H/2+2,240,24,l2,C_DIM,0);}

/* SNAKE */
#define SN_C 16
#define SN_W 32
#define SN_H 22
typedef struct{short x,y;}Pt;
static Pt snB[600];static int snL=0,snDx=1,snDy=0,snNx=1,snNy=0;static Pt snF;static int snSc=0,snBest=0,snLvl=1;static BOOL snOn=FALSE;
static void snFood(void){while(1){int fx=rand()%SN_W,fy=rand()%SN_H;BOOL ok=TRUE;for(int i=0;i<snL;i++)if(snB[i].x==fx&&snB[i].y==fy){ok=FALSE;break;}if(ok){snF.x=fx;snF.y=fy;return;}}}
static void snReset(void){snL=4;snDx=1;snDy=0;snNx=1;snNy=0;snSc=0;snLvl=1;for(int i=0;i<snL;i++){snB[i].x=SN_W/2-i;snB[i].y=SN_H/2;}snFood();snOn=TRUE;}
static void snStep(void){if(!snOn)return;snDx=snNx;snDy=snNy;int hx=snB[0].x+snDx,hy=snB[0].y+snDy;if(hx<0||hx>=SN_W||hy<0||hy>=SN_H){snOn=FALSE;if(snSc>snBest)snBest=snSc;return;}for(int i=1;i<snL;i++)if(snB[i].x==hx&&snB[i].y==hy){snOn=FALSE;if(snSc>snBest)snBest=snSc;return;}for(int i=snL-1;i>0;i--)snB[i]=snB[i-1];snB[0].x=hx;snB[0].y=hy;if(hx==snF.x&&hy==snF.y){if(snL<599)snL++;snSc+=10*snLvl;if(snSc>=snLvl*100)snLvl++;snFood();}}
static void snDraw(HDC hdc,int W,int H){fillR(hdc,0,0,W,H,C_BG);drawGrid(hdc,SN_W,SN_H,SN_C,C_BDR);drawCirc(hdc,snF.x*SN_C+SN_C/2,snF.y*SN_C+SN_C/2,SN_C/2-2,C_RED);for(int i=0;i<snL;i++){int g2=max(60,255-i*4);COLORREF c=i==0?C_ACC:RGB(20,g2/2,g2);fillR(hdc,snB[i].x*SN_C+1,snB[i].y*SN_C+1,SN_C-2,SN_C-2,c);}char buf[64];snprintf(buf,63,"Punkte:%d  Rekord:%d  Level:%d",snSc,snBest,snLvl);drawTxt(hdc,4,SN_H*SN_C+4,400,18,buf,C_TXT,0);if(!snOn){char l1[48];if(snL==4&&snSc==0)strcpy(l1,"LEERTASTE = Starten");else snprintf(l1,47,"GAME OVER  %d Punkte",snSc);overlayMsg(hdc,SN_W*SN_C,SN_H*SN_C,l1,"LEERTASTE = Neu");}}

/* PONG */
#define PN_W 480
#define PN_H 300
#define PN_PW 10
#define PN_PH 56
static float pnP1=122,pnP2=122,pnBx=240,pnBy=150,pnVx=3.5f,pnVy=2.5f;static int pnYou=0,pnAI=0;static BOOL pnOn=FALSE;static BOOL pnKeys[256]={0};
static void pnRstBall(float d){pnBx=PN_W/2;pnBy=PN_H/2;pnVx=3.5f*d;pnVy=2.5f*(rand()%2?1:-1);}
static void pnReset(void){pnP1=PN_H/2-PN_PH/2;pnP2=pnP1;pnYou=0;pnAI=0;pnOn=TRUE;pnRstBall(rand()%2?1:-1);}
static void pnStep(void){if(!pnOn)return;if(pnKeys['W']||pnKeys[VK_UP]){pnP1-=5;if(pnP1<0)pnP1=0;}if(pnKeys['S']||pnKeys[VK_DOWN]){pnP1+=5;if(pnP1>PN_H-PN_PH)pnP1=PN_H-PN_PH;}float aim=pnBy-PN_PH/2;pnP2+=(aim-pnP2)*0.09f;if(pnP2<0)pnP2=0;if(pnP2>PN_H-PN_PH)pnP2=PN_H-PN_PH;pnBx+=pnVx;pnBy+=pnVy;if(pnBy<5||pnBy>PN_H-5)pnVy*=-1;if(pnBx<PN_PW+9&&pnBy>=pnP1&&pnBy<=pnP1+PN_PH){pnVx=fabsf(pnVx)+.15f;pnVy+=(pnBy-(pnP1+PN_PH/2))*.1f;}if(pnBx>PN_W-PN_PW-9&&pnBy>=pnP2&&pnBy<=pnP2+PN_PH)pnVx=-(fabsf(pnVx)+.1f);if(pnBx<0){pnAI++;pnRstBall(1);}if(pnBx>PN_W){pnYou++;pnRstBall(-1);}if(pnYou>=7||pnAI>=7)pnOn=FALSE;}
static void pnDraw(HDC hdc){fillR(hdc,0,0,PN_W,PN_H,C_BG);drawLine(hdc,PN_W/2,0,PN_W/2,PN_H,C_BDR,1);fillR(hdc,4,(int)pnP1,PN_PW,PN_PH,C_ACC);fillR(hdc,PN_W-PN_PW-4,(int)pnP2,PN_PW,PN_PH,C_RED);drawCirc(hdc,(int)pnBx,(int)pnBy,6,C_TXT);char b[8];snprintf(b,7,"%d",pnYou);drawCentTxt(hdc,PN_W/2-60,6,50,22,b,C_ACC,1);snprintf(b,7,"%d",pnAI);drawCentTxt(hdc,PN_W/2+10,6,50,22,b,C_RED,1);if(!pnOn)overlayMsg(hdc,PN_W,PN_H,pnYou>=7?"DU GEWINNST!":"KI GEWINNT!","LEERTASTE = Neu");}

/* BREAKOUT */
#define BR_W 480
#define BR_H 340
#define BR_RW 5
#define BR_CL 10
#define BR_BW 42
#define BR_BH 14
#define BR_G 2
typedef struct{short x,y,hits,max,alive;}Brick;
static Brick brBk[BR_RW*BR_CL];static float brPad=200,brPW=80,brBx,brBy,brVx,brVy;static int brSc=0,brLv=1,brLi=3;static BOOL brOn=FALSE;static BOOL brKeys[256]={0};
static COLORREF brCols[BR_RW]={RGB(220,55,55),RGB(220,140,40),RGB(220,200,40),RGB(55,180,55),RGB(55,140,220)};
static void brMake(void){for(int r=0;r<BR_RW;r++)for(int c=0;c<BR_CL;c++){int i=r*BR_CL+c;brBk[i].x=8+c*(BR_BW+BR_G);brBk[i].y=36+r*(BR_BH+BR_G);brBk[i].hits=brBk[i].max=(short)(brLv+(r<2?1:0));brBk[i].alive=1;}}
static void brRstBall(void){brBx=BR_W/2;brBy=BR_H-60;brVx=3.f*(rand()%2?1:-1);brVy=-4.f;}
static void brReset(void){brSc=0;brLi=3;brLv=1;brPad=BR_W/2-brPW/2;brMake();brRstBall();brOn=TRUE;}
static void brStep(void){if(!brOn)return;if(brKeys['A']||brKeys[VK_LEFT]){brPad-=6;if(brPad<0)brPad=0;}if(brKeys['D']||brKeys[VK_RIGHT]){brPad+=6;if(brPad>BR_W-brPW)brPad=BR_W-brPW;}brBx+=brVx;brBy+=brVy;if(brBx<6||brBx>BR_W-6)brVx*=-1;if(brBy<6)brVy*=-1;if(brBy>BR_H-18&&brBx>=brPad&&brBx<=brPad+brPW){brVy=-fabsf(brVy);brVx+=(brBx-(brPad+brPW/2))*.08f;}if(brBy>BR_H){brLi--;brRstBall();if(brLi<=0)brOn=FALSE;}for(int i=0;i<BR_RW*BR_CL;i++){if(!brBk[i].alive)continue;if(brBx>brBk[i].x&&brBx<brBk[i].x+BR_BW&&brBy>brBk[i].y&&brBy<brBk[i].y+BR_BH){brBk[i].hits--;if(brBk[i].hits<=0){brBk[i].alive=0;brSc+=10*brLv;}brVy*=-1;break;}}int al=0;for(int i=0;i<BR_RW*BR_CL;i++)if(brBk[i].alive)al++;if(!al){brLv++;brMake();brRstBall();brVx*=1.1f;brVy*=1.1f;}}
static void brDraw(HDC hdc){fillR(hdc,0,0,BR_W,BR_H,C_BG);for(int r=0;r<BR_RW;r++)for(int c=0;c<BR_CL;c++){Brick*b=&brBk[r*BR_CL+c];if(!b->alive)continue;COLORREF bc=brCols[r];float t=(float)b->hits/b->max;fillR(hdc,b->x,b->y,BR_BW,BR_BH,RGB((int)(GetRValue(bc)*t),(int)(GetGValue(bc)*t),(int)(GetBValue(bc)*t)));borderR(hdc,b->x,b->y,BR_BW,BR_BH,C_BG);}fillR(hdc,(int)brPad,BR_H-14,(int)brPW,8,C_ACC);drawCirc(hdc,(int)brBx,(int)brBy,5,C_TXT);char buf[64];snprintf(buf,63,"Punkte:%d  Leben:%d  Level:%d",brSc,brLi,brLv);drawTxt(hdc,4,4,300,18,buf,C_TXT,0);if(!brOn)overlayMsg(hdc,BR_W,BR_H,"GAME OVER","LEERTASTE = Neu");}

/* TETRIS */
#define TT_W 10
#define TT_H 20
#define TT_S 24
static int ttB[TT_H][TT_W];static COLORREF ttC[TT_H][TT_W];static int ttPx=3,ttPy=0,ttSc=0,ttLi=0,ttLv=1,ttDrop=500;static BOOL ttOn=FALSE;static DWORD ttLD=0;
typedef struct{int cells[4][4];COLORREF c;}Pc;
static Pc ttPcs[]={{{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},RGB(55,200,220)},{{{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},RGB(220,200,40)},{{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},RGB(160,55,220)},{{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},RGB(55,200,55)},{{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},RGB(220,55,55)},{{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},RGB(55,80,220)},{{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},RGB(220,130,40)}};
static Pc ttCur;
static void ttRot(Pc*p){int tmp[4][4]={0};for(int r=0;r<4;r++)for(int c=0;c<4;c++)tmp[c][3-r]=p->cells[r][c];memcpy(p->cells,tmp,sizeof(tmp));}
static BOOL ttCol(Pc*p,int ox,int oy){for(int r=0;r<4;r++)for(int c=0;c<4;c++){if(!p->cells[r][c])continue;int nx=ox+c,ny=oy+r;if(nx<0||nx>=TT_W||ny>=TT_H)return TRUE;if(ny>=0&&ttB[ny][nx])return TRUE;}return FALSE;}
static void ttLock(void){for(int r=0;r<4;r++)for(int c=0;c<4;c++){if(!ttCur.cells[r][c])continue;int ny=ttPy+r,nx=ttPx+c;if(ny>=0&&ny<TT_H){ttB[ny][nx]=1;ttC[ny][nx]=ttCur.c;}}int cl=0;for(int r=TT_H-1;r>=0;){int full=1;for(int c=0;c<TT_W;c++)if(!ttB[r][c]){full=0;break;}if(full){for(int rr=r;rr>0;rr--){memcpy(ttB[rr],ttB[rr-1],sizeof(ttB[0]));memcpy(ttC[rr],ttC[rr-1],sizeof(ttC[0]));}memset(ttB[0],0,sizeof(ttB[0]));cl++;}else r--;}if(cl){ttLi+=cl;ttSc+=cl*100*ttLv;ttLv=(ttLi/10)+1;ttDrop=max(80,500-ttLv*40);}ttPx=3;ttPy=0;ttCur=ttPcs[rand()%7];if(ttCol(&ttCur,ttPx,ttPy))ttOn=FALSE;}
static void ttReset(void){memset(ttB,0,sizeof(ttB));ttSc=0;ttLi=0;ttLv=1;ttDrop=500;ttPx=3;ttPy=0;ttCur=ttPcs[rand()%7];ttOn=TRUE;ttLD=GetTickCount();}
static void ttTick(void){if(!ttOn)return;DWORD now=GetTickCount();if(now-ttLD>=(DWORD)ttDrop){ttLD=now;ttPy++;if(ttCol(&ttCur,ttPx,ttPy)){ttPy--;ttLock();}if(hTtW)InvalidateRect(hTtW,NULL,FALSE);}}
static void ttDraw(HDC hdc){int CW=TT_W*TT_S,CH=TT_H*TT_S;fillR(hdc,0,0,CW+148,CH+10,C_BG);borderR(hdc,0,0,CW+2,CH+2,C_BDR);for(int r=0;r<TT_H;r++)for(int c=0;c<TT_W;c++){if(ttB[r][c])fillR(hdc,c*TT_S+1,r*TT_S+1,TT_S-2,TT_S-2,ttC[r][c]);borderR(hdc,c*TT_S,r*TT_S,TT_S,TT_S,C_BDR);}int gy=ttPy;while(!ttCol(&ttCur,ttPx,gy+1))gy++;for(int r=0;r<4;r++)for(int c=0;c<4;c++){if(!ttCur.cells[r][c])continue;int ny=gy+r,nx=ttPx+c;if(ny>=0&&ny<TT_H)borderR(hdc,nx*TT_S,ny*TT_S,TT_S,TT_S,C_DIM);}for(int r=0;r<4;r++)for(int c=0;c<4;c++){if(!ttCur.cells[r][c])continue;int ny=ttPy+r,nx=ttPx+c;if(ny>=0&&ny<TT_H)fillR(hdc,nx*TT_S+1,ny*TT_S+1,TT_S-2,TT_S-2,ttCur.c);}int sx=CW+10;char b[24];drawTxt(hdc,sx,10,120,18,"PUNKTE",C_DIM,0);snprintf(b,23,"%d",ttSc);drawTxt(hdc,sx,28,120,22,b,C_ACC,1);drawTxt(hdc,sx,60,120,18,"LINIEN",C_DIM,0);snprintf(b,23,"%d",ttLi);drawTxt(hdc,sx,78,120,22,b,C_TXT,1);drawTxt(hdc,sx,110,120,18,"LEVEL",C_DIM,0);snprintf(b,23,"%d",ttLv);drawTxt(hdc,sx,128,120,22,b,C_YEL,1);if(!ttOn)overlayMsg(hdc,CW,CH,"GAME OVER","LEERTASTE = Neu");}

/* DOOM WP */
static LRESULT CALLBACK DoomWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);
        if(doom_running)doom_paint(hdc,rc.right,rc.bottom);
        else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(5,5,10));drawCentTxt(hdc,0,rc.bottom/2-55,rc.right,44,"DOOM",RGB(220,0,0),1);drawCentTxt(hdc,0,rc.bottom/2+10,rc.right,22,"Klick zum Starten",C_ACC,0);drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"WASD+Maus=bewegen  LMB=schiessen  1/2/3=Waffe  M=Musik  4 Level+Boss",C_DIM,0);}
        EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!doom_running){doom_start();doom_capture_mouse(hw);InvalidateRect(hw,NULL,FALSE);}else doom_shoot();return 0;
    case WM_RBUTTONDOWN:doom_shoot();return 0;
    case WM_MOUSEMOVE:if(doom_running&&doom_mouse_captured){RECT rc;GetClientRect(hw,&rc);POINT c={rc.right/2,rc.bottom/2};POINT cur;GetCursorPos(&cur);ScreenToClient(hw,&cur);doom_mouse_dx+=cur.x-c.x;doom_mouse_dy+=cur.y-c.y;ClientToScreen(hw,&c);SetCursorPos(c.x,c.y);}return 0;
    case WM_SETFOCUS:if(doom_running)doom_capture_mouse(hw);return 0;
    case WM_KILLFOCUS:doom_release_mouse();return 0;
    case WM_KEYDOWN:doom_keys[wp&0xFF]=TRUE;if(wp==VK_SPACE)doom_shoot();if(wp=='1')player.weapon=0;if(wp=='2')player.weapon=1;if(wp=='3')player.weapon=2;if(wp=='M'||wp=='m')music_on=!music_on;if((wp=='R'||wp=='r')&&player.dead){player.hp=player.max_hp;player.dead=FALSE;level_init(player.level);}if(wp==VK_ESCAPE){doom_release_mouse();doom_running=FALSE;music_quit();InvalidateRect(hw,NULL,FALSE);}if(doom_running)InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYUP:doom_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* RACE WP */
static LRESULT CALLBACK RaceWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);if(race_running)race_paint(hdc,rc.right,rc.bottom);else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(10,30,10));drawCentTxt(hdc,0,rc.bottom/2-50,rc.right,40,"RACING",RGB(255,200,40),1);drawCentTxt(hdc,0,rc.bottom/2+10,rc.right,22,"Klick zum Starten",C_ACC,0);drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,18,"A/D=Ausweichen  R=Neustart",C_DIM,0);}EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!race_running){race_init(0);player_lap=0;}InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYDOWN:race_keys[wp&0xFF]=TRUE;if((wp=='R'||wp=='r')&&racer.dead){race_init(0);player_lap=0;InvalidateRect(hw,NULL,FALSE);}if(wp==VK_ESCAPE){race_running=FALSE;InvalidateRect(hw,NULL,FALSE);}return 0;
    case WM_KEYUP:race_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* SHOOTER WP */
static LRESULT CALLBACK SSWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);if(ss_running)ss_draw(hdc,rc.right,rc.bottom);else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(2,4,20));drawCentTxt(hdc,0,rc.bottom/2-50,rc.right,40,"SPACE SHOOTER",RGB(0,200,255),1);drawCentTxt(hdc,0,rc.bottom/2+10,rc.right,22,"Klick zum Starten",C_ACC,0);drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"WASD=bewegen  Z/Space=schiessen  X=Bombe",C_DIM,0);}EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!ss_running)ss_reset();InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYDOWN:ss_keys[wp&0xFF]=TRUE;if((wp=='R'||wp=='r')&&ss_player.dead){ss_reset();InvalidateRect(hw,NULL,FALSE);}if(wp=='X'||wp=='x')ss_bomb();if(wp==VK_ESCAPE){ss_running=FALSE;InvalidateRect(hw,NULL,FALSE);}return 0;
    case WM_KEYUP:ss_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* ASTEROIDS WP */
static LRESULT CALLBACK AstWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);if(ast_running)ast_draw(hdc,rc.right,rc.bottom);else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(2,3,18));drawCentTxt(hdc,0,rc.bottom/2-50,rc.right,40,"ASTEROIDS",RGB(200,200,100),1);drawCentTxt(hdc,0,rc.bottom/2+10,rc.right,22,"Klick zum Starten",C_ACC,0);drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"A/D=drehen  W=Schub  Z/Space=schiessen",C_DIM,0);}EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!ast_running)ast_reset();InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYDOWN:ast_keys[wp&0xFF]=TRUE;if((wp=='R'||wp=='r')&&ast_player.dead){ast_reset();InvalidateRect(hw,NULL,FALSE);}if(wp==VK_ESCAPE){ast_running=FALSE;InvalidateRect(hw,NULL,FALSE);}return 0;
    case WM_KEYUP:ast_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* PACMAN WP */
static LRESULT CALLBACK PMWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);if(pm_running)pm_draw(hdc,rc.right,rc.bottom);else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(0,0,0));drawCentTxt(hdc,0,rc.bottom/2-50,rc.right,40,"PAC-MAN",RGB(255,220,0),1);drawCentTxt(hdc,0,rc.bottom/2+10,rc.right,22,"Klick zum Starten",C_ACC,0);drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"WASD/Pfeiltasten  |  Punkte fressen",C_DIM,0);}EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!pm_running)pm_reset();InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYDOWN:pm_keys[wp&0xFF]=TRUE;if(wp==VK_UP||wp=='W')pm_player.ndir=2;if(wp==VK_DOWN||wp=='S')pm_player.ndir=3;if(wp==VK_LEFT||wp=='A')pm_player.ndir=0;if(wp==VK_RIGHT||wp=='D')pm_player.ndir=1;if((wp=='R'||wp=='r')&&pm_player.dead){pm_reset();InvalidateRect(hw,NULL,FALSE);}if(wp==VK_ESCAPE){pm_running=FALSE;InvalidateRect(hw,NULL,FALSE);}return 0;
    case WM_KEYUP:pm_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* GAME WP */
static LRESULT CALLBACK GameWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);HDC mem=CreateCompatibleDC(hdc);HBITMAP bmp=CreateCompatibleBitmap(hdc,rc.right,rc.bottom);HBITMAP ob=(HBITMAP)SelectObject(mem,bmp);if(hw==hSnW)snDraw(mem,rc.right,rc.bottom);else if(hw==hPnW)pnDraw(mem);else if(hw==hBrW)brDraw(mem);else if(hw==hTtW)ttDraw(mem);BitBlt(hdc,0,0,rc.right,rc.bottom,mem,0,0,SRCCOPY);SelectObject(mem,ob);DeleteObject(bmp);DeleteDC(mem);EndPaint(hw,&ps);return 0;}
    case WM_KEYDOWN:{BOOL sp=(wp==VK_SPACE);
        if(hw==hSnW){if(sp&&!snOn){snReset();InvalidateRect(hw,NULL,FALSE);}if(snOn&&(wp==VK_UP||wp=='W')&&snDy!=1){snNx=0;snNy=-1;}if(snOn&&(wp==VK_DOWN||wp=='S')&&snDy!=-1){snNx=0;snNy=1;}if(snOn&&(wp==VK_LEFT||wp=='A')&&snDx!=1){snNx=-1;snNy=0;}if(snOn&&(wp==VK_RIGHT||wp=='D')&&snDx!=-1){snNx=1;snNy=0;}}
        if(hw==hPnW){pnKeys[wp&0xFF]=TRUE;if(sp&&!pnOn){pnReset();InvalidateRect(hw,NULL,FALSE);}}
        if(hw==hBrW){brKeys[wp&0xFF]=TRUE;if(sp&&!brOn){brReset();InvalidateRect(hw,NULL,FALSE);}}
        if(hw==hTtW){if(sp&&!ttOn){ttReset();InvalidateRect(hw,NULL,FALSE);}if(ttOn){if(wp==VK_LEFT&&!ttCol(&ttCur,ttPx-1,ttPy)){ttPx--;InvalidateRect(hw,NULL,FALSE);}if(wp==VK_RIGHT&&!ttCol(&ttCur,ttPx+1,ttPy)){ttPx++;InvalidateRect(hw,NULL,FALSE);}if(wp==VK_DOWN){ttPy++;if(ttCol(&ttCur,ttPx,ttPy)){ttPy--;ttLock();}InvalidateRect(hw,NULL,FALSE);}if(wp==VK_UP){Pc tmp=ttCur;ttRot(&ttCur);if(ttCol(&ttCur,ttPx,ttPy))ttCur=tmp;InvalidateRect(hw,NULL,FALSE);}if(sp){while(!ttCol(&ttCur,ttPx,ttPy+1))ttPy++;ttLock();InvalidateRect(hw,NULL,FALSE);}}}
        return 0;}
    case WM_KEYUP:if(hw==hPnW)pnKeys[wp&0xFF]=FALSE;if(hw==hBrW)brKeys[wp&0xFF]=FALSE;return 0;
    case WM_MOUSEMOVE:if(hw==hPnW){POINT p;GetCursorPos(&p);ScreenToClient(hw,&p);pnP1=(float)p.y-PN_PH/2;if(pnP1<0)pnP1=0;if(pnP1>PN_H-PN_PH)pnP1=PN_H-PN_PH;}if(hw==hBrW){POINT p;GetCursorPos(&p);ScreenToClient(hw,&p);brPad=(float)p.x-brPW/2;if(brPad<0)brPad=0;if(brPad>BR_W-brPW)brPad=BR_W-brPW;}return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* DOWNHILL WP */
static LRESULT CALLBACK DHWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){(void)lp;
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);
        if(dh.started)dh_paint(hdc,rc.right,rc.bottom);
        else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(100,160,220));
            drawCentTxt(hdc,0,rc.bottom/2-55,rc.right,44,"SKI DOWNHILL",RGB(255,255,255),1);
            drawCentTxt(hdc,0,rc.bottom/2+8,rc.right,22,"Klick zum Starten",RGB(200,230,255),0);
            drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"A/D steuern  |  Steinen+Baeumen ausweichen  |  Muenzen sammeln!",RGB(180,210,240),0);}
        EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:SetFocus(hw);if(!dh.started){dh_reset();}InvalidateRect(hw,NULL,FALSE);return 0;
    case WM_KEYDOWN:dh_keys[wp&0xFF]=TRUE;
        if(!dh.started){dh_reset();InvalidateRect(hw,NULL,FALSE);return 0;}
        if((wp=='R'||wp=='r')&&dh.dead){dh_reset();InvalidateRect(hw,NULL,FALSE);}
        if(wp==VK_ESCAPE){dh.started=FALSE;dh_running=FALSE;if(dh_bmp){DeleteObject(dh_bmp);dh_bmp=NULL;dh_px=NULL;}InvalidateRect(hw,NULL,FALSE);}
        return 0;
    case WM_KEYUP:dh_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

/* WORLDMAP WP - Real map */
static LRESULT CALLBACK WMWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps;HDC hdc=BeginPaint(hw,&ps);RECT rc;GetClientRect(hw,&rc);
        if(wmr_running)wmr_draw(hdc,rc.right,rc.bottom);
        else{fillR(hdc,0,0,rc.right,rc.bottom,RGB(18,40,80));
            drawCentTxt(hdc,0,rc.bottom/2-55,rc.right,44,"ECHTE WELTKARTE",RGB(100,200,255),1);
            drawCentTxt(hdc,0,rc.bottom/2+8,rc.right,22,"Klick zum Starten",RGB(60,150,255),0);
            drawCentTxt(hdc,0,rc.bottom/2+40,rc.right,16,"199 echte Laender | Zoom mit Mausrad | 1=Asteroid 2=Atombombe",RGB(70,90,110),0);}
        EndPaint(hw,&ps);return 0;}
    case WM_LBUTTONDOWN:{SetFocus(hw);if(!wmr_running){wmr_reset();InvalidateRect(hw,NULL,FALSE);return 0;}
        RECT rc;GetClientRect(hw,&rc);wmr_click(LOWORD(lp),HIWORD(lp),rc.right,rc.bottom);
        InvalidateRect(hw,NULL,FALSE);return 0;}
    case WM_RBUTTONDOWN:{if(!wmr_running)return 0;
        RECT rc;GetClientRect(hw,&rc);
        /* set aim target at click position */
        wmr_screen_to_world(LOWORD(lp),HIWORD(lp),rc.right,rc.bottom,&wmr_aim_x,&wmr_aim_y);
        wmr_aim_set=TRUE;
        /* also select country there */
        wmr_selected=wmr_country_at(wmr_aim_x,wmr_aim_y);
        InvalidateRect(hw,NULL,FALSE);return 0;}
    case WM_MOUSEWHEEL:{RECT rc;GetClientRect(hw,&rc);POINT pt={LOWORD(lp),HIWORD(lp)};ScreenToClient(hw,&pt);
        float factor=(GET_WHEEL_DELTA_WPARAM(wp)>0)?1.2f:0.83f;
        wmr_zoom_at(pt.x,pt.y,rc.right,rc.bottom,factor);
        InvalidateRect(hw,NULL,FALSE);return 0;}
    case WM_KEYDOWN:wmr_keys[wp&0xFF]=TRUE;
        if(!wmr_running){wmr_reset();InvalidateRect(hw,NULL,FALSE);return 0;}
        if(wp=='1')wmr_bomb_type=0;
        if(wp=='2')wmr_bomb_type=1;
        if(wp==VK_RETURN||wp==VK_SPACE){if(wmr_selected>=0)wmr_drop(wmr_selected,wmr_bomb_type);}
        if(wp=='R'||wp=='r')wmr_reset();
        if(wp==VK_OEM_PLUS||wp==VK_ADD){RECT rc;GetClientRect(hw,&rc);wmr_zoom_at(rc.right/2,rc.bottom/2,rc.right,rc.bottom,1.2f);}
        if(wp==VK_OEM_MINUS||wp==VK_SUBTRACT){RECT rc;GetClientRect(hw,&rc);wmr_zoom_at(rc.right/2,rc.bottom/2,rc.right,rc.bottom,0.83f);}
        if(wp==VK_ESCAPE){wmr_running=FALSE;InvalidateRect(hw,NULL,FALSE);}
        return 0;
    case WM_KEYUP:wmr_keys[wp&0xFF]=FALSE;return 0;}
    return DefWindowProcA(hw,msg,wp,lp);}

static void showTab(int idx){for(int i=0;i<NUM_TABS;i++)if(hPane[i])ShowWindow(hPane[i],i==idx?SW_SHOW:SW_HIDE);g_curtab=idx;if(idx==TAB_SNAKE&&hSnW)SetFocus(hSnW);else if(idx==TAB_PONG&&hPnW)SetFocus(hPnW);else if(idx==TAB_BREAKOUT&&hBrW)SetFocus(hBrW);else if(idx==TAB_TETRIS&&hTtW)SetFocus(hTtW);else if(idx==TAB_DOOM&&hDoomWnd){SetFocus(hDoomWnd);InvalidateRect(hDoomWnd,NULL,FALSE);}else if(idx==TAB_RACE&&hRaceWnd){SetFocus(hRaceWnd);InvalidateRect(hRaceWnd,NULL,FALSE);}else if(idx==TAB_SHOOTER&&hSSWnd){SetFocus(hSSWnd);InvalidateRect(hSSWnd,NULL,FALSE);}else if(idx==TAB_ASTEROIDS&&hAstWnd){SetFocus(hAstWnd);InvalidateRect(hAstWnd,NULL,FALSE);}else if(idx==TAB_PACMAN&&hPMWnd){SetFocus(hPMWnd);InvalidateRect(hPMWnd,NULL,FALSE);}
    else if(idx==TAB_WORLDMAP&&hWMWnd){SetFocus(hWMWnd);InvalidateRect(hWMWnd,NULL,FALSE);}
    else if(idx==TAB_DOWNHILL&&hDHWnd){SetFocus(hDHWnd);InvalidateRect(hDHWnd,NULL,FALSE);}
const char*n[]={"Snake","Pong","Breakout","Tetris","DOOM","Racing","Shooter","Asteroids","Pac-Man","Weltkarte","Ski"};char st[48];snprintf(st,47,"Aktiv: %s",n[idx]);SetWindowTextA(hStatus,st);}

static LRESULT CALLBACK MainWP(HWND hw,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_CREATE:{
        INITCOMMONCONTROLSEX ic={sizeof(ic),ICC_TAB_CLASSES};InitCommonControlsEx(&ic);
        hTab=CreateWindowExA(0,WC_TABCONTROLA,"",WS_CHILD|WS_VISIBLE|TCS_FLATBUTTONS,0,0,1010,28,hw,(HMENU)ID_TAB,NULL,NULL);
        const char*tabs[]={"Snake","Pong","Breakout","Tetris","DOOM","Racing","Shooter","Asteroids","Pac-Man","Weltkarte","Ski"};
        for(int i=0;i<NUM_TABS;i++){TCITEMA ti={0};ti.mask=TCIF_TEXT;ti.pszText=(char*)tabs[i];TabCtrl_InsertItem(hTab,i,&ti);}
        WNDCLASSA gc={0};gc.lpfnWndProc=GameWP;gc.hInstance=GetModuleHandle(NULL);gc.lpszClassName="NPGame";gc.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&gc);
        WNDCLASSA dc={0};dc.lpfnWndProc=DoomWP;dc.hInstance=GetModuleHandle(NULL);dc.lpszClassName="NPDoom";dc.hCursor=LoadCursor(NULL,IDC_CROSS);RegisterClassA(&dc);
        WNDCLASSA rc2={0};rc2.lpfnWndProc=RaceWP;rc2.hInstance=GetModuleHandle(NULL);rc2.lpszClassName="NPRace";rc2.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&rc2);
        WNDCLASSA sc={0};sc.lpfnWndProc=SSWP;sc.hInstance=GetModuleHandle(NULL);sc.lpszClassName="NPShoot";sc.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&sc);
        WNDCLASSA ac={0};ac.lpfnWndProc=AstWP;ac.hInstance=GetModuleHandle(NULL);ac.lpszClassName="NPAst";ac.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&ac);
        WNDCLASSA pc={0};pc.lpfnWndProc=PMWP;pc.hInstance=GetModuleHandle(NULL);pc.lpszClassName="NPPac";pc.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&pc);
        WNDCLASSA wmc={0};wmc.lpfnWndProc=WMWP;wmc.hInstance=GetModuleHandle(NULL);wmc.lpszClassName="NPWMap";wmc.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&wmc);
        WNDCLASSA dhc={0};dhc.lpfnWndProc=DHWP;dhc.hInstance=GetModuleHandle(NULL);dhc.lpszClassName="NPDHill";dhc.hCursor=LoadCursor(NULL,IDC_ARROW);RegisterClassA(&dhc);
        for(int i=0;i<NUM_TABS;i++)hPane[i]=CreateWindowA("STATIC","",i==0?WS_CHILD|WS_VISIBLE:WS_CHILD,0,28,1010,570,hw,0,NULL,NULL);
        hSnW=CreateWindowA("NPGame","",WS_CHILD|WS_VISIBLE,4,4,SN_W*SN_C+4,SN_H*SN_C+28,hPane[TAB_SNAKE],0,NULL,NULL);
        CreateWindowA("STATIC","WASD/Pfeiltasten | LEERTASTE=Start",WS_CHILD|WS_VISIBLE,SN_W*SN_C+14,4,280,18,hPane[TAB_SNAKE],0,NULL,NULL);
        hPnW=CreateWindowA("NPGame","",WS_CHILD|WS_VISIBLE,4,4,PN_W,PN_H,hPane[TAB_PONG],0,NULL,NULL);
        hBrW=CreateWindowA("NPGame","",WS_CHILD|WS_VISIBLE,4,4,BR_W,BR_H,hPane[TAB_BREAKOUT],0,NULL,NULL);
        hTtW=CreateWindowA("NPGame","",WS_CHILD|WS_VISIBLE,4,4,TT_W*TT_S+148,TT_H*TT_S+4,hPane[TAB_TETRIS],0,NULL,NULL);
        hDoomWnd=CreateWindowA("NPDoom","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_DOOM],0,NULL,NULL);
        hRaceWnd=CreateWindowA("NPRace","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_RACE],0,NULL,NULL);
        hSSWnd=CreateWindowA("NPShoot","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_SHOOTER],0,NULL,NULL);
        hAstWnd=CreateWindowA("NPAst","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_ASTEROIDS],0,NULL,NULL);
        hPMWnd=CreateWindowA("NPPac","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_PACMAN],0,NULL,NULL);
        hWMWnd=CreateWindowA("NPWMap","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_WORLDMAP],0,NULL,NULL);
        hDHWnd=CreateWindowA("NPDHill","",WS_CHILD|WS_VISIBLE,0,0,1000,560,hPane[TAB_DOWNHILL],0,NULL,NULL);
        hStatus=CreateWindowA("STATIC","Klick ins Spiel zum Starten!",WS_CHILD|WS_VISIBLE|SS_SUNKEN,0,598,1010,20,hw,(HMENU)ID_STATUS,NULL,NULL);
        SetTimer(hw,TIMER_SNAKE,110,NULL);SetTimer(hw,TIMER_PONG,16,NULL);SetTimer(hw,TIMER_BREAKOUT,16,NULL);SetTimer(hw,TIMER_TETRIS,16,NULL);
        SetTimer(hw,TIMER_DOOM,33,NULL);SetTimer(hw,TIMER_RACE,16,NULL);SetTimer(hw,TIMER_SHOOTER,16,NULL);SetTimer(hw,TIMER_ASTEROIDS,16,NULL);SetTimer(hw,TIMER_PACMAN,16,NULL);
        SetTimer(hw,TIMER_WORLDMAP,33,NULL);
        SetTimer(hw,TIMER_DOWNHILL,16,NULL);
        snReset();snOn=FALSE;pnReset();pnOn=FALSE;brReset();brOn=FALSE;ttReset();ttOn=FALSE;
        showTab(TAB_SNAKE);break;}
    case WM_TIMER:switch(wp){
        case TIMER_SNAKE:snStep();if(hSnW&&g_curtab==TAB_SNAKE)InvalidateRect(hSnW,NULL,FALSE);break;
        case TIMER_PONG:pnStep();if(hPnW&&g_curtab==TAB_PONG)InvalidateRect(hPnW,NULL,FALSE);break;
        case TIMER_BREAKOUT:brStep();if(hBrW&&g_curtab==TAB_BREAKOUT)InvalidateRect(hBrW,NULL,FALSE);break;
        case TIMER_TETRIS:ttTick();break;
        case TIMER_DOOM:if(doom_running&&hDoomWnd&&g_curtab==TAB_DOOM)InvalidateRect(hDoomWnd,NULL,FALSE);break;
        case TIMER_RACE:{DWORD now=GetTickCount();float dt=g_race_last?((float)(now-g_race_last)/1000.f):0.016f;if(dt>0.05f)dt=0.05f;g_race_last=now;race_update(dt);if(race_running&&hRaceWnd&&g_curtab==TAB_RACE)InvalidateRect(hRaceWnd,NULL,FALSE);break;}
        case TIMER_SHOOTER:ss_update();if(hSSWnd&&g_curtab==TAB_SHOOTER)InvalidateRect(hSSWnd,NULL,FALSE);break;
        case TIMER_ASTEROIDS:ast_update();if(hAstWnd&&g_curtab==TAB_ASTEROIDS)InvalidateRect(hAstWnd,NULL,FALSE);break;
        case TIMER_PACMAN:pm_update();if(hPMWnd&&g_curtab==TAB_PACMAN)InvalidateRect(hPMWnd,NULL,FALSE);break;
        case TIMER_WORLDMAP:wmr_update();if(hWMWnd&&g_curtab==TAB_WORLDMAP)InvalidateRect(hWMWnd,NULL,FALSE);break;
        case TIMER_DOWNHILL:if(hDHWnd&&g_curtab==TAB_DOWNHILL)InvalidateRect(hDHWnd,NULL,FALSE);break;}
        return 0;
    case WM_NOTIFY:{NMHDR*nm=(NMHDR*)lp;if(nm->hwndFrom==hTab&&nm->code==TCN_SELCHANGE){if(g_curtab==TAB_DOOM)doom_release_mouse();showTab(TabCtrl_GetCurSel(hTab));}break;}
    case WM_SIZE:{int W=LOWORD(lp),H=HIWORD(lp);if(hTab)SetWindowPos(hTab,NULL,0,0,W,28,SWP_NOZORDER);if(hStatus)SetWindowPos(hStatus,NULL,0,H-20,W,20,SWP_NOZORDER);int pH=H-48;for(int i=0;i<NUM_TABS;i++)if(hPane[i])SetWindowPos(hPane[i],NULL,0,28,W,pH,SWP_NOZORDER);if(hDoomWnd)SetWindowPos(hDoomWnd,NULL,0,0,W,pH,SWP_NOZORDER);if(hRaceWnd)SetWindowPos(hRaceWnd,NULL,0,0,W,pH,SWP_NOZORDER);if(hSSWnd)SetWindowPos(hSSWnd,NULL,0,0,W,pH,SWP_NOZORDER);if(hAstWnd)SetWindowPos(hAstWnd,NULL,0,0,W,pH,SWP_NOZORDER);if(hPMWnd)SetWindowPos(hPMWnd,NULL,0,0,W,pH,SWP_NOZORDER);
        if(hWMWnd)SetWindowPos(hWMWnd,NULL,0,0,W,pH,SWP_NOZORDER);
        if(hDHWnd)SetWindowPos(hDHWnd,NULL,0,0,W,pH,SWP_NOZORDER);break;}
    case WM_DESTROY:doom_release_mouse();music_quit();PostQuitMessage(0);break;}
    return DefWindowProcA(hw,msg,wp,lp);}

int WINAPI WinMain(HINSTANCE hi,HINSTANCE hp,LPSTR cmd,int show){(void)hp;(void)cmd;
    srand((unsigned)time(NULL));
    INITCOMMONCONTROLSEX ic={sizeof(ic),ICC_TAB_CLASSES};InitCommonControlsEx(&ic);
    WNDCLASSA wc={0};wc.lpfnWndProc=MainWP;wc.hInstance=hi;wc.lpszClassName="NPG";
    wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);wc.hCursor=LoadCursor(NULL,IDC_ARROW);wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);RegisterClassA(&wc);
    g_hwnd=CreateWindowA("NPG","NetPanel Games",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1020,650,NULL,NULL,hi,NULL);
    ShowWindow(g_hwnd,show);UpdateWindow(g_hwnd);
    MSG m;while(GetMessage(&m,NULL,0,0)){TranslateMessage(&m);DispatchMessage(&m);}
    return 0;}
