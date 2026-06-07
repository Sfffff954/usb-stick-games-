#pragma once
#include <stdint.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mmsystem.h>

/*        CONFIG                                                                                                                                                     */
#define DW  800
#define DH  480
#define MAPSZ 24
#define MAX_ENEMIES 48
#define NUM_LEVELS   4
#define TEX_S 64

/*        MAPS                                                                                                                                                           */
static const uint8_t LEVELS[NUM_LEVELS][MAPSZ][MAPSZ]={
{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
 {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,2,2,2,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,1},
 {1,0,0,2,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,1},
 {1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,3,0,0,0,0,0,0,3,0,0,0,0,3,0,0,0,3,0,0,0,1},
 {1,0,0,3,0,0,0,0,0,0,3,0,0,0,0,3,0,0,0,3,0,0,0,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,1,1,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,0,0,4,4,4,4,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,1},
 {1,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,1},
 {1,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,9,0,0,1},
 {1,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
{{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,5,5,5,0,0,2,0,0,5,5,0,5,5,0,0,0,0,0,0,0,2},
 {2,0,0,5,0,5,0,0,0,0,0,5,0,0,0,5,0,0,0,0,0,0,0,2},
 {2,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,2,2,0,0,0,2,2,2,2,2,0,0,0,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,7,7,7,7,0,0,0,0,0,0,7,7,7,7,0,0,0,0,0,0,2},
 {2,0,0,7,0,0,7,0,0,0,0,0,0,7,0,0,7,0,0,0,0,0,0,2},
 {2,0,0,7,0,0,7,0,0,0,0,0,0,7,0,0,7,0,0,0,9,0,0,2},
 {2,0,0,7,0,0,7,0,0,0,0,0,0,7,0,0,7,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}},
{{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
 {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3},
 {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3},
 {3,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,6,0,0,0,0,0,0,0,0,0,0,6,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,7,7,0,0,7,7,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,0,0,0,0,0,0,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,0,0,0,0,0,0,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,0,0,0,0,0,0,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,0,0,0,0,0,0,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,0,0,0,0,0,0,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,7,7,7,0,0,7,7,7,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,0,0,6,0,0,0,0,0,0,0,0,0,0,6,0,0,4,0,0,3},
 {3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,3},
 {3,0,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,3},
 {3,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,3},
 {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3},
 {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3},
 {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}}
};

/*        TEXTURES                                                                                                                                               */
static uint32_t WALL_COLORS[8]={0xFF888888,0xFF8B3A1A,0xFF505050,0xFF1E5080,0xFF808080,0xFF6E4112,0xFF005080,0xFF9B8028};
static uint32_t tex[8][TEX_S*TEX_S];
static BOOL tex_done=FALSE;
static void init_textures(void){
    if(tex_done)return; tex_done=TRUE;
    for(int y=0;y<TEX_S;y++) for(int x=0;x<TEX_S;x++){
        int i=y*TEX_S+x;
        /* 1 red brick */
        {int bx=x%16,by=y%8;if(y%16>=8)bx=(x+8)%16;int g=(bx<1||by<1);tex[1][i]=g?0xFF3A1008:RGB(160+bx%20,40+bx%10,15);}
        /* 2 dark stone */
        {int n=(x*7+y*13)%28;tex[2][i]=RGB(52+n,48+n,42+n);if((x%8==0)||(y%8==0))tex[2][i]=RGB(30,28,24);}
        /* 3 blue panel */
        {int px=x%32,py=y%32;if(px<2||py<2||px>29||py>29)tex[3][i]=RGB(20,80,200);else if(px==16||py==16)tex[3][i]=RGB(50,140,255);else tex[3][i]=RGB(12,28,90+(x^y)%20);}
        /* 4 metal */
        {int s=y%4<1;tex[4][i]=s?RGB(70,70,80):RGB(110+(x%16),110+(x%16),120+(x%12));}
        /* 5 wood */
        {int gr=(int)(sinf(x*0.3f+y*0.05f)*12);tex[5][i]=RGB(110+gr,65+gr/2,18);}
        /* 6 dark circuit */
        {int ci=(x%16==0||y%16==0||(x%16==y%16));tex[6][i]=ci?RGB(0,100,140):RGB(8,18,45+(x%8));}
        /* 7 yellow stone */
        {int bmp=(x*3+y*7)%22;tex[7][i]=RGB(170+bmp,135+bmp,40+bmp/2);}
    }
}
static uint32_t gtex(int id,int tx,int ty){
    if(id<1||id>7)return 0xFF888888;
    return tex[id][(ty&(TEX_S-1))*TEX_S+(tx&(TEX_S-1))];
}

/*        ENEMY                                                                                                                                                        */
typedef struct{
    float x,y,hp,max_hp;
    BOOL alive;
    int pain,anim;
    float shoot_cd;
    int is_boss;      /* 1=boss, shoots 3-way, more hp */
} Enemy;
static Enemy enemies[MAX_ENEMIES];
static int n_enemies=0;

/*        BLOOD                                                                                                                                                        */
#define MAX_BLOOD 40
typedef struct{float x,y,vx,vy;int life;uint32_t col;}Blood;
static Blood bloods[MAX_BLOOD];
static int n_blood=0;
static void spawn_blood(float x,float y,int n){
    for(int i=0;i<n;i++){
        if(n_blood>=MAX_BLOOD)n_blood=0;
        Blood*b=&bloods[n_blood++];
        b->x=x;b->y=y;
        float a=((float)rand()/RAND_MAX)*6.28f;
        float sp=0.04f+((float)rand()/RAND_MAX)*0.12f;
        b->vx=cosf(a)*sp;b->vy=sinf(a)*sp;
        b->life=20+rand()%15;
        b->col=RGB(160+rand()%60,0,0);
    }
}

/*        MUSIC                                                                                                                                                        */
/* Simple Doom-style music using Windows Beep in a thread */
static BOOL music_on=TRUE;
static BOOL music_stop=FALSE;
static HANDLE music_thread=NULL;

/* Doom-like melody: note frequencies and durations */
static const int MELODY[][2]={
    /* freq, ms */
    {220,120},{165,80},{220,120},{196,80},{165,120},{0,60},
    {220,120},{247,80},{220,120},{196,200},{0,60},
    {196,120},{165,80},{196,120},{175,80},{165,120},{0,60},
    {220,200},{196,120},{165,200},{0,100},
    {330,100},{294,80},{330,100},{311,80},{294,100},{0,50},
    {330,100},{370,80},{330,100},{294,200},{0,80},
    {294,100},{262,80},{294,100},{277,80},{262,100},{0,50},
    {294,200},{262,100},{247,200},{0,150},
    {0,0}
};

static DWORD WINAPI music_thread_fn(LPVOID a){(void)a;
    while(!music_stop){
        for(int i=0;MELODY[i][1]!=0&&!music_stop;i++){
            if(!music_on){Sleep(50);continue;}
            if(MELODY[i][0]>0) Beep(MELODY[i][0],MELODY[i][1]);
            else Sleep(MELODY[i][1]);
        }
        if(music_stop)break;
        Sleep(200);
    }
    return 0;}

static void music_start(void){
    music_stop=FALSE;
    if(!music_thread){
        music_thread=CreateThread(NULL,0,music_thread_fn,NULL,0,NULL);
    }
}
static void music_quit(void){
    music_stop=TRUE;
    if(music_thread){WaitForSingleObject(music_thread,500);CloseHandle(music_thread);music_thread=NULL;}
}

/*        PLAYER                                                                                                                                                     */
static struct{
    float x,y,angle,pitch; /* pitch for mouse up/down feel (visual only) */
    float vx,vy;
    int hp,max_hp;
    int ammo[3];
    int score,level,weapon;
    int shoot_anim,pain_anim;
    float bob;
    BOOL dead;
} player;

/*        RENDER                                                                                                                                                     */
/* automap: tracks which cells player has visited */
static BOOL automap_seen[MAPSZ][MAPSZ];
static BOOL automap_show=FALSE;  /* TAB toggles full automap */
static HBITMAP doom_bmp=NULL;
static uint32_t *doom_px=NULL;
static float zbuf[DW];
static BOOL doom_keys[256]={0};
static BOOL doom_running=FALSE;
static int doom_mouse_dx=0;
static int doom_mouse_dy=0;  /* for pitch */
static BOOL doom_mouse_captured=FALSE;
static HWND doom_hwnd_ref=NULL;
static float doom_pitch=0.f; /* vertical look offset in pixels */

static uint32_t shade(uint32_t c,float d){
    float f=fmaxf(0.05f,1.f-d/20.f);f=fminf(1.f,f);
    return 0xFF000000|(((int)(((c>>16)&0xFF)*f))<<16)|(((int)(((c>>8)&0xFF)*f))<<8)|((int)((c&0xFF)*f));
}

/*        LEVEL INIT                                                                                                                                         */
static void level_init(int lv){
    player.level=lv; n_enemies=0; n_blood=0;
    memset(enemies,0,sizeof(enemies));
    player.x=1.5f;player.y=1.5f;player.angle=0.3f;
    player.vx=0;player.vy=0;player.bob=0;player.dead=FALSE;
    player.hp=player.max_hp; doom_pitch=0;
    memset(automap_seen,0,sizeof(automap_seen));
    int nE=5+lv*6; srand(lv*7919+42);
    for(int i=0;i<nE&&n_enemies<MAX_ENEMIES;i++){
        int ex,ey,tr=0;
        do{ex=2+rand()%(MAPSZ-4);ey=2+rand()%(MAPSZ-4);tr++;}
        while(LEVELS[lv][ey][ex]!=0&&tr<200);
        if(LEVELS[lv][ey][ex]==0){
            enemies[n_enemies].x=(float)ex+0.5f;
            enemies[n_enemies].y=(float)ey+0.5f;
            enemies[n_enemies].hp=enemies[n_enemies].max_hp=(float)(35+lv*15);
            enemies[n_enemies].alive=TRUE;
            enemies[n_enemies].shoot_cd=1.5f+(float)(rand()%30)/10.f;
            n_enemies++;
        }
    }
}

static void doom_start(void){
    init_textures();
    player.hp=player.max_hp=100;
    player.ammo[0]=50;player.ammo[1]=20;player.ammo[2]=100;
    player.score=0;player.weapon=0;player.dead=FALSE;
    player.shoot_anim=0;player.pain_anim=0;
    level_init(0);doom_running=TRUE;
    music_start();
}

/*        MOUSE                                                                                                                                                        */
static void doom_capture_mouse(HWND hw){
    if(doom_mouse_captured)return;
    doom_mouse_captured=TRUE;doom_hwnd_ref=hw;
    SetCapture(hw);
    RECT rc;GetClientRect(hw,&rc);
    POINT c={rc.right/2,rc.bottom/2};ClientToScreen(hw,&c);
    SetCursorPos(c.x,c.y);ShowCursor(FALSE);
}
static void doom_release_mouse(void){
    if(!doom_mouse_captured)return;
    doom_mouse_captured=FALSE;
    ReleaseCapture();ShowCursor(TRUE);
}

/*        AUTO-AIM                                                                                                                                               */
static float aim_snap(void){
    float best_sc=-1,best_a=0;
    for(int i=0;i<n_enemies;i++){
        if(!enemies[i].alive)continue;
        float dx=enemies[i].x-player.x,dy=enemies[i].y-player.y;
        float dist=sqrtf(dx*dx+dy*dy);
        if(dist>14.f)continue;
        float ea=atan2f(dy,dx);
        float diff=ea-player.angle;
        while(diff>3.14159f)diff-=6.28318f;
        while(diff<-3.14159f)diff+=6.28318f;
        if(fabsf(diff)<0.4f){
            float sc=(1.f-fabsf(diff)/0.4f)*(1.f-dist/14.f);
            if(sc>best_sc){best_sc=sc;best_a=diff;}
        }
    }
    return best_sc>0.25f?best_a*0.45f:0;
}

/*        SHOOT                                                                                                                                                        */
static void doom_shoot(void){
    if(!doom_running||player.dead)return;
    int w=player.weapon;
    if(player.ammo[w]<=0){
        for(int i=0;i<3;i++)if(player.ammo[i]>0){player.weapon=i;w=i;break;}
        if(player.ammo[w]<=0)return;
    }
    player.ammo[w]--;
    player.shoot_anim=8;
    float pa=player.angle+aim_snap();
    int shots=w==1?4:1;
    int dmg[]={25,18,55};
    float spread[]={0.0f,0.14f,0.03f};
    BOOL hit=FALSE;
    for(int s=0;s<shots;s++){
        float ang=pa+((float)rand()/RAND_MAX-0.5f)*spread[w]*2;
        float rdx=cosf(ang),rdy=sinf(ang);
        float best=18.f;int besti=-1;
        for(int i=0;i<n_enemies;i++){
            if(!enemies[i].alive)continue;
            float dx=enemies[i].x-player.x,dy=enemies[i].y-player.y;
            float dist=sqrtf(dx*dx+dy*dy);
            if(dist>17.f)continue;
            float dot=(dx*rdx+dy*rdy)/dist;
            /* bigger hit box = easier to hit */
            float hitbox=0.12f+0.3f/fmaxf(dist,1.f);
            if(dot>1.f-hitbox&&dist<best){best=dist;besti=i;}
        }
        if(besti>=0){
            enemies[besti].hp-=(float)dmg[w];
            enemies[besti].pain=8;
            spawn_blood(enemies[besti].x,enemies[besti].y,6);
            hit=TRUE;
            if(enemies[besti].hp<=0){
                enemies[besti].alive=FALSE;
                player.score+=200*(player.level+1);
            }
        }
    }
    /* hit marker sound */
    if(hit) Beep(800,30);
    else     Beep(600,20);
}

/*        RENDER FRAME                                                                                                                                   */
static void render_frame(void){
    if(!doom_px)return;
    const uint8_t(*lvmap)[MAPSZ]=LEVELS[player.level];
    float px2=player.x,py2=player.y,pa=player.angle;
    float plX=-sinf(pa)*0.66f,plY=cosf(pa)*0.66f;
    float dirX=cosf(pa),dirY=sinf(pa);
    int pitch_off=(int)doom_pitch; /* vertical pixel offset */

    /* floor/ceiling */
    for(int y=0;y<DH;y++){
        float rDX0=dirX-plX,rDY0=dirY-plY;
        float rDX1=dirX+plX,rDY1=dirY+plY;
        int p2=y-DH/2-pitch_off;
        float rowD=fabsf(p2)<1?(1e10f):(float)(DH/2)/(float)p2;
        if(rowD<0)rowD=-rowD;
        float fsX=rowD*(rDX1-rDX0)/DW,fsY=rowD*(rDY1-rDY0)/DW;
        float fX=px2+rowD*rDX0,fY=py2+rowD*rDY0;
        for(int x=0;x<DW;x++){
            int cx=(int)fX,cy=(int)fY;
            int tx=(int)((fX-cx)*TEX_S)&(TEX_S-1);
            int ty=(int)((fY-cy)*TEX_S)&(TEX_S-1);
            fX+=fsX;fY+=fsY;
            float ff=fmaxf(0.05f,1.f-rowD/16.f)*0.65f;
            uint32_t fc=gtex(4,tx,ty),cc=gtex(2,tx,ty);
            if(y>DH/2+pitch_off)doom_px[y*DW+x]=0xFF000000|(((int)(((fc>>16)&0xFF)*ff))<<16)|(((int)(((fc>>8)&0xFF)*ff))<<8)|((int)((fc&0xFF)*ff));
            else doom_px[y*DW+x]=0xFF000000|(((int)(((cc>>16)&0xFF)*ff*0.7f))<<16)|(((int)(((cc>>8)&0xFF)*ff*0.7f))<<8)|((int)((cc&0xFF)*ff*0.7f));
        }
    }

    /* walls */
    for(int x=0;x<DW;x++){
        float camX=2.f*x/DW-1.f;
        float rdx=dirX+plX*camX,rdy=dirY+plY*camX;
        int mx=(int)px2,my=(int)py2;
        float ddx=fabsf(rdx)<1e-6f?1e30f:fabsf(1.f/rdx);
        float ddy=fabsf(rdy)<1e-6f?1e30f:fabsf(1.f/rdy);
        float sdx,sdy;int stepx,stepy;
        if(rdx<0){stepx=-1;sdx=(px2-mx)*ddx;}else{stepx=1;sdx=(mx+1.f-px2)*ddx;}
        if(rdy<0){stepy=-1;sdy=(py2-my)*ddy;}else{stepy=1;sdy=(my+1.f-py2)*ddy;}
        int hit=0,side=0;
        for(int d=0;d<64&&!hit;d++){
            if(sdx<sdy){sdx+=ddx;mx+=stepx;side=0;}
            else{sdy+=ddy;my+=stepy;side=1;}
            if(mx>=0&&mx<MAPSZ&&my>=0&&my<MAPSZ&&lvmap[my][mx]>0&&lvmap[my][mx]!=9)hit=1;
        }
        if(!hit){zbuf[x]=64;continue;}
        float dist;
        if(side==0)dist=(mx-px2+(1-stepx)/2.f)/rdx;
        else        dist=(my-py2+(1-stepy)/2.f)/rdy;
        if(dist<0.1f)dist=0.1f;
        zbuf[x]=dist;
        int wh=(int)(DH/dist);
        int top=DH/2-wh/2+pitch_off;
        int bot=top+wh;
        float wallX=side==0?py2+dist*rdy:px2+dist*rdx;
        wallX-=floorf(wallX);
        int tx=(int)(wallX*TEX_S);
        if((side==0&&rdx>0)||(side==1&&rdy<0))tx=TEX_S-1-tx;
        uint8_t tile=lvmap[my][mx];
        for(int y=max(0,top);y<=min(bot,DH-1);y++){
            int ty2=(int)((float)(y-top)/fmaxf(1,wh)*TEX_S);
            if(ty2<0)ty2=0;if(ty2>=TEX_S)ty2=TEX_S-1;
            uint32_t c=gtex(tile,tx,ty2);
            if(side==1){int r=(int)(((c>>16)&0xFF)*0.68f),g2=(int)(((c>>8)&0xFF)*0.68f),b2=(int)((c&0xFF)*0.68f);c=0xFF000000|(r<<16)|(g2<<8)|b2;}
            doom_px[y*DW+x]=shade(c,dist);
        }
    }

    /* blood */
    float invD=1.f/(plX*dirY-dirX*plY);
    for(int i=0;i<n_blood;i++){
        Blood*b=&bloods[i];if(b->life<=0)continue;
        float dx=b->x-px2,dy=b->y-py2;
        float tx2=invD*(dirY*dx-dirX*dy);
        float tz=invD*(-plY*dx+plX*dy);
        if(tz<=0.1f)continue;
        int spx=(int)(DW/2*(1+tx2/tz));
        int spy=(int)(DH/2+pitch_off+20.f/tz);
        if(spx<0||spx>=DW||spy<0||spy>=DH)continue;
        if(zbuf[spx]<tz)continue;
        int sz=max(1,(int)(3.f/tz));
        for(int dy2=-sz;dy2<=sz;dy2++)for(int ddx=-sz;ddx<=sz;ddx++){int ppx=spx+ddx,ppy=spy+dy2;if(ppx>=0&&ppx<DW&&ppy>=0&&ppy<DH)doom_px[ppy*DW+ppx]=b->col;}
    }

    /* exit */
    {float ex=-1,ey=-1;
     for(int emy=0;emy<MAPSZ;emy++)for(int emx=0;emx<MAPSZ;emx++)if(lvmap[emy][emx]==9){ex=emx+0.5f;ey=emy+0.5f;}
     if(ex>=0){
         float dx=ex-px2,dy=ey-py2;
         float tx2=invD*(dirY*dx-dirX*dy);
         float tz=invD*(-plY*dx+plX*dy);
         if(tz>0.5f){int spx=(int)(DW/2*(1+tx2/tz));int sh=abs((int)(DH/tz));
             for(int ix=spx-sh/6;ix<=spx+sh/6;ix++){if(ix<0||ix>=DW||zbuf[ix]<tz)continue;
                 for(int iy=max(0,DH/2+pitch_off-sh/2);iy<=min(DH-1,DH/2+pitch_off+sh/2);iy++)doom_px[iy*DW+ix]=(ix==spx||iy==DH/2+pitch_off-sh/2||iy==DH/2+pitch_off+sh/2)?0xFFFFFF00:0x88FFFF00;}}
    }}

    /* enemies */
    typedef struct{float d;int i;}SO;
    SO ord[MAX_ENEMIES];int ns=0;
    for(int i=0;i<n_enemies;i++){if(!enemies[i].alive)continue;float dx=enemies[i].x-px2,dy=enemies[i].y-py2;ord[ns].d=dx*dx+dy*dy;ord[ns].i=i;ns++;}
    for(int i=0;i<ns-1;i++)for(int j=i+1;j<ns;j++)if(ord[j].d>ord[i].d){SO t=ord[i];ord[i]=ord[j];ord[j]=t;}
    for(int s=0;s<ns;s++){
        Enemy*e=&enemies[ord[s].i];
        float dx=e->x-px2,dy=e->y-py2;
        float tx2=invD*(dirY*dx-dirX*dy);
        float tz=invD*(-plY*dx+plX*dy);
        if(tz<=0.15f)continue;
        int spx=(int)(DW/2*(1+tx2/tz));
        int sh=abs((int)(DH/tz));
        int x0=spx-sh/2,x1=spx+sh/2;
        int y0=DH/2+pitch_off-sh/2,y1=DH/2+pitch_off+sh/2;
        float hpf=fmaxf(0,e->hp/e->max_hp);
        int boss_scale=e->is_boss?2:1;
        int sh2=sh*boss_scale; int x02=spx-sh2/2,x12=spx+sh2/2;
        int y02=DH/2+pitch_off-sh2/2,y12=DH/2+pitch_off+sh2/2;
        if(e->is_boss){x0=x02;x1=x12;y0=y02;y1=y12;}
        for(int ix=x0;ix<x1;ix++){
            if(ix<0||ix>=DW||zbuf[ix]<tz)continue;
            float u=(float)(ix-x0)/(x1-x0+1);
            for(int iy=y0;iy<y1;iy++){
                if(iy<0||iy>=DH)continue;
                float v=(float)(iy-y0)/(y1-y0+1);
                float cx2=fabsf(u-0.5f),cy2=fabsf(v-0.5f);
                uint32_t c;
                if(e->is_boss){
                    /* BOSS: big red demon */
                    if(cx2>0.46f||cy2>0.48f)c=0xFF330000;
                    else if(v<0.15f){ /* horns */
                        c=(u<0.3f||u>0.7f)?0xFF882200:0xFF550000;
                    }else if(v<0.35f){ /* head */
                        c=e->pain>0?0xFFFFFFFF:shade(RGB(180,40,40),tz);
                        if(v>0.18f&&v<0.28f&&(u<0.35f||u>0.65f))c=0xFFFF8800; /* eyes */
                        if(v>0.28f&&v<0.34f&&cx2<0.25f)c=0xFF220000; /* mouth */
                    }else{ /* body */
                        int rv=(int)(150*hpf);
                        c=e->pain>0?0xFFFF4400:shade(RGB(120+rv/3,20,20),tz);
                        if(v>0.5f&&v<0.56f)c=shade(RGB(60,60,20),tz); /* belt */
                        if(cx2>0.35f&&v>0.4f&&v<0.7f)c=shade(RGB(80,10,10),tz);/*arms*/
                    }
                } else {
                if(cx2>0.44f||cy2>0.47f)c=0xFF002200;
                else if(v<0.32f){
                    c=e->pain>0?0xFFFF5533:shade(RGB(200,155,95),tz);
                    if(v>0.10f&&v<0.20f&&(u<0.34f||u>0.66f))c=0xFFFF2200;
                }else{
                    int g2=(int)(80*hpf);
                    c=e->pain>0?0xFFFF3300:shade(RGB(20,80+g2,20),tz);
                    if(v>0.52f&&v<0.58f)c=shade(RGB(90,65,20),tz);
                }
                }
                doom_px[iy*DW+ix]=c;
            }
        }
        if(e->pain>0)e->pain--;
    }

    /* weapon */
    {int gw=180,gh=140;
     int bx2=(int)(sinf(player.bob*2)*9);
     int by2=(int)(fabsf(cosf(player.bob))*7);
     int gx=DW/2-gw/2+bx2,gy=DH-gh+by2;
     static const uint32_t WC[]={0xFFBBBBBB,0xFF996633,0xFF33AACC};
     uint32_t wc=WC[player.weapon];
     for(int y=20;y<gh;y++)for(int x=gw/2-11;x<gw/2+11;x++){int px3=gx+x,py3=gy+y;if(px3>=0&&px3<DW&&py3>=0&&py3<DH)doom_px[py3*DW+px3]=shade(wc,1.f);}
     for(int y=50;y<gh;y++)for(int x=gw/4;x<3*gw/4;x++){int px3=gx+x,py3=gy+y;if(px3>=0&&px3<DW&&py3>=0&&py3<DH)doom_px[py3*DW+px3]=shade(wc,1.5f);}
     if(player.shoot_anim>4){
         uint32_t fl[]={0xFFFFFFAA,0xFFFFCC44,0xFFFF8800};
         for(int r=0;r<3;r++){int rad=(player.shoot_anim-3)*(3-r)*3;for(int dy2=-rad;dy2<=rad;dy2++)for(int ddx=-rad;ddx<=rad;ddx++){if(ddx*ddx+dy2*dy2>rad*rad)continue;int px3=gx+gw/2+ddx,py3=gy+18+dy2;if(px3>=0&&px3<DW&&py3>=0&&py3<DH)doom_px[py3*DW+px3]=fl[r];}}
     }
     if(player.shoot_anim>0)player.shoot_anim--;
    }

    /* crosshair - green when aim-snap active */
    {int cx=DW/2,cy=DH/2;
     float sn=aim_snap();
     uint32_t cc=fabsf(sn)>0.02f?0xFF00FF44:0xFFFFFFFF;
     for(int i=-8;i<=8;i++){if(abs(i)<3)continue;if(cx+i>=0&&cx+i<DW)doom_px[cy*DW+cx+i]=cc;if(cy+i>=0&&cy+i<DH)doom_px[(cy+i)*DW+cx]=cc;}
     doom_px[cy*DW+cx]=cc;
    }

    /* pain overlay */
    if(player.pain_anim>0){
        int a=player.pain_anim*14;if(a>180)a=180;
        for(int i=0;i<DW*DH;i++){uint32_t p2=doom_px[i];int r=(int)(((p2>>16)&0xFF)+(a));if(r>255)r=255;doom_px[i]=(p2&0xFF00FFFF)|(r<<16);}
        player.pain_anim--;
    }
    if(player.dead){
        for(int i=0;i<DW*DH;i++){uint32_t p2=doom_px[i];int r=(p2>>16)&0xFF,g2=(p2>>8)&0xFF,b2=p2&0xFF;int gr=(r*3+g2+b2)/5;doom_px[i]=0xFF000000|(gr<<16)|((gr/3)<<8);}
    }
}

/*        UPDATE                                                                                                                                                     */
static void doom_update(void){
    if(!doom_running||player.dead)return;
    const uint8_t(*lvmap)[MAPSZ]=LEVELS[player.level];

    /* mouse turn + pitch */
    if(doom_mouse_dx!=0){player.angle+=doom_mouse_dx*0.0022f;doom_mouse_dx=0;}
    if(doom_mouse_dy!=0){
        doom_pitch-=doom_mouse_dy*0.25f;
        if(doom_pitch>80)doom_pitch=80;
        if(doom_pitch<-80)doom_pitch=-80;
        doom_mouse_dy=0;
    }

    float rot=0.04f;
    if(doom_keys[VK_LEFT] ||doom_keys['A'])player.angle+=rot;
    if(doom_keys[VK_RIGHT]||doom_keys['D'])player.angle-=rot;

    float spd=doom_keys[VK_SHIFT]?0.10f:0.065f;
    float dx=0,dy=0;
    if(doom_keys[VK_UP]  ||doom_keys['W']){dx+=cosf(player.angle)*spd;dy+=sinf(player.angle)*spd;}
    if(doom_keys[VK_DOWN]||doom_keys['S']){dx-=cosf(player.angle)*spd;dy-=sinf(player.angle)*spd;}
    if(doom_keys['Q']){dx+=cosf(player.angle+1.5708f)*spd*0.7f;dy+=sinf(player.angle+1.5708f)*spd*0.7f;}
    if(doom_keys['E']){dx-=cosf(player.angle+1.5708f)*spd*0.7f;dy-=sinf(player.angle+1.5708f)*spd*0.7f;}

    if(fabsf(dx)+fabsf(dy)>0.001f)player.bob+=0.14f; else player.bob*=0.88f;

    float m=0.28f;
    float nx=player.x+dx,ny=player.y+dy;
    if(nx-m>=0&&nx+m<MAPSZ){
        int t1=lvmap[(int)player.y][(int)(nx-m)];
        int t2=lvmap[(int)player.y][(int)(nx+m)];
        if((t1==0||t1==9)&&(t2==0||t2==9))player.x=nx;
    }
    if(ny-m>=0&&ny+m<MAPSZ){
        int t1=lvmap[(int)(ny-m)][(int)player.x];
        int t2=lvmap[(int)(ny+m)][(int)player.x];
        if((t1==0||t1==9)&&(t2==0||t2==9))player.y=ny;
    }

    /* exit */
    if(lvmap[(int)player.y][(int)player.x]==9){
        int al=0;for(int i=0;i<n_enemies;i++)if(enemies[i].alive)al++;
        if(al==0&&player.level<NUM_LEVELS-1){player.level++;level_init(player.level);}
    }

    /* blood update */
    for(int i=0;i<n_blood;i++){Blood*b=&bloods[i];if(b->life<=0)continue;b->x+=b->vx;b->y+=b->vy;b->life--;}

    /* enemy AI - they shoot back! */
    for(int i=0;i<n_enemies;i++){
        if(!enemies[i].alive)continue;
        float edx=player.x-enemies[i].x,edy=player.y-enemies[i].y;
        float dist=sqrtf(edx*edx+edy*edy);
        /* move toward player */
        if(dist>1.2f&&dist<12.f){
            float mv=0.022f;
            float nx2=enemies[i].x+edx/dist*mv,ny2=enemies[i].y+edy/dist*mv;
            if(lvmap[(int)ny2][(int)nx2]==0){enemies[i].x=nx2;enemies[i].y=ny2;}
        }
        /* shoot at player */
        enemies[i].shoot_cd-=0.016f;
        float shoot_range=enemies[i].is_boss?16.f:10.f;
        if(enemies[i].shoot_cd<=0&&dist<shoot_range){
            float base_cd=enemies[i].is_boss?0.8f:2.0f+(float)(rand()%20)/10.f;
            enemies[i].shoot_cd=base_cd;
            BOOL los=TRUE;
            float steps=dist*4;
            for(float t=1;t<steps;t++){
                float tx2=enemies[i].x+edx/dist*(t/4.f);
                float ty2=enemies[i].y+edy/dist*(t/4.f);
                if(lvmap[(int)ty2][(int)tx2]>0&&lvmap[(int)ty2][(int)tx2]!=9){los=FALSE;break;}
            }
            if(los){
                int shots=enemies[i].is_boss?3:1;
                for(int s=0;s<shots;s++){
                    float spread=(s-shots/2)*0.25f;
                    float da=atan2f(edx,edy)+spread; /* angle toward player +spread */
                    int dmg=enemies[i].is_boss?12+rand()%8:8+rand()%12;
                    player.hp-=dmg;
                    player.pain_anim=12;
                    if(player.hp<=0){player.hp=0;player.dead=TRUE;}
                }
            }
        }
        enemies[i].anim++;
    }
    /* reveal automap around player */
    {int px2=(int)player.x,py2=(int)player.y;
     for(int dy=-3;dy<=3;dy++)for(int dx=-3;dx<=3;dx++){
         int nx=px2+dx,ny=py2+dy;
         if(nx>=0&&nx<MAPSZ&&ny>=0&&ny<MAPSZ)
             automap_seen[ny][nx]=TRUE;
     }}
    /* weapon auto-switch */
    if(player.ammo[player.weapon]==0)
        for(int i=0;i<3;i++)if(player.ammo[i]>0){player.weapon=i;break;}
}

/*        HUD                                                                                                                                                              */
static void doom_draw_hud(HDC hdc,int W,int H){
    HBRUSH hb=CreateSolidBrush(RGB(0,0,0));RECT hr={0,H-52,W,H};FillRect(hdc,&hr,hb);DeleteObject(hb);
    int bw=(int)(player.hp*160/player.max_hp);
    COLORREF hpcol=player.hp>50?RGB(0,220,0):player.hp>25?RGB(220,180,0):RGB(220,0,0);
    HBRUSH hpbr=CreateSolidBrush(hpcol);RECT hprc={10,H-46,10+bw,H-34};FillRect(hdc,&hprc,hpbr);DeleteObject(hpbr);
    HBRUSH hpbg=CreateSolidBrush(RGB(50,0,0));RECT hpbgr={10+bw,H-46,172,H-34};FillRect(hdc,&hpbgr,hpbg);DeleteObject(hpbg);
    HFONT f=CreateFontA(22,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT sf=CreateFontA(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT oldf=(HFONT)SelectObject(hdc,f);
    SetBkMode(hdc,TRANSPARENT);char buf[80];
    SetTextColor(hdc,hpcol);snprintf(buf,79,"HP: %d",player.hp);TextOutA(hdc,10,H-32,buf,strlen(buf));
    const char*wn[]={"PISTOL","SHOTGUN","PLASMA"};COLORREF wc[]={RGB(200,200,200),RGB(220,140,40),RGB(60,180,255)};
    SetTextColor(hdc,wc[player.weapon]);snprintf(buf,79,"%s [%d]",wn[player.weapon],player.ammo[player.weapon]);TextOutA(hdc,W/2-80,H-44,buf,strlen(buf));
    SetTextColor(hdc,RGB(100,200,255));snprintf(buf,79,"SCORE:%d",player.score);TextOutA(hdc,W-200,H-44,buf,strlen(buf));
    snprintf(buf,79,"LVL:%d/%d",player.level+1,NUM_LEVELS);TextOutA(hdc,W-200,H-22,buf,strlen(buf));
    int alive=0;for(int i=0;i<n_enemies;i++)if(enemies[i].alive)alive++;
    /* Boss HP bar */
    for(int i=0;i<n_enemies;i++){
        if(!enemies[i].alive||!enemies[i].is_boss)continue;
        float hpf=enemies[i].hp/enemies[i].max_hp;
        HBRUSH bbg=CreateSolidBrush(RGB(60,0,0));RECT bbr={W/2-150,4,W/2+150,18};FillRect(hdc,&bbr,bbg);DeleteObject(bbg);
        int bw=(int)(300*hpf);
        HBRUSH bbr2=CreateSolidBrush(RGB(220,0,0));RECT bb2={W/2-150,4,W/2-150+bw,18};FillRect(hdc,&bb2,bbr2);DeleteObject(bbr2);
        SelectObject(hdc,sf);SetTextColor(hdc,RGB(255,255,255));
        char bs[32];snprintf(bs,31,"BOSS HP: %d",(int)enemies[i].hp);
        RECT btr={W/2-145,4,W/2+145,18};DrawTextA(hdc,bs,-1,&btr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        break;
    }
    SelectObject(hdc,sf);SetTextColor(hdc,RGB(180,80,80));snprintf(buf,79,"%d Feinde",alive);TextOutA(hdc,10,H-16,buf,strlen(buf));
    /* weapon slots */
    for(int i=0;i<3;i++){COLORREF c2=i==player.weapon?wc[i]:RGB(60,60,60);HPEN p=CreatePen(PS_SOLID,i==player.weapon?2:1,c2);HPEN op=(HPEN)SelectObject(hdc,p);SelectObject(hdc,GetStockObject(NULL_BRUSH));Rectangle(hdc,W/2+60+i*44,H-48,W/2+100+i*44,H-34);SelectObject(hdc,op);DeleteObject(p);SetTextColor(hdc,c2);char kk[2]={'1'+(char)i,0};TextOutA(hdc,W/2+66+i*44,H-46,kk,1);}
    /* ammo bars */
    for(int i=0;i<3;i++){int mx[]={50,20,100};int aw=(int)(player.ammo[i]*38/mx[i]);HBRUSH ab=CreateSolidBrush(i==player.weapon?wc[i]:RGB(40,40,40));RECT ar={W/2+62+i*44,H-32,W/2+62+i*44+aw,H-20};FillRect(hdc,&ar,ab);DeleteObject(ab);}
    SetTextColor(hdc,RGB(50,50,70));TextOutA(hdc,W/2-80,H-20,"LMB=schiessen  1/2/3=Waffe  Q/E=strafe  M=Musik",47);
    /* music toggle */
    SetTextColor(hdc,music_on?RGB(0,200,0):RGB(100,100,100));
    TextOutA(hdc,W-60,H-16,music_on?"[M=ON]":"[M=OFF]",7);
    if(player.dead){HFONT df=CreateFontA(48,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");SelectObject(hdc,df);SetTextColor(hdc,RGB(220,0,0));TextOutA(hdc,W/2-100,H/2-60,"YOU DIED",8);SelectObject(hdc,sf);SetTextColor(hdc,RGB(180,180,180));TextOutA(hdc,W/2-60,H/2,"R = Neustart",12);DeleteObject(df);}
    if(!player.dead&&alive==0&&player.level==NUM_LEVELS-1){
        SelectObject(hdc,f);SetTextColor(hdc,RGB(255,220,0));
        TextOutA(hdc,W/2-130,H/2-40,"DU HAST GEWONNEN!",17);
        SelectObject(hdc,sf);SetTextColor(hdc,RGB(200,200,200));
        TextOutA(hdc,W/2-80,H/2,"Der Boss ist besiegt!",21);
    }
    if(!player.dead&&alive==0&&player.level<NUM_LEVELS-1){HFONT wf=CreateFontA(32,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");SelectObject(hdc,wf);SetTextColor(hdc,RGB(255,220,0));TextOutA(hdc,W/2-130,H/2-40,"LEVEL KLAR!",11);SelectObject(hdc,sf);SetTextColor(hdc,RGB(200,200,200));TextOutA(hdc,W/2-100,H/2,"Finde den EXIT (gelb)",21);DeleteObject(wf);}
    SelectObject(hdc,oldf);DeleteObject(f);DeleteObject(sf);
}

static void doom_paint(HDC hdc,int W,int H){
    if(!doom_bmp||!doom_px){
        BITMAPINFO bi={0};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=DW;bi.bmiHeader.biHeight=-DH;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;
        doom_bmp=CreateDIBSection(hdc,&bi,DIB_RGB_COLORS,(void**)&doom_px,NULL,0);if(!doom_bmp)return;}
    doom_update();
    if(doom_running)render_frame();
    else memset(doom_px,0x08,DW*DH*4);
    HDC mdc=CreateCompatibleDC(hdc);HBITMAP ob=(HBITMAP)SelectObject(mdc,doom_bmp);
    SetStretchBltMode(hdc,HALFTONE);
    StretchBlt(hdc,0,0,W,H-52,mdc,0,0,DW,DH,SRCCOPY);
    SelectObject(mdc,ob);DeleteDC(mdc);
    doom_draw_hud(hdc,W,H);
}
