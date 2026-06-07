#pragma once
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SS_W 600
#define SS_H 560
#define MAX_BULLETS  64
#define MAX_ENEMIES  48
#define MAX_STARS    80
#define MAX_EXPL     24
#define MAX_PWUP      8

typedef struct { float x,y,vx,vy; BOOL active; int type; } SSBullet;
typedef struct { float x,y,vx,vy,hp; BOOL active; int type,anim; float shoot_cd; } SSEnemy;
typedef struct { float x,y; int life; COLORREF col; float vx,vy; } SSExpl;
typedef struct { float x,y; BOOL active; int type; } SSPwup;

static struct {
    float x,y,vx,vy;
    int hp,max_hp,score,level,lives;
    int shoot_cd, bomb_cd;
    int bombs;
    BOOL dead,invincible;
    int invincible_frames;
    float power; /* 1-3 */
} ss_player;

static SSBullet ss_bullets[MAX_BULLETS];
static SSEnemy  ss_enemies[MAX_ENEMIES];
static SSExpl   ss_expls[MAX_EXPL];
static SSPwup   ss_pwups[MAX_PWUP];
static int      ss_stars[MAX_STARS][3]; /* x,y,speed */
static BOOL     ss_running=FALSE;
static BOOL     ss_keys[256]={0};
static float    ss_spawn_cd=0;
static int      ss_wave=0;
static float    ss_wave_cd=0;
static int      ss_kill_count=0;
static int      ss_high_score=0;

static void ss_expl_spawn(float x,float y,int n,COLORREF c){
    for(int i=0;i<n;i++){
        int slot=-1;for(int j=0;j<MAX_EXPL;j++)if(ss_expls[j].life<=0){slot=j;break;}
        if(slot<0)slot=0;
        float a=((float)rand()/RAND_MAX)*6.28f;
        float sp=0.5f+((float)rand()/RAND_MAX)*3.f;
        ss_expls[slot].x=x;ss_expls[slot].y=y;
        ss_expls[slot].vx=cosf(a)*sp;ss_expls[slot].vy=sinf(a)*sp;
        ss_expls[slot].life=20+rand()%20;ss_expls[slot].col=c;
    }
}

static void ss_shoot(void){
    if(ss_player.shoot_cd>0)return;
    ss_player.shoot_cd=8-(int)ss_player.power;
    int shots=(int)ss_player.power;
    float offsets[]={0,-8,8,-16,16};
    for(int s=0;s<shots&&s<5;s++){
        for(int i=0;i<MAX_BULLETS;i++){
            if(!ss_bullets[i].active){
                ss_bullets[i].active=TRUE;ss_bullets[i].type=0;
                ss_bullets[i].x=ss_player.x+offsets[s];
                ss_bullets[i].y=ss_player.y-20;
                ss_bullets[i].vx=offsets[s]*0.1f;
                ss_bullets[i].vy=-10.f;
                break;
            }
        }
    }
    Beep(1200,15);
}

static void ss_bomb(void){
    if(ss_player.bomb_cd>0||ss_player.bombs<=0)return;
    ss_player.bombs--;ss_player.bomb_cd=60;
    for(int i=0;i<MAX_ENEMIES;i++){
        if(!ss_enemies[i].active)continue;
        ss_enemies[i].hp-=200;
        ss_expl_spawn(ss_enemies[i].x,ss_enemies[i].y,8,RGB(255,180,0));
        if(ss_enemies[i].hp<=0){ss_enemies[i].active=FALSE;ss_player.score+=50*(ss_enemies[i].type+1);}
    }
    Beep(300,100);
}

static void ss_spawn_wave(void){
    int n=3+ss_wave*2; if(n>12)n=12;
    for(int i=0;i<n;i++){
        int slot=-1;for(int j=0;j<MAX_ENEMIES;j++)if(!ss_enemies[j].active){slot=j;break;}
        if(slot<0)break;
        int type=ss_wave>=3?(rand()%3):(ss_wave>=1?(rand()%2):0);
        float ex=50.f+((float)rand()/RAND_MAX)*(SS_W-100);
        ss_enemies[slot].x=ex;ss_enemies[slot].y=-20.f-i*40.f;
        ss_enemies[slot].hp=type==2?80:type==1?40:20;
        ss_enemies[slot].vx=(type==1)?((float)(rand()%3-1)*0.5f):0;
        ss_enemies[slot].vy=1.0f+type*0.3f+(float)ss_wave*0.1f;
        ss_enemies[slot].active=TRUE;ss_enemies[slot].type=type;
        ss_enemies[slot].shoot_cd=60+rand()%60;
    }
    ss_wave++;
}

