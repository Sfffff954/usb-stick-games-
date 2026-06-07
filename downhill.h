/*
 * downhill.h - First-Person Downhill Skiing
 * Raycaster-style 3D with pseudo-perspective
 * Dodge trees, rocks - collect coins
 */
#pragma once
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DH_W  800
#define DH_H  520
#define DH_FOV 0.9f
#define MAX_OBJ 120
#define DH_LANE_W 6.0f   /* half-width of slope */
#define DH_HORIZON (DH_H*2/5)

/* object types */
#define OBJ_TREE   0
#define OBJ_ROCK   1
#define OBJ_COIN   2
#define OBJ_GATE   3  /* speed gate - go through for bonus */
#define OBJ_RAMP   4  /* ramp - launches you in air */

typedef struct {
    float x;      /* horizontal position -DH_LANE_W..+DH_LANE_W */
    float z;      /* distance ahead of player */
    int   type;
    BOOL  active;
    float scale;  /* visual scale variation */
} DHObj;

static DHObj  dh_objs[MAX_OBJ];
static int    dh_n_objs=0;

/*        Player                                                                                                                                                        */
static struct {
    float x;          /* horizontal pos */
    float vx;         /* horizontal speed */
    float speed;      /* forward speed (increases) */
    float dist;       /* total distance travelled */
    int   coins;
    int   score;
    int   lives;
    int   combo;      /* coin combo */
    float combo_timer;
    BOOL  dead;
    BOOL  airborne;
    float air_timer;
    float air_height;
    int   hit_flash;   /* red flash on hit */
    float lean;        /* visual lean left/right */
    int   gate_bonus;
    float gate_flash;
    BOOL  started;
    int   high_score;
} dh;

/*        Particles                                                                                                                                               */
#define MAX_SNOW 200
typedef struct { float x,y,vx,vy; int life; } SnowP;
static SnowP snow[MAX_SNOW];

#define MAX_SPARKS 32
typedef struct { float x,y,vx,vy; int life; COLORREF col; } Spark;
static Spark sparks[MAX_SPARKS];

/*        Terrain strips                                                                                                                                   */
static float dh_terrain_scroll=0;
static BOOL  dh_running=FALSE;
static BOOL  dh_keys[256]={0};
static float dh_spawn_cd=0;
static float dh_time=0;

/*        Sky gradient colors                                                                                                                 */
static uint32_t dh_pixels[DH_W*DH_H];
static HBITMAP  dh_bmp=NULL;
static uint32_t *dh_px=NULL;

static void dh_spark_spawn(float sx, float sy, int n, COLORREF c){
    for(int i=0;i<n;i++){
        int slot=-1;for(int j=0;j<MAX_SPARKS;j++)if(sparks[j].life<=0){slot=j;break;}
        if(slot<0)slot=rand()%MAX_SPARKS;
        float a=((float)rand()/RAND_MAX)*6.28f;
        float sp=1.f+((float)rand()/RAND_MAX)*3.f;
        sparks[slot].x=sx;sparks[slot].y=sy;
        sparks[slot].vx=cosf(a)*sp;sparks[slot].vy=sinf(a)*sp-1.f;
        sparks[slot].life=15+rand()%20;sparks[slot].col=c;
    }
}

static void dh_spawn_obj(void){
    /* find free slot */
    int slot=-1;
    for(int i=0;i<MAX_OBJ;i++)if(!dh_objs[i].active){slot=i;break;}
    if(slot<0)return;

    DHObj *o=&dh_objs[slot];
    o->active=TRUE;
    o->z=80.f+((float)rand()/RAND_MAX)*20.f;

    /* weighted random type */
    int r=rand()%100;
    if(r<30)      o->type=OBJ_TREE;
    else if(r<50) o->type=OBJ_ROCK;
    else if(r<75) o->type=OBJ_COIN;
    else if(r<90) o->type=OBJ_GATE;
    else          o->type=OBJ_RAMP;

    /* position */
    if(o->type==OBJ_GATE){
        o->x=0; /* gates are centered */
    } else if(o->type==OBJ_COIN){
        /* coins in clusters */
        o->x=((float)rand()/RAND_MAX-0.5f)*(DH_LANE_W*1.5f);
    } else {
        o->x=((float)rand()/RAND_MAX-0.5f)*(DH_LANE_W*1.8f);
    }
    o->scale=0.7f+((float)rand()/RAND_MAX)*0.6f;
}

