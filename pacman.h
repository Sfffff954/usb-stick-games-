#pragma once
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PM_COLS 21
#define PM_ROWS 23
#define PM_CELL 24
#define PM_W (PM_COLS*PM_CELL)
#define PM_H (PM_ROWS*PM_CELL+32)

/* 0=empty 1=wall 2=dot 3=power 4=ghost_house */
static const uint8_t PM_MAP[PM_ROWS][PM_COLS]={
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,1},
    {1,3,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,1,1,3,1},
    {1,2,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,2,1,1,1,1,1,1,1,2,1,2,1,1,2,1},
    {1,2,2,2,2,1,2,2,2,2,1,2,2,2,2,1,2,2,2,2,1},
    {1,1,1,1,2,1,1,1,0,0,0,0,0,1,1,1,2,1,1,1,1},
    {1,1,1,1,2,1,0,0,0,4,4,4,0,0,0,1,2,1,1,1,1},
    {1,1,1,1,2,0,0,4,4,4,4,4,4,4,0,0,2,1,1,1,1},
    {0,0,0,0,2,0,0,4,4,4,4,4,4,4,0,0,2,0,0,0,0},
    {1,1,1,1,2,0,0,4,4,4,4,4,4,4,0,0,2,1,1,1,1},
    {1,1,1,1,2,1,0,0,0,0,0,0,0,0,0,1,2,1,1,1,1},
    {1,1,1,1,2,1,0,1,1,1,0,1,1,1,0,1,2,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,1,1,2,1},
    {1,3,2,1,2,2,2,2,2,2,0,2,2,2,2,2,2,1,2,3,1},
    {1,1,2,1,2,1,2,1,1,1,1,1,1,1,2,1,2,1,2,1,1},
    {1,2,2,2,2,1,2,2,2,2,1,2,2,2,2,1,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,1,1,2,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};
static uint8_t pm_dots[PM_ROWS][PM_COLS];

typedef struct { float x,y,tx,ty; int dir,ndir,mode,scared,scatter_t; COLORREF col; } Ghost;

static struct {
    float x,y;
    int dir,ndir;
    int score,lives,level;
    float anim;
    BOOL dead,powered;
    int power_t,death_anim;
} pm_player;

static Ghost pm_ghosts[4];
static BOOL  pm_running=FALSE;
static BOOL  pm_keys[256]={0};
static int   pm_dots_left=0;
static int   pm_high=0;
static float pm_ghost_cd=0;

static COLORREF GHOST_COLS[]={RGB(255,0,0),RGB(255,180,255),RGB(0,220,255),RGB(255,180,40)};

static int pm_can_move(int cx,int cy,int dir){
    int nx=cx,ny=cy;
    if(dir==0)nx--;else if(dir==1)nx++;else if(dir==2)ny--;else ny++;
    if(nx<0)nx=PM_COLS-1;if(nx>=PM_COLS)nx=0;
    if(ny<0||ny>=PM_ROWS)return 0;
    return PM_MAP[ny][nx]!=1;
}

static void pm_reset_ghosts(void){
    float sx[]={10,10,9,11};float sy[]={9,11,10,10};
    for(int i=0;i<4;i++){
        pm_ghosts[i].x=sx[i];pm_ghosts[i].y=sy[i];
        pm_ghosts[i].tx=sx[i];pm_ghosts[i].ty=sy[i];
        pm_ghosts[i].dir=i%2;pm_ghosts[i].ndir=i%2;
        pm_ghosts[i].mode=0;pm_ghosts[i].scared=0;
        pm_ghosts[i].scatter_t=60+i*20;
        pm_ghosts[i].col=GHOST_COLS[i];
    }
}

static void pm_reset(void){
    memcpy(pm_dots,PM_MAP,sizeof(PM_MAP));
    pm_dots_left=0;
    for(int y=0;y<PM_ROWS;y++)for(int x=0;x<PM_COLS;x++)if(pm_dots[y][x]==2||pm_dots[y][x]==3)pm_dots_left++;
    pm_player.x=10;pm_player.y=16;pm_player.dir=1;pm_player.ndir=1;
    pm_player.score=0;pm_player.lives=3;pm_player.level=1;pm_player.anim=0;
    pm_player.dead=FALSE;pm_player.powered=FALSE;pm_player.power_t=0;pm_player.death_anim=0;
    pm_reset_ghosts();pm_running=TRUE;pm_ghost_cd=0;
}

