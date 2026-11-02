/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Original classic drawing/action icons, generated from scalable geometry.
 * No external files or image-loader dependency is required.
 */
#include "paint-icons.h"
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <math.h>

typedef struct { Display *d; Pixmap pix; GC gc; double scale; Pixel ink; int disabled; } Pen;
typedef struct { Pixmap normal, active, disabled; } Icons;
static int px(Pen *p,int n) { return (int)lround(n*p->scale); }
static Pixel rgb(Pen *p,unsigned c)
{
    Visual *v=DefaultVisual(p->d,DefaultScreen(p->d));
    unsigned masks[3]={(unsigned)v->red_mask,(unsigned)v->green_mask,(unsigned)v->blue_mask};
    unsigned values[3]={c>>16,(c>>8)&255,c&255}; Pixel out=0;int i;
    if(p->disabled) return p->ink;
    for(i=0;i<3;++i) { unsigned m=masks[i],shift=0;if(!m)continue;while(!(m&1)){m>>=1;++shift;}out|=((values[i]*m+127)/255)<<shift; }
    return out;
}
static void line(Pen *p,int x0,int y0,int x1,int y1)
{ XSetForeground(p->d,p->gc,p->ink);XDrawLine(p->d,p->pix,p->gc,px(p,x0),px(p,y0),px(p,x1),px(p,y1)); }
static void polygon(Pen *p,const int *xy,int n,unsigned color)
{
    XPoint pts[16];int i;
    for(i=0;i<n;++i){pts[i].x=px(p,xy[2*i]);pts[i].y=px(p,xy[2*i+1]);}
    XSetForeground(p->d,p->gc,rgb(p,color));XFillPolygon(p->d,p->pix,p->gc,pts,n,Complex,CoordModeOrigin);
    pts[n]=pts[0];XSetForeground(p->d,p->gc,p->ink);XDrawLines(p->d,p->pix,p->gc,pts,n+1,CoordModeOrigin);
}
static void box(Pen *p,int x,int y,int w,int h,unsigned color)
{ int xy[]={x,y,x+w,y,x+w,y+h,x,y+h};polygon(p,xy,4,color); }
static void oval(Pen *p,int x,int y,int w,int h,unsigned color,int filled)
{
    XSetForeground(p->d,p->gc,rgb(p,color));
    if(filled)XFillArc(p->d,p->pix,p->gc,px(p,x),px(p,y),px(p,w),px(p,h),0,360*64);
    XSetForeground(p->d,p->gc,p->ink);XDrawArc(p->d,p->pix,p->gc,px(p,x),px(p,y),px(p,w),px(p,h),0,360*64);
}
static void document(Pen *p,int x,int y)
{ box(p,x,y,16,21,0xfffcf0);line(p,x+4,y+7,x+12,y+7);line(p,x+4,y+11,x+12,y+11);line(p,x+4,y+15,x+10,y+15); }
static Pixmap render(Widget w,int icon,Pixel bg,Pixel ink,int disabled)
{
    Pen p; double scale=1; int side; Pixmap pix;
    XmGetWidgetScale(w,&scale);p.scale=scale*0.75;side=px(&p,32);
    p.d=XtDisplay(w);p.ink=ink;p.disabled=disabled;
    pix=XCreatePixmap(p.d,RootWindowOfScreen(XtScreen(w)),side,side,DefaultDepthOfScreen(XtScreen(w)));
    p.pix=pix;p.gc=XCreateGC(p.d,pix,0,NULL);XSetForeground(p.d,p.gc,bg);XFillRectangle(p.d,pix,p.gc,0,0,side,side);
    XSetLineAttributes(p.d,p.gc,px(&p,1)>0?px(&p,1):1,LineSolid,CapRound,JoinRound);
    switch(icon) {
    case ICON_PENCIL: {
        int body[]={8,22,23,7,27,11,12,26};int tip[]={8,22,5,29,12,26};int rubber[]={23,7,26,4,30,8,27,11};
        polygon(&p,body,4,0xf5c447);polygon(&p,tip,3,0xf1d9ae);polygon(&p,rubber,4,0xe4878c);line(&p,11,22,25,8);line(&p,5,29,7,25);break;
    }
    case ICON_BRUSH: {
        int handle[]={13,18,23,3,28,6,18,22};int ferrule[]={11,18,15,15,21,20,17,25};int bristle[]={11,20,17,25,13,29,4,29,8,25};
        polygon(&p,handle,4,0xb77a45);polygon(&p,ferrule,4,0xb8c6ce);polygon(&p,bristle,5,0x446b9a);break;
    }
    case ICON_ERASER: {
        int shape[]={4,20,17,6,28,16,15,29,11,29};int end[]={4,20,10,14,21,24,15,29,11,29};
        polygon(&p,shape,5,0xed8a9d);polygon(&p,end,5,0xf5d9dd);line(&p,15,29,29,29);break;
    }
    case ICON_LINE:line(&p,5,27,27,5);box(&p,4,26,3,3,0x397dc4);box(&p,25,3,3,3,0x397dc4);break;
    case ICON_RECTANGLE:box(&p,4,7,24,19,0xd9e9fa);break;
    case ICON_ELLIPSE:oval(&p,3,7,26,19,0xd9e9fa,1);break;
    case ICON_FILL: {
        int bucket[]={5,13,15,4,26,16,16,26};int lip[]={5,13,16,25,20,21,9,10};int drop[]={27,20,30,26,29,29,25,29,24,26};
        polygon(&p,bucket,4,0xb9c8d4);polygon(&p,lip,4,0xfffcf0);polygon(&p,drop,5,0x408ad5);line(&p,9,8,9,3);line(&p,9,3,16,3);break;
    }
    case ICON_PICKER: {
        int glass[]={7,23,19,11,23,15,11,27,5,29};int bulb[]={17,8,23,2,30,9,24,15};
        polygon(&p,glass,5,0xc6e7ef);polygon(&p,bulb,4,0x516b85);line(&p,16,9,25,18);line(&p,8,24,19,13);break;
    }
    case ICON_TEXT:
        XSetLineAttributes(p.d,p.gc,px(&p,2),LineSolid,CapButt,JoinMiter);
        line(&p,6,27,16,4);line(&p,16,4,26,27);line(&p,10,19,22,19);line(&p,3,27,11,27);line(&p,22,27,29,27);break;
    case ICON_SELECT:
        XSetForeground(p.d,p.gc,p.ink);XSetLineAttributes(p.d,p.gc,px(&p,1),LineOnOffDash,CapButt,JoinMiter);
        XDrawRectangle(p.d,p.pix,p.gc,px(&p,4),px(&p,5),px(&p,24),px(&p,22));break;
    case ICON_NEW:document(&p,8,5);break;
    case ICON_OPEN: {
        int back[]={3,9,12,9,15,12,28,12,28,27,3,27};int front[]={7,16,31,16,25,27,3,27};
        polygon(&p,back,6,0xd9aa4e);polygon(&p,front,4,0xf2cd73);break;
    }
    case ICON_SAVE:box(&p,4,4,24,24,0x6899cb);box(&p,9,4,14,8,0xe8edf1);box(&p,19,5,3,6,0x526478);box(&p,9,18,14,10,0xfffcf0);line(&p,12,22,20,22);line(&p,12,25,20,25);break;
    case ICON_UNDO:case ICON_REDO: {
        int pts[]={12,5,3,13,12,21,12,16,20,16,24,20,24,27,29,27,29,18,23,11,12,11};int i;
        if(icon==ICON_REDO)for(i=0;i<11;++i)pts[2*i]=32-pts[2*i];
        polygon(&p,pts,11,0x79ad8b);break;
    }
    case ICON_COPY:document(&p,4,3);document(&p,12,10);break;
    case ICON_CUT:
        line(&p,10,23,25,4);line(&p,21,23,7,4);oval(&p,3,21,10,8,0xd3e6f5,1);oval(&p,19,21,10,8,0xd3e6f5,1);oval(&p,14,15,3,3,0xbac9d4,1);break;
    case ICON_PASTE:box(&p,5,5,22,24,0xc69d61);box(&p,8,8,16,18,0xfffcf0);box(&p,11,3,10,6,0xb4c7d4);line(&p,11,14,21,14);line(&p,11,18,21,18);line(&p,11,22,18,22);break;
    case ICON_ZOOM_IN:case ICON_ZOOM_OUT:
        oval(&p,3,3,19,19,0xd9eff7,1);line(&p,20,20,29,29);line(&p,21,20,30,29);line(&p,7,12,18,12);
        if(icon==ICON_ZOOM_IN)line(&p,12,7,12,18);break;
    case ICON_FIT:
        box(&p,3,5,26,22,0xe6edf4);box(&p,9,10,14,12,0xb1d0eb);line(&p,3,5,8,10);line(&p,29,5,24,10);line(&p,3,27,8,22);line(&p,29,27,24,22);break;
    case ICON_RESIZE:
        box(&p,4,7,20,20,0xfffcf0);line(&p,13,18,29,2);line(&p,21,2,29,2);line(&p,29,2,29,10);line(&p,13,10,13,18);line(&p,13,18,21,18);break;
    }
    /* Center the visible artwork, not only its nominal 32-unit viewport.
     * This gives asymmetric tools (brush handles, magnifiers) equal padding. */
    {
        XImage *image=XGetImage(p.d,pix,0,0,side,side,AllPlanes,ZPixmap);
        int x,y,left=side,top=side,right=-1,bottom=-1;
        if(image) {
            for(y=0;y<side;++y)for(x=0;x<side;++x)if(XGetPixel(image,x,y)!=bg) {
                if(x<left)left=x;if(x>right)right=x;if(y<top)top=y;if(y>bottom)bottom=y;
            }
            XDestroyImage(image);
            if(right>=left && bottom>=top) {
                int width=right-left+1,height=bottom-top+1;
                Pixmap centered=XCreatePixmap(p.d,RootWindowOfScreen(XtScreen(w)),side,side,DefaultDepthOfScreen(XtScreen(w)));
                XSetForeground(p.d,p.gc,bg);XFillRectangle(p.d,centered,p.gc,0,0,side,side);
                XCopyArea(p.d,pix,centered,p.gc,left,top,width,height,(side-width)/2,(side-height)/2);
                XFreePixmap(p.d,pix);pix=centered;
            }
        }
    }
    XFreeGC(p.d,p.gc);return pix;
}
static void release(Widget w,XtPointer closure,XtPointer call)
{ Icons *icons=closure;(void)call;XFreePixmap(XtDisplay(w),icons->normal);XFreePixmap(XtDisplay(w),icons->active);XFreePixmap(XtDisplay(w),icons->disabled);free(icons); }
void paint_button_icon(Widget w,int icon,Boolean toggle)
{
    Pixel bg,fg,active,dim;double scale=1;Dimension side;Icons *icons=calloc(1,sizeof(*icons));
    if(!icons)return;
    XtVaGetValues(w,XmNbackground,&bg,XmNforeground,&fg,XmNbottomShadowColor,&dim,NULL);
    XtVaGetValues(w,toggle?XmNselectColor:XmNarmColor,&active,NULL);
    XmGetWidgetScale(w,&scale);side=(Dimension)lround(38*scale);
    icons->normal=render(w,icon,bg,fg,0);icons->active=render(w,icon,active,fg,0);icons->disabled=render(w,icon,bg,dim,1);
    XtVaSetValues(w,XmNlabelType,XmPIXMAP,XmNlabelPixmap,icons->normal,XmNlabelInsensitivePixmap,icons->disabled,
        toggle?XmNselectPixmap:XmNarmPixmap,icons->active,XmNrecomputeSize,False,
        XmNalignment,XmALIGNMENT_CENTER,
        XmNmarginWidth,0,XmNmarginHeight,0,XmNmarginLeft,0,XmNmarginRight,0,XmNmarginTop,0,XmNmarginBottom,0,XmNwidth,side,XmNheight,side,NULL);
    XtAddCallback(w,XmNdestroyCallback,release,icons);
}