static void dh_reset(void){
    memset(dh_objs,0,sizeof(dh_objs));
    memset(snow,0,sizeof(snow));
    memset(sparks,0,sizeof(sparks));
    dh.x=0;dh.vx=0;dh.speed=8.f;
    dh.dist=0;dh.coins=0;dh.score=0;dh.lives=3;
    dh.combo=0;dh.combo_timer=0;dh.dead=FALSE;
    dh.airborne=FALSE;dh.air_timer=0;dh.air_height=0;
    dh.hit_flash=0;dh.lean=0;dh.gate_bonus=0;dh.gate_flash=0;
    dh.started=TRUE;
    dh_spawn_cd=0;dh_time=0;
    /* init snow particles */
    for(int i=0;i<MAX_SNOW;i++){
        snow[i].x=((float)rand()/RAND_MAX)*DH_W;
        snow[i].y=((float)rand()/RAND_MAX)*DH_H;
        snow[i].vx=((float)rand()/RAND_MAX-0.5f)*0.5f;
        snow[i].vy=0.5f+((float)rand()/RAND_MAX)*1.5f;
        snow[i].life=999;
    }
    dh_running=TRUE;
}

/*        Project 3D point to screen                                                                                            */
typedef struct { int sx,sy,scale_h; BOOL visible; } Proj;
static Proj dh_project(float obj_x, float obj_z){
    Proj p; p.visible=FALSE;
    if(obj_z<0.5f)return p;
    float fov_scale=DH_W*0.5f/tanf(DH_FOV*0.5f);
    /* account for player x offset */
    float rel_x=obj_x-dh.x;
    p.sx=(int)(DH_W/2+rel_x/obj_z*fov_scale);
    /* vertical: horizon + perspective */
    float air_off=dh.airborne?dh.air_height*20.f:0;
    p.sy=(int)(DH_HORIZON+(1.f/obj_z)*fov_scale*0.5f-air_off);
    p.scale_h=(int)(fov_scale/obj_z);
    p.visible=(p.sx>-200&&p.sx<DH_W+200&&p.sy>0&&p.sy<DH_H);
    return p;
}

