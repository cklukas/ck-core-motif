/* Revisit the real Container preview after destroying gadgets at reused addresses. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/IconGP.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern Widget gallery_create_component_example(Widget, WidgetClass, const char *, XtCallbackProc, XtPointer);
extern void gallery_refresh_icon_resources(void);
static unsigned iteration, profile, destroyed;
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL Container appearance profile=%u iteration=%u line=%d: %s\n",profile,iteration,__LINE__,#c);exit(1);} }while(0)
static void dead(Widget w,XtPointer c,XtPointer d){(void)w;(void)c;(void)d;++destroyed;}
static void pump(XtAppContext a,Display*d){for(unsigned i=0;i<4;++i){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);}}
static void row(Widget w, Widget parent, Boolean modern, Boolean pixels){
 Pixel bg,pbg;Dimension shadow;Pixmap icon;XGCValues gc;unsigned char lines;
 XtVaGetValues(parent,XmNbackground,&pbg,XmNoutlineLineStyle,&lines,NULL);
 XtVaGetValues(w,XmNbackground,&bg,XmNshadowThickness,&shadow,XmNsmallIconPixmap,&icon,NULL);
 CHECK(icon!=None&&icon!=XmUNSPECIFIED_PIXMAP);
 if(modern){CHECK(bg==pbg);CHECK(shadow==0);CHECK(lines==XmNO_LINE);}
 CHECK(XGetGCValues(XtDisplayOfObject(w),IG_BackgroundGC(w),GCForeground,&gc));CHECK(gc.foreground==bg);
 if(pixels){
  Window root;int x,y;unsigned width,height,border,depth;
  CHECK(XGetGeometry(XtDisplayOfObject(w),icon,&root,&x,&y,&width,&height,&border,&depth));
  XImage *image=XGetImage(XtDisplayOfObject(w),icon,0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
  CHECK(XGetPixel(image,0,0)==bg);
  unsigned colors=0;Pixel distinct[16]={0};
  for(unsigned yy=0;yy<height;++yy)for(unsigned xx=0;xx<width;++xx){Pixel pixel=XGetPixel(image,xx,yy);unsigned k=0;for(;k<colors;++k)if(distinct[k]==pixel)break;if(k==colors&&colors<16)distinct[colors++]=pixel;}
  CHECK(colors>2);XDestroyImage(image);
  Position rx,ry;Dimension rw,rh;XtVaGetValues(w,XmNx,&rx,XmNy,&ry,XmNwidth,&rw,XmNheight,&rh,NULL);
  XWindowAttributes attributes;CHECK(XGetWindowAttributes(XtDisplay(parent),XtWindow(parent),&attributes));
  CHECK(rx>=0&&ry>=0&&rx<attributes.width&&ry<attributes.height);
  if(rx+rw>attributes.width)rw=attributes.width-rx;
  if(ry+rh>attributes.height)rh=attributes.height-ry;
  image=XGetImage(XtDisplay(parent),XtWindow(parent),rx,ry,rw,rh,AllPlanes,ZPixmap);CHECK(image);
  unsigned background=0;for(unsigned yy=0;yy<rh;++yy)for(unsigned xx=0;xx<rw;++xx)background+=XGetPixel(image,xx,yy)==bg;
  CHECK(background>rw*rh/3);XDestroyImage(image);
 }
}
int main(int argc,char**argv){XtAppContext app;Widget shell=XtVaAppInitialize(&app,"ContainerAppearance",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);CHECK(XmSetAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
 Widget host=XtVaCreateManagedWidget("host",xmBulletinBoardWidgetClass,shell,XmNwidth,1000,XmNheight,600,NULL);XtRealizeWidget(shell);NativeWaitForViewable(host);
 uintptr_t addresses[100]={0};unsigned reused=0,cases=0;
 for(iteration=0;iteration<100;++iteration){
  Widget container=gallery_create_component_example(host,xmContainerWidgetClass,"container",NULL,NULL);Widget folder=XtNameToWidget(container,"folder");CHECK(folder);
  uintptr_t address=(uintptr_t)folder;for(unsigned i=0;i<iteration;++i)if(addresses[i]==address){++reused;break;}addresses[iteration]=address;
  XtAddCallback(folder,XmNdestroyCallback,dead,NULL);row(folder,container,True,False);++cases;
  if(iteration<4){
   const XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT};
   for(profile=0;profile<4;++profile){CHECK(XmApplyAppearanceProfile(d,profiles[profile]));CHECK(XmSetDisplayScaleOverride(d,profile==1?2:1));gallery_refresh_icon_resources();pump(app,d);
    WidgetList children;Cardinal count;XtVaGetValues(container,XmNchildren,&children,XmNnumChildren,&count,NULL);
    for(Cardinal c=0;c<count;++c)if(XmIsIconGadget(children[c])){row(children[c],container,profiles[profile]!=XmAPPEARANCE_CLASSIC,True);++cases;}
   }
  }
  XtDestroyWidget(container);pump(app,d);CHECK(destroyed==iteration+1);
 }
 CHECK(reused>0);
 /* Explicit application choices remain authoritative on newly created items. */
 Widget c=XtVaCreateManagedWidget("explicitContainer",xmContainerWidgetClass,host,XmNoutlineLineStyle,XmSINGLE,NULL);
 Widget g=XtVaCreateManagedWidget("explicitIcon",xmIconGadgetClass,c,XmNbackground,0x123456,XmNshadowThickness,3,NULL);
 CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_DARK));Pixel bg;Dimension st;XtVaGetValues(g,XmNbackground,&bg,XmNshadowThickness,&st,NULL);CHECK(bg==0x123456&&st==3);unsigned char lines;XtVaGetValues(c,XmNoutlineLineStyle,&lines,NULL);CHECK(lines==XmSINGLE);
 XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS Container appearance: %u checks, %u reused gadget addresses, 100 destroy callbacks\n",cases,reused);return 0;}
