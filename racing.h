/*
 * racing.h v2 - Dodge game: avoid oncoming traffic
 */
#pragma once
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RACE_W  500
#define RACE_H  600
#define NUM_LANES 4
#define MAX_TRAFFIC 16
#define MAX_RPARTS 32

static const float LANE_X[NUM_LANES]={110,185,260,335};
#define ROAD_LEFT  80.f
#define ROAD_RIGHT 365.f

static float player_lap=0;
static float race_best_lap=999999.f;
static BOOL race_running=FALSE;
static BOOL race_keys[256]={0};
static int race_track=0;

static struct{
    float x,y,vx;
    int lane,hp,score;
    float speed;
    BOOL dead;
    int invincible,shake;
} racer;

typedef struct{float x,y,vy;int lane,type;COLORREF color;BOOL active;}Traffic;
static Traffic traffic[MAX_TRAFFIC];
static int n_traffic=0;

typedef struct{float x,y,vx,vy;int life;COLORREF col;}RPart;
static RPart rparts[MAX_RPARTS];
static int n_rparts=0;

static float road_y=0,spawn_cd=0,game_time=0,near_miss_cd=0;
static int combo=0;

static const COLORREF CAR_COLORS[]={RGB(220,60,60),RGB(60,180,255),RGB(60,220,60),RGB(255,200,40),RGB(200,60,255),RGB(255,120,40),RGB(60,200,180),RGB(255,255,255)};

static void rpart_spawn(float x,float y,int n,COLORREF c){
    for(int i=0;i<n;i++){if(n_rparts>=MAX_RPARTS)n_rparts=0;RPart*p=&rparts[n_rparts++];float a=((float)rand()/RAND_MAX)*6.28f;float sp=1.f+((float)rand()/RAND_MAX)*3.f;p->x=x;p->y=y;p->vx=cosf(a)*sp;p->vy=sinf(a)*sp;p->life=20+rand()%20;p->col=c;}}

static void race_init(int track){
    (void)track;
    memset(traffic,0,sizeof(traffic));n_traffic=0;
    memset(rparts,0,sizeof(rparts));n_rparts=0;
    racer.x=LANE_X[1];racer.y=RACE_H-100;racer.vx=0;racer.lane=1;
    racer.hp=3;racer.score=0;racer.speed=3.5f;racer.dead=FALSE;
    racer.invincible=0;racer.shake=0;
    road_y=0;spawn_cd=0;game_time=0;combo=0;near_miss_cd=0;
    race_running=TRUE;
}

static void spawn_traffic(void){
    int slot=-1;for(int i=0;i<MAX_TRAFFIC;i++)if(!traffic[i].active){slot=i;break;}
    if(slot<0)return;
    Traffic*t=&traffic[slot];
    t->lane=rand()%NUM_LANES;t->x=LANE_X[t->lane];t->y=-80.f;
    t->vy=racer.speed*0.5f+((float)rand()/RAND_MAX)*racer.speed*0.4f;
    t->color=CAR_COLORS[rand()%8];t->type=rand()%3;t->active=TRUE;n_traffic++;
}

static void race_update(float dt){
    if(!race_running)return;
    game_time+=dt;
    racer.speed=3.5f+game_time*0.045f;if(racer.speed>10.f)racer.speed=10.f;
    racer.score=(int)(game_time*10+combo*50);player_lap=(float)racer.score;
    /* steering */
    float target_x=LANE_X[racer.lane];
    if(race_keys[VK_LEFT]||race_keys['A']){racer.vx-=320.f*dt;if(racer.x<LANE_X[racer.lane]-18&&racer.lane>0)racer.lane--;}
    if(race_keys[VK_RIGHT]||race_keys['D']){racer.vx+=320.f*dt;if(racer.x>LANE_X[racer.lane]+18&&racer.lane<NUM_LANES-1)racer.lane++;}
    racer.vx+=(target_x-racer.x)*5.f*dt;racer.vx*=0.80f;
    racer.x+=racer.vx*dt;
    if(racer.x<ROAD_LEFT+15){racer.x=ROAD_LEFT+15;racer.vx*=-0.4f;}
    if(racer.x>ROAD_RIGHT-15){racer.x=ROAD_RIGHT-15;racer.vx*=-0.4f;}
    road_y+=racer.speed;if(road_y>80)road_y-=80;
    /* spawn */
    spawn_cd-=dt;float si=1.8f-racer.speed*0.1f;if(si<0.25f)si=0.25f;
    if(spawn_cd<=0){spawn_traffic();spawn_cd=si+((float)rand()/RAND_MAX)*0.4f;}
    /* traffic */
    for(int i=0;i<MAX_TRAFFIC;i++){
        if(!traffic[i].active)continue;
        traffic[i].y+=traffic[i].vy+racer.speed;
        float dx=fabsf(traffic[i].x-racer.x),dy=fabsf(traffic[i].y-racer.y);
        if(dy<80&&dy>30&&dx<55&&near_miss_cd<=0){combo++;near_miss_cd=0.5f;}
        if(!racer.dead&&racer.invincible<=0&&dx<28&&dy<33){
            racer.hp--;racer.invincible=90;racer.shake=20;combo=0;
            rpart_spawn(racer.x,racer.y,14,RGB(255,180,0));Beep(180,100);
            if(racer.hp<=0){racer.dead=TRUE;Beep(120,300);}
        }
        if(traffic[i].y>RACE_H+100){traffic[i].active=FALSE;n_traffic--;}
    }
    near_miss_cd-=dt;if(racer.invincible>0)racer.invincible--;if(racer.shake>0)racer.shake--;
    for(int i=0;i<n_rparts;i++){RPart*p=&rparts[i];if(p->life<=0)continue;p->x+=p->vx;p->y+=p->vy;p->vy+=0.18f;p->life--;}
}