/*        Render frame                                                                                                                                      */
static void dh_render(void){
    if(!dh_px)return;

    /*        Sky gradient        */
    for(int y=0;y<DH_HORIZON;y++){
        float t=(float)y/DH_HORIZON;
        int r=(int)(100+t*60),g=(int)(160+t*80),b2=(int)(220+t*35);
        uint32_t c=0xFF000000|(r<<16)|(g<<8)|b2;
        for(int x=0;x<DH_W;x++) dh_px[y*DH_W+x]=c;
    }

    /*        Snow ground with slope strips        */
    for(int y=DH_HORIZON;y<DH_H;y++){
        float t=(float)(y-DH_HORIZON)/(DH_H-DH_HORIZON);
        /* base snow color shaded by distance */
        int base=200+(int)(t*55);
        int r2=base,g2=base,b3=(int)(base*1.05f);if(b3>255)b3=255;
        /* blue tint in shadows */
        r2=(int)(r2*0.92f);g2=(int)(g2*0.95f);
        uint32_t snow_c=0xFF000000|(r2<<16)|(g2<<8)|b3;
        /* slope edge markers */
        float dist=1.f/fmaxf(0.01f,t);
        float edge_x_r=DH_W/2+DH_LANE_W/dist*(DH_W*0.3f);
        float edge_x_l=DH_W/2-DH_LANE_W/dist*(DH_W*0.3f);
        for(int x=0;x<DH_W;x++){
            uint32_t c=snow_c;
            /* outside slope = darker snow/forest */
            if(x<(int)edge_x_l||x>(int)edge_x_r){
                c=0xFF000000|((r2/3)<<16)|((int)(g2*0.6f)<<8)|(b3/3);
                /* trees at edge */
                if((x+(int)(dh_terrain_scroll*8))%24<3)
                    c=0xFF115511;
            }
            /* centre line dashes */
            if(abs(x-DH_W/2)<2&&((int)(dh_terrain_scroll*6)+y)%40<20)
                c=0xFFEEEECC;
            /* edge red/yellow warning line */
            if(fabsf(x-edge_x_l)<3||fabsf(x-edge_x_r)<3)
                c=((int)(dh_time*4))%2?0xFFFF2200:0xFFFFCC00;
            dh_px[y*DH_W+x]=c;
        }
    }

    /*        Draw objects back to front        */
    /* sort by z descending */
    int order[MAX_OBJ];int nc=0;
    for(int i=0;i<MAX_OBJ;i++) if(dh_objs[i].active) order[nc++]=i;
    for(int i=0;i<nc-1;i++) for(int j=i+1;j<nc;j++)
        if(dh_objs[order[j]].z>dh_objs[order[i]].z){int t=order[i];order[i]=order[j];order[j]=t;}

    for(int oi=0;oi<nc;oi++){
        DHObj *o=&dh_objs[order[oi]];
        Proj p=dh_project(o->x,o->z);
        if(!p.visible||p.scale_h<2)continue;
        int sh=p.scale_h;
        int sw=(int)(sh*o->scale);

        switch(o->type){
        case OBJ_TREE:{
            /* trunk */
            int tw=max(2,sw/6),th=sh/2;
            int tx=p.sx-tw/2,ty=p.sy-sh;
            for(int y=p.sy-th;y<p.sy;y++) for(int x=tx;x<tx+tw;x++){
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H) dh_px[y*DH_W+x]=0xFF5C3A1A;}
            /* pine layers */
            for(int layer=0;layer<3;layer++){
                int lw=sw-layer*sw/4,lh=sh/3;
                int lx=p.sx-lw/2,ly=p.sy-th-layer*lh/2;
                uint32_t gc=(layer==0)?0xFF1A5C1A:(layer==1)?0xFF206020:0xFF267026;
                POINT pts[3]={(LONG)p.sx,(LONG)(ly-lh),(LONG)(lx),(LONG)ly,(LONG)(lx+lw),(LONG)ly};
                /* draw triangle manually */
                for(int y=ly-lh;y<ly;y++){
                    float t=(float)(y-(ly-lh))/lh;
                    int x0=p.sx-(int)(t*lw/2),x1=p.sx+(int)(t*lw/2);
                    for(int x=x0;x<x1;x++)if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=gc;
                }
            }
            /* snow cap */
            for(int y=p.sy-sh;y<p.sy-sh+sh/5;y++) for(int x=p.sx-sw/8;x<p.sx+sw/8;x++)
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=0xFFEEEEFF;
            break;}

        case OBJ_ROCK:{
            int rw=sw,rh=(int)(sh*0.6f);
            for(int y=p.sy-rh;y<p.sy;y++) for(int x=p.sx-rw/2;x<p.sx+rw/2;x++){
                float nx=(float)(x-(p.sx-rw/2))/rw;
                float ny=(float)(y-(p.sy-rh))/rh;
                /* oval shape */
                float dx2=nx-0.5f,dy2=ny-0.5f;
                if(dx2*dx2*4+dy2*dy2*4>1.0f)continue;
                int bright=(int)(150+nx*30+ny*20);
                uint32_t rc=0xFF000000|((bright-20)<<16)|((bright-10)<<8)|bright;
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=rc;
            }
            /* snow on top */
            for(int y=p.sy-rh;y<p.sy-rh+rh/4;y++) for(int x=p.sx-rw/3;x<p.sx+rw/3;x++)
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=0xFFDDDDFF;
            break;}

        case OBJ_COIN:{
            int cr=max(4,sh/4);
            int cx=p.sx,cy=p.sy-sh/2;
            /* glowing coin */
            for(int y=cy-cr;y<=cy+cr;y++) for(int x=cx-cr;x<=cx+cr;x++){
                float dx2=x-cx,dy2=y-cy;
                float d=sqrtf(dx2*dx2+dy2*dy2);
                if(d>cr)continue;
                float glow=1.f-d/cr;
                int rv=(int)(255*glow),gv=(int)(200*glow),bv=(int)(20*glow);
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=0xFF000000|(rv<<16)|(gv<<8)|bv;
            }
            /* sparkle */
            if((int)(dh_time*6+o->x)%3==0){
                for(int i=-1;i<=1;i++) for(int j=-1;j<=1;j++) if(abs(i)+abs(j)==1){
                    int sx2=cx+i*(cr+2),sy2=cy+j*(cr+2);
                    if(sx2>=0&&sx2<DH_W&&sy2>=0&&sy2<DH_H)dh_px[sy2*DH_W+sx2]=0xFFFFFFFF;
                }
            }
            break;}

        case OBJ_GATE:{
            /* two poles with banner */
            int gh=sh,gw=sw*3;
            int lx=p.sx-gw/2,rx=p.sx+gw/2;
            /* poles */
            for(int y=p.sy-gh;y<p.sy;y++){
                if(lx>=0&&lx<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+lx]=0xFFFF4400;
                if(lx+1>=0&&lx+1<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+lx+1]=0xFFFF4400;
                if(rx>=0&&rx<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+rx]=0xFFFF4400;
                if(rx-1>=0&&rx-1<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+rx-1]=0xFFFF4400;
            }
            /* banner */
            int by2=p.sy-gh;
            for(int y=by2;y<by2+gh/5;y++) for(int x=lx;x<rx;x++){
                uint32_t bc=((x/6+y/6)%2)?0xFFFF2200:0xFFFFEE00;
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=bc;
            }
            break;}

        case OBJ_RAMP:{
            int rw2=sw*2,rh2=sh/3;
            for(int y=p.sy-rh2;y<p.sy;y++) for(int x=p.sx-rw2/2;x<p.sx+rw2/2;x++){
                float t=(float)(p.sy-y)/rh2;
                int bri=(int)(200+t*30);
                if(x>=0&&x<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+x]=0xFF000000|(bri<<16)|(bri<<8)|(int)(bri*1.1f);
            }
            /* ramp arrow */
            for(int y=p.sy-rh2*2;y<p.sy-rh2;y++){
                int xc=p.sx;float t=(float)(p.sy-rh2-y)/rh2;
                int hw2=(int)(8*t);
                for(int x=xc-hw2;x<xc+hw2;x++) if(x>=0&&x<DH_W&&y>=0&&y<DH_H)
                    dh_px[y*DH_W+x]=0xFF00AAFF;
            }
            break;}
        }
    }

    /*        Player skis overlay        */
    {
        int air_off=dh.airborne?(int)(dh.air_height*30):0;
        float lean=dh.lean;
        /* left ski */
        int lx=DH_W/2-30+(int)(lean*20),ly=DH_H-60+air_off;
        for(int y=ly;y<DH_H-20;y++){int x=lx+(int)((y-ly)*0.3f);if(x>=0&&x<DH_W&&y>=0&&y<DH_H){dh_px[y*DH_W+x]=0xFFCCCCDD;if(x+1<DH_W)dh_px[y*DH_W+x+1]=0xFFCCCCDD;}}
        /* right ski */
        int rx=DH_W/2+20+(int)(lean*20);int ry=ly;
        for(int y=ry;y<DH_H-20;y++){int x=rx+(int)((y-ry)*0.3f);if(x>=0&&x<DH_W&&y>=0&&y<DH_H){dh_px[y*DH_W+x]=0xFFCCCCDD;if(x+1<DH_W)dh_px[y*DH_W+x+1]=0xFFCCCCDD;}}
        /* ski poles */
        for(int y=DH_H/2+50;y<DH_H-40+air_off;y++){
            int plx=DH_W/2-60+(int)(lean*10),prx=DH_W/2+55+(int)(lean*10);
            if(plx>=0&&plx<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+plx]=0xFFAAAAAA;
            if(prx>=0&&prx<DH_W&&y>=0&&y<DH_H)dh_px[y*DH_W+prx]=0xFFAAAAAA;
        }
    }

    /*        Snow particles        */
    for(int i=0;i<MAX_SNOW;i++){
        int sx=(int)snow[i].x,sy=(int)snow[i].y;
        if(sx>=0&&sx<DH_W&&sy>=0&&sy<DH_H){
            int bri=180+rand()%75;
            dh_px[sy*DH_W+sx]=0xFF000000|(bri<<16)|(bri<<8)|bri;
        }
    }

    /*        Sparks        */
    for(int i=0;i<MAX_SPARKS;i++){
        if(sparks[i].life<=0)continue;
        int sx=(int)sparks[i].x,sy=(int)sparks[i].y;
        if(sx>=0&&sx<DH_W&&sy>=0&&sy<DH_H)dh_px[sy*DH_W+sx]=sparks[i].col;
    }

    /* hit flash */
    if(dh.hit_flash>0){
        int a=dh.hit_flash*18;if(a>180)a=180;
        for(int i=0;i<DH_W*DH_H;i++){uint32_t p=dh_px[i];int r=(int)(((p>>16)&0xFF)+a);if(r>255)r=255;dh_px[i]=(p&0xFF00FFFF)|(r<<16);}
        dh.hit_flash--;
    }
    /* gate flash */
    if(dh.gate_flash>0){
        int a=(int)(dh.gate_flash*120);if(a>120)a=120;
        for(int i=0;i<DH_W*DH_H;i++){uint32_t p=dh_px[i];int g=(int)(((p>>8)&0xFF)+a);if(g>255)g=255;dh_px[i]=(p&0xFFFF00FF)|(g<<8);}
        dh.gate_flash-=0.05f;if(dh.gate_flash<0)dh.gate_flash=0;
    }
}

