/* Partial tab repaints must be pixel-identical to a complete redraw. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/TabStack.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);exit(1);} } while(0)
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static void inside(Widget child,Widget parent){
 CHECK(child && parent);
 CHECK(XtX(child)>=0 && XtY(child)>=0);
 CHECK(XtWidth(child)>0 && XtHeight(child)>0);
 CHECK((unsigned)XtX(child)+XtWidth(child)<=XtWidth(parent));
 CHECK((unsigned)XtY(child)+XtHeight(child)<=XtHeight(parent));
}
int main(int argc,char**argv){
 XtAppContext a;Display*d;unsigned cases=0;
 XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
 double scales[]={1,1.5,2};
 unsigned char sides[]={XmTABS_ON_TOP,XmTABS_ON_BOTTOM,XmTABS_ON_LEFT,XmTABS_ON_RIGHT};
 unsigned extended=getenv("XM_TAB_EXTENDED")!=NULL;
 unsigned modes_count=extended?3:1, payloads=extended?3:1, sizes=extended?2:1;
 XmTabMode modes[]={XmTABS_BASIC,XmTABS_STACKED,XmTABS_STACKED_STATIC};
 XmTabStyle styles[]={XmTABS_BEVELED,XmTABS_SQUARED,XmTABS_ROUNDED};
 XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"repaint","Repaint",NULL,0,&argc,argv);CHECK(d);
 for(unsigned p=0;p<4;p++)for(unsigned scale=0;scale<3;scale++)for(unsigned side=0;side<4;side++)for(unsigned style=0;style<3;style++)
 for(unsigned mode=0;mode<modes_count;mode++)for(unsigned payload=0;payload<payloads;payload++)for(unsigned size=0;size<sizes;size++){
  char case_name[128];snprintf(case_name,sizeof(case_name),"p%u-s%g-side%u-style%u-mode%u-payload%u-size%u",p,scales[scale],side,style,mode,payload,size);
  const char *only=getenv("XM_TAB_REPAINT_CASE");if(only && strcmp(only,case_name))continue;
  printf("CASE p%u scale%g side%u style%u mode%u payload%u size%u\n",p,scales[scale],side,style,mode,payload,size);fflush(stdout);
  CHECK(XmApplyAppearanceProfile(d,profiles[p]));CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
  Widget shell=XtAppCreateShell("repaint","Repaint",applicationShellWidgetClass,d,NULL,0);
  XtVaSetValues(shell,XtNwidth,size?420:900,XtNheight,size?360:700,NULL);
  Widget stack=XtVaCreateManagedWidget("stack",xmTabStackWidgetClass,shell,XmNtabSide,sides[side],XmNtabStyle,styles[style],XmNtabMode,modes[mode],XmNuniformTabSize,True,NULL);
  static const char bits[]={0x7e,0x42,0x5a,0x5a,0x42,0x7e,0x00,0x00};
  Pixmap icon=XmUNSPECIFIED_PIXMAP;
  if(payload==1) icon=XCreateBitmapFromData(d,DefaultRootWindow(d),bits,8,8);
  if(payload==2){
   icon=XCreatePixmap(d,DefaultRootWindow(d),13,11,DefaultDepth(d,DefaultScreen(d)));
   GC gc=XCreateGC(d,icon,0,NULL);XSetForeground(d,gc,WhitePixel(d,DefaultScreen(d)));XFillRectangle(d,icon,gc,0,0,13,11);
   XSetForeground(d,gc,BlackPixel(d,DefaultScreen(d)));XFillRectangle(d,icon,gc,3,2,7,7);XFreeGC(d,gc);
  }
  for(int i=0;i<(extended?7:3);i++){
   char text[96];snprintf(text,sizeof(text),extended?"Section %d: configuration details":"Section %d",i+1);
   XmString t=XmStringCreateLocalized(extended?text:(i==0?"Overview":i==1?"Details":"Activity"));
   XtVaCreateManagedWidget("page",xmLabelWidgetClass,stack,XmNtabLabelString,t,XmNlabelString,t,
       XmNtabLabelPixmap,icon,XmNtabPixmapPlacement,XmPIXMAP_LEFT,NULL);XmStringFree(t);
  }
  XtRealizeWidget(shell);drain(a,d);Widget canvas=XtNameToWidget(stack,"tabBox.canvas");CHECK(canvas);
  Widget box=XtParent(canvas);
  inside(box,stack);inside(XtNameToWidget(stack,"page"),stack);
  /* A synchronous full draw supplies the reference, without pending damage. */
  XtClass(canvas)->core_class.expose(canvas,NULL,NULL);XSync(d,False);
  unsigned width=XtWidth(canvas),height=XtHeight(canvas);
  if(only){Position rx,ry;XtTranslateCoords(canvas,0,0,&rx,&ry);
   fprintf(stderr,"Canvas %ux%u root %d,%d; stack %ux%u\n",width,height,rx,ry,XtWidth(stack),XtHeight(stack));}
  XImage*before=XGetImage(d,XtWindow(canvas),0,0,width,height,AllPlanes,ZPixmap);CHECK(before);
  for(unsigned damage=0;damage<2;damage++){
   XRectangle rect={0,0,(unsigned short)width,(unsigned short)height};
   if(side<2){rect.y=damage?height/2:0;rect.height=damage?3:1;}
   else {rect.x=damage?width/2:0;rect.width=damage?3:1;}
   Region region=XCreateRegion();XUnionRectWithRegion(&rect,region,region);
   XClearArea(d,XtWindow(canvas),rect.x,rect.y,rect.width,rect.height,False);
   XtClass(canvas)->core_class.expose(canvas,NULL,region);XDestroyRegion(region);XSync(d,False);
   XImage*after=XGetImage(d,XtWindow(canvas),0,0,width,height,AllPlanes,ZPixmap);CHECK(after);
   for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++)if(XGetPixel(before,x,y)!=XGetPixel(after,x,y)){
    fprintf(stderr,"FAIL tab repaint p%u scale%g side%u style%u mode%u payload%u size%u damage%u at %u,%u expected%lx actual%lx\n",p,scales[scale],side,style,mode,payload,size,damage,x,y,XGetPixel(before,x,y),XGetPixel(after,x,y));return 1;
   }
   XDestroyImage(after);cases++;
  }
  XDestroyImage(before);XtDestroyWidget(shell);drain(a,d);if(icon!=XmUNSPECIFIED_PIXMAP)XFreePixmap(d,icon);
 }
 unsigned layouts=0;
 if(extended && !getenv("XM_TAB_REPAINT_CASE")){
  const unsigned dimensions[][2]={{1,1},{32,24},{420,360}};
  for(unsigned p=0;p<4;p++)for(unsigned scale=0;scale<3;scale++)
  for(unsigned side=0;side<4;side++)for(unsigned mode=0;mode<3;mode++)for(unsigned size=0;size<3;size++){
   CHECK(XmApplyAppearanceProfile(d,profiles[p]));CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
   Widget shell=XtAppCreateShell("bounds","Bounds",applicationShellWidgetClass,d,NULL,0);
   XtVaSetValues(shell,XtNwidth,dimensions[size][0],XtNheight,dimensions[size][1],NULL);
   Widget stack=XtVaCreateManagedWidget("stack",xmTabStackWidgetClass,shell,
     XmNtabSide,sides[side],XmNtabMode,modes[mode],XmNuniformTabSize,True,
     XmNmarginWidth,23,XmNmarginHeight,7,NULL);
   XmString label=XmStringCreateLocalized("Long tab label for constrained geometry");
   Widget first=NULL;
   for(int i=0;i<7;i++){
    Widget child=XtVaCreateManagedWidget("page",xmLabelWidgetClass,stack,
      XmNtabLabelString,label,XmNlabelString,label,NULL);
    if(!first)first=child;
   }
   XmStringFree(label);XtRealizeWidget(shell);drain(a,d);
   inside(XtNameToWidget(stack,"tabBox"),stack);inside(first,stack);
   XtDestroyWidget(shell);drain(a,d);layouts++;
  }
 }
 CHECK(cases>0);XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS tab repaint: %u partial-damage cases, %u constrained layouts\n",cases,layouts);return 0;
}