static void ss_reset(void){
    memset(ss_bullets,0,sizeof(ss_bullets));
    memset(ss_enemies,0,sizeof(ss_enemies));
    memset(ss_expls,0,sizeof(ss_expls));
    memset(ss_pwups,0,sizeof(ss_pwups));
    ss_player.x=SS_W/2;ss_player.y=SS_H-60;
    ss_player.vx=0;ss_player.vy=0;
    ss_player.hp=ss_player.max_hp=100;
    ss_player.score=0;ss_player.lives=3;
    ss_player.shoot_cd=0;ss_player.bomb_cd=0;
    ss_player.bombs=3;ss_player.power=1;
    ss_player.dead=FALSE;ss_player.invincible=FALSE;
    ss_wave=0;ss_spawn_cd=0;ss_wave_cd=0;ss_kill_count=0;
    /* stars */
    for(int i=0;i<MAX_STARS;i++){ss_stars[i][0]=rand()%SS_W;ss_stars[i][1]=rand()%SS_H;ss_stars[i][2]=1+rand()%3;}
    ss_running=TRUE;
    ss_spawn_wave();
}

static void ss_update(void){
    if(!ss_running||ss_player.dead)return;
    /* movement */
    float spd=4.5f;
    if(ss_keys[VK_LEFT]||ss_keys['A']){ss_player.vx-=0.8f;}
    if(ss_keys[VK_RIGHT]||ss_keys['D']){ss_player.vx+=0.8f;}
    if(ss_keys[VK_UP]||ss_keys['W']){ss_player.vy-=0.8f;}
    if(ss_keys[VK_DOWN]||ss_keys['S']){ss_player.vy+=0.8f;}
    ss_player.vx*=0.85f; ss_player.vy*=0.85f;
    if(fabsf(ss_player.vx)>spd)ss_player.vx=spd*(ss_player.vx>0?1:-1);
    if(fabsf(ss_player.vy)>spd)ss_player.vy=spd*(ss_player.vy>0?1:-1);
    ss_player.x+=ss_player.vx; ss_player.y+=ss_player.vy;
    if(ss_player.x<20)ss_player.x=20;if(ss_player.x>SS_W-20)ss_player.x=SS_W-20;
    if(ss_player.y<20)ss_player.y=20;if(ss_player.y>SS_H-20)ss_player.y=SS_H-20;
    /* auto shoot */
    if(ss_keys[VK_SPACE]||ss_keys['Z'])ss_shoot();
    if(ss_player.shoot_cd>0)ss_player.shoot_cd--;
    if(ss_player.bomb_cd>0)ss_player.bomb_cd--;
    if(ss_player.invincible_frames>0){ss_player.invincible_frames--;if(ss_player.invincible_frames==0)ss_player.invincible=FALSE;}
    /* stars scroll */
    for(int i=0;i<MAX_STARS;i++){ss_stars[i][1]+=ss_stars[i][2];if(ss_stars[i][1]>SS_H){ss_stars[i][1]=0;ss_stars[i][0]=rand()%SS_W;}}
    /* bullets */
    for(int i=0;i<MAX_BULLETS;i++){
        if(!ss_bullets[i].active)continue;
        ss_bullets[i].x+=ss_bullets[i].vx;ss_bullets[i].y+=ss_bullets[i].vy;
        if(ss_bullets[i].y<-10||ss_bullets[i].y>SS_H+10||ss_bullets[i].x<-10||ss_bullets[i].x>SS_W+10){ss_bullets[i].active=FALSE;continue;}
        if(ss_bullets[i].type==0){ /* player bullet hits enemies */
            for(int j=0;j<MAX_ENEMIES;j++){
                if(!ss_enemies[j].active)continue;
                float dx=ss_bullets[i].x-ss_enemies[j].x,dy=ss_bullets[i].y-ss_enemies[j].y;
                if(dx*dx+dy*dy<400){
                    ss_bullets[i].active=FALSE;
                    ss_enemies[j].hp-=20+(int)ss_player.power*5;
                    ss_expl_spawn(ss_enemies[j].x,ss_enemies[j].y,3,RGB(255,200,50));
                    if(ss_enemies[j].hp<=0){
                        ss_enemies[j].active=FALSE;ss_kill_count++;
                        ss_player.score+=30*(ss_enemies[j].type+1)*(ss_wave/3+1);
                        ss_expl_spawn(ss_enemies[j].x,ss_enemies[j].y,12,RGB(255,120,0));
                        /* drop powerup */
                        if(rand()%4==0){for(int k=0;k<MAX_PWUP;k++)if(!ss_pwups[k].active){ss_pwups[k].x=ss_enemies[j].x;ss_pwups[k].y=ss_enemies[j].y;ss_pwups[k].active=TRUE;ss_pwups[k].type=rand()%3;break;}}
                        Beep(400,25);
                    }
                    break;
                }
            }
        }else{ /* enemy bullet hits player */
            if(!ss_player.invincible){
                float dx=ss_bullets[i].x-ss_player.x,dy=ss_bullets[i].y-ss_player.y;
                if(dx*dx+dy*dy<300){ss_bullets[i].active=FALSE;ss_player.hp-=15;ss_player.invincible=TRUE;ss_player.invincible_frames=60;ss_expl_spawn(ss_player.x,ss_player.y,5,RGB(255,0,0));Beep(200,40);if(ss_player.hp<=0)ss_player.dead=TRUE;}
            }
        }
    }
    /* enemies */
    int alive=0;
    for(int i=0;i<MAX_ENEMIES;i++){
        if(!ss_enemies[i].active)continue; alive++;
        ss_enemies[i].x+=ss_enemies[i].vx;ss_enemies[i].y+=ss_enemies[i].vy;
        if(ss_enemies[i].type==1){ss_enemies[i].vx=sinf(ss_enemies[i].y*0.03f)*2.f;}
        if(ss_enemies[i].type==2){/* spiral */ss_enemies[i].vx=sinf(ss_enemies[i].y*0.05f)*1.5f;ss_enemies[i].vy=0.8f+0.5f*cosf(ss_enemies[i].x*0.03f);}
        if(ss_enemies[i].x<10)ss_enemies[i].vx=fabsf(ss_enemies[i].vx);
        if(ss_enemies[i].x>SS_W-10)ss_enemies[i].vx=-fabsf(ss_enemies[i].vx);
        if(ss_enemies[i].y>SS_H+20){ss_enemies[i].active=FALSE;ss_player.hp-=5;ss_player.invincible=TRUE;ss_player.invincible_frames=30;continue;}
        /* enemy shoots */
        ss_enemies[i].shoot_cd--;
        if(ss_enemies[i].shoot_cd<=0&&ss_wave>=2){
            ss_enemies[i].shoot_cd=80-ss_wave*5;if(ss_enemies[i].shoot_cd<20)ss_enemies[i].shoot_cd=20;
            float dx=ss_player.x-ss_enemies[i].x,dy=ss_player.y-ss_enemies[i].y;
            float dist=sqrtf(dx*dx+dy*dy);
            for(int k=0;k<MAX_BULLETS;k++)if(!ss_bullets[k].active){ss_bullets[k].active=TRUE;ss_bullets[k].type=1;ss_bullets[k].x=ss_enemies[i].x;ss_bullets[k].y=ss_enemies[i].y;ss_bullets[k].vx=dx/dist*3.f;ss_bullets[k].vy=dy/dist*3.f;break;}
        }
        /* collide with player */
        if(!ss_player.invincible){float dx=ss_enemies[i].x-ss_player.x,dy=ss_enemies[i].y-ss_player.y;if(dx*dx+dy*dy<600){ss_player.hp-=20;ss_player.invincible=TRUE;ss_player.invincible_frames=90;ss_expl_spawn(ss_player.x,ss_player.y,6,RGB(255,0,0));}}
        ss_enemies[i].anim++;
    }
    /* powerups */
    for(int i=0;i<MAX_PWUP;i++){
        if(!ss_pwups[i].active)continue;
        ss_pwups[i].y+=1.5f;if(ss_pwups[i].y>SS_H){ss_pwups[i].active=FALSE;continue;}
        float dx=ss_pwups[i].x-ss_player.x,dy=ss_pwups[i].y-ss_player.y;
        if(dx*dx+dy*dy<400){
            ss_pwups[i].active=FALSE;
            if(ss_pwups[i].type==0){ss_player.hp+=30;if(ss_player.hp>ss_player.max_hp)ss_player.hp=ss_player.max_hp;}
            else if(ss_pwups[i].type==1){ss_player.power+=1;if(ss_player.power>3)ss_player.power=3;}
            else{ss_player.bombs++;}
            Beep(900,30);
        }
    }
    /* explosions */
    for(int i=0;i<MAX_EXPL;i++){if(ss_expls[i].life<=0)continue;ss_expls[i].x+=ss_expls[i].vx;ss_expls[i].y+=ss_expls[i].vy;ss_expls[i].life--;}
    /* wave management */
    if(alive==0){ss_wave_cd++;if(ss_wave_cd>90){ss_wave_cd=0;ss_spawn_wave();if(ss_wave%5==0)ss_player.bombs++;}}
    if(ss_player.score>ss_high_score)ss_high_score=ss_player.score;
}