/*        Update                                                                                                                                                        */
static void dh_update(void){
    if(!dh_running||!dh.started)return;
    if(dh.dead)return;

    dh_time+=0.016f;
    /* ramp speed over time */
    dh.speed=8.f+dh_time*0.8f;
    if(dh.speed>35.f)dh.speed=35.f;
    dh.dist+=dh.speed*0.016f;

    /* steering */
    float steer=0;
    if(dh_keys[VK_LEFT]||dh_keys['A'])steer=-1.f;
    if(dh_keys[VK_RIGHT]||dh_keys['D'])steer=1.f;

    float turn_speed=dh.airborne?0.04f:0.10f;
    dh.vx+=steer*turn_speed*dh.speed;
    dh.vx*=0.80f;
    if(fabsf(dh.vx)>dh.speed*0.5f)dh.vx=dh.speed*0.5f*(dh.vx>0?1:-1);
    dh.x+=dh.vx*0.016f;
    dh.lean=dh.lean*0.7f+steer*0.3f;

    /* out of bounds */
    if(dh.x<-DH_LANE_W*1.2f||dh.x>DH_LANE_W*1.2f){
        dh.lives--;dh.hit_flash=15;
        dh_spark_spawn(DH_W/2,DH_H*2/3,12,RGB(255,100,0));
        dh.x=0;dh.vx=0;dh.speed=8.f;
        if(dh.lives<=0){dh.dead=TRUE;if(dh.score>dh.high_score)dh.high_score=dh.score;}
    }

    /* airborne */
    if(dh.airborne){
        dh.air_timer+=0.016f;
        dh.air_height=sinf(dh.air_timer*2.f);
        if(dh.air_timer>1.5f){dh.airborne=FALSE;dh.air_height=0;dh.air_timer=0;}
    }

    /* combo timer */
    if(dh.combo_timer>0){dh.combo_timer-=0.016f;if(dh.combo_timer<=0)dh.combo=0;}

    /* gate bonus display */
    if(dh.gate_bonus>0){dh.gate_bonus--;}

    /* snow update */
    for(int i=0;i<MAX_SNOW;i++){
        snow[i].x+=snow[i].vx+dh.vx*0.2f;snow[i].y+=snow[i].vy+dh.speed*0.03f;
        if(snow[i].y>DH_H){snow[i].y=0;snow[i].x=((float)rand()/RAND_MAX)*DH_W;}
        if(snow[i].x<0)snow[i].x+=DH_W;if(snow[i].x>=DH_W)snow[i].x-=DH_W;
    }

    /* sparks */
    for(int i=0;i<MAX_SPARKS;i++){if(sparks[i].life<=0)continue;sparks[i].x+=sparks[i].vx;sparks[i].y+=sparks[i].vy;sparks[i].vy+=0.3f;sparks[i].life--;}

    /* move objects toward player */
    dh_terrain_scroll+=dh.speed*0.016f;
    for(int i=0;i<MAX_OBJ;i++){
        if(!dh_objs[i].active)continue;
        dh_objs[i].z-=dh.speed*0.016f;

        /* check collision when close */
        if(dh_objs[i].z<1.5f&&dh_objs[i].z>-1.f){
            float dx=fabsf(dh_objs[i].x-dh.x);
            float hit_r=dh_objs[i].type==OBJ_COIN?0.8f:dh_objs[i].type==OBJ_GATE?3.5f:0.6f;
            if(dx<hit_r){
                switch(dh_objs[i].type){
                case OBJ_TREE:
                case OBJ_ROCK:
                    dh.lives--;dh.hit_flash=20;dh.speed=8.f;dh.vx=0;
                    dh_spark_spawn(DH_W/2+(int)(dh_objs[i].x*30),DH_H/2,10,
                        dh_objs[i].type==OBJ_TREE?RGB(50,180,50):RGB(180,180,180));
                    Beep(150,60);
                    if(dh.lives<=0){dh.dead=TRUE;if(dh.score>dh.high_score)dh.high_score=dh.score;}
                    dh_objs[i].active=FALSE;break;
                case OBJ_COIN:
                    dh.coins++;dh.combo++;dh.combo_timer=2.f;
                    dh.score+=10*(dh.combo>3?dh.combo:1);
                    dh_spark_spawn(DH_W/2,DH_H/2,6,RGB(255,200,0));
                    Beep(900+dh.combo*50,20);
                    dh_objs[i].active=FALSE;break;
                case OBJ_GATE:
                    dh.gate_flash=1.f;dh.gate_bonus=60;
                    dh.score+=50;dh.speed+=2.f;
                    dh_objs[i].active=FALSE;break;
                case OBJ_RAMP:
                    if(!dh.airborne){dh.airborne=TRUE;dh.air_timer=0;dh.score+=25;}
                    dh_objs[i].active=FALSE;break;
                }
            }
        }
        if(dh_objs[i].z<-3.f)dh_objs[i].active=FALSE;
    }

    /* spawn new objects */
    dh_spawn_cd-=0.016f;
    float spawn_rate=fmaxf(0.15f,0.5f-dh_time*0.005f);
    if(dh_spawn_cd<=0){dh_spawn_obj();dh_spawn_cd=spawn_rate;}

    /* score from distance */
    dh.score=(int)(dh.dist*2)+dh.coins*10;
}

