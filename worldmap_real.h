/*
 * worldmap_real.h - Real world map bombardment
 * Uses actual country polygon data from Natural Earth / OpenStreetMap
 * Mercator projection, 199 countries, click to select, bomb to destroy
 */
#pragma once
#include "worldmap_pts.h"

/*        Bomb                                                                                                                                                              */
typedef struct {
    float x,y,tx,ty,vx,vy;
    int type;       /* 0=asteroid 1=nuke */
    BOOL active;
    float ex_r;
    int explode;
    int target;
} RBomb;

/*        Crater                                                                                                                                                        */
typedef struct { float x,y,r; COLORREF col; } RCrater;

/*        Smoke                                                                                                                                                           */
typedef struct { float x,y,vx,vy,r; int life; COLORREF col; } RSmoke;

static RBomb   rb_bombs[MAX_RBOMBS];
static RCrater rb_craters[MAX_RCRATERS];
static int     rb_n_craters=0;
static RSmoke  rb_smoke[MAX_RSMOKE];
static int     rb_n_smoke=0;

static BOOL  wmr_running=FALSE;
static int   wmr_selected=-1;
static int   wmr_bomb_type=0;
static int   wmr_score=0;
static int   wmr_nukes=5;
static float wmr_shake=0;
static char  wmr_msg[80]="";
static int   wmr_msg_timer=0;

/* camera/zoom */
static float wmr_cam_x=0,wmr_cam_y=0,wmr_zoom=1.0f;
static BOOL  wmr_keys[256]={0};

/*        coordinate transform                                                                                                              */
static void wmr_world_to_screen(float wx,float wy,int W,int H,int*sx,int*sy){
    /* apply camera and zoom */
    float fx=(wx-WMR_W/2)*wmr_zoom+WMR_W/2+wmr_cam_x;
    float fy=(wy-WMR_H/2)*wmr_zoom+WMR_H/2+wmr_cam_y;
    /* scale to window */
    *sx=(int)(fx*(float)W/WMR_W);
    *sy=(int)(fy*(float)H/WMR_H);
}
static void wmr_screen_to_world(int sx,int sy,int W,int H,float*wx,float*wy){
    float fx=(float)sx*WMR_W/W;
    float fy=(float)sy*WMR_H/H;
    *wx=(fx-WMR_W/2-wmr_cam_x)/wmr_zoom+WMR_W/2;
    *wy=(fy-WMR_H/2-wmr_cam_y)/wmr_zoom+WMR_H/2;
}

/*        Smoke                                                                                                                                                           */
static void wmr_smoke_spawn(float x,float y,int n,COLORREF col){
    for(int i=0;i<n;i++){
        if(rb_n_smoke>=MAX_RSMOKE)rb_n_smoke=0;
        RSmoke*s=&rb_smoke[rb_n_smoke++];
        float a=((float)rand()/RAND_MAX)*6.28f;
        float sp=0.3f+((float)rand()/RAND_MAX)*2.f;
        s->x=x;s->y=y;s->vx=cosf(a)*sp;s->vy=sinf(a)*sp-0.4f;
        s->r=3.f+((float)rand()/RAND_MAX)*6.f;
        s->life=25+rand()%25;s->col=col;
    }
}

/*        Drop bomb                                                                                                                                               */
/* last right-click world position for targeting */
static float wmr_aim_x=-1,wmr_aim_y=-1;
static BOOL  wmr_aim_set=FALSE;