static void draw_car_top(HDC hdc,float x,float y,COLORREF col,int type,BOOL is_player){
    int w=type==1?18:type==2?8:14,h=type==1?44:type==2?26:32;
    int sx=(int)(x-w/2),sy=(int)(y-h/2);
    COLORREF bc=is_player?RGB(40,120,255):col;
    HBRUSH b=CreateSolidBrush(bc);HBRUSH ob=(HBRUSH)SelectObject(hdc,b);
    HPEN p=CreatePen(PS_SOLID,1,RGB(0,0,0));HPEN op=(HPEN)SelectObject(hdc,p);
    if(type==2)Ellipse(hdc,sx,sy,sx+w,sy+h);else RoundRect(hdc,sx,sy,sx+w,sy+h,4,4);
    /* windshields */
    SelectObject(hdc,ob);SelectObject(hdc,op);DeleteObject(b);DeleteObject(p);
    HBRUSH wb=CreateSolidBrush(RGB(160,220,255));HBRUSH owb=(HBRUSH)SelectObject(hdc,wb);
    HPEN np=CreatePen(PS_NULL,0,0);HPEN onp=(HPEN)SelectObject(hdc,np);
    if(type!=2){Rectangle(hdc,sx+2,sy+4,sx+w-2,sy+12);Rectangle(hdc,sx+2,sy+h-12,sx+w-2,sy+h-4);}
    /* headlights */
    HBRUSH hl=CreateSolidBrush(is_player?RGB(255,255,180):RGB(255,200,100));SelectObject(hdc,hl);
    Ellipse(hdc,sx,sy+1,sx+5,sy+7);Ellipse(hdc,sx+w-5,sy+1,sx+w,sy+7);
    /* tail lights */
    HBRUSH tl=CreateSolidBrush(RGB(220,30,30));SelectObject(hdc,tl);
    Ellipse(hdc,sx,sy+h-7,sx+5,sy+h-1);Ellipse(hdc,sx+w-5,sy+h-7,sx+w,sy+h-1);
    SelectObject(hdc,owb);SelectObject(hdc,onp);
    DeleteObject(wb);DeleteObject(np);DeleteObject(hl);DeleteObject(tl);
}

