/* Existing switches must follow live defaults exactly like newly created ones. */
#include <Xm/ToggleSwitchP.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void drain(XtAppContext app,Display *d){XSync(d,False);unsigned n=0;while(XtAppPending(app)){CHECK(++n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void extent(Widget w,Dimension *x,Dimension *y){
 XmRenderTable a=NULL,b=NULL;XtVaGetValues(w,XmNrenderTable,&a,XmNfontList,&b,NULL);CHECK(a&&a==b);
 XmString s=XmStringCreateLocalized("Kitchen lights");XmStringExtent(a,s,x,y);XmStringFree(s);
}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"SwitchLive",NULL,0,&argc,argv,NULL,XmNwidth,1400,XmNheight,1000,NULL);
 Display *d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
 CHECK(XmSetDisplayScaleOverride(d,1));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
 XmString text=XmStringCreateLocalized("Kitchen lights");
 Widget live=XtVaCreateManagedWidget("live",xmToggleSwitchWidgetClass,host,XmNlabelString,text,NULL);
 Widget fixed=XtVaCreateManagedWidget("fixed",xmToggleSwitchWidgetClass,host,XmNy,150,XmNwidth,200,XmNheight,70,XmNlabelString,text,NULL);
 XmRenderTable font=NULL;XtVaGetValues(live,XmNrenderTable,&font,NULL);
 Widget custom=XtVaCreateManagedWidget("custom",xmToggleSwitchWidgetClass,host,XmNy,300,XmNfontList,font,XmNlabelString,text,NULL);
 Dimension original_x,original_y;extent(custom,&original_x,&original_y);
 XtRealizeWidget(shell);drain(app,d);unsigned cases=0;
 double scales[]={1,1.25,1.5,2,1};
 for(unsigned theme=0;theme<3;theme++)for(unsigned density=0;density<2;density++)for(unsigned scale=0;scale<5;scale++){
  CHECK(XmApplyAppearanceProfile(d,theme==2?XmAPPEARANCE_CLASSIC:theme?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  CHECK(XmApplyAppearanceDensity(d,density?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
  CHECK(XmSetDisplayScaleOverride(d,scales[scale]));drain(app,d);
  Widget fresh=XtVaCreateManagedWidget("fresh",xmToggleSwitchWidgetClass,host,XmNy,500,XmNlabelString,text,NULL);drain(app,d);
  Dimension ax,ay,bx,by;extent(live,&ax,&ay);extent(fresh,&bx,&by);CHECK(ax==bx&&ay==by);
  fprintf(stderr,"STATE theme=%u density=%u scale=%g live=%ux%u fresh=%ux%u\n",theme,density,scales[scale],live->core.width,live->core.height,fresh->core.width,fresh->core.height);
  CHECK(live->core.width==fresh->core.width&&live->core.height==fresh->core.height);
  CHECK(fixed->core.width==200&&fixed->core.height==70);
  extent(custom,&ax,&ay);CHECK(ax==original_x&&ay==original_y);
  XtDestroyWidget(fresh);++cases;
 }
 XmStringFree(text);XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS switch live metrics: %u profile/density/scale transitions\n",cases);return 0;
}