static void wmr_drop(int ci,int type){
    if(ci<0||ci>=NUM_RCOUNTRIES||rcountries[ci].destroyed)return;
    if(type==1&&wmr_nukes<=0){strcpy(wmr_msg,"Keine Atomwaffen mehr!");wmr_msg_timer=90;return;}
    for(int i=0;i<MAX_RBOMBS;i++){
        if(!rb_bombs[i].active){
            RCountry*c=&rcountries[ci];
            RBomb*b=&rb_bombs[i];
            b->active=TRUE;
            /* target = right-click position if set, else country center */
            if(wmr_aim_set){
                b->tx=wmr_aim_x;
                b->ty=wmr_aim_y;
            } else {
                b->tx=(float)c->cx+((float)rand()/RAND_MAX-0.5f)*8;
                b->ty=(float)c->cy+((float)rand()/RAND_MAX-0.5f)*8;
            }
            /* start from random top position above target */
            b->x=b->tx+((float)rand()/RAND_MAX-0.5f)*80;
            b->y=-30.f;
            float dx=b->tx-b->x,dy=b->ty-b->y;
            float dist=sqrtf(dx*dx+dy*dy);if(dist<1)dist=1;
            float spd=type==1?3.5f:2.5f;
            b->vx=dx/dist*spd;b->vy=dy/dist*spd;
            b->type=type;b->explode=0;b->ex_r=0;b->target=ci;
            if(type==1)wmr_nukes--;
            snprintf(wmr_msg,sizeof(wmr_msg),type==0?"Asteroid -> %s!":"NUKE -> %s!",c->name);
            wmr_msg_timer=90;
            return;
        }
    }
}

/*        Hit test: point in polygon                                                                                            */
static BOOL wmr_point_in_poly(float px,float py,const POINT*pts,int n){
    BOOL inside=FALSE;
    for(int i=0,j=n-1;i<n;j=i++){
        float xi=pts[i].x,yi=pts[i].y,xj=pts[j].x,yj=pts[j].y;
        if(((yi>py)!=(yj>py))&&(px<(xj-xi)*(py-yi)/(yj-yi)+xi))
            inside=!inside;
    }
    return inside;
}

/*        Find country at world pos                                                                                               */
static int wmr_country_at(float wx,float wy){
    /* first check centroid distance for speed */
    int best=-1;float best_d=30.f;
    for(int i=0;i<NUM_RCOUNTRIES;i++){
        if(rcountries[i].destroyed)continue;
        float dx=wx-rcountries[i].cx,dy=wy-rcountries[i].cy;
        float d=sqrtf(dx*dx+dy*dy);
        if(d<best_d){best_d=d;best=i;}
    }
    /* also do point-in-polygon for top candidates */
    for(int i=0;i<NUM_RCOUNTRIES;i++){
        if(rcountries[i].destroyed)continue;
        float dx=wx-rcountries[i].cx,dy=wy-rcountries[i].cy;
        if(dx*dx+dy*dy>80.f*80.f)continue;
        if(wmr_point_in_poly(wx,wy,rcountries[i].pts,rcountries[i].n_pts))
            return i;
    }
    return best;
}

