/* Count-aware natural metrics, live defaults, and bounded native drawing. */
#include <Xm/RatingControlP.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void drain(XtAppContext app,Display*d){XSync(d,False);unsigned n=0;while(XtAppPending(app)){CHECK(++n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void pixels(Widget w){
 XmRatingControlWidget p=(XmRatingControlWidget)w;p->primitive.highlighted=False;
 XtClass(w)->core_class.expose(w,NULL,NULL);XSync(XtDisplay(w),False);
 XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,p->core.width,p->core.height,AllPlanes,ZPixmap);CHECK(im);
 unsigned frame=p->primitive.highlight_thickness+p->primitive.shadow_thickness;
 for(int y=0;y<im->height;y++)for(int x=0;x<im->width;x++)if(XGetPixel(im,x,y)!=p->core.background_pixel)
  CHECK(x>=(int)frame && y>=(int)frame && x+(int)frame<im->width && y+(int)frame<im->height);
 XDestroyImage(im);
}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"RatingLayout",NULL,0,&argc,argv,NULL,XmNwidth,1500,XmNheight,1000,NULL);
 Display*d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
 Widget w=XtVaCreateManagedWidget("live",xmRatingControlWidgetClass,host,XmNratingValue,3,NULL);
 XtRealizeWidget(shell);unsigned cases=0;
 double factors[]={1,1.25,1.5,2,1};
 for(unsigned theme=0;theme<3;theme++)for(unsigned compact=0;compact<2;compact++)for(unsigned scale=0;scale<5;scale++){
  CHECK(XmApplyAppearanceProfile(d,theme==2?XmAPPEARANCE_CLASSIC:theme?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
  CHECK(XmSetDisplayScaleOverride(d,factors[scale]));drain(app,d);
  Widget fresh=XtVaCreateManagedWidget("fresh",xmRatingControlWidgetClass,host,XmNy,300,NULL);drain(app,d);
  fprintf(stderr,"STATE theme=%u compact=%u scale=%g live=%ux%u fresh=%ux%u\n",theme,compact,factors[scale],w->core.width,w->core.height,fresh->core.width,fresh->core.height);
  CHECK(w->core.width==fresh->core.width && w->core.height==fresh->core.height);pixels(w);
  Dimension base=w->core.width;XtVaSetValues(w,XmNratingCount,9,NULL);CHECK(w->core.width>base);pixels(w);
  XtVaSetValues(w,XmNwidth,99,XmNheight,41,NULL);XtVaSetValues(w,XmNratingCount,2,NULL);
  XtWidgetGeometry q={0};XtQueryGeometry(w,NULL,&q);CHECK(q.width==99 && q.height==41);pixels(w);
  XtVaSetValues(w,XmNwidth,0,NULL);CHECK(w->core.width<base && w->core.height==41);
  XtVaSetValues(w,XmNheight,0,XmNratingCount,5,NULL);CHECK(w->core.width==fresh->core.width && w->core.height==fresh->core.height);
  XtDestroyWidget(fresh);++cases;
 }
 for(unsigned count=0;count<3;count++){
  XtVaSetValues(w,XmNwidth,7,XmNheight,7,XmNratingCount,count==0?1:count==1?65536:INT_MAX,NULL);pixels(w);
 }
 XtVaSetValues(w,XmNwidth,0,NULL);XtWidgetGeometry q={0};XtQueryGeometry(w,NULL,&q);CHECK(q.width==USHRT_MAX);
 XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS rating layout: %u live cases, count/axis ownership, tiny and extreme counts\n",cases);return 0;
}
