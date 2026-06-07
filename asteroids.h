#pragma once
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define AST_W 700
#define AST_H 520
#define MAX_ROCKS  32
#define MAX_ABUL   64
#define MAX_AEXPL  32
#define MAX_ADEBRI 48

typedef struct { float x,y,vx,vy,angle,spin,size; int hp,tier; BOOL active; } Rock;
typedef struct { float x,y,vx,vy; int life; BOOL active; } ABul;
typedef struct { float x,y,vx,vy; int life; } ADebri;

static struct {
    float x,y,vx,vy,angle;
    BOOL thrusting,dead;
    int score,lives,level;
    int shoot_cd,invincible;
    float thrust_anim;
} ast_player;

static Rock   rocks[MAX_ROCKS];
static ABul   abuls[MAX_ABUL];
static ADebri adebri[MAX_ADEBRI];
static BOOL   ast_running=FALSE;
static BOOL   ast_keys[256]={0};
static int    ast_high=0;
static int    ast_rock_count=0;

static void ast_debri_spawn(float x,float y,int n){
    for(int i=0;i<n;i++){
        int slot=-1;for(int j=0;j<MAX_ADEBRI;j++)if(adebri[j].life<=0){slot=j;break;}
        if(slot<0)slot=rand()%MAX_ADEBRI;
        float a=((float)rand()/RAND_MAX)*6.28f;float sp=0.5f+((float)rand()/RAND_MAX)*2.f;
        adebri[slot].x=x;adebri[slot].y=y;adebri[slot].vx=cosf(a)*sp;adebri[slot].vy=sinf(a)*sp;adebri[slot].life=30+rand()%30;
    }
}

static void ast_spawn_rock(float x,float y,float vx,float vy,int tier){
    for(int i=0;i<MAX_ROCKS;i++){
        if(rocks[i].active)continue;
        rocks[i].x=x;rocks[i].y=y;rocks[i].vx=vx;rocks[i].vy=vy;
        rocks[i].tier=tier;rocks[i].size=tier==0?40.f:tier==1?22.f:12.f;
        rocks[i].hp=tier==0?3:tier==1?2:1;
        rocks[i].angle=((float)rand()/RAND_MAX)*6.28f;
        rocks[i].spin=(((float)rand()/RAND_MAX)-0.5f)*0.06f;
        rocks[i].active=TRUE;ast_rock_count++;
        return;
    }
}

static void ast_level_init(int lv){
    memset(rocks,0,sizeof(rocks));ast_rock_count=0;
    int n=3+lv;if(n>8)n=8;
    for(int i=0;i<n;i++){
        float a=((float)rand()/RAND_MAX)*6.28f;
        float r=180.f+((float)rand()/RAND_MAX)*100.f;
        float x=AST_W/2+cosf(a)*r,y=AST_H/2+sinf(a)*r;
        float va=((float)rand()/RAND_MAX)*6.28f;
        float sp=0.4f+lv*0.08f;
        ast_spawn_rock(x,y,cosf(va)*sp,sinf(va)*sp,0);
    }
}

static void ast_reset(void){
    memset(abuls,0,sizeof(abuls));memset(adebri,0,sizeof(adebri));
    ast_player.x=AST_W/2;ast_player.y=AST_H/2;
    ast_player.vx=0;ast_player.vy=0;ast_player.angle=0;
    ast_player.dead=FALSE;ast_player.score=0;ast_player.lives=3;ast_player.level=1;
    ast_player.shoot_cd=0;ast_player.invincible=120;ast_player.thrust_anim=0;
    ast_level_init(1);ast_running=TRUE;
}

static void ast_shoot(void){
    if(ast_player.shoot_cd>0)return;
    ast_player.shoot_cd=12;
    for(int i=0;i<MAX_ABUL;i++)if(!abuls[i].active){
        abuls[i].x=ast_player.x+sinf(ast_player.angle)*18;
        abuls[i].y=ast_player.y-cosf(ast_player.angle)*18;
        abuls[i].vx=ast_player.vx+sinf(ast_player.angle)*9.f;
        abuls[i].vy=ast_player.vy-cosf(ast_player.angle)*9.f;
        abuls[i].life=55;abuls[i].active=TRUE;Beep(1400,12);break;
    }
}