/*        Update                                                                                                                                                        */
static void wmr_update(void){
    if(!wmr_running)return;
    if(wmr_shake>0)wmr_shake-=0.4f;
    if(wmr_msg_timer>0)wmr_msg_timer--;

    /* camera pan with arrow keys */
    float pan_spd=4.f/wmr_zoom;
    if(wmr_keys[VK_LEFT]||wmr_keys['A'])wmr_cam_x+=pan_spd;
    if(wmr_keys[VK_RIGHT]||wmr_keys['D'])wmr_cam_x-=pan_spd;
    if(wmr_keys[VK_UP]||wmr_keys['W'])wmr_cam_y+=pan_spd;
    if(wmr_keys[VK_DOWN]||wmr_keys['S'])wmr_cam_y-=pan_spd;

    /* update bombs */
    for(int i=0;i<MAX_RBOMBS;i++){
        RBomb*b=&rb_bombs[i];if(!b->active)continue;
        if(b->explode>0){
            b->explode--;
            b->ex_r+=b->type==0?1.8f:3.5f;
            float mr=b->type==0?20.f:55.f;
            if(b->ex_r>mr)b->ex_r=mr;
            wmr_smoke_spawn(b->tx,b->ty,2,b->type==0?RGB(220,100,30):RGB(220,220,60));
            if(b->explode<=0)b->active=FALSE;
            continue;
        }
        b->x+=b->vx;b->y+=b->vy;
        /* wobble asteroid */
        if(b->type==0)b->x+=sinf(b->y*0.08f)*0.4f;
        float dx=b->tx-b->x,dy=b->ty-b->y;
        if(dx*dx+dy*dy<25.f){
            b->explode=45;b->ex_r=2.f;b->x=b->tx;b->y=b->ty;
            wmr_shake=b->type==0?15.f:35.f;
            RCountry*c=&rcountries[b->target];
            if(!c->destroyed){
                c->hp--;
                if(c->hp<=0){
                    c->destroyed=TRUE;
                    int pts=b->type==0?100:350;
                    wmr_score+=pts;
                    snprintf(wmr_msg,sizeof(wmr_msg),"%s zerstoert! +%d",c->name,pts);
                    wmr_msg_timer=150;
                }
            }
            if(rb_n_craters<MAX_RCRATERS){
                rb_craters[rb_n_craters].x=b->tx;
                rb_craters[rb_n_craters].y=b->ty;
                rb_craters[rb_n_craters].r=b->type==0?5.f:14.f;
                rb_craters[rb_n_craters].col=b->type==0?RGB(60,40,20):RGB(80,70,0);
                rb_n_craters++;
            }
            wmr_smoke_spawn(b->tx,b->ty,b->type==0?10:28,b->type==0?RGB(255,130,0):RGB(255,230,60));
        }
        if(b->y>WMR_H+50)b->active=FALSE;
    }
    /* smoke */
    for(int i=0;i<rb_n_smoke;i++){RSmoke*s=&rb_smoke[i];if(s->life<=0)continue;s->x+=s->vx;s->y+=s->vy;s->r+=0.15f;s->life--;}
}

