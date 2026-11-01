/* Native raster geometry: complete rings, sectors, and actual controls. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/ProgressRingP.h>
#include <Xm/SpinnerP.h>
#include "AppearanceDrawI.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned checks;
static void drain(XtAppContext app,Display*d){XSync(d,False);while(XtAppPending(app)){XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void clear(Display*d,Pixmap p,GC g,Pixel white){XSetForeground(d,g,white);XFillRectangle(d,p,g,0,0,100,100);XSetForeground(d,g,BlackPixel(d,DefaultScreen(d)));}
int main(int argc,char **argv){
 XtAppContext app;XtToolkitInitialize();app=XtCreateApplicationContext();
 Display*d=XtOpenDisplay(app,NULL,"ringRaster","RingRaster",NULL,0,&argc,argv);CHECK(d);
 Pixel black=BlackPixel(d,DefaultScreen(d)),white=WhitePixel(d,DefaultScreen(d));
 Pixmap p=XCreatePixmap(d,DefaultRootWindow(d),100,100,DefaultDepth(d,DefaultScreen(d)));GC g=XCreateGC(d,p,0,NULL);
 const int sizes[]={1,2,3,4,7,8,15,16,31,32,63,64};
 const int strokes[]={1,2,3,4,6,100};
 for(unsigned n=0;n<XtNumber(sizes);++n)for(unsigned t=0;t<XtNumber(strokes);++t){
  int edge=sizes[n];clear(d,p,g,white);_XmAppearanceDrawRing(d,p,g,4,4,edge,strokes[t],0,360*64);
  XImage *full=XGetImage(d,p,0,0,100,100,AllPlanes,ZPixmap);CHECK(full);
  int top=0,bottom=0,left=0,right=0;
  int inner=edge-2*strokes[t];if(inner<0)inner=0;
  for(int y=0;y<edge;++y)for(int x=0;x<edge;++x){
   int dx=2*x+1-edge,dy=2*y+1-edge,distance=dx*dx+dy*dy;
   CHECK((XGetPixel(full,x+4,y+4)==black)==(distance<=edge*edge&&distance>=inner*inner));
  }
  XImage *phase_zero=NULL;
  for(int y=0;y<100;++y)for(int x=0;x<100;++x)if(XGetPixel(full,x,y)==black){
   CHECK(x>=4&&y>=4&&x<4+edge&&y<4+edge);
   CHECK(XGetPixel(full,7+edge-x,y)==black);CHECK(XGetPixel(full,x,7+edge-y)==black);
   CHECK(XGetPixel(full,y,x)==black);
   top|=y==4;bottom|=y==edge+3;left|=x==4;right|=x==edge+3;
  }
  CHECK(top&&bottom&&left&&right);++checks;
  for(unsigned phase=0;phase<12;++phase){
   clear(d,p,g,white);_XmAppearanceDrawRing(d,p,g,4,4,edge,strokes[t],phase*30*64,250*64);
   XImage *part=XGetImage(d,p,0,0,100,100,AllPlanes,ZPixmap);CHECK(part);
   for(int y=0;y<100;++y)for(int x=0;x<100;++x)
    if(XGetPixel(part,x,y)==black)CHECK(XGetPixel(full,x,y)==black);
   if(phase==3){
    for(int y=0;y<edge;++y)for(int x=0;x<edge;++x)
     CHECK(XGetPixel(part,x+4,y+4)==XGetPixel(phase_zero,4+edge-1-y,4+x));
   }
   if(phase==0)phase_zero=part;else XDestroyImage(part);
   ++checks;
  }
  XDestroyImage(phase_zero);
  /* Quarter sectors must tile precisely the complete ring. */
  clear(d,p,g,white);for(int quadrant=0;quadrant<4;++quadrant)
   _XmAppearanceDrawRing(d,p,g,4,4,edge,strokes[t],quadrant*90*64,90*64);
  XImage *quarters=XGetImage(d,p,0,0,100,100,AllPlanes,ZPixmap);CHECK(quarters);
  for(int y=0;y<100;++y)for(int x=0;x<100;++x)CHECK(XGetPixel(quarters,x,y)==XGetPixel(full,x,y));
  XDestroyImage(quarters);XDestroyImage(full);++checks;
 }
 /* Clockwise progress starts at twelve o'clock and fills the right half. */
 clear(d,p,g,white);_XmAppearanceDrawRing(d,p,g,4,4,32,4,90*64,-180*64);
 XImage *half=XGetImage(d,p,0,0,100,100,AllPlanes,ZPixmap);CHECK(half);
 CHECK(XGetPixel(half,35,20)==black);CHECK(XGetPixel(half,4,20)==white);
 XDestroyImage(half);++checks;
 XFreeGC(d,g);XFreePixmap(d,p);
 const double scales[]={1,1.5,2};
 for(unsigned profile=0;profile<2;++profile)for(unsigned scale=0;scale<3;++scale){
  CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
  CHECK(XmApplyAppearanceProfile(d,profile?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  Widget shell=XtAppCreateShell("rings","Rings",applicationShellWidgetClass,d,NULL,0);
  Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,700,XmNheight,400,NULL);
  for(unsigned n=0;n<XtNumber(sizes);++n){
   int edge=sizes[n];
   Widget ring=XtVaCreateManagedWidget("ring",xmProgressRingWidgetClass,board,
    XmNx,10+n*50,XmNy,10,XmNwidth,edge,XmNheight,edge,
    XmNprogressTextVisible,False,XmNtraversalOn,False,NULL);
   Widget spinner=XtVaCreateManagedWidget("spinner",xmSpinnerWidgetClass,board,
    XmNx,10+n*50,XmNy,100,XmNwidth,edge,XmNheight,edge,XmNtraversalOn,False,NULL);
   if(!XtIsRealized(shell))XtRealizeWidget(shell);
   drain(app,d);
   int x,y,diameter;Dimension spacing,focus;XtVaGetValues(ring,XmNspacing,&spacing,XmNhighlightThickness,&focus,NULL);
   _XmAppearanceRingBounds(edge,edge,spacing,focus,32767,&x,&y,&diameter);
   CHECK(diameter>0&&x>=0&&y>=0&&x+diameter<=edge&&y+diameter<=edge);
   XmProgressRingWidget r=(XmProgressRingWidget)ring;
   XImage *im=XGetImage(d,XtWindow(ring),0,0,edge,edge,AllPlanes,ZPixmap);CHECK(im);
   unsigned painted=0;
   for(int yy=0;yy<edge;++yy)for(int xx=0;xx<edge;++xx){
    painted += XGetPixel(im,xx,yy)!=r->core.background_pixel;
    CHECK(XGetPixel(im,xx,yy)==XGetPixel(im,edge-1-xx,yy));
    CHECK(XGetPixel(im,xx,yy)==XGetPixel(im,xx,edge-1-yy));
    if(xx<x||yy<y||xx>=x+diameter||yy>=y+diameter)CHECK(XGetPixel(im,xx,yy)==r->core.background_pixel);
   }
   CHECK(painted>0);
   if(edge<=16){
    XtVaSetValues(ring,XmNprogressTextVisible,True,NULL);drain(app,d);
    XImage *with_text=XGetImage(d,XtWindow(ring),0,0,edge,edge,AllPlanes,ZPixmap);CHECK(with_text);
    for(int yy=0;yy<edge;++yy)for(int xx=0;xx<edge;++xx)
     CHECK(XGetPixel(im,xx,yy)==XGetPixel(with_text,xx,yy));
    XDestroyImage(with_text);
   }
   XDestroyImage(im);++checks;
   XmSpinnerWidget sp=(XmSpinnerWidget)spinner;XtVaGetValues(spinner,XmNspacing,&spacing,XmNhighlightThickness,&focus,NULL);
   _XmAppearanceRingBounds(edge,edge,spacing,focus,(int)(32*scales[scale]+.5),&x,&y,&diameter);
   x=(edge-diameter)/2;
   unsigned long previous_hash=0;
   for(unsigned phase=0;phase<12;++phase){
    sp->spinner.phase=phase;XClearArea(d,XtWindow(spinner),0,0,0,0,True);drain(app,d);
    im=XGetImage(d,XtWindow(spinner),0,0,edge,edge,AllPlanes,ZPixmap);CHECK(im);
    for(int yy=0;yy<edge;++yy)for(int xx=0;xx<edge;++xx)
     if(xx<x||yy<y||xx>=x+diameter||yy>=y+diameter)CHECK(XGetPixel(im,xx,yy)==sp->core.background_pixel);
    unsigned long hash=5381;painted=0;
    for(int yy=0;yy<edge;++yy)for(int xx=0;xx<edge;++xx){
     Pixel pixel=XGetPixel(im,xx,yy);painted+=pixel!=sp->core.background_pixel;
     hash=((hash<<5)+hash)^pixel;
    }
    CHECK(painted>0);
    if(phase&&diameter>=15)CHECK(hash!=previous_hash);
    previous_hash=hash;XDestroyImage(im);++checks;
   }
   XtDestroyWidget(ring);XtDestroyWidget(spinner);drain(app,d);
  }
  XtDestroyWidget(shell);drain(app,d);
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS ring raster: %u cases\n",checks);return 0;
}