static void ast_update(void){
    if(!ast_running||ast_player.dead)return;
    /* rotation */
    if(ast_keys[VK_LEFT]||ast_keys['A'])ast_player.angle-=0.06f;
    if(ast_keys[VK_RIGHT]||ast_keys['D'])ast_player.angle+=0.06f;
    /* thrust */
    ast_player.thrusting=(ast_keys[VK_UP]||ast_keys['W']);
    if(ast_player.thrusting){
        ast_player.vx+=sinf(ast_player.angle)*0.22f;
        ast_player.vy-=cosf(ast_player.angle)*0.22f;
        ast_player.thrust_anim+=0.3f;
    }else ast_player.thrust_anim*=0.8f;
    float spd=sqrtf(ast_player.vx*ast_player.vx+ast_player.vy*ast_player.vy);
    if(spd>6.f){ast_player.vx=ast_player.vx/spd*6.f;ast_player.vy=ast_player.vy/spd*6.f;}
    ast_player.vx*=0.99f;ast_player.vy*=0.99f;
    ast_player.x+=ast_player.vx;ast_player.y+=ast_player.vy;
    /* wrap */
    if(ast_player.x<0)ast_player.x+=AST_W;if(ast_player.x>AST_W)ast_player.x-=AST_W;
    if(ast_player.y<0)ast_player.y+=AST_H;if(ast_player.y>AST_H)ast_player.y-=AST_H;
    if(ast_keys[VK_SPACE]||ast_keys['Z'])ast_shoot();
    if(ast_player.shoot_cd>0)ast_player.shoot_cd--;
    if(ast_player.invincible>0)ast_player.invincible--;
    /* bullets */
    for(int i=0;i<MAX_ABUL;i++){
        if(!abuls[i].active)continue;
        abuls[i].x+=abuls[i].vx;abuls[i].y+=abuls[i].vy;abuls[i].life--;
        if(abuls[i].x<0)abuls[i].x+=AST_W;if(abuls[i].x>AST_W)abuls[i].x-=AST_W;
        if(abuls[i].y<0)abuls[i].y+=AST_H;if(abuls[i].y>AST_H)abuls[i].y-=AST_H;
        if(abuls[i].life<=0){abuls[i].active=FALSE;continue;}
        for(int j=0;j<MAX_ROCKS;j++){
            if(!rocks[j].active)continue;
            float dx=abuls[i].x-rocks[j].x,dy=abuls[i].y-rocks[j].y;
            if(dx*dx+dy*dy<rocks[j].size*rocks[j].size){
                abuls[i].active=FALSE;rocks[j].hp--;
                ast_debri_spawn(rocks[j].x,rocks[j].y,5);
                if(rocks[j].hp<=0){
                    ast_player.score+=(rocks[j].tier==0?20:rocks[j].tier==1?50:100)*(ast_player.level);
                    ast_debri_spawn(rocks[j].x,rocks[j].y,12);Beep(300,30);
                    if(rocks[j].tier<2){
                        for(int k=0;k<2;k++){float a=((float)rand()/RAND_MAX)*6.28f;float sp=1.f+rocks[j].tier*0.5f;ast_spawn_rock(rocks[j].x,rocks[j].y,cosf(a)*sp,sinf(a)*sp,rocks[j].tier+1);}
                    }
                    rocks[j].active=FALSE;ast_rock_count--;
                }
                break;
            }
        }
    }
    /* rocks */
    for(int i=0;i<MAX_ROCKS;i++){
        if(!rocks[i].active)continue;
        rocks[i].x+=rocks[i].vx;rocks[i].y+=rocks[i].vy;rocks[i].angle+=rocks[i].spin;
        if(rocks[i].x<-50)rocks[i].x+=AST_W+100;if(rocks[i].x>AST_W+50)rocks[i].x-=AST_W+100;
        if(rocks[i].y<-50)rocks[i].y+=AST_H+100;if(rocks[i].y>AST_H+50)rocks[i].y-=AST_H+100;
        if(ast_player.invincible<=0){
            float dx=rocks[i].x-ast_player.x,dy=rocks[i].y-ast_player.y;
            if(dx*dx+dy*dy<(rocks[i].size*0.8f)*(rocks[i].size*0.8f)){
                ast_player.lives--;ast_player.invincible=150;ast_debri_spawn(ast_player.x,ast_player.y,10);Beep(150,100);
                if(ast_player.lives<=0)ast_player.dead=TRUE;
            }
        }
    }
    /* debri */
    for(int i=0;i<MAX_ADEBRI;i++){if(adebri[i].life<=0)continue;adebri[i].x+=adebri[i].vx;adebri[i].y+=adebri[i].vy;adebri[i].vx*=0.97f;adebri[i].vy*=0.97f;adebri[i].life--;}
    /* next level */
    int alive=0;for(int i=0;i<MAX_ROCKS;i++)if(rocks[i].active)alive++;
    if(alive==0){ast_player.level++;ast_level_init(ast_player.level);ast_player.invincible=120;}
    if(ast_player.score>ast_high)ast_high=ast_player.score;
}

