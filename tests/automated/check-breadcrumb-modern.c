/* Rendering and actual input regression for live modern breadcrumbs. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <Xm/BreadcrumbBarP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks,changes,activations;
#define CHECK(c) do{++checks;if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void changed(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;CHECK(data!=NULL);changes++;}
static void activated(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;CHECK(data!=NULL);activations++;}
static void tick(XtPointer p,XtIntervalId *id){(void)id;*(Boolean*)p=True;}
static void settle(XtAppContext app,Display*d){Boolean b=False;XtAppAddTimeOut(app,25,tick,&b);while(!b)XtAppProcessEvent(app,XtIMAll);XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);}
static void key(Widget w,KeySym sym){XEvent e={0};e.xkey.type=KeyPress;e.xkey.display=XtDisplay(w);e.xkey.window=XtWindow(w);e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),sym);XtDispatchEvent(&e);}
static void click(Widget w,int x){XEvent e={0};e.xbutton.type=ButtonPress;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);e.xbutton.button=Button1;e.xbutton.x=x;e.xbutton.y=15;XtDispatchEvent(&e);e.xbutton.type=ButtonRelease;XtDispatchEvent(&e);}
static XImage *snapshot(Widget w){return XGetImage(XtDisplay(w),XtWindow(w),0,0,w->core.width,w->core.height,AllPlanes,ZPixmap);}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"BreadcrumbModern",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);
 Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,1300,XmNheight,500,NULL);
 Widget widget=XtVaCreateManagedWidget("bar",xmBreadcrumbBarWidgetClass,board,XmNwidth,1200,XmNheight,50,NULL);XmBreadcrumbBarWidget bar=(XmBreadcrumbBarWidget)widget;
 XmBreadcrumbSegment segments[]={{"Home",(XtPointer)"home"},{"Components",(XtPointer)"components"},{"Current",(XtPointer)"current"}};
 XmBreadcrumbBarSetSegments(widget,segments,3);XtAddCallback(widget,XmNbreadcrumbCurrentChangedCallback,changed,NULL);XtAddCallback(widget,XmNbreadcrumbActivateCallback,activated,NULL);XtRealizeWidget(shell);
 const double scales[]={1,1.5,2};const XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT};
 for(unsigned si=0;si<3;si++)for(unsigned rtl=0;rtl<2;rtl++)for(unsigned pi=0;pi<4;pi++){
  CHECK(XmSetDisplayScaleOverride(d,scales[si]));XtVaSetValues(widget,XmNheight,(int)(50*scales[si]),XmNbreadcrumbRtl,rtl,XmNbreadcrumbSeparator,NULL,NULL);
  unsigned oldchanges=changes;CHECK(XmApplyAppearanceProfile(d,profiles[pi]));settle(app,d);CHECK(changes==oldchanges);CHECK(bar->breadcrumb.separator==NULL);
  XmBreadcrumbBarSetCurrentIndex(widget,2,False);settle(app,d);
  if(profiles[pi]!=XmAPPEARANCE_CLASSIC){
   XmBreadcrumbSegment long_segments[3];memcpy(long_segments,segments,sizeof(segments));
   long_segments[2].label="WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW";
   XmBreadcrumbBarSetSegments(widget,long_segments,3);
   XtWidgetGeometry preferred={0};XtQueryGeometry(widget,NULL,&preferred);CHECK(preferred.width>240);
   XmBreadcrumbBarSetSegments(widget,segments,3);settle(app,d);CHECK(preferred.height>=(Dimension)(36*scales[si]));
   XImage *image=snapshot(widget);CHECK(image!=NULL);unsigned long bg=bar->core.background_pixel,fg=bar->primitive.foreground,accent=bar->primitive.highlight_color;unsigned accent_pixels=0;
   /* No inverted rectangle anywhere along the quiet top strip. */
   for(unsigned x=6;x<600;x++)CHECK(XGetPixel(image,x,5)==bg);
   for(unsigned y=8;y<widget->core.height-5;y++)for(unsigned x=4;x<600;x++)if(XGetPixel(image,x,y)==accent)accent_pixels++;
   CHECK(fg!=bg);CHECK(accent_pixels>10);XDestroyImage(image);
   /* Explicit strings, including slash, override automatic chevrons. */
   XImage *before=snapshot(widget);XtVaSetValues(widget,XmNbreadcrumbSeparator,"/",NULL);settle(app,d);CHECK(!strcmp(bar->breadcrumb.separator,"/"));XImage *after=snapshot(widget);unsigned different=0;
   for(unsigned y=8;y<widget->core.height-5;y++)for(unsigned x=4;x<600;x++)different+=XGetPixel(before,x,y)!=XGetPixel(after,x,y);
   CHECK(different>10);XDestroyImage(before);XDestroyImage(after);XtVaSetValues(widget,XmNbreadcrumbSeparator,NULL,NULL);settle(app,d);
  }
  key(widget,rtl?XK_Right:XK_Left);CHECK(XmBreadcrumbBarGetCurrentIndex(widget)==1);key(widget,XK_Return);CHECK(activations>0);
  /* Clicking the first visual label selects its logical ID in either direction. */
  XmString first=XmStringCreateLocalized((char*)segments[rtl?2:0].label);Dimension first_width,first_height;
  XmStringExtent(bar->breadcrumb.font,first,&first_width,&first_height);XmStringFree(first);
  click(widget,4+first_width/2);CHECK(XmBreadcrumbBarGetCurrentIndex(widget)==(rtl?2:0));settle(app,d);
 }
 XtDestroyWidget(shell);printf("PASS modern BreadcrumbBar: %u checks, palette/scale/RTL transitions, plain current surface, accent indicator, custom separators, keyboard/pointer callbacks\n",checks);return 0;
}