static void race_paint(HDC hdc,int W,int H){
    HDC mdc=CreateCompatibleDC(hdc);
    HBITMAP mbmp=CreateCompatibleBitmap(hdc,RACE_W,RACE_H);
    HBITMAP mob=(HBITMAP)SelectObject(mdc,mbmp);
    int shx=racer.shake>0?(rand()%6-3):0,shy=racer.shake>0?(rand()%4-2):0;
    /* grass */
    HBRUSH g=CreateSolidBrush(RGB(35,90,25));RECT gr={0,0,RACE_W,RACE_H};FillRect(mdc,&gr,g);DeleteObject(g);
    /* road */
    HBRUSH r=CreateSolidBrush(RGB(58,58,66));RECT rr={(int)ROAD_LEFT,0,(int)ROAD_RIGHT,RACE_H};FillRect(mdc,&rr,r);DeleteObject(r);
    /* edge lines */
    HPEN ep=CreatePen(PS_SOLID,4,RGB(255,255,255));HPEN oep=(HPEN)SelectObject(mdc,ep);
    MoveToEx(mdc,(int)ROAD_LEFT,0,NULL);LineTo(mdc,(int)ROAD_LEFT,RACE_H);
    MoveToEx(mdc,(int)ROAD_RIGHT,0,NULL);LineTo(mdc,(int)ROAD_RIGHT,RACE_H);
    SelectObject(mdc,oep);DeleteObject(ep);
    /* lane dashes */
    HPEN lp=CreatePen(PS_DASH,2,RGB(200,200,80));HPEN olp=(HPEN)SelectObject(mdc,lp);
    for(int l=1;l<NUM_LANES;l++){float lx=(LANE_X[l-1]+LANE_X[l])/2.f;for(int y2=-(int)road_y;y2<RACE_H;y2+=80){MoveToEx(mdc,(int)lx+shx,y2+shy,NULL);LineTo(mdc,(int)lx+shx,y2+40+shy);}}
    SelectObject(mdc,olp);DeleteObject(lp);
    /* traffic */
    for(int i=0;i<MAX_TRAFFIC;i++)if(traffic[i].active)draw_car_top(mdc,traffic[i].x+shx,traffic[i].y+shy,traffic[i].color,traffic[i].type,FALSE);
    /* player */
    if(racer.invincible==0||(racer.invincible/6)%2==0)draw_car_top(mdc,racer.x+shx,racer.y+shy,0,0,TRUE);
    /* particles */
    for(int i=0;i<n_rparts;i++){RPart*p=&rparts[i];if(p->life<=0)continue;HBRUSH pb=CreateSolidBrush(p->col);HBRUSH opb=(HBRUSH)SelectObject(mdc,pb);HPEN pp=CreatePen(PS_NULL,0,0);HPEN opp=(HPEN)SelectObject(mdc,pp);int pr=3+p->life/10;Ellipse(mdc,(int)p->x-pr,(int)p->y-pr,(int)p->x+pr,(int)p->y+pr);SelectObject(mdc,opb);SelectObject(mdc,opp);DeleteObject(pb);DeleteObject(pp);}
    /* HUD */
    HFONT f=CreateFontA(22,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT sf=CreateFontA(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT of=(HFONT)SelectObject(mdc,f);
    SetBkMode(mdc,TRANSPARENT);char buf[80];
    SetTextColor(mdc,RGB(255,255,255));snprintf(buf,79,"SCORE: %d",racer.score);TextOutA(mdc,5,5,buf,strlen(buf));
    SetTextColor(mdc,RGB(255,200,40));snprintf(buf,79,"%.0f km/h",racer.speed*28);TextOutA(mdc,5,30,buf,strlen(buf));
    for(int i=0;i<3;i++){SetTextColor(mdc,i<racer.hp?RGB(255,60,60):RGB(60,60,60));TextOutA(mdc,RACE_W-80+i*24,8,"<3",2);}
    if(combo>1){SetTextColor(mdc,RGB(0,255,150));snprintf(buf,79,"COMBO x%d!",combo);TextOutA(mdc,RACE_W/2-55,RACE_H/2-90,buf,strlen(buf));}
    if(near_miss_cd>0){SetTextColor(mdc,RGB(255,220,0));TextOutA(mdc,RACE_W/2-50,65,"NEAR MISS!",10);}
    SetTextColor(mdc,RGB(70,70,70));
    SelectObject(mdc,sf);TextOutA(mdc,5,RACE_H-18,"A/D = Ausweichen  |  R = Neustart",33);
    if(racer.dead){
        HBRUSH db=CreateSolidBrush(RGB(0,0,0));RECT dr={0,RACE_H/2-55,RACE_W,RACE_H/2+75};FillRect(mdc,&dr,db);DeleteObject(db);
        HFONT bf=CreateFontA(42,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
        HFONT obf=(HFONT)SelectObject(mdc,bf);
        SetTextColor(mdc,RGB(220,0,0));TextOutA(mdc,RACE_W/2-80,RACE_H/2-45,"CRASH!",6);
        SelectObject(mdc,f);SetTextColor(mdc,RGB(255,255,255));snprintf(buf,79,"Score: %d",racer.score);TextOutA(mdc,RACE_W/2-65,RACE_H/2+5,buf,strlen(buf));
        SelectObject(mdc,sf);SetTextColor(mdc,RGB(180,180,180));TextOutA(mdc,RACE_W/2-55,RACE_H/2+38,"R = Neustart",12);
        SelectObject(mdc,obf);DeleteObject(bf);
    }
    SelectObject(mdc,of);DeleteObject(f);DeleteObject(sf);
    /* scale to window */
    float scale=fminf((float)W/RACE_W,(float)H/RACE_H);
    int dw=(int)(RACE_W*scale),dh=(int)(RACE_H*scale),ox=(W-dw)/2,oy=(H-dh)/2;
    HBRUSH bg=CreateSolidBrush(RGB(15,15,15));
    RECT lt={0,0,ox,H},rt={ox+dw,0,W,H},tp={0,0,W,oy},bt={0,oy+dh,W,H};
    FillRect(hdc,&lt,bg);FillRect(hdc,&rt,bg);FillRect(hdc,&tp,bg);FillRect(hdc,&bt,bg);
    DeleteObject(bg);
    SetStretchBltMode(hdc,HALFTONE);StretchBlt(hdc,ox,oy,dw,dh,mdc,0,0,RACE_W,RACE_H,SRCCOPY);
    SelectObject(mdc,mob);DeleteObject(mbmp);DeleteDC(mdc);
}