/*        HUD                                                                                                                                                                    */
static void dh_draw_hud(HDC hdc,int W,int H){
    HFONT bf=CreateFontA(24,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT sf=CreateFontA(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT of=(HFONT)SelectObject(hdc,bf);
    SetBkMode(hdc,TRANSPARENT);char buf[64];

    /* top bar */
    HBRUSH tbb=CreateSolidBrush(RGB(0,0,20));RECT tbr={0,0,W,30};FillRect(hdc,&tbr,tbb);DeleteObject(tbb);

    /* score */
    SetTextColor(hdc,RGB(255,220,40));
    snprintf(buf,63,"PUNKTE: %d",dh.score);TextOutA(hdc,8,4,buf,strlen(buf));

    /* coins */
    SetTextColor(hdc,RGB(255,180,0));
    snprintf(buf,63,"Muenzen: %d",dh.coins);TextOutA(hdc,W/2-60,4,buf,strlen(buf));

    /* speed */
    SetTextColor(hdc,RGB(100,200,255));
    snprintf(buf,63,"%.0f km/h",(double)dh.speed*8);TextOutA(hdc,W-160,4,buf,strlen(buf));

    /* lives */
    for(int i=0;i<3;i++){
        SetTextColor(hdc,i<dh.lives?RGB(255,80,80):RGB(60,60,60));
        TextOutA(hdc,W-200+i*22,4,"<3",2);
    }

    /* combo */
    if(dh.combo>=2&&dh.combo_timer>0){
        SelectObject(hdc,bf);
        SetTextColor(hdc,RGB(255,220,0));
        snprintf(buf,63,"COMBO x%d!",dh.combo);
        HFONT cbf=CreateFontA(28,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
        HFONT ocbf=(HFONT)SelectObject(hdc,cbf);
        TextOutA(hdc,W/2-60,H/2-80,buf,strlen(buf));
        SelectObject(hdc,ocbf);DeleteObject(cbf);
    }

    /* gate bonus */
    if(dh.gate_bonus>0){
        SetTextColor(hdc,RGB(100,255,100));
        TextOutA(hdc,W/2-50,H/2-50,"+50 TOR!",8);
    }

    /* airborne */
    if(dh.airborne){
        SetTextColor(hdc,RGB(100,180,255));
        TextOutA(hdc,W/2-30,H/2-30,"LUFT!",5);
    }

    /* speedometer bar */
    SelectObject(hdc,sf);
    float spd_pct=(dh.speed-8.f)/27.f;if(spd_pct>1)spd_pct=1;
    HBRUSH spdb=CreateSolidBrush(RGB(0,0,30));RECT spdbr={8,H-20,208,H-6};FillRect(hdc,&spdbr,spdb);DeleteObject(spdb);
    COLORREF sc2=spd_pct>0.8f?RGB(255,50,50):spd_pct>0.5f?RGB(255,180,0):RGB(0,200,100);
    HBRUSH spdbb=CreateSolidBrush(sc2);RECT spdbbr={8,H-20,(int)(8+200*spd_pct),H-6};FillRect(hdc,&spdbbr,spdbb);DeleteObject(spdbb);
    SetTextColor(hdc,RGB(200,200,200));TextOutA(hdc,10,H-20,"TEMPO",5);

    /* highscore */
    SetTextColor(hdc,RGB(80,80,80));
    snprintf(buf,63,"REKORD: %d",dh.high_score);TextOutA(hdc,8,32,buf,strlen(buf));

    /* controls */
    SetTextColor(hdc,RGB(50,50,70));
    TextOutA(hdc,W/2-120,H-18,"A/D oder Links/Rechts steuern  |  ESC = Verlassen",48);

    /* dead screen */
    if(dh.dead){
        HBRUSH db=CreateSolidBrush(RGB(0,0,0));RECT dr={W/2-140,H/2-60,W/2+140,H/2+80};FillRect(hdc,&dr,db);DeleteObject(db);
        HPEN dp=CreatePen(PS_SOLID,2,RGB(220,0,0));HPEN odp=(HPEN)SelectObject(hdc,dp);
        SelectObject(hdc,GetStockObject(NULL_BRUSH));Rectangle(hdc,W/2-140,H/2-60,W/2+140,H/2+80);
        SelectObject(hdc,odp);DeleteObject(dp);
        HFONT gof=CreateFontA(36,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
        HFONT ogof=(HFONT)SelectObject(hdc,gof);
        SetTextColor(hdc,RGB(220,0,0));TextOutA(hdc,W/2-90,H/2-50,"GAME OVER",9);
        SelectObject(hdc,bf);
        SetTextColor(hdc,RGB(255,220,40));
        snprintf(buf,63,"Punkte: %d",dh.score);TextOutA(hdc,W/2-65,H/2-10,buf,strlen(buf));
        snprintf(buf,63,"Muenzen: %d",dh.coins);TextOutA(hdc,W/2-65,H/2+18,buf,strlen(buf));
        SelectObject(hdc,sf);SetTextColor(hdc,RGB(180,180,180));
        TextOutA(hdc,W/2-55,H/2+50,"R = Neustart",12);
        SelectObject(hdc,ogof);DeleteObject(gof);
    }

    SelectObject(hdc,of);DeleteObject(bf);DeleteObject(sf);
}

/*        Paint                                                                                                                                                           */
static void dh_paint(HDC hdc,int W,int H){
    if(!dh_bmp||!dh_px){
        BITMAPINFO bi={0};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth=DH_W;bi.bmiHeader.biHeight=-DH_H;
        bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;
        dh_bmp=CreateDIBSection(hdc,&bi,DIB_RGB_COLORS,(void**)&dh_px,NULL,0);
        if(!dh_bmp)return;
    }
    dh_update();
    if(dh.started)dh_render();
    else memset(dh_px,0x10,DH_W*DH_H*4);

    HDC mdc=CreateCompatibleDC(hdc);
    HBITMAP ob=(HBITMAP)SelectObject(mdc,dh_bmp);
    SetStretchBltMode(hdc,HALFTONE);
    int gh=H-30;
    StretchBlt(hdc,0,0,W,gh,mdc,0,0,DH_W,DH_H,SRCCOPY);
    SelectObject(mdc,ob);DeleteDC(mdc);
    dh_draw_hud(hdc,W,H);
}