/* draw polygon asteroid */
static void ast_draw_rock(HDC hdc,float x,float y,float size,float angle){
    int n=8;POINT pts[8];
    for(int i=0;i<n;i++){
        float a=angle+(float)i/n*6.28f;
        float r=size*(0.7f+0.3f*sinf(i*2.3f+angle*3));
        pts[i].x=(LONG)(x+cosf(a)*r);pts[i].y=(LONG)(y+sinf(a)*r);
    }
    HBRUSH rb=CreateSolidBrush(RGB(90,80,70));HPEN rp=CreatePen(PS_SOLID,1,RGB(180,160,140));
    HBRUSH orb=(HBRUSH)SelectObject(hdc,rb);HPEN orp=(HPEN)SelectObject(hdc,rp);
    Polygon(hdc,pts,n);SelectObject(hdc,orb);SelectObject(hdc,orp);DeleteObject(rb);DeleteObject(rp);
}

static void ast_draw(HDC hdc,int W,int H){
    HDC mdc=CreateCompatibleDC(hdc);
    HBITMAP bmp=CreateCompatibleBitmap(hdc,AST_W,AST_H);HBITMAP ob=(HBITMAP)SelectObject(mdc,bmp);
    HBRUSH bg=CreateSolidBrush(RGB(2,3,18));RECT r={0,0,AST_W,AST_H};FillRect(mdc,&r,bg);DeleteObject(bg);
    /* stars */
    for(int i=0;i<60;i++){int sx=(i*97+13)%AST_W,sy=(i*143+7)%AST_H,bri=100+i%155;HPEN sp=CreatePen(PS_SOLID,1,RGB(bri,bri,bri));HPEN osp=(HPEN)SelectObject(mdc,sp);SetPixel(mdc,sx,sy,RGB(bri,bri,bri));SelectObject(mdc,osp);DeleteObject(sp);}
    /* debri */
    for(int i=0;i<MAX_ADEBRI;i++){if(adebri[i].life<=0)continue;int sz=max(1,adebri[i].life/10);HBRUSH db=CreateSolidBrush(RGB(255,160+sz*3,sz*5));HPEN dp=CreatePen(PS_NULL,0,0);HBRUSH odb=(HBRUSH)SelectObject(mdc,db);HPEN odp=(HPEN)SelectObject(mdc,dp);Ellipse(mdc,(int)adebri[i].x-sz,(int)adebri[i].y-sz,(int)adebri[i].x+sz,(int)adebri[i].y+sz);SelectObject(mdc,odb);SelectObject(mdc,odp);DeleteObject(db);DeleteObject(dp);}
    /* rocks */
    for(int i=0;i<MAX_ROCKS;i++)if(rocks[i].active)ast_draw_rock(mdc,rocks[i].x,rocks[i].y,rocks[i].size,rocks[i].angle);
    /* bullets */
    for(int i=0;i<MAX_ABUL;i++){if(!abuls[i].active)continue;HPEN bp=CreatePen(PS_SOLID,2,RGB(255,255,150));HPEN obp=(HPEN)SelectObject(mdc,bp);MoveToEx(mdc,(int)abuls[i].x,(int)abuls[i].y,NULL);LineTo(mdc,(int)(abuls[i].x-abuls[i].vx*2),(int)(abuls[i].y-abuls[i].vy*2));SelectObject(mdc,obp);DeleteObject(bp);}
    /* player ship */
    if(!ast_player.dead&&(ast_player.invincible==0||(ast_player.invincible/5)%2==0)){
        float x=ast_player.x,y=ast_player.y,a=ast_player.angle;
        float s=cosf(a),c2=sinf(a);
        POINT ship[3]={(LONG)(x+c2*16),(LONG)(y-s*16),(LONG)(x-c2*10+s*10),(LONG)(y+s*10+c2*10),(LONG)(x-c2*10-s*10),(LONG)(y+s*10-c2*10)};
        HBRUSH pb=CreateSolidBrush(RGB(80,200,255));HPEN pp=CreatePen(PS_SOLID,1,RGB(200,240,255));
        HBRUSH opb=(HBRUSH)SelectObject(mdc,pb);HPEN opp=(HPEN)SelectObject(mdc,pp);
        Polygon(mdc,ship,3);SelectObject(mdc,opb);SelectObject(mdc,opp);DeleteObject(pb);DeleteObject(pp);
        /* thrust flame */
        if(ast_player.thrusting){
            int fl=4+(int)(ast_player.thrust_anim*2)%8;
            HBRUSH fb=CreateSolidBrush(RGB(255,120,0));HPEN fp=CreatePen(PS_NULL,0,0);
            HBRUSH ofb=(HBRUSH)SelectObject(mdc,fb);HPEN ofp=(HPEN)SelectObject(mdc,fp);
            POINT flame[3]={(LONG)(x-c2*10+s*10),(LONG)(y+s*10+c2*10),(LONG)(x-c2*10-s*10),(LONG)(y+s*10-c2*10),(LONG)(x-c2*(10+fl)),(LONG)(y+s*(10+fl))};
            Polygon(mdc,flame,3);SelectObject(mdc,ofb);SelectObject(mdc,ofp);DeleteObject(fb);DeleteObject(fp);
        }
    }
    /* HUD */
    HFONT hf=CreateFontA(18,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT sf=CreateFontA(13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT ohf=(HFONT)SelectObject(mdc,hf);SetBkMode(mdc,TRANSPARENT);char buf[80];
    SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"SCORE:%d",ast_player.score);TextOutA(mdc,8,8,buf,strlen(buf));
    SetTextColor(mdc,RGB(100,200,255));snprintf(buf,79,"LEVEL:%d",ast_player.level);TextOutA(mdc,AST_W/2-40,8,buf,strlen(buf));
    SetTextColor(mdc,RGB(255,255,255));snprintf(buf,79,"HI:%d",ast_high);TextOutA(mdc,AST_W-110,8,buf,strlen(buf));
    SelectObject(mdc,sf);
    for(int i=0;i<ast_player.lives;i++){SetTextColor(mdc,RGB(80,180,255));TextOutA(mdc,8+i*16,30,"^",1);}
    SetTextColor(mdc,RGB(50,50,70));TextOutA(mdc,8,AST_H-16,"A/D=drehen  W=Schub  Z/Space=schiessen",38);
    if(ast_player.dead){
        HBRUSH db=CreateSolidBrush(RGB(0,0,0));RECT dr={AST_W/2-110,AST_H/2-50,AST_W/2+110,AST_H/2+70};FillRect(mdc,&dr,db);DeleteObject(db);
        HFONT bf=CreateFontA(36,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");HFONT obf=(HFONT)SelectObject(mdc,bf);
        SetTextColor(mdc,RGB(220,0,0));TextOutA(mdc,AST_W/2-90,AST_H/2-40,"GAME OVER",9);
        SelectObject(mdc,hf);SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"Score:%d",ast_player.score);TextOutA(mdc,AST_W/2-60,AST_H/2+4,buf,strlen(buf));
        SelectObject(mdc,sf);SetTextColor(mdc,RGB(180,180,180));TextOutA(mdc,AST_W/2-55,AST_H/2+34,"R = Neustart",12);
        SelectObject(mdc,obf);DeleteObject(bf);
    }
    SelectObject(mdc,ohf);DeleteObject(hf);DeleteObject(sf);
    float sc=fminf((float)W/AST_W,(float)H/AST_H);
    int dw=(int)(AST_W*sc),dh=(int)(AST_H*sc),ox=(W-dw)/2,oy=(H-dh)/2;
    HBRUSH bbg=CreateSolidBrush(RGB(0,0,0));RECT lt={0,0,ox,H},rt={ox+dw,0,W,H},tp={0,0,W,oy},bt={0,oy+dh,W,H};FillRect(hdc,&lt,bbg);FillRect(hdc,&rt,bbg);FillRect(hdc,&tp,bbg);FillRect(hdc,&bt,bbg);DeleteObject(bbg);
    SetStretchBltMode(hdc,HALFTONE);StretchBlt(hdc,ox,oy,dw,dh,mdc,0,0,AST_W,AST_H,SRCCOPY);
    SelectObject(mdc,ob);DeleteObject(bmp);DeleteDC(mdc);
}