static void pm_update(void){
    if(!pm_running||pm_player.dead)return;
    /* try new direction */
    int cx=(int)(pm_player.x+0.5f),cy=(int)(pm_player.y+0.5f);
    if(pm_can_move(cx,cy,pm_player.ndir))pm_player.dir=pm_player.ndir;
    /* move */
    float spd=0.1f;
    float nx2=pm_player.x,ny2=pm_player.y;
    if(pm_player.dir==0)nx2-=spd;else if(pm_player.dir==1)nx2+=spd;
    else if(pm_player.dir==2)ny2-=spd;else ny2+=spd;
    /* wrap */
    if(nx2<0)nx2=PM_COLS-1;if(nx2>=PM_COLS)nx2=0;
    int tnx=(int)(nx2+0.5f),tny=(int)(ny2+0.5f);
    if(tny>=0&&tny<PM_ROWS&&PM_MAP[tny][tnx]!=1){pm_player.x=nx2;pm_player.y=ny2;}
    pm_player.anim+=0.15f;
    /* eat dot */
    int ex=(int)(pm_player.x+0.5f),ey=(int)(pm_player.y+0.5f);
    if(ey>=0&&ey<PM_ROWS&&ex>=0&&ex<PM_COLS){
        if(pm_dots[ey][ex]==2){pm_dots[ey][ex]=0;pm_player.score+=10;pm_dots_left--;Beep(600,10);}
        if(pm_dots[ey][ex]==3){pm_dots[ey][ex]=0;pm_player.score+=50;pm_dots_left--;pm_player.powered=TRUE;pm_player.power_t=300;for(int i=0;i<4;i++)pm_ghosts[i].scared=250;Beep(300,30);}
    }
    if(pm_player.power_t>0){pm_player.power_t--;if(pm_player.power_t==0){pm_player.powered=FALSE;for(int i=0;i<4;i++)pm_ghosts[i].scared=0;}}
    /* ghosts */
    pm_ghost_cd+=0.08f;
    for(int g=0;g<4;g++){
        pm_ghosts[g].scatter_t--;
        float gcx=pm_ghosts[g].x,gcy=pm_ghosts[g].y;
        int igx=(int)(gcx+0.5f),igy=(int)(gcy+0.5f);
        /* move */
        float gspd=pm_ghosts[g].scared>0?0.07f:0.09f;
        float gnx=gcx,gny=gcy;
        if(pm_ghosts[g].dir==0)gnx-=gspd;else if(pm_ghosts[g].dir==1)gnx+=gspd;
        else if(pm_ghosts[g].dir==2)gny-=gspd;else gny+=gspd;
        if(gnx<0)gnx=PM_COLS-1;if(gnx>=PM_COLS)gnx=0;
        int tgnx=(int)(gnx+0.5f),tgny=(int)(gny+0.5f);
        if(tgny>=0&&tgny<PM_ROWS&&PM_MAP[tgny][tgnx]!=1){pm_ghosts[g].x=gnx;pm_ghosts[g].y=gny;}
        else{/* choose new direction */
            int best=-1,bd=999999;
            for(int d=0;d<4;d++){
                if((d==0&&pm_ghosts[g].dir==1)||(d==1&&pm_ghosts[g].dir==0)||(d==2&&pm_ghosts[g].dir==3)||(d==3&&pm_ghosts[g].dir==2))continue;
                if(!pm_can_move(igx,igy,d))continue;
                int tx=(int)pm_player.x,ty=(int)pm_player.y;
                if(pm_ghosts[g].scatter_t<=0){tx=d==0?igx-1:d==1?igx+1:igx;ty=d==2?igy-1:d==3?igy+1:igy;}
                int nnx=igx+(d==0?-1:d==1?1:0),nny=igy+(d==2?-1:d==3?1:0);
                int dist=(nnx-tx)*(nnx-tx)+(nny-ty)*(nny-ty);
                if(pm_ghosts[g].scared>0)dist=-dist;
                if(dist<bd){bd=dist;best=d;}
            }
            if(best>=0)pm_ghosts[g].dir=best;
        }
        if(pm_ghosts[g].scared>0)pm_ghosts[g].scared--;
        /* collide with player */
        float dx=(pm_ghosts[g].x-pm_player.x),dy=(pm_ghosts[g].y-pm_player.y);
        if(dx*dx+dy*dy<0.5f){
            if(pm_ghosts[g].scared>0){pm_ghosts[g].scared=0;pm_player.score+=200;pm_reset_ghosts();Beep(1000,40);}
            else{pm_player.lives--;pm_player.death_anim=60;pm_player.dead=TRUE;Beep(100,200);if(pm_player.lives>0){pm_player.dead=FALSE;pm_player.x=10;pm_player.y=16;pm_reset_ghosts();}}
        }
    }
    if(pm_dots_left==0){pm_player.level++;memcpy(pm_dots,PM_MAP,sizeof(PM_MAP));pm_dots_left=0;for(int y=0;y<PM_ROWS;y++)for(int x=0;x<PM_COLS;x++)if(pm_dots[y][x]==2||pm_dots[y][x]==3)pm_dots_left++;pm_reset_ghosts();}
    if(pm_player.score>pm_high)pm_high=pm_player.score;
}