/*        Draw                                                                                                                                                              */
static void wmr_draw(HDC hdc,int W,int H){
    HDC mdc=CreateCompatibleDC(hdc);
    HBITMAP bmp=CreateCompatibleBitmap(hdc,W,H);
    HBITMAP ob=(HBITMAP)SelectObject(mdc,bmp);

    int shx=(int)(wmr_shake>0?(((float)rand()/RAND_MAX-0.5f)*wmr_shake*0.4f):0);
    int shy=(int)(wmr_shake>0?(((float)rand()/RAND_MAX-0.5f)*wmr_shake*0.25f):0);

    /* ocean */
    HBRUSH ocb=CreateSolidBrush(RGB(18,40,80));
    RECT ocr={0,0,W,H};FillRect(mdc,&ocr,ocb);DeleteObject(ocb);

    /* grid lines (lat/lon) */
    HPEN gp=CreatePen(PS_SOLID,1,RGB(22,50,95));HPEN ogp=(HPEN)SelectObject(mdc,gp);
    /* vertical lines every 30 degrees lon */
    for(int lon=-180;lon<=180;lon+=30){
        float wx=(float)((lon+180.0)/360.0*WMR_W);
        int sx1,sy1,sx2,sy2;
        wmr_world_to_screen(wx,0,W,H,&sx1,&sy1);
        wmr_world_to_screen(wx,(float)WMR_H,W,H,&sx2,&sy2);
        MoveToEx(mdc,sx1+shx,sy1+shy,NULL);LineTo(mdc,sx2+shx,sy2+shy);
    }
    /* horizontal lines every 30 degrees lat */
    for(int lat=-60;lat<=80;lat+=30){
        float wy=0;
        /* compute y from lat using same mercator formula */
        double lr=lat*3.14159/180.0;
        double y2=log(tan(3.14159/4+lr/2));
        wy=(float)((1.0-(y2+3.14159)/(2*3.14159))*WMR_H);
        int sx1,sy1,sx2,sy2;
        wmr_world_to_screen(0,wy,W,H,&sx1,&sy1);
        wmr_world_to_screen((float)WMR_W,wy,W,H,&sx2,&sy2);
        MoveToEx(mdc,sx1+shx,sy1+shy,NULL);LineTo(mdc,sx2+shx,sy2+shy);
    }
    SelectObject(mdc,ogp);DeleteObject(gp);

    /* draw countries */
    for(int i=0;i<NUM_RCOUNTRIES;i++){
        RCountry*c=&rcountries[i];
        COLORREF fill=c->destroyed?RGB(40,40,40):c->base_color;
        /* selected: brighter */
        if(i==wmr_selected&&!c->destroyed){
            int r=min(255,GetRValue(fill)+60);
            int g2=min(255,GetGValue(fill)+60);
            int b2=min(255,GetBValue(fill)+60);
            fill=RGB(r,g2,b2);
        }
        HBRUSH cb=CreateSolidBrush(fill);
        HPEN cp=CreatePen(PS_SOLID,1,RGB(0,20,50));
        HBRUSH ocb2=(HBRUSH)SelectObject(mdc,cb);
        HPEN ocp=(HPEN)SelectObject(mdc,cp);

        /* transform polygon points */
        POINT *tpts=(POINT*)malloc(c->n_pts*sizeof(POINT));
        for(int j=0;j<c->n_pts;j++){
            int sx,sy;
            wmr_world_to_screen((float)c->pts[j].x,(float)c->pts[j].y,W,H,&sx,&sy);
            tpts[j].x=sx+shx;tpts[j].y=sy+shy;
        }
        Polygon(mdc,tpts,c->n_pts);
        free(tpts);
        SelectObject(mdc,ocb2);SelectObject(mdc,ocp);DeleteObject(cb);DeleteObject(cp);
    }

    /* craters */
    for(int i=0;i<rb_n_craters;i++){
        int sx,sy;
        wmr_world_to_screen(rb_craters[i].x,rb_craters[i].y,W,H,&sx,&sy);
        int r=(int)(rb_craters[i].r*wmr_zoom);if(r<2)r=2;
        HBRUSH crb=CreateSolidBrush(rb_craters[i].col);HPEN crp=CreatePen(PS_NULL,0,0);
        HBRUSH ocrb=(HBRUSH)SelectObject(mdc,crb);HPEN ocrp=(HPEN)SelectObject(mdc,crp);
        Ellipse(mdc,sx-r+shx,sy-r+shy,sx+r+shx,sy+r+shy);
        SelectObject(mdc,ocrb);SelectObject(mdc,ocrp);DeleteObject(crb);DeleteObject(crp);
    }

    /* country labels (only when zoomed in enough or selected) */
    HFONT lf=CreateFontA(max(9,(int)(11*wmr_zoom)),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT olf=(HFONT)SelectObject(mdc,lf);
    SetBkMode(mdc,TRANSPARENT);
    for(int i=0;i<NUM_RCOUNTRIES;i++){
        RCountry*c=&rcountries[i];
        if(c->destroyed)continue;
        int sx,sy;
        wmr_world_to_screen((float)c->cx,(float)c->cy,W,H,&sx,&sy);
        if(sx<-20||sx>W+20||sy<0||sy>H)continue;
        /* only show label when zoomed or selected */
        if(wmr_zoom<1.5f&&i!=wmr_selected)continue;
        SetTextColor(mdc,i==wmr_selected?RGB(255,255,0):RGB(255,255,255));
        TextOutA(mdc,sx-strlen(c->name)*3+shx,sy-6+shy,c->name,strlen(c->name));
        /* HP dots */
        for(int h=0;h<c->hp;h++){
            HBRUSH hb=CreateSolidBrush(h<c->hp?RGB(0,230,0):RGB(60,60,60));
            HPEN hp=CreatePen(PS_NULL,0,0);
            HBRUSH ohb=(HBRUSH)SelectObject(mdc,hb);HPEN ohp=(HPEN)SelectObject(mdc,hp);
            Ellipse(mdc,sx-8+h*7+shx,sy+6+shy,sx-3+h*7+shx,sy+11+shy);
            SelectObject(mdc,ohb);SelectObject(mdc,ohp);DeleteObject(hb);DeleteObject(hp);
        }
    }
    SelectObject(mdc,olf);DeleteObject(lf);

    /* smoke */
    for(int i=0;i<rb_n_smoke;i++){
        RSmoke*s=&rb_smoke[i];if(s->life<=0)continue;
        int sx,sy;wmr_world_to_screen(s->x,s->y,W,H,&sx,&sy);
        int r=(int)(s->r*wmr_zoom);if(r<1)r=1;
        HBRUSH sb=CreateSolidBrush(s->col);HPEN sp=CreatePen(PS_NULL,0,0);
        HBRUSH osb=(HBRUSH)SelectObject(mdc,sb);HPEN osp=(HPEN)SelectObject(mdc,sp);
        Ellipse(mdc,sx-r+shx,sy-r+shy,sx+r+shx,sy+r+shy);
        SelectObject(mdc,osb);SelectObject(mdc,osp);DeleteObject(sb);DeleteObject(sp);
    }

    /* bombs */
    for(int i=0;i<MAX_RBOMBS;i++){
        RBomb*b=&rb_bombs[i];if(!b->active)continue;
        int bsx,bsy;wmr_world_to_screen(b->x,b->y,W,H,&bsx,&bsy);
        int tsx,tsy;wmr_world_to_screen(b->tx,b->ty,W,H,&tsx,&tsy);
        if(b->explode>0){
            int r=(int)(b->ex_r*wmr_zoom);if(r<2)r=2;
            COLORREF ec=b->type==0?RGB(255,130,0):RGB(255,230,60);
            HBRUSH eb=CreateSolidBrush(ec);HPEN ep=CreatePen(PS_SOLID,1,RGB(255,255,200));
            HBRUSH oeb=(HBRUSH)SelectObject(mdc,eb);HPEN oep=(HPEN)SelectObject(mdc,ep);
            Ellipse(mdc,tsx-r+shx,tsy-r+shy,tsx+r+shx,tsy+r+shy);
            SelectObject(mdc,oeb);SelectObject(mdc,oep);DeleteObject(eb);DeleteObject(ep);
            /* mushroom cloud for nuke */
            if(b->type==1&&r>8){
                HBRUSH mb=CreateSolidBrush(RGB(160,160,160));HPEN mp=CreatePen(PS_NULL,0,0);
                HBRUSH omb=(HBRUSH)SelectObject(mdc,mb);HPEN omp=(HPEN)SelectObject(mdc,mp);
                Ellipse(mdc,tsx-r/2+shx,tsy-r*2+shy,tsx+r/2+shx,tsy-r+shy);
                SelectObject(mdc,omb);SelectObject(mdc,omp);DeleteObject(mb);DeleteObject(mp);
            }
        } else {
            /* trajectory dotted line */
            HPEN tp=CreatePen(PS_DOT,1,b->type==0?RGB(200,100,0):RGB(200,50,50));
            HPEN otp=(HPEN)SelectObject(mdc,tp);
            MoveToEx(mdc,bsx+shx,bsy+shy,NULL);LineTo(mdc,tsx+shx,tsy+shy);
            SelectObject(mdc,otp);DeleteObject(tp);
            if(b->type==0){
                /* asteroid */
                int r=(int)(5*wmr_zoom);if(r<3)r=3;
                HBRUSH ab=CreateSolidBrush(RGB(150,110,60));HPEN ap=CreatePen(PS_SOLID,1,RGB(220,180,80));
                HBRUSH oab=(HBRUSH)SelectObject(mdc,ab);HPEN oap=(HPEN)SelectObject(mdc,ap);
                POINT aps[6];
                for(int k=0;k<6;k++){float a=k*1.047f;float ar=r+sinf(a*2)*r*0.4f;aps[k].x=bsx+shx+(LONG)(cosf(a)*ar);aps[k].y=bsy+shy+(LONG)(sinf(a)*ar);}
                Polygon(mdc,aps,6);
                /* trail */
                HPEN trp=CreatePen(PS_SOLID,2,RGB(255,150,0));HPEN otrp=(HPEN)SelectObject(mdc,trp);
                MoveToEx(mdc,bsx+shx,bsy+shy,NULL);LineTo(mdc,(int)(bsx-b->vx*5)+shx,(int)(bsy-b->vy*5)+shy);
                SelectObject(mdc,otrp);DeleteObject(trp);
                SelectObject(mdc,oab);SelectObject(mdc,oap);DeleteObject(ab);DeleteObject(ap);
            } else {
                /* nuke missile */
                HBRUSH nb=CreateSolidBrush(RGB(210,210,210));HPEN np=CreatePen(PS_SOLID,1,RGB(255,0,0));
                HBRUSH onb=(HBRUSH)SelectObject(mdc,nb);HPEN onp=(HPEN)SelectObject(mdc,np);
                POINT ms[]={(LONG)bsx+shx,(LONG)(bsy-6)+shy,(LONG)(bsx+4)+shx,(LONG)(bsy+6)+shy,(LONG)(bsx-4)+shx,(LONG)(bsy+6)+shy};
                Polygon(mdc,ms,3);
                SelectObject(mdc,onb);SelectObject(mdc,onp);DeleteObject(nb);DeleteObject(np);
                /* flame */
                HBRUSH fb=CreateSolidBrush(RGB(255,100,0));HPEN fp=CreatePen(PS_NULL,0,0);
                HBRUSH ofb=(HBRUSH)SelectObject(mdc,fb);HPEN ofp=(HPEN)SelectObject(mdc,fp);
                POINT fl[]={(LONG)(bsx-3)+shx,(LONG)(bsy+6)+shy,(LONG)bsx+shx,(LONG)(bsy+12)+shy,(LONG)(bsx+3)+shx,(LONG)(bsy+6)+shy};
                Polygon(mdc,fl,3);SelectObject(mdc,ofb);SelectObject(mdc,ofp);DeleteObject(fb);DeleteObject(fp);
            }
        }
    }

    /*        HUD        */
    HFONT hf=CreateFontA(17,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT hsf=CreateFontA(12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");
    HFONT ohf=(HFONT)SelectObject(mdc,hf);SetBkMode(mdc,TRANSPARENT);char buf[80];

    /* top bar */
    HBRUSH tbb=CreateSolidBrush(RGB(0,0,0));RECT tbr={0,0,W,26};FillRect(mdc,&tbr,tbb);DeleteObject(tbb);
    SetTextColor(mdc,RGB(255,220,40));snprintf(buf,79,"SCORE: %d",wmr_score);TextOutA(mdc,6,4,buf,strlen(buf));
    SetTextColor(mdc,wmr_nukes>0?RGB(255,80,80):RGB(80,40,40));
    snprintf(buf,79,"NUKES: %d",wmr_nukes);TextOutA(mdc,200,4,buf,strlen(buf));
    SetTextColor(mdc,RGB(100,180,255));
    snprintf(buf,79,"ZOOM: %.1fx  WASD/Pfeile=verschieben",wmr_zoom);
    SelectObject(mdc,hsf);TextOutA(mdc,380,7,buf,strlen(buf));

    /* selected info */
    if(wmr_selected>=0&&!rcountries[wmr_selected].destroyed){
        SelectObject(mdc,hf);
        SetTextColor(mdc,RGB(255,255,200));
        snprintf(buf,79,"[ %s ]  HP:%d  1=Asteroid  2=Nuke  ENTER=Abfeuern",
            rcountries[wmr_selected].name,rcountries[wmr_selected].hp);
        RECT sel_r={0,H-26,W,H};HBRUSH selb=CreateSolidBrush(RGB(0,0,20));FillRect(mdc,&sel_r,selb);DeleteObject(selb);
        TextOutA(mdc,6,H-22,buf,strlen(buf));
    } else {
        SelectObject(mdc,hsf);SetTextColor(mdc,RGB(70,70,70));
        RECT sr={0,H-22,W,H};HBRUSH sb2=CreateSolidBrush(RGB(0,0,0));FillRect(mdc,&sr,sb2);DeleteObject(sb2);
        TextOutA(mdc,6,H-18,"Klick=Land auswaehlen | +/-=Zoom | R=Reset | Scroll=Zoom",55);
    }

    /* weapon selector */
    {RECT w1={W-210,H-26,W-110,H};HBRUSH wb=CreateSolidBrush(wmr_bomb_type==0?RGB(40,60,140):RGB(15,15,30));FillRect(mdc,&w1,wb);DeleteObject(wb);}
    {RECT w2={W-105,H-26,W,H};HBRUSH wb=CreateSolidBrush(wmr_bomb_type==1?RGB(100,20,20):RGB(15,15,30));FillRect(mdc,&w2,wb);DeleteObject(wb);}
    SelectObject(mdc,hsf);
    SetTextColor(mdc,wmr_bomb_type==0?RGB(255,255,255):RGB(100,100,100));TextOutA(mdc,W-204,H-20,"[1] Asteroid",12);
    SetTextColor(mdc,wmr_bomb_type==1?RGB(255,100,100):RGB(100,100,100));TextOutA(mdc,W-100,H-20,"[2] Atombombe",13);

    /* message */
    if(wmr_msg_timer>0){
        SelectObject(mdc,hf);float al=(float)wmr_msg_timer/90.f;
        SetTextColor(mdc,RGB((int)(255*al),(int)(200*al),(int)(50*al)));
        int len=(int)strlen(wmr_msg)*10;
        TextOutA(mdc,W/2-len/2,H/2-14,wmr_msg,strlen(wmr_msg));
    }

    SelectObject(mdc,ohf);DeleteObject(hf);DeleteObject(hsf);
    BitBlt(hdc,0,0,W,H,mdc,0,0,SRCCOPY);
    SelectObject(mdc,ob);DeleteObject(bmp);DeleteDC(mdc);
}

/*        Click handler                                                                                                                                   */
static void wmr_click(int sx,int sy,int W,int H){
    float wx,wy;wmr_screen_to_world(sx,sy,W,H,&wx,&wy);
    wmr_selected=wmr_country_at(wx,wy);
}

/*        Scroll zoom                                                                                                                                         */
static void wmr_zoom_at(int sx,int sy,int W,int H,float factor){
    /* zoom toward mouse position */
    float wx,wy;wmr_screen_to_world(sx,sy,W,H,&wx,&wy);
    wmr_zoom*=factor;
    if(wmr_zoom<0.5f)wmr_zoom=0.5f;
    if(wmr_zoom>8.f)wmr_zoom=8.f;
    /* adjust camera to keep point under mouse */
    float nx,ny;wmr_screen_to_world(sx,sy,W,H,&nx,&ny);
    wmr_cam_x+=(nx-wx)*wmr_zoom;
    wmr_cam_y+=(ny-wy)*wmr_zoom;
}

/*        Reset                                                                                                                                                           */
static void wmr_reset(void){
    for(int i=0;i<NUM_RCOUNTRIES;i++){rcountries[i].destroyed=FALSE;rcountries[i].hp=3;}
    memset(rb_bombs,0,sizeof(rb_bombs));
    rb_n_craters=0;rb_n_smoke=0;wmr_score=0;wmr_nukes=5;
    wmr_selected=-1;wmr_shake=0;wmr_cam_x=0;wmr_cam_y=0;wmr_zoom=1.0f;
    wmr_aim_set=FALSE;wmr_aim_x=-1;wmr_aim_y=-1;
    strcpy(wmr_msg,"Echte Weltkarte! Rechtsklick=Ziel setzen, ENTER=Abfeuern");
    wmr_msg_timer=200;wmr_running=TRUE;
}