static void ss_draw(HDC hdc,int W,int H){
    HDC mdc=CreateCompatibleDC(hdc);
    HBITMAP bmp=CreateCompatibleBitmap(hdc,SS_W,SS_H);
    HBITMAP ob=(HBITMAP)SelectObject(mdc,bmp);
    /* background */
    HBRUSH bg=CreateSolidBrush(RGB(2,4,20));RECT r={0,0,SS_W,SS_H};FillRect(mdc,&r,bg);DeleteObject(bg);
    /* stars */
    for(int i=0;i<MAX_STARS;i++){
        int bri=80+ss_stars[i][2]*50;HPEN sp=CreatePen(PS_SOLID,ss_stars[i][2],RGB(bri,bri,bri+40));HPEN osp=(HPEN)SelectObject(mdc,sp);
        MoveToEx(mdc,ss_stars[i][0],ss_stars[i][1],NULL);LineTo(mdc,ss_stars[i][0],ss_stars[i][1]+ss_stars[i][2]*2);
        SelectObject(mdc,osp);DeleteObject(sp);
    }
    /* bullets - player=cyan, enemy=red */
    for(int i=0;i<MAX_BULLETS;i++){
        if(!ss_bullets[i].active)continue;
        COLORREF c=ss_bullets[i].type==0?RGB(0,255,255):RGB(255,60,60);
        HPEN bp=CreatePen(PS_SOLID,2,c);HPEN obp=(HPEN)SelectObject(mdc,bp);
        MoveToEx(mdc,(int)ss_bullets[i].x,(int)ss_bullets[i].y,NULL);
        LineTo(mdc,(int)ss_bullets[i].x,(int)(ss_bullets[i].y-(ss_bullets[i].type==0?10:6)));
        SelectObject(mdc,obp);DeleteObject(bp);
    }
    /* enemies */
    for(int i=0;i<MAX_ENEMIES;i++){
        if(!ss_enemies[i].active)continue;
        float x=ss_enemies[i].x,y=ss_enemies[i].y;
        COLORREF c=ss_enemies[i].type==0?RGB(220,60,60):ss_enemies[i].type==1?RGB(180,60,220):RGB(220,140,40);
        HBRUSH eb=CreateSolidBrush(c);HPEN ep=CreatePen(PS_SOLID,1,RGB(255,255,255));
        HBRUSH oeb=(HBRUSH)SelectObject(mdc,eb);HPEN oep=(HPEN)SelectObject(mdc,ep);
        int sz=ss_enemies[i].type==2?14:10;
        /* draw enemy ship shape */
        POINT pts[5];
        if(ss_enemies[i].type==2){
            pts[0].x=(LONG)x;pts[0].y=(LONG)(y-sz);
            pts[1].x=(LONG)(x+sz);pts[1].y=(LONG)(y+sz);
            pts[2].x=(LONG)(x+sz/2);pts[2].y=(LONG)(y+sz/2);
            pts[3].x=(LONG)(x-sz/2);pts[3].y=(LONG)(y+sz/2);
            pts[4].x=(LONG)(x-sz);pts[4].y=(LONG)(y+sz);
            Polygon(mdc,pts,5);
        }else{
            pts[0].x=(LONG)x;pts[0].y=(LONG)(y-sz);
            pts[1].x=(LONG)(x+sz);pts[1].y=(LONG)(y+sz);
            pts[2].x=(LONG)(x-sz);pts[2].y=(LONG)(y+sz);
            Polygon(mdc,pts,3);
        }
        SelectObject(mdc,oeb);SelectObject(mdc,oep);DeleteObject(eb);DeleteObject(ep);
        /* hp bar */
        float hpf=ss_enemies[i].hp/(ss_enemies[i].type==2?80.f:ss_enemies[i].type==1?40.f:20.f);
        HBRUSH hb=CreateSolidBrush(RGB(0,200,0));RECT hr={(LONG)(x-10),(LONG)(y-sz-5),(LONG)(x-10+(int)(20*hpf)),(LONG)(y-sz-2)};FillRect(mdc,&hr,hb);DeleteObject(hb);
    }
    /* powerups */
    for(int i=0;i<MAX_PWUP;i++){
        if(!ss_pwups[i].active)continue;
        COLORREF pc=ss_pwups[i].type==0?RGB(255,80,80):ss_pwups[i].type==1?RGB(80,255,80):RGB(255,220,40);
        HBRUSH pb=CreateSolidBrush(pc);HPEN pp=CreatePen(PS_SOLID,1,RGB(255,255,255));
        HBRUSH opb=(HBRUSH)SelectObject(mdc,pb);HPEN opp=(HPEN)SelectObject(mdc,pp);
        Ellipse(mdc,(int)ss_pwups[i].x-8,(int)ss_pwups[i].y-8,(int)ss_pwups[i].x+8,(int)ss_pwups[i].y+8);
        SelectObject(mdc,opb);SelectObject(mdc,opp);DeleteObject(pb);DeleteObject(pp);
        HFONT pf=CreateFontA(10,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,"Arial");HFONT opf=(HFONT)SelectObject(mdc,pf);
        SetTextColor(mdc,RGB(0,0,0));SetBkMode(mdc,TRANSPARENT);
        const char*sym[]={"H","P","B"};TextOutA(mdc,(int)ss_pwups[i].x-4,(int)ss_pwups[i].y-6,sym[ss_pwups[i].type],1);
        SelectObject(mdc,opf);DeleteObject(pf);
    }
    /* player ship */
    if(!ss_player.dead&&(ss_player.invincible==FALSE||(ss_player.invincible_frames/4)%2==0)){
        float x=ss_player.x,y=ss_player.y;
        HBRUSH pb=CreateSolidBrush(RGB(40,180,255));HPEN pp=CreatePen(PS_SOLID,1,RGB(200,240,255));
        HBRUSH opb=(HBRUSH)SelectObject(mdc,pb);HPEN opp=(HPEN)SelectObject(mdc,pp);
        POINT ship[]={(LONG)x,(LONG)(y-18),(LONG)(x+14),(LONG)(y+14),(LONG)(x+6),(LONG)(y+8),(LONG)(x-6),(LONG)(y+8),(LONG)(x-14),(LONG)(y+14)};
        Polygon(mdc,ship,5);
        /* engine glow */
        HBRUSH eg=CreateSolidBrush(RGB(0,200,255));SelectObject(mdc,eg);
        int fl=(rand()%6)+4;Ellipse(mdc,(int)x-6,(int)y+14,(int)x+6,(int)y+14+fl);
        SelectObject(mdc,opb);SelectObject(mdc,opp);DeleteObject(pb);DeleteObject(pp);DeleteObject(eg);
    }
    /* explosions */
    for(int i=0;i<MAX_EXPL;i++){
        if(ss_expls[i].life<=0)continue;
        int sz=max(1,ss_expls[i].life/4);
        HBRUSH eb=CreateSolidBrush(ss_expls[i].col);HPEN ep=CreatePen(PS_NULL,0,0);
        HBRUSH oeb=(HBRUSH)SelectObject(mdc,eb);HPEN oep=(HPEN)SelectObject(mdc,ep);
        Ellipse(mdc,(int)ss_expls[i].x-sz,(int)ss_expls[i].y-sz,(int)ss_expls[i].x+sz,(int)ss_expls[i].y+sz);
        SelectObject(mdc,oeb);SelectObject(mdc,oep);DeleteObject(eb);DeleteObject(ep);
    }
    /* HUD */
    HFONT hf=CreateFontA(18,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT sf=CreateFontA(13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT ohf=(HFONT)SelectObject(mdc,hf);
    SetBkMode(mdc,TRANSPARENT);char buf[80];
    /* HP bar */
    HBRUSH hpbg=CreateSolidBrush(RGB(60,0,0));RECT hpr={8,8,148,22};FillRect(mdc,&hpr,hpbg);DeleteObject(hpbg);
    int hpw=(int)(140*ss_player.hp/ss_player.max_hp);
    COLORREF hpc=ss_player.hp>50?RGB(0,220,0):ss_player.hp>25?RGB(220,180,0):RGB(220,0,0);
    HBRUSH hpbr=CreateSolidBrush(hpc);RECT hpr2={8,8,8+hpw,22};FillRect(mdc,&hpr2,hpbr);DeleteObject(hpbr);
    SetTextColor(mdc,RGB(255,255,255));snprintf(buf,79,"HP:%d",ss_player.hp);TextOutA(mdc,8,8,buf,strlen(buf));
    SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"SCORE:%d",ss_player.score);TextOutA(mdc,SS_W/2-60,8,buf,strlen(buf));
    SetTextColor(mdc,RGB(100,200,255));snprintf(buf,79,"WELLE:%d",ss_wave);TextOutA(mdc,SS_W-110,8,buf,strlen(buf));
    SelectObject(mdc,sf);
    SetTextColor(mdc,RGB(255,200,40));snprintf(buf,79,"Bomben:");TextOutA(mdc,8,26,buf,strlen(buf));
    for(int i=0;i<ss_player.bombs;i++){SetTextColor(mdc,RGB(255,120,0));TextOutA(mdc,72+i*14,26,"*",1);}
    SetTextColor(mdc,RGB(100,200,255));snprintf(buf,79,"Power:%.0f  Hi:%d",ss_player.power,ss_high_score);TextOutA(mdc,8,42,buf,strlen(buf));
    SetTextColor(mdc,RGB(60,60,80));TextOutA(mdc,8,SS_H-16,"WASD=bewegen  Z/Space=schiessen  X=Bombe  ESC=Zurueck",53);
    if(ss_player.dead){
        HBRUSH db=CreateSolidBrush(RGB(0,0,0));RECT dr={SS_W/2-110,SS_H/2-50,SS_W/2+110,SS_H/2+70};FillRect(mdc,&dr,db);DeleteObject(db);
        HFONT bf=CreateFontA(36,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");HFONT obf=(HFONT)SelectObject(mdc,bf);
        SetTextColor(mdc,RGB(220,0,0));TextOutA(mdc,SS_W/2-90,SS_H/2-40,"GAME OVER",9);
        SelectObject(mdc,hf);SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"Score: %d",ss_player.score);TextOutA(mdc,SS_W/2-65,SS_H/2+2,buf,strlen(buf));
        SelectObject(mdc,sf);SetTextColor(mdc,RGB(180,180,180));TextOutA(mdc,SS_W/2-55,SS_H/2+30,"R = Neustart",12);
        SelectObject(mdc,obf);DeleteObject(bf);
    }
    SelectObject(mdc,ohf);DeleteObject(hf);DeleteObject(sf);
    /* scale to window */
    float sc=fminf((float)W/SS_W,(float)H/SS_H);
    int dw=(int)(SS_W*sc),dh=(int)(SS_H*sc),ox=(W-dw)/2,oy=(H-dh)/2;
    HBRUSH bbg=CreateSolidBrush(RGB(0,0,0));RECT lt={0,0,ox,H},rt={ox+dw,0,W,H},tp={0,0,W,oy},bt={0,oy+dh,W,H};FillRect(hdc,&lt,bbg);FillRect(hdc,&rt,bbg);FillRect(hdc,&tp,bbg);FillRect(hdc,&bt,bbg);DeleteObject(bbg);
    SetStretchBltMode(hdc,HALFTONE);StretchBlt(hdc,ox,oy,dw,dh,mdc,0,0,SS_W,SS_H,SRCCOPY);
    SelectObject(mdc,ob);DeleteObject(bmp);DeleteDC(mdc);
}