static void pm_draw(HDC hdc,int W,int H){
    HDC mdc=CreateCompatibleDC(hdc);HBITMAP bmp=CreateCompatibleBitmap(hdc,PM_W,PM_H);HBITMAP ob=(HBITMAP)SelectObject(mdc,bmp);
    HBRUSH bg=CreateSolidBrush(RGB(0,0,0));RECT r={0,0,PM_W,PM_H};FillRect(mdc,&r,bg);DeleteObject(bg);
    /* walls+dots */
    for(int y=0;y<PM_ROWS;y++)for(int x=0;x<PM_COLS;x++){
        int px=x*PM_CELL,py=y*PM_CELL;
        if(PM_MAP[y][x]==1){HBRUSH wb=CreateSolidBrush(RGB(20,30,200));HPEN wp=CreatePen(PS_SOLID,1,RGB(60,80,255));HBRUSH owb=(HBRUSH)SelectObject(mdc,wb);HPEN owp=(HPEN)SelectObject(mdc,wp);RoundRect(mdc,px+1,py+1,px+PM_CELL-1,py+PM_CELL-1,4,4);SelectObject(mdc,owb);SelectObject(mdc,owp);DeleteObject(wb);DeleteObject(wp);}
        if(pm_dots[y][x]==2){HBRUSH db=CreateSolidBrush(RGB(255,220,180));HPEN dp=CreatePen(PS_NULL,0,0);HBRUSH odb=(HBRUSH)SelectObject(mdc,db);HPEN odp=(HPEN)SelectObject(mdc,dp);int cx=px+PM_CELL/2,cy=py+PM_CELL/2;Ellipse(mdc,cx-3,cy-3,cx+3,cy+3);SelectObject(mdc,odb);SelectObject(mdc,odp);DeleteObject(db);DeleteObject(dp);}
        if(pm_dots[y][x]==3){int blink=(int)(pm_ghost_cd*3)%2;HBRUSH pb=CreateSolidBrush(blink?RGB(255,255,255):RGB(255,180,255));HPEN pp=CreatePen(PS_NULL,0,0);HBRUSH opb=(HBRUSH)SelectObject(mdc,pb);HPEN opp=(HPEN)SelectObject(mdc,pp);int cx=px+PM_CELL/2,cy=py+PM_CELL/2;Ellipse(mdc,cx-6,cy-6,cx+6,cy+6);SelectObject(mdc,opb);SelectObject(mdc,opp);DeleteObject(pb);DeleteObject(pp);}
    }
    /* player */
    {int px=(int)(pm_player.x*PM_CELL),py=(int)(pm_player.y*PM_CELL);
     float mouthA=fabsf(sinf(pm_player.anim))*45.f;
     int mx=px+PM_CELL/2,my=py+PM_CELL/2;
     HBRUSH yb=CreateSolidBrush(RGB(255,220,0));HPEN yp=CreatePen(PS_NULL,0,0);HBRUSH oyb=(HBRUSH)SelectObject(mdc,yb);HPEN oyp=(HPEN)SelectObject(mdc,yp);
     int r2=PM_CELL/2-1;
     /* draw filled circle then erase mouth */
     Ellipse(mdc,mx-r2,my-r2,mx+r2,my+r2);
     /* mouth */
     float ma=mouthA*3.14159f/180.f;float baseA=pm_player.dir==1?0:pm_player.dir==0?3.14159f:pm_player.dir==2?-1.5708f:1.5708f;
     POINT mouth[3]={(LONG)mx,(LONG)my,(LONG)(mx+(int)(cosf(baseA+ma)*r2)),(LONG)(my+(int)(sinf(baseA+ma)*r2)),(LONG)(mx+(int)(cosf(baseA-ma)*r2)),(LONG)(my+(int)(sinf(baseA-ma)*r2))};
     HBRUSH mb=CreateSolidBrush(RGB(0,0,0));HBRUSH omb=(HBRUSH)SelectObject(mdc,mb);Polygon(mdc,mouth,3);
     SelectObject(mdc,oyb);SelectObject(mdc,oyp);SelectObject(mdc,omb);DeleteObject(yb);DeleteObject(yp);DeleteObject(mb);}
    /* ghosts */
    for(int g=0;g<4;g++){
        int gx=(int)(pm_ghosts[g].x*PM_CELL),gy=(int)(pm_ghosts[g].y*PM_CELL);
        int gmx=gx+PM_CELL/2,gmy=gy+PM_CELL/2;
        COLORREF gc=pm_ghosts[g].scared>0?(pm_ghosts[g].scared<60&&(pm_ghosts[g].scared/8)%2?RGB(255,255,255):RGB(30,30,200)):pm_ghosts[g].col;
        HBRUSH gb=CreateSolidBrush(gc);HPEN gp=CreatePen(PS_NULL,0,0);HBRUSH ogb=(HBRUSH)SelectObject(mdc,gb);HPEN ogp=(HPEN)SelectObject(mdc,gp);
        int gr2=PM_CELL/2-1;
        /* ghost body */
        POINT ghost[8];int bn=7;
        for(int i=0;i<bn-1;i++){float a=3.14159f*(float)i/(bn-2);ghost[i].x=gmx+(LONG)(cosf(a)*gr2);ghost[i].y=gmy-(LONG)(sinf(a)*gr2);}
        ghost[bn-1].x=gmx-gr2;ghost[bn-1].y=gmy+gr2;
        /* simple approach: just draw ellipse top + rectangle bottom */
        Ellipse(mdc,gmx-gr2,gmy-gr2,gmx+gr2,gmy+1);
        RECT br2={gmx-gr2,gmy,gmx+gr2,gmy+gr2};FillRect(mdc,&br2,gb);
        SelectObject(mdc,ogb);SelectObject(mdc,ogp);DeleteObject(gb);DeleteObject(gp);
        /* eyes */
        if(!pm_ghosts[g].scared){
            HBRUSH eb=CreateSolidBrush(RGB(255,255,255));HPEN ep=CreatePen(PS_NULL,0,0);HBRUSH oeb=(HBRUSH)SelectObject(mdc,eb);HPEN oep=(HPEN)SelectObject(mdc,ep);
            Ellipse(mdc,gmx-5,gmy-6,gmx-1,gmy-1);Ellipse(mdc,gmx+1,gmy-6,gmx+5,gmy-1);
            HBRUSH pb2=CreateSolidBrush(RGB(0,0,200));SelectObject(mdc,pb2);
            Ellipse(mdc,gmx-4,gmy-5,gmx-2,gmy-2);Ellipse(mdc,gmx+2,gmy-5,gmx+4,gmy-2);
            SelectObject(mdc,oeb);SelectObject(mdc,oep);DeleteObject(eb);DeleteObject(ep);DeleteObject(pb2);
        }
    }
    /* HUD */
    HFONT hf=CreateFontA(18,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");HFONT ohf=(HFONT)SelectObject(mdc,hf);
    SetBkMode(mdc,TRANSPARENT);char buf[80];
    SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"SCORE:%d",pm_player.score);TextOutA(mdc,4,PM_ROWS*PM_CELL+4,buf,strlen(buf));
    SetTextColor(mdc,RGB(255,255,255));for(int i=0;i<pm_player.lives;i++){/* draw tiny pacman */HBRUSH lb=CreateSolidBrush(RGB(255,220,0));HPEN lp=CreatePen(PS_NULL,0,0);HBRUSH olb=(HBRUSH)SelectObject(mdc,lb);HPEN olp=(HPEN)SelectObject(mdc,lp);Ellipse(mdc,PM_W/2+i*20-6,PM_ROWS*PM_CELL+4,PM_W/2+i*20+6,PM_ROWS*PM_CELL+16);SelectObject(mdc,olb);SelectObject(mdc,olp);DeleteObject(lb);DeleteObject(lp);}
    SetTextColor(mdc,RGB(100,200,255));snprintf(buf,79,"LVL:%d HI:%d",pm_player.level,pm_high);TextOutA(mdc,PM_W-150,PM_ROWS*PM_CELL+4,buf,strlen(buf));
    if(pm_player.dead){SetTextColor(mdc,RGB(255,0,0));TextOutA(mdc,PM_W/2-40,PM_ROWS/2*PM_CELL,"GAME OVER",9);}
    SelectObject(mdc,ohf);DeleteObject(hf);
    float sc=fminf((float)W/PM_W,(float)H/PM_H);
    int dw=(int)(PM_W*sc),dh=(int)(PM_H*sc),ox=(W-dw)/2,oy=(H-dh)/2;
    HBRUSH bbg=CreateSolidBrush(RGB(0,0,0));RECT lt={0,0,ox,H},rt={ox+dw,0,W,H},tp={0,0,W,oy},bt={0,oy+dh,W,H};FillRect(hdc,&lt,bbg);FillRect(hdc,&rt,bbg);FillRect(hdc,&tp,bbg);FillRect(hdc,&bt,bbg);DeleteObject(bbg);
    SetStretchBltMode(hdc,HALFTONE);StretchBlt(hdc,ox,oy,dw,dh,mdc,0,0,PM_W,PM_H,SRCCOPY);
    SelectObject(mdc,ob);DeleteObject(bmp);DeleteDC(mdc);
}
